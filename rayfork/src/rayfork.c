#include "rayfork.h"
#include "math.h"

//region internal

// Prefix for all the private functions
#define RF_INTERNAL static

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN 1024
#endif

#ifndef RF_ASSERT
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#endif

#define RF_INTERNAL_STRINGS_MATCH(a, a_len, b, b_len) ((a_len) == (b_len) && (strncmp((a), (b), (a_len)) == 0))

RF_INTERNAL void* rf_internal_realloc_wrapper(rf_allocator allocator, void* source, int old_size, int new_size)
{
    void* new_alloc = RF_ALLOC(allocator, new_size);
    if (new_alloc && source && old_size) { memcpy(new_alloc, source, old_size); }
    if (source) { RF_FREE(allocator, source); }
    return new_alloc;
}

RF_INTERNAL void* rf_internal_calloc_wrapper(rf_allocator allocator, int amount, int size)
{
    void* ptr = RF_ALLOC(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

RF_INTERNAL bool rf_internal_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return false;
    }

    return RF_INTERNAL_STRINGS_MATCH(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

// String pointer reverse break: returns right-most occurrence of charset in s
RF_INTERNAL const char* rf_internal_strprbrk(const char* s, const char* charset)
{
    const char* latestMatch = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) { }
    return latestMatch;
}

RF_INTERNAL RF_THREAD_LOCAL char rf_internal_dir_path[RF_MAX_FILEPATH_LEN];

// Get directory for a given filePath
RF_INTERNAL const char* rf_internal_get_directory_path(const char* filePath)
{
    const char* last_slash = NULL;
    memset(rf_internal_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = rf_internal_strprbrk(filePath, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(rf_internal_dir_path, filePath, strlen(filePath) - (strlen(last_slash) - 1));
    rf_internal_dir_path[strlen(filePath) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return rf_internal_dir_path;
}

#define RF_MIN(a, b) ((a) < (b) ? (a) : (b))
#define RF_MAX(a, b) ((a) > (b) ? (a) : (b))

RF_INTERNAL const unsigned char rf_internal_base64_table[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 62, 0, 0, 0, 63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 0, 0, 0, 0, 0, 0, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51
};

// Global pointer to context struct
RF_INTERNAL rf_context* rf_internal_ctx;

//endregion

//region dependencies
//region stb_image

//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf_internal_stbi_allocator;

#define RF_SET_STBI_ALLOCATOR(allocator) rf_internal_stbi_allocator = (allocator)

//#define STBI_NO_GIF
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)                     RF_ALLOC(rf_internal_stbi_allocator, sz)
#define STBI_FREE(p)                        RF_FREE(rf_internal_stbi_allocator, p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) rf_internal_realloc_wrapper(rf_internal_stbi_allocator, p, oldsz, newsz)
#define STBI_ASSERT(it)                     RF_ASSERT(it)
#define STBIDEF                             RF_INTERNAL
#include "stb/stb_image.h"
//endregion

//region stb_image_resize

//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf_internal_stbir_allocator;

#define RF_SET_STBIR_ALLOCATOR(allocator) rf_internal_stbir_allocator = (allocator)

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(sz,c)   ((void)(c), RF_ALLOC(rf_internal_stbir_allocator, sz))
#define STBIR_FREE(p,c)      ((void)(c), RF_FREE(rf_internal_stbir_allocator, p))
#define STBIR_ASSERT(it)     RF_ASSERT(it)
#define STBIRDEF             RF_INTERNAL
#include "stb/stb_image_resize.h"
//endregion

//region stb_rect_pack
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT RF_ASSERT
#include "stb/stb_rect_pack.h"
//endregion

//region stb_truetype
//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf_internal_stbtt_allocator;

#define RF_SET_STBTT_ALLOCATOR(allocator) rf_internal_stbtt_allocator = (allocator)

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_malloc(sz, u) RF_ALLOC(rf_internal_stbtt_allocator, sz)
#define STBTT_free(p, u)    RF_FREE(rf_internal_stbtt_allocator, p)
#define STBTT_assert(it)    RF_ASSERT(it)
#include "stb/stb_truetype.h"
//endregion

//region stb_perlin
#define STB_PERLIN_IMPLEMENTATION
#include "stb/stb_perlin.h"
//endregion

//region par shapes
//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf_internal_par_allocator;

#define RF_SET_PARSHAPES_ALLOCATOR(allocator) rf_internal_par_allocator = (allocator)

#define PAR_SHAPES_IMPLEMENTATION
#define PAR_MALLOC(T, N)                    ((T*)RF_ALLOC(rf_internal_par_allocator, N * sizeof(T)))
#define PAR_CALLOC(T, N)                    ((T*)rf_internal_calloc_wrapper(rf_internal_par_allocator, N, sizeof(T)))
#define PAR_FREE(BUF)                       RF_FREE(rf_internal_par_allocator, BUF)
#define PAR_REALLOC(T, BUF, N, OLD_SZ)      ((T*) rf_internal_realloc_wrapper(rf_internal_par_allocator, BUF, sizeof(T) * (N), OLD_SZ))

#include "par/par_shapes.h"
//endregion

//region tinyobj loader
//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf_internal_tinyobj_allocator;
RF_INTERNAL RF_THREAD_LOCAL rf_io_callbacks rf_tinyobj_io;

#define RF_SET_TINYOBJ_ALLOCATOR(allocator) rf_internal_tinyobj_allocator = allocator
#define RF_SET_TINYOBJ_IO_CALLBACKS(io) rf_tinyobj_io = io;

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#define TINYOBJ_MALLOC(size)             RF_ALLOC(rf_internal_tinyobj_allocator, size)
#define TINYOBJ_REALLOC(p, oldsz, newsz) rf_internal_realloc_wrapper(rf_internal_tinyobj_allocator, p, oldsz, newsz)
#define TINYOBJ_CALLOC(amount, size)     rf_internal_calloc_wrapper(rf_internal_tinyobj_allocator, amount, size)
#define TINYOBJ_FREE(p)                  RF_FREE(rf_internal_tinyobj_allocator, p)

#define TINYOBJ_GET_FILE_SIZE(filename) (rf_tinyobj_io.get_file_size_proc(filename))
#define TINYOBJ_LOAD_FILE_IN_BUFFER(filename, buffer, buffer_size) (rf_tinyobj_io.read_file_into_buffer_proc(filename, buffer, buffer_size))

#include "tinyobjloader-c/tinyobj_loader_c.h"
//endregion

//region cgltf
RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf_internal_cgltf_allocator;

#define RF_SET_CGLTF_ALLOCATOR(allocator) rf_internal_cgltf_allocator = allocator

#define CGLTF_IMPLEMENTATION
#define CGLTF_MALLOC(size) RF_ALLOC(rf_internal_cgltf_allocator, size)
#define CGLTF_FREE(ptr)    RF_FREE(rf_internal_cgltf_allocator, ptr)

#include "cgltf/cgltf.h"

cgltf_result rf_internal_cgltf_io_read(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data)
{
    ((void) memory_options);
    ((void) file_options);

    rf_io_callbacks* io = (rf_io_callbacks*) file_options->user_data;

    int file_size = io->get_file_size_proc(path);

    if (file_size == 0)
    {
        return cgltf_result_file_not_found;
    }

    *data = CGLTF_MALLOC(file_size);

    if (data == NULL)
    {
        return cgltf_result_out_of_memory;
    }

    if (!io->read_file_into_buffer_proc(path, *data, file_size))
    {
        CGLTF_FREE(*data);
        return cgltf_result_io_error;
    }

    return cgltf_result_success;
}

void rf_internal_cgltf_io_release(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, void* data)
{
    ((void) memory_options);
    ((void) file_options);

    CGLTF_FREE(data);
}
//endregion
//endregion

//region init and setup
// Set viewport for a provided width and height
RF_API void rf_setup_viewport(int width, int height)
{
    rf_internal_ctx->render_width  = width;
    rf_internal_ctx->render_height = height;

    // Set viewport width and height
    // NOTE: We consider render size and offset in case black bars are required and
    // render area does not match full global_display area (this situation is only applicable on fullscreen mode)
    rf_gfx_viewport(rf_internal_ctx->render_offset_x/2, rf_internal_ctx->render_offset_y/2, rf_internal_ctx->render_width - rf_internal_ctx->render_offset_x, rf_internal_ctx->render_height - rf_internal_ctx->render_offset_y);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size
    // NOTE: Confirf_gfx_projectiongured top-left corner as (0, 0)
    rf_gfx_ortho(0, rf_internal_ctx->render_width, rf_internal_ctx->render_height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}

// Define default texture used to draw shapes
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source)
{
    rf_internal_ctx->tex_shapes = texture;
    rf_internal_ctx->rec_tex_shapes = source;
}

// Load the raylib default font
RF_API void rf_load_default_font(rf_allocator allocator, rf_allocator temp_allocator)
{
    // NOTE: Using UTF8 encoding table for Unicode U+0000..U+00FF Basic Latin + Latin-1 Supplement
    // http://www.utf8-chartable.de/unicode-utf8-table.pl

    rf_internal_ctx->default_font.chars_count = 224; // Number of chars included in our default font

    // Default font is directly defined here (data generated from a sprite font image)
    // This way, we reconstruct rf_font without creating large global variables
    // This data is automatically allocated to Stack and automatically deallocated at the end of this function
    unsigned int default_font_data[512] = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
        0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
        0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
        0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
        0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
        0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
        0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
        0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
        0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
        0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
        0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
        0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000f01, 0x00000000, 0x06000000, 0x24000000, 0x00000f01, 0x00000000, 0x09108000,
        0x24fa28a2, 0x00000f01, 0x00000000, 0x013e0000, 0x2242252a, 0x00000f52, 0x00000000, 0x038a8000, 0x2422222a, 0x00000f29, 0x00000000, 0x010a8000,
        0x2412252a, 0x00000f01, 0x00000000, 0x010a8000, 0x24fbe8be, 0x00000f01, 0x00000000, 0x0ebe8000, 0xac020000, 0x00000f01, 0x00000000, 0x00048000,
        0x0003e000, 0x00000f00, 0x00000000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000038, 0x8443b80e, 0x00203a03,
        0x02bea080, 0xf0000020, 0xc452208a, 0x04202b02, 0xf8029122, 0x07f0003b, 0xe44b388e, 0x02203a02, 0x081e8a1c, 0x0411e92a, 0xf4420be0, 0x01248202,
        0xe8140414, 0x05d104ba, 0xe7c3b880, 0x00893a0a, 0x283c0e1c, 0x04500902, 0xc4400080, 0x00448002, 0xe8208422, 0x04500002, 0x80400000, 0x05200002,
        0x083e8e00, 0x04100002, 0x804003e0, 0x07000042, 0xf8008400, 0x07f00003, 0x80400000, 0x04000022, 0x00000000, 0x00000000, 0x80400000, 0x04000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800702, 0x1848a0c2, 0x84010000, 0x02920921, 0x01042642, 0x00005121, 0x42023f7f, 0x00291002,
        0xefc01422, 0x7efdfbf7, 0xefdfa109, 0x03bbbbf7, 0x28440f12, 0x42850a14, 0x20408109, 0x01111010, 0x28440408, 0x42850a14, 0x2040817f, 0x01111010,
        0xefc78204, 0x7efdfbf7, 0xe7cf8109, 0x011111f3, 0x2850a932, 0x42850a14, 0x2040a109, 0x01111010, 0x2850b840, 0x42850a14, 0xefdfbf79, 0x03bbbbf7,
        0x001fa020, 0x00000000, 0x00001000, 0x00000000, 0x00002070, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x08022800, 0x00012283, 0x02430802, 0x01010001, 0x8404147c, 0x20000144, 0x80048404, 0x00823f08, 0xdfbf4284, 0x7e03f7ef, 0x142850a1, 0x0000210a,
        0x50a14684, 0x528a1428, 0x142850a1, 0x03efa17a, 0x50a14a9e, 0x52521428, 0x142850a1, 0x02081f4a, 0x50a15284, 0x4a221428, 0xf42850a1, 0x03efa14b,
        0x50a16284, 0x4a521428, 0x042850a1, 0x0228a17a, 0xdfbf427c, 0x7e8bf7ef, 0xf7efdfbf, 0x03efbd0b, 0x00000000, 0x04000000, 0x00000000, 0x00000008,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200508, 0x00840400, 0x11458122, 0x00014210,
        0x00514294, 0x51420800, 0x20a22a94, 0x0050a508, 0x00200000, 0x00000000, 0x00050000, 0x08000000, 0xfefbefbe, 0xfbefbefb, 0xfbeb9114, 0x00fbefbe,
        0x20820820, 0x8a28a20a, 0x8a289114, 0x3e8a28a2, 0xfefbefbe, 0xfbefbe0b, 0x8a289114, 0x008a28a2, 0x228a28a2, 0x08208208, 0x8a289114, 0x088a28a2,
        0xfefbefbe, 0xfbefbefb, 0xfa2f9114, 0x00fbefbe, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00210100, 0x00000004, 0x00000000, 0x00000000, 0x14508200, 0x00001402, 0x00000000, 0x00000000,
        0x00000010, 0x00000020, 0x00000000, 0x00000000, 0xa28a28be, 0x00002228, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000,
        0xa28a28aa, 0x000022a8, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000, 0xbefbefbe, 0x00003e2f, 0x00000000, 0x00000000,
        0x00000004, 0x00002028, 0x00000000, 0x00000000, 0x80000000, 0x00003e0f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    int chars_height  = 10;
    int chars_divisor = 1; // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

    int chars_width[224] = {
        3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
        7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
        2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 5, 5, 5, 7, 1, 5, 3, 7, 3, 5, 4, 1, 7, 4, 3, 5, 3, 3, 2, 5, 6, 1, 2, 2, 3, 5, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 3, 3, 3, 3, 7, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 6,
        5, 5, 5, 5, 5, 5, 9, 5, 5, 5, 5, 5, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5
    };

    // Re-construct image from rf_internal_ctx->default_font_data and generate OpenGL texture
    //----------------------------------------------------------------------
    const int im_width = 128;
    const int im_height = 128;

    rf_color image_pixels[128 * 128] = {0};

    int counter = 0; // rf_font data elements counter

    // Fill img_data with rf_internal_ctx->default_font_data (convert from bit to pixel!)
    for (int i = 0; i < im_width * im_height; i += 32)
    {
        for (int j = 31; j >= 0; j--)
        {
            const int bit_check = (default_font_data[counter]) & (1u << j);
            if (bit_check) image_pixels[i + j] = RF_WHITE;
        }

        counter++;

        if (counter > 512) counter = 0; // Security check...
    }

    bool format_success = rf_format_pixels(image_pixels, 128 * 128 * sizeof(rf_color), RF_UNCOMPRESSED_R8G8B8A8,
                     rf_internal_ctx->default_font_buffers->pixels, 128 * 128 * 2, RF_UNCOMPRESSED_GRAY_ALPHA);
    RF_ASSERT(format_success);

    rf_image im_font = { .data = rf_internal_ctx->default_font_buffers->pixels, .format = RF_UNCOMPRESSED_GRAY_ALPHA, .width = 128, .height = 128 };

    rf_internal_ctx->default_font.texture = rf_load_texture_from_image(im_font);

    // Allocate space for our characters info data
    rf_internal_ctx->default_font.chars = rf_internal_ctx->default_font_buffers->chars;
    rf_internal_ctx->default_font.recs  = rf_internal_ctx->default_font_buffers->recs;

    int current_line     = 0;
    int current_pos_x    = chars_divisor;
    int test_pos_x       = chars_divisor;
    int char_pixels_iter = 0;

    for (int i = 0; i < rf_internal_ctx->default_font.chars_count; i++)
    {
        rf_internal_ctx->default_font.chars[i].value = 32 + i; // First char is 32

        rf_internal_ctx->default_font.recs[i].x      = (float) current_pos_x;
        rf_internal_ctx->default_font.recs[i].y      = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
        rf_internal_ctx->default_font.recs[i].width  = (float) chars_width[i];
        rf_internal_ctx->default_font.recs[i].height = (float) chars_height;

        test_pos_x += (int) (rf_internal_ctx->default_font.recs[i].width + (float)chars_divisor);

        if (test_pos_x >= rf_internal_ctx->default_font.texture.width)
        {
            current_line++;
            current_pos_x = 2 * chars_divisor + chars_width[i];
            test_pos_x = current_pos_x;

            rf_internal_ctx->default_font.recs[i].x = (float) (chars_divisor);
            rf_internal_ctx->default_font.recs[i].y = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
        }
        else current_pos_x = test_pos_x;

        // NOTE: On default font character offsets and xAdvance are not required
        rf_internal_ctx->default_font.chars[i].offset_x = 0;
        rf_internal_ctx->default_font.chars[i].offset_y = 0;
        rf_internal_ctx->default_font.chars[i].advance_x = 0;

        // Fill character image data
        {
            rf_rec   char_img_rec    = rf_internal_ctx->default_font_buffers->recs[i];
            int      char_img_width  = char_img_rec.width;
            int      char_img_height = char_img_rec.height;
            int      char_img_size   = rf_pixel_buffer_size(im_font.format, char_img_width, char_img_height);
            void*    char_img_pixels = rf_internal_ctx->default_font_buffers->chars_pixels + char_pixels_iter;
            rf_image char_img        = rf_image_crop_to_buffer(im_font, rf_internal_ctx->default_font.recs[i], char_img_pixels, char_img_size);

            rf_internal_ctx->default_font.chars[i].image = char_img;
            char_pixels_iter += char_img_size;
        }
    }

    rf_internal_ctx->default_font.base_size = (int)rf_internal_ctx->default_font.recs[0].height;

    RF_LOG_V(RF_LOG_TYPE_INFO, "[TEX ID %i] Default font loaded successfully", rf_internal_ctx->default_font.texture.id);
}

// Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
RF_API rf_material rf_load_default_material(rf_allocator allocator)
{
    rf_material material = {0};
    material.maps = (rf_material_map*) RF_ALLOC(allocator, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));
    memset(material.maps, 0, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));

    material.shader = rf_get_default_shader();
    material.maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // White texture (1x1 pixel)
    //material.maps[RF_MAP_NORMAL].texture;         // NOTE: By default, not set
    //material.maps[RF_MAP_SPECULAR].texture;       // NOTE: By default, not set

    material.maps[RF_MAP_DIFFUSE].color = RF_WHITE; // Diffuse color
    material.maps[RF_MAP_SPECULAR].color = RF_WHITE; // Specular color

    return material;
}
//endregion

//region time functions

//Used to set custom functions
RF_API void rf_set_time_functions(void (*wait_proc)(float), double (*get_time_proc)(void))
{
    rf_internal_ctx->wait_proc = wait_proc;
    rf_internal_ctx->get_time_proc = get_time_proc;
}

// If the user disabled the default time functions implementation or we are compiling for a platform that does not have a default implementation for the time functions
#if defined(RF_NO_DEFAULT_TIME) || (!defined(_WIN32) && !defined(__linux__) && !defined(__MACH__))
    // Wait for some milliseconds (pauses program execution)
    RF_API void rf_wait(float it)
    {
        if (rf_internal_ctx->wait_proc)
        {
            rf_internal_ctx->wait_proc(it);
        }
    }

    // Returns elapsed time in seconds since rf_context_init
    RF_API double rf_get_time(void)
    {
        if (rf_internal_ctx->get_time_proc)
        {
            return rf_internal_ctx->get_time_proc();
        }

        return 0;
    }

#else //#if !defined(RF_NO_DEFAULT_TIME)
    //Windows only
    #ifdef _WIN32
        RF_INTERNAL long long int rf_internal_global_performance_counter_frequency;
        RF_INTERNAL bool rf_internal_global_performance_counter_frequency_initialised;

        //If windows.h is not included
        #if !defined(_WINDOWS_)
            //Definition so that we don't have to include windows.h
            #ifdef __cplusplus
                extern "C" int __stdcall QueryPerformanceCounter(long long int* lpPerformanceCount);
                extern "C" int __stdcall QueryPerformanceFrequency(long long int* lpFrequency);
                extern "C" void __stdcall Sleep(int dwMilliseconds);
            #else
                extern int __stdcall QueryPerformanceCounter(long long int* lpPerformanceCount);
                extern int __stdcall QueryPerformanceFrequency(long long int* lpFrequency);
                extern void __stdcall Sleep(int dwMilliseconds);
            #endif
        #endif

        // Returns elapsed time in seconds since program started
        RF_API double rf_get_time(void)
        {
            if (rf_internal_ctx->get_time_proc)
            {
                return rf_internal_ctx->get_time_proc();
            }

            if (!rf_internal_global_performance_counter_frequency_initialised)
            {
                rf_internal_global_performance_counter_frequency_initialised = true;
            }

            long long int qpc_result = {0};

            return (double) qpc_result / (double) rf_internal_global_performance_counter_frequency;
        }

        RF_API void rf_wait(float duration)
        {
            if (rf_internal_ctx->wait_proc)
            {
                rf_internal_ctx->wait_proc(duration);
                return;
            }

            Sleep((int) duration);
        }
    #endif

    #if defined(__linux__)
        #include <time.h>

        //Source: http://man7.org/linux/man-pages/man2/clock_gettime.2.html
        RF_API double rf_get_time(void)
        {
            if (rf_internal_ctx->get_time_proc)
            {
                return rf_internal_ctx->get_time_proc();
            }

            struct timespec result;

            RF_ASSERT(clock_gettime(CLOCK_MONOTONIC_RAW, &result) == 0);

            return (double) result.tv_sec;
        }

        RF_API void rf_wait(float duration)
        {
            if (rf_internal_ctx->wait_proc)
            {
                rf_internal_ctx->wait_proc(duration);
                return;
            }

            long milliseconds = (long) duration;
            struct timespec ts;
            ts.tv_sec = milliseconds / 1000;
            ts.tv_nsec = (milliseconds % 1000) * 1000000;
            nanosleep(&ts, NULL);
        }
    #endif //#elif defined(__linux__)

    #if defined(__MACH__)
        #include <mach/mach_time.h>
        #include <unistd.h>

        RF_INTERNAL bool rf_internal_global_mach_time_initialized;
        RF_INTERNAL uint64_t rf_internal_global_mach_time_start;
        RF_INTERNAL double rf_internal_global_mach_time_seconds_factor;

        RF_API double rf_get_time(void)
        {
            if (rf_internal_ctx->get_time_proc)
            {
                return rf_internal_ctx->get_time_proc();
            }

            uint64_t time;
            if (!rf_internal_global_mach_time_initialized)
            {
                mach_timebase_info_data_t timebase;
                mach_timebase_info(&timebase);
                rf_internal_global_mach_time_seconds_factor = 1e-9 * (double)timebase.numer / (double)timebase.denom;
                rf_internal_global_mach_time_start = mach_absolute_time();
                rf_internal_global_mach_time_initialized = true;
            }
            time = mach_absolute_time();
            return (double)(time - rf_internal_global_mach_time_start) * rf_internal_global_mach_time_seconds_factor;
        }

        RF_API void rf_wait(float duration)
        {
            if (rf_internal_ctx->wait_proc)
            {
                rf_internal_ctx->wait_proc(duration);
                return;
            }

            usleep(duration * 1000);
        }
    #endif //#if defined(__MACH__)
#endif //if !defined(RF_CUSTOM_TIME)
//endregion

//region default io & allocator
#include "malloc.h"

void* rf_malloc_wrapper(rf_allocator_mode mode, int size_to_alloc, void* pointer_to_free, void* user_data)
{
    ((void)user_data);

    switch (mode)
    {
        case RF_AM_ALLOC: return malloc(size_to_alloc);

        case RF_AM_FREE:
        {
            free(pointer_to_free);
        }
        break;
    }

    return NULL;
}

#if !defined(RF_NO_DEFAULT_IO)
#include "stdio.h"

//Get the size of the file
RF_API int rf_get_file_size(const char* filename)
{
    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

//Load the file into a buffer
RF_API bool rf_load_file_into_buffer(const char* filename, void* buffer, int buffer_size)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return false;

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (file_size > buffer_size) return false;

    size_t read_size = fread(buffer, sizeof(char), buffer_size, file);

    if (ferror(file) != 0) return false;
    if (read_size != file_size) return false;

    fclose(file);

    return true;
}
#endif
//endregion

//region getters
// Returns time in seconds for last frame drawn
RF_API float rf_get_frame_time()
{
    return (float) rf_internal_ctx->frame_time;
}

// Returns current FPS
RF_API int rf_get_fps()
{
    return (int) roundf(1.0f / rf_get_frame_time());
}

// Get the default font, useful to be used with extended parameters
RF_API rf_font rf_get_default_font()
{
    return rf_internal_ctx->default_font;
}

// Get default shader
RF_API rf_shader rf_get_default_shader()
{
    return rf_internal_ctx->gfx_ctx.default_shader;
}

// Get default internal texture (white texture)
RF_API rf_texture2d rf_get_default_texture()
{
    rf_texture2d texture = {0};
    texture.id = rf_internal_ctx->gfx_ctx.default_texture_id;
    texture.width = 1;
    texture.height = 1;
    texture.mipmaps = 1;
    texture.format = RF_UNCOMPRESSED_R8G8B8A8;

    return texture;
}

//Get the context pointer
RF_API rf_context* rf_get_context()
{
    return rf_internal_ctx;
}

// Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
RF_API rf_image rf_get_screen_data(rf_color* dst, int dst_count)
{
    rf_image image = {0};

    if (dst_count == rf_internal_ctx->screen_width * rf_internal_ctx->screen_height)
    {
        rf_gfx_read_screen_pixels(dst, rf_internal_ctx->screen_width, rf_internal_ctx->screen_height);

        image.data   = dst;
        image.width  = rf_internal_ctx->screen_width;
        image.height = rf_internal_ctx->screen_height;
        image.format = RF_UNCOMPRESSED_R8G8B8A8;
        image.valid  = true;
    }

    return image;
}
//endregion

//region setters
// Set the global context pointer
RF_API void rf_set_global_context_pointer(rf_context* ctx)
{
    rf_internal_ctx = ctx;
}

// Set viewport for a provided width and height
RF_API void rf_set_viewport(int width, int height)
{
    rf_internal_ctx->render_width = width;
    rf_internal_ctx->render_height = height;

    // Set viewport width and height
    // NOTE: We consider render size and offset in case black bars are required and
    // render area does not match full global_display area (this situation is only applicable on fullscreen mode)
    rf_gfx_viewport(rf_internal_ctx->render_offset_x/2, rf_internal_ctx->render_offset_y/2, rf_internal_ctx->render_width - rf_internal_ctx->render_offset_x, rf_internal_ctx->render_height - rf_internal_ctx->render_offset_y);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size
    // NOTE: Confirf_gfx_projectiongured top-left corner as (0, 0)
    rf_gfx_ortho(0, rf_internal_ctx->render_width, rf_internal_ctx->render_height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}

// Set target FPS (maximum)
RF_API void rf_set_target_fps(int fps)
{
    if (fps < 1) rf_internal_ctx->target_time = RF_UNLOCKED_FPS;
    else rf_internal_ctx->target_time = 1.0 / ((double) fps);

    RF_LOG_V(RF_LOG_TYPE_INFO, "Target time per frame: %02.03f milliseconds", (float) rf_internal_global_context_ptr->target_time * 1000);
}
//endregion

//region math
#include "rayfork_math.inc"
//endregion

//region drawing
#include "rayfork_drawing.inc"
//endregion

//region pixel format
RF_API const char* rf_pixel_format_string(rf_pixel_format format)
{
    switch (format)
    {
        case RF_UNCOMPRESSED_GRAYSCALE: return "RF_UNCOMPRESSED_GRAYSCALE";
        case RF_UNCOMPRESSED_GRAY_ALPHA: return "RF_UNCOMPRESSED_GRAY_ALPHA";
        case RF_UNCOMPRESSED_R5G6B5: return "RF_UNCOMPRESSED_R5G6B5";
        case RF_UNCOMPRESSED_R8G8B8: return "RF_UNCOMPRESSED_R8G8B8";
        case RF_UNCOMPRESSED_R5G5B5A1: return "RF_UNCOMPRESSED_R5G5B5A1";
        case RF_UNCOMPRESSED_R4G4B4A4: return "RF_UNCOMPRESSED_R4G4B4A4";
        case RF_UNCOMPRESSED_R8G8B8A8: return "RF_UNCOMPRESSED_R8G8B8A8";
        case RF_UNCOMPRESSED_R32: return "RF_UNCOMPRESSED_R32";
        case RF_UNCOMPRESSED_R32G32B32: return "RF_UNCOMPRESSED_R32G32B32";
        case RF_UNCOMPRESSED_R32G32B32A32: return "RF_UNCOMPRESSED_R32G32B32A32";
        case RF_COMPRESSED_DXT1_RGB: return "RF_COMPRESSED_DXT1_RGB";
        case RF_COMPRESSED_DXT1_RGBA: return "RF_COMPRESSED_DXT1_RGBA";
        case RF_COMPRESSED_DXT3_RGBA: return "RF_COMPRESSED_DXT3_RGBA";
        case RF_COMPRESSED_DXT5_RGBA: return "RF_COMPRESSED_DXT5_RGBA";
        case RF_COMPRESSED_ETC1_RGB: return "RF_COMPRESSED_ETC1_RGB";
        case RF_COMPRESSED_ETC2_RGB: return "RF_COMPRESSED_ETC2_RGB";
        case RF_COMPRESSED_ETC2_EAC_RGBA: return "RF_COMPRESSED_ETC2_EAC_RGBA";
        case RF_COMPRESSED_PVRT_RGB: return "RF_COMPRESSED_PVRT_RGB";
        case RF_COMPRESSED_PVRT_RGBA: return "RF_COMPRESSED_PVRT_RGBA";
        case RF_COMPRESSED_ASTC_4x4_RGBA: return "RF_COMPRESSED_ASTC_4x4_RGBA";
        case RF_COMPRESSED_ASTC_8x8_RGBA: return "RF_COMPRESSED_ASTC_8x8_RGBA";
        default: return NULL;
    }
}

RF_API bool rf_is_uncompressed_format(rf_pixel_format format)
{
    return format >= RF_UNCOMPRESSED_GRAYSCALE && format <= RF_UNCOMPRESSED_R32G32B32A32;
}

RF_API bool rf_is_compressed_format(rf_pixel_format format)
{
    return format >= RF_COMPRESSED_DXT1_RGB && format <= RF_COMPRESSED_ASTC_8x8_RGBA;
}

RF_API int rf_bits_per_pixel(rf_pixel_format format)
{
    switch (format)
    {
        case RF_UNCOMPRESSED_GRAYSCALE: return 8; // 8 bit per pixel (no alpha)
        case RF_UNCOMPRESSED_GRAY_ALPHA: return 8 * 2; // 8 * 2 bpp (2 channels)
        case RF_UNCOMPRESSED_R5G6B5: return 16; // 16 bpp
        case RF_UNCOMPRESSED_R8G8B8: return 24; // 24 bpp
        case RF_UNCOMPRESSED_R5G5B5A1: return 16; // 16 bpp (1 bit alpha)
        case RF_UNCOMPRESSED_R4G4B4A4: return 16; // 16 bpp (4 bit alpha)
        case RF_UNCOMPRESSED_R8G8B8A8: return 32; // 32 bpp
        case RF_UNCOMPRESSED_R32: return 32; // 32 bpp (1 channel - float)
        case RF_UNCOMPRESSED_R32G32B32: return 32 * 3; // 32 * 3 bpp (3 channels - float)
        case RF_UNCOMPRESSED_R32G32B32A32: return 32 * 4; // 32 * 4 bpp (4 channels - float)
        case RF_COMPRESSED_DXT1_RGB: return 4; // 4 bpp (no alpha)
        case RF_COMPRESSED_DXT1_RGBA: return 4; // 4 bpp (1 bit alpha)
        case RF_COMPRESSED_DXT3_RGBA: return 8; // 8 bpp
        case RF_COMPRESSED_DXT5_RGBA: return 8; // 8 bpp
        case RF_COMPRESSED_ETC1_RGB: return 4; // 4 bpp
        case RF_COMPRESSED_ETC2_RGB: return 4; // 4 bpp
        case RF_COMPRESSED_ETC2_EAC_RGBA: return 8; // 8 bpp
        case RF_COMPRESSED_PVRT_RGB: return 4; // 4 bpp
        case RF_COMPRESSED_PVRT_RGBA: return 4; // 4 bpp
        case RF_COMPRESSED_ASTC_4x4_RGBA: return 8; // 8 bpp
        case RF_COMPRESSED_ASTC_8x8_RGBA: return 2; // 2 bpp
        default: return 0;
    }
}

RF_API int rf_bytes_per_pixel(rf_uncompressed_pixel_format format)
{
    switch (format)
    {
        case RF_UNCOMPRESSED_GRAYSCALE: return 1;
        case RF_UNCOMPRESSED_GRAY_ALPHA: return 2;
        case RF_UNCOMPRESSED_R5G5B5A1: return 2;
        case RF_UNCOMPRESSED_R5G6B5: return 2;
        case RF_UNCOMPRESSED_R4G4B4A4: return 2;
        case RF_UNCOMPRESSED_R8G8B8A8: return 4;
        case RF_UNCOMPRESSED_R8G8B8: return 3;
        case RF_UNCOMPRESSED_R32: return 4;
        case RF_UNCOMPRESSED_R32G32B32: return 12;
        case RF_UNCOMPRESSED_R32G32B32A32: return 16;
        default: return 0;
    }
}

// Get the buffer size of an image of a specific width and height in a given format
RF_API int rf_pixel_buffer_size(rf_pixel_format format, int width, int height)
{
    int data_size = 0; // Size in bytes
    int bpp = rf_bits_per_pixel(format); // Bits per pixel

    data_size = (width * height * bpp) / 8; // Total data size in bytes

    // Most compressed formats works on 4x4 blocks,
    // if texture is smaller, minimum data_size is 8 or 16
    if ((width < 4) && (height < 4))
    {
        if ((format >= RF_COMPRESSED_DXT1_RGB) && (format < RF_COMPRESSED_DXT3_RGBA)) data_size = 8;
        else if ((format >= RF_COMPRESSED_DXT3_RGBA) && (format < RF_COMPRESSED_ASTC_8x8_RGBA)) data_size = 16;
    }

    return data_size;
}


RF_API bool rf_format_pixels_to_normalized(const void* src, int src_size, rf_uncompressed_pixel_format src_format, rf_vec4* dst, int dst_size)
{
    bool success = false;

    int src_bpp = rf_bytes_per_pixel(src_format);
    int src_pixel_count = src_size / src_bpp;
    int dst_pixel_count = dst_size / sizeof(rf_vec4);

    if (dst_pixel_count >= src_pixel_count)
    {
        if (src_format == RF_UNCOMPRESSED_R32G32B32A32)
        {
            success = true;
            memcpy(dst, src, src_size);
        }
        else
        {
            success = true;

            #define RF_FOR_EACH_PIXEL for (int dst_iter = 0, src_iter = 0; src_iter < src_size && dst_iter < dst_size; dst_iter++, src_iter += src_bpp)
            switch (src_format)
            {
                case RF_UNCOMPRESSED_GRAYSCALE:
                    RF_FOR_EACH_PIXEL
                    {
                        float value = ((unsigned char*)src)[src_iter] / 255.0f;

                        dst[dst_iter].x = value;
                        dst[dst_iter].y = value;
                        dst[dst_iter].z = value;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_GRAY_ALPHA:
                    RF_FOR_EACH_PIXEL
                    {
                        float value0 = (float)((unsigned char*)src)[src_iter + 0] / 255.0f;
                        float value1 = (float)((unsigned char*)src)[src_iter + 1] / 255.0f;

                        dst[dst_iter].x = value0;
                        dst[dst_iter].y = value0;
                        dst[dst_iter].z = value0;
                        dst[dst_iter].w = value1;
                    }
                    break;

                case RF_UNCOMPRESSED_R5G5B5A1:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f/31);
                        dst[dst_iter].y = (float)((pixel & 0b0000011111000000) >>  6) * (1.0f/31);
                        dst[dst_iter].z = (float)((pixel & 0b0000000000111110) >>  1) * (1.0f/31);
                        dst[dst_iter].w = ((pixel & 0b0000000000000001) == 0) ? 0.0f : 1.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_R5G6B5:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*)src)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f / 31);
                        dst[dst_iter].y = (float)((pixel & 0b0000011111100000) >>  5) * (1.0f / 63);
                        dst[dst_iter].z = (float) (pixel & 0b0000000000011111)        * (1.0f / 31);
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_R4G4B4A4:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*)src)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111000000000000) >> 12) * (1.0f / 15);
                        dst[dst_iter].y = (float)((pixel & 0b0000111100000000) >> 8)  * (1.0f / 15);
                        dst[dst_iter].z = (float)((pixel & 0b0000000011110000) >> 4)  * (1.0f / 15);
                        dst[dst_iter].w = (float) (pixel & 0b0000000000001111)        * (1.0f / 15);
                    }
                    break;

                case RF_UNCOMPRESSED_R8G8B8A8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = (float)((unsigned char*)src)[src_iter + 0] / 255.0f;
                        dst[dst_iter].y = (float)((unsigned char*)src)[src_iter + 1] / 255.0f;
                        dst[dst_iter].z = (float)((unsigned char*)src)[src_iter + 2] / 255.0f;
                        dst[dst_iter].w = (float)((unsigned char*)src)[src_iter + 3] / 255.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_R8G8B8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = (float)((unsigned char*)src)[src_iter + 0] / 255.0f;
                        dst[dst_iter].y = (float)((unsigned char*)src)[src_iter + 1] / 255.0f;
                        dst[dst_iter].z = (float)((unsigned char*)src)[src_iter + 2] / 255.0f;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_R32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = ((float*)src)[src_iter];
                        dst[dst_iter].y = 0.0f;
                        dst[dst_iter].z = 0.0f;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_R32G32B32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = ((float*)src)[src_iter + 0];
                        dst[dst_iter].y = ((float*)src)[src_iter + 1];
                        dst[dst_iter].z = ((float*)src)[src_iter + 2];
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case RF_UNCOMPRESSED_R32G32B32A32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = ((float*)src)[src_iter + 0];
                        dst[dst_iter].y = ((float*)src)[src_iter + 1];
                        dst[dst_iter].z = ((float*)src)[src_iter + 2];
                        dst[dst_iter].w = ((float*)src)[src_iter + 3];
                    }
                    break;

                default: break;
            }
            #undef RF_FOR_EACH_PIXEL
        }
    }
    else RF_LOG_ERROR_V(RF_ERROR_BAD_SIZE, "Buffer is size %d but function expected a size of at least %d", dst_size, src_pixel_count * sizeof(rf_vec4));

    return success;
}

RF_API bool rf_format_pixels_to_rgba32(const void* src, int src_size, rf_uncompressed_pixel_format src_format, rf_color* dst, int dst_size)
{
    bool success = false;

    int src_bpp = rf_bytes_per_pixel(src_format);
    int src_pixel_count = src_size / src_bpp;
    int dst_pixel_count = dst_size / sizeof(rf_color);

    if (dst_pixel_count >= src_pixel_count)
    {
        if (src_format == RF_UNCOMPRESSED_R8G8B8A8)
        {
            success = true;
            memcpy(dst, src, src_size);
        }
        else
        {
            success = true;
            #define RF_FOR_EACH_PIXEL for (int dst_iter = 0, src_iter = 0; src_iter < src_size && dst_iter < dst_size; dst_iter++, src_iter += src_bpp)
            switch (src_format)
            {
                case RF_UNCOMPRESSED_GRAYSCALE:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned char value = ((unsigned char*) src)[src_iter];
                        dst[dst_iter].r = value;
                        dst[dst_iter].g = value;
                        dst[dst_iter].b = value;
                        dst[dst_iter].a = 255;
                    }
                    break;

                case RF_UNCOMPRESSED_GRAY_ALPHA:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned char value0 = ((unsigned char*) src)[src_iter + 0];
                        unsigned char value1 = ((unsigned char*) src)[src_iter + 1];

                        dst[dst_iter].r = value0;
                        dst[dst_iter].g = value0;
                        dst[dst_iter].b = value0;
                        dst[dst_iter].a = value1;
                    }
                    break;

                case RF_UNCOMPRESSED_R5G5B5A1:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000011111000000) >>  6) * (255 / 31));
                        dst[dst_iter].b = (unsigned char)((float)((pixel & 0b0000000000111110) >>  1) * (255 / 31));
                        dst[dst_iter].a = (unsigned char)        ((pixel & 0b0000000000000001)        *  255);
                    }
                    break;

                case RF_UNCOMPRESSED_R5G6B5:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)* (255 / 31));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000011111100000) >>  5)* (255 / 63));
                        dst[dst_iter].b = (unsigned char)((float) (pixel & 0b0000000000011111)       * (255 / 31));
                        dst[dst_iter].a = 255;
                    }
                    break;

                case RF_UNCOMPRESSED_R4G4B4A4:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)  * (255 / 15));
                        dst[dst_iter].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)  * (255 / 15));
                        dst[dst_iter].a = (unsigned char)((float) (pixel & 0b0000000000001111)        * (255 / 15));
                    }
                    break;

                case RF_UNCOMPRESSED_R8G8B8A8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = ((unsigned char*) src)[src_iter + 0];
                        dst[dst_iter].g = ((unsigned char*) src)[src_iter + 1];
                        dst[dst_iter].b = ((unsigned char*) src)[src_iter + 2];
                        dst[dst_iter].a = ((unsigned char*) src)[src_iter + 3];
                    }
                    break;

                case RF_UNCOMPRESSED_R8G8B8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)((unsigned char*) src)[src_iter + 0];
                        dst[dst_iter].g = (unsigned char)((unsigned char*) src)[src_iter + 1];
                        dst[dst_iter].b = (unsigned char)((unsigned char*) src)[src_iter + 2];
                        dst[dst_iter].a = 255;
                    }
                    break;

                case RF_UNCOMPRESSED_R32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = 0;
                        dst[dst_iter].b = 0;
                        dst[dst_iter].a = 255;
                    }
                    break;

                case RF_UNCOMPRESSED_R32G32B32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = (unsigned char)(((float*) src)[src_iter + 1] * 255.0f);
                        dst[dst_iter].b = (unsigned char)(((float*) src)[src_iter + 2] * 255.0f);
                        dst[dst_iter].a = 255;
                    }
                    break;

                case RF_UNCOMPRESSED_R32G32B32A32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = (unsigned char)(((float*) src)[src_iter + 1] * 255.0f);
                        dst[dst_iter].b = (unsigned char)(((float*) src)[src_iter + 2] * 255.0f);
                        dst[dst_iter].a = (unsigned char)(((float*) src)[src_iter + 3] * 255.0f);
                    }
                    break;

                default: break;
            }
            #undef RF_FOR_EACH_PIXEL
        }
    }
    else RF_LOG_ERROR_V(RF_ERROR_BAD_SIZE, "Buffer is size %d but function expected a size of at least %d", dst_size, src_pixel_count * sizeof(rf_color));

    return success;
}

RF_API bool rf_format_pixels(const void* src, int src_size, rf_uncompressed_pixel_format src_format, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format)
{
    bool success = false;

    if (rf_is_uncompressed_format(src_format) && dst_format == RF_UNCOMPRESSED_R32G32B32A32)
    {
        success = rf_format_pixels_to_normalized(src, src_size, src_format, dst, dst_size);
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == RF_UNCOMPRESSED_R8G8B8A8)
    {
        success = rf_format_pixels_to_rgba32(src, src_size, src_format, dst, dst_size);
    }
    else if (rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        int src_bpp = rf_bytes_per_pixel(src_format);
        int dst_bpp = rf_bytes_per_pixel(dst_format);

        int src_pixel_count = src_size / src_bpp;
        int dst_pixel_count = dst_size / dst_bpp;

        if (dst_pixel_count >= src_pixel_count)
        {
            success = true;

            //Loop over both src and dst
            #define RF_FOR_EACH_PIXEL for (int src_iter = 0, dst_iter = 0; src_iter < src_size && dst_iter < dst_size; src_iter += src_bpp, dst_iter += dst_bpp)
            #define RF_COMPUTE_NORMALIZED_PIXEL() rf_format_one_pixel_to_normalized(((unsigned char*) src) + src_iter, src_format);
            if (src_format == dst_format)
            {
                memcpy(dst, src, src_size);
            }
            else
            {
                switch (dst_format)
                {
                    case RF_UNCOMPRESSED_GRAYSCALE:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();
                            ((unsigned char*)dst)[dst_iter] = (unsigned char)((normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f) * 255.0f);
                        }
                        break;

                    case RF_UNCOMPRESSED_GRAY_ALPHA:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((unsigned char*)dst)[dst_iter    ] = (unsigned char)((normalized.x * 0.299f + (float)normalized.y * 0.587f + (float)normalized.z * 0.114f) * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 1] = (unsigned char) (normalized.w * 255.0f);
                        }
                        break;

                    case RF_UNCOMPRESSED_R5G6B5:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 63.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 31.0f));

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
                        }
                        break;

                    case RF_UNCOMPRESSED_R8G8B8:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((unsigned char*)dst)[dst_iter    ] = (unsigned char)(normalized.x * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 1] = (unsigned char)(normalized.y * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 2] = (unsigned char)(normalized.z * 255.0f);
                        }
                        break;

                    case RF_UNCOMPRESSED_R5G5B5A1:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            int ALPHA_THRESHOLD = 50;
                            unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 31.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 31.0f));
                            unsigned char a = (normalized.w > ((float)ALPHA_THRESHOLD / 255.0f)) ? 1 : 0;

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
                        }
                        break;

                    case RF_UNCOMPRESSED_R4G4B4A4:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            unsigned char r = (unsigned char)(round(normalized.x * 15.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 15.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 15.0f));
                            unsigned char a = (unsigned char)(round(normalized.w * 15.0f));

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
                        }
                        break;

                    case RF_UNCOMPRESSED_R32:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((float*)dst)[dst_iter] = (float)(normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f);
                        }
                        break;

                    case RF_UNCOMPRESSED_R32G32B32:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((float*)dst)[dst_iter    ] = normalized.x;
                            ((float*)dst)[dst_iter + 1] = normalized.y;
                            ((float*)dst)[dst_iter + 2] = normalized.z;
                        }
                        break;

                    default: break;
                }
            }
            #undef RF_FOR_EACH_PIXEL
            #undef RF_COMPUTE_NORMALIZED_PIXEL
        }
        else RF_LOG_ERROR_V(RF_ERROR_BAD_SIZE, "Buffer is size %d but function expected a size of at least %d", dst_size, src_pixel_count * dst_bpp);
    }
    else RF_LOG_ERROR_V("Function expected uncompressed pixel formats. src format: %d, dst format: %d", src_format, dst_format);

    return success;
}


RF_API rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_uncompressed_pixel_format src_format)
{
    rf_vec4 result = {0};

    switch (src_format)
    {
        case RF_UNCOMPRESSED_GRAYSCALE:
        {
            float value = ((unsigned char*)src)[0] / 255.0f;

            result.x = value;
            result.y = value;
            result.z = value;
            result.w = 1.0f;
        }
        break;

        case RF_UNCOMPRESSED_GRAY_ALPHA:
        {
            float value0 = (float)((unsigned char*)src)[0] / 255.0f;
            float value1 = (float)((unsigned char*)src)[1] / 255.0f;

            result.x = value0;
            result.y = value0;
            result.z = value0;
            result.w = value1;
        }
        break;

        case RF_UNCOMPRESSED_R5G5B5A1:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f/31);
            result.y = (float)((pixel & 0b0000011111000000) >>  6) * (1.0f/31);
            result.z = (float)((pixel & 0b0000000000111110) >>  1) * (1.0f/31);
            result.w = ((pixel & 0b0000000000000001) == 0) ? 0.0f : 1.0f;
        }
        break;

        case RF_UNCOMPRESSED_R5G6B5:
        {
            unsigned short pixel = ((unsigned short*)src)[0];

            result.x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f / 31);
            result.y = (float)((pixel & 0b0000011111100000) >>  5) * (1.0f / 63);
            result.z = (float) (pixel & 0b0000000000011111)        * (1.0f / 31);
            result.w = 1.0f;
        }
        break;

        case RF_UNCOMPRESSED_R4G4B4A4:
        {
            unsigned short pixel = ((unsigned short*)src)[0];

            result.x = (float)((pixel & 0b1111000000000000) >> 12) * (1.0f / 15);
            result.y = (float)((pixel & 0b0000111100000000) >> 8)  * (1.0f / 15);
            result.z = (float)((pixel & 0b0000000011110000) >> 4)  * (1.0f / 15);
            result.w = (float) (pixel & 0b0000000000001111)        * (1.0f / 15);
        }
        break;

        case RF_UNCOMPRESSED_R8G8B8A8:
        {
            result.x = (float)((unsigned char*)src)[0] / 255.0f;
            result.y = (float)((unsigned char*)src)[1] / 255.0f;
            result.z = (float)((unsigned char*)src)[2] / 255.0f;
            result.w = (float)((unsigned char*)src)[3] / 255.0f;
        }
        break;

        case RF_UNCOMPRESSED_R8G8B8:
        {
            result.x = (float)((unsigned char*)src)[0] / 255.0f;
            result.y = (float)((unsigned char*)src)[1] / 255.0f;
            result.z = (float)((unsigned char*)src)[2] / 255.0f;
            result.w = 1.0f;
        }
        break;

        case RF_UNCOMPRESSED_R32:
        {
            result.x = ((float*)src)[0];
            result.y = 0.0f;
            result.z = 0.0f;
            result.w = 1.0f;
        }
        break;

        case RF_UNCOMPRESSED_R32G32B32:
        {
            result.x = ((float*)src)[0];
            result.y = ((float*)src)[1];
            result.z = ((float*)src)[2];
            result.w = 1.0f;
        }
        break;

        case RF_UNCOMPRESSED_R32G32B32A32:
        {
            result.x = ((float*)src)[0];
            result.y = ((float*)src)[1];
            result.z = ((float*)src)[2];
            result.w = ((float*)src)[3];
        }
        break;

        default: break;
    }

    return result;
}

RF_API rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_uncompressed_pixel_format src_format)
{
    rf_color result = {0};

    switch (src_format)
    {
        case RF_UNCOMPRESSED_GRAYSCALE:
        {
            unsigned char value = ((unsigned char*) src)[0];
            result.r = value;
            result.g = value;
            result.b = value;
            result.a = 255;
        }
        break;

        case RF_UNCOMPRESSED_GRAY_ALPHA:
        {
            unsigned char value0 = ((unsigned char*) src)[0];
            unsigned char value1 = ((unsigned char*) src)[1];

            result.r = value0;
            result.g = value0;
            result.b = value0;
            result.a = value1;
        }
        break;

        case RF_UNCOMPRESSED_R5G5B5A1:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
            result.g = (unsigned char)((float)((pixel & 0b0000011111000000) >>  6) * (255 / 31));
            result.b = (unsigned char)((float)((pixel & 0b0000000000111110) >>  1) * (255 / 31));
            result.a = (unsigned char)        ((pixel & 0b0000000000000001)        *  255);
        }
        break;

        case RF_UNCOMPRESSED_R5G6B5:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)* (255 / 31));
            result.g = (unsigned char)((float)((pixel & 0b0000011111100000) >>  5)* (255 / 63));
            result.b = (unsigned char)((float) (pixel & 0b0000000000011111)       * (255 / 31));
            result.a = 255;
        }
        break;

        case RF_UNCOMPRESSED_R4G4B4A4:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
            result.g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)  * (255 / 15));
            result.b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)  * (255 / 15));
            result.a = (unsigned char)((float) (pixel & 0b0000000000001111)        * (255 / 15));
        }
        break;

        case RF_UNCOMPRESSED_R8G8B8A8:
        {
            result.r = ((unsigned char*) src)[0];
            result.g = ((unsigned char*) src)[1];
            result.b = ((unsigned char*) src)[2];
            result.a = ((unsigned char*) src)[3];
        }
        break;

        case RF_UNCOMPRESSED_R8G8B8:
        {
            result.r = (unsigned char)((unsigned char*) src)[0];
            result.g = (unsigned char)((unsigned char*) src)[1];
            result.b = (unsigned char)((unsigned char*) src)[2];
            result.a = 255;
        }
        break;

        case RF_UNCOMPRESSED_R32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = 0;
            result.b = 0;
            result.a = 255;
        }
        break;

        case RF_UNCOMPRESSED_R32G32B32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = (unsigned char)(((float*) src)[1] * 255.0f);
            result.b = (unsigned char)(((float*) src)[2] * 255.0f);
            result.a = 255;
        }
        break;

        case RF_UNCOMPRESSED_R32G32B32A32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = (unsigned char)(((float*) src)[1] * 255.0f);
            result.b = (unsigned char)(((float*) src)[2] * 255.0f);
            result.a = (unsigned char)(((float*) src)[3] * 255.0f);
        }
        break;

        default: break;
    }

    return result;
}

RF_API void rf_format_one_pixel(const void* src, rf_uncompressed_pixel_format src_format, void* dst, rf_uncompressed_pixel_format dst_format)
{
    if (src_format == dst_format && rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        memcpy(dst, src, rf_bytes_per_pixel(src_format));
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == RF_UNCOMPRESSED_R32G32B32A32)
    {
        *((rf_vec4*)dst) = rf_format_one_pixel_to_normalized(src, src_format);
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == RF_UNCOMPRESSED_R8G8B8A8)
    {
        *((rf_color*)dst) = rf_format_one_pixel_to_rgba32(src, src_format);
    }
    else if (rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        switch (dst_format)
        {
            case RF_UNCOMPRESSED_GRAYSCALE:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0] = (unsigned char)((normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f) * 255.0f);
            }
            break;

            case RF_UNCOMPRESSED_GRAY_ALPHA:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)((normalized.x * 0.299f + (float)normalized.y * 0.587f + (float)normalized.z * 0.114f) * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char) (normalized.w * 255.0f);
            }
            break;

            case RF_UNCOMPRESSED_R5G6B5:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 63.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 31.0f));

                ((unsigned short*)dst)[0] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
            }
            break;

            case RF_UNCOMPRESSED_R8G8B8:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)(normalized.x * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char)(normalized.y * 255.0f);
                ((unsigned char*)dst)[0 + 2] = (unsigned char)(normalized.z * 255.0f);
            }
            break;

            case RF_UNCOMPRESSED_R5G5B5A1:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                int ALPHA_THRESHOLD = 50;
                unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 31.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 31.0f));
                unsigned char a = (normalized.w > ((float)ALPHA_THRESHOLD / 255.0f)) ? 1 : 0;

                ((unsigned short*)dst)[0] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
            }
            break;

            case RF_UNCOMPRESSED_R4G4B4A4:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                unsigned char r = (unsigned char)(round(normalized.x * 15.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 15.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 15.0f));
                unsigned char a = (unsigned char)(round(normalized.w * 15.0f));

                ((unsigned short*)dst)[0] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
            }
            break;

            case RF_UNCOMPRESSED_R8G8B8A8:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)(normalized.x * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char)(normalized.y * 255.0f);
                ((unsigned char*)dst)[0 + 2] = (unsigned char)(normalized.z * 255.0f);
                ((unsigned char*)dst)[0 + 3] = (unsigned char)(normalized.w * 255.0f);
            }
            break;

            case RF_UNCOMPRESSED_R32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0] = (float)(normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f);
            }
            break;

            case RF_UNCOMPRESSED_R32G32B32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0    ] = normalized.x;
                ((float*)dst)[0 + 1] = normalized.y;
                ((float*)dst)[0 + 2] = normalized.z;
            }
            break;

            case RF_UNCOMPRESSED_R32G32B32A32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0    ] = normalized.x;
                ((float*)dst)[0 + 1] = normalized.y;
                ((float*)dst)[0 + 2] = normalized.z;
                ((float*)dst)[0 + 3] = normalized.w;
            }
            break;

            default: break;
        }
    }
}

//endregion

//region mipmaps_image
RF_API int rf_mipmaps_image_size(rf_mipmaps_image image)
{
    int size   = 0;
    int width  = image.width;
    int height = image.height;

    for (int i = 0; i < image.mipmaps; i++)
    {
        size += rf_pixel_buffer_size(image.format, width, height);

        width  /= 2;
        height /= 2;

        // Security check for NPOT textures
        if (width  < 1) width  = 1;
        if (height < 1) height = 1;
    }

    return size;
}

// Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
RF_API rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int gen_mipmaps_count, void* dst, int dst_size, rf_allocator temp_allocator)
{
    rf_mipmaps_image result = {0};

    if (image.valid)
    {
        int possible_mip_count  = 1;
        int mipmaps_size        = rf_pixel_buffer_size(image.format, image.width, image.height);

        // Count mipmap levels required
        {
            int mip_width  = image.width;
            int mip_height = image.height;

            while (mip_width != 1 || mip_height != 1 || possible_mip_count == gen_mipmaps_count)
            {
                if (mip_width  != 1) mip_width  /= 2;
                if (mip_height != 1) mip_height /= 2;

                // Safety check for NPOT textures
                if (mip_width  < 1) mip_width  = 1;
                if (mip_height < 1) mip_height = 1;

                mipmaps_size += rf_pixel_buffer_size(image.format, mip_width, mip_height);

                possible_mip_count++;
            }
        }

        if (possible_mip_count == gen_mipmaps_count)
        {
            if (dst_size == mipmaps_size)
            {
                // Pointer to current mip location in the dst buffer
                unsigned char* dst_iter = dst;

                // Copy the image to the dst as the first mipmap level
                memcpy(dst_iter, image.data, rf_image_size(image));
                dst_iter += rf_image_size(image);

                // Create a rgba32 buffer for the mipmap result, half the image size is enough for any mipmap level
                int temp_mipmap_buffer_size = rf_pixel_buffer_size(RF_UNCOMPRESSED_R8G8B8A8, image.width / 2, image.height / 2);
                void* temp_mipmap_buffer = RF_ALLOC(temp_allocator, temp_mipmap_buffer_size);

                if (temp_mipmap_buffer)
                {
                    int mip_width  = image.width  / 2;
                    int mip_height = image.height / 2;
                    int mip_count = 1;
                    for (; mip_count < gen_mipmaps_count; mip_count++)
                    {
                        rf_image mipmap = rf_image_resize_to_buffer(image, mip_width, mip_height, temp_mipmap_buffer, temp_mipmap_buffer_size, temp_allocator);

                        if (mipmap.valid)
                        {
                            int dst_iter_size = dst_size - ((int)(dst_iter - ((unsigned char*)(dst))));

                            bool success = rf_format_pixels(mipmap.data, rf_image_size(mipmap), mipmap.format, dst_iter, dst_iter_size, image.format);
                            RF_ASSERT(success);
                        }
                        else break;

                        mip_width  /= 2;
                        mip_height /= 2;

                        // Security check for NPOT textures
                        if (mip_width  < 1) mip_width  = 1;
                        if (mip_height < 1) mip_height = 1;

                        // Compute next mipmap location in the dst buffer
                        dst_iter += rf_pixel_buffer_size(image.format, mip_width, mip_height);
                    }

                    if (mip_count == gen_mipmaps_count)
                    {
                        result = (rf_mipmaps_image){
                            .data = dst,
                            .width = image.width,
                            .height = image.height,
                            .mipmaps = gen_mipmaps_count,
                            .format = image.format,
                            .valid = true
                        };
                    }
                }

                RF_FREE(temp_allocator, temp_mipmap_buffer);
            }
        }
        else RF_LOG_V(RF_LOG_TYPE_WARNING, "rf_image mipmaps already available");
    }

    return result;
}

RF_API void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator)
{
    RF_FREE(allocator, image.data);
}
//endregion

//region image
#include "rayfork_image.inc"
//endregion

//region gif

// Load animated GIF data
//  - rf_image.data buffer includes all frames: [image#0][image#1][image#2][...]
//  - Number of frames is returned through 'frames' parameter
//  - Frames delay is returned through 'delays' parameter (int array)
//  - All frames are returned in RGBA format
RF_API rf_gif rf_load_animated_gif(const void* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_gif gif = {0};

    int comp = 0;

    RF_SET_STBI_ALLOCATOR(allocator);
    {
        gif.data = stbi_load_gif_from_memory(data, data_size, &gif.frame_delays, &gif.width, &gif.height, &gif.frames_count, &comp, 4);
    }
    RF_SET_STBI_ALLOCATOR(RF_NULL_ALLOCATOR);

    gif.format = RF_UNCOMPRESSED_R8G8B8A8;

    return gif;
}

RF_API rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_gif result = (rf_gif) {};

    int file_size = io.get_file_size_proc(filename);
    unsigned char* buffer = RF_ALLOC(temp_allocator, file_size);

    if (io.read_file_into_buffer_proc(filename, buffer, file_size))
    {
        result = rf_load_animated_gif(buffer, file_size, allocator, temp_allocator);
    }

    RF_FREE(temp_allocator, buffer);

    return result;
}

RF_API rf_sizei rf_gif_frame_size(rf_gif gif)
{
    return (rf_sizei) { gif.width / gif.frames_count, gif.height / gif.frames_count };
}

// Returns an image pointing to the frame in the gif
RF_API rf_image rf_get_frame_from_gif(rf_gif gif, int frame)
{
    rf_sizei size = rf_gif_frame_size(gif);

    return (rf_image)
    {
        .data   = ((unsigned char*)gif.data) + rf_pixel_buffer_size(gif.format, size.width, size.height) * frame,
        .width  = size.width,
        .height = size.height,
        .format = gif.format,
    };
}

RF_API void rf_unload_gif(rf_gif gif, rf_allocator allocator)
{
    RF_FREE(allocator, gif.frame_delays);
    rf_unload_image(gif.image, allocator);
}
//endregion

//region textures

// Load texture from file into GPU memory (VRAM)
RF_API rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_image img = rf_load_image_from_file(filename, temp_allocator, temp_allocator, io);

    rf_texture2d texture = rf_load_texture_from_image(img);

    rf_unload_image(img, temp_allocator);

    return texture;
}

// Load texture from an image file data
RF_API rf_texture2d rf_load_texture_from_file_data(const void* dst, int dst_size, rf_allocator temp_allocator)
{
    rf_image img = rf_load_image_from_file_data(dst, dst_size, temp_allocator, temp_allocator);

    rf_texture2d texture = rf_load_texture_from_image(img);

    rf_unload_image(img, temp_allocator);

    return texture;
}

// Load texture from image data
RF_API rf_texture2d rf_load_texture_from_image(rf_image image)
{
    rf_texture2d texture = {0};

    if (image.valid)
    {
        texture.id = rf_gfx_load_texture(image.data, image.width, image.height, image.format, 1);

        if (texture.id != 0)
        {
            texture.width  = image.width;
            texture.height = image.height;
            texture.format = image.format;
            texture.valid  = true;
        }
    }
    else RF_LOG(RF_LOG_TYPE_WARNING, "rf_texture could not be loaded from rf_image");

    return texture;
}

// Load cubemap from image, multiple image cubemap layouts supported
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator)
{
    rf_texture_cubemap cubemap = {0};

    if (layout_type == RF_CUBEMAP_AUTO_DETECT) // Try to automatically guess layout type
    {
        // Check image width/height to determine the type of cubemap provided
        if (image.width > image.height)
        {
            if ((image.width / 6) == image.height) { layout_type = RF_CUBEMAP_LINE_HORIZONTAL; cubemap.width = image.width / 6; }
            else if ((image.width / 4) == (image.height/3)) { layout_type = RF_CUBEMAP_CROSS_FOUR_BY_TREE; cubemap.width = image.width / 4; }
            else if (image.width >= (int)((float)image.height * 1.85f)) { layout_type = RF_CUBEMAP_PANORAMA; cubemap.width = image.width / 4; }
        }
        else if (image.height > image.width)
        {
            if ((image.height / 6) == image.width) { layout_type = RF_CUBEMAP_LINE_VERTICAL; cubemap.width = image.height / 6; }
            else if ((image.width / 3) == (image.height/4)) { layout_type = RF_CUBEMAP_CROSS_THREE_BY_FOUR; cubemap.width = image.width / 3; }
        }

        cubemap.height = cubemap.width;
    }

    if (layout_type != RF_CUBEMAP_AUTO_DETECT)
    {
        int size = cubemap.width;

        rf_image faces = {0}; // Vertical column image
        rf_rec face_recs[6] = {0}; // Face source rectangles
        for (int i = 0; i < 6; i++) face_recs[i] = (rf_rec) {0, 0, size, size };

        if (layout_type == RF_CUBEMAP_LINE_VERTICAL)
        {
            faces = image;
            for (int i = 0; i < 6; i++) face_recs[i].y = size*i;
        }
        else if (layout_type == RF_CUBEMAP_PANORAMA)
        {
            // TODO: Convert panorama image to square faces...
            // Ref: https://github.com/denivip/panorama/blob/master/panorama.cpp
        }
        else
        {
            if (layout_type == RF_CUBEMAP_LINE_HORIZONTAL) { for (int i = 0; i < 6; i++) { face_recs[i].x = size * i; } }
            else if (layout_type == RF_CUBEMAP_CROSS_THREE_BY_FOUR)
            {
                face_recs[0].x = size; face_recs[0].y = size;
                face_recs[1].x = size; face_recs[1].y = 3*size;
                face_recs[2].x = size; face_recs[2].y = 0;
                face_recs[3].x = size; face_recs[3].y = 2*size;
                face_recs[4].x = 0; face_recs[4].y = size;
                face_recs[5].x = 2*size; face_recs[5].y = size;
            }
            else if (layout_type == RF_CUBEMAP_CROSS_FOUR_BY_TREE)
            {
                face_recs[0].x = 2*size; face_recs[0].y = size;
                face_recs[1].x = 0; face_recs[1].y = size;
                face_recs[2].x = size; face_recs[2].y = 0;
                face_recs[3].x = size; face_recs[3].y = 2*size;
                face_recs[4].x = size; face_recs[4].y = size;
                face_recs[5].x = 3*size; face_recs[5].y = size;
            }

            // Convert image data to 6 faces in a vertical column, that's the optimum layout for loading
            rf_image faces_colors = rf_gen_image_color(size, size * 6, RF_MAGENTA, temp_allocator);
            faces = rf_image_format(faces_colors, image.format, temp_allocator);
            rf_unload_image(faces_colors, temp_allocator);

            // TODO: rf_image formating does not work with compressed textures!
        }

        for (int i = 0; i < 6; i++)
        {
            rf_image_draw(&faces, image, face_recs[i], (rf_rec) {0, size * i, size, size }, RF_WHITE, temp_allocator);
        }

        cubemap.id = rf_gfx_load_texture_cubemap(faces.data, size, faces.format);

        if (cubemap.id == 0) RF_LOG(RF_LOG_TYPE_WARNING, "Cubemap image could not be loaded.");

        rf_unload_image(faces, temp_allocator);
    }
    else RF_LOG(RF_LOG_TYPE_WARNING, "Cubemap image layout can not be detected.");

    return cubemap;
}

// Load texture for rendering (framebuffer)
RF_API rf_render_texture2d rf_load_render_texture(int width, int height)
{
    rf_render_texture2d target = rf_gfx_load_render_texture(width, height, RF_UNCOMPRESSED_R8G8B8A8, 24, false);

    return target;
}

// Get pixel data from GPU texture and return an rf_image
RF_API rf_image rf_get_texture_data(rf_texture2d texture, rf_allocator allocator)
{
    rf_image image = {0};

    if (texture.format < 8)
    {
        image.data = rf_gfx_read_texture_pixels(texture, allocator);

        if (image.data != NULL)
        {
            image.width = texture.width;
            image.height = texture.height;
            image.format = texture.format;

            // NOTE: Data retrieved on OpenGL ES 2.0 should be RGBA
            // coming from FBO color buffer, but it seems original
            // texture format is retrieved on RPI... weird...
            //image.format = RF_UNCOMPRESSED_R8G8B8A8;

            RF_LOG(RF_LOG_TYPE_INFO, "rf_texture pixel data obtained successfully");
        }
        else RF_LOG(RF_LOG_TYPE_WARNING, "rf_texture pixel data could not be obtained");
    }
    else RF_LOG(RF_LOG_TYPE_WARNING, "Compressed texture data could not be obtained");

    return image;
}

// Update GPU texture with new data. Pixels data must match texture.format
RF_API void rf_update_texture(rf_texture2d texture, const void* pixels)
{
    rf_gfx_update_texture(texture.id, texture.width, texture.height, texture.format, pixels);
}

// Generate GPU mipmaps for a texture
RF_API void rf_gen_texture_mipmaps(rf_texture2d* texture)
{
    // NOTE: NPOT textures support check inside function
    // On WebGL (OpenGL ES 2.0) NPOT textures support is limited
    rf_gfx_generate_mipmaps(texture);
}

// Set texture wrapping mode
RF_API void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode)
{
    rf_gfx_set_texture_wrap(texture, wrap_mode);
}

// Set texture scaling filter mode
RF_API void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode)
{
    rf_gfx_set_texture_filter(texture, filter_mode);
}

// Unload texture from GPU memory (VRAM)
RF_API void rf_unload_texture(rf_texture2d texture)
{
    if (texture.id > 0)
    {
        rf_gfx_delete_textures(texture.id);

        RF_LOG_V(RF_LOG_TYPE_INFO, "[TEX ID %i] Unloaded texture data from VRAM (GPU)", texture.id);
    }
}

// Unload render texture from GPU memory (VRAM)
RF_API void rf_unload_render_texture(rf_render_texture2d target)
{
    if (target.id > 0)
    {
        rf_gfx_delete_render_textures(target);

        RF_LOG_V(RF_LOG_TYPE_INFO, "[TEX ID %i] Unloaded render texture data from VRAM (GPU)", target.id);
    }
}

//endregion

//region font & text
#define RF_SDF_CHAR_PADDING       (4)
#define RF_SDF_ON_EDGE_VALUE      (128)
#define RF_SDF_PIXEL_DIST_SCALE   (64.0f)
#define RF_BITMAP_ALPHA_THRESHOLD (80)

// Default hardcoded values for ttf file loading
#define RF_DEFAULT_TTF_FONT_SIZE (32) // rf_font first character (32 - space)
#define RF_DEFAULT_TTF_NUMCHARS  (95) // ASCII 32..126 is 95 glyphs
#define RF_DEFAULT_FIRST_CHAR    (32) // Expected first char for image sprite font

/*
   Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found or the length is exhausted
   When a invalid UTF8 rf_byte is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
   Total number of bytes processed are returned as a parameter
   NOTE: the standard says U+FFFD should be returned in case of errors
   but that character is not supported by the default font in raylib
   TODO: optimize this code for speed!!
*/
RF_API rf_utf8_codepoint rf_get_next_utf8_codepoint(const char* text, int len)
{
    /*
    UTF8 specs from https://www.ietf.org/rfc/rfc3629.txt
    Char. number range  |        UTF-8 octet sequence
      (hexadecimal)     |              (binary)
    --------------------+---------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */

    #define RF_DEFAULT_CODEPOINT (0x3f) // Codepoint defaults to '?' if invalid

    if (len < 1)
    {
        return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT };
    }

    const int octet = (unsigned char)(text[0]); // The first UTF8 octet

    if (octet <= 0x7f)
    {
        // Only one octet (ASCII range x00-7F)
        const int code = text[0];

        // Codepoints after U+10ffff are invalid
        return (rf_utf8_codepoint) { code > 0x10ffff ? RF_DEFAULT_CODEPOINT : code, .bytes_processed = 1 };
    }
    else if ((octet & 0xe0) == 0xc0)
    {
        if (len < 2)
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 1 };
        }

        // Two octets
        // [0]xC2-DF    [1]UTF8-tail(x80-BF)
        const unsigned char octet1 = text[1];

        // Check for unexpected sequence
        if ((octet1 == '\0') || ((octet1 >> 6) != 2))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 2 };
        }

        if ((octet >= 0xc2) && (octet <= 0xdf))
        {
            const int code = ((octet & 0x1f) << 6) | (octet1 & 0x3f);

            // Codepoints after U+10ffff are invalid
            return (rf_utf8_codepoint) { code > 0x10ffff ? RF_DEFAULT_CODEPOINT : code, .bytes_processed = 2 };
        }
    }
    else if ((octet & 0xf0) == 0xe0)
    {
        if (len < 2)
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 1 };
        }

        // Three octets
        const unsigned char octet1 = text[1];

        // Check for unexpected sequence
        if ((octet1 == '\0') || (len < 3) || ((octet1 >> 6) != 2))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 2 };
        }

        const unsigned char octet2 = text[2];

        // Check for unexpected sequence
        if ((octet2 == '\0') || ((octet2 >> 6) != 2))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 3 };
        }

        /*
            [0]xE0    [1]xA0-BF       [2]UTF8-tail(x80-BF)
            [0]xE1-EC [1]UTF8-tail    [2]UTF8-tail(x80-BF)
            [0]xED    [1]x80-9F       [2]UTF8-tail(x80-BF)
            [0]xEE-EF [1]UTF8-tail    [2]UTF8-tail(x80-BF)
        */
        if (((octet == 0xe0) && !((octet1 >= 0xa0) && (octet1 <= 0xbf))) ||
            ((octet == 0xed) && !((octet1 >= 0x80) && (octet1 <= 0x9f))))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 2 };
        }

        if ((octet >= 0xe0) && (0 <= 0xef))
        {
            const int code = ((octet & 0xf) << 12) | ((octet1 & 0x3f) << 6) | (octet2 & 0x3f);

            // Codepoints after U+10ffff are invalid
            return (rf_utf8_codepoint) { code > 0x10ffff ? RF_DEFAULT_CODEPOINT : code, .bytes_processed = 3 };
        }
    }
    else if ((octet & 0xf8) == 0xf0)
    {
        // Four octets
        if (octet > 0xf4 || len < 2)
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 1 };
        }

        const unsigned char octet1 = text[1];

        // Check for unexpected sequence
        if ((octet1 == '\0') || (len < 3) || ((octet1 >> 6) != 2))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 2 };
        }

        const unsigned char octet2 = text[2];

        // Check for unexpected sequence
        if ((octet2 == '\0') || (len < 4) || ((octet2 >> 6) != 2))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 3 };
        }

        const unsigned char octet3 = text[3];

        // Check for unexpected sequence
        if ((octet3 == '\0') || ((octet3 >> 6) != 2))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 4 };
        }

        /*
            [0]xF0       [1]x90-BF       [2]UTF8-tail  [3]UTF8-tail
            [0]xF1-F3    [1]UTF8-tail    [2]UTF8-tail  [3]UTF8-tail
            [0]xF4       [1]x80-8F       [2]UTF8-tail  [3]UTF8-tail
        */

        // Check for unexpected sequence
        if (((octet == 0xf0) && !((octet1 >= 0x90) && (octet1 <= 0xbf))) ||
            ((octet == 0xf4) && !((octet1 >= 0x80) && (octet1 <= 0x8f))))
        {
            return (rf_utf8_codepoint) { RF_DEFAULT_CODEPOINT, .bytes_processed = 2 };
        }

        if (octet >= 0xf0)
        {
            const int code = ((octet & 0x7) << 18) | ((octet1 & 0x3f) << 12) | ((octet2 & 0x3f) << 6) | (octet3 & 0x3f);

            // Codepoints after U+10ffff are invalid
            return (rf_utf8_codepoint) { code > 0x10ffff ? RF_DEFAULT_CODEPOINT : code, .bytes_processed = 4 };
        }
    }

    return (rf_utf8_codepoint) {0};
}

// Load rf_font from file into GPU memory (VRAM)
RF_API rf_font rf_load_font_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_font font = {0};

    if (rf_internal_is_file_extension(filename, ".ttf") || rf_internal_is_file_extension(filename, ".otf"))
    {
        int file_size = io.get_file_size_proc(filename);
        void* data = RF_ALLOC(temp_allocator, file_size);
        font = rf_load_font(data, file_size, RF_DEFAULT_TTF_FONT_SIZE, NULL, RF_DEFAULT_TTF_NUMCHARS, allocator, temp_allocator);
        RF_FREE(temp_allocator, data);
    }
    else
    {
        rf_image image = rf_load_image_from_file(filename, temp_allocator, temp_allocator, io);
        if (image.data != NULL) font = rf_load_font_from_image(image, RF_MAGENTA, RF_DEFAULT_FIRST_CHAR, allocator, temp_allocator);
        rf_unload_image(image, temp_allocator);
    }

    if (font.texture.id == 0)
    {
        RF_LOG_V(RF_LOG_TYPE_WARNING, "[%s] rf_font could not be loaded, using default font", filename);
        font = rf_get_default_font();
    }
    else rf_set_texture_filter(font.texture, RF_FILTER_POINT); // By default we set point filter (best performance)

    return font;
}

// Load rf_font from TTF font file with generation parameters
// NOTE: You can pass an array with desired characters, those characters should be available in the font
// if array is NULL, default char set is selected 32..126
RF_API rf_font rf_load_font(const void* font_file_data, int font_file_data_size, int font_size, int chars_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_font font = {0};

    font.base_size = font_size;
    font.chars_count = (chars_count > 0) ? chars_count : 95;
    font.chars = rf_load_font_data(font_file_data, font_file_data_size, font.base_size, font.chars_count, RF_FONT_DEFAULT, allocator, temp_allocator);

    rf_image atlas = rf_gen_image_font_atlas(font.chars, &font.recs, font.chars_count, font.base_size, 2, 0, allocator, temp_allocator);
    font.texture = rf_load_texture_from_image(atlas);

    // Update chars[i].image to use alpha, required to be used on rf_image_draw_text()
    for (int i = 0; i < font.chars_count; i++)
    {
        rf_unload_image(font.chars[i].image);
        font.chars[i].image = rf_image_from_image(atlas, font.recs[i], allocator, temp_allocator);
    }

    rf_unload_image(atlas);

    return font;
}

// Load font data for further use. Note: Requires TTF font and can generate SDF data
RF_API bool rf_load_font_data(const void* font_data, int font_data_size, int font_size, int* chars, int chars_count, rf_font_type type, rf_char_info* dst, int dst_count)
{
    bool success = false;

    if (dst_count >= chars_count)
    {
        // The stbtt functions called here should not require any allocations
        RF_SET_STBTT_ALLOCATOR(RF_NULL_ALLOCATOR);

        // Init font for data reading
        stbtt_fontinfo font_info = {0};
        if (stbtt_InitFont(&font_info, font_data, 0))
        {
            chars_count = chars_count == 0 ? 95 : chars_count;

            // Calculate font scale factor
            float scale_factor = stbtt_ScaleForPixelHeight(&font_info, (float)font_size);

            // Calculate font basic metrics
            // NOTE: ascent is equivalent to font baseline
            int ascent, descent, line_gap;
            stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

            // NOTE: Using simple packaging, one char after another
            for (int i = 0; i < chars_count; i++)
            {
                int char_width  = 0;
                int char_height = 0;

                dst[i].value = chars == NULL ? i + 32 : chars[i];
                stbtt_GetCodepointBitmapBox(&font_info, scale_factor, scale_factor, dst[i].value, &char_width, &char_height, &dst[i].offset_x, &dst[i].offset_y);
                dst[i].width  = char_width;
                dst[i].height = char_height;
                dst[i].offset_y += (int)((float)ascent * scale_factor);

                stbtt_GetCodepointHMetrics(&font_info, dst[i].value, &dst[i].advance_x, NULL);
                dst[i].advance_x *= scale_factor;
            }

            success = true;
        }
        else RF_LOG(RF_LOG_TYPE_WARNING, "Failed to init font!");
    }

    return success;
}

// Load an rf_image font file (XNA style)
RF_API rf_font rf_load_font_from_image(rf_image image, rf_color key, int firstChar, rf_allocator allocator, rf_allocator temp_allocator)
{
    #define rf_color_equal(col1, col2) ((col1.r == col2.r)&&(col1.g == col2.g)&&(col1.b == col2.b)&&(col1.a == col2.a))

    int charSpacing = 0;
    int lineSpacing = 0;

    int x = 0;
    int y = 0;

    // We allocate a temporal arrays for chars data measures,
    // once we get the actual number of chars, we copy data to a sized arrays
    int tempCharValues[RF_MAX_FONT_CHARS];
    rf_rec tempCharRecs[RF_MAX_FONT_CHARS];

    rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

    // Parse image data to get charSpacing and lineSpacing
    for (y = 0; y < image.height; y++)
    {
        for (x = 0; x < image.width; x++)
        {
            if (!rf_color_equal(pixels[y*image.width + x], key)) break;
        }

        if (!rf_color_equal(pixels[y*image.width + x], key)) break;
    }

    charSpacing = x;
    lineSpacing = y;

    int charHeight = 0;
    int j = 0;

    while (!rf_color_equal(pixels[(lineSpacing + j)*image.width + charSpacing], key)) j++;

    charHeight = j;

    // Check array values to get characters: value, x, y, w, h
    int index = 0;
    int lineToRead = 0;
    int xPosToRead = charSpacing;

    // Parse image data to get rectangle sizes
    while ((lineSpacing + lineToRead*(charHeight + lineSpacing)) < image.height)
    {
        while ((xPosToRead < image.width) &&
               !rf_color_equal((pixels[(lineSpacing + (charHeight+lineSpacing)*lineToRead)*image.width + xPosToRead]), key))
        {
            tempCharValues[index] = firstChar + index;

            tempCharRecs[index].x = (float)xPosToRead;
            tempCharRecs[index].y = (float)(lineSpacing + lineToRead*(charHeight + lineSpacing));
            tempCharRecs[index].height = (float)charHeight;

            int charWidth = 0;

            while (!rf_color_equal(pixels[(lineSpacing + (charHeight+lineSpacing)*lineToRead)*image.width + xPosToRead + charWidth], key)) charWidth++;

            tempCharRecs[index].width = (float)charWidth;

            index++;

            xPosToRead += (charWidth + charSpacing);
        }

        lineToRead++;
        xPosToRead = charSpacing;
    }

    RF_LOG(RF_LOG_TYPE_DEBUG, "rf_font data parsed correctly from image");

    // NOTE: We need to remove key color borders from image to avoid weird
    // artifacts on texture scaling when using rf_filter_bilinear or rf_filter_trilinear
    for (int i = 0; i < image.height*image.width; i++) if (rf_color_equal(pixels[i], key)) pixels[i] = RF_BLANK;

    // Create a new image with the processed color data (key color replaced by RF_BLANK)
    rf_image font_clear = rf_load_image_from_rgba32(pixels, image.width, image.height, temp_allocator);

    RF_FREE(temp_allocator, pixels); // Free pixels array memory

    // Create spritefont with all data parsed from image
    rf_font spriteFont = {0};
    spriteFont.allocator = allocator;
    spriteFont.texture = rf_load_texture_from_image(font_clear); // Convert processed image to OpenGL texture
    spriteFont.chars_count = index;

    // We got tempCharValues and tempCharsRecs populated with chars data
    // Now we move temp data to sized charValues and charRecs arrays
    spriteFont.chars = (rf_char_info*) RF_ALLOC(allocator, spriteFont.chars_count * sizeof(rf_char_info));
    spriteFont.recs = (rf_rec*) RF_ALLOC(allocator, spriteFont.chars_count * sizeof(rf_rec));

    for (int i = 0; i < spriteFont.chars_count; i++)
    {
        spriteFont.chars[i].value = tempCharValues[i];

        // Get character rectangle in the font atlas texture
        spriteFont.recs[i] = tempCharRecs[i];

        // NOTE: On image based fonts (XNA style), character offsets and xAdvance are not required (set to 0)
        spriteFont.chars[i].offset_x = 0;
        spriteFont.chars[i].offset_y = 0;
        spriteFont.chars[i].advance_x = 0;

        // Fill character image data from font_clear data
        spriteFont.chars[i].image = rf_image_from_image(font_clear, tempCharRecs[i], allocator, temp_allocator);
    }

    rf_unload_image(font_clear); // Unload processed image once converted to texture

    spriteFont.base_size = (int) spriteFont.recs[0].height;

    RF_LOG(RF_LOG_TYPE_INFO, "rf_image file loaded correctly as rf_font");

    return spriteFont;
}

// Generate image font atlas using chars info. Note: Packing method: 0-Default, 1-Skyline
RF_API rf_image rf_gen_image_font_atlas(rf_char_info* chars, int chars_count, int font_size, int padding, bool use_skyline_rect_packing, rf_rec largest_char, unsigned short* dst, int dst_count, rf_allocator temp_allocator)
{
    //Note: We switch the allocator and the buffer of this image at the end of the function before returning. The code is a bit weird, would be a good candidate for refactoring
    rf_image atlas = {0};

    // In case no chars count provided we suppose default of 95
    chars_count = (chars_count > 0) ? chars_count : 95;

    // Calculate image size based on required pixel area
    // NOTE 1: rf_image is forced to be squared and POT... very conservative!
    // NOTE 2: SDF font characters already contain an internal padding,
    // so image size would result bigger than default font type
    float required_area = 0;
    for (int i = 0; i < chars_count; i++)
    {
        required_area += ((chars[i].width + 2 * padding) * (chars[i].height + 2 * padding));
    }

    float guess_size = sqrtf(required_area) * 1.3f;
    int image_size = (int) rf_next_pot(guess_size);

    if (dst_count >= image_size * image_size)
    {
        // DEBUG: We can see padding in the generated image setting a gray background...
        //for (int i = 0; i < atlas.width*atlas.height; i++) ((unsigned char* )atlas.data)[i] = 100;

        if (!use_skyline_rect_packing) // Use basic packing algorythm
        {
            int offset_x = padding;
            int offset_y = padding;

            // NOTE: Using simple packaging, one char after another
            for (int i = 0; i < chars_count; i++)
            {
                // Copy pixel data from fc.data to atlas
                for (int y = 0; y < chars[i].height; y++)
                {
                    for (int x = 0; x < chars[i].width; x++)
                    {
                        ((unsigned char*)atlas.data)[(offset_y + y) * atlas.width + (offset_x + x)] = ((unsigned char*)chars[i].image.data)[y * chars[i].width + x];
                    }
                }

                // Fill chars rectangles in atlas info
                recs[i].x = (float)offset_x;
                recs[i].y = (float)offset_y;

                // Move atlas position X for next character drawing
                offset_x += (chars[i].image.width + 2*padding);

                if (offset_x >= (atlas.width - chars[i].image.width - padding))
                {
                    offset_x = padding;

                    // NOTE: Be careful on offset_y for SDF fonts, by default SDF
                    // use an internal padding of 4 pixels, it means char rectangle
                    // height is bigger than font_size, it could be up to (font_size + 8)
                    offset_y += (font_size + 2*padding);

                    if (offset_y > (atlas.height - font_size - padding)) break;
                }
            }
        }
        else if (use_skyline_rect_packing) // Use Skyline rect packing algorythm (stb_pack_rect)
        {
            RF_LOG(RF_LOG_TYPE_DEBUG, "Using Skyline packing algorythm!");

            stbrp_context context = {0};
            stbrp_node* nodes = (stbrp_node*) RF_ALLOC(temp_allocator, chars_count * sizeof(*nodes));

            stbrp_init_target(&context, atlas.width, atlas.height, nodes, chars_count);
            stbrp_rect* rects = (stbrp_rect*) RF_ALLOC(temp_allocator, chars_count * sizeof(stbrp_rect));

            // Fill rectangles for packaging
            for (int i = 0; i < chars_count; i++)
            {
                rects[i].id = i;
                rects[i].w = chars[i].image.width + 2 * padding;
                rects[i].h = chars[i].image.height + 2 * padding;
            }

            // Package rectangles into atlas
            stbrp_pack_rects(&context, rects, chars_count);

            for (int i = 0; i < chars_count; i++)
            {
                // It return char rectangles in atlas
                recs[i].x = rects[i].x + (float) padding;
                recs[i].y = rects[i].y + (float) padding;
                recs[i].width = (float) chars[i].image.width;
                recs[i].height = (float) chars[i].image.height;

                if (rects[i].was_packed)
                {
                    // Copy pixel data from fc.data to atlas
                    for (int y = 0; y < chars[i].image.height; y++)
                    {
                        for (int x = 0; x < chars[i].image.width; x++)
                        {
                            ((unsigned char *) atlas.data)[(rects[i].y + padding + y) * atlas.width + (rects[i].x + padding + x)] = ((unsigned char *) chars[i].image.data)[y * chars[i].image.width + x];
                        }
                    }
                }
                else RF_LOG_V(RF_LOG_TYPE_WARNING, "Character could not be packed: %i", i);
            }

            RF_FREE(temp_allocator, recs);
            RF_FREE(temp_allocator, nodes);
        }

        atlas.width  = image_size; // Atlas bitmap width
        atlas.height = image_size; // Atlas bitmap height
        atlas.format = RF_UNCOMPRESSED_GRAY_ALPHA;
    }

    // TODO: Crop image if required for smaller size
    // Convert image data from GRAYSCALE to GRAY_ALPHA
    // WARNING: rf_image_alpha_mask(&atlas, atlas) does not work in this case, requires manual operation
    unsigned char* data_gray_alpha = (unsigned char*) RF_ALLOC(allocator, atlas.width*atlas.height * sizeof(unsigned char) * 2); // Two channels

    for (int i = 0, k = 0; i < atlas.width*atlas.height; i++, k += 2)
    {
        data_gray_alpha[k] = 255;
        data_gray_alpha[k + 1] = ((unsigned char* )atlas.data)[i];
    }

    atlas.data = data_gray_alpha;
    atlas.format = RF_UNCOMPRESSED_GRAY_ALPHA;

    return atlas;
}


//Note: Must call rf_finish_load_font_thread_safe on the gl thread afterwards to finish loading the font
RF_API rf_load_font_async_result rf_load_font_async(const unsigned char* font_file_data, int font_file_data_size, int font_size, int chars_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_font font = {0};

    font.base_size = font_size;
    font.chars_count = (chars_count > 0)? chars_count : 95;
    font.chars = rf_load_font_data(font_file_data, font_file_data_size, font.base_size, font.chars_count, RF_FONT_DEFAULT, allocator, temp_allocator);

    rf_image atlas = rf_gen_image_font_atlas(font.chars, &font.recs, font.chars_count, font.base_size, 2, 0, allocator, temp_allocator);

    // Update chars[i].image to use alpha, required to be used on rf_image_draw_text()
    for (int i = 0; i < font.chars_count; i++)
    {
        rf_unload_image(font.chars[i].image);
        font.chars[i].image = rf_image_from_image(atlas, font.recs[i], allocator, temp_allocator);
    }

    return (rf_load_font_async_result) { font, atlas };
}

RF_API rf_font rf_finish_load_font_async(rf_load_font_async_result font_job_result, rf_allocator font_job_allocator)
{
    font_job_result.font.texture = rf_load_texture_from_image(font_job_result.atlas);
    rf_unload_image(font_job_result.atlas, font_job_allocator);

    return font_job_result.font;
}


// Unload rf_font from GPU memory (VRAM)
RF_API void rf_unload_font(rf_font font, rf_allocator allocator)
{
    rf_unload_texture(font.texture);

    RF_FREE(allocator, font.chars);

    RF_LOG(RF_LOG_TYPE_DEBUG, "Unloaded sprite font data");
}


// Returns index position for a unicode character on spritefont
RF_API int rf_get_glyph_index(rf_font font, int character)
{
    return (character - 32);
}

// Measure string size for rf_font
RF_API rf_sizef rf_measure_text(rf_font font, const char* text, int len, float font_size, float spacing)
{
    int temp_len = 0; // Used to count longer text line num chars
    int len_counter = 0;

    float text_width = 0.0f;
    float temp_text_width = 0.0f; // Used to count longer text line width

    float text_height  = (float)font.base_size;
    float scale_factor = font_size/(float)font.base_size;

    int letter = 0; // Current character
    int index  = 0; // Index position in sprite font

    for (int i = 0; i < len; i++)
    {
        len_counter++;

        rf_utf8_codepoint codepoint = rf_get_next_utf8_codepoint(&text[i], len - i);
        index = rf_get_glyph_index(font, codepoint.bytes_processed);

        // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) { codepoint.bytes_processed = 1; }
        i += codepoint.bytes_processed - 1;

        if (letter != '\n')
        {
            if (font.chars[index].advance_x != 0) { text_width += font.chars[index].advance_x; }
            else { text_width += (font.recs[index].width + font.chars[index].offset_x); }
        }
        else
        {
            if (temp_text_width < text_width) { temp_text_width = text_width; }

            len_counter = 0;
            text_width = 0;
            text_height += ((float)font.base_size*1.5f); // NOTE: Fixed line spacing of 1.5 lines
        }

        if (temp_len < len_counter) { temp_len = len_counter; }
    }

    if (temp_text_width < text_width) temp_text_width = text_width;

    return (rf_sizef) {
        temp_text_width * scale_factor + (float)((temp_len - 1)*spacing), // Adds chars spacing to measure
        text_height * scale_factor,
    };
}

RF_API rf_sizef rf_measure_text_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float extra_spacing, bool wrap)
{
    rf_sizef result = {0};
    int text_offset_x = 0; // Offset between characters
    int text_offset_y = 0; // Required for line break!
    float scale_factor = 0.0f;

    int letter = 0; // Current character
    int index = 0; // Index position in sprite font

    scale_factor = font_size / font.base_size;

    enum
    {
        MEASURE_WRAP_STATE = 0,
        MEASURE_REGULAR_STATE = 1
    };

    int state = wrap ? MEASURE_WRAP_STATE : MEASURE_REGULAR_STATE;
    int start_line = -1; // Index where to begin drawing (where a line begins)
    int end_line = -1; // Index where to stop drawing (where a line ends)
    int lastk = -1; // Holds last value of the character position

    int max_y   = 0;
    int first_y = 0;
    bool first_y_set = false;

    for (int i = 0, k = 0; i < text_len; i++, k++)
    {
        int glyph_width = 0;

        rf_utf8_codepoint codepoint = rf_get_next_utf8_codepoint(&text[i], text_len - i);
        letter = codepoint.value;
        index = rf_get_glyph_index(font, letter);

        // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) codepoint.bytes_processed = 1;
        i += codepoint.bytes_processed - 1;

        if (letter != '\n')
        {
            glyph_width = (font.chars[index].advance_x == 0) ?
                          (int)(font.recs[index].width * scale_factor + extra_spacing) :
                          (int)(font.chars[index].advance_x * scale_factor + extra_spacing);
        }

        // NOTE: When word_wrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in start_line and end_line, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When word_wrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_WRAP_STATE)
        {
            // TODO: there are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // See: http://jkorpela.fi/chars/spaces.html
            if ((letter == ' ') || (letter == '\t') || (letter == '\n')) { end_line = i; }

            if ((text_offset_x + glyph_width + 1) >= rec.width)
            {
                end_line = (end_line < 1) ? i : end_line;
                if (i == end_line) { end_line -= codepoint.bytes_processed; }
                if ((start_line + codepoint.bytes_processed) == end_line) { end_line = i - codepoint.bytes_processed; }
                state = !state;
            }
            else if ((i + 1) == text_len)
            {
                end_line = i;
                state = !state;
            }
            else if (letter == '\n')
            {
                state = !state;
            }

            if (state == MEASURE_REGULAR_STATE)
            {
                text_offset_x = 0;
                i = start_line;
                glyph_width = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (letter == '\n')
            {
                if (!wrap)
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                }
            }
            else
            {
                if (!wrap && (text_offset_x + glyph_width + 1) >= rec.width)
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                }

                if ((text_offset_y + (int)(font.base_size*scale_factor)) > rec.height) break;

                // The right side expression is the offset of the latest character plus its width (so the end of the line)
                // We want the highest value of that expression by the end of the function
                result.width  = RF_MAX(result.width,  rec.x + text_offset_x - 1 + glyph_width);

                if (!first_y_set)
                {
                    first_y = rec.y + text_offset_y;
                    first_y_set = true;
                }

                max_y = RF_MAX(max_y, rec.y + text_offset_y + font.base_size * scale_factor);
            }

            if (wrap && i == end_line)
            {
                text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                text_offset_x = 0;
                start_line = end_line;
                end_line = -1;
                glyph_width = 0;
                k = lastk;
                state = !state;
            }
        }

        text_offset_x += glyph_width;
    }

    result.height = max_y - first_y;

    return result;
}
//endregion

//region model
#include "rayfork_model.inc"
//endregion