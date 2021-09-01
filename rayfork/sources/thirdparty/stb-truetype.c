#include "rayfork/foundation/allocator.h"
#include "rayfork/internal/foundation/assert.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(sz, u) rf_alloc(rf_global_allocator_for_thirdparty_libraries, sz)
#define STBTT_free(p, u)    rf_free(rf_global_allocator_for_thirdparty_libraries, p)
#define STBTT_assert(it)    rf_assert(it)
#include "stb_truetype/stb_truetype.h"