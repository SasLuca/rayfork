#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_WIN32_IO
#define JAR_XM_IMPLEMENTATION
#define JAR_MOD_IMPLEMENTATION
#define STB_VORBIS_IMPLEMENTATION
#include "dr_wav.h"
#include "dr_mp3.h"
#include "dr_flac.h"
#include "jar_xm.h"
#include "jar_mod.h"
#include "stb_vorbis.h"

#pragma region audio loading
RF_API rf_audio_player rf_load_static_audio_player_from_wav_to_buffer (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator)
{
    if (!src || src_size <= 0) return (rf_audio_player){0};

    rf_audio_player result = {0};
    drwav wav_decoder      = {0};
    drwav_allocation_callbacks wav_decoder_allocator = {
        .pUserData = &temp_allocator,
        .onMalloc  = rf_drlib_malloc_wrapper,
        .onFree    = rf_drlib_free_wrapper,
        .onRealloc = rf_drlib_realloc_wrapper,
    };

    if (drwav_init_memory(&wav_decoder, src, src_size, &wav_decoder_allocator))
    {
        result.channels     = wav_decoder.channels;
        result.sample_size  = 16; // Note: We are forcing conversion to 16bit
        result.sample_count = wav_decoder.totalPCMFrameCount * wav_decoder.channels;
        result.sample_rate  = wav_decoder.sampleRate;
        result.source_size  = result.sample_count * sizeof(uint16_t);

        if (dst_size >= result.source_size)
        {
            drwav_read_pcm_frames_s16(&wav_decoder, wav_decoder.totalPCMFrameCount, dst);
            result.source = dst;
            result.valid = true;
        }
    }

    drwav_uninit(&wav_decoder);

    return result;
}

RF_API rf_audio_player rf_load_static_audio_player_from_ogg_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator)
{
    if (!src || src_size <= 0 || !dst || dst_size < 0) return (rf_audio_player){0};

    rf_audio_player result = {0};

    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
    {
        stb_vorbis* ogg_decoder = stb_vorbis_open_memory(src, src_size, NULL, NULL);
        if (ogg_decoder)
        {
            stb_vorbis_info ogg_info = stb_vorbis_get_info(ogg_decoder);

            result.sample_rate = ogg_info.sample_rate;
            result.sample_size = 16;
            result.channels = ogg_info.channels;
            result.sample_count = stb_vorbis_stream_length_in_samples(ogg_decoder) * ogg_info.channels;
            result.source_size = result.sample_count * result.channels * sizeof(short);
            result.source = dst;

            if (dst_size >= result.source_size)
            {
                int shorts_processed = stb_vorbis_get_samples_short_interleaved(ogg_decoder, ogg_info.channels, dst, result.sample_count * result.channels);
                result.valid = result.sample_count * result.channels;
            }
        }
    }
    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(RF_NULL_ALLOCATOR);

    return result;
}

RF_API rf_audio_player rf_load_static_audio_player_from_flac_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator)
{
    if (!src || src_size <= 0 || !dst || dst_size < 0) return (rf_audio_player){0};
    rf_audio_player result = {0};

    drflac flac_decoder = {0};
    drflac_allocation_callbacks flac_decoder_allocator = {
        .pUserData = &temp_allocator,
        .onMalloc = rf_drlib_malloc_wrapper,
        .onFree = rf_drlib_free_wrapper,
        .onRealloc = rf_drlib_realloc_wrapper,
    };

    unsigned int channels    = 0;
    unsigned int sample_rate = 0;
    uint64_t sample_count    = 0;
    void* data = drflac_open_memory_and_read_pcm_frames_s16(src, src_size, &channels, &sample_rate, &sample_count, &flac_decoder_allocator);

    if (data)
    {
        result = (rf_audio_player) {
            .source = data,
            .source_size = sample_count * sizeof(uint16_t),
            .channels = channels,
            .sample_rate = sample_rate,
            .sample_count = sample_count,
            .sample_size = 16,
            .valid = true,
        };
    }

    return result;
}

RF_API rf_audio_player rf_load_static_audio_player_from_mp3_to_buffer (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator)
{
    if (!src || src_size <= 0 || !dst || dst_size < 0) return (rf_audio_player){0};
    rf_audio_player result = {0};

    drmp3_config mp3_decoder = {0};
    drmp3_allocation_callbacks mp3_decoder_allocator = {
        .pUserData = &temp_allocator,
        .onMalloc = rf_drlib_malloc_wrapper,
        .onFree = rf_drlib_free_wrapper,
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

    return result;
}

RF_API rf_audio_data rf_load_audio_from_xm  (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator)
{

}

RF_API rf_audio_data rf_load_audio_from_mod (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);

RF_API rf_audio_format rf_audio_format_from_filename_extension(const char* text);
RF_API rf_audio_format rf_audio_format_from_filename_extension_string(const char* string, int string_len);

RF_API rf_audio_data rf_load_audio_from_file_data_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_audio_data rf_load_audio_from_file_data(const void* src, int src_size, rf_allocator allocator);
RF_API rf_audio_data rf_load_audio_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);

RF_API rf_audio_player rf_static_audio_player_from_memory(const void* src, int src_size, rf_allocator allocator);
RF_API rf_audio_player rf_static_audio_player_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_audio_player rf_static_audio_player_from_audio_data(rf_audio_data, rf_allocator allocator);

RF_API rf_audio_player rf_streamed_audio_player_from_memory(const void* src, int src_size, rf_allocator allocator);
RF_API rf_audio_player rf_streamed_audio_player_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_audio_player rf_streamed_audio_player_from_audio_data(rf_audio_data, rf_allocator allocator);
#pragma endregion