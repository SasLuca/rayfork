#ifndef RAYFORK_ASSERT_H
#define RAYFORK_ASSERT_H

#if !defined(RF_ASSERT) && defined(RAYFORK_ENABLE_ASSERTIONS)
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#else
    #define RF_ASSERT(condition)
#endif

#endif // RAYFORK_ASSERT_H