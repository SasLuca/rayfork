#include "rayfork-audio.h"

/*
 * We must include stb_vorbis without defining the header before we include miniaudio.
 * https://github.com/mackron/miniaudio#vorbis-decoding
 */
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"
#undef  STB_VORBIS_HEADER_ONLY

#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_WIN32_IO
#define JAR_XM_IMPLEMENTATION
#define JAR_MOD_IMPLEMENTATION
#define STB_VORBIS_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "stb_vorbis.h"
#include "jar_mod.h"
#include "jar_xm.h"

#define RF_SUPPORT_FILEFORMAT_WAV
#define RF_SUPPORT_FILEFORMAT_OGG
#define RF_SUPPORT_FILEFORMAT_FLAC
#define RF_SUPPORT_FILEFORMAT_MP3

/*rf_public rf_bool rf_audio_init()
{
    rf_valid        valid          = {0};
    ma_context      context        = {0};
    ma_device_info* playback_infos = {0};
    ma_uint32       playback_count = {0};
    ma_device_info* capture_infos  = {0};
    ma_uint32       capture_count  = {0};

    if (ma_context_init(NULL, 0, NULL, &context) == MA_SUCCESS)
    {
        if (ma_context_get_devices(&context, &playback_infos, &playback_count, &capture_infos, &capture_count) == MA_SUCCESS)
        {
            //Loop over each device info and do something with it. Here we just print the name with their index. You may want
            //to give the user the opportunity to choose which device they'd prefer.
            for (rf_int device_iter = 0; device_iter < playback_count; device_iter++)
            {
                printf("%d - %s\n", device_iter, playback_infos[device_iter].name);
            }

            ma_device_config config = ma_device_config_init(ma_device_type_playback);
            config.playback.pDeviceID = &playback_infos[chosenPlaybackDeviceIndex].id;
            config.playback.format    = FORMAT;
            config.playback.channels  = MY_CHANNEL_COUNT;
            config.sampleRate         = MY_SAMPLE_RATE;
            config.dataCallback       = data_callback;
            config.pUserData          = pMyCustomData;

            ma_device device = {0};
            if (ma_device_init(&context, &config, &device) != MA_SUCCESS)
            {
                // Error
            }
        }
    }
}*/

#define rf_audio_buffers rf_global_audio_ctx.buffers

rf_audio_context rf_global_audio_ctx = {
    /*
      NOTE: Music buffer size is defined by number of samples, independent of sample size and channels number
      After some math, considering a sampleRate of 48000, a buffer refill rate of 1/60 seconds and a
      standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
      In case of music-stalls, just increase this number
    */
    .default_buffer_size = rf_default_audio_buffer_size
};

#pragma region private api
#pragma region drlibs allocator wrappers
rf_internal void* rf_drlib_malloc_wrapper(size_t size, void* user_data)
{
    void* result = 0;
    rf_allocator* allocator = user_data;
    result = rf_alloc(*allocator, size);
    return result;
}

rf_internal void* rf_drlib_realloc_wrapper(void* p, size_t new_size, void* user_data)
{
    void* result = 0;
    rf_allocator* allocator = user_data;
    result = rf_realloc(*allocator, p, new_size, rf_old_allocation_size_unknown);
    return result;
}

rf_internal void rf_drlib_free_wrapper(void* p, void* user_data)
{
    rf_allocator* allocator = user_data;
    rf_free(*allocator, p);
}
#pragma endregion

rf_internal void rf_init_audio_buffer_pool(void);  // Initialise the multichannel buffer pool
rf_internal void rf_close_audio_buffer_pool(void); // Close the audio buffers pool

#pragma region audio mixing and io
rf_internal rf_int rf_read_audio_buffer_frames_in_internal_format(rf_audio_buffer* audio_buffer, char* frames_dst, rf_int frames_count);
rf_internal rf_int rf_read_audio_buffer_frames_in_mixing_format(rf_audio_buffer* audio_buffer, float* frames_dst, rf_int frames_count);
rf_internal void rf_mix_audio_frames(float* dst, const float* src, ma_uint32 frame_count, float local_volume);
rf_internal void rf_on_send_audio_data_to_device(ma_device* device, void* dst, const void* src, ma_uint32 frame_count);
#pragma endregion

#pragma region audio buffer api
rf_internal rf_audio_buffer_handle rf_load_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 size_in_frames, rf_audio_buffer_type type);
rf_internal void rf_unload_audio_buffer(rf_audio_buffer* buffer);

rf_internal rf_bool rf_audio_buffer_is_playing(rf_audio_buffer* buffer);
rf_internal void rf_audio_buffer_play(rf_audio_buffer* buffer);
rf_internal void rf_audio_buffer_stop(rf_audio_buffer* buffer);
rf_internal void rf_audio_buffer_pause(rf_audio_buffer* buffer);
rf_internal void rf_audio_buffer_resume(rf_audio_buffer* buffer);
rf_internal void rf_audio_buffer_set_volume(rf_audio_buffer* buffer, float volume);
rf_internal void rf_audio_buffer_set_pitch(rf_audio_buffer* buffer, float pitch);
rf_internal void rf_audio_buffer_untrack(rf_audio_buffer* buffer);
#pragma endregion
#pragma endregion

#pragma region audio mixing and io
rf_internal rf_int rf_read_audio_buffer_frames_in_internal_format(rf_audio_buffer* audio_buffer, char* frames_dst, rf_int frames_count)
{
    rf_int sub_buffer_size_in_frames = (audio_buffer->size_in_frames > 1) ? audio_buffer->size_in_frames / 2 : audio_buffer->size_in_frames;
    rf_int current_sub_buffer_index  = audio_buffer->frame_cursor_pos / sub_buffer_size_in_frames;

    if (current_sub_buffer_index > 1) {
        return 0;
    }

    // Another thread can update the processed state of buffers so
    // we just take a copy here to try and avoid potential synchronization problems
    bool is_sub_buffer_processed[2] = {0};
    is_sub_buffer_processed[0] = audio_buffer->is_sub_buffer_processed[0];
    is_sub_buffer_processed[1] = audio_buffer->is_sub_buffer_processed[1];

    rf_int frame_size_in_bytes = ma_get_bytes_per_frame(audio_buffer->converter.config.formatIn, audio_buffer->converter.config.channelsIn);

    // Fill out every frame until we find a buffer that's marked as processed. Then fill the remainder with 0
    rf_int frames_read = 0;
    while (1)
    {
        // We break from this loop differently depending on the buffer's usage
        //  - For static buffers, we simply fill as much data as we can
        //  - For streaming buffers we only fill the halves of the buffer that are processed
        //    Unprocessed halves must keep their audio data in-tact
        if ((audio_buffer->type == rf_audio_buffer_type_static     && frames_read >= frames_count) ||
            (audio_buffer->type == rf_audio_buffer_type_streamed && is_sub_buffer_processed[current_sub_buffer_index]))
        {
            break;
        }

        rf_int totals_frames_remaining = (frames_count - frames_read);
        if (totals_frames_remaining == 0) {
            break;
        }

        rf_int frames_remaining_in_output_buffer = 0;
        if (audio_buffer->type == rf_audio_buffer_type_static)
        {
            frames_remaining_in_output_buffer = audio_buffer->size_in_frames - audio_buffer->frame_cursor_pos;
        }
        else
        {
            rf_int first_frame_index_of_this_sub_buffer = sub_buffer_size_in_frames * current_sub_buffer_index;
            frames_remaining_in_output_buffer = sub_buffer_size_in_frames - (audio_buffer->frame_cursor_pos - first_frame_index_of_this_sub_buffer);
        }

        rf_int frames_to_read = totals_frames_remaining;
        if (frames_to_read > frames_remaining_in_output_buffer) {
            frames_to_read = frames_remaining_in_output_buffer;
        }

        char* dst = frames_dst + (frames_read * frame_size_in_bytes);
        char* src = audio_buffer->data + (audio_buffer->frame_cursor_pos * frame_size_in_bytes); 
        memcpy(dst, src, frames_to_read * frame_size_in_bytes);

        audio_buffer->frame_cursor_pos = (audio_buffer->frame_cursor_pos + frames_to_read) % audio_buffer->size_in_frames;
        frames_read += frames_to_read;

        // If we've read to the end of the buffer, mark it as processed
        if (frames_to_read == frames_remaining_in_output_buffer)
        {
            audio_buffer->is_sub_buffer_processed[current_sub_buffer_index] = true;
            is_sub_buffer_processed[current_sub_buffer_index] = true;

            current_sub_buffer_index = (current_sub_buffer_index + 1) % 2;

            // We need to break from this loop if we're not looping
            if (!audio_buffer->looping)
            {
                rf_audio_buffer_stop(audio_buffer);
                break;
            }
        }
    }

    // Zero-fill excess
    rf_int total_frames_remaining = (frames_count - frames_read);
    if (total_frames_remaining > 0)
    {
        memset(frames_dst + (frames_read * frame_size_in_bytes), 0, total_frames_remaining * frame_size_in_bytes);

        // For static buffers we can fill the remaining frames with silence for safety, but we don't want
        // to report those frames as "read". The reason for this is that the caller uses the return value
        // to know whether or not a non-looping sound has finished playback.
        if (audio_buffer->type != rf_audio_buffer_type_static) {
            frames_read += total_frames_remaining;
        }
    }

    return frames_read;
}

rf_internal rf_int rf_read_audio_buffer_frames_in_mixing_format(rf_audio_buffer* audio_buffer, float* frames_dst, rf_int frames_count)
{
    // What's going on here is that we're continuously converting data from the rf_audio_buffer's internal format to the mixing format, which
    // should be defined by the output format of the data converter. We do this until frames_count frames have been output. The important
    // detail to remember here is that we never, ever attempt to read more input data than is required for the specified number of output
    // frames. This can be achieved with ma_data_converter_get_required_input_frame_count().
    char input_buffer[4096] = {0};
    rf_int input_buffer_frame_cap = sizeof(input_buffer) / ma_get_bytes_per_frame(audio_buffer->converter.config.formatIn, audio_buffer->converter.config.channelsIn);

    rf_int total_output_frames_processed = 0;
    while (total_output_frames_processed < frames_count)
    {
        rf_int output_frames_to_process_this_iteration = frames_count - total_output_frames_processed;

        rf_int input_frames_to_process_this_iteration = ma_data_converter_get_required_input_frame_count(&audio_buffer->converter, output_frames_to_process_this_iteration);
        if (input_frames_to_process_this_iteration > input_buffer_frame_cap) 
        {
            input_frames_to_process_this_iteration = input_buffer_frame_cap;
        }

        float* running_frames_dst = frames_dst + (total_output_frames_processed * audio_buffer->converter.config.channelsOut);

        // At this point we can convert the data to our mixing format.
        ma_uint64 input_frames_processed  = rf_read_audio_buffer_frames_in_internal_format(audio_buffer, input_buffer, (rf_int) input_frames_to_process_this_iteration);
        ma_uint64 output_frames_processed = output_frames_to_process_this_iteration;
        ma_data_converter_process_pcm_frames(&audio_buffer->converter, input_buffer, &input_frames_processed, running_frames_dst, &output_frames_processed);

        total_output_frames_processed += output_frames_processed;

        // Ran out of input data.
        if (input_frames_processed < input_frames_to_process_this_iteration) {
            break;
        }

        // This should never be hit, but will add it here for safety. Ensures we get out of the loop when no input nor output frames are processed.
        if (input_frames_processed == 0 && output_frames_processed == 0) {
            break;
        }
    }

    return total_output_frames_processed;
}

rf_internal void rf_on_send_audio_data_to_device(ma_device* device, void* dst, const void* src, ma_uint32 frame_count)
{
    (void)device;

    // Mixing is basically just an accumulation, we need to initialize the output buffer to 0
    memset(dst, 0, frame_count * device->playback.channels * ma_get_bytes_per_sample(device->playback.format));

    // Using a mutex here for thread-safety which makes things not real-time
    // This is unlikely to be necessary for this project, but may want to consider how you might want to avoid this
    ma_mutex_lock(&rf_global_audio_ctx.lock);
    {
        for (rf_int i = 0; i < rf_arr_size(rf_global_audio_ctx.buffers); i++)
        {
            rf_audio_buffer* audio_buffer = &rf_global_audio_ctx.buffers[i];
            
            // Ignore invalid or stopped or paused audio buffers
            if (!audio_buffer->valid || !audio_buffer->playing || audio_buffer->paused) {
                continue;
            }
            
            ma_uint32 frames_read = 0;

            while (frames_read < frame_count)
            {
                // Just read as much data as we can from the stream
                ma_uint32 frames_to_read = (frame_count - frames_read);

                while (frames_to_read > 0)
                {
                    float temp_buf[1024] = {0}; // 512 frames for stereo

                    ma_uint32 frames_to_read_right_now = frames_to_read;
                    if (frames_to_read_right_now > sizeof(temp_buf) / sizeof(temp_buf[0]) / device->playback.channels)
                    {
                        frames_to_read_right_now = sizeof(temp_buf) / sizeof(temp_buf[0]) / device->playback.channels;
                    }

                    ma_uint32 frames_just_read = rf_read_audio_buffer_frames_in_mixing_format(audio_buffer, temp_buf, frames_to_read_right_now);
                    if (frames_just_read > 0)
                    {
                        float* frames_dst = ((float*)dst) + (frames_read * rf_global_audio_ctx.device.playback.channels);
                        float* framesIn   = temp_buf;

                        rf_mix_audio_frames(frames_dst, framesIn, frames_just_read, audio_buffer->volume);

                        frames_to_read -= frames_just_read;
                        frames_read    += frames_just_read;
                    }

                    if (!audio_buffer->playing)
                    {
                        frames_read = frame_count;
                        break;
                    }

                    // If we weren't able to read all the frames we requested, break
                    if (frames_just_read < frames_to_read_right_now)
                    {
                        if (!audio_buffer->looping)
                        {
                            rf_audio_buffer_stop(audio_buffer);
                            break;
                        }
                        else
                        {
                            // Should never get here, but just for safety,
                            // move the cursor position back to the start and continue the loop
                            audio_buffer->frame_cursor_pos = 0;
                            continue;
                        }
                    }
                }

                // If for some reason we weren't able to read every frame we'll need to break from the loop
                // Not doing this could theoretically put us into an infinite loop
                if (frames_to_read > 0) {
                    break;
                }
            }
        }
    }

    ma_mutex_unlock(&rf_global_audio_ctx.lock);
}

rf_internal void rf_mix_audio_frames(float* dst, const float* src, ma_uint32 frame_count, float local_volume)
{
    for (rf_int frame_index = 0; frame_index < frame_count; frame_index++)
    {
        for (rf_int channel_index = 0; channel_index < rf_global_audio_ctx.device.playback.channels; channel_index++)
        {
                  float* frame_to_write = dst + (frame_index * rf_global_audio_ctx.device.playback.channels);
            const float* fream_to_read  = src + (frame_index * rf_global_audio_ctx.device.playback.channels);

            frame_to_write[channel_index] += fream_to_read[channel_index] * local_volume;
        }
    }
}
#pragma endregion

#pragma region audio buffer
rf_internal rf_audio_buffer_handle rf_load_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 size_in_frames, rf_audio_buffer_type type)
{
    rf_audio_buffer_handle result = -1;

    // Audio data runs through a format converter
    rf_audio_buffer buffer = {0};
    ma_data_converter_config converter_config = ma_data_converter_config_init(format, rf_default_audio_device_format, channels, rf_default_audio_device_channels, sample_rate, rf_default_audio_device_sample_rate);
    converter_config.resampling.allowDynamicSampleRate = true; // Required for pitch shifting

    if (ma_data_converter_init(&converter_config, &buffer.converter) == MA_SUCCESS && size_in_frames > 0)
    {
        buffer.data = rf_calloc(rf_default_allocator, size_in_frames * channels * ma_get_bytes_per_sample(format));

        if (buffer.data)
        {
            // Init audio buffer values
            buffer = (rf_audio_buffer) {
                .type             = type,
                .volume           = 1.0f,
                .pitch            = 1.0f,
                .playing          = false,
                .paused           = false,
                .looping          = false,
                .frame_cursor_pos = 0,
                .size_in_frames   = size_in_frames,

                .data             = buffer.data,
                .converter        = buffer.converter,

                /* @Note(raylib): Buffers should be marked as processed by default so that a call to
                   rf_update_audio_stream() immediately after initialization works correctly */
                .is_sub_buffer_processed[0] = true,
                .is_sub_buffer_processed[1] = true,
            };

            // Track audio buffer
            ma_mutex_lock(&rf_global_audio_ctx.lock);
            {
                rf_arr_add(&rf_global_audio_ctx.buffers, buffer);
                result = rf_arr_size(rf_global_audio_ctx.buffers);
            }
            ma_mutex_unlock(&rf_global_audio_ctx.lock);
        }
    }

    return result;
}

rf_internal void rf_unload_audio_buffer(rf_audio_buffer* buffer)
{
    if (buffer)
    {
        ma_data_converter_uninit(buffer->converter);

        rf_free(rf_default_allocator, buffer->data);

        rf_arr_remove_ordered(rf_global_audio_ctx.buffers, rf_arr_index_of(buffer));
    }
}

rf_internal rf_bool rf_audio_buffer_is_playing(rf_audio_buffer* buffer)
{
    rf_bool result = buffer && buffer->playing && !buffer->paused;
    return result;
}

rf_internal void rf_audio_buffer_play(rf_audio_buffer* buffer)
{
    if (buffer)
    {
        buffer->playing = true;
        buffer->paused = false;
        buffer->frame_cursor_pos = 0;
    }
}

rf_internal void rf_audio_buffer_stop(rf_audio_buffer* buffer)
{
    if (rf_audio_buffer_is_playing(buffer))
    {
        buffer->playing = false;
        buffer->paused = false;
        buffer->frame_cursor_pos = 0;
        buffer->total_frames_processed = 0;
        buffer->is_sub_buffer_processed[0] = true;
        buffer->is_sub_buffer_processed[1] = true;
    }
}

rf_internal void rf_audio_buffer_pause(rf_audio_buffer* buffer)
{
    if (buffer) {
        buffer->paused = true;
    }
}

rf_internal void rf_audio_buffer_resume(rf_audio_buffer* buffer)
{
    if (buffer) {
        buffer->paused = false;
    }
}

rf_internal void rf_audio_buffer_set_volume(rf_audio_buffer* buffer, float volume)
{
    if (buffer) {
        buffer->volume = volume;
    }
}

rf_internal void rf_audio_buffer_set_pitch(rf_audio_buffer* buffer, float pitch)
{
    if (buffer)
    {
        float pitch_mul = pitch / buffer->pitch;

        /*
         Pitching is just an adjustment of the sample rate.
         Note that this changes the duration of the sound:
            - higher pitch = faster
            - lower pitch  = slower
        */

        ma_uint32 new_output_sample_rate = (ma_uint32)((float)buffer->converter.config.sampleRateOut / pitchMul);

        buffer->pitch *= (float)buffer->converter.config.sampleRateOut / new_output_sample_rate;

        ma_data_converter_set_rate(&buffer->converter, buffer->converter.config.sampleRateIn, new_output_sample_rate);
    }
}

rf_internal void rf_audio_buffer_untrack(rf_audio_buffer* buffer)
{
    ma_mutex_lock(&rf_global_audio_ctx.lock);
    {
        rf_arr_remove_ptr_ordered(&rf_global_audio_ctx.buffers, buffer);
    }
    ma_mutex_unlock(&rf_global_audio_ctx.lock);
}
#pragma endregion

#pragma region context
rf_public void rf_init_audio_device(void)
{
    ma_context_config ctx_config = {0};
    ma_device_config  config     = {0};
    ma_result         result     = {0};

    // Init audio context
    ctx_config = ma_context_config_init();
    ctx_config.logCallback = OnLog;

    if (ma_context_init(NULL, 0, &ctx_config, &rf_global_audio_ctx.ma_ctx) != MA_SUCCESS)
    {
        //TRACELOG(LOG_ERROR, "AUDIO: Failed to initialize context");
    }

    // Init audio device
    // NOTE(raylib): Using the default device. Format is floating point because it simplifies mixing.
    config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = rf_default_audio_device_id;
    config.playback.format    = rf_default_audio_device_format;
    config.playback.channels  = rf_default_audio_device_channels;
    config.capture.pDeviceID  = rf_default_audio_device_id;
    config.capture.format     = ma_format_s16;
    config.capture.channels   = 1;
    config.sampleRate         = AUDIO_DEVICE_SAMPLE_RATE;
    config.dataCallback       = OnSendAudioDataToDevice;
    config.pUserData          = 0;

    if (ma_device_init(&rf_global_audio_ctx.ma_ctx, &config, &rf_global_audio_ctx.device) != MA_SUCCESS)
    {
        TRACELOG(LOG_ERROR, "AUDIO: Failed to initialize playback device");
        ma_context_uninit(&rf_global_audio_ctx.ma_ctx);
        return;
    }

    // Keep the device running the whole time. May want to consider doing something a bit smarter and only have the device running
    // while there's at least one sound being played.
    if (ma_device_start(&rf_global_audio_ctx.device) != MA_SUCCESS)
    {
        TRACELOG(LOG_ERROR, "AUDIO: Failed to start playback device");
        ma_device_uninit(&rf_global_audio_ctx.device);
        ma_context_uninit(&rf_global_audio_ctx.ma_ctx);
        return;
    }

    // Mixing happens on a seperate thread which means we need to synchronize. I'm using a mutex here to make things simple, but may
    // want to look at something a bit smarter later on to keep everything real-time, if that's necessary.
    if (ma_mutex_init(&rf_global_audio_ctx.lock) != MA_SUCCESS)
    {
        TRACELOG(LOG_ERROR, "AUDIO: Failed to create mutex for mixing");
        ma_device_uninit(&rf_global_audio_ctx.device);
        ma_context_uninit(&rf_global_audio_ctx.ma_ctx);
        return;
    }

    TRACELOG(LOG_INFO, "AUDIO: Device initialized successfully");
    TRACELOG(LOG_INFO, "    > Backend:       miniaudio / %s", ma_get_backend_name(rf_global_audio_ctx.context.backend));
    TRACELOG(LOG_INFO, "    > Format:        %s -> %s", ma_get_format_name(rf_global_audio_ctx.device.playback.format), ma_get_format_name(rf_global_audio_ctx.device.playback.internalFormat));
    TRACELOG(LOG_INFO, "    > Channels:      %d -> %d", rf_global_audio_ctx.device.playback.channels, rf_global_audio_ctx.device.playback.internalChannels);
    TRACELOG(LOG_INFO, "    > Sample rate:   %d -> %d", rf_global_audio_ctx.device.sampleRate, rf_global_audio_ctx.device.playback.internalSampleRate);
    TRACELOG(LOG_INFO, "    > Periods size:  %d", rf_global_audio_ctx.device.playback.internalPeriodSizeInFrames*rf_global_audio_ctx.device.playback.internalPeriods);

    // Init the multichannel buffer pool
    for (rf_int i = 0; i < rf_max_audio_buffer_pool_channels; i++)
    {
        rf_global_audio_ctx.multichannel_pool[i] = rf_load_audio_buffer(rf_default_audio_device_format,
                                                                        rf_default_audio_device_channels,
                                                                        rf_default_audio_device_sample_rate,
                                                                        /*size in frames*/ 0,
                                                                        rf_audio_buffer_type_static);
    }

    rf_global_audio_ctx.valid = true;
}

rf_public void rf_close_audio_device(void)
{
    if (rf_global_audio_ctx.valid)
    {
        ma_mutex_uninit(&rf_global_audio_ctx.lock);
        ma_device_uninit(&rf_global_audio_ctx.device);
        ma_context_uninit(&rf_global_audio_ctx.ma_ctx);

        rf_close_audio_buffer_pool();

        TRACELOG(LOG_INFO, "AUDIO: Device closed successfully");
    }
    else {
        TRACELOG(LOG_WARNING, "AUDIO: Device could not be closed, not currently initialized");
    }
}

rf_public bool rf_is_audio_device_ready(void)
{
    return rf_global_audio_ctx.valid;
}

rf_public void rf_set_master_volume(float volume)
{
    ma_device_set_master_volume(&rf_global_audio_ctx.device, volume);
}
#pragma endregion

#pragma region wave
rf_wave rf_load_wave_from_data(const char* src, rf_int src_size, rf_audio_format format)
{
     rf_wave wave = { 0 };

     if (format == rf_audio_format_mp3)
     {
        drmp3_config mp3_decoder = {0};
        drmp3_allocation_callbacks mp3_decoder_allocator = {
            .pUserData = &rf_default_allocator,
            .onMalloc  = rf_drlib_malloc_wrapper,
            .onFree    = rf_drlib_free_wrapper,
            .onRealloc = rf_drlib_realloc_wrapper,
        };

        uint64_t frame_count = 0;
        void* data = drmp3_open_memory_and_read_pcm_frames_f32(src, src_size, &mp3_decoder, &frame_count, &mp3_decoder_allocator);

        if (data)
        {
            int sample_count = frame_count * mp3_decoder.channels;

            result = (rf_audio_player) {
                .source = data,
                .source_size = sample_count * sizeof(uint16_t),
                .channels = mp3_decoder.channels,
                .sample_rate = mp3_decoder.sampleRate,
                .sample_count = sample_count,
                .sample_size = 16,
                .valid = true,
            };
        }
     }

     return wave;
}

rf_wave rf_load_wave_from_file(const char* file_name);

void rf_unload_wave(Wave wave);
void rf_wave_format(rf_wave* wave, int sample_rate, int sample_size, int channels);
void rf_wave_crop(rf_wave* wave, int init_sample, int final_sample);
rf_wave rf_wave_copy(rf_wave wave);
float* rf_get_wave_data(rf_wave wave);

#pragma endregion

#pragma region sound
rf_sound rf_load_sound_from_data(const char* src, rf_int src, rf_audio_format format);
rf_sound rf_load_sound_from_file(const char* filename);
rf_sound rf_load_sound_from_wave(rf_wave wave);

void rf_sound_unload(rf_sound);
void rf_sound_update(rf_sound);
void rf_sound_play(rf_sound souund);
void rf_sound_stop(rf_sound sound);
void rf_sound_pause(rf_sound sound);
void rf_sound_resume(rf_sound sound);
void rf_sound_play_multi(rf_sound sound);
void rf_sound_stop_multi(void);
int rf_get_sounds_playing(void);
bool rf_sound_is_playing(rf_sound souund);
void rf_set_sound_volume(rf_sound sound, float volume);
void rf_set_sound_pitch(rf_sound sound, float pitch);
#pragma endregion

#pragma region music
rf_music rf_load_music_stream_from_data(const char* src, rf_int src_size, rf_audio_format format);
rf_music rf_load_music_stream_from_file(const char *fileName);

void rf_music_unload(rf_music music);
void rf_music_play(rf_music music);
void rf_music_update(rf_music music);
void rf_music_stop(rf_music music);
void rf_music_pause(rf_music music);
void rf_music_resume(rf_music music);
bool rf_music_is_playing(rf_music music);
void rf_music_set_volume(rf_music music, float volume);
void rf_music_set_pitch(rf_music music, float pitch);
float rf_music_get_time_len(rf_music music);
float rf_music_get_time_played(rf_music music);
#pragma endregion

#pragma region audio stream
void rf_set_audio_stream_buffer_size_default(int size);

rf_audio_stream rf_audio_stream_init(int sampleRate, int sampleSize, int channels);      // Init audio stream (to stream raw audio pcm data)
void rf_audio_stream_update(rf_audio_stream stream, const void *data, int samplesCount); // Update audio stream buffers with data
bool rf_audio_stream_is_processed(rf_audio_stream stream);                               // Check if any audio stream buffers requires refill
void rf_audio_stream_close(rf_audio_stream stream);
void rf_audio_stream_play(rf_audio_stream stream);
void rf_audio_stream_pause(rf_audio_stream stream);
void rf_audio_stream_resume(rf_audio_stream stream);
bool rf_audio_stream_is_playing(rf_audio_stream stream);
void rf_audio_stream_stop(rf_audio_stream stream);
void rf_audio_stream_set_volume(rf_audio_stream stream, float volume);
void rf_audio_stream_set_pitch(rf_audio_stream stream, float pitch);
#pragma endregion