#include "rayfork/foundation/allocator.h"
#include "rayfork/foundation/io.h"
#include "rayfork/internal/thirdparty/cgltf-utils.h"

#define CGLTF_IMPLEMENTATION
#define CGLTF_MALLOC(size) rf_alloc(rf_global_allocator_for_thirdparty_libraries, size)
#define CGLTF_FREE(ptr)    rf_free(rf_global_allocator_for_thirdparty_libraries, ptr)
#include "cgltf.h"

rf_extern cgltf_result rf_cgltf_io_read(const cgltf_memory_options* memory_options, const cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data)
{
    cgltf_result result = cgltf_result_file_not_found;
    rf_allocator* allocator = memory_options->user_data;
    rf_io_callbacks* io = file_options->user_data;
    rf_file_contents file_contents = rf_read_entire_file(path, rf_global_allocator_for_thirdparty_libraries, io);

    if (file_contents.valid)
    {
        *data = file_contents.data;
        *size = file_contents.size;
        result = cgltf_result_success;
    }

    return result;
}

rf_extern void rf_cgltf_io_release(const cgltf_memory_options* memory_options, const cgltf_file_options* file_options, void* data)
{
    ((void) file_options);

    rf_free(*(rf_allocator*)memory_options->user_data, (data));
}

rf_extern void* rf_cgltf_alloc(void* user, cgltf_size size)
{
    rf_allocator* allocator = (rf_allocator*) user;

    void* result = rf_alloc(*allocator, size);

    return result;
}

rf_extern void rf_cgltf_free(void* user, void* ptr)
{
    rf_allocator* allocator = (rf_allocator*) user;
    rf_free(*allocator, ptr);
}