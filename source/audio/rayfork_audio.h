#ifndef RF_AUDIO_H
#define RF_AUDIO_H

#include "rayfork_internal_base.h"
#include "miniaudio/miniaudio.h"

/*
This buffer size is defined by number of samples, independent of sample size and channels number
After some math, considering a sampleRate of 48000, a buffer refill rate of 1/60 seconds and a
standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
In case of music-stalls, just increase this number
*/
#define RF_DEFAULT_AUDIO_BUFFER_SIZE     (4096)
#define RF_DEFAULT_AUDIO_DEVICE_FORMAT   ma_format_f32
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

typedef struct rf_audio_source_base
{
    ma_uint64 (*decode) (struct rf_audio_source_base* this_source, void* frames_dst, ma_uint64 frames_count);
    ma_result (*seek)   (struct rf_audio_source_base* this_source, ma_uint64 target_frame);
} rf_audio_source_base;

typedef struct rf_audio_source
{
    rf_audio_source_base base;

    rf_audio_format format;
    const void* source;
    int source_size;

    rf_audio_channels channels;
    int sample_rate; // Frequency (samples per second)
    int sample_size; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    int size_in_frames;
    int frame_cursor_pos;
    int total_frames_processed;
    bool valid;
} rf_audio_source;

int rf_decoded_wav_size(const void* wav_file_contents, int wav_file_contents_size);
int rf_decoded_flac_size(const void* flac_file_contents, int flac_file_contents_size);

rf_audio_source rf_static_audio_from_memory(const void* file, rf_allocator allocator);
rf_audio_source rf_static_audio_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);

#endif // RF_AUDIO_H