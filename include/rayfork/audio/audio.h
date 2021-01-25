#ifndef RAYFORK_AUDIO_H
#define RAYFORK_AUDIO_H

#include "rayfork-core.h"
#include "rayfork-arr.h"
#include "miniaudio.h"

/*
 This buffer size is defined by number of samples, independent of sample size and channels number
 After some math, considering a sampleRate of 48000, a buffer refill rate of 1/60 seconds and a
 standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
 In case of music-stalls, just increase this number.
*/
#define rf_default_audio_buffer_size        (4096)
#define rf_default_audio_device_format      ma_format_f32
#define rf_default_audio_device_sample_rate (44100)
#define rf_max_audio_buffer_pool_channels   (16)
#define rf_default_audio_device_channels    (2)
#define rf_default_audio_device_id          (0)

typedef enum rf_audio_format
{
    rf_audio_format_unknown = 0,
    rf_audio_format_wav,
    rf_audio_format_ogg,
    rf_audio_format_flac,
    rf_audio_format_mp3,
    rf_audio_format_xm,
    rf_audio_format_mod,
} rf_audio_format;

typedef enum rf_audio_buffer_type
{
    rf_audio_buffer_type_streamed,
    rf_audio_buffer_type_static,
} rf_audio_buffer_type;

typedef struct rf_audio_buffer
{
    ma_data_converter converter;

    float volume;
    float pitch;

    rf_bool playing;
    rf_bool paused;
    rf_bool looping;
    rf_audio_buffer_type type;

    int size_in_frames;
    int frame_cursor_pos;
    int total_frames_processed;
    rf_bool is_sub_buffer_processed[2];

    char* data;
    rf_bool valid;
} rf_audio_buffer;

typedef rf_int rf_audio_buffer_handle;

typedef struct rf_audio_context
{
    ma_context ma_ctx;
    ma_device  device;
    ma_mutex   lock;
    rf_bool    valid;

    rf_arr(rf_audio_buffer) buffers;
    int default_buffer_size;

    rf_audio_buffer_handle multichannel_pool[rf_max_audio_buffer_pool_channels];
    int multichannel_pool_ct;
    int channels[rf_max_audio_buffer_pool_channels];
} rf_audio_context;

typedef struct rf_wave
{
    int sample_count; // Total number of samples
    int sample_rate;  // Frequency (samples per second)
    int sample_size;  // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    int channels;     // Number of channels (1-mono, 2-stereo)
    void* data;       // Buffer data pointer
} rf_wave;

typedef struct rf_audio_stream
{
    int sample_rate; // Frequency (samples per second)
    int sample_size; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    int channels;    // Number of channels (1-mono, 2-stereo)

    rf_audio_buffer_handle buffer; // Pointer to internal data used by the audio system
} rf_audio_stream;

typedef struct rf_sound
{
    int sample_count;       // Total number of samples
    rf_audio_stream stream; // Audio stream
} rf_sound;

typedef struct rf_music
{
    rf_audio_format ctx_type; // Type of music context (audio filetype)
    void*           ctx_data; // Audio context data, depends on type

    bool looping;             // rf_music looping enable
    int sample_count;         // Total number of samples

    rf_audio_stream stream;   // Audio stream
} rf_music;

#pragma region context
rf_extern void rf_init_audio_device(void);         // Initialize audio device and context
rf_extern void rf_close_audio_device(void);        // Close the audio device and context
rf_extern bool rf_is_audio_device_ready(void);     // Check if audio device has been initialized successfully
rf_extern void rf_set_master_volume(float volume); // Set master volume (listener)
#pragma endregion

#pragma region wave
rf_wave rf_load_wave_from_data(const char* src, rf_int src, rf_audio_format format);
rf_wave rf_load_wave_from_file(const char* file_name);

void rf_unload_wave(Wave wave);
void rf_wave_format(rf_wave* wave, int sample_rate, int sample_size, int channels); // Convert wave data to desired format
void rf_wave_crop(rf_wave* wave, int init_sample, int final_sample);                // Crop a wave to defined samples range
rf_wave rf_wave_copy(rf_wave wave);                                                 // Copy a wave to a new wave
float* rf_get_wave_data(rf_wave wave);                                              // Get samples data from wave as a floats array
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

#endif // RAYFORK_AUDIO_H