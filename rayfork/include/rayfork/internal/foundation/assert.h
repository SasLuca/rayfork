#ifndef RAYFORK_FOUNDATION_INTERNAL_ASSERT_H
#define RAYFORK_FOUNDATION_INTERNAL_ASSERT_H

#if !defined(rf_assert) && defined(rayfork_enable_assertions)
    #include "assert.h"
    #define rf_assert(condition) assert(condition)
#else
    #define rf_assert(condition)
#endif

#endif // RAYFORK_FOUNDATION_INTERNAL_ASSERT_H