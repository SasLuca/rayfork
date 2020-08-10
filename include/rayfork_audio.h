#ifndef RAYFORK_AUDIO_H
#define RAYFORK_AUDIO_H

#include "rayfork_error.h"
#include "rayfork_allocator.h"
#include "rayfork_io.h"
#include "rayfork_logger.h"

/*
 This buffer size is defined by number of samples, independent of sample size and channels number
 After some math, considering a sampleRate of 48000, a buffer refill rate of 1/60 seconds and a
 standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
 In case of music-stalls, just increase this number.
*/
#define RF_DEFAULT_AUDIO_BUFFER_SIZE     (4096)
#define RF_DEFAULT_AUDIO_DEVICE_FORMAT   uint16_t
#define RF_DEFAULT_DEVICE_SAMPLE_RATE    (44100)
#define RF_DEFAULT_AUDIO_DEVICE_CHANNELS (RF_STEREO)

typedef enum rf_audio_format
{
    RF_AUDIO_FORMAT_UNKNOWN = 0,
    RF_AUDIO_FORMAT_WAV,
    RF_AUDIO_FORMAT_OGG,
    RF_AUDIO_FORMAT_FLAC,
    RF_AUDIO_FORMAT_MP3,
    RF_AUDIO_FORMAT_XM,
    RF_AUDIO_FORMAT_MOD,
} rf_audio_format;

typedef enum rf_audio_channels
{
    RF_MONO   = 1,
    RF_STEREO = 2,
} rf_audio_channels;

typedef struct rf_audio_buffer rf_audio_buffer;

typedef struct rf_decoded_audio_data
{
    void* data;
    int data_size;

    int size_in_frames;
    int sample_count; // Total number of samples
    int sample_rate;  // Frequency (samples per second)
    int sample_size;  // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    enum rf_audio_channels channels;
} rf_decoded_audio_data;

typedef struct rf_compressed_audio_data
{
    rf_audio_format format;
    int size_in_frames;
    int sample_count; // Total number of samples
    int sample_rate;  // Frequency (samples per second)
    int sample_size;  // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    enum rf_audio_channels channels;
} rf_compressed_audio_data;

typedef struct rf_static_audio_player
{
    rf_decoded_audio_data* source;
    float volume;
    bool looping;
    bool valid;
} rf_audio_player;

typedef struct rf_streaming_audio_player
{
    rf_
} rf_stream_audio_player;

typedef struct rf_audio_device
{
    int id;
} rf_audio_device;

#pragma region audio device
RF_API rf_audio_device rf_audio_device_count();
RF_API rf_audio_device rf_default_audio_device();
RF_API bool rf_start_audio_device(rf_audio_device);
RF_API void rf_close_audio_device(rf_audio_device);
RF_API bool rf_is_audio_device_ready(rf_audio_device);

RF_API void rf_set_master_volume(float);
#pragma endregion

#pragma region audio loading
RF_API rf_audio_format rf_audio_format_from_filename_extension(const char* text);
RF_API rf_audio_format rf_audio_format_from_filename_extension_string(const char* string, int string_len);

RF_API rf_audio_player rf_load_static_audio_player_from_wav_to_buffer (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_ogg_to_buffer (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_flac_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_mp3_to_buffer (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_xm_to_buffer  (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_mod_to_buffer (const void* src, int src_size, void* dst, int dst_size, rf_allocator temp_allocator);

RF_API rf_audio_player rf_load_static_audio_player_from_wav (const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_ogg (const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_flac(const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_mp3 (const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_xm  (const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_audio_player rf_load_static_audio_player_from_mod (const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_audio_player rf_load_static_audio_player_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);

RF_API rf_audio_player rf_streamed_audio_player_from_memory(const void* src, int src_size, rf_allocator allocator);
RF_API rf_audio_player rf_streamed_audio_player_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region audio controls
RF_API void rf_audio_play(rf_audio_player* audio);
RF_API void rf_audio_stop(rf_audio_player* audio);
RF_API void rf_audio_pause(rf_audio_player* audio);
RF_API void rf_audio_resume(rf_audio_player* audio);
RF_API void rf_audio_update(rf_audio_player* audio);

RF_API void rf_audio_set_volume(rf_audio_player* audio);
RF_API void rf_audio_set_pitch(rf_audio_player* audio);

RF_API float rf_audio_time_len(rf_audio_player* audio);
RF_API float rf_audio_time_played(rf_audio_player* audio);
RF_API bool  rf_audio_is_playing(rf_audio_player* audio);
RF_API int   rf_audio_volume(rf_audio_player* audio);
RF_API int   rf_audio_pitch(rf_audio_player* audio);
#pragma endregion

#endif // RAYFORK_AUDIO_H