#include "rayfork/foundation/allocator.h"
#include "rayfork/internal/foundation/assert.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(sz,c)   ((void)(c), rf_alloc(rf_global_allocator_for_thirdparty_libraries, sz))
#define STBIR_FREE(p,c)      ((void)(c), rf_free(rf_global_allocator_for_thirdparty_libraries, p))
#define STBIR_ASSERT(it)     rf_assert(it)
#define STBIRDEF             rf_extern
#include "stb_image_resize/stb_image_resize.h"