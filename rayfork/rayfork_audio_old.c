#include "rayfork_audio_old.h"

#pragma region includes

#include <assert.h>
#include <stdbool.h>

#define MA_NO_JACK
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h" // miniaudio library

#if !defined(RF_NO_OGG_LOADING)
#define STB_VORBIS_IMPLEMENTATION
#include "libs/stb_vorbis.h"
#endif

#if !defined(RF_NO_XM_LOADING)
#define JAR_XM_IMPLEMENTATION
#include "jar/jar_xm.h"
#endif

#if !defined(RF_NO_MOD_LOADING)
#define JAR_MOD_IMPLEMENTATION
#include "jar/jar_mod.h"
#endif

#if !defined(RF_NO_FLAC_LOADING)
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_WIN32_IO
#include "dr_libs/dr_flac.h"
#endif

#if !defined(RF_NO_MP3_LOADING)
#define DR_MP3_IMPLEMENTATION
#include "dr_libs/dr_mp3.h"
#endif

#pragma endregion

#pragma region structs and enums

typedef enum rf_audio_buffer_type
{
    RF_STATIC_AUDIO_BUFFER = 0,
    RF_STREAMED_AUDIO_BUFFER
} rf_audio_buffer_type;

// Audio buffer structure
// NOTE: Slightly different logic is used when feeding data to the
// playback device depending on whether or not data is streamed
struct rf_audio_buffer
{
    ma_pcm_converter dsp;   // PCM data converter

    float volume;           // Audio buffer volume
    float pitch;            // Audio buffer pitch

    bool playing;           // Audio buffer state: AUDIO_PLAYING
    bool paused;            // Audio buffer state: AUDIO_PAUSED
    bool looping;           // Audio buffer looping, always true for AudioStreams
    rf_audio_buffer_type usage; // Audio buffer usage mode: STATIC or STREAM

    bool is_sub_buffer_processed[2];     // SubBuffer processed (virtual double buffer)
    unsigned int frame_cursor_pos;       // Frame cursor position
    unsigned int frames_count;           // Total buffer size in frames
    unsigned int total_frames_processed; // Total frames processed in this buffer (required for play timming)

    unsigned char* buffer; // Data buffer, on music stream keeps filling

    rf_audio_buffer* next; // Next audio buffer on the list
    rf_audio_buffer* prev; // Previous audio buffer on the list
};

struct rf_audio_context
{
    // Audio buffers are tracked in a linked list
    rf_audio_buffer* first_audio_buffer;
    rf_audio_buffer* last_audio_buffer;

    // miniaudio global variables
    ma_context context;
    ma_device device;
    ma_mutex audio_lock;
    bool is_audio_initialised;
    float master_volume;

    // Multi channel playback global variables
    rf_audio_buffer* audio_buffer_pool[RF_MAX_AUDIO_BUFFER_POOL_CHANNELS];
    unsigned int audio_buffer_pool_counter;
    unsigned int audio_buffer_pool_channels[RF_MAX_AUDIO_BUFFER_POOL_CHANNELS];
};

#pragma endregion

#pragma region internals

#pragma region declarations

RF_INTERNAL rf_audio_context* rf__global_audio_context;

RF_INTERNAL void rf__ma_callback__on_log(ma_context* pContext, ma_device* pDevice, ma_uint32 logLevel, const char* message);
RF_INTERNAL void rf__ma_callback__on_send_audio_data_to_device(ma_device* pDevice, void* pFramesOut, const void* pFramesInput, ma_uint32 frameCount);
RF_INTERNAL ma_uint32 rf__ma_callback__on_audio_buffer_dsp_read(ma_pcm_converter* pDSP, void* pFramesOut, ma_uint32 frameCount, void* pUserData);
RF_INTERNAL void rf__mix_audio_frames(float* framesOut, const float* framesIn, ma_uint32 frameCount, float localVolume);

RF_INTERNAL rf_audio_buffer* rf__init_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 buffer_size_in_frames, int usage);
RF_INTERNAL void rf__close_audio_buffer(rf_audio_buffer* buffer);
RF_INTERNAL bool rf__is_audio_buffer_playing(rf_audio_buffer* buffer);
RF_INTERNAL void rf__play_audio_buffer(rf_audio_buffer* buffer);
RF_INTERNAL void rf__stop_audio_buffer(rf_audio_buffer* buffer);
RF_INTERNAL void rf__pause_audio_buffer(rf_audio_buffer* buffer);
RF_INTERNAL void rf__resume_audio_buffer(rf_audio_buffer* buffer);
RF_INTERNAL void rf__set_audio_buffer_volume(rf_audio_buffer* buffer, float volume);
RF_INTERNAL void rf__set_audio_buffer_pitch(rf_audio_buffer* buffer, float pitch);
RF_INTERNAL void rf__track_audio_buffer(rf_audio_buffer* buffer);
RF_INTERNAL void rf__untrack_audio_buffer(rf_audio_buffer* buffer);

#pragma endregion

#pragma region miniaudio callbacks

// Log callback function
RF_INTERNAL void rf__ma_callback__on_log(ma_context* pContext, ma_device*  pDevice, ma_uint32 logLevel, const char* message)
{
    (void)pContext;
    (void)pDevice;

    RF_LOG(RF_LOG_ERROR, message);   // All log messages from miniaudio are errors
}

// Sending audio data to device callback function. All audio mixing happens here.
RF_INTERNAL void rf__ma_callback__on_send_audio_data_to_device(ma_device*  pDevice, void* pFramesOut, const void* pFramesInput, ma_uint32 frameCount)
{
    // Mixing is basically just an accumulation, we need to initialize the output buffer to 0
    memset(pFramesOut, 0, frameCount*pDevice->playback.channels*ma_get_bytes_per_sample(pDevice->playback.format));

    // Using a mutex here for thread-safety which makes things not real-time
    // This is unlikely to be necessary for this project, but may want to consider how you might want to avoid this
    ma_mutex_lock(&rf__global_audio_context->audio_lock);
    {
        for (rf_audio_buffer* rf_audio_buffer = rf__global_audio_context->first_audio_buffer;
             rf_audio_buffer != NULL;
             rf_audio_buffer = rf_audio_buffer->next)
        {
            // Ignore stopped or paused sounds
            if (!rf_audio_buffer->playing || rf_audio_buffer->paused) continue;

            ma_uint32 framesRead = 0;

            while (1)
            {
                if (framesRead > frameCount)
                {
                    RF_LOG(RF_LOG_DEBUG, "Mixed too many frames from audio buffer");
                    break;
                }

                if (framesRead == frameCount) break;

                // Just read as much data as we can from the stream
                ma_uint32 framesToRead = (frameCount - framesRead);

                while (framesToRead > 0)
                {
                    float tempBuffer[1024]; // 512 frames for stereo

                    ma_uint32 framesToReadRightNow = framesToRead;
                    if (framesToReadRightNow > sizeof(tempBuffer)/sizeof(tempBuffer[0])/RF_DEVICE_CHANNELS)
                    {
                        framesToReadRightNow = sizeof(tempBuffer)/sizeof(tempBuffer[0])/RF_DEVICE_CHANNELS;
                    }

                    ma_uint32 framesJustRead = (ma_uint32)ma_pcm_converter_read(&rf_audio_buffer->dsp, tempBuffer, framesToReadRightNow);
                    if (framesJustRead > 0)
                    {
                        float* framesOut = (float* )pFramesOut + (framesRead*rf__global_audio_context->device.playback.channels);
                        float* framesIn  = tempBuffer;

                        rf__mix_audio_frames(framesOut, framesIn, framesJustRead, rf_audio_buffer->volume);

                        framesToRead -= framesJustRead;
                        framesRead += framesJustRead;
                    }

                    // If we weren't able to read all the frames we requested, break
                    if (framesJustRead < framesToReadRightNow)
                    {
                        if (!rf_audio_buffer->looping)
                        {
                            rf__stop_audio_buffer(rf_audio_buffer);
                            break;
                        }
                        else
                        {
                            // Should never get here, but just for safety,
                            // move the cursor position back to the start and continue the loop
                            rf_audio_buffer->frame_cursor_pos = 0;
                            continue;
                        }
                    }
                }

                // If for some reason we weren't able to read every frame we'll need to break from the loop
                // Not doing this could theoretically put us into an infinite loop
                if (framesToRead > 0) break;
            }
        }
    }

    ma_mutex_unlock(&rf__global_audio_context->audio_lock);
}

// DSP read from audio buffer callback function
RF_INTERNAL ma_uint32 rf__ma_callback__on_audio_buffer_dsp_read(ma_pcm_converter* pDSP, void* pFramesOut, ma_uint32 frameCount, void* pUserData)
{
    rf_audio_buffer* audio_buffer = (rf_audio_buffer*) pUserData;

    ma_uint32 sub_buffer_size_in_frames = (audio_buffer->frames_count > 1) ? audio_buffer->frames_count / 2 : audio_buffer->frames_count;
    ma_uint32 current_sub_buffer_index = audio_buffer->frame_cursor_pos / sub_buffer_size_in_frames;

    if (current_sub_buffer_index > 1)
    {
        RF_LOG(RF_LOG_DEBUG, "Frame cursor position moved too far forward in audio stream");
        return 0;
    }

    // Another thread can update the processed state of buffers so
    // we just take a copy here to try and avoid potential synchronization problems
    bool is_sub_buffer_processed[2];
    is_sub_buffer_processed[0] = audio_buffer->is_sub_buffer_processed[0];
    is_sub_buffer_processed[1] = audio_buffer->is_sub_buffer_processed[1];

    ma_uint32 frame_size_in_bytes = ma_get_bytes_per_sample(audio_buffer->dsp.formatConverterIn.config.formatIn) * audio_buffer->dsp.formatConverterIn.config.channels;

    // Fill out every frame until we find a buffer that's marked as processed. Then fill the remainder with 0
    ma_uint32 frames_read = 0;
    while (1)
    {
        // We break from this loop differently depending on the buffer's usage
        //  - For RF_INTERNAL buffers, we simply fill as much data as we can
        //  - For streaming buffers we only fill the halves of the buffer that are processed
        //    Unprocessed halves must keep their audio data in-tact
        if (audio_buffer->usage == RF_STATIC_AUDIO_BUFFER)
        {
            if (frames_read >= frameCount) break;
        }
        else
        {
            if (is_sub_buffer_processed[current_sub_buffer_index]) break;
        }

        ma_uint32 total_frames_remaining = (frameCount - frames_read);
        if (total_frames_remaining == 0) break;

        ma_uint32 frames_remaining_in_output_buffer;
        if (audio_buffer->usage == RF_STATIC_AUDIO_BUFFER)
        {
            frames_remaining_in_output_buffer = audio_buffer->frames_count - audio_buffer->frame_cursor_pos;
        }
        else
        {
            ma_uint32 first_frame_index_of_this_sub_buffer = sub_buffer_size_in_frames * current_sub_buffer_index;
            frames_remaining_in_output_buffer = sub_buffer_size_in_frames - (audio_buffer->frame_cursor_pos - first_frame_index_of_this_sub_buffer);
        }

        ma_uint32 frames_to_read = total_frames_remaining;
        if (frames_to_read > frames_remaining_in_output_buffer) frames_to_read = frames_remaining_in_output_buffer;

        memcpy((unsigned char*)pFramesOut + (frames_read * frame_size_in_bytes), audio_buffer->buffer + (audio_buffer->frame_cursor_pos * frame_size_in_bytes), frames_to_read * frame_size_in_bytes);
        audio_buffer->frame_cursor_pos = (audio_buffer->frame_cursor_pos + frames_to_read) % audio_buffer->frames_count;
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
                rf__stop_audio_buffer(audio_buffer);
                break;
            }
        }
    }

    // Zero-fill excess
    ma_uint32 total_frames_remaining = (frameCount - frames_read);
    if (total_frames_remaining > 0)
    {
        memset((unsigned char* )pFramesOut + (frames_read * frame_size_in_bytes), 0, total_frames_remaining * frame_size_in_bytes);

        // For RF_INTERNAL buffers we can fill the remaining frames with silence for safety, but we don't want
        // to report those frames as "read". The reason for this is that the caller uses the return value
        // to know whether or not a non-looping sound has finished playback.
        if (audio_buffer->usage != RF_STATIC_AUDIO_BUFFER)
        {
            frames_read += total_frames_remaining;
        }
    }

    return frames_read;
}

// This is the main mixing function. Mixing is pretty simple in this project - it's just an accumulation. framesOut is both an input and an output. It will be initially filled with zeros outside of this function.
RF_INTERNAL void rf__mix_audio_frames(float* framesOut, const float* framesIn, ma_uint32 frameCount, float localVolume)
{
    for (ma_uint32 iFrame = 0; iFrame < frameCount; ++iFrame)
    {
        for (ma_uint32 iChannel = 0; iChannel < rf__global_audio_context->device.playback.channels; ++iChannel)
        {
            float* frameOut = framesOut + (iFrame*rf__global_audio_context->device.playback.channels);
            const float* frameIn  = framesIn  + (iFrame*rf__global_audio_context->device.playback.channels);

            frameOut[iChannel] += (frameIn[iChannel]*rf__global_audio_context->master_volume*localVolume);
        }
    }
}

#pragma endregion

// Close the audio buffers pool
RF_INTERNAL void rf__close_audio_buffer_pool()
{
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) RF_FREE(rf__global_audio_context->audio_buffer_pool[i]);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Audio Buffer management
//----------------------------------------------------------------------------------

// Initialize a new audio buffer (filled with silence)
RF_INTERNAL rf_audio_buffer* rf__init_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 buffer_size_in_frames, int usage)
{
    rf_audio_buffer* audio_buffer = (rf_audio_buffer*) RF_MALLOC(sizeof(rf_audio_buffer));
    *audio_buffer = (rf_audio_buffer){0};
    audio_buffer->buffer = (unsigned char*) RF_MALLOC(buffer_size_in_frames * channels * ma_get_bytes_per_sample(format));
    memset(audio_buffer->buffer, 0, buffer_size_in_frames * channels * ma_get_bytes_per_sample(format));

    //RF_ASSERT(audio_buffer == NULL);

    // Audio data runs through a format converter
    ma_pcm_converter_config dspConfig;
    memset(&dspConfig, 0, sizeof(dspConfig));
    dspConfig.formatIn = format;
    dspConfig.formatOut = RF_DEVICE_FORMAT;
    dspConfig.channelsIn = channels;
    dspConfig.channelsOut = RF_DEVICE_CHANNELS;
    dspConfig.sampleRateIn = sample_rate;
    dspConfig.sampleRateOut = RF_DEVICE_SAMPLE_RATE;
    dspConfig.onRead = rf__ma_callback__on_audio_buffer_dsp_read;        // Callback on data reading
    dspConfig.pUserData = audio_buffer;              // Audio data pointer
    dspConfig.allowDynamicSampleRate = true;        // Required for pitch shifting

    ma_result result = ma_pcm_converter_init(&dspConfig, &audio_buffer->dsp);

    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "rf_init_audio_buffer() : Failed to create data conversion pipeline");
        RF_FREE(audio_buffer);
        return NULL;
    }

    // Init audio buffer values
    audio_buffer->volume = 1.0f;
    audio_buffer->pitch = 1.0f;
    audio_buffer->playing = false;
    audio_buffer->paused = false;
    audio_buffer->looping = false;
    audio_buffer->usage = usage;
    audio_buffer->frame_cursor_pos = 0;
    audio_buffer->frames_count = buffer_size_in_frames;

    // Buffers should be marked as processed by default so that a call to
    // UpdateAudioStream() immediately after initialization works correctly
    audio_buffer->is_sub_buffer_processed[0] = true;
    audio_buffer->is_sub_buffer_processed[1] = true;

    // Track audio buffer to linked list next position
    rf__track_audio_buffer(audio_buffer);

    return audio_buffer;
}

// Delete an audio buffer
RF_INTERNAL void rf__close_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    rf__untrack_audio_buffer(buffer);
    RF_FREE(buffer->buffer);
    RF_FREE(buffer);
}

// Check if an audio buffer is playing
RF_INTERNAL bool rf__is_audio_buffer_playing(rf_audio_buffer* buffer)
{
    return (buffer->playing && !buffer->paused);
}

// Play an audio buffer
// NOTE: Buffer is restarted to the start.
// Use PauseAudioBuffer() and ResumeAudioBuffer() if the playback position should be maintained.
RF_INTERNAL void rf__play_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    buffer->playing = true;
    buffer->paused = false;
    buffer->frame_cursor_pos = 0;
}

// Stop an audio buffer
RF_INTERNAL void rf__stop_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    if (rf__is_audio_buffer_playing(buffer))
    {
        buffer->playing = false;
        buffer->paused = false;
        buffer->frame_cursor_pos = 0;
        buffer->total_frames_processed = 0;
        buffer->is_sub_buffer_processed[0] = true;
        buffer->is_sub_buffer_processed[1] = true;
    }
}

// Pause an audio buffer
RF_INTERNAL void rf__pause_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);
    buffer->paused = true;
}

// Resume an audio buffer
RF_INTERNAL void rf__resume_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    buffer->paused = false;
}

// Set volume for an audio buffer
RF_INTERNAL void rf__set_audio_buffer_volume(rf_audio_buffer* buffer, float volume)
{
    RF_ASSERT(buffer != NULL);

    buffer->volume = volume;
}

// Set pitch for an audio buffer
RF_INTERNAL void rf__set_audio_buffer_pitch(rf_audio_buffer* buffer, float pitch)
{
    RF_ASSERT(buffer != NULL);

    float pitchMul = pitch/buffer->pitch;

    // Pitching is just an adjustment of the sample rate.
    // Note that this changes the duration of the sound:
    //  - higher pitches will make the sound faster
    //  - lower pitches make it slower
    ma_uint32 newOutputSampleRate = (ma_uint32)((float)buffer->dsp.src.config.sampleRateOut/pitchMul);
    buffer->pitch *= (float)buffer->dsp.src.config.sampleRateOut/newOutputSampleRate;

    ma_pcm_converter_set_output_sample_rate(&buffer->dsp, newOutputSampleRate);
}

// Track audio buffer to linked list next position
RF_INTERNAL void rf__track_audio_buffer(rf_audio_buffer* buffer)
{
    ma_mutex_lock(&rf__global_audio_context->audio_lock);
    {
        if (rf__global_audio_context->first_audio_buffer == NULL)
        {
            rf__global_audio_context->first_audio_buffer = buffer;
        }
        else
        {
            rf__global_audio_context->last_audio_buffer->next = buffer;
            buffer->prev = rf__global_audio_context->last_audio_buffer;
        }

        rf__global_audio_context->last_audio_buffer = buffer;
    }
    ma_mutex_unlock(&rf__global_audio_context->audio_lock);
}

// Untrack audio buffer from linked list
RF_INTERNAL void rf__untrack_audio_buffer(rf_audio_buffer* buffer)
{
    ma_mutex_lock(&rf__global_audio_context->audio_lock);
    {
        if (buffer->prev == NULL) rf__global_audio_context->first_audio_buffer = buffer->next;
        else buffer->prev->next = buffer->next;

        if (buffer->next == NULL) rf__global_audio_context->last_audio_buffer = buffer->prev;
        else buffer->next->prev = buffer->prev;

        buffer->prev = NULL;
        buffer->next = NULL;
    }
    ma_mutex_unlock(&rf__global_audio_context->audio_lock);
}

#pragma endregion

#pragma region core

RF_API int rf_audio_context_size()
{
    return sizeof(rf_audio_context);
}

// Initialize audio device
RF_API void rf_audio_init(rf_audio_context* audio_ctx, rf_allocator allocator)
{
     rf__global_audio_context = audio_ctx;
    *rf__global_audio_context = (rf_audio_context) {
        .master_volume = 1.0f
    };

    // Init audio context
    ma_context_config context_config = ma_context_config_init();
    context_config.logCallback = rf__ma_callback__on_log;

    ma_result result = ma_context_init(NULL, 0, &context_config, &rf__global_audio_context->context);
    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to initialize audio context");
        return;
    }

    // Init audio device
    // NOTE: Using the default device. Format is floating point because it simplifies mixing.
    ma_device_config config   = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL; // NULL for the default playback device.
    config.playback.format    = RF_DEVICE_FORMAT;
    config.playback.channels  = RF_DEVICE_CHANNELS;
    config.capture.pDeviceID  = NULL; // NULL for the default capture device.
    config.capture.format     = ma_format_s16;
    config.capture.channels   = 1;
    config.sampleRate         = RF_DEVICE_SAMPLE_RATE;
    config.dataCallback       = rf__ma_callback__on_send_audio_data_to_device;
    config.pUserData          = NULL;

    result = ma_device_init(&rf__global_audio_context->context, &config, &rf__global_audio_context->device);
    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to initialize audio playback device");
        ma_context_uninit(&rf__global_audio_context->context);
        return;
    }

    // Keep the device running the whole time. May want to consider doing something a bit smarter and only have the device running
    // while there's at least one sound being played.
    result = ma_device_start(&rf__global_audio_context->device);
    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to start audio playback device");
        ma_device_uninit(&rf__global_audio_context->device);
        ma_context_uninit(&rf__global_audio_context->context);
        return;
    }

    // Mixing happens on a seperate thread which means we need to synchronize. I'm using a mutex here to make things simple, but may
    // want to look at something a bit smarter later on to keep everything real-time, if that's necessary.
    if (ma_mutex_init(&rf__global_audio_context->context, &rf__global_audio_context->audio_lock) != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to create mutex for audio mixing");
        ma_device_uninit(&rf__global_audio_context->device);
        ma_context_uninit(&rf__global_audio_context->context);
        return;
    }

    RF_LOG(RF_LOG_INFO, "Audio device initialized successfully");
    RF_LOG_V(RF_LOG_INFO, "Audio backend: miniaudio / %s", ma_get_backend_name(rf__global_audio_context->context.backend));
    RF_LOG_V(RF_LOG_INFO, "Audio format: %s -> %s", ma_get_format_name(rf__global_audio_context->device.playback.format), ma_get_format_name(rf__global_audio_context->device.playback.internalFormat));
    RF_LOG_V(RF_LOG_INFO, "Audio channels: %d -> %d", rf__global_audio_context->device.playback.channels, rf__global_audio_context->device.playback.internalChannels);
    RF_LOG_V(RF_LOG_INFO, "Audio sample rate: %d -> %d", rf__global_audio_context->device.sample_rate, rf__global_audio_context->device.playback.internalSampleRate);
    RF_LOG_V(RF_LOG_INFO, "Audio buffer size: %d", rf__global_audio_context->device.playback.internalBufferSizeInFrames);

    // Initialise the multichannel buffer pool with dummy buffers
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++)
    {
        rf__global_audio_context->audio_buffer_pool[i] = rf__init_audio_buffer(RF_DEVICE_FORMAT, RF_DEVICE_CHANNELS, RF_DEVICE_SAMPLE_RATE, 0, RF_STATIC_AUDIO_BUFFER);
    }

    RF_LOG_V(RF_LOG_INFO, "Audio multichannel pool size: %i", RF_MAX_AUDIO_BUFFER_POOL_CHANNELS);

    rf__global_audio_context->is_audio_initialised = true;
}

//Sets the global context ptr
RF_API void rf_audio_set_context_ptr(rf_audio_context* audio_ctx)
{
    rf__global_audio_context = audio_ctx;
    RF_LOG(RF_LOG_WARNING, "Global context pointer set.");
}

// Close the audio device for all contexts
RF_API void rf_audio_cleanup(void)
{
    if (rf__global_audio_context->is_audio_initialised)
    {
        ma_mutex_uninit(&rf__global_audio_context->audio_lock);
        ma_device_uninit(&rf__global_audio_context->device);
        ma_context_uninit(&rf__global_audio_context->context);

        rf__close_audio_buffer_pool();

        RF_LOG(RF_LOG_INFO, "Audio device closed successfully");
    }
    else RF_LOG(RF_LOG_WARNING, "Could not close audio device because it is not currently initialized");
}

// Check if device has been initialized successfully
RF_API bool rf_is_audio_device_ready(void)
{
    return rf__global_audio_context->is_audio_initialised;
}

// Set master volume (listener)
RF_API void rf_set_master_volume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    else if (volume > 1.0f) volume = 1.0f;

    rf__global_audio_context->master_volume = volume;
}

#pragma endregion

#pragma region wave

RF_API rf_wave rf_load_wave_from_file(const char* filename)
{
    rf_wave rf_wave = { 0 };

    if (false) { }
    else if (rf__is_file_extension(filename, ".wav")) rf_wave = rf_load_wav(filename);
    else if (rf__is_file_extension(filename, ".ogg")) rf_wave = rf_load_ogg(filename);
    else if (rf__is_file_extension(filename, ".flac")) rf_wave = rf_load_flac(filename);
    else if (rf__is_file_extension(filename, ".mp3")) rf_wave = rf_load_mp3(filename);
    else RF_LOG_V(RF_LOG_WARNING, "[%s] Audio fileformat not supported, it can't be loaded", filename);

    return rf_wave;
}

RF_API int rf_wave_size(rf_wave wave)
{
    return wave.sample_count * wave.sample_size / 8 * wave.channels;
}

RF_API void rf_unload_wave(rf_wave wave, rf_allocator allocator)
{
    RF_FREE(allocator, wave.data);
}

// Export wave sample data to code (.h)
RF_API void rf_export_wave_as_code(rf_wave wave, const char* filename)
{
#define BYTES_TEXT_PER_LINE 20

    char varFileName[256] = { 0 };
    int dataSize = wave.sample_count * wave.channels * wave.sample_size / 8;

    FILE *txtFile = fopen(filename, "wt");

    if (txtFile != NULL)
    {
        fprintf(txtFile, "\n//////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(txtFile, "//                                                                              //\n");
        fprintf(txtFile, "// WaveAsCode exporter v1.0 - Wave data exported as an array of bytes           //\n");
        fprintf(txtFile, "//                                                                              //\n");
        fprintf(txtFile, "// more info and bugs-report:  github.com/raysan5/raylib                        //\n");
        fprintf(txtFile, "// feedback and support:       ray[at]raylib.com                                //\n");
        fprintf(txtFile, "//                                                                              //\n");
        fprintf(txtFile, "// Copyright (c) 2018 Ramon Santamaria (@raysan5)                               //\n");
        fprintf(txtFile, "//                                                                              //\n");
        fprintf(txtFile, "//////////////////////////////////////////////////////////////////////////////////\n\n");

#if !defined(RAUDIO_STANDALONE)
        // Get file name from path and convert variable name to uppercase
        //strcpy(varFileName, GetFileNameWithoutExt(filename)); //@Note: Replace GetFileNameWithoutExt
        for (int i = 0; varFileName[i] != '\0'; i++) if (varFileName[i] >= 'a' && varFileName[i] <= 'z') { varFileName[i] = varFileName[i] - 32; }
#else
        strcpy(varFileName, filename);
#endif

        fprintf(txtFile, "// Wave data information\n");
        fprintf(txtFile, "#define %s_SAMPLE_COUNT     %i\n", varFileName, wave.sample_count);
        fprintf(txtFile, "#define %s_SAMPLE_RATE      %i\n", varFileName, wave.sample_rate);
        fprintf(txtFile, "#define %s_SAMPLE_SIZE      %i\n", varFileName, wave.sample_size);
        fprintf(txtFile, "#define %s_CHANNELS         %i\n\n", varFileName, wave.channels);

        // Write byte data as hexadecimal text
        fprintf(txtFile, "RF_INTERNAL unsigned char %s_DATA[%i] = { ", varFileName, dataSize);
        for (int i = 0; i < dataSize - 1; i++) fprintf(txtFile, ((i%BYTES_TEXT_PER_LINE == 0)? "0x%x,\n" : "0x%x, "), ((unsigned char* )wave.data)[i]);
        fprintf(txtFile, "0x%x };\n", ((unsigned char* )wave.data)[dataSize - 1]);

        fclose(txtFile);
    }
}

// Convert wave data to desired format
RF_API void rf_format_wave(rf_wave* wave, int sample_rate, int sample_size, int channels)
{
    ma_format format_in  = ((wave->sample_size == 8) ? ma_format_u8 : ((wave->sample_size == 16) ? ma_format_s16 : ma_format_f32));
    ma_format format_out = ((sample_size == 8) ? ma_format_u8 : ((sample_size == 16) ? ma_format_s16 : ma_format_f32));

    ma_uint32 frame_count_in = wave->sample_count;  // Is wave->sampleCount actually the frame count? That terminology needs to change, if so.
    ma_uint32 frame_count = (ma_uint32) ma_convert_frames(NULL, format_out, channels, sample_rate, NULL, format_in, wave->channels, wave->sample_rate, frame_count_in);

    if (frame_count == 0)
    {
        RF_LOG(RF_LOG_ERROR, "WaveFormat() : Failed to get frame count for format conversion.");
        return;
    }

    void* data = RF_MALLOC(frame_count * channels * (sample_size / 8));

    // Does ma use allocations here???
    frame_count = (ma_uint32) ma_convert_frames(data, format_out, channels, sample_rate, wave->data, format_in, wave->channels, wave->sample_rate, frame_count_in);
    if (frame_count == 0)
    {
        RF_LOG(RF_LOG_ERROR, "WaveFormat() : Format conversion failed.");
        return;
    }

    wave->sample_count = frame_count;
    wave->sample_size = sample_size;
    wave->sample_rate = sample_rate;
    wave->channels = channels;
    RF_FREE_REPLACE_WITH_BUFFER(wave->data);
    wave->data = data;
}

RF_API rf_wave rf_copy_wave_to_buffer(rf_wave wave, void* dst, int dst_size)
{
    rf_wave result = { 0 };
    int wave_size = rf_wave_size(wave);

    if (wave.valid && dst && wave_size > 0 && dst_size >= wave_size)
    {
        memcpy(dst, wave.data, wave.sample_count * wave.channels * wave.sample_size / 8);

        result = (rf_wave) {
            .data = dst,
            .sample_count = wave.sample_count,
            .sample_rate = wave.sample_rate,
            .sample_size = wave.sample_size,
            .channels = wave.channels,
            .valid = true,
        };
    }

    return result;
}

RF_API rf_wave rf_copy_wave(rf_wave wave, rf_allocator allocator)
{
    rf_wave result = {0};
    int wave_size = rf_wave_size(wave);

    if (wave.valid && wave_size > 0)
    {
        void* dst = RF_ALLOC(allocator, wave_size);

        if (dst)
        {
            memcpy(dst, wave.data, wave.sample_count * wave.channels * wave.sample_size / 8);

            result = (rf_wave) {
                .data = dst,
                .sample_count = wave.sample_count,
                .sample_rate = wave.sample_rate,
                .sample_size = wave.sample_size,
                .channels = wave.channels,
                .valid = true,
            };
        }
    }

    return result;
}

// Crop a wave to defined samples range
RF_API void rf_crop_wave(rf_wave* rf_wave, int init_sample, int final_sample)
{
    if ((init_sample >= 0) && (init_sample < final_sample) &&
        (final_sample > 0) && ((unsigned int)final_sample < rf_wave->sample_count))
    {
        int sample_count = final_sample - init_sample;

        void* data = RF_MALLOC(sample_count*rf_wave->sample_size/8*rf_wave->channels);

        memcpy(data, (unsigned char* )rf_wave->data + (init_sample * rf_wave->channels * rf_wave->sample_size / 8), sample_count * rf_wave->channels * rf_wave->sample_size / 8);

        RF_FREE_REPLACE_WITH_BUFFER(rf_wave->data);
        rf_wave->data = data;
    }
    else RF_LOG(RF_LOG_WARNING, "Wave crop range out of bounds");
}

// Get samples data from wave as a floats array. Returned sample values are normalized to range [-1..1]
RF_API float* rf_get_wave_data(rf_wave rf_wave)
{
    float* samples = (float*) RF_MALLOC(rf_wave.sample_count*rf_wave.channels*sizeof(float));

    for (unsigned int i = 0; i < rf_wave.sample_count; i++)
    {
        for (unsigned int j = 0; j < rf_wave.channels; j++)
        {
            if (rf_wave.sample_size == 8) samples[rf_wave.channels*i + j] = (float)(((unsigned char* )rf_wave.data)[rf_wave.channels*i + j] - 127)/256.0f;
            else if (rf_wave.sample_size == 16) samples[rf_wave.channels*i + j] = (float)((short *)rf_wave.data)[rf_wave.channels*i + j]/32767.0f;
            else if (rf_wave.sample_size == 32) samples[rf_wave.channels*i + j] = ((float* )rf_wave.data)[rf_wave.channels*i + j];
        }
    }

    return samples;
}

#pragma endregion

#pragma region short audio stream

// Load sound from file
// NOTE: The entire file is loaded to memory to be played (no-streaming)
RF_API rf_short_audio rf_load_short_audio_from_file(const char* filename)
{
    rf_wave rf_wave = rf_load_wave_from_file(filename);

    rf_short_audio rf_short_audio = rf_load_short_audio_from_wave(rf_wave);

    rf_unload_wave(rf_wave);       // Sound is loaded, we can unload wave

    return rf_short_audio;
}

// Load sound from wave data
// NOTE: Wave data must be unallocated manually
RF_API rf_short_audio rf_load_short_audio_from_wave(rf_wave wave)
{
    rf_short_audio short_audio = {0 };

    if (wave.data != NULL)
    {
        // When using miniaudio we need to do our own mixing.
        // To simplify this we need convert the format of each sound to be consistent with
        // the format used to open the playback device. We can do this two ways:
        //
        //   1) Convert the whole sound in one go at load time (here).
        //   2) Convert the audio data in chunks at mixing time.
        //
        // First option has been selected, format conversion is done on the loading stage.
        // The downside is that it uses more memory if the original sound is u8 or s16.
        ma_format formatIn  = ((wave.sample_size == 8) ? ma_format_u8 : ((wave.sample_size == 16) ? ma_format_s16 : ma_format_f32));
        ma_uint32 frameCountIn = wave.sample_count / wave.channels;

        ma_uint32 frameCount = (ma_uint32)ma_convert_frames(NULL, RF_DEVICE_FORMAT, RF_DEVICE_CHANNELS, RF_DEVICE_SAMPLE_RATE, NULL, formatIn, wave.channels, wave.sample_rate, frameCountIn);
        if (frameCount == 0) RF_LOG(RF_LOG_WARNING, "LoadSoundFromWave() : Failed to get frame count for format conversion");

        rf_audio_buffer* rf_audio_buffer = rf__init_audio_buffer(RF_DEVICE_FORMAT, RF_DEVICE_CHANNELS, RF_DEVICE_SAMPLE_RATE, frameCount, RF_STATIC_AUDIO_BUFFER);
        if (rf_audio_buffer == NULL) RF_LOG(RF_LOG_WARNING, "LoadSoundFromWave() : Failed to create audio buffer");

        frameCount = (ma_uint32)ma_convert_frames(rf_audio_buffer->buffer, rf_audio_buffer->dsp.formatConverterIn.config.formatIn, rf_audio_buffer->dsp.formatConverterIn.config.channels, rf_audio_buffer->dsp.src.config.sampleRateIn, wave.data, formatIn, wave.channels, wave.sample_rate, frameCountIn);
        if (frameCount == 0) RF_LOG(RF_LOG_WARNING, "LoadSoundFromWave() : Format conversion failed");

        short_audio.sample_count = frameCount*RF_DEVICE_CHANNELS;
        short_audio.stream.sample_rate = RF_DEVICE_SAMPLE_RATE;
        short_audio.stream.sample_size = 32;
        short_audio.stream.channels = RF_DEVICE_CHANNELS;
        short_audio.stream.buffer = rf_audio_buffer;
    }

    return short_audio;
}

// Update sound buffer with new data
RF_API void rf_update_short_audio(rf_short_audio short_audio, const void* data, int samples_count)
{
    rf_audio_buffer* rf_audio_buffer = short_audio.stream.buffer;

    RF_ASSERT(rf_audio_buffer != NULL);

    rf__stop_audio_buffer(rf_audio_buffer);

    // TODO: May want to lock/unlock this since this data buffer is read at mixing time
    memcpy(rf_audio_buffer->buffer, data, samples_count * rf_audio_buffer->dsp.formatConverterIn.config.channels * ma_get_bytes_per_sample(rf_audio_buffer->dsp.formatConverterIn.config.formatIn));
}

// Unload sound
RF_API void rf_unload_sound(rf_short_audio short_audio)
{
    rf__close_audio_buffer(short_audio.stream.buffer);
}

// Play a sound
RF_API void rf_play_short_audio(rf_short_audio short_audio)
{
    rf__play_audio_buffer(short_audio.stream.buffer);
}

// Play a sound in the multichannel buffer pool
RF_API void rf_play_short_audio_multi(rf_short_audio short_audio)
{
    int index = -1;
    unsigned int oldAge = 0;
    int oldIndex = -1;

    // find the first non playing pool entry
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++)
    {
        if (rf__global_audio_context->audio_buffer_pool_channels[i] > oldAge)
        {
            oldAge = rf__global_audio_context->audio_buffer_pool_channels[i];
            oldIndex = i;
        }

        if (!rf__is_audio_buffer_playing(rf__global_audio_context->audio_buffer_pool[i]))
        {
            index = i;
            break;
        }
    }

    // If no none playing pool members can be index choose the oldest
    if (index == -1)
    {
        RF_LOG_V(RF_LOG_WARNING,"pool age %i ended a sound early no room in buffer pool", rf__global_audio_context->audio_buffer_pool_counter);

        if (oldIndex == -1)
        {
            // Shouldn't be able to get here... but just in case something odd happens!
            RF_LOG(RF_LOG_ERROR,"sound buffer pool couldn't determine oldest buffer not playing sound");

            return;
        }

        index = oldIndex;

        // Just in case...
        rf__stop_audio_buffer(rf__global_audio_context->audio_buffer_pool[index]);
    }

    // Experimentally mutex lock doesn't seem to be needed this makes sense
    // as audioBufferPool[index] isn't playing and the only stuff we're copying
    // shouldn't be changing...

    rf__global_audio_context->audio_buffer_pool_channels[index] = rf__global_audio_context->audio_buffer_pool_counter;
    rf__global_audio_context->audio_buffer_pool_counter++;

    rf__global_audio_context->audio_buffer_pool[index]->volume = short_audio.stream.buffer->volume;
    rf__global_audio_context->audio_buffer_pool[index]->pitch = short_audio.stream.buffer->pitch;
    rf__global_audio_context->audio_buffer_pool[index]->looping = short_audio.stream.buffer->looping;
    rf__global_audio_context->audio_buffer_pool[index]->usage = short_audio.stream.buffer->usage;
    rf__global_audio_context->audio_buffer_pool[index]->is_sub_buffer_processed[0] = false;
    rf__global_audio_context->audio_buffer_pool[index]->is_sub_buffer_processed[1] = false;
    rf__global_audio_context->audio_buffer_pool[index]->frames_count = short_audio.stream.buffer->frames_count;
    rf__global_audio_context->audio_buffer_pool[index]->buffer = short_audio.stream.buffer->buffer;

    rf__play_audio_buffer(rf__global_audio_context->audio_buffer_pool[index]);
}

// Stop any sound played with PlaySoundMulti()
RF_API void rf_stop_short_audio_multi(void)
{
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) rf__stop_audio_buffer(rf__global_audio_context->audio_buffer_pool[i]);
}

// Get number of sounds playing in the multichannel buffer pool
RF_API int rf_get_short_audios_playing(void)
{
    int counter = 0;

    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++)
    {
        if (rf__is_audio_buffer_playing(rf__global_audio_context->audio_buffer_pool[i])) counter++;
    }

    return counter;
}

// Pause a sound
RF_API void rf_pause_short_audio(rf_short_audio short_audio)
{
    rf__pause_audio_buffer(short_audio.stream.buffer);
}

// Resume a paused sound
RF_API void rf_resume_short_audio(rf_short_audio short_audio)
{
    rf__resume_audio_buffer(short_audio.stream.buffer);
}

// Stop reproducing a sound
RF_API void rf_stop_short_audio(rf_short_audio short_audio)
{
    rf__stop_audio_buffer(short_audio.stream.buffer);
}

// Check if a sound is playing
RF_API bool rf_is_short_audio_playing(rf_short_audio short_audio)
{
    return rf__is_audio_buffer_playing(short_audio.stream.buffer);
}

// Set volume for a sound
RF_API void rf_set_short_audio_volume(rf_short_audio short_audio, float volume)
{
    rf__set_audio_buffer_volume(short_audio.stream.buffer, volume);
}

// Set pitch for a sound
RF_API void rf_set_short_audio_pitch(rf_short_audio short_audio, float pitch)
{
    rf__set_audio_buffer_pitch(short_audio.stream.buffer, pitch);
}

#pragma endregion

#pragma region long audio stream

// Load music stream from file
RF_API rf_long_audio rf_load_long_audio(const char* filename)
{
    rf_long_audio long_audio = { 0 };
    bool musicLoaded = false;

    if (false) { }
    #ifndef RF_NO_OGG_LOADING
    else if (rf__is_file_extension(filename, ".ogg"))
    {
        // Open ogg audio stream
        long_audio.ctx_data = stb_vorbis_open_filename(filename, NULL, NULL);

        stb_vorbis_open_memory();

        if (long_audio.ctx_data != NULL)
        {
            long_audio.ctx_type = RF_AUDIO_FORMAT_OGG;
            stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis*)long_audio.ctx_data); // Get Ogg file info

            // OGG bit rate defaults to 16 bit, it's enough for compressed format
            long_audio.stream = rf_create_audio_stream(info.sample_rate, 16, info.channels);
            long_audio.sample_count = (unsigned int) stb_vorbis_stream_length_in_samples((stb_vorbis*)long_audio.ctx_data) * info.channels;
            long_audio.loop_count = 0; // Infinite loop by default
            musicLoaded = true;
        }
    }
    #endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
    else if (rf__is_file_extension(filename, ".flac"))
    {
        long_audio.ctx_data = drflac_open_file(filename);

        if (long_audio.ctx_data != NULL)
        {
            long_audio.ctx_type = RF_AUDIO_FORMAT_FLAC;
            drflac *ctxFlac = (drflac *)long_audio.ctx_data;

            long_audio.stream = rf_create_audio_stream(ctxFlac->sample_rate, ctxFlac->bitsPerSample, ctxFlac->channels);
            long_audio.sample_count = (unsigned int)ctxFlac->totalSampleCount;
            long_audio.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
    else if (rf__is_file_extension(filename, ".mp3"))
    {
        drmp3* ctxMp3 = (drmp3*) RF_MALLOC(sizeof(drmp3));
        long_audio.ctx_data = ctxMp3;

        int result = drmp3_init_file(ctxMp3, filename, NULL, NULL);

        if (result > 0)
        {
            long_audio.ctx_type = RF_AUDIO_FORMAT_MP3;

            long_audio.stream = rf_create_audio_stream(ctxMp3->sampleRate, 32, ctxMp3->channels);
            long_audio.sample_count = drmp3_get_pcm_frame_count(ctxMp3)*ctxMp3->channels;
            long_audio.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RF_SUPPORT_FILEFORMAT_XM)
    else if (rf__is_file_extension(filename, ".xm"))
    {
        jar_xm_context_t *ctxXm = NULL;

        int result = jar_xm_create_context_from_file(&ctxXm, 48000, filename);

        if (result == 0)    // XM context created successfully
        {
            long_audio.ctx_type = RF_AUDIO_FORMAT_XM;
            jar_xm_set_max_loop_count(ctxXm, 0);    // Set infinite number of loops

            // NOTE: Only stereo is supported for XM
            long_audio.stream = rf_create_audio_stream(48000, 16, 2);
            long_audio.sample_count = (unsigned int)jar_xm_get_remaining_samples(ctxXm);
            long_audio.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;

            long_audio.ctx_data = ctxXm;
        }
    }
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MOD)
    else if (rf__is_file_extension(filename, ".mod"))
    {
        jar_mod_context_t* ctxMod = (jar_mod_context_t*) RF_MALLOC(sizeof(jar_mod_context_t));
        long_audio.ctx_data = ctxMod;

        jar_mod_init(ctxMod);
        int result = jar_mod_load_file(ctxMod, filename);

        if (result > 0)
        {
            long_audio.ctx_type = RF_AUDIO_FORMAT_MOD;

            // NOTE: Only stereo is supported for MOD
            long_audio.stream = rf_create_audio_stream(48000, 16, 2);
            long_audio.sample_count = (unsigned int)jar_mod_max_samples(ctxMod);
            long_audio.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif

    if (!musicLoaded)
    {
        if (false) { }
        #if defined(RF_SUPPORT_FILEFORMAT_OGG)
            else if (long_audio.ctx_type == rf_audio_context_ogg) stb_vorbis_close((stb_vorbis *)long_audio.ctx_data);
        #endif
        #if defined(RF_SUPPORT_FILEFORMAT_FLAC)
            else if (long_audio.ctx_type == RF_AUDIO_FORMAT_FLAC) drflac_free((drflac *)long_audio.ctx_data);
        #endif
        #if defined(RF_SUPPORT_FILEFORMAT_MP3)
            else if (long_audio.ctx_type == RF_AUDIO_FORMAT_MP3) { drmp3_uninit((drmp3 *)long_audio.ctx_data); RF_FREE_REPLACE_WITH_FUNC_ALLOCATOR(long_audio.ctx_data); }
        #endif
        #if defined(RF_SUPPORT_FILEFORMAT_XM)
            else if (long_audio.ctx_type == RF_AUDIO_FORMAT_XM) jar_xm_free_context((jar_xm_context_t *)long_audio.ctx_data);
        #endif
        #if defined(RF_SUPPORT_FILEFORMAT_MOD)
            else if (long_audio.ctx_type == RF_AUDIO_FORMAT_MOD) { jar_mod_unload((jar_mod_context_t *)long_audio.ctx_data); RF_FREE_REPLACE_WITH_FUNC_ALLOCATOR(long_audio.ctx_data); }
        #endif

        RF_LOG_V(RF_LOG_WARNING, "[%s] Music file could not be opened", filename);
    }
    else
    {
        // Show some music stream info
        RF_LOG_V(RF_LOG_INFO, "[%s] Music file successfully loaded:", filename);
        RF_LOG_V(RF_LOG_INFO, "   Total samples: %i", long_audio.sample_count);
        RF_LOG_V(RF_LOG_INFO, "   Sample rate: %i Hz", long_audio.stream.sample_rate);
        RF_LOG_V(RF_LOG_INFO, "   Sample size: %i bits", long_audio.stream.sample_size);
        RF_LOG_V(RF_LOG_INFO, "   Channels: %i (%s)", long_audio.stream.channels, (long_audio.stream.channels == 1)? "Mono" : (long_audio.stream.channels == 2)? "Stereo" : "Multi");
    }

    return long_audio;
}

// Unload music stream
RF_API void rf_unload_long_audio(rf_long_audio long_audio)
{
    rf_close_audio_stream(long_audio.stream);

    if (false) { }
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
    else if (long_audio.ctx_type == rf_audio_context_ogg) stb_vorbis_close((stb_vorbis *)long_audio.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
    else if (long_audio.ctx_type == RF_AUDIO_FORMAT_FLAC) drflac_free((drflac *)long_audio.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
    else if (long_audio.ctx_type == RF_AUDIO_FORMAT_MP3) { drmp3_uninit((drmp3 *)long_audio.ctx_data); RF_FREE_REPLACE_WITH_FUNC_ALLOCATOR(long_audio.ctx_data); }
#endif
#if defined(RF_SUPPORT_FILEFORMAT_XM)
    else if (long_audio.ctx_type == RF_AUDIO_FORMAT_XM) jar_xm_free_context((jar_xm_context_t *)long_audio.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MOD)
    else if (long_audio.ctx_type == RF_AUDIO_FORMAT_MOD) { jar_mod_unload((jar_mod_context_t *)long_audio.ctx_data); RF_FREE_REPLACE_WITH_FUNC_ALLOCATOR(long_audio.ctx_data); }
#endif
}

// Start music playing (open stream)
RF_API void rf_play_long_audio(rf_long_audio long_audio)
{
    rf_audio_buffer* rf_audio_buffer = long_audio.stream.buffer;

    if (rf_audio_buffer != NULL)
    {
        // For music streams, we need to make sure we maintain the frame cursor position
        // This is a hack for this section of code in UpdateMusicStream()
        // NOTE: In case window is minimized, music stream is stopped, just make sure to
        // play again on window restore: if (IsMusicPlaying(music)) PlayMusicStream(music);
        ma_uint32 frame_cursor_pos = rf_audio_buffer->frame_cursor_pos;
        rf_play_audio_stream(long_audio.stream);  // WARNING: This resets the cursor position.
        rf_audio_buffer->frame_cursor_pos = frame_cursor_pos;
    }
    else RF_LOG(RF_LOG_ERROR, "PlayMusicStream() : No audio buffer");

}

// Pause music playing
RF_API void rf_pause_long_audio(rf_long_audio long_audio)
{
    rf_pause_audio_stream(long_audio.stream);
}

// Resume music playing
RF_API void rf_resume_long_audio(rf_long_audio long_audio)
{
    rf_resume_audio_stream(long_audio.stream);
}

// Stop music playing (close stream)
RF_API void rf_stop_long_audio(rf_long_audio long_audio)
{
    rf_stop_audio_stream(long_audio.stream);

    // Restart music context
    switch (long_audio.ctx_type)
    {
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
        case rf_audio_context_ogg: stb_vorbis_seek_start((stb_vorbis *)long_audio.ctx_data); break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
        case RF_AUDIO_FORMAT_FLAC: drflac_seek_to_pcm_frame((drflac *)long_audio.ctx_data, 0); break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
        case RF_AUDIO_FORMAT_MP3: drmp3_seek_to_pcm_frame((drmp3 *)long_audio.ctx_data, 0); break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_XM)
        case RF_AUDIO_FORMAT_XM: jar_xm_reset((jar_xm_context_t *)long_audio.ctx_data); break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MOD)
        case RF_AUDIO_FORMAT_MOD: jar_mod_seek_start((jar_mod_context_t *)long_audio.ctx_data); break;
#endif
        default: break;
    }
}

// Update (re-fill) music buffers if data already processed
RF_API void rf_update_long_audio(rf_long_audio long_audio)
{
    bool streamEnding = false;

    unsigned int sub_buffer_frames_count = long_audio.stream.buffer->frames_count / 2;

    // NOTE: Using dynamic allocation because it could require more than 16KB
    void* pcm = RF_MALLOC(sub_buffer_frames_count * long_audio.stream.channels * long_audio.stream.sample_size / 8);
    memset(pcm, 0, sub_buffer_frames_count * long_audio.stream.channels * long_audio.stream.sample_size / 8);

    int samplesCount = 0;    // Total size of data streamed in L+R samples for xm floats, individual L or R for ogg shorts

    // TODO: Get the sampleLeft using totalFramesProcessed... but first, get total frames processed correctly...
    //ma_uint32 frameSizeInBytes = ma_get_bytes_per_sample(music.stream.buffer->dsp.formatConverterIn.config.formatIn)*music.stream.buffer->dsp.formatConverterIn.config.channels;
    int sampleLeft = long_audio.sample_count - (long_audio.stream.buffer->total_frames_processed * long_audio.stream.channels);

    while (rf_is_audio_stream_processed(long_audio.stream))
    {
        if ((sampleLeft / long_audio.stream.channels) >= sub_buffer_frames_count) samplesCount = sub_buffer_frames_count * long_audio.stream.channels;
        else samplesCount = sampleLeft;

        switch (long_audio.ctx_type)
        {
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
            case rf_audio_context_ogg:
            {
                // NOTE: Returns the number of samples to process (be careful! we ask for number of shorts!)
                stb_vorbis_get_samples_short_interleaved((stb_vorbis *)long_audio.ctx_data, long_audio.stream.channels, (short *)pcm, samplesCount);

            } break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
            case RF_AUDIO_FORMAT_FLAC:
            {
                // NOTE: Returns the number of samples to process (not required)
                drflac_read_pcm_frames_s16((drflac *)long_audio.ctx_data, samplesCount, (short *)pcm);

            } break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
            case RF_AUDIO_FORMAT_MP3:
            {
                // NOTE: samplesCount, actually refers to framesCount and returns the number of frames processed
                drmp3_read_pcm_frames_f32((drmp3 *)long_audio.ctx_data, samplesCount/long_audio.stream.channels, (float* )pcm);

            } break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_XM)
            case RF_AUDIO_FORMAT_XM:
            {
                // NOTE: Internally this function considers 2 channels generation, so samplesCount/2
                jar_xm_generate_samples_16bit((jar_xm_context_t *)long_audio.ctx_data, (short *)pcm, samplesCount/2);
            } break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MOD)
            case RF_AUDIO_FORMAT_MOD:
            {
                // NOTE: 3rd parameter (nbsample) specify the number of stereo 16bits samples you want, so sampleCount/2
                jar_mod_fillbuffer((jar_mod_context_t *)long_audio.ctx_data, (short *)pcm, samplesCount/2, 0);
            } break;
#endif
            default: break;
        }

        rf_update_audio_stream(long_audio.stream, pcm, samplesCount);

        if ((long_audio.ctx_type == RF_AUDIO_FORMAT_XM) || (long_audio.ctx_type == RF_AUDIO_FORMAT_MOD))
        {
            if (samplesCount > 1) sampleLeft -= samplesCount/2;
            else sampleLeft -= samplesCount;
        }
        else sampleLeft -= samplesCount;

        if (sampleLeft <= 0)
        {
            streamEnding = true;
            break;
        }
    }

    // Free allocated pcm data
    RF_FREE(pcm);

    // Reset audio stream for looping
    if (streamEnding)
    {
        rf_stop_long_audio(long_audio); // Stop music (and reset)

        // Decrease loopCount to stop when required
        if (long_audio.loop_count > 1)
        {
            long_audio.loop_count--; // Decrease loop count
            rf_play_long_audio(long_audio); // Play again
        }
        else if (long_audio.loop_count == 0) rf_play_long_audio(long_audio);
    }
    else
    {
        // NOTE: In case window is minimized, music stream is stopped,
        // just make sure to play again on window restore
        if (rf_is_long_audio_playing(long_audio)) rf_play_long_audio(long_audio);
    }
}

// Check if any music is playing
RF_API bool rf_is_long_audio_playing(rf_long_audio long_audio)
{
    return rf_is_audio_stream_playing(long_audio.stream);
}

// Set volume for music
RF_API void rf_set_long_audio_volume(rf_long_audio long_audio, float volume)
{
    rf_set_audio_stream_volume(long_audio.stream, volume);
}

// Set pitch for music
RF_API void rf_set_long_audio_pitch(rf_long_audio long_audio, float pitch)
{
    rf_set_audio_stream_pitch(long_audio.stream, pitch);
}

// Set music loop count (loop repeats). If set to 0, means infinite loop
RF_API void rf_set_long_audio_loop_count(rf_long_audio long_audio, int count)
{
    long_audio.loop_count = count;
}

// Get music time length (in seconds)
RF_API float rf_get_long_audio_time_length(rf_long_audio long_audio)
{
    float totalSeconds = 0.0f;

    totalSeconds = (float)long_audio.sample_count / (long_audio.stream.sample_rate * long_audio.stream.channels);

    return totalSeconds;
}

// Get current music time played (in seconds)
RF_API float rf_get_long_audio_time_played(rf_long_audio long_audio)
{
    float secondsPlayed = 0.0f;

    //ma_uint32 frameSizeInBytes = ma_get_bytes_per_sample(music.stream.buffer->dsp.formatConverterIn.config.formatIn)*music.stream.buffer->dsp.formatConverterIn.config.channels;
    unsigned int samplesPlayed = long_audio.stream.buffer->total_frames_processed * long_audio.stream.channels;
    secondsPlayed = (float)samplesPlayed/(long_audio.stream.sample_rate * long_audio.stream.channels);

    return secondsPlayed;
}

#pragma endregion

#pragma region audio stream

// Init audio stream (to stream audio pcm data)
RF_API rf_audio_stream rf_create_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels)
{
    rf_audio_stream stream = { 0 };

    stream.sample_rate = sample_rate;
    stream.sample_size = sample_size;
    stream.channels = channels;

    ma_format formatIn = ((stream.sample_size == 8)? ma_format_u8 : ((stream.sample_size == 16)? ma_format_s16 : ma_format_f32));

    // The size of a streaming buffer must be at least double the size of a period
    unsigned int periodSize = rf__global_audio_context->device.playback.internalBufferSizeInFrames/rf__global_audio_context->device.playback.internalPeriods;
    unsigned int subBufferSize = RF_AUDIO_BUFFER_SIZE;

    if (subBufferSize < periodSize) subBufferSize = periodSize;

    stream.buffer = rf__init_audio_buffer(formatIn, stream.channels, stream.sample_rate, subBufferSize*2, RF_STREAMED_AUDIO_BUFFER);

    if (stream.buffer != NULL)
    {
        stream.buffer->looping = true;    // Always loop for streaming buffers
        RF_LOG_V(RF_LOG_INFO, "Audio stream loaded successfully (%i Hz, %i bit, %s)", stream.sample_rate, stream.sample_size, (stream.channels == 1)? "Mono" : "Stereo");
    }
    else RF_LOG(RF_LOG_ERROR, "InitAudioStream() : Failed to create audio buffer");

    return stream;
}

// Close audio stream and free memory
RF_API void rf_close_audio_stream(rf_audio_stream stream)
{
    rf__close_audio_buffer(stream.buffer);

    RF_LOG(RF_LOG_INFO, "Unloaded audio stream data");
}

// Update audio stream buffers with data
// NOTE 1: Only updates one buffer of the stream source: unqueue -> update -> queue
// NOTE 2: To unqueue a buffer it needs to be processed: IsAudioStreamProcessed()
RF_API void rf_update_audio_stream(rf_audio_stream stream, const void* data, int samplesCount)
{
    rf_audio_buffer* rf_audio_buffer = stream.buffer;

    if (rf_audio_buffer != NULL)
    {
        if (rf_audio_buffer->is_sub_buffer_processed[0] || rf_audio_buffer->is_sub_buffer_processed[1])
        {
            ma_uint32 subBufferToUpdate = 0;

            if (rf_audio_buffer->is_sub_buffer_processed[0] && rf_audio_buffer->is_sub_buffer_processed[1])
            {
                // Both buffers are available for updating.
                // Update the first one and make sure the cursor is moved back to the front.
                subBufferToUpdate = 0;
                rf_audio_buffer->frame_cursor_pos = 0;
            }
            else
            {
                // Just update whichever sub-buffer is processed.
                subBufferToUpdate = (rf_audio_buffer->is_sub_buffer_processed[0])? 0 : 1;
            }

            ma_uint32 subBufferSizeInFrames = rf_audio_buffer->frames_count / 2;
            unsigned char* subBuffer = rf_audio_buffer->buffer + ((subBufferSizeInFrames*stream.channels*(stream.sample_size/8))*subBufferToUpdate);

            // TODO: Get total frames processed on this buffer... DOES NOT WORK.
            rf_audio_buffer->total_frames_processed += subBufferSizeInFrames;

            // Does this API expect a whole buffer to be updated in one go?
            // Assuming so, but if not will need to change this logic.
            if (subBufferSizeInFrames >= (ma_uint32)samplesCount/stream.channels)
            {
                ma_uint32 framesToWrite = subBufferSizeInFrames;

                if (framesToWrite > ((ma_uint32)samplesCount/stream.channels)) framesToWrite = (ma_uint32)samplesCount/stream.channels;

                ma_uint32 bytesToWrite = framesToWrite*stream.channels*(stream.sample_size/8);
                memcpy(subBuffer, data, bytesToWrite);

                // Any leftover frames should be filled with zeros.
                ma_uint32 leftoverFrameCount = subBufferSizeInFrames - framesToWrite;

                if (leftoverFrameCount > 0) memset(subBuffer + bytesToWrite, 0, leftoverFrameCount*stream.channels*(stream.sample_size/8));

                rf_audio_buffer->is_sub_buffer_processed[subBufferToUpdate] = false;
            }
            else RF_LOG(RF_LOG_ERROR, "UpdateAudioStream() : Attempting to write too many frames to buffer");
        }
        else RF_LOG(RF_LOG_ERROR, "UpdateAudioStream() : Audio buffer not available for updating");
    }
    else RF_LOG(RF_LOG_ERROR, "UpdateAudioStream() : No audio buffer");
}

// Check if any audio stream buffers requires refill
RF_API bool rf_is_audio_stream_processed(rf_audio_stream stream)
{
    if (stream.buffer == NULL)
    {
        RF_LOG(RF_LOG_ERROR, "IsAudioStreamProcessed() : No audio buffer");
        return false;
    }

    return (stream.buffer->is_sub_buffer_processed[0] || stream.buffer->is_sub_buffer_processed[1]);
}

// Play audio stream
RF_API void rf_play_audio_stream(rf_audio_stream stream)
{
    rf__play_audio_buffer(stream.buffer);
}

// Play audio stream
RF_API void rf_pause_audio_stream(rf_audio_stream stream)
{
    rf__pause_audio_buffer(stream.buffer);
}

// Resume audio stream playing
RF_API void rf_resume_audio_stream(rf_audio_stream stream)
{
    rf__resume_audio_buffer(stream.buffer);
}

// Check if audio stream is playing.
RF_API bool rf_is_audio_stream_playing(rf_audio_stream stream)
{
    return rf__is_audio_buffer_playing(stream.buffer);
}

// Stop audio stream
RF_API void rf_stop_audio_stream(rf_audio_stream stream)
{
    rf__stop_audio_buffer(stream.buffer);
}

RF_API void rf_set_audio_stream_volume(rf_audio_stream stream, float volume)
{
    rf__set_audio_buffer_volume(stream.buffer, volume);
}

RF_API void rf_set_audio_stream_pitch(rf_audio_stream stream, float pitch)
{
    rf__set_audio_buffer_pitch(stream.buffer, pitch);
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

#if defined(RF_SUPPORT_FILEFORMAT_WAV)
// Load WAV file into Wave structure
RF_INTERNAL rf_wave rf_load_wav(const char* filename)
{
    // Basic WAV headers structs
    typedef struct {
        char chunkID[4];
        int chunkSize;
        char format[4];
    } WAVRiffHeader;

    typedef struct {
        char subChunkID[4];
        int subChunkSize;
        short audioFormat;
        short numChannels;
        int sample_rate;
        int byteRate;
        short blockAlign;
        short bitsPerSample;
    } WAVFormat;

    typedef struct {
        char subChunkID[4];
        int subChunkSize;
    } WAVData;

    WAVRiffHeader wavRiffHeader;
    WAVFormat wavFormat;
    WAVData wavData;

    rf_wave rf_wave = { 0 };
    FILE *wavFile;

    wavFile = fopen(filename, "rb");

    if (wavFile == NULL)
    {
        RF_LOG(RF_LOG_TYPE_WARNING, "[%s] WAV file could not be opened", filename);
        rf_wave.data = NULL;
    }
    else
    {
        // Read in the first chunk into the struct
        fread(&wavRiffHeader, sizeof(WAVRiffHeader), 1, wavFile);

        // Check for RIFF and WAVE tags
        if (strncmp(wavRiffHeader.chunkID, "RIFF", 4) ||
            strncmp(wavRiffHeader.format, "WAVE", 4))
        {
                RF_LOG(RF_LOG_TYPE_WARNING, "[%s] Invalid RIFF or WAVE Header", filename);
        }
        else
        {
            // Read in the 2nd chunk for the wave info
            fread(&wavFormat, sizeof(WAVFormat), 1, wavFile);

            // Check for fmt tag
            if ((wavFormat.subChunkID[0] != 'f') || (wavFormat.subChunkID[1] != 'm') ||
                (wavFormat.subChunkID[2] != 't') || (wavFormat.subChunkID[3] != ' '))
            {
                RF_LOG(RF_LOG_TYPE_WARNING, "[%s] Invalid Wave format", filename);
            }
            else
            {
                // Check for extra parameters;
                if (wavFormat.subChunkSize > 16) fseek(wavFile, sizeof(short), SEEK_CUR);

                // Read in the the last byte of data before the sound file
                fread(&wavData, sizeof(WAVData), 1, wavFile);

                // Check for data tag
                if ((wavData.subChunkID[0] != 'd') || (wavData.subChunkID[1] != 'a') ||
                    (wavData.subChunkID[2] != 't') || (wavData.subChunkID[3] != 'a'))
                {
                    RF_LOG(RF_LOG_TYPE_WARNING, "[%s] Invalid data header", filename);
                }
                else
                {
                    // Allocate memory for data
                    rf_wave.data = RF_MALLOC(wavData.subChunkSize);

                    // Read in the sound data into the soundData variable
                    fread(rf_wave.data, wavData.subChunkSize, 1, wavFile);

                    // Store wave parameters
                    rf_wave.sample_rate = wavFormat.sample_rate;
                    rf_wave.sample_size = wavFormat.bitsPerSample;
                    rf_wave.channels = wavFormat.numChannels;

                    // NOTE: Only support 8 bit, 16 bit and 32 bit sample sizes
                    if ((rf_wave.sample_size != 8) && (rf_wave.sample_size != 16) && (rf_wave.sample_size != 32))
                    {
                        RF_LOG(RF_LOG_TYPE_WARNING, "[%s] WAV sample size (%ibit) not supported, converted to 16bit", filename, rf_wave.sample_size);
                        rf_format_wave(&rf_wave, rf_wave.sample_rate, 16, rf_wave.channels);
                    }

                    // NOTE: Only support up to 2 channels (mono, stereo)
                    if (rf_wave.channels > 2)
                    {
                        rf_format_wave(&rf_wave, rf_wave.sample_rate, rf_wave.sample_size, 2);
                        RF_LOG(RF_LOG_TYPE_WARNING, "[%s] WAV channels number (%i) not supported, converted to 2 channels", filename, rf_wave.channels);
                    }

                    // NOTE: subChunkSize comes in bytes, we need to translate it to number of samples
                    rf_wave.sample_count = (wavData.subChunkSize/(rf_wave.sample_size/8))/rf_wave.channels;

                    RF_LOG(RF_LOG_TYPE_INFO, "[%s] WAV file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");
                }
            }
        }

        fclose(wavFile);
    }

    return rf_wave;
}

// Save wave data as WAV file
RF_INTERNAL int rf_save_wav(rf_wave rf_wave, const char* filename)
{
    int success = 0;
    int dataSize = rf_wave.sample_count * rf_wave.channels * rf_wave.sample_size/8;

    // Basic WAV headers structs
    typedef struct {
        char chunkID[4];
        int chunkSize;
        char format[4];
    } RiffHeader;

    typedef struct {
        char subChunkID[4];
        int subChunkSize;
        short audioFormat;
        short numChannels;
        int sample_rate;
        int byteRate;
        short blockAlign;
        short bitsPerSample;
    } rf_format_wave;

    typedef struct {
        char subChunkID[4];
        int subChunkSize;
    } WaveData;

    FILE *wavFile = fopen(filename, "wb");

    if (wavFile == NULL) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] WAV audio file could not be created", filename);
    else
    {
        RiffHeader riffHeader;
        rf_format_wave rf_format_wave;
        WaveData waveData;

        // Fill structs with data
        riffHeader.chunkID[0] = 'R';
        riffHeader.chunkID[1] = 'I';
        riffHeader.chunkID[2] = 'F';
        riffHeader.chunkID[3] = 'F';
        riffHeader.chunkSize = 44 - 4 + rf_wave.sample_count*rf_wave.sample_size/8;
        riffHeader.format[0] = 'W';
        riffHeader.format[1] = 'A';
        riffHeader.format[2] = 'V';
        riffHeader.format[3] = 'E';

        rf_format_wave.subChunkID[0] = 'f';
        rf_format_wave.subChunkID[1] = 'm';
        rf_format_wave.subChunkID[2] = 't';
        rf_format_wave.subChunkID[3] = ' ';
        rf_format_wave.subChunkSize = 16;
        rf_format_wave.audioFormat = 1;
        rf_format_wave.numChannels = rf_wave.channels;
        rf_format_wave.sample_rate = rf_wave.sample_rate;
        rf_format_wave.byteRate = rf_wave.sample_rate*rf_wave.sample_size/8;
        rf_format_wave.blockAlign = rf_wave.sample_size/8;
        rf_format_wave.bitsPerSample = rf_wave.sample_size;

        waveData.subChunkID[0] = 'd';
        waveData.subChunkID[1] = 'a';
        waveData.subChunkID[2] = 't';
        waveData.subChunkID[3] = 'a';
        waveData.subChunkSize = dataSize;

        success = fwrite(&riffHeader, sizeof(RiffHeader), 1, wavFile);
        success = fwrite(&rf_format_wave, sizeof(rf_format_wave), 1, wavFile);
        success = fwrite(&waveData, sizeof(WaveData), 1, wavFile);

        success = fwrite(rf_wave.data, dataSize, 1, wavFile);

        fclose(wavFile);
    }

    // If all data has been written correctly to file, success = 1
    return success;
}
#endif // defined(RF_SUPPORT_FILEFORMAT_WAV)

#if defined(RF_SUPPORT_FILEFORMAT_OGG)
// Load OGG file into Wave structure
// NOTE: Using stb_vorbis library
RF_INTERNAL rf_wave rf_load_ogg(const char* filename)
{
    rf_wave rf_wave = { 0 };

    stb_vorbis *oggFile = stb_vorbis_open_filename(filename, NULL, NULL);

    if (oggFile == NULL) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] OGG file could not be opened", filename);
    else
    {
        stb_vorbis_info info = stb_vorbis_get_info(oggFile);

        rf_wave.sample_rate = info.sample_rate;
        rf_wave.sample_size = 16;                   // 16 bit per sample (short)
        rf_wave.channels = info.channels;
        rf_wave.sample_count = (unsigned int)stb_vorbis_stream_length_in_samples(oggFile)*info.channels;  // Independent by channel

        float totalSeconds = stb_vorbis_stream_length_in_seconds(oggFile);
        if (totalSeconds > 10) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] Ogg audio length is larger than 10 seconds (%f), that's a big file in memory, consider music streaming", filename, totalSeconds);

        rf_wave.data = (short*) RF_MALLOC(rf_wave.sample_count*rf_wave.channels*sizeof(short));

        // NOTE: Returns the number of samples to process (be careful! we ask for number of shorts!)
        int numSamplesOgg = stb_vorbis_get_samples_short_interleaved(oggFile, info.channels, (short *)rf_wave.data, rf_wave.sample_count*rf_wave.channels);

        RF_LOG(RF_LOG_TYPE_DEBUG, "[%s] Samples obtained: %i", filename, numSamplesOgg);

        RF_LOG(RF_LOG_TYPE_INFO, "[%s] OGG file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");

        stb_vorbis_close(oggFile);
    }

    return rf_wave;
}
#endif // defined(RF_SUPPORT_FILEFORMAT_OGG)

#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
// Load FLAC file into Wave structure
// NOTE: Using dr_flac library
RF_INTERNAL rf_wave rf_load_flac(const char* filename)
{
    rf_wave rf_wave;

    // Decode an entire FLAC file in one go
    uint64_t totalSampleCount;
    rf_wave.data = drflac_open_file_and_read_pcm_frames_s16(filename, &rf_wave.channels, &rf_wave.sample_rate, &totalSampleCount);

    rf_wave.sample_count = (unsigned int)totalSampleCount;
    rf_wave.sample_size = 16;

    // NOTE: Only support up to 2 channels (mono, stereo)
    if (rf_wave.channels > 2) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] FLAC channels number (%i) not supported", filename, rf_wave.channels);

    if (rf_wave.data == NULL) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] FLAC data could not be loaded", filename);
    else RF_LOG(RF_LOG_TYPE_INFO, "[%s] FLAC file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");

    return rf_wave;
}
#endif // defined(RF_SUPPORT_FILEFORMAT_FLAC)

#if defined(RF_SUPPORT_FILEFORMAT_MP3)
// Load MP3 file into Wave structure
// NOTE: Using dr_mp3 library
RF_INTERNAL rf_wave rf_load_mp3(const char* filename)
{
    rf_wave rf_wave = { 0 };

    // Decode an entire MP3 file in one go
    uint64_t totalFrameCount = 0;
    drmp3_config config = { 0 };
    rf_wave.data = drmp3_open_file_and_read_pcm_frames_f32(filename, &config, &totalFrameCount, NULL);

    rf_wave.channels = config.outputChannels;
    rf_wave.sample_rate = config.outputSampleRate;
    rf_wave.sample_count = (int)totalFrameCount*rf_wave.channels;
    rf_wave.sample_size = 32;

    // NOTE: Only support up to 2 channels (mono, stereo)
    if (rf_wave.channels > 2) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] MP3 channels number (%i) not supported", filename, rf_wave.channels);

    if (rf_wave.data == NULL) RF_LOG(RF_LOG_TYPE_WARNING, "[%s] MP3 data could not be loaded", filename);
    else RF_LOG(RF_LOG_TYPE_INFO, "[%s] MP3 file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");

    return rf_wave;
}
#endif // defined(RF_SUPPORT_FILEFORMAT_MP3)

#pragma endregion