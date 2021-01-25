#include "rayfork/foundation/allocator.h"
#include "rayfork/internal/foundation/assert.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)                     rf_alloc(rf_global_allocator_for_thirdparty_libraries, sz)
#define STBI_FREE(p)                        rf_free(rf_global_allocator_for_thirdparty_libraries, p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) rf_realloc_ex(rf_global_allocator_for_thirdparty_libraries, p, oldsz, newsz)
#define STBI_ASSERT(it)                     rf_assert(it)
#define STBIDEF                             rf_extern
#include "stb_image.h"