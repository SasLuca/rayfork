/**********************************************************************************************
*
*   rayfork_audio - A fork of the amazing raudio
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

//region interface

#ifndef RF_AUDIO_H
#define RF_AUDIO_H

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Allow custom memory allocators
#ifndef RF_MALLOC
#define RF_MALLOC(sz) malloc(sz)
#endif

#ifndef RF_FREE
#define RF_FREE(p) free(p)
#endif

#define RF_LOG_TRACE 0
#define RF_LOG_DEBUG 1
#define RF_LOG_INFO 2
#define RF_LOG_WARNING 3
#define RF_LOG_ERROR 4
#define RF_LOG_FATAL 5

#ifndef RF_LOG
#define RF_LOG(log_type, msg, ...)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#include <stdbool.h>

// Wave type, defines audio wave data
typedef struct rf_wave rf_wave;
struct rf_wave
{
    unsigned int sample_count; // Total number of samples
    unsigned int sample_rate;  // Frequency (samples per second)
    unsigned int sample_size;  // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;     // Number of channels (1-mono, 2-stereo)
    void* data;                // Buffer data pointer
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
typedef struct rf_short_audio_stream rf_short_audio_stream;
struct rf_short_audio_stream
{
    unsigned int    sample_count; // Total number of samples
    rf_audio_stream stream;       // Audio stream
};

// Music stream type (audio file streaming from memory)
// NOTE: Anything longer than ~10 seconds should be streamed
typedef struct rf_long_audio_stream rf_long_audio_stream;
struct rf_long_audio_stream
{
    int   ctx_type; // Type of music context (audio filetype)
    void* ctx_data; // Audio context data, depends on type

    unsigned int sample_count; // Total number of samples
    unsigned int loop_count;   // Loops count (times music will play), 0 means infinite loop

    rf_audio_stream stream;    // Audio stream
};

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

// Audio device management functions
typedef struct rf_audio_context rf_audio_context; // Forward declaration

extern void rf_audio_init(rf_audio_context* audio_ctx); // Initialize audio device and context
extern void rf_audio_set_context_ptr(rf_audio_context* audio_ctx); //Sets the global context ptr
extern void rf_audio_cleanup(void);             // Close the audio device and context
extern bool rf_is_audio_device_ready(void);     // Check if audio device has been initialized successfully
extern void rf_set_master_volume(float volume); // Set master volume (listener)

// Wave/Sound loading/unloading functions
extern rf_wave rf_load_wave_from_file(const char* filename);                             // Load wave data from file
extern rf_short_audio_stream rf_load_short_audio_stream_from_file(const char* filename); // Load sound from file
extern rf_short_audio_stream rf_load_short_audio_stream_from_wave(rf_wave wave);         // Load sound from wave data
extern void rf_update_short_audio_stream(rf_short_audio_stream short_audio_stream, const void* data, int samples_count); // Update sound buffer with new data
extern void rf_unload_wave(rf_wave wave);                                                // Unload wave data
extern void rf_unload_sound(rf_short_audio_stream short_audio_stream);                   // Unload sound
extern void rf_export_wave(rf_wave wave, const char* filename);                          // Export wave data to file
extern void rf_export_wave_as_code(rf_wave wave, const char* filename);                  // Export wave sample data to code (.h)

// Wave/Sound management functions
extern void rf_play_short_audio_stream(rf_short_audio_stream short_audio_stream);            // Play a sound
extern void rf_stop_short_audio_stream(rf_short_audio_stream short_audio_stream);            // Stop playing a sound
extern void rf_pause_short_audio_stream(rf_short_audio_stream short_audio_stream);           // Pause a sound
extern void rf_resume_short_audio_stream(rf_short_audio_stream short_audio_stream);          // Resume a paused sound
extern void rf_play_short_audio_stream_multi(rf_short_audio_stream short_audio_stream);      // Play a sound (using multichannel buffer pool)
extern void rf_stop_short_audio_stream_multi(void);                                             // Stop any sound playing (using multichannel buffer pool)
extern int rf_get_short_audio_streams_playing(void);                                            // Get number of sounds playing in the multichannel
extern bool rf_is_short_audio_stream_playing(rf_short_audio_stream short_audio_stream);      // Check if a sound is currently playing
extern void rf_set_short_audio_stream_volume(rf_short_audio_stream short_audio_stream, float volume); // Set volume for a sound (1.0 is max level)
extern void rf_set_short_audio_stream_pitch(rf_short_audio_stream short_audio_stream, float pitch);   // Set pitch for a sound (1.0 is base level)
extern void rf_format_wave(rf_wave* wave, int sample_rate, int sample_size, int channels);  // Convert wave data to desired format
extern rf_wave rf_copy_wave(rf_wave wave);                                                  // Copy a wave to a new wave
extern void rf_crop_wave(rf_wave* rf_wave, int init_sample, int final_sample);                   // Crop a wave to defined samples range
extern float* rf_get_wave_data(rf_wave rf_wave);                                               // Get samples data from wave as a floats array

// Music management functions
extern rf_long_audio_stream rf_load_long_audio_stream(const char* filename);                   // Load music stream from file
extern void rf_unload_long_audio_stream(rf_long_audio_stream long_audio_stream);            // Unload music stream
extern void rf_play_long_audio_stream(rf_long_audio_stream long_audio_stream);              // Start music playing
extern void rf_update_long_audio_stream(rf_long_audio_stream long_audio_stream);            // Updates buffers for music streaming
extern void rf_stop_long_audio_stream(rf_long_audio_stream long_audio_stream);              // Stop music playing
extern void rf_pause_long_audio_stream(rf_long_audio_stream long_audio_stream);             // Pause music playing
extern void rf_resume_long_audio_stream(rf_long_audio_stream long_audio_stream);            // Resume playing paused music
extern bool rf_is_long_audio_stream_playing(rf_long_audio_stream long_audio_stream);                // Check if music is playing
extern void rf_set_long_audio_stream_volume(rf_long_audio_stream long_audio_stream, float volume);  // Set volume for music (1.0 is max level)
extern void rf_set_long_audio_stream_pitch(rf_long_audio_stream long_audio_stream, float pitch);    // Set pitch for a music (1.0 is base level)
extern void rf_set_long_audio_stream_loop_count(rf_long_audio_stream long_audio_stream, int count); // Set music loop count (loop repeats)
extern float rf_get_long_audio_stream_time_length(rf_long_audio_stream long_audio_stream);          // Get music time length (in seconds)
extern float rf_get_long_audio_stream_time_played(rf_long_audio_stream long_audio_stream);          // Get current music time played (in seconds)

// AudioStream management functions
extern rf_audio_stream rf_create_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels); // Init audio stream (to stream raw audio pcm data)
extern void rf_update_audio_stream(rf_audio_stream stream, const void* data, int samplesCount);                           // Update audio stream buffers with data
extern void rf_close_audio_stream(rf_audio_stream stream);                    // Close audio stream and free memory
extern bool rf_is_audio_stream_processed(rf_audio_stream stream);             // Check if any audio stream buffers requires refill
extern void rf_play_audio_stream(rf_audio_stream stream);                     // Play audio stream
extern void rf_pause_audio_stream(rf_audio_stream stream);                    // Pause audio stream
extern void rf_resume_audio_stream(rf_audio_stream stream);                   // Resume audio stream
extern bool rf_is_audio_stream_playing(rf_audio_stream stream);               // Check if audio stream is playing
extern void rf_stop_audio_stream(rf_audio_stream stream);                     // Stop audio stream
extern void rf_set_audio_stream_volume(rf_audio_stream stream, float volume); // Set volume for audio stream (1.0 is max level)
extern void rf_set_audio_stream_pitch(rf_audio_stream stream, float pitch);   // Set pitch for audio stream (1.0 is base level)

#ifdef __cplusplus
}
#endif

#endif // RF_AUDIO_H
//endregion

//region implementation
#ifdef RF_AUDIO_IMPL
#include <stdarg.h> // Required for: va_list, va_start(), vfprintf(), va_end()

#define MA_NO_JACK
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h" // miniaudio library

#include <stdlib.h> // Required for: malloc(), free()
#include <string.h> // Required for: strcmp(), strncmp()
#include <stdio.h>  // Required for: FILE, fopen(), fclose(), fread()
#include <assert.h>

#define _rf_is_file_extension(filename, ext) (strrchr(filename, '.') != NULL && strcmp(strrchr(filename, '.'), ext))

#ifndef RF_ASSERT
#define RF_ASSERT(condition) assert(condition);
#endif

#if defined(RF_SUPPORT_FILEFORMAT_OGG)
#define STB_VORBIS_IMPLEMENTATION
    #include "stb_vorbis.h"    // OGG loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_XM)
#define JAR_XM_IMPLEMENTATION
    #include "jar_xm.h"        // XM loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_MOD)
#define JAR_MOD_IMPLEMENTATION
    #include "jar_mod.h"       // MOD loading functions
#endif

#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
#define DR_FLAC_IMPLEMENTATION
    #define DR_FLAC_NO_WIN32_IO
    #include "dr_flac.h"       // FLAC loading functions
#endif

#if defined(RF_SUPPORT_FILEFORMAT_MP3)
#define DR_MP3_IMPLEMENTATION
    #include "dr_mp3.h"        // MP3 loading functions
#endif

#if defined(_MSC_VER)
#undef bool
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// NOTE: Music buffer size is defined by number of samples, independent of sample size and channels number
// After some math, considering a sample_rate of 48000, a buffer refill rate of 1/60 seconds and a
// standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
// In case of music-stalls, just increase this number
#define RF_AUDIO_BUFFER_SIZE 4096 // PCM data samples (i.e. 16bit, Mono: 8Kb)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Music context type
// NOTE: Depends on data structure provided by the library
// in charge of reading the different file types
typedef enum rf_audio_context_type
{
    rf_audio_context_wave = 0,
    rf_audio_context_ogg,
    rf_audio_context_flac,
    rf_audio_context_mp3,
    rf_audio_context_xm,
    rf_audio_context_mod
} rf_audio_context_type;

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined(RF_SUPPORT_FILEFORMAT_WAV)
static rf_wave rf_load_wav(const char* filename);  // Load WAV file
static int rf_save_wav(rf_wave rf_wave, const char* filename); // Save wave data as WAV file
#endif
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
static rf_wave rf_load_ogg(const char* filename);  // Load OGG file
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
static rf_wave rf_load_flac(const char* filename); // Load FLAC file
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
static rf_wave rf_load_mp3(const char* filename);  // Load MP3 file
#endif

//----------------------------------------------------------------------------------
// AudioBuffer Functionality
//----------------------------------------------------------------------------------
#define RF_DEVICE_FORMAT       ma_format_f32
#define RF_DEVICE_CHANNELS     2
#define RF_DEVICE_SAMPLE_RATE  44100

#define RF_MAX_AUDIO_BUFFER_POOL_CHANNELS 16

typedef enum rf_audio_buffer_type
{
    rf_audio_buffer_static = 0,
    rf_audio_buffer_stream
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
    int usage;              // Audio buffer usage mode: STATIC or STREAM

    bool is_sub_buffer_processed[2];       // SubBuffer processed (virtual double buffer)
    unsigned int frame_cursor_pos;        // Frame cursor position
    unsigned int buffer_size_in_frames;    // Total buffer size in frames
    unsigned int total_frames_processed;  // Total frames processed in this buffer (required for play timming)

    unsigned char* buffer;              // Data buffer, on music stream keeps filling

    rf_audio_buffer* next;     // Next audio buffer on the list
    rf_audio_buffer* prev;     // Previous audio buffer on the list
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

static rf_audio_context* _rf_global_audio_context = NULL;

// miniaudio functions declaration
static void _rf_ma_callback__on_log(ma_context* pContext, ma_device*  pDevice, ma_uint32 logLevel, const char* message);
static void _rf_ma_callback__on_send_audio_data_to_device(ma_device*  pDevice, void* pFramesOut, const void* pFramesInput, ma_uint32 frameCount);
static ma_uint32 _rf_ma_callback__on_audio_buffer_dsp_read(ma_pcm_converter* pDSP, void* pFramesOut, ma_uint32 frameCount, void* pUserData);
static void _rf_ma_callback__mix_audio_frames(float* framesOut, const float* framesIn, ma_uint32 frameCount, float localVolume);

// AudioBuffer management functions declaration
// NOTE: Those functions are not exposed by raylib... for the moment
static rf_audio_buffer* _rf_init_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 buffer_size_in_frames, int usage);
static void _rf_close_audio_buffer(rf_audio_buffer* buffer);
static bool _rf_is_audio_buffer_playing(rf_audio_buffer* buffer);
static void _rf_play_audio_buffer(rf_audio_buffer* buffer);
static void _rf_stop_audio_buffer(rf_audio_buffer* buffer);
static void _rf_pause_audio_buffer(rf_audio_buffer* buffer);
static void _rf_resume_audio_buffer(rf_audio_buffer* buffer);
static void _rf_set_audio_buffer_volume(rf_audio_buffer* buffer, float volume);
static void _rf_set_audio_buffer_pitch(rf_audio_buffer* buffer, float pitch);
static void _rf_track_audio_buffer(rf_audio_buffer* buffer);
static void _rf_untrack_audio_buffer(rf_audio_buffer* buffer);

//----------------------------------------------------------------------------------
// miniaudio functions definitions
//----------------------------------------------------------------------------------

// Log callback function
static void _rf_ma_callback__on_log(ma_context* pContext, ma_device*  pDevice, ma_uint32 logLevel, const char* message)
{
    (void)pContext;
    (void)pDevice;

    RF_LOG(RF_LOG_ERROR, message);   // All log messages from miniaudio are errors
}

// Sending audio data to device callback function
// NOTE: All the mixing takes place here
static void _rf_ma_callback__on_send_audio_data_to_device(ma_device*  pDevice, void* pFramesOut, const void* pFramesInput, ma_uint32 frameCount)
{
    (void)pDevice;

    // Mixing is basically just an accumulation, we need to initialize the output buffer to 0
    memset(pFramesOut, 0, frameCount*pDevice->playback.channels*ma_get_bytes_per_sample(pDevice->playback.format));

    // Using a mutex here for thread-safety which makes things not real-time
    // This is unlikely to be necessary for this project, but may want to consider how you might want to avoid this
    ma_mutex_lock(&_rf_global_audio_context->audio_lock);
    {
        for (rf_audio_buffer* rf_audio_buffer = _rf_global_audio_context->first_audio_buffer; rf_audio_buffer != NULL; rf_audio_buffer = rf_audio_buffer->next)
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
                        float* framesOut = (float* )pFramesOut + (framesRead*_rf_global_audio_context->device.playback.channels);
                        float* framesIn  = tempBuffer;

                        _rf_ma_callback__mix_audio_frames(framesOut, framesIn, framesJustRead, rf_audio_buffer->volume);

                        framesToRead -= framesJustRead;
                        framesRead += framesJustRead;
                    }

                    // If we weren't able to read all the frames we requested, break
                    if (framesJustRead < framesToReadRightNow)
                    {
                        if (!rf_audio_buffer->looping)
                        {
                            _rf_stop_audio_buffer(rf_audio_buffer);
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

    ma_mutex_unlock(&_rf_global_audio_context->audio_lock);
}

// DSP read from audio buffer callback function
static ma_uint32 _rf_ma_callback__on_audio_buffer_dsp_read(ma_pcm_converter* pDSP, void* pFramesOut, ma_uint32 frameCount, void* pUserData)
{
    rf_audio_buffer* audio_buffer = (rf_audio_buffer*) pUserData;

    ma_uint32 sub_buffer_size_in_frames = (audio_buffer->buffer_size_in_frames > 1) ? audio_buffer->buffer_size_in_frames / 2 : audio_buffer->buffer_size_in_frames;
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
        //  - For static buffers, we simply fill as much data as we can
        //  - For streaming buffers we only fill the halves of the buffer that are processed
        //    Unprocessed halves must keep their audio data in-tact
        if (audio_buffer->usage == rf_audio_buffer_static)
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
        if (audio_buffer->usage == rf_audio_buffer_static)
        {
            frames_remaining_in_output_buffer = audio_buffer->buffer_size_in_frames - audio_buffer->frame_cursor_pos;
        }
        else
        {
            ma_uint32 first_frame_index_of_this_sub_buffer = sub_buffer_size_in_frames * current_sub_buffer_index;
            frames_remaining_in_output_buffer = sub_buffer_size_in_frames - (audio_buffer->frame_cursor_pos - first_frame_index_of_this_sub_buffer);
        }

        ma_uint32 frames_to_read = total_frames_remaining;
        if (frames_to_read > frames_remaining_in_output_buffer) frames_to_read = frames_remaining_in_output_buffer;

        memcpy((unsigned char* )pFramesOut + (frames_read * frame_size_in_bytes), audio_buffer->buffer + (audio_buffer->frame_cursor_pos * frame_size_in_bytes), frames_to_read * frame_size_in_bytes);
        audio_buffer->frame_cursor_pos = (audio_buffer->frame_cursor_pos + frames_to_read) % audio_buffer->buffer_size_in_frames;
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
                _rf_stop_audio_buffer(audio_buffer);
                break;
            }
        }
    }

    // Zero-fill excess
    ma_uint32 total_frames_remaining = (frameCount - frames_read);
    if (total_frames_remaining > 0)
    {
        memset((unsigned char* )pFramesOut + (frames_read * frame_size_in_bytes), 0, total_frames_remaining * frame_size_in_bytes);

        // For static buffers we can fill the remaining frames with silence for safety, but we don't want
        // to report those frames as "read". The reason for this is that the caller uses the return value
        // to know whether or not a non-looping sound has finished playback.
        if (audio_buffer->usage != rf_audio_buffer_static) frames_read += total_frames_remaining;
    }

    return frames_read;
}

// This is the main mixing function. Mixing is pretty simple in this project - it's just an accumulation.
// NOTE: framesOut is both an input and an output. It will be initially filled with zeros outside of this function.
static void _rf_ma_callback__mix_audio_frames(float* framesOut, const float* framesIn, ma_uint32 frameCount, float localVolume)
{
    for (ma_uint32 iFrame = 0; iFrame < frameCount; ++iFrame)
    {
        for (ma_uint32 iChannel = 0; iChannel < _rf_global_audio_context->device.playback.channels; ++iChannel)
        {
            float* frameOut = framesOut + (iFrame*_rf_global_audio_context->device.playback.channels);
            const float* frameIn  = framesIn  + (iFrame*_rf_global_audio_context->device.playback.channels);

            frameOut[iChannel] += (frameIn[iChannel]*_rf_global_audio_context->master_volume*localVolume);
        }
    }
}

// Initialise the multichannel buffer pool
static void _rf_init_audio_buffer_pool()
{
    // Dummy buffers
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++)
    {
        _rf_global_audio_context->audio_buffer_pool[i] = _rf_init_audio_buffer(RF_DEVICE_FORMAT, RF_DEVICE_CHANNELS, RF_DEVICE_SAMPLE_RATE, 0, rf_audio_buffer_static);
    }
}

// Close the audio buffers pool
static void _rf_close_audio_buffer_pool()
{
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) RF_FREE(_rf_global_audio_context->audio_buffer_pool[i]);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Audio Device initialization and Closing
//----------------------------------------------------------------------------------
// Initialize audio device
extern void rf_audio_init(rf_audio_context* audio_ctx)
{
    _rf_global_audio_context = audio_ctx;
    *_rf_global_audio_context = (rf_audio_context) {}; //Zero the struct just to be sure
    _rf_global_audio_context->master_volume = 1.0f;

    // Init audio context
    ma_context_config contextConfig = ma_context_config_init();
    contextConfig.logCallback = _rf_ma_callback__on_log;

    ma_result result = ma_context_init(NULL, 0, &contextConfig, &_rf_global_audio_context->context);
    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to initialize audio context");
        return;
    }

    // Init audio device
    // NOTE: Using the default device. Format is floating point because it simplifies mixing.
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;  // NULL for the default playback device.
    config.playback.format    = RF_DEVICE_FORMAT;
    config.playback.channels  = RF_DEVICE_CHANNELS;
    config.capture.pDeviceID  = NULL;  // NULL for the default capture device.
    config.capture.format     = ma_format_s16;
    config.capture.channels   = 1;
    config.sampleRate        = RF_DEVICE_SAMPLE_RATE;
    config.dataCallback       = _rf_ma_callback__on_send_audio_data_to_device;
    config.pUserData          = NULL;

    result = ma_device_init(&_rf_global_audio_context->context, &config, &_rf_global_audio_context->device);
    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to initialize audio playback device");
        ma_context_uninit(&_rf_global_audio_context->context);
        return;
    }

    // Keep the device running the whole time. May want to consider doing something a bit smarter and only have the device running
    // while there's at least one sound being played.
    result = ma_device_start(&_rf_global_audio_context->device);
    if (result != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to start audio playback device");
        ma_device_uninit(&_rf_global_audio_context->device);
        ma_context_uninit(&_rf_global_audio_context->context);
        return;
    }

    // Mixing happens on a seperate thread which means we need to synchronize. I'm using a mutex here to make things simple, but may
    // want to look at something a bit smarter later on to keep everything real-time, if that's necessary.
    if (ma_mutex_init(&_rf_global_audio_context->context, &_rf_global_audio_context->audio_lock) != MA_SUCCESS)
    {
        RF_LOG(RF_LOG_ERROR, "Failed to create mutex for audio mixing");
        ma_device_uninit(&_rf_global_audio_context->device);
        ma_context_uninit(&_rf_global_audio_context->context);
        return;
    }

    RF_LOG(RF_LOG_INFO, "Audio device initialized successfully");
    RF_LOG(RF_LOG_INFO, "Audio backend: miniaudio / %s", ma_get_backend_name(_rf_global_audio_context->context.backend));
    RF_LOG(RF_LOG_INFO, "Audio format: %s -> %s", ma_get_format_name(_rf_global_audio_context->device.playback.format), ma_get_format_name(_rf_global_audio_context->device.playback.internalFormat));
    RF_LOG(RF_LOG_INFO, "Audio channels: %d -> %d", _rf_global_audio_context->device.playback.channels, _rf_global_audio_context->device.playback.internalChannels);
    RF_LOG(RF_LOG_INFO, "Audio sample rate: %d -> %d", _rf_global_audio_context->device.sample_rate, _rf_global_audio_context->device.playback.internalSampleRate);
    RF_LOG(RF_LOG_INFO, "Audio buffer size: %d", _rf_global_audio_context->device.playback.internalBufferSizeInFrames);

    _rf_init_audio_buffer_pool();
    RF_LOG(RF_LOG_INFO, "Audio multichannel pool size: %i", RF_MAX_AUDIO_BUFFER_POOL_CHANNELS);

    _rf_global_audio_context->is_audio_initialised = true;
}

//Sets the global context ptr
extern void rf_audio_set_context_ptr(rf_audio_context* audio_ctx)
{
    _rf_global_audio_context = audio_ctx;
    RF_LOG(RF_LOG_WARNING, "Global context pointer set.");
}

// Close the audio device for all contexts
extern void rf_audio_cleanup(void)
{
    if (_rf_global_audio_context->is_audio_initialised)
    {
        ma_mutex_uninit(&_rf_global_audio_context->audio_lock);
        ma_device_uninit(&_rf_global_audio_context->device);
        ma_context_uninit(&_rf_global_audio_context->context);

        _rf_close_audio_buffer_pool();

        RF_LOG(RF_LOG_INFO, "Audio device closed successfully");
    }
    else RF_LOG(RF_LOG_WARNING, "Could not close audio device because it is not currently initialized");
}

// Check if device has been initialized successfully
extern bool rf_is_audio_device_ready(void)
{
    return _rf_global_audio_context->is_audio_initialised;
}

// Set master volume (listener)
extern void rf_set_master_volume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    else if (volume > 1.0f) volume = 1.0f;

    _rf_global_audio_context->master_volume = volume;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Audio Buffer management
//----------------------------------------------------------------------------------

// Initialize a new audio buffer (filled with silence)
static rf_audio_buffer* _rf_init_audio_buffer(ma_format format, ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 buffer_size_in_frames, int usage)
{
    rf_audio_buffer* audio_buffer = (rf_audio_buffer*) RF_MALLOC(sizeof(rf_audio_buffer));
    *audio_buffer = (rf_audio_buffer){};
    audio_buffer->buffer = RF_MALLOC(buffer_size_in_frames * channels * ma_get_bytes_per_sample(format));
    memset(audio_buffer, 0, buffer_size_in_frames * channels * ma_get_bytes_per_sample(format));

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
    dspConfig.onRead = _rf_ma_callback__on_audio_buffer_dsp_read;        // Callback on data reading
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
    audio_buffer->buffer_size_in_frames = buffer_size_in_frames;

    // Buffers should be marked as processed by default so that a call to
    // UpdateAudioStream() immediately after initialization works correctly
    audio_buffer->is_sub_buffer_processed[0] = true;
    audio_buffer->is_sub_buffer_processed[1] = true;

    // Track audio buffer to linked list next position
    _rf_track_audio_buffer(audio_buffer);

    return audio_buffer;
}

// Delete an audio buffer
static void _rf_close_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    _rf_untrack_audio_buffer(buffer);
    RF_FREE(buffer->buffer);
    RF_FREE(buffer);
}

// Check if an audio buffer is playing
static bool _rf_is_audio_buffer_playing(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    bool result = false;

    result = (buffer->playing && !buffer->paused);

    return result;
}

// Play an audio buffer
// NOTE: Buffer is restarted to the start.
// Use PauseAudioBuffer() and ResumeAudioBuffer() if the playback position should be maintained.
static void _rf_play_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    buffer->playing = true;
    buffer->paused = false;
    buffer->frame_cursor_pos = 0;
}

// Stop an audio buffer
static void _rf_stop_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    if (_rf_is_audio_buffer_playing(buffer))
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
static void _rf_pause_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);
    buffer->paused = true;
}

// Resume an audio buffer
static void _rf_resume_audio_buffer(rf_audio_buffer* buffer)
{
    RF_ASSERT(buffer != NULL);

    buffer->paused = false;
}

// Set volume for an audio buffer
static void _rf_set_audio_buffer_volume(rf_audio_buffer* buffer, float volume)
{
    RF_ASSERT(buffer != NULL);

    buffer->volume = volume;
}

// Set pitch for an audio buffer
static void _rf_set_audio_buffer_pitch(rf_audio_buffer* buffer, float pitch)
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
static void _rf_track_audio_buffer(rf_audio_buffer* buffer)
{
    ma_mutex_lock(&_rf_global_audio_context->audio_lock);
    {
        if (_rf_global_audio_context->first_audio_buffer == NULL) _rf_global_audio_context->first_audio_buffer = buffer;
        else
        {
            _rf_global_audio_context->last_audio_buffer->next = buffer;
            buffer->prev = _rf_global_audio_context->last_audio_buffer;
        }

        _rf_global_audio_context->last_audio_buffer = buffer;
    }
    ma_mutex_unlock(&_rf_global_audio_context->audio_lock);
}

// Untrack audio buffer from linked list
static void _rf_untrack_audio_buffer(rf_audio_buffer* buffer)
{
    ma_mutex_lock(&_rf_global_audio_context->audio_lock);
    {
        if (buffer->prev == NULL) _rf_global_audio_context->first_audio_buffer = buffer->next;
        else buffer->prev->next = buffer->next;

        if (buffer->next == NULL) _rf_global_audio_context->last_audio_buffer = buffer->prev;
        else buffer->next->prev = buffer->prev;

        buffer->prev = NULL;
        buffer->next = NULL;
    }
    ma_mutex_unlock(&_rf_global_audio_context->audio_lock);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Sounds loading and playing (.WAV)
//----------------------------------------------------------------------------------

// Load wave data from file
extern rf_wave rf_load_wave_from_file(const char* filename)
{
    rf_wave rf_wave = { 0 };

    if (false) { }
#if defined(RF_SUPPORT_FILEFORMAT_WAV)
        else if (_rf_is_file_extension(filename, ".wav")) rf_wave = rf_load_wav(filename);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
        else if (_rf_is_file_extension(filename, ".ogg")) rf_wave = rf_load_ogg(filename);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
        else if (_rf_is_file_extension(filename, ".flac")) rf_wave = rf_load_flac(filename);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
        else if (_rf_is_file_extension(filename, ".mp3")) rf_wave = rf_load_mp3(filename);
#endif
    else RF_LOG(RF_LOG_WARNING, "[%s] Audio fileformat not supported, it can't be loaded", filename);

    return rf_wave;
}

// Load sound from file
// NOTE: The entire file is loaded to memory to be played (no-streaming)
extern rf_short_audio_stream rf_load_short_audio_stream_from_file(const char* filename)
{
    rf_wave rf_wave = rf_load_wave_from_file(filename);

    rf_short_audio_stream rf_short_audio_stream = rf_load_short_audio_stream_from_wave(rf_wave);

    rf_unload_wave(rf_wave);       // Sound is loaded, we can unload wave

    return rf_short_audio_stream;
}

// Load sound from wave data
// NOTE: Wave data must be unallocated manually
extern rf_short_audio_stream rf_load_short_audio_stream_from_wave(rf_wave wave)
{
    rf_short_audio_stream rf_short_audio_stream = { 0 };

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

        rf_audio_buffer* rf_audio_buffer = _rf_init_audio_buffer(RF_DEVICE_FORMAT, RF_DEVICE_CHANNELS, RF_DEVICE_SAMPLE_RATE, frameCount, rf_audio_buffer_static);
        if (rf_audio_buffer == NULL) RF_LOG(RF_LOG_WARNING, "LoadSoundFromWave() : Failed to create audio buffer");

        frameCount = (ma_uint32)ma_convert_frames(rf_audio_buffer->buffer, rf_audio_buffer->dsp.formatConverterIn.config.formatIn, rf_audio_buffer->dsp.formatConverterIn.config.channels, rf_audio_buffer->dsp.src.config.sampleRateIn, wave.data, formatIn, wave.channels, wave.sample_rate, frameCountIn);
        if (frameCount == 0) RF_LOG(RF_LOG_WARNING, "LoadSoundFromWave() : Format conversion failed");

        rf_short_audio_stream.sample_count = frameCount*RF_DEVICE_CHANNELS;
        rf_short_audio_stream.stream.sample_rate = RF_DEVICE_SAMPLE_RATE;
        rf_short_audio_stream.stream.sample_size = 32;
        rf_short_audio_stream.stream.channels = RF_DEVICE_CHANNELS;
        rf_short_audio_stream.stream.buffer = rf_audio_buffer;
    }

    return rf_short_audio_stream;
}

// Unload wave data
extern void rf_unload_wave(rf_wave wave)
{
    if (wave.data != NULL) RF_FREE(wave.data);
}

// Unload sound
extern void rf_unload_sound(rf_short_audio_stream short_audio_stream)
{
    _rf_close_audio_buffer(short_audio_stream.stream.buffer);
}

// Update sound buffer with new data
extern void rf_update_short_audio_stream(rf_short_audio_stream short_audio_stream, const void* data, int samples_count)
{
    rf_audio_buffer* rf_audio_buffer = short_audio_stream.stream.buffer;

    RF_ASSERT(rf_audio_buffer != NULL);

    _rf_stop_audio_buffer(rf_audio_buffer);

    // TODO: May want to lock/unlock this since this data buffer is read at mixing time
    memcpy(rf_audio_buffer->buffer, data, samples_count * rf_audio_buffer->dsp.formatConverterIn.config.channels * ma_get_bytes_per_sample(rf_audio_buffer->dsp.formatConverterIn.config.formatIn));
}

// Export wave data to file
extern void rf_export_wave(rf_wave wave, const char* filename)
{
    bool success = false;

    if (false) { }
#if defined(RF_SUPPORT_FILEFORMAT_WAV)
        else if (_rf_is_file_extension(filename, ".wav")) success = rf_save_wav(rf_wave, filename);
#endif
    else if (_rf_is_file_extension(filename, ".raw"))
    {
        // Export raw sample data (without header)
        // NOTE: It's up to the user to track wave parameters
        FILE *rawFile = fopen(filename, "wb");
        success = fwrite(wave.data, wave.sample_count * wave.channels * wave.sample_size / 8, 1, rawFile);
        fclose(rawFile);
    }

    if (success) RF_LOG(RF_LOG_INFO, "Wave exported successfully: %s", filename);
    else RF_LOG(RF_LOG_WARNING, "Wave could not be exported.");
}

// Export wave sample data to code (.h)
extern void rf_export_wave_as_code(rf_wave wave, const char* filename)
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
        fprintf(txtFile, "static unsigned char %s_DATA[%i] = { ", varFileName, dataSize);
        for (int i = 0; i < dataSize - 1; i++) fprintf(txtFile, ((i%BYTES_TEXT_PER_LINE == 0)? "0x%x,\n" : "0x%x, "), ((unsigned char* )wave.data)[i]);
        fprintf(txtFile, "0x%x };\n", ((unsigned char* )wave.data)[dataSize - 1]);

        fclose(txtFile);
    }
}

// Play a sound
extern void rf_play_short_audio_stream(rf_short_audio_stream short_audio_stream)
{
    _rf_play_audio_buffer(short_audio_stream.stream.buffer);
}

// Play a sound in the multichannel buffer pool
extern void rf_play_short_audio_stream_multi(rf_short_audio_stream short_audio_stream)
{
    int index = -1;
    unsigned int oldAge = 0;
    int oldIndex = -1;

    // find the first non playing pool entry
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++)
    {
        if (_rf_global_audio_context->audio_buffer_pool_channels[i] > oldAge)
        {
            oldAge = _rf_global_audio_context->audio_buffer_pool_channels[i];
            oldIndex = i;
        }

        if (!_rf_is_audio_buffer_playing(_rf_global_audio_context->audio_buffer_pool[i]))
        {
            index = i;
            break;
        }
    }

    // If no none playing pool members can be index choose the oldest
    if (index == -1)
    {
        RF_LOG(RF_LOG_WARNING,"pool age %i ended a sound early no room in buffer pool", _rf_global_audio_context->audio_buffer_pool_counter);

        if (oldIndex == -1)
        {
            // Shouldn't be able to get here... but just in case something odd happens!
            RF_LOG(RF_LOG_ERROR,"sound buffer pool couldn't determine oldest buffer not playing sound");

            return;
        }

        index = oldIndex;

        // Just in case...
        _rf_stop_audio_buffer(_rf_global_audio_context->audio_buffer_pool[index]);
    }

    // Experimentally mutex lock doesn't seem to be needed this makes sense
    // as audioBufferPool[index] isn't playing and the only stuff we're copying
    // shouldn't be changing...

    _rf_global_audio_context->audio_buffer_pool_channels[index] = _rf_global_audio_context->audio_buffer_pool_counter;
    _rf_global_audio_context->audio_buffer_pool_counter++;

    _rf_global_audio_context->audio_buffer_pool[index]->volume = short_audio_stream.stream.buffer->volume;
    _rf_global_audio_context->audio_buffer_pool[index]->pitch = short_audio_stream.stream.buffer->pitch;
    _rf_global_audio_context->audio_buffer_pool[index]->looping = short_audio_stream.stream.buffer->looping;
    _rf_global_audio_context->audio_buffer_pool[index]->usage = short_audio_stream.stream.buffer->usage;
    _rf_global_audio_context->audio_buffer_pool[index]->is_sub_buffer_processed[0] = false;
    _rf_global_audio_context->audio_buffer_pool[index]->is_sub_buffer_processed[1] = false;
    _rf_global_audio_context->audio_buffer_pool[index]->buffer_size_in_frames = short_audio_stream.stream.buffer->buffer_size_in_frames;
    _rf_global_audio_context->audio_buffer_pool[index]->buffer = short_audio_stream.stream.buffer->buffer;

    _rf_play_audio_buffer(_rf_global_audio_context->audio_buffer_pool[index]);
}

// Stop any sound played with PlaySoundMulti()
extern void rf_stop_short_audio_stream_multi(void)
{
    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++) _rf_stop_audio_buffer(_rf_global_audio_context->audio_buffer_pool[i]);
}

// Get number of sounds playing in the multichannel buffer pool
extern int rf_get_short_audio_streams_playing(void)
{
    int counter = 0;

    for (int i = 0; i < RF_MAX_AUDIO_BUFFER_POOL_CHANNELS; i++)
    {
        if (_rf_is_audio_buffer_playing(_rf_global_audio_context->audio_buffer_pool[i])) counter++;
    }

    return counter;
}

// Pause a sound
extern void rf_pause_short_audio_stream(rf_short_audio_stream short_audio_stream)
{
    _rf_pause_audio_buffer(short_audio_stream.stream.buffer);
}

// Resume a paused sound
extern void rf_resume_short_audio_stream(rf_short_audio_stream short_audio_stream)
{
    _rf_resume_audio_buffer(short_audio_stream.stream.buffer);
}

// Stop reproducing a sound
extern void rf_stop_short_audio_stream(rf_short_audio_stream short_audio_stream)
{
    _rf_stop_audio_buffer(short_audio_stream.stream.buffer);
}

// Check if a sound is playing
extern bool rf_is_short_audio_stream_playing(rf_short_audio_stream short_audio_stream)
{
    return _rf_is_audio_buffer_playing(short_audio_stream.stream.buffer);
}

// Set volume for a sound
extern void rf_set_short_audio_stream_volume(rf_short_audio_stream short_audio_stream, float volume)
{
    _rf_set_audio_buffer_volume(short_audio_stream.stream.buffer, volume);
}

// Set pitch for a sound
extern void rf_set_short_audio_stream_pitch(rf_short_audio_stream short_audio_stream, float pitch)
{
    _rf_set_audio_buffer_pitch(short_audio_stream.stream.buffer, pitch);
}

// Convert wave data to desired format
extern void rf_format_wave(rf_wave* wave, int sample_rate, int sample_size, int channels)
{
    ma_format formatIn  = ((wave->sample_size == 8) ? ma_format_u8 : ((wave->sample_size == 16) ? ma_format_s16 : ma_format_f32));
    ma_format formatOut = ((      sample_size == 8)? ma_format_u8 : ((      sample_size == 16)? ma_format_s16 : ma_format_f32));

    ma_uint32 frameCountIn = wave->sample_count;  // Is wave->sampleCount actually the frame count? That terminology needs to change, if so.

    ma_uint32 frameCount = (ma_uint32)ma_convert_frames(NULL, formatOut, channels, sample_rate, NULL, formatIn, wave->channels, wave->sample_rate, frameCountIn);
    if (frameCount == 0)
    {
        RF_LOG(RF_LOG_ERROR, "WaveFormat() : Failed to get frame count for format conversion.");
        return;
    }

    void* data = RF_MALLOC(frameCount*channels*(sample_size/8));

    frameCount = (ma_uint32)ma_convert_frames(data, formatOut, channels, sample_rate, wave->data, formatIn, wave->channels, wave->sample_rate, frameCountIn);
    if (frameCount == 0)
    {
        RF_LOG(RF_LOG_ERROR, "WaveFormat() : Format conversion failed.");
        return;
    }

    wave->sample_count = frameCount;
    wave->sample_size = sample_size;
    wave->sample_rate = sample_rate;
    wave->channels = channels;
    RF_FREE(wave->data);
    wave->data = data;
}

// Copy a wave to a new wave
extern rf_wave rf_copy_wave(rf_wave wave)
{
    rf_wave newWave = { 0 };

    newWave.data = RF_MALLOC(wave.sample_count * wave.sample_size / 8 * wave.channels);

    if (newWave.data != NULL)
    {
        // NOTE: Size must be provided in bytes
        memcpy(newWave.data, wave.data, wave.sample_count * wave.channels * wave.sample_size / 8);

        newWave.sample_count = wave.sample_count;
        newWave.sample_rate = wave.sample_rate;
        newWave.sample_size = wave.sample_size;
        newWave.channels = wave.channels;
    }

    return newWave;
}

// Crop a wave to defined samples range
// NOTE: Security check in case of out-of-range
extern void rf_crop_wave(rf_wave* rf_wave, int init_sample, int final_sample)
{
    if ((init_sample >= 0) && (init_sample < final_sample) &&
        (final_sample > 0) && ((unsigned int)final_sample < rf_wave->sample_count))
    {
        int sample_count = final_sample - init_sample;

        void* data = RF_MALLOC(sample_count*rf_wave->sample_size/8*rf_wave->channels);

        memcpy(data, (unsigned char* )rf_wave->data + (init_sample * rf_wave->channels * rf_wave->sample_size / 8), sample_count * rf_wave->channels * rf_wave->sample_size / 8);

        RF_FREE(rf_wave->data);
        rf_wave->data = data;
    }
    else RF_LOG(RF_LOG_WARNING, "Wave crop range out of bounds");
}

// Get samples data from wave as a floats array
// NOTE: Returned sample values are normalized to range [-1..1]
extern float* rf_get_wave_data(rf_wave rf_wave)
{
    float* samples = (float* )RF_MALLOC(rf_wave.sample_count*rf_wave.channels*sizeof(float));

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

//----------------------------------------------------------------------------------
// Module Functions Definition - Music loading and stream playing (.OGG)
//----------------------------------------------------------------------------------

// Load music stream from file
extern rf_long_audio_stream rf_load_long_audio_stream(const char* filename)
{
    rf_long_audio_stream rf_long_audio_stream = { 0 };
    bool musicLoaded = false;

    if (false) { }
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
    else if (_rf_is_file_extension(filename, ".ogg"))
    {
        // Open ogg audio stream
        rf_long_audio_stream.ctx_data = stb_vorbis_open_filename(filename, NULL, NULL);

        if (rf_long_audio_stream.ctx_data != NULL)
        {
            rf_long_audio_stream.ctx_type = rf_audio_context_ogg;
            stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis *)rf_long_audio_stream.ctx_data);  // Get Ogg file info

            // OGG bit rate defaults to 16 bit, it's enough for compressed format
            rf_long_audio_stream.stream = rf_create_audio_stream(info.sample_rate, 16, info.channels);
            rf_long_audio_stream.sample_count = (unsigned int)stb_vorbis_stream_length_in_samples((stb_vorbis *)rf_long_audio_stream.ctx_data)*info.channels;
            rf_long_audio_stream.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
    else if (_rf_is_file_extension(filename, ".flac"))
    {
        rf_long_audio_stream.ctx_data = drflac_open_file(filename);

        if (rf_long_audio_stream.ctx_data != NULL)
        {
            rf_long_audio_stream.ctx_type = rf_audio_context_flac;
            drflac *ctxFlac = (drflac *)rf_long_audio_stream.ctx_data;

            rf_long_audio_stream.stream = rf_create_audio_stream(ctxFlac->sample_rate, ctxFlac->bitsPerSample, ctxFlac->channels);
            rf_long_audio_stream.sample_count = (unsigned int)ctxFlac->totalSampleCount;
            rf_long_audio_stream.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
    else if (_rf_is_file_extension(filename, ".mp3"))
    {
        drmp3 *ctxMp3 = RF_MALLOC(sizeof(drmp3));
        rf_long_audio_stream.ctx_data = ctxMp3;

        int result = drmp3_init_file(ctxMp3, filename, NULL);

        if (result > 0)
        {
            rf_long_audio_stream.ctx_type = rf_audio_context_mp3;

            rf_long_audio_stream.stream = rf_create_audio_stream(ctxMp3->sample_rate, 32, ctxMp3->channels);
            rf_long_audio_stream.sample_count = drmp3_get_pcm_frame_count(ctxMp3)*ctxMp3->channels;
            rf_long_audio_stream.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif
#if defined(SUPPORT_FILEFORMAT_XM)
    else if (_rf_is_file_extension(filename, ".xm"))
    {
        jar_xm_context_t *ctxXm = NULL;

        int result = jar_xm_create_context_from_file(&ctxXm, 48000, filename);

        if (result == 0)    // XM context created successfully
        {
            rf_long_audio_stream.ctx_type = rf_audio_context_xm;
            jar_xm_set_max_loop_count(ctxXm, 0);    // Set infinite number of loops

            // NOTE: Only stereo is supported for XM
            rf_long_audio_stream.stream = rf_create_audio_stream(48000, 16, 2);
            rf_long_audio_stream.sample_count = (unsigned int)jar_xm_get_remaining_samples(ctxXm);
            rf_long_audio_stream.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;

            rf_long_audio_stream.ctx_data = ctxXm;
        }
    }
#endif
#if defined(SUPPORT_FILEFORMAT_MOD)
    else if (_rf_is_file_extension(filename, ".mod"))
    {
        jar_mod_context_t *ctxMod = RF_MALLOC(sizeof(jar_mod_context_t));
        rf_long_audio_stream.ctx_data = ctxMod;

        jar_mod_init(ctxMod);
        int result = jar_mod_load_file(ctxMod, filename);

        if (result > 0)
        {
            rf_long_audio_stream.ctx_type = rf_audio_context_mod;

            // NOTE: Only stereo is supported for MOD
            rf_long_audio_stream.stream = rf_create_audio_stream(48000, 16, 2);
            rf_long_audio_stream.sample_count = (unsigned int)jar_mod_max_samples(ctxMod);
            rf_long_audio_stream.loop_count = 0;   // Infinite loop by default
            musicLoaded = true;
        }
    }
#endif

    if (!musicLoaded)
    {
        if (false) { }
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
            else if (rf_long_audio_stream.ctx_type == rf_audio_context_ogg) stb_vorbis_close((stb_vorbis *)rf_long_audio_stream.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
            else if (rf_long_audio_stream.ctx_type == rf_audio_context_flac) drflac_free((drflac *)rf_long_audio_stream.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
            else if (rf_long_audio_stream.ctx_type == rf_audio_context_mp3) { drmp3_uninit((drmp3 *)rf_long_audio_stream.ctx_data); RF_FREE(rf_long_audio_stream.ctx_data); }
#endif
#if defined(SUPPORT_FILEFORMAT_XM)
            else if (rf_long_audio_stream.ctx_type == rf_audio_context_xm) jar_xm_free_context((jar_xm_context_t *)rf_long_audio_stream.ctx_data);
#endif
#if defined(SUPPORT_FILEFORMAT_MOD)
            else if (rf_long_audio_stream.ctx_type == rf_audio_context_mod) { jar_mod_unload((jar_mod_context_t *)rf_long_audio_stream.ctx_data); RF_FREE(rf_long_audio_stream.ctx_data); }
#endif

        RF_LOG(RF_LOG_WARNING, "[%s] Music file could not be opened", filename);
    }
    else
    {
        // Show some music stream info
        RF_LOG(RF_LOG_INFO, "[%s] Music file successfully loaded:", filename);
        RF_LOG(RF_LOG_INFO, "   Total samples: %i", rf_long_audio_stream.sample_count);
        RF_LOG(RF_LOG_INFO, "   Sample rate: %i Hz", rf_long_audio_stream.stream.sample_rate);
        RF_LOG(RF_LOG_INFO, "   Sample size: %i bits", rf_long_audio_stream.stream.sample_size);
        RF_LOG(RF_LOG_INFO, "   Channels: %i (%s)", rf_long_audio_stream.stream.channels, (rf_long_audio_stream.stream.channels == 1)? "Mono" : (rf_long_audio_stream.stream.channels == 2)? "Stereo" : "Multi");
    }

    return rf_long_audio_stream;
}

// Unload music stream
extern void rf_unload_long_audio_stream(rf_long_audio_stream long_audio_stream)
{
    rf_close_audio_stream(long_audio_stream.stream);

    if (false) { }
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
    else if (rf_long_audio_stream.ctx_type == rf_audio_context_ogg) stb_vorbis_close((stb_vorbis *)rf_long_audio_stream.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
    else if (rf_long_audio_stream.ctx_type == rf_audio_context_flac) drflac_free((drflac *)rf_long_audio_stream.ctx_data);
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
    else if (rf_long_audio_stream.ctx_type == rf_audio_context_mp3) { drmp3_uninit((drmp3 *)rf_long_audio_stream.ctx_data); RF_FREE(rf_long_audio_stream.ctx_data); }
#endif
#if defined(SUPPORT_FILEFORMAT_XM)
    else if (rf_long_audio_stream.ctx_type == rf_audio_context_xm) jar_xm_free_context((jar_xm_context_t *)rf_long_audio_stream.ctx_data);
#endif
#if defined(SUPPORT_FILEFORMAT_MOD)
    else if (rf_long_audio_stream.ctx_type == rf_audio_context_mod) { jar_mod_unload((jar_mod_context_t *)rf_long_audio_stream.ctx_data); RF_FREE(rf_long_audio_stream.ctx_data); }
#endif
}

// Start music playing (open stream)
extern void rf_play_long_audio_stream(rf_long_audio_stream long_audio_stream)
{
    rf_audio_buffer* rf_audio_buffer = long_audio_stream.stream.buffer;

    if (rf_audio_buffer != NULL)
    {
        // For music streams, we need to make sure we maintain the frame cursor position
        // This is a hack for this section of code in UpdateMusicStream()
        // NOTE: In case window is minimized, music stream is stopped, just make sure to
        // play again on window restore: if (IsMusicPlaying(music)) PlayMusicStream(music);
        ma_uint32 frame_cursor_pos = rf_audio_buffer->frame_cursor_pos;
        rf_play_audio_stream(long_audio_stream.stream);  // WARNING: This resets the cursor position.
        rf_audio_buffer->frame_cursor_pos = frame_cursor_pos;
    }
    else RF_LOG(RF_LOG_ERROR, "PlayMusicStream() : No audio buffer");

}

// Pause music playing
extern void rf_pause_long_audio_stream(rf_long_audio_stream long_audio_stream)
{
    rf_pause_audio_stream(long_audio_stream.stream);
}

// Resume music playing
extern void rf_resume_long_audio_stream(rf_long_audio_stream long_audio_stream)
{
    rf_resume_audio_stream(long_audio_stream.stream);
}

// Stop music playing (close stream)
extern void rf_stop_long_audio_stream(rf_long_audio_stream long_audio_stream)
{
    rf_stop_audio_stream(long_audio_stream.stream);

    // Restart music context
    switch (long_audio_stream.ctx_type)
    {
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
        case rf_audio_context_ogg: stb_vorbis_seek_start((stb_vorbis *)rf_long_audio_stream.ctx_data); break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
        case rf_audio_context_flac: drflac_seek_to_pcm_frame((drflac *)rf_long_audio_stream.ctx_data, 0); break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
        case rf_audio_context_mp3: drmp3_seek_to_pcm_frame((drmp3 *)rf_long_audio_stream.ctx_data, 0); break;
#endif
#if defined(SUPPORT_FILEFORMAT_XM)
        case rf_audio_context_xm: jar_xm_reset((jar_xm_context_t *)rf_long_audio_stream.ctx_data); break;
#endif
#if defined(SUPPORT_FILEFORMAT_MOD)
        case rf_audio_context_mod: jar_mod_seek_start((jar_mod_context_t *)rf_long_audio_stream.ctx_data); break;
#endif
        default: break;
    }
}

// Update (re-fill) music buffers if data already processed
extern void rf_update_long_audio_stream(rf_long_audio_stream long_audio_stream)
{
    bool streamEnding = false;

    unsigned int subBufferSizeInFrames = long_audio_stream.stream.buffer->buffer_size_in_frames / 2;

    // NOTE: Using dynamic allocation because it could require more than 16KB
    void* pcm = RF_MALLOC(subBufferSizeInFrames * long_audio_stream.stream.channels * long_audio_stream.stream.sample_size / 8);
    memset(pcm, 0, subBufferSizeInFrames * long_audio_stream.stream.channels * long_audio_stream.stream.sample_size / 8);

    int samplesCount = 0;    // Total size of data streamed in L+R samples for xm floats, individual L or R for ogg shorts

    // TODO: Get the sampleLeft using totalFramesProcessed... but first, get total frames processed correctly...
    //ma_uint32 frameSizeInBytes = ma_get_bytes_per_sample(music.stream.buffer->dsp.formatConverterIn.config.formatIn)*music.stream.buffer->dsp.formatConverterIn.config.channels;
    int sampleLeft = long_audio_stream.sample_count - (long_audio_stream.stream.buffer->total_frames_processed * long_audio_stream.stream.channels);

    while (rf_is_audio_stream_processed(long_audio_stream.stream))
    {
        if ((sampleLeft / long_audio_stream.stream.channels) >= subBufferSizeInFrames) samplesCount = subBufferSizeInFrames * long_audio_stream.stream.channels;
        else samplesCount = sampleLeft;

        switch (long_audio_stream.ctx_type)
        {
#if defined(RF_SUPPORT_FILEFORMAT_OGG)
            case rf_audio_context_ogg:
            {
                // NOTE: Returns the number of samples to process (be careful! we ask for number of shorts!)
                stb_vorbis_get_samples_short_interleaved((stb_vorbis *)rf_long_audio_stream.ctx_data, rf_long_audio_stream.stream.channels, (short *)pcm, samplesCount);

            } break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
            case rf_audio_context_flac:
            {
                // NOTE: Returns the number of samples to process (not required)
                drflac_read_pcm_frames_s16((drflac *)rf_long_audio_stream.ctx_data, samplesCount, (short *)pcm);

            } break;
#endif
#if defined(RF_SUPPORT_FILEFORMAT_MP3)
            case rf_audio_context_mp3:
            {
                // NOTE: samplesCount, actually refers to framesCount and returns the number of frames processed
                drmp3_read_pcm_frames_f32((drmp3 *)rf_long_audio_stream.ctx_data, samplesCount/rf_long_audio_stream.stream.channels, (float* )pcm);

            } break;
#endif
#if defined(SUPPORT_FILEFORMAT_XM)
            case rf_audio_context_xm:
            {
                // NOTE: Internally this function considers 2 channels generation, so samplesCount/2
                jar_xm_generate_samples_16bit((jar_xm_context_t *)rf_long_audio_stream.ctx_data, (short *)pcm, samplesCount/2);
            } break;
#endif
#if defined(SUPPORT_FILEFORMAT_MOD)
            case rf_audio_context_mod:
            {
                // NOTE: 3rd parameter (nbsample) specify the number of stereo 16bits samples you want, so sampleCount/2
                jar_mod_fillbuffer((jar_mod_context_t *)rf_long_audio_stream.ctx_data, (short *)pcm, samplesCount/2, 0);
            } break;
#endif
            default: break;
        }

        rf_update_audio_stream(long_audio_stream.stream, pcm, samplesCount);

        if ((long_audio_stream.ctx_type == rf_audio_context_xm) || (long_audio_stream.ctx_type == rf_audio_context_mod))
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
        rf_stop_long_audio_stream(long_audio_stream);        // Stop music (and reset)

        // Decrease loopCount to stop when required
        if (long_audio_stream.loop_count > 1)
        {
            long_audio_stream.loop_count--;         // Decrease loop count
            rf_play_long_audio_stream(long_audio_stream);    // Play again
        }
        else if (long_audio_stream.loop_count == 0) rf_play_long_audio_stream(long_audio_stream);
    }
    else
    {
        // NOTE: In case window is minimized, music stream is stopped,
        // just make sure to play again on window restore
        if (rf_is_long_audio_stream_playing(long_audio_stream)) rf_play_long_audio_stream(long_audio_stream);
    }
}

// Check if any music is playing
extern bool rf_is_long_audio_stream_playing(rf_long_audio_stream long_audio_stream)
{
    return rf_is_audio_stream_playing(long_audio_stream.stream);
}

// Set volume for music
extern void rf_set_long_audio_stream_volume(rf_long_audio_stream long_audio_stream, float volume)
{
    rf_set_audio_stream_volume(long_audio_stream.stream, volume);
}

// Set pitch for music
extern void rf_set_long_audio_stream_pitch(rf_long_audio_stream long_audio_stream, float pitch)
{
    rf_set_audio_stream_pitch(long_audio_stream.stream, pitch);
}

// Set music loop count (loop repeats)
// NOTE: If set to 0, means infinite loop
extern void rf_set_long_audio_stream_loop_count(rf_long_audio_stream long_audio_stream, int count)
{
    long_audio_stream.loop_count = count;
}

// Get music time length (in seconds)
extern float rf_get_long_audio_stream_time_length(rf_long_audio_stream long_audio_stream)
{
    float totalSeconds = 0.0f;

    totalSeconds = (float)long_audio_stream.sample_count / (long_audio_stream.stream.sample_rate * long_audio_stream.stream.channels);

    return totalSeconds;
}

// Get current music time played (in seconds)
extern float rf_get_long_audio_stream_time_played(rf_long_audio_stream long_audio_stream)
{
    float secondsPlayed = 0.0f;

    //ma_uint32 frameSizeInBytes = ma_get_bytes_per_sample(music.stream.buffer->dsp.formatConverterIn.config.formatIn)*music.stream.buffer->dsp.formatConverterIn.config.channels;
    unsigned int samplesPlayed = long_audio_stream.stream.buffer->total_frames_processed * long_audio_stream.stream.channels;
    secondsPlayed = (float)samplesPlayed/(long_audio_stream.stream.sample_rate * long_audio_stream.stream.channels);

    return secondsPlayed;
}

// Init audio stream (to stream audio pcm data)
extern rf_audio_stream rf_create_audio_stream(unsigned int sample_rate, unsigned int sample_size, unsigned int channels)
{
    rf_audio_stream stream = { 0 };

    stream.sample_rate = sample_rate;
    stream.sample_size = sample_size;
    stream.channels = channels;

    ma_format formatIn = ((stream.sample_size == 8)? ma_format_u8 : ((stream.sample_size == 16)? ma_format_s16 : ma_format_f32));

    // The size of a streaming buffer must be at least double the size of a period
    unsigned int periodSize = _rf_global_audio_context->device.playback.internalBufferSizeInFrames/_rf_global_audio_context->device.playback.internalPeriods;
    unsigned int subBufferSize = RF_AUDIO_BUFFER_SIZE;

    if (subBufferSize < periodSize) subBufferSize = periodSize;

    stream.buffer = _rf_init_audio_buffer(formatIn, stream.channels, stream.sample_rate, subBufferSize*2, rf_audio_buffer_stream);

    if (stream.buffer != NULL)
    {
        stream.buffer->looping = true;    // Always loop for streaming buffers
        RF_LOG(RF_LOG_INFO, "Audio stream loaded successfully (%i Hz, %i bit, %s)", stream.sample_rate, stream.sample_size, (stream.channels == 1)? "Mono" : "Stereo");
    }
    else RF_LOG(RF_LOG_ERROR, "InitAudioStream() : Failed to create audio buffer");

    return stream;
}

// Close audio stream and free memory
extern void rf_close_audio_stream(rf_audio_stream stream)
{
    _rf_close_audio_buffer(stream.buffer);

    RF_LOG(RF_LOG_INFO, "Unloaded audio stream data");
}

// Update audio stream buffers with data
// NOTE 1: Only updates one buffer of the stream source: unqueue -> update -> queue
// NOTE 2: To unqueue a buffer it needs to be processed: IsAudioStreamProcessed()
extern void rf_update_audio_stream(rf_audio_stream stream, const void* data, int samplesCount)
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

            ma_uint32 subBufferSizeInFrames = rf_audio_buffer->buffer_size_in_frames/2;
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
extern bool rf_is_audio_stream_processed(rf_audio_stream stream)
{
    if (stream.buffer == NULL)
    {
        RF_LOG(RF_LOG_ERROR, "IsAudioStreamProcessed() : No audio buffer");
        return false;
    }

    return (stream.buffer->is_sub_buffer_processed[0] || stream.buffer->is_sub_buffer_processed[1]);
}

// Play audio stream
extern void rf_play_audio_stream(rf_audio_stream stream)
{
    _rf_play_audio_buffer(stream.buffer);
}

// Play audio stream
extern void rf_pause_audio_stream(rf_audio_stream stream)
{
    _rf_pause_audio_buffer(stream.buffer);
}

// Resume audio stream playing
extern void rf_resume_audio_stream(rf_audio_stream stream)
{
    _rf_resume_audio_buffer(stream.buffer);
}

// Check if audio stream is playing.
extern bool rf_is_audio_stream_playing(rf_audio_stream stream)
{
    return _rf_is_audio_buffer_playing(stream.buffer);
}

// Stop audio stream
extern void rf_stop_audio_stream(rf_audio_stream stream)
{
    _rf_stop_audio_buffer(stream.buffer);
}

extern void rf_set_audio_stream_volume(rf_audio_stream stream, float volume)
{
    _rf_set_audio_buffer_volume(stream.buffer, volume);
}

extern void rf_set_audio_stream_pitch(rf_audio_stream stream, float pitch)
{
    _rf_set_audio_buffer_pitch(stream.buffer, pitch);
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

#if defined(RF_SUPPORT_FILEFORMAT_WAV)
// Load WAV file into Wave structure
static rf_wave rf_load_wav(const char* filename)
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
        RF_LOG(RF_LOG_WARNING, "[%s] WAV file could not be opened", filename);
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
                RF_LOG(RF_LOG_WARNING, "[%s] Invalid RIFF or WAVE Header", filename);
        }
        else
        {
            // Read in the 2nd chunk for the wave info
            fread(&wavFormat, sizeof(WAVFormat), 1, wavFile);

            // Check for fmt tag
            if ((wavFormat.subChunkID[0] != 'f') || (wavFormat.subChunkID[1] != 'm') ||
                (wavFormat.subChunkID[2] != 't') || (wavFormat.subChunkID[3] != ' '))
            {
                RF_LOG(RF_LOG_WARNING, "[%s] Invalid Wave format", filename);
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
                    RF_LOG(RF_LOG_WARNING, "[%s] Invalid data header", filename);
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
                        RF_LOG(RF_LOG_WARNING, "[%s] WAV sample size (%ibit) not supported, converted to 16bit", filename, rf_wave.sample_size);
                        rf_format_wave(&rf_wave, rf_wave.sample_rate, 16, rf_wave.channels);
                    }

                    // NOTE: Only support up to 2 channels (mono, stereo)
                    if (rf_wave.channels > 2)
                    {
                        rf_format_wave(&rf_wave, rf_wave.sample_rate, rf_wave.sample_size, 2);
                        RF_LOG(RF_LOG_WARNING, "[%s] WAV channels number (%i) not supported, converted to 2 channels", filename, rf_wave.channels);
                    }

                    // NOTE: subChunkSize comes in bytes, we need to translate it to number of samples
                    rf_wave.sample_count = (wavData.subChunkSize/(rf_wave.sample_size/8))/rf_wave.channels;

                    RF_LOG(RF_LOG_INFO, "[%s] WAV file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");
                }
            }
        }

        fclose(wavFile);
    }

    return rf_wave;
}

// Save wave data as WAV file
static int rf_save_wav(rf_wave rf_wave, const char* filename)
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

    if (wavFile == NULL) RF_LOG(RF_LOG_WARNING, "[%s] WAV audio file could not be created", filename);
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
#endif

#if defined(RF_SUPPORT_FILEFORMAT_OGG)
// Load OGG file into Wave structure
// NOTE: Using stb_vorbis library
static rf_wave rf_load_ogg(const char* filename)
{
    rf_wave rf_wave = { 0 };

    stb_vorbis *oggFile = stb_vorbis_open_filename(filename, NULL, NULL);

    if (oggFile == NULL) RF_LOG(RF_LOG_WARNING, "[%s] OGG file could not be opened", filename);
    else
    {
        stb_vorbis_info info = stb_vorbis_get_info(oggFile);

        rf_wave.sample_rate = info.sample_rate;
        rf_wave.sample_size = 16;                   // 16 bit per sample (short)
        rf_wave.channels = info.channels;
        rf_wave.sample_count = (unsigned int)stb_vorbis_stream_length_in_samples(oggFile)*info.channels;  // Independent by channel

        float totalSeconds = stb_vorbis_stream_length_in_seconds(oggFile);
        if (totalSeconds > 10) RF_LOG(RF_LOG_WARNING, "[%s] Ogg audio length is larger than 10 seconds (%f), that's a big file in memory, consider music streaming", filename, totalSeconds);

        rf_wave.data = (short *)RF_MALLOC(rf_wave.sample_count*rf_wave.channels*sizeof(short));

        // NOTE: Returns the number of samples to process (be careful! we ask for number of shorts!)
        int numSamplesOgg = stb_vorbis_get_samples_short_interleaved(oggFile, info.channels, (short *)rf_wave.data, rf_wave.sample_count*rf_wave.channels);

        RF_LOG(RF_LOG_DEBUG, "[%s] Samples obtained: %i", filename, numSamplesOgg);

        RF_LOG(RF_LOG_INFO, "[%s] OGG file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");

        stb_vorbis_close(oggFile);
    }

    return rf_wave;
}
#endif

#if defined(RF_SUPPORT_FILEFORMAT_FLAC)
// Load FLAC file into Wave structure
// NOTE: Using dr_flac library
static rf_wave rf_load_flac(const char* filename)
{
    rf_wave rf_wave;

    // Decode an entire FLAC file in one go
    uint64_t totalSampleCount;
    rf_wave.data = drflac_open_file_and_read_pcm_frames_s16(filename, &rf_wave.channels, &rf_wave.sample_rate, &totalSampleCount);

    rf_wave.sample_count = (unsigned int)totalSampleCount;
    rf_wave.sample_size = 16;

    // NOTE: Only support up to 2 channels (mono, stereo)
    if (rf_wave.channels > 2) RF_LOG(RF_LOG_WARNING, "[%s] FLAC channels number (%i) not supported", filename, rf_wave.channels);

    if (rf_wave.data == NULL) RF_LOG(RF_LOG_WARNING, "[%s] FLAC data could not be loaded", filename);
    else RF_LOG(RF_LOG_INFO, "[%s] FLAC file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");

    return rf_wave;
}
#endif

#if defined(RF_SUPPORT_FILEFORMAT_MP3)
// Load MP3 file into Wave structure
// NOTE: Using dr_mp3 library
static rf_wave rf_load_mp3(const char* filename)
{
    rf_wave rf_wave = { 0 };

    // Decode an entire MP3 file in one go
    uint64_t totalFrameCount = 0;
    drmp3_config config = { 0 };
    rf_wave.data = drmp3_open_file_and_read_f32(filename, &config, &totalFrameCount);

    rf_wave.channels = config.outputChannels;
    rf_wave.sample_rate = config.outputSampleRate;
    rf_wave.sample_count = (int)totalFrameCount*rf_wave.channels;
    rf_wave.sample_size = 32;

    // NOTE: Only support up to 2 channels (mono, stereo)
    if (rf_wave.channels > 2) RF_LOG(RF_LOG_WARNING, "[%s] MP3 channels number (%i) not supported", filename, rf_wave.channels);

    if (rf_wave.data == NULL) RF_LOG(RF_LOG_WARNING, "[%s] MP3 data could not be loaded", filename);
    else RF_LOG(RF_LOG_INFO, "[%s] MP3 file loaded successfully (%i Hz, %i bit, %s)", filename, rf_wave.sample_rate, rf_wave.sample_size, (rf_wave.channels == 1)? "Mono" : "Stereo");

    return rf_wave;
}
#endif

#undef rf_audio_buffer
#endif
//endregion