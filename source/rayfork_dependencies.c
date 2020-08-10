#pragma region stb_image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)                     RF_ALLOC(rf__global_allocator_for_dependencies, sz)
#define STBI_FREE(p)                        RF_FREE(rf__global_allocator_for_dependencies, p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) rf_realloc_wrapper(rf__global_allocator_for_dependencies, p, oldsz, newsz)
#define STBI_ASSERT(it)                     RF_ASSERT(it)
#define STBIDEF                             RF_INTERNAL
#include "stb_image.h"
#pragma endregion

#pragma region stb_image_resize
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(sz,c)   ((void)(c), RF_ALLOC(rf__global_allocator_for_dependencies, sz))
#define STBIR_FREE(p,c)      ((void)(c), RF_FREE(rf__global_allocator_for_dependencies, p))
#define STBIR_ASSERT(it)     RF_ASSERT(it)
#define STBIRDEF RF_INTERNAL
#include "stb_image_resize.h"
#pragma endregion

#pragma region stb_rect_pack
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_ASSERT RF_ASSERT
#define STBRP_STATIC
#include "stb_rect_pack.h"
#pragma endregion

#pragma region stb_truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(sz, u) RF_ALLOC(rf__global_allocator_for_dependencies, sz)
#define STBTT_free(p, u)    RF_FREE(rf__global_allocator_for_dependencies, p)
#define STBTT_assert(it)    RF_ASSERT(it)
#define STBTT_STATIC
#include "stb_truetype.h"
#pragma endregion

#pragma region stb_perlin
#define STB_PERLIN_IMPLEMENTATION
#define STBPDEF RF_INTERNAL
#include "stb_perlin.h"
#pragma endregion

#pragma region par shapes
#define PAR_SHAPES_IMPLEMENTATION
#define PAR_MALLOC(T, N)               ((T*)RF_ALLOC(rf__global_allocator_for_dependencies, N * sizeof(T)))
#define PAR_CALLOC(T, N)               ((T*)rf_calloc_wrapper(rf__global_allocator_for_dependencies, N, sizeof(T)))
#define PAR_FREE(BUF)                  (RF_FREE(rf__global_allocator_for_dependencies, BUF))
#define PAR_REALLOC(T, BUF, N, OLD_SZ) ((T*) rf_realloc_wrapper(rf__global_allocator_for_dependencies, BUF, sizeof(T) * (N), (OLD_SZ)))
#define PARDEF                         RF_INTERNAL
#include "par_shapes.h"
#pragma endregion

#pragma region tinyobj loader
RF_INTERNAL RF_THREAD_LOCAL rf_io_callbacks rf__tinyobj_io;
#define RF_SET_TINYOBJ_ALLOCATOR(allocator) rf__tinyobj_allocator = allocator
#define RF_SET_TINYOBJ_IO_CALLBACKS(io) rf__tinyobj_io = io;

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#define TINYOBJ_MALLOC(size)             (RF_ALLOC(rf__global_allocator_for_dependencies, (size)))
#define TINYOBJ_REALLOC(p, oldsz, newsz) (rf_realloc_wrapper(rf__global_allocator_for_dependencies, (p), (oldsz), (newsz)))
#define TINYOBJ_CALLOC(amount, size)     (rf_calloc_wrapper(rf__global_allocator_for_dependencies, (amount), (size)))
#define TINYOBJ_FREE(p)                  (RF_FREE(rf__global_allocator_for_dependencies, (p)))
#define TINYOBJDEF                       RF_INTERNAL
#include "tinyobjloader.h"

RF_INTERNAL void rf_tinyobj_file_reader_callback(const char* filename, char** buf, size_t* len)
{
    if (!filename || !buf || !len) return;

    *len = RF_FILE_SIZE(rf__tinyobj_io, filename);

    if (*len)
    {
        if (!RF_READ_FILE(rf__tinyobj_io, filename, *buf, *len))
        {
            // On error we set the size of output buffer to 0
            *len = 0;
        }
    }
}
#pragma endregion

#pragma region cgltf
#define CGLTF_IMPLEMENTATION
#define CGLTF_MALLOC(size) RF_ALLOC(rf__global_allocator_for_dependencies, size)
#define CGLTF_FREE(ptr)    RF_FREE(rf__global_allocator_for_dependencies, ptr)
#include "cgltf.h"

RF_INTERNAL cgltf_result rf_cgltf_io_read(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data)
{
    ((void) memory_options);
    ((void) file_options);

    cgltf_result result = cgltf_result_file_not_found;
    rf_io_callbacks* io = (rf_io_callbacks*) file_options->user_data;

    int file_size = RF_FILE_SIZE(*io, path);

    if (file_size > 0)
    {
        void* dst = CGLTF_MALLOC(file_size);

        if (dst == NULL)
        {
            if (RF_READ_FILE(*io, path, data, file_size) && data && size)
            {
                *data = dst;
                *size = file_size;
                result = cgltf_result_success;
            }
            else
            {
                CGLTF_FREE(dst);
                result = cgltf_result_io_error;
            }
        }
        else
        {
            result = cgltf_result_out_of_memory;
        }
    }

    return result;
}

RF_INTERNAL void rf_cgltf_io_release(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, void* data)
{
    ((void) memory_options);
    ((void) file_options);

    CGLTF_FREE(data);
}
#pragma endregion