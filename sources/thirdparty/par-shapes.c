#include "rayfork/foundation/allocator.h"
#include "rayfork/foundation/io.h"

#define PAR_SHAPES_IMPLEMENTATION
#define PAR_MALLOC(T, N)               ((T*)rf_alloc(rf_global_allocator_for_thirdparty_libraries, N * sizeof(T)))
#define PAR_CALLOC(T, N)               ((T*)rf_calloc(rf_global_allocator_for_thirdparty_libraries, N, sizeof(T)))
#define PAR_FREE(BUF)                  (rf_free(rf_global_allocator_for_thirdparty_libraries, BUF))
#define PAR_REALLOC(T, BUF, N, OLD_SZ) ((T*) rf_realloc_ex(rf_global_allocator_for_thirdparty_libraries, BUF, OLD_SZ, sizeof(T) * (N)))
#define PARDEF                         rf_extern
#include "par_shapes.h"