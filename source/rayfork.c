#include "rayfork.h"

#include "rayfork-core.c"

#if !defined(RAYFORK_NO_STR)
    #include "rayfork-str.c"
#endif

#if !defined(RAYFORK_NO_MATH)
    #include "rayfork-math.c"
#endif

#if !defined(RAYFORK_NO_GFX)
    #if defined(RAYFORK_NO_STR) || defined(RAYFORK_NO_MATH)
        #error rayfork: rayfork-gfx requires rayfork-unicode and rayfork-math
    #endif

    #include "rayfork-gfx.c"
#endif

#if defined(RAYFORK_NO_AUDIO)
    #include "rayfork-audio.c"
#endif