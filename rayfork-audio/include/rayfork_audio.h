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

// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized from { } initializers.
#ifdef __cplusplus
#define RF_CLITERAL(type) type
#else
#define RF_CLITERAL(type) (type)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#include <stdbool.h>



#ifdef __cplusplus
}
#endif

#endif // RF_AUDIO_H
//endregion