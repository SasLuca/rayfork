/**********************************************************************************************
*
*   rayfork-audio - A fork of the amazing raudio
*
*   FEATURES:
*       - Manage audio device (init/close)
*       - Load and unload audio files
*       - Format wave data (sample rate, size, channels)
*       - Play/Stop/Pause/Resume loaded audio
*       - Manage mixing channels
*       - Manage raw audio context
*
*   DIFFERENCES FROM RAUDIO:
*       - No global variables
*       - Async loading
*       - More control over memory
*       - More control over IO
*
*   DEPENDENCIES:
*       miniaudio.h  - Audio device management lib (https://github.com/dr-soft/miniaudio)
*       stb_vorbis.h - Ogg audio files loading (http://www.nothings.org/stb_vorbis/)
*       dr_mp3.h     - MP3 audio file loading (https://github.com/mackron/dr_libs)
*       dr_flac.h    - FLAC audio file loading (https://github.com/mackron/dr_libs)
*       jar_xm.h     - XM module file loading
*       jar_mod.h    - MOD audio file loading
**********************************************************************************************/

#ifndef RF_AUDIO_H
#define RF_AUDIO_H

#include "rayfork_base.h"

#pragma region macros and constants

// NOTE: Music buffer size is defined by number of samples, independent of sample size and channels number
// After some math, considering a sample_rate of 48000, a buffer refill rate of 1/60 seconds and a
// standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough.
// In case of music-stalls, just increase this number
#ifndef RF_AUDIO_BUFFER_SIZE
    #define RF_AUDIO_BUFFER_SIZE (4096) // PCM data samples (i.e. 16bit, Mono: 8Kb)
#endif

#ifndef RF_DEVICE_SAMPLE_RATE
    #define RF_DEVICE_SAMPLE_RATE (44100)
#endif

#ifndef RF_MAX_AUDIO_BUFFER_POOL_CHANNELS
    #define RF_MAX_AUDIO_BUFFER_POOL_CHANNELS (16)
#endif

#define RF_DEVICE_FORMAT ma_format_f32
#define RF_DEVICE_CHANNELS (2)

#pragma endregion

#pragma region structs

// Music context type
// NOTE: Depends on data structure provided by the library
// in charge of reading the different file types
typedef enum rf_audio_format
{
    RF_AUDIO_FORMAT_WAVE = 0,
    RF_AUDIO_FORMAT_OGG,
    RF_AUDIO_FORMAT_FLAC,
    RF_AUDIO_FORMAT_MP3,
    RF_AUDIO_FORMAT_XM,
    RF_AUDIO_FORMAT_MOD,
} rf_audio_format;

// Wave type, defines audio wave data
typedef struct rf_wave rf_wave;
struct rf_wave
{
    void* data;                // Buffer data pointer
    unsigned int channels;     // Number of channels (1-mono, 2-stereo)
    unsigned int sample_count; // Total number of samples
    unsigned int sample_rate;  // Frequency (samples per second)
    unsigned int sample_size;  // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    bool valid;
};

typedef struct rf_audio_buffer rf_audio_buffer;

// Audio stream type
// NOTE: Useful to create custom audio streams not bound to a specific file
typedef struct rf_audio_stream rf_audio_stream;
struct rf_audio_stream
{
    unsigned int sample_rate; // Frequency (samples per second)
    unsigned int sample_size; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;    // Number of channels (1-mono, 2-stereo)

    rf_audio_buffer* buffer;  // Pointer to internal data used by the audio system
};

// Sound source type
typedef struct rf_short_audio rf_short_audio;
struct rf_short_audio
{
    unsigned int    sample_count; // Total number of samples
    rf_audio_stream stream;       // Audio stream
};

// Music stream type (audio file streaming from memory)
// NOTE: Anything longer than ~10 seconds should be streamed
typedef struct rf_long_audio rf_long_audio;
struct rf_long_audio
{
    int   ctx_type; // Type of music context (audio filetype)
    void* ctx_data; // Audio context data, depends on type

    unsigned int sample_count; // Total number of samples
    unsigned int loop_count;   // Loops count (times music will play), 0 means infinite loop
    rf_audio_stream stream;    // Audio stream
};

// Audio device management functions
typedef struct rf_audio_context rf_audio_context; // Forward declaration

#pragma endregion

#pragma region core

RF_API int  rf_audio_context_size(void);
RF_API void rf_audio_init(rf_audio_context* audio_ctx);            // Initialize audio device and context
RF_API void rf_audio_set_context_ptr(rf_audio_context* audio_ctx); // Sets the global context ptr
RF_API void rf_audio_cleanup(void);                                // Close the audio device and context
RF_API bool rf_is_audio_device_ready(void);                        // Check if audio device has been initialized successfully
RF_API void rf_set_master_volume(float volume);                    // Set master volume (listener)

// Wave/Sound loading/unloading functions

#pragma endregion

#pragma region wave
RF_API rf_wave rf_load_wave_from_file(const char* filename); // Load wave data from file
RF_API int rf_wave_size(rf_wave wave);
RF_API void rf_unload_wave(rf_wave wave, rf_allocator allocator); // Unload wave data
RF_API void rf_export_wave_as_code(rf_wave wave, const char* filename); // Export wave sample data to code (.h)
RF_API void rf_format_wave(rf_wave* wave, int sample_rate, int sample_size, int channels); // Convert wave data to desired format
RF_API rf_wave rf_copy_wave_to_buffer(rf_wave wave, void* dst, int dst_size);
RF_API rf_wave rf_copy_wave(rf_wave, rf_allocator allocator);
RF_API void rf_crop_wave(rf_wave* rf_wave, int init_sample, int final_sample); // Crop a wave to defined samples range
RF_API float* rf_get_wave_data(rf_wave rf_wave); // Get samples data from wave as a floats array

#pragma endregion

#pragma region audio stream

RF_API rf_audio_stream rf_create_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels); // Init audio stream (to stream raw audio pcm data)
RF_API void rf_update_audio_stream(rf_audio_stream stream, const void* data, int samplesCount);                           // Update audio stream buffers with data
RF_API void rf_close_audio_stream(rf_audio_stream stream);                    // Close audio stream and free memory
RF_API bool rf_is_audio_stream_processed(rf_audio_stream stream);             // Check if any audio stream buffers requires refill
RF_API void rf_play_audio_stream(rf_audio_stream stream);                     // Play audio stream
RF_API void rf_pause_audio_stream(rf_audio_stream stream);                    // Pause audio stream
RF_API void rf_resume_audio_stream(rf_audio_stream stream);                   // Resume audio stream
RF_API bool rf_is_audio_stream_playing(rf_audio_stream stream);               // Check if audio stream is playing
RF_API void rf_stop_audio_stream(rf_audio_stream stream);                     // Stop audio stream
RF_API void rf_set_audio_stream_volume(rf_audio_stream stream, float volume); // Set volume for audio stream (1.0 is max level)
RF_API void rf_set_audio_stream_pitch(rf_audio_stream stream, float pitch);   // Set pitch for audio stream (1.0 is base level)

#pragma endregion

#pragma region short audio stream

RF_API rf_short_audio rf_load_short_audio_from_file(const char* filename);                                 // Load sound from file
RF_API rf_short_audio rf_load_short_audio_from_wave(rf_wave wave);                                         // Load sound from wave data
RF_API void rf_update_short_audio(rf_short_audio short_audio, const void* data, int samples_count); // Update sound buffer with new data
RF_API void rf_unload_sound(rf_short_audio short_audio);                                                   // Unload sound

RF_API void rf_play_short_audio(rf_short_audio short_audio);                     // Play a sound
RF_API void rf_stop_short_audio(rf_short_audio short_audio);                     // Stop playing a sound
RF_API void rf_pause_short_audio(rf_short_audio short_audio);                    // Pause a sound
RF_API void rf_resume_short_audio(rf_short_audio short_audio);                   // Resume a paused sound
RF_API void rf_play_short_audio_multi(rf_short_audio short_audio);               // Play a sound (using multichannel buffer pool)
RF_API void rf_stop_short_audio_multi(void);                                                   // Stop any sound playing (using multichannel buffer pool)
RF_API int rf_get_short_audios_playing(void);                                                  // Get number of sounds playing in the multichannel
RF_API bool rf_is_short_audio_playing(rf_short_audio short_audio);               // Check if a sound is currently playing
RF_API void rf_set_short_audio_volume(rf_short_audio short_audio, float volume); // Set volume for a sound (1.0 is max level)
RF_API void rf_set_short_audio_pitch(rf_short_audio short_audio, float pitch);   // Set pitch for a sound (1.0 is base level)

#pragma endregion

#pragma region long audio stream

RF_API rf_long_audio rf_load_long_audio(const char* filename);                        // Load music stream from file
RF_API void rf_unload_long_audio(rf_long_audio long_audio);                    // Unload music stream
RF_API void rf_play_long_audio(rf_long_audio long_audio);                      // Start music playing
RF_API void rf_update_long_audio(rf_long_audio long_audio);                    // Updates buffers for music streaming
RF_API void rf_stop_long_audio(rf_long_audio long_audio);                      // Stop music playing
RF_API void rf_pause_long_audio(rf_long_audio long_audio);                     // Pause music playing
RF_API void rf_resume_long_audio(rf_long_audio long_audio);                    // Resume playing paused music
RF_API bool rf_is_long_audio_playing(rf_long_audio long_audio);                // Check if music is playing
RF_API void rf_set_long_audio_volume(rf_long_audio long_audio, float volume);  // Set volume for music (1.0 is max level)
RF_API void rf_set_long_audio_pitch(rf_long_audio long_audio, float pitch);    // Set pitch for a music (1.0 is base level)
RF_API void rf_set_long_audio_loop_count(rf_long_audio long_audio, int count); // Set music loop count (loop repeats)
RF_API float rf_get_long_audio_time_length(rf_long_audio long_audio);          // Get music time length (in seconds)
RF_API float rf_get_long_audio_time_played(rf_long_audio long_audio);          // Get current music time played (in seconds)

// AudioStream management functions

#pragma endregion

#endif // RF_AUDIO_H