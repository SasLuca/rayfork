#include "rayfork.h"

#include "rayfork-core.c"
#include "rayfork-str.c"
#include "rayfork-math.c"

#if !defined(RAYFORK_NO_GFX)
#include "rayfork-gfx.c"
#endif

#if !defined(RAYFORK_NO_AUDIO)
#include "rayfork-audio.c"
#endif
