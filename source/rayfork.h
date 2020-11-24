#ifndef RAYFORK_H
#define RAYFORK_H

#include "rayfork-core.h"

#if !defined(RAYFORK_NO_STR)
    #include "rayfork-str.h"
#endif

#if !defined(RAYFORK_NO_CSV)
    //#include "rayfork-csv.h"
#endif

#if !defined(RAYFORK_NO_MATH)
    #include "rayfork-math.h"
#endif

#if !defined(RAYFORK_NO_GFX)
    #if defined(RAYFORK_NO_UNICODE) || defined(RAYFORK_NO_MATH)
        #error rayfork: rayfork-gfx requires rayfork-unicode and rayfork-math
    #endif

    #include "rayfork-gfx.h"
#endif

#if defined(RAYFORK_NO_AUDIO)
    #include "rayfork-audio.h"
#endif

#endif // RAYFORK_H