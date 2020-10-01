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

RF_API void rf_init_audio(rf_audio_backend_data* audio_data)
{
}