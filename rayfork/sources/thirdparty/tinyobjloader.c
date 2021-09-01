#include "rayfork/foundation/allocator.h"
#include "rayfork/foundation/io.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#define TINYOBJ_MALLOC(size)             (rf_alloc(rf_global_allocator_for_thirdparty_libraries, (size)))
#define TINYOBJ_REALLOC(p, newsz)        (rf_realloc_ez(rf_global_allocator_for_thirdparty_libraries, (p), (newsz)))
#define TINYOBJ_CALLOC(amount, size)     (rf_calloc(rf_global_allocator_for_thirdparty_libraries, (amount), (size)))
#define TINYOBJ_FREE(p)                  (rf_free(rf_global_allocator_for_thirdparty_libraries, (p)))
#define TINYOBJDEF                       rf_extern
#include "tinyobj_loader_c/tinyobj_loader_c.h"

#include "rayfork/internal/thirdparty/tinyobj-utils.h"

rf_extern void rf_tinyobj_file_reader_callback(const char* filename, int is_mtl, const char* obj_filename, char** buf, size_t* len)
{
    if (!filename || !buf || !len) return;

    rf_file_contents contents = rf_read_entire_file(filename, rf_global_allocator_for_thirdparty_libraries, &rf_global_io_for_thirdparty_libraries);

    if (contents.valid)
    {
        *buf = (char*) contents.data;
        *len = contents.size;
    }
}