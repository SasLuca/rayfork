#include "rayfork.h"

#include "rayfork-core.c"
#include "rayfork-str.c"
#include "rayfork-math.c"

#if !defined(rayfork_no_gfx)
#include "rayfork-gfx.c"
#endif

#if !defined(rayfork_no_audio)
#include "rayfork-audio.c"
#endif