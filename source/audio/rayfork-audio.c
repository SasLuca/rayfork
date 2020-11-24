#include "rayfork-audio.h"

#define STB_VORBIS_IMPLEMENTATION
#define JAR_XM_IMPLEMENTATION
#define JAR_MOD_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_WIN32_IO
#define DR_MP3_IMPLEMENTATION
#include "miniaudio.h"
#include "jar_mod.h"
#include "jar_xm.h"

RF_API rf_valid rf_audio_init()
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
            /* Loop over each device info and do something with it. Here we just print the name with their index. You may want
               to give the user the opportunity to choose which device they'd prefer. */
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

            ma_device device;
            if (ma_device_init(&context, &config, &device) != MA_SUCCESS)
            {
                // Error
            }
        }
    }
}

RF_API rf_audio_device rf_audio_device_count();
RF_API rf_audio_device rf_default_audio_device();