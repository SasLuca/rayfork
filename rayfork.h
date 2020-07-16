#ifndef RAYFORK_H
#define RAYFORK_H

// Libc includes
#include "stdbool.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "math.h"

#pragma region macros

#ifndef RF_API
    #ifdef __cplusplus
        #define RF_API extern "C"
    #else
        #define RF_API extern
    #endif
#endif

#ifndef RF_INTERNAL
    #define RF_INTERNAL static
#endif

// Used to make constant literals work even in C++ mode
#ifdef __cplusplus
    #define RF_LIT(type) type
#else
    #define RF_LIT(type) (type)
#endif

#ifndef RF_THREAD_LOCAL
    #if __cplusplus >= 201103L
        #define RF_THREAD_LOCAL thread_local
    #elif __STDC_VERSION_ >= 201112L
        #define RF_THREAD_LOCAL _Thread_local
    #elif defined(__GNUC__) /* note that ICC (linux) and Clang are covered by __GNUC__ */
        #define RF_THREAD_LOCAL __thread
    #elif defined(_MSC_VER)
        #define RF_THREAD_LOCAL __declspec(thread)
    #endif
#endif

#define RF_CONCAT2(a, b) a##b
#define RF_CONCAT(a, b) RF_CONCAT2(a, b)

#pragma endregion

#pragma region error
typedef enum rf_error_type
{
    RF_NO_ERROR,
    RF_BAD_ARGUMENT,
    RF_BAD_ALLOC,
    RF_BAD_IO,
    RF_BAD_BUFFER_SIZE,
    RF_BAD_FORMAT,
    RF_LIMIT_REACHED,
    RF_STBI_FAILED,
    RF_STBTT_FAILED,
    RF_UNSUPPORTED,
} rf_error_type;

RF_API rf_error_type rf_last_error();
#pragma endregion

#pragma region logger
#define RF_DEFAULT_LOGGER rf_libc_printf_logger
#define RF_NULL_LOGGER NULL

typedef enum rf_log_type
{
    RF_LOG_TYPE_NONE    = 0,
    RF_LOG_TYPE_DEBUG   = 0x1, // Useful mostly to rayfork devs
    RF_LOG_TYPE_INFO    = 0x2, // Information
    RF_LOG_TYPE_WARNING = 0x4, // Warnings about things to be careful about
    RF_LOG_TYPE_ERROR   = 0x8, // Errors that prevented functions from doing everything they advertised
} rf_log_type;

typedef void (*rf_log_proc)(const char* file, int line, const char* proc_name, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

RF_API void rf_set_log_callback(rf_log_proc);
RF_API void rf_set_log_filter(rf_log_type);
RF_API rf_log_type rf_current_log_filter();
RF_API const char* rf_log_type_str(rf_log_type);

RF_API void rf_libc_printf_logger(const char* file, int line, const char* proc_name, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);
#pragma endregion

#pragma region allocator
#define RF_DEFAULT_ALLOCATOR (RF_LIT(rf_allocator) { NULL, rf_libc_malloc_wrapper, rf_libc_free_wrapper })
#define RF_NULL_ALLOCATOR           (RF_LIT(rf_allocator) {0})
#define RF_ALLOC(allocator, size)   ((allocator).alloc_proc((allocator).user_data, (size)))
#define RF_CALLOC(allocator, size)  rf_calloc_wrapper((allocator), 1, size)
#define RF_FREE(allocator, pointer) ((allocator).free_proc((allocator).user_data, (pointer)))

typedef struct rf_allocator
{
    void* user_data;
    void* (*alloc_proc) (void* user_data, int size_to_alloc);
    void  (*free_proc) (void* user_data, void* ptr_to_free);
} rf_allocator;

RF_API void* rf_calloc_wrapper(rf_allocator allocator, int amount, int size);

RF_API void* rf_libc_malloc_wrapper(void* user_data, int size_to_alloc);
RF_API void  rf_libc_free_wrapper(void* user_data, void* ptr_to_free);
#pragma endregion

#pragma region rand
#define RF_DEFAULT_RAND_PROC (rf_libc_rand_wrapper)
typedef int (*rf_rand_proc)(int min, int max);
RF_API int rf_libc_rand_wrapper(int min, int max);
#pragma endregion

#pragma region io
#define RF_NULL_IO                                (RF_LIT(rf_io_callbacks) {0})
#define RF_FILE_SIZE(io, filename)                ((io).file_size_proc((io).user_data, filename))
#define RF_READ_FILE(io, filename, dst, dst_size) ((io).read_file_proc((io).user_data, filename, dst, dst_size))

#define RF_DEFAULT_IO        (RF_LIT(rf_io_callbacks) { NULL, rf_libc_get_file_size, rf_libc_load_file_into_buffer })

typedef struct rf_io_callbacks
{
    void* user_data;
    int (*file_size_proc) (void* user_data, const char* filename);
    bool (*read_file_proc) (void* user_data, const char* filename, void* dst, int dst_size); // Returns true if operation was successful
} rf_io_callbacks;

RF_API int  rf_libc_get_file_size(void* user_data, const char* filename);
RF_API bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, int dst_size);
#pragma endregion

#pragma region graphics

#pragma region macros and constants

#if !defined(RAYFORK_GRAPHICS_BACKEND_GL_33) && !defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) && !defined(RAYFORK_GRAPHICS_BACKEND_METAL) && !defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    #define RF_NO_GRAPHICS_BACKEND_SELECTED_BY_THE_USER (1)
#endif

// If no graphics backend was set, choose OpenGL33 on desktop and OpenGL ES3 on mobile
#if RF_NO_GRAPHICS_BACKEND_SELECTED_BY_THE_USER
    #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) || (defined(__linux__) && !defined(__ANDROID__ )) || (defined(__APPLE__) && defined(TARGET_OS_MAC))
        #define RAYFORK_GRAPHICS_BACKEND_GL_33 (1)
    #else // if on mobile
        #define RAYFORK_GRAPHICS_BACKEND_GL_ES3 (1)
    #endif
#endif

// Check to make sure only one graphics backend was selected
#if (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) + defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) + defined(RAYFORK_GRAPHICS_BACKEND_METAL) + defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)) != 1
    #error rayfork error: you can only set one graphics backend but 2 or more were detected.
#endif

#define RF_SDF_CHAR_PADDING         (4)
#define RF_SDF_ON_EDGE_VALUE        (128)
#define RF_SDF_PIXEL_DIST_SCALE     (64.0f)

#define RF_BITMAP_ALPHA_THRESHOLD   (80)
#define RF_DEFAULT_FONT_SIZE        (64)

#define RF_BUILTIN_FONT_CHARS       { ' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~', }
#define RF_BUILTIN_FONT_FIRST_CHAR  (32)
#define RF_BUILTIN_FONT_LAST_CHAR   (126)
#define RF_BUILTIN_CODEPOINTS_COUNT (96) // ASCII 32 up to 126 is 96 glyphs (note that the range is inclusive)
#define RF_BUILTIN_FONT_PADDING     (2)

#define RF_GLYPH_NOT_FOUND          (-1)
#define RF_INVALID_CODEPOINT        ('?')
#define RF_DEFAULT_KEY_COLOR        (RF_MAGENTA)

#define RF_LIGHTGRAY  (RF_LIT(rf_color) { 200, 200, 200, 255 })
#define RF_GRAY       (RF_LIT(rf_color) { 130, 130, 130, 255 })
#define RF_DARKGRAY   (RF_LIT(rf_color) {  80,  80,  80, 255 })
#define RF_YELLOW     (RF_LIT(rf_color) { 253, 249,   0, 255 })
#define RF_GOLD       (RF_LIT(rf_color) { 255, 203,   0, 255 })
#define RF_ORANGE     (RF_LIT(rf_color) { 255, 161,   0, 255 })
#define RF_PINK       (RF_LIT(rf_color) { 255, 109, 194, 255 })
#define RF_RED        (RF_LIT(rf_color) { 230,  41,  55, 255 })
#define RF_MAROON     (RF_LIT(rf_color) { 190,  33,  55, 255 })
#define RF_GREEN      (RF_LIT(rf_color) {   0, 228,  48, 255 })
#define RF_LIME       (RF_LIT(rf_color) {   0, 158,  47, 255 })
#define RF_DARKGREEN  (RF_LIT(rf_color) {   0, 117,  44, 255 })
#define RF_SKYBLUE    (RF_LIT(rf_color) { 102, 191, 255, 255 })
#define RF_BLUE       (RF_LIT(rf_color) {   0, 121, 241, 255 })
#define RF_DARKBLUE   (RF_LIT(rf_color) {   0,  82, 172, 255 })
#define RF_PURPLE     (RF_LIT(rf_color) { 200, 122, 255, 255 })
#define RF_VIOLET     (RF_LIT(rf_color) { 135,  60, 190, 255 })
#define RF_DARKPURPLE (RF_LIT(rf_color) { 112,  31, 126, 255 })
#define RF_BEIGE      (RF_LIT(rf_color) { 211, 176, 131, 255 })
#define RF_BROWN      (RF_LIT(rf_color) { 127, 106,  79, 255 })
#define RF_DARKBROWN  (RF_LIT(rf_color) {  76,  63,  47, 255 })

#define RF_WHITE      (RF_LIT(rf_color) { 255, 255, 255, 255 })
#define RF_BLACK      (RF_LIT(rf_color) {   0,   0,   0, 255 })
#define RF_BLANK      (RF_LIT(rf_color) {   0,   0,   0,   0 })
#define RF_MAGENTA    (RF_LIT(rf_color) { 255,   0, 255, 255 })
#define RF_RAYWHITE   (RF_LIT(rf_color) { 245, 245, 245, 255 })

#ifndef RF_DEFAULT_BATCH_ELEMENTS_COUNT
    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL)
        #define RF_DEFAULT_BATCH_ELEMENTS_COUNT (2048)
    #else
        #define RF_DEFAULT_BATCH_ELEMENTS_COUNT (8192)
    #endif
#endif

#if !defined(RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT)
    #define RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT (1) // Max number of buffers for batching (multi-buffering)
#endif

#if !defined(RF_MAX_MATRIX_STACK_SIZE)
    #define RF_MAX_MATRIX_STACK_SIZE (32) // Max size of rf_mat rf__ctx->gl_ctx.stack
#endif

#if !defined(RF_DEFAULT_BATCH_DRAW_CALLS_COUNT)
    #define RF_DEFAULT_BATCH_DRAW_CALLS_COUNT (256) // Max rf__ctx->gl_ctx.draws by state changes (mode, texture)
#endif

//rf_shader and material limits
#if !defined(RF_MAX_SHADER_LOCATIONS)
    #define RF_MAX_SHADER_LOCATIONS (32) // Maximum number of predefined locations stored in shader struct
#endif

#if !defined(RF_MAX_MATERIAL_MAPS)
    #define RF_MAX_MATERIAL_MAPS (12) // Maximum number of texture maps stored in shader struct
#endif

#if !defined(RF_MAX_TEXT_BUFFER_LENGTH)
    #define RF_MAX_TEXT_BUFFER_LENGTH (1024) // Size of internal RF_INTERNAL buffers used on some functions:
#endif

#if !defined(RF_MAX_MESH_VBO)
    #define RF_MAX_MESH_VBO (7) // Maximum number of vbo per mesh
#endif

#define RF_BUILTIN_FONT_CHARS_COUNT (224) // Number of characters in the raylib font

// Default vertex attribute names on shader to set location points
#define RF_DEFAULT_ATTRIB_POSITION_NAME  "vertex_position"    // shader-location = 0
#define RF_DEFAULT_ATTRIB_TEXCOORD_NAME  "vertex_tex_coord"   // shader-location = 1
#define RF_DEFAULT_ATTRIB_NORMAL_NAME    "vertex_normal"      // shader-location = 2
#define RF_DEFAULT_ATTRIB_COLOR_NAME     "vertex_color"       // shader-location = 3
#define RF_DEFAULT_ATTRIB_TANGENT_NAME   "vertex_tangent"     // shader-location = 4
#define RF_DEFAULT_ATTRIB_TEXCOORD2_NAME "vertex_tex_coord2"  // shader-location = 5

// Math
#define RF_PI (3.14159265358979323846f)
#define RF_DEG2RAD (RF_PI / 180.0f)
#define RF_RAD2DEG (180.0f / RF_PI)

#pragma endregion

#pragma region enums

// Matrix modes (equivalent to OpenGL)
typedef enum rf_matrix_mode
{
    RF_MODELVIEW  = 0x1700,  // GL_MODELVIEW
    RF_PROJECTION = 0x1701,  // GL_PROJECTION
    RF_TEXTURE    = 0x1702,  // GL_TEXTURE
} rf_matrix_mode;

// Drawing modes (equivalent to OpenGL)
typedef enum rf_drawing_mode
{
    RF_LINES     = 0x0001, // GL_LINES
    RF_TRIANGLES = 0x0004, // GL_TRIANGLES
    RF_QUADS     = 0x0007, // GL_QUADS
} rf_drawing_mode;

// rf_shader location point type
typedef enum rf_shader_location_index
{
    RF_LOC_VERTEX_POSITION = 0,
    RF_LOC_VERTEX_TEXCOORD01 = 1,
    RF_LOC_VERTEX_TEXCOORD02 = 2,
    RF_LOC_VERTEX_NORMAL = 3,
    RF_LOC_VERTEX_TANGENT = 4,
    RF_LOC_VERTEX_COLOR = 5,
    RF_LOC_MATRIX_MVP = 6,
    RF_LOC_MATRIX_MODEL = 7,
    RF_LOC_MATRIX_VIEW = 8,
    RF_LOC_MATRIX_PROJECTION = 9,
    RF_LOC_VECTOR_VIEW = 10,
    RF_LOC_COLOR_DIFFUSE = 11,
    RF_LOC_COLOR_SPECULAR = 12,
    RF_LOC_COLOR_AMBIENT = 13,

    // These 2 are the same
    RF_LOC_MAP_ALBEDO = 14,
    RF_LOC_MAP_DIFFUSE = 14,

    // These 2 are the same
    RF_LOC_MAP_METALNESS = 15,
    RF_LOC_MAP_SPECULAR = 15,

    RF_LOC_MAP_NORMAL = 16,
    RF_LOC_MAP_ROUGHNESS = 17,
    RF_LOC_MAP_OCCLUSION = 18,
    RF_LOC_MAP_EMISSION = 19,
    RF_LOC_MAP_HEIGHT = 20,
    RF_LOC_MAP_CUBEMAP = 21,
    RF_LOC_MAP_IRRADIANCE = 22,
    RF_LOC_MAP_PREFILTER = 23,
    RF_LOC_MAP_BRDF = 24,
} rf_shader_location_index;

// rf_shader uniform data types
typedef enum rf_shader_uniform_data_type
{
    RF_UNIFORM_FLOAT = 0,
    RF_UNIFORM_VEC2,
    RF_UNIFORM_VEC3,
    RF_UNIFORM_VEC4,
    RF_UNIFORM_INT,
    RF_UNIFORM_IVEC2,
    RF_UNIFORM_IVEC3,
    RF_UNIFORM_IVEC4,
    RF_UNIFORM_SAMPLER2D
} rf_shader_uniform_data_type;

// rf_material map type
typedef enum rf_material_map_type
{
    // These 2 are the same
    RF_MAP_ALBEDO = 0,
    RF_MAP_DIFFUSE = 0,

    // These 2 are the same
    RF_MAP_METALNESS = 1,
    RF_MAP_SPECULAR = 1,

    RF_MAP_NORMAL = 2,
    RF_MAP_ROUGHNESS = 3,
    RF_MAP_OCCLUSION = 4,
    RF_MAP_EMISSION = 5,
    RF_MAP_HEIGHT = 6,
    RF_MAP_CUBEMAP = 7,     // NOTE: Uses GL_TEXTURE_CUBE_MAP
    RF_MAP_IRRADIANCE = 8,  // NOTE: Uses GL_TEXTURE_CUBE_MAP
    RF_MAP_PREFILTER = 9,   // NOTE: Uses GL_TEXTURE_CUBE_MAP
    RF_MAP_BRDF = 10
} rf_material_map_type;

typedef enum rf_text_wrap_mode
{
    RF_CHAR_WRAP,
    RF_WORD_WRAP,
} rf_text_wrap_mode;

// Pixel formats
typedef enum rf_pixel_format
{
    RF_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    RF_UNCOMPRESSED_GRAY_ALPHA, // 8 * 2 bpp (2 channels)
    RF_UNCOMPRESSED_R5G6B5, // 16 bpp
    RF_UNCOMPRESSED_R8G8B8, // 24 bpp
    RF_UNCOMPRESSED_R5G5B5A1, // 16 bpp (1 bit alpha)
    RF_UNCOMPRESSED_R4G4B4A4, // 16 bpp (4 bit alpha)
    RF_UNCOMPRESSED_R8G8B8A8, // 32 bpp
    RF_UNCOMPRESSED_RGBA32 = RF_UNCOMPRESSED_R8G8B8A8, //32 bpp
    RF_UNCOMPRESSED_R32, // 32 bpp (1 channel - float)
    RF_UNCOMPRESSED_R32G32B32, // 32 * 3 bpp (3 channels - float)
    RF_UNCOMPRESSED_R32G32B32A32, // 32 * 4 bpp (4 channels - float)
    RF_UNCOMPRESSED_NORMALIZED = RF_UNCOMPRESSED_R32G32B32A32, // 32 * 4 bpp (4 channels - float)
    RF_COMPRESSED_DXT1_RGB, // 4 bpp (no alpha)
    RF_COMPRESSED_DXT1_RGBA, // 4 bpp (1 bit alpha)
    RF_COMPRESSED_DXT3_RGBA, // 8 bpp
    RF_COMPRESSED_DXT5_RGBA, // 8 bpp
    RF_COMPRESSED_ETC1_RGB, // 4 bpp
    RF_COMPRESSED_ETC2_RGB, // 4 bpp
    RF_COMPRESSED_ETC2_EAC_RGBA, // 8 bpp
    RF_COMPRESSED_PVRT_RGB, // 4 bpp
    RF_COMPRESSED_PVRT_RGBA, // 4 bpp
    RF_COMPRESSED_ASTC_4x4_RGBA, // 8 bpp
    RF_COMPRESSED_ASTC_8x8_RGBA // 2 bpp
} rf_pixel_format;

typedef enum rf_pixel_format rf_compressed_pixel_format;
typedef enum rf_pixel_format rf_uncompressed_pixel_format;

typedef enum rf_desired_channels
{
    RF_ANY_CHANNELS = 0,
    RF_1BYTE_GRAYSCALE = 1,
    RF_2BYTE_GRAY_ALPHA = 2,
    RF_3BYTE_R8G8B8 = 3,
    RF_4BYTE_R8G8B8A8 = 4,
} rf_desired_channels;

// rf_texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum rf_texture_filter_mode
{
    RF_FILTER_POINT = 0, // No filter, just pixel aproximation
    RF_FILTER_BILINEAR, // Linear filtering
    RF_FILTER_TRILINEAR, // Trilinear filtering (linear with mipmaps)
    RF_FILTER_ANISOTROPIC_4x, // Anisotropic filtering 4x
    RF_FILTER_ANISOTROPIC_8x, // Anisotropic filtering 8x
    RF_FILTER_ANISOTROPIC_16x, // Anisotropic filtering 16x
} rf_texture_filter_mode;

// Cubemap layout type
typedef enum rf_cubemap_layout_type
{
    RF_CUBEMAP_AUTO_DETECT = 0, // Automatically detect layout type
    RF_CUBEMAP_LINE_VERTICAL, // Layout is defined by a vertical line with faces
    RF_CUBEMAP_LINE_HORIZONTAL, // Layout is defined by an horizontal line with faces
    RF_CUBEMAP_CROSS_THREE_BY_FOUR, // Layout is defined by a 3x4 cross with cubemap faces
    RF_CUBEMAP_CROSS_FOUR_BY_TREE, // Layout is defined by a 4x3 cross with cubemap faces
    RF_CUBEMAP_PANORAMA // Layout is defined by a panorama image (equirectangular map)
} rf_cubemap_layout_type;

// rf_texture parameters: wrap mode
typedef enum rf_texture_wrap_mode
{
    RF_WRAP_REPEAT = 0, // Repeats texture in tiled mode
    RF_WRAP_CLAMP, // Clamps texture to edge pixel in tiled mode
    RF_WRAP_MIRROR_REPEAT, // Mirrors and repeats the texture in tiled mode
    RF_WRAP_MIRROR_CLAMP // Mirrors and clamps to border the texture in tiled mode
} rf_texture_wrap_mode;

// rf_color blending modes (pre-defined)
typedef enum rf_blend_mode
{
    RF_BLEND_ALPHA = 0, // Blend textures considering alpha (default)
    RF_BLEND_ADDITIVE, // Blend textures adding colors
    RF_BLEND_MULTIPLIED // Blend textures multiplying colors
} rf_blend_mode;

// Camera system modes
typedef enum rf_camera3d_mode
{
    RF_CAMERA_CUSTOM = 0,
    RF_CAMERA_FREE,
    RF_CAMERA_ORBITAL,
    RF_CAMERA_FIRST_PERSON,
    RF_CAMERA_THIRD_PERSON
} rf_camera3d_mode;

// Camera GL_PROJECTION modes
typedef enum rf_camera_type
{
    RF_CAMERA_PERSPECTIVE = 0,
    RF_CAMERA_ORTHOGRAPHIC
} rf_camera_type;

// Type of n-patch
typedef enum rf_ninepatch_type
{
    RF_NPT_9PATCH = 0,       // Npatch defined by 3x3 tiles
    RF_NPT_3PATCH_VERTICAL,  // Npatch defined by 1x3 tiles
    RF_NPT_3PATCH_HORIZONTAL // Npatch defined by 3x1 tiles
} rf_ninepatch_type;

// rf_font type, defines generation method
typedef enum rf_font_antialias
{
    RF_FONT_ANTIALIAS = 0, // Default font generation, anti-aliased
    RF_FONT_NO_ANTIALIAS,  // Bitmap font generation, no anti-aliasing
} rf_font_antialias;

#pragma endregion

#pragma region structs

#pragma region math structs

typedef struct rf_sizei rf_sizei;
struct rf_sizei
{
    int width;
    int height;
};

typedef struct rf_sizef rf_sizef;
struct rf_sizef
{
    float width;
    float height;
};

typedef union rf_vec2 rf_vec2;
union rf_vec2
{
    float v[2];

    struct
    {
        float x;
        float y;
    };
};

typedef union rf_vec3 rf_vec3;
union rf_vec3
{
    float v[3];

    struct
    {
        float x;
        float y;
        float z;
    };
};

typedef struct rf_vec4 rf_vec4;
typedef struct rf_vec4 rf_quaternion;
struct rf_vec4
{
    float v[4];

    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
};

// The matrix is OpenGL style 4x4 - right handed, column major
typedef struct rf_mat rf_mat;
struct rf_mat
{
    float m0, m4, m8,  m12;
    float m1, m5, m9,  m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
};

typedef struct rf_float16 rf_float16;
struct rf_float16
{
    float v[16];
};

typedef union rf_rec rf_rec;
union rf_rec
{
    struct
    {
        float x;
        float y;
        float width;
        float height;
    };

    struct
    {
        rf_vec2 pos;
        rf_sizef size;
    };

    rf_vec4 v;
};

#pragma endregion

// R8G8B8A8 format
typedef union rf_color
{
    unsigned char rgba[4];

    struct
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
} rf_color;

typedef struct rf_palette
{
    rf_color* colors;
    int count;
} rf_palette;

typedef struct rf_image
{
    void*           data;    // image raw data
    int             width;   // image base width
    int             height;  // image base height
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    bool            valid;   // True if the image is valid and can be used
} rf_image;

typedef struct rf_mipmaps_stats
{
    int possible_mip_counts;
    int mipmaps_buffer_size;
} rf_mipmaps_stats;

typedef struct rf_mipmaps_image
{
    union
    {
        rf_image image;
        struct
        {
            void*           data;    // image raw data
            int             width;   // image base width
            int             height;  // image base height
            rf_pixel_format format;  // Data format (rf_pixel_format type)
            bool            valid;
        };
    };

    int mipmaps; // Mipmap levels, 1 by default
} rf_mipmaps_image;

typedef struct rf_gif
{
    int frames_count;
    int* frame_delays;

    union
    {
        rf_image image;

        struct
        {
            void*           data;    // rf_image raw data
            int             width;   // rf_image base width
            int             height;  // rf_image base height
            rf_pixel_format format;  // Data format (rf_pixel_format type)
            bool         valid;
        };
    };
} rf_gif;

typedef struct rf_gfx_pixel_format
{
    unsigned int internal_format;
    unsigned int format;
    unsigned int type;
    bool valid;
} rf_gfx_pixel_format;

typedef struct rf_texture2d
{
    unsigned int id;        // OpenGL texture id
    int width;              // rf_texture base width
    int height;             // rf_texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    rf_pixel_format format; // Data format (rf_pixel_format type)
    bool valid;
} rf_texture2d, rf_texture_cubemap;

typedef struct rf_render_texture2d
{
    unsigned int id;            // OpenGL Framebuffer Object (FBO) id
    rf_texture2d texture;       // rf_color buffer attachment texture
    rf_texture2d depth;         // Depth buffer attachment texture
    int          depth_texture; // Track if depth attachment is a texture or renderbuffer
} rf_render_texture2d;

typedef struct rf_npatch_info
{
    rf_rec source_rec;   // Region in the texture
    int left;            // left border offset
    int top;             // top border offset
    int right;           // right border offset
    int bottom;          // bottom border offset
    int type;            // layout of the n-patch: 3x3, 1x3 or 3x1
} rf_npatch_info;

typedef uint32_t rf_rune;

typedef struct rf_decoded_utf8_stats
{
    int bytes_processed;
    int invalid_bytes;
    int valid_rune_count;
    int total_rune_count;
} rf_decoded_utf8_stats;

typedef struct rf_decoded_rune
{
    rf_rune  codepoint;
    int      bytes_processed;
    bool     valid;
} rf_decoded_rune;

typedef struct rf_decoded_string
{
    rf_rune* codepoints;
    int      size;
    int      invalid_bytes_count;
    bool     valid;
} rf_decoded_string;

typedef struct rf_glyph_info
{
    union
    {
        rf_rec rec;    // Characters rectangles in texture
        struct { float x, y, width, height; };
    };

    int codepoint;   // Character value (Unicode)
    int offset_x;    // Character offset X when drawing
    int offset_y;    // Character offset Y when drawing
    int advance_x;   // Character advance position X
} rf_glyph_info;

typedef struct rf_ttf_font_info
{
    // Font details
    const void* ttf_data;
    int         font_size;
    int         largest_glyph_size;

    // Font metrics
    float scale_factor;
    int   ascent;
    int   descent;
    int   line_gap;

    // Take directly from stb_truetype because we don't want to include it's heaeder in our public API
    struct
    {
        void* userdata;
        unsigned char* data;
        int fontstart;
        int numGlyphs;
        int loca, head, glyf, hhea, hmtx, kern, gpos, svg;
        int index_map;
        int indexToLocFormat;

        struct
        {
            unsigned char* data;
            int cursor;
            int size;
        } cff, charstrings, gsubrs, subrs, fontdicts, fdselect;
    } internal_stb_font_info;

    bool valid;
} rf_ttf_font_info;

typedef struct rf_font
{
    int            base_size;
    rf_texture2d   texture;
    rf_glyph_info* glyphs;
    int            glyphs_count;
    bool        valid;
} rf_font;

typedef int rf_glyph_index;

typedef struct rf_camera3d
{
    rf_camera_type type;     // Camera type, defines GL_PROJECTION type: RF_CAMERA_PERSPECTIVE or RF_CAMERA_ORTHOGRAPHIC
    rf_vec3        position; // Camera position
    rf_vec3        target;   // Camera target it looks-at
    rf_vec3        up;       // Camera up vector (rotation over its axis)
    float          fovy;     // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
} rf_camera3d;

typedef struct rf_camera2d
{
    rf_vec2 offset;   // Camera offset (displacement from target)
    rf_vec2 target;   // Camera target (rotation and zoom origin)
    float   rotation; // Camera rotation in degrees
    float   zoom;     // Camera zoom (scaling), should be 1.0f by default
} rf_camera2d;

typedef struct rf_mesh
{
    int vertex_count; // Number of vertices stored in arrays
    int triangle_count; // Number of triangles stored (indexed or not)

    // Default vertex data
    float* vertices;         // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float* texcoords;        // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float* texcoords2;       // Vertex second texture coordinates (useful for lightmaps) (shader-location = 5)
    float* normals;          // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
    float* tangents;         // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
    unsigned char* colors;   // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    unsigned short* indices; // Vertex indices (in case vertex data comes indexed)

    // Animation vertex data
    float* anim_vertices; // Animated vertex positions (after bones transformations)
    float* anim_normals;  // Animated normals (after bones transformations)
    int*   bone_ids;      // Vertex bone ids, up to 4 bones influence by vertex (skinning)
    float* bone_weights;  // Vertex bone weight, up to 4 bones influence by vertex (skinning)

    // OpenGL identifiers
    unsigned int  vao_id; // OpenGL Vertex Array Object id
    unsigned int* vbo_id; // OpenGL Vertex Buffer Objects id (default vertex data)
} rf_mesh;

typedef struct rf_input_state_for_update_camera
{
    rf_vec2     mouse_position;
    int         mouse_wheel_move;                  //mouse wheel movement Y
    bool     is_camera_pan_control_key_down;    //MOUSE_MIDDLE_BUTTON
    bool     is_camera_alt_control_key_down;    //KEY_LEFT_ALT
    bool     is_camera_smooth_zoom_control_key; //KEY_LEFT_CONTROL
    bool     direction_keys[6];                 //'W', 'S', 'D', 'A', 'E', 'Q'
} rf_input_state_for_update_camera;

typedef struct rf_shader
{
    unsigned int id; // rf_shader program id
    int locs[RF_MAX_SHADER_LOCATIONS];       // rf_shader locations array (RF_MAX_SHADER_LOCATIONS)
} rf_shader;

typedef struct rf_material_map
{
    rf_texture2d texture; // rf_material map texture
    rf_color color;       // rf_material map color
    float value;       // rf_material map value
} rf_material_map;

typedef struct rf_material
{
    rf_shader shader;     // rf_material shader
    rf_material_map* maps; // rf_material maps array (RF_MAX_MATERIAL_MAPS)
    float* params;     // rf_material generic parameters (if required)
} rf_material;

typedef struct rf_transform
{
    rf_vec3 translation; // Translation
    rf_quaternion rotation; // Rotation
    rf_vec3 scale;       // Scale
} rf_transform;

typedef struct rf_bone_info
{
    char name[32]; // Bone name
    int  parent;   // Bone parent
} rf_bone_info;

typedef struct rf_model
{
    rf_mat transform; // Local transform matrix
    int mesh_count;    // Number of meshes
    rf_mesh* meshes;     // Meshes array

    int material_count;   // Number of materials
    rf_material* materials; // Materials array
    int* mesh_material;   // rf_mesh material number

    // Animation data
    int bone_count;       // Number of bones
    rf_bone_info* bones;     // Bones information (skeleton)
    rf_transform* bind_pose; // Bones base transformation (pose)
} rf_model;

typedef struct rf_model_animation
{
    int bone_count;             // Number of bones
    rf_bone_info* bones;        // Bones information (skeleton)
    int frame_count;            // Number of animation frames
    rf_transform** frame_poses; // Poses array by frame
} rf_model_animation;

typedef struct rf_model_animation_array
{
    int                 size;
    rf_model_animation* anims;
} rf_model_animation_array;

typedef struct rf_materials_array
{
    int size;
    rf_material* materials;
} rf_materials_array;

typedef struct rf_ray
{
    rf_vec3 position;  // position (origin)
    rf_vec3 direction; // direction
} rf_ray;

typedef struct rf_ray_hit_info
{
    bool hit; // Did the ray hit something?
    float distance; // Distance to nearest hit
    rf_vec3 position; // Position of nearest hit
    rf_vec3 normal; // Surface normal of hit
} rf_ray_hit_info;

typedef struct rf_bounding_box
{
    rf_vec3 min; // Minimum vertex box-corner
    rf_vec3 max; // Maximum vertex box-corner
} rf_bounding_box;

typedef struct rf_base64_output
{
    int size;
    unsigned char* buffer;
} rf_base64_output;

typedef struct rf_camera3d_state
{
    rf_vec2 camera_angle;         // rf_camera3d angle in plane XZ
    float camera_target_distance; // rf_camera3d distance from position to target
    float player_eyes_position;
    rf_camera3d_mode camera_mode; // Current camera mode
    int swing_counter; // Used for 1st person swinging movement
    rf_vec2 previous_mouse_position;
} rf_camera3d_state;
#pragma endregion

#pragma region graphics backends and context struct

#pragma region gl backend structs
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)

#pragma region opengl procs macro
#if !defined(RF_GL_CALLING_CONVENTION) && (defined(_WIN32) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__))
    #define RF_GL_CALLING_CONVENTION __stdcall *
#else
    #define RF_GL_CALLING_CONVENTION *
#endif

typedef struct rf_opengl_procs rf_opengl_procs;
struct rf_opengl_procs
{
    void (RF_GL_CALLING_CONVENTION Viewport)(int x, int y, int width, int height);
    void (RF_GL_CALLING_CONVENTION BindTexture)(unsigned int target, unsigned int texture);
    void (RF_GL_CALLING_CONVENTION TexParameteri)(unsigned int target, unsigned int pname, int param);
    void (RF_GL_CALLING_CONVENTION TexParameterf)(unsigned int target, unsigned int pname, float param);
    void (RF_GL_CALLING_CONVENTION TexParameteriv)(unsigned int target, unsigned int pname, const int* params);
    void (RF_GL_CALLING_CONVENTION BindFramebuffer)(unsigned int target, unsigned int framebuffer);
    void (RF_GL_CALLING_CONVENTION Enable)(unsigned int cap);
    void (RF_GL_CALLING_CONVENTION Disable)(unsigned int cap);
    void (RF_GL_CALLING_CONVENTION Scissor)(int x, int y, int width, int height);
    void (RF_GL_CALLING_CONVENTION DeleteTextures)(int n, const unsigned int* textures);
    void (RF_GL_CALLING_CONVENTION DeleteRenderbuffers)(int n, const unsigned int* renderbuffers);
    void (RF_GL_CALLING_CONVENTION DeleteFramebuffers)(int n, const unsigned int* framebuffers);
    void (RF_GL_CALLING_CONVENTION DeleteVertexArrays)(int n, const unsigned int* arrays);
    void (RF_GL_CALLING_CONVENTION DeleteBuffers)(int n, const unsigned int* buffers);
    void (RF_GL_CALLING_CONVENTION ClearColor)(float red, float green, float blue, float alpha);
    void (RF_GL_CALLING_CONVENTION Clear)(unsigned int mask);
    void (RF_GL_CALLING_CONVENTION BindBuffer)(unsigned int target, unsigned int buffer);
    void (RF_GL_CALLING_CONVENTION BufferSubData)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data);
    void (RF_GL_CALLING_CONVENTION BindVertexArray)(unsigned int array);
    void (RF_GL_CALLING_CONVENTION GenBuffers)(int n, unsigned int* buffers);
    void (RF_GL_CALLING_CONVENTION BufferData)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
    void (RF_GL_CALLING_CONVENTION VertexAttribPointer)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
    void (RF_GL_CALLING_CONVENTION EnableVertexAttribArray)(unsigned int index);
    void (RF_GL_CALLING_CONVENTION GenVertexArrays)(int n, unsigned int* arrays);
    void (RF_GL_CALLING_CONVENTION VertexAttrib3f)(unsigned int index, float x, float y, float z);
    void (RF_GL_CALLING_CONVENTION DisableVertexAttribArray)(unsigned int index);
    void (RF_GL_CALLING_CONVENTION VertexAttrib4f)(unsigned int index, float x, float y, float z, float w);
    void (RF_GL_CALLING_CONVENTION VertexAttrib2f)(unsigned int index, float x, float y);
    void (RF_GL_CALLING_CONVENTION UseProgram)(unsigned int program);
    void (RF_GL_CALLING_CONVENTION Uniform4f)(int location, float v0, float v1, float v2, float v3);
    void (RF_GL_CALLING_CONVENTION ActiveTexture)(unsigned int texture);
    void (RF_GL_CALLING_CONVENTION Uniform1i)(int location, int v0);
    void (RF_GL_CALLING_CONVENTION UniformMatrix4fv)(int location, int count, unsigned char transpose, const float* value);
    void (RF_GL_CALLING_CONVENTION DrawElements)(unsigned int mode, int count, unsigned int type, const void* indices);
    void (RF_GL_CALLING_CONVENTION DrawArrays)(unsigned int mode, int first, int count);
    void (RF_GL_CALLING_CONVENTION PixelStorei)(unsigned int pname, int param);
    void (RF_GL_CALLING_CONVENTION GenTextures)(int n, unsigned int* textures);
    void (RF_GL_CALLING_CONVENTION TexImage2D)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
    void (RF_GL_CALLING_CONVENTION GenRenderbuffers)(int n, unsigned int* renderbuffers);
    void (RF_GL_CALLING_CONVENTION BindRenderbuffer)(unsigned int target, unsigned int renderbuffer);
    void (RF_GL_CALLING_CONVENTION RenderbufferStorage)(unsigned int target, unsigned int internalformat, int width, int height);
    void (RF_GL_CALLING_CONVENTION CompressedTexImage2D)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data);
    void (RF_GL_CALLING_CONVENTION TexSubImage2D)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels);
    void (RF_GL_CALLING_CONVENTION GenerateMipmap)(unsigned int target);
    void (RF_GL_CALLING_CONVENTION ReadPixels)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
    void (RF_GL_CALLING_CONVENTION GenFramebuffers)(int n, unsigned int* framebuffers);
    void (RF_GL_CALLING_CONVENTION FramebufferTexture2D)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
    void (RF_GL_CALLING_CONVENTION FramebufferRenderbuffer)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
    unsigned int (RF_GL_CALLING_CONVENTION CheckFramebufferStatus)(unsigned int target);
    unsigned int (RF_GL_CALLING_CONVENTION CreateShader)(unsigned int type);
    void (RF_GL_CALLING_CONVENTION ShaderSource)(unsigned int shader, int count, const char** string, const int* length);
    void (RF_GL_CALLING_CONVENTION CompileShader)(unsigned int shader);
    void (RF_GL_CALLING_CONVENTION GetShaderiv)(unsigned int shader, unsigned int pname, int* params);
    void (RF_GL_CALLING_CONVENTION GetShaderInfoLog)(unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int (RF_GL_CALLING_CONVENTION CreateProgram)();
    void (RF_GL_CALLING_CONVENTION AttachShader)(unsigned int program, unsigned int shader);
    void (RF_GL_CALLING_CONVENTION BindAttribLocation)(unsigned int program, unsigned int index, const char* name);
    void (RF_GL_CALLING_CONVENTION LinkProgram)(unsigned int program);
    void (RF_GL_CALLING_CONVENTION GetProgramiv)(unsigned int program, unsigned int pname, int* params);
    void (RF_GL_CALLING_CONVENTION GetProgramInfoLog)(unsigned int program, int bufSize, int* length, char* infoLog);
    void (RF_GL_CALLING_CONVENTION DeleteProgram)(unsigned int program);
    int (RF_GL_CALLING_CONVENTION GetAttribLocation)(unsigned int program, const char* name);
    int (RF_GL_CALLING_CONVENTION GetUniformLocation)(unsigned int program, const char* name);
    void (RF_GL_CALLING_CONVENTION DetachShader)(unsigned int program, unsigned int shader);
    void (RF_GL_CALLING_CONVENTION DeleteShader)(unsigned int shader);

    void (RF_GL_CALLING_CONVENTION GetTexImage)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels); // NULL for OpenGL ES3

    void (RF_GL_CALLING_CONVENTION GetActiveUniform)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name);
    void (RF_GL_CALLING_CONVENTION Uniform1f)(int location, float v0);
    void (RF_GL_CALLING_CONVENTION Uniform1fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform2fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform3fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform4fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform1iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION Uniform2iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION Uniform3iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION Uniform4iv)(int location, int count, const int* value);
    const unsigned char* (RF_GL_CALLING_CONVENTION GetString)(unsigned int name);
    void (RF_GL_CALLING_CONVENTION GetFloatv)(unsigned int pname, float* data);
    void (RF_GL_CALLING_CONVENTION DepthFunc)(unsigned int func);
    void (RF_GL_CALLING_CONVENTION BlendFunc)(unsigned int sfactor, unsigned int dfactor);
    void (RF_GL_CALLING_CONVENTION CullFace)(unsigned int mode);
    void (RF_GL_CALLING_CONVENTION FrontFace)(unsigned int mode);

    //On OpenGL33 we only set glClearDepth but on OpenGL ES3 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version
    void (RF_GL_CALLING_CONVENTION ClearDepth)(double depth);
    void (RF_GL_CALLING_CONVENTION ClearDepthf)(float depth);

    void (RF_GL_CALLING_CONVENTION GetIntegerv)(unsigned int pname, int* data); // OpenGL 33 only, can be NULL in OpenGL ES3
    const unsigned char* (RF_GL_CALLING_CONVENTION GetStringi)(unsigned int name, unsigned int index);

    void (RF_GL_CALLING_CONVENTION PolygonMode)(unsigned int face, unsigned int mode); // OpenGL 33 only
};

#define RF_OPENGL_PROC_EXT(ext, gl_proc_name) RF_CONCAT(ext, gl_proc_name)

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
#define RF_DEFAULT_OPENGL_PROCS_EXT(ext) (rf_opengl_procs) { \
    ((void (RF_GL_CALLING_CONVENTION )(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Viewport)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int texture))RF_OPENGL_PROC_EXT(ext, BindTexture)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, TexParameteri)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int pname, float param))RF_OPENGL_PROC_EXT(ext, TexParameterf)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int pname, const int* params))RF_OPENGL_PROC_EXT(ext, TexParameteriv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int framebuffer))RF_OPENGL_PROC_EXT(ext, BindFramebuffer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Enable)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Disable)),\
    ((void (RF_GL_CALLING_CONVENTION )(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Scissor)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* textures))RF_OPENGL_PROC_EXT(ext, DeleteTextures)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, DeleteRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, DeleteFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, DeleteVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, DeleteBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(float red, float green, float blue, float alpha))RF_OPENGL_PROC_EXT(ext, ClearColor)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mask))RF_OPENGL_PROC_EXT(ext, Clear)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int buffer))RF_OPENGL_PROC_EXT(ext, BindBuffer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data))RF_OPENGL_PROC_EXT(ext, BufferSubData)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int array))RF_OPENGL_PROC_EXT(ext, BindVertexArray)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, GenBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage))RF_OPENGL_PROC_EXT(ext, BufferData)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer))RF_OPENGL_PROC_EXT(ext, VertexAttribPointer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index))RF_OPENGL_PROC_EXT(ext, EnableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, GenVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, float x, float y, float z))RF_OPENGL_PROC_EXT(ext, VertexAttrib3f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index))RF_OPENGL_PROC_EXT(ext, DisableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, float x, float y, float z, float w))RF_OPENGL_PROC_EXT(ext, VertexAttrib4f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, float x, float y))RF_OPENGL_PROC_EXT(ext, VertexAttrib2f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program))RF_OPENGL_PROC_EXT(ext, UseProgram)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, float v0, float v1, float v2, float v3))RF_OPENGL_PROC_EXT(ext, Uniform4f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int texture))RF_OPENGL_PROC_EXT(ext, ActiveTexture)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int v0))RF_OPENGL_PROC_EXT(ext, Uniform1i)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, unsigned char transpose, const float* value))RF_OPENGL_PROC_EXT(ext, UniformMatrix4fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode, int count, unsigned int type, const void* indices))RF_OPENGL_PROC_EXT(ext, DrawElements)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode, int first, int count))RF_OPENGL_PROC_EXT(ext, DrawArrays)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, PixelStorei)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* textures))RF_OPENGL_PROC_EXT(ext, GenTextures)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, GenRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, BindRenderbuffer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int internalformat, int width, int height))RF_OPENGL_PROC_EXT(ext, RenderbufferStorage)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data))RF_OPENGL_PROC_EXT(ext, CompressedTexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexSubImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target))RF_OPENGL_PROC_EXT(ext, GenerateMipmap)),\
    ((void (RF_GL_CALLING_CONVENTION )(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, ReadPixels)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, GenFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level))RF_OPENGL_PROC_EXT(ext, FramebufferTexture2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, FramebufferRenderbuffer)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION )(unsigned int target))RF_OPENGL_PROC_EXT(ext, CheckFramebufferStatus)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION )(unsigned int type))RF_OPENGL_PROC_EXT(ext, CreateShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader, int count, const char** string, const int* length))RF_OPENGL_PROC_EXT(ext, ShaderSource)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader))RF_OPENGL_PROC_EXT(ext, CompileShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetShaderiv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetShaderInfoLog)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION )())RF_OPENGL_PROC_EXT(ext, CreateProgram)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, AttachShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int index, const char* name))RF_OPENGL_PROC_EXT(ext, BindAttribLocation)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program))RF_OPENGL_PROC_EXT(ext, LinkProgram)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetProgramiv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetProgramInfoLog)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program))RF_OPENGL_PROC_EXT(ext, DeleteProgram)),\
    ((int (RF_GL_CALLING_CONVENTION )(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetAttribLocation)),\
    ((int (RF_GL_CALLING_CONVENTION )(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetUniformLocation)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, DetachShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader))RF_OPENGL_PROC_EXT(ext, DeleteShader)),\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, GetTexImage)),\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name))RF_OPENGL_PROC_EXT(ext, GetActiveUniform)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, float v0))RF_OPENGL_PROC_EXT(ext, Uniform1f)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform1fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform2fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform3fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform4fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform1iv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform2iv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform3iv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform4iv)),\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION )(unsigned int name))RF_OPENGL_PROC_EXT(ext, GetString)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int pname, float* data))RF_OPENGL_PROC_EXT(ext, GetFloatv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int func))RF_OPENGL_PROC_EXT(ext, DepthFunc)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int sfactor, unsigned int dfactor))RF_OPENGL_PROC_EXT(ext, BlendFunc)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode))RF_OPENGL_PROC_EXT(ext, CullFace)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode))RF_OPENGL_PROC_EXT(ext, FrontFace)),\
    \
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/\
    ((void (RF_GL_CALLING_CONVENTION )(double depth))RF_OPENGL_PROC_EXT(ext, ClearDepth)),\
    NULL /*glClearDepthf*/,\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int pname, int* data))RF_OPENGL_PROC_EXT(ext, GetIntegerv)),\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION )(unsigned int name, unsigned int index))RF_OPENGL_PROC_EXT(ext, GetStringi)),\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int face, unsigned int mode))RF_OPENGL_PROC_EXT(ext, PolygonMode)), /*OpenGL 33 only*/ \
}

#else
#define RF_DEFAULT_OPENGL_PROCS_EXT(ext) (rf_opengl_procs) { \
    ((void (RF_GL_CALLING_CONVENTION)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Viewport)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int texture))RF_OPENGL_PROC_EXT(ext, BindTexture)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, TexParameteri)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int pname, float param))RF_OPENGL_PROC_EXT(ext, TexParameterf)),\
    NULL, /*glTexParameteri not needed for OpenGL ES3*/ \
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int framebuffer))RF_OPENGL_PROC_EXT(ext, BindFramebuffer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Enable)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Disable)),\
    ((void (RF_GL_CALLING_CONVENTION)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Scissor)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* textures))RF_OPENGL_PROC_EXT(ext, DeleteTextures)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, DeleteRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, DeleteFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, DeleteVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* vertex_buffers))RF_OPENGL_PROC_EXT(ext, DeleteBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(float red, float green, float blue, float alpha))RF_OPENGL_PROC_EXT(ext, ClearColor)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mask))RF_OPENGL_PROC_EXT(ext, Clear)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int buffer))RF_OPENGL_PROC_EXT(ext, BindBuffer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data))RF_OPENGL_PROC_EXT(ext, BufferSubData)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int array))RF_OPENGL_PROC_EXT(ext, BindVertexArray)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* vertex_buffers))RF_OPENGL_PROC_EXT(ext, GenBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage))RF_OPENGL_PROC_EXT(ext, BufferData)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer))RF_OPENGL_PROC_EXT(ext, VertexAttribPointer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index))RF_OPENGL_PROC_EXT(ext, EnableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, GenVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, float x, float y, float z))RF_OPENGL_PROC_EXT(ext, VertexAttrib3f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index))RF_OPENGL_PROC_EXT(ext, DisableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, float x, float y, float z, float w))RF_OPENGL_PROC_EXT(ext, VertexAttrib4f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, float x, float y))RF_OPENGL_PROC_EXT(ext, VertexAttrib2f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program))RF_OPENGL_PROC_EXT(ext, UseProgram)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, float v0, float v1, float v2, float v3))RF_OPENGL_PROC_EXT(ext, Uniform4f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int texture))RF_OPENGL_PROC_EXT(ext, ActiveTexture)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int v0))RF_OPENGL_PROC_EXT(ext, Uniform1i)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, unsigned char transpose, const float* value))RF_OPENGL_PROC_EXT(ext, UniformMatrix4fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode, int count, unsigned int type, const void* indices))RF_OPENGL_PROC_EXT(ext, DrawElements)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode, int first, int count))RF_OPENGL_PROC_EXT(ext, DrawArrays)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, PixelStorei)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* textures))RF_OPENGL_PROC_EXT(ext, GenTextures)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, GenRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, BindRenderbuffer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int internalformat, int width, int height))RF_OPENGL_PROC_EXT(ext, RenderbufferStorage)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data))RF_OPENGL_PROC_EXT(ext, CompressedTexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexSubImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target))RF_OPENGL_PROC_EXT(ext, GenerateMipmap)),\
    ((void (RF_GL_CALLING_CONVENTION)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, ReadPixels)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, GenFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level))RF_OPENGL_PROC_EXT(ext, FramebufferTexture2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, FramebufferRenderbuffer)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION)(unsigned int target))RF_OPENGL_PROC_EXT(ext, CheckFramebufferStatus)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION)(unsigned int type))RF_OPENGL_PROC_EXT(ext, CreateShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader, int count, const char** string, const int* length))RF_OPENGL_PROC_EXT(ext, ShaderSource)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, CompileShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetShaderiv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetShaderInfoLog)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION)())RF_OPENGL_PROC_EXT(ext, CreateProgram)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, AttachShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int index, const char* name))RF_OPENGL_PROC_EXT(ext, BindAttribLocation)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program))RF_OPENGL_PROC_EXT(ext, LinkProgram)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetProgramiv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetProgramInfoLog)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program))RF_OPENGL_PROC_EXT(ext, DeleteProgram)),\
    ((int (RF_GL_CALLING_CONVENTION)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetAttribLocation)),\
    ((int (RF_GL_CALLING_CONVENTION)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetUniformLocation)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, DetachShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, DeleteShader)),\
    NULL, /*glGetTexImage is NULL for opengl es3*/ \
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name))RF_OPENGL_PROC_EXT(ext, GetActiveUniform)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, float v0))RF_OPENGL_PROC_EXT(ext, Uniform1f)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform1fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform2fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform3fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform4fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform1iv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform2iv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform3iv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform4iv)),\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION)(unsigned int name))RF_OPENGL_PROC_EXT(ext, GetString)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int pname, float* data))RF_OPENGL_PROC_EXT(ext, GetFloatv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int func))RF_OPENGL_PROC_EXT(ext, DepthFunc)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int sfactor, unsigned int dfactor))RF_OPENGL_PROC_EXT(ext, BlendFunc)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, CullFace)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, FrontFace)),\
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/\
    NULL /*glClearDepth*/,\
    ((void (RF_GL_CALLING_CONVENTION)(float depth))RF_OPENGL_PROC_EXT(ext, ClearDepthf)),\
    NULL /*OpenGL 33 only, should be NULL in OpenGL ES3*/,\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION)(unsigned int name, unsigned int index))RF_OPENGL_PROC_EXT(ext, GetStringi)),\
    NULL /*glPolygonMode is OpenGL 33 only*/ \
}
#endif

#define RF_DEFAULT_OPENGL_PROCS RF_DEFAULT_OPENGL_PROCS_EXT(gl)
#define RF_DEFAULT_GFX_BACKEND_INIT_DATA &RF_DEFAULT_OPENGL_PROCS

#pragma endregion

typedef float rf_gfx_vertex_data_type;
typedef float rf_gfx_texcoord_data_type;
typedef unsigned char rf_gfx_color_data_type;
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
typedef unsigned int rf_gfx_vertex_index_data_type;
#else
typedef unsigned short rf_gfx_vertex_index_data_type;
#endif

#define RF_GFX_VERTEX_COMPONENT_COUNT       (3 * 4) // 3 float by vertex, 4 vertex by quad
#define RF_GFX_TEXCOORD_COMPONENT_COUNT     (2 * 4) // 2 float by texcoord, 4 texcoord by quad
#define RF_GFX_COLOR_COMPONENT_COUNT        (4 * 4) // 4 float by color, 4 colors by quad
#define RF_GFX_VERTEX_INDEX_COMPONENT_COUNT (6)     // 6 int by quad (indices)

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rf_vertex_buffer
{
    int elements_count; // Number of elements in the buffer (QUADS)
    int v_counter;      // Vertex position counter to process (and draw) from full buffer
    int tc_counter;     // Vertex texcoord counter to process (and draw) from full buffer
    int c_counter;      // Vertex color counter to process (and draw) from full buffer

    unsigned int vao_id;    // OpenGL Vertex Array Object id
    unsigned int vbo_id[4]; // OpenGL Vertex Buffer Objects id (4 types of vertex data)

    rf_gfx_vertex_data_type*       vertices;   // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    rf_gfx_texcoord_data_type*     texcoords;  // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    rf_gfx_color_data_type*        colors;     // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    rf_gfx_vertex_index_data_type* indices;    // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
} rf_vertex_buffer;

typedef struct rf_draw_call
{
    rf_drawing_mode mode;    // Drawing mode: RF_LINES, RF_TRIANGLES, RF_QUADS
    int vertex_count;        // Number of vertex of the draw
    int vertex_alignment;    // Number of vertex required for index alignment (LINES, TRIANGLES)
    //unsigned int vao_id;   // Vertex array id to be used on the draw
    //unsigned int shaderId; // rf_shader id to be used on the draw
    unsigned int texture_id; // rf_texture id to be used on the draw
    // TODO: Support additional texture units?

    //rf_mat projection;     // Projection matrix for this draw
    //rf_mat modelview;      // Modelview matrix for this draw
} rf_draw_call;

typedef struct rf_gfx_context
{
    rf_opengl_procs gl;

    struct {
        bool tex_comp_dxt_supported;             // DDS texture compression support
        bool tex_comp_etc1_supported;            // ETC1 texture compression support
        bool tex_comp_etc2_supported;            // ETC2/EAC texture compression support
        bool tex_comp_pvrt_supported;            // PVR texture compression support
        bool tex_comp_astc_supported;            // ASTC texture compression support
        bool tex_npot_supported;                 // NPOT textures full support
        bool tex_float_supported;                // float textures support (32 bit per channel)
        bool tex_depth_supported;                // Depth textures supported
        int max_depth_bits;                      // Maximum bits for depth component
        bool tex_mirror_clamp_supported;         // Clamp mirror wrap mode supported
        bool tex_anisotropic_filter_supported;   // Anisotropic texture filtering support
        float max_anisotropic_level;             // Maximum anisotropy level supported (minimum is 2.0f)
        bool debug_marker_supported;             // Debug marker support
    } extensions;
} rf_gfx_context;

#endif // defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)

#pragma endregion

typedef void rf_gfx_backend_init_data;

typedef struct rf_render_batch
{
    int vertex_buffers_count;
    int current_buffer;
    rf_vertex_buffer* vertex_buffers;

    int draw_calls_size;
    int draw_calls_counter;
    rf_draw_call* draw_calls;
    float current_depth; // Current depth value for next draw

    bool valid;
} rf_render_batch;

typedef struct rf_one_element_vertex_buffer
    {
        rf_gfx_vertex_data_type vertices[1 *RF_GFX_VERTEX_COMPONENT_COUNT];
        rf_gfx_texcoord_data_type texcoords[1 * RF_GFX_TEXCOORD_COMPONENT_COUNT];
        rf_gfx_color_data_type colors[1 * RF_GFX_COLOR_COMPONENT_COUNT];
        rf_gfx_vertex_index_data_type indices[1 * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT];
    } rf_one_element_vertex_buffer;

typedef struct rf_default_vertex_buffer
{
    rf_gfx_vertex_data_type vertices[RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_VERTEX_COMPONENT_COUNT];
    rf_gfx_texcoord_data_type texcoords[RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_TEXCOORD_COMPONENT_COUNT];
    rf_gfx_color_data_type colors[RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_COLOR_COMPONENT_COUNT];
    rf_gfx_vertex_index_data_type indices[RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT];
} rf_default_vertex_buffer;

typedef struct rf_default_render_batch
{
    rf_vertex_buffer vertex_buffers[RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT];
    rf_draw_call      draw_calls[RF_DEFAULT_BATCH_DRAW_CALLS_COUNT];
    rf_default_vertex_buffer vertex_buffers_memory[RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT];
} rf_default_render_batch;

typedef struct rf_default_font
{
    unsigned short pixels[128 * 128]; // Default font buffer
    rf_glyph_info   chars[RF_BUILTIN_FONT_CHARS_COUNT];
    unsigned short chars_pixels[128 * 128];
} rf_default_font;

typedef struct rf_context
{
    int current_width;
    int current_height;

    int render_width;
    int render_height;

    rf_mat screen_scaling;

    rf_render_batch* current_batch;

    int     current_matrix_mode;
    rf_mat* current_matrix;
    rf_mat  modelview;
    rf_mat  projection;
    rf_mat  transform;
    bool    transform_matrix_required;
    rf_mat  stack[RF_MAX_MATRIX_STACK_SIZE];
    int     stack_counter;

    unsigned int default_texture_id;       // Default texture (1px white) useful for plain color polys (required by shader)
    unsigned int default_vertex_shader_id; // Default vertex shader id (used by default shader program)
    unsigned int default_frag_shader_id;   // Default fragment shader Id (used by default shader program)

    rf_shader default_shader; // Basic shader, support vertex color and diffuse texture
    rf_shader current_shader; // Shader to be used on rendering (by default, default_shader)

    rf_blend_mode blend_mode; // Track current blending mode

    int framebuffer_width;  // Default framebuffer width
    int framebuffer_height; // Default framebuffer height

    rf_gfx_context gfx_ctx;

    rf_log_proc log;
    rf_log_type log_filter;

    // Shapes global data
    rf_texture2d tex_shapes;
    rf_rec rec_tex_shapes;

    rf_font default_font;
    rf_default_font default_font_buffers;
} rf_context;

#pragma endregion

#pragma region init
RF_API void rf_init(rf_context* ctx, int screen_width, int screen_height, rf_log_proc logger, rf_gfx_backend_init_data* gfx_data);
#pragma endregion

#pragma region defaults
RF_API rf_material rf_load_default_material(rf_allocator allocator); // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
RF_API rf_shader rf_load_default_shader();
#pragma endregion

#pragma region render batch
RF_API rf_render_batch rf_create_custom_render_batch_from_buffers(rf_vertex_buffer* vertex_buffers, int vertex_buffers_count, rf_draw_call* draw_calls, int draw_calls_count);
RF_API rf_render_batch rf_create_custom_render_batch(int vertex_buffers_count, int draw_calls_count, int vertex_buffer_elements_count, rf_allocator allocator);
RF_API rf_render_batch rf_create_default_render_batch(rf_allocator allocator);

RF_API void rf_set_active_render_batch(rf_render_batch* batch);
RF_API void rf_unload_render_batch(rf_render_batch batch, rf_allocator allocator);
#pragma endregion

#pragma region getters
RF_API rf_render_batch* rf_get_current_render_batch(); // Return a pointer to the current render batch
RF_API rf_font rf_get_default_font(); // Get the default font, useful to be used with extended parameters
RF_API rf_shader rf_get_default_shader(); // Get default shader
RF_API rf_texture2d rf_get_default_texture(); // Get default internal texture (white texture)
RF_API rf_context* rf_get_context(); //Get the context pointer
RF_API rf_image rf_get_screen_data(rf_color* dst, int dst_size); // Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
#pragma endregion

#pragma region setters
RF_API void rf_set_global_context_pointer(rf_context* ctx); // Set the global context pointer
RF_API void rf_set_viewport(int width, int height); // Set viewport for a provided width and height
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes
#pragma endregion

#pragma region math

#pragma region misc
RF_API float rf_next_pot(float it);
RF_API rf_vec2 rf_center_to_screen(float w, float h); // Returns the position of an object such that it will be centered to the screen
RF_API rf_vec2 rf_center_to_object(rf_sizef center_this, rf_rec to_this); // Returns the position of an object such that it will be centered to a rectangle
RF_API float rf_clamp(float value, float min, float max); // Clamp float value
RF_API float rf_lerp(float start, float end, float amount); // Calculate linear interpolation between two floats
#pragma endregion

#pragma region base64
RF_API int rf_get_size_base64(const unsigned char* input);
RF_API rf_base64_output rf_decode_base64(const unsigned char* input, rf_allocator allocator);
#pragma endregion

#pragma region color
RF_API bool rf_color_equal_rgb(rf_color a, rf_color b); // Returns true if the two colors have the same values for the rgb components
RF_API bool rf_color_equal(rf_color a, rf_color b); // Returns true if the two colors have the same values
RF_API int rf_color_to_int(rf_color color); // Returns hexadecimal value for a rf_color
RF_API rf_vec4 rf_color_normalize(rf_color color); // Returns color normalized as float [0..1]
RF_API rf_color rf_color_from_normalized(rf_vec4 normalized); // Returns color from normalized values [0..1]
RF_API rf_vec3 rf_color_to_hsv(rf_color color); // Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
RF_API rf_color rf_color_from_hsv(rf_vec3 hsv); // Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
RF_API rf_color rf_color_from_int(int hex_value); // Returns a rf_color struct from hexadecimal value
RF_API rf_color rf_fade(rf_color color, float alpha); // rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
#pragma endregion

#pragma region camera
RF_API rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view); // Get world coordinates from screen coordinates
RF_API rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera); // Returns a ray trace from mouse position
RF_API rf_mat rf_get_camera_matrix(rf_camera3d camera); // Get transform matrix for camera
RF_API rf_mat rf_get_camera_matrix2d(rf_camera2d camera); // Returns camera 2d transform matrix
RF_API rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera); // Returns the screen space position from a 3d world space position
RF_API rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera); // Returns the screen space position for a 2d camera world space position
RF_API rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera); // Returns the world space position for a 2d camera screen space position
RF_API void rf_set_camera3d_mode(rf_camera3d_state* state, rf_camera3d camera, rf_camera3d_mode mode);
RF_API void rf_update_camera3d(rf_camera3d* camera, rf_camera3d_state* state, rf_input_state_for_update_camera input_state);
#pragma endregion

#pragma region vec and matrix math

RF_API rf_vec2 rf_vec2_add(rf_vec2 v1, rf_vec2 v2); // Add two vectors (v1 + v2)
RF_API rf_vec2 rf_vec2_sub(rf_vec2 v1, rf_vec2 v2); // Subtract two vectors (v1 - v2)
RF_API float rf_vec2_len(rf_vec2 v); // Calculate vector length
RF_API float rf_vec2_dot_product(rf_vec2 v1, rf_vec2 v2); // Calculate two vectors dot product
RF_API float rf_vec2_distance(rf_vec2 v1, rf_vec2 v2); // Calculate distance between two vectors
RF_API float rf_vec2_angle(rf_vec2 v1, rf_vec2 v2); // Calculate angle from two vectors in X-axis
RF_API rf_vec2 rf_vec2_scale(rf_vec2 v, float scale); // Scale vector (multiply by value)
RF_API rf_vec2 rf_vec2_mul_v(rf_vec2 v1, rf_vec2 v2); // Multiply vector by vector
RF_API rf_vec2 rf_vec2_negate(rf_vec2 v); // Negate vector
RF_API rf_vec2 rf_vec2_div(rf_vec2 v, float div); // Divide vector by a float value
RF_API rf_vec2 rf_vec2_div_v(rf_vec2 v1, rf_vec2 v2); // Divide vector by vector
RF_API rf_vec2 rf_vec2_normalize(rf_vec2 v); // Normalize provided vector
RF_API rf_vec2 rf_vec2_lerp(rf_vec2 v1, rf_vec2 v2, float amount); // Calculate linear interpolation between two vectors

RF_API rf_vec3 rf_vec3_add(rf_vec3 v1, rf_vec3 v2); // Add two vectors
RF_API rf_vec3 rf_vec3_sub(rf_vec3 v1, rf_vec3 v2); // Subtract two vectors
RF_API rf_vec3 rf_vec3_mul(rf_vec3 v, float scalar); // Multiply vector by scalar
RF_API rf_vec3 rf_vec3_mul_v(rf_vec3 v1, rf_vec3 v2); // Multiply vector by vector
RF_API rf_vec3 rf_vec3_cross_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors cross product
RF_API rf_vec3 rf_vec3_perpendicular(rf_vec3 v); // Calculate one vector perpendicular vector
RF_API float rf_vec3_len(rf_vec3 v); // Calculate vector length
RF_API float rf_vec3_dot_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors dot product
RF_API float rf_vec3_distance(rf_vec3 v1, rf_vec3 v2); // Calculate distance between two vectors
RF_API rf_vec3 rf_vec3_scale(rf_vec3 v, float scale); // Scale provided vector
RF_API rf_vec3 rf_vec3_negate(rf_vec3 v); // Negate provided vector (invert direction)
RF_API rf_vec3 rf_vec3_div(rf_vec3 v, float div); // Divide vector by a float value
RF_API rf_vec3 rf_vec3_div_v(rf_vec3 v1, rf_vec3 v2); // Divide vector by vector
RF_API rf_vec3 rf_vec3_normalize(rf_vec3 v); // Normalize provided vector
RF_API void rf_vec3_ortho_normalize(rf_vec3* v1, rf_vec3* v2); // Orthonormalize provided vectors. Makes vectors normalized and orthogonal to each other. Gram-Schmidt function implementation
RF_API rf_vec3 rf_vec3_transform(rf_vec3 v, rf_mat mat); // Transforms a rf_vec3 by a given rf_mat
RF_API rf_vec3 rf_vec3_rotate_by_quaternion(rf_vec3 v, rf_quaternion q); // rf_transform a vector by quaternion rotation
RF_API rf_vec3 rf_vec3_lerp(rf_vec3 v1, rf_vec3 v2, float amount); // Calculate linear interpolation between two vectors
RF_API rf_vec3 rf_vec3_reflect(rf_vec3 v, rf_vec3 normal); // Calculate reflected vector to normal
RF_API rf_vec3 rf_vec3_min(rf_vec3 v1, rf_vec3 v2); // Return min value for each pair of components
RF_API rf_vec3 rf_vec3_max(rf_vec3 v1, rf_vec3 v2); // Return max value for each pair of components
RF_API rf_vec3 rf_vec3_barycenter(rf_vec3 p, rf_vec3 a, rf_vec3 b, rf_vec3 c); // Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c) NOTE: Assumes P is on the plane of the triangle

RF_API float rf_mat_determinant(rf_mat mat); // Compute matrix determinant
RF_API float rf_mat_trace(rf_mat mat); // Returns the trace of the matrix (sum of the values along the diagonal)
RF_API rf_mat rf_mat_transpose(rf_mat mat); // Transposes provided matrix
RF_API rf_mat rf_mat_invert(rf_mat mat); // Invert provided matrix
RF_API rf_mat rf_mat_normalize(rf_mat mat); // Normalize provided matrix
RF_API rf_mat rf_mat_identity(void); // Returns identity matrix
RF_API rf_mat rf_mat_add(rf_mat left, rf_mat right); // Add two matrices
RF_API rf_mat rf_mat_sub(rf_mat left, rf_mat right); // Subtract two matrices (left - right)
RF_API rf_mat rf_mat_translate(float x, float y, float z); // Returns translation matrix
RF_API rf_mat rf_mat_rotate(rf_vec3 axis, float angle); // Create rotation matrix from axis and angle. NOTE: Angle should be provided in radians
RF_API rf_mat rf_mat_rotate_xyz(rf_vec3 ang); // Returns xyz-rotation matrix (angles in radians)
RF_API rf_mat rf_mat_rotate_x(float angle); // Returns x-rotation matrix (angle in radians)
RF_API rf_mat rf_mat_rotate_y(float angle); // Returns y-rotation matrix (angle in radians)
RF_API rf_mat rf_mat_rotate_z(float angle); // Returns z-rotation matrix (angle in radians)
RF_API rf_mat rf_mat_scale(float x, float y, float z); // Returns scaling matrix
RF_API rf_mat rf_mat_mul(rf_mat left, rf_mat right); // Returns two matrix multiplication. NOTE: When multiplying matrices... the order matters!
RF_API rf_mat rf_mat_frustum(double left, double right, double bottom, double top, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix
RF_API rf_mat rf_mat_perspective(double fovy, double aspect, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix. NOTE: Angle should be provided in radians
RF_API rf_mat rf_mat_ortho(double left, double right, double bottom, double top, double near_val, double far_val); // Returns orthographic GL_PROJECTION matrix
RF_API rf_mat rf_mat_look_at(rf_vec3 eye, rf_vec3 target, rf_vec3 up); // Returns camera look-at matrix (view matrix)
RF_API rf_float16 rf_mat_to_float16(rf_mat mat); // Returns the matrix as an array of 16 floats

RF_API rf_quaternion rf_quaternion_identity(void); // Returns identity quaternion
RF_API float rf_quaternion_len(rf_quaternion q); // Computes the length of a quaternion
RF_API rf_quaternion rf_quaternion_normalize(rf_quaternion q); // Normalize provided quaternion
RF_API rf_quaternion rf_quaternion_invert(rf_quaternion q); // Invert provided quaternion
RF_API rf_quaternion rf_quaternion_mul(rf_quaternion q1, rf_quaternion q2); // Calculate two quaternion multiplication
RF_API rf_quaternion rf_quaternion_lerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate linear interpolation between two quaternions
RF_API rf_quaternion rf_quaternion_nlerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate slerp-optimized interpolation between two quaternions
RF_API rf_quaternion rf_quaternion_slerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculates spherical linear interpolation between two quaternions
RF_API rf_quaternion rf_quaternion_from_vec3_to_vec3(rf_vec3 from, rf_vec3 to); // Calculate quaternion based on the rotation from one vector to another
RF_API rf_quaternion rf_quaternion_from_mat(rf_mat mat); // Returns a quaternion for a given rotation matrix
RF_API rf_mat rf_quaternion_to_mat(rf_quaternion q); // Returns a matrix for a given quaternion
RF_API rf_quaternion rf_quaternion_from_axis_angle(rf_vec3 axis, float angle); // Returns rotation quaternion for an angle and axis. NOTE: angle must be provided in radians
RF_API void rf_quaternion_to_axis_angle(rf_quaternion q, rf_vec3* outAxis, float* outAngle); // Returns the rotation angle and axis for a given quaternion
RF_API rf_quaternion rf_quaternion_from_euler(float roll, float pitch, float yaw); // Returns he quaternion equivalent to Euler angles
RF_API rf_vec3 rf_quaternion_to_euler(rf_quaternion q); // Return the Euler angles equivalent to quaternion (roll, pitch, yaw). NOTE: Angles are returned in a rf_vec3 struct in degrees
RF_API rf_quaternion rf_quaternion_transform(rf_quaternion q, rf_mat mat); // rf_transform a quaternion given a transformation matrix

#pragma endregion

#pragma region collision detection

RF_API bool rf_rec_match(rf_rec a, rf_rec b);

RF_API bool rf_check_collision_recs(rf_rec rec1, rf_rec rec2); // Check collision between two rectangles
RF_API bool rf_check_collision_circles(rf_vec2 center1, float radius1, rf_vec2 center2, float radius2); // Check collision between two circles
RF_API bool rf_check_collision_circle_rec(rf_vec2 center, float radius, rf_rec rec); // Check collision between circle and rectangle
RF_API rf_rec rf_get_collision_rec(rf_rec rec1, rf_rec rec2); // Get collision rectangle for two rectangles collision
RF_API bool rf_check_collision_point_rec(rf_vec2 point, rf_rec rec); // Check if point is inside rectangle
RF_API bool rf_check_collision_point_circle(rf_vec2 point, rf_vec2 center, float radius); // Check if point is inside circle
RF_API bool rf_check_collision_point_triangle(rf_vec2 point, rf_vec2 p1, rf_vec2 p2, rf_vec2 p3); // Check if point is inside a triangle

RF_API bool rf_check_collision_spheres(rf_vec3 center_a, float radius_a, rf_vec3 center_b, float radius_b); // Detect collision between two spheres
RF_API bool rf_check_collision_boxes(rf_bounding_box box1, rf_bounding_box box2); // Detect collision between two bounding boxes
RF_API bool rf_check_collision_box_sphere(rf_bounding_box box, rf_vec3 center, float radius); // Detect collision between box and sphere
RF_API bool rf_check_collision_ray_sphere(rf_ray ray, rf_vec3 center, float radius); // Detect collision between ray and sphere
RF_API bool rf_check_collision_ray_sphere_ex(rf_ray ray, rf_vec3 center, float radius, rf_vec3* collision_point); // Detect collision between ray and sphere, returns collision point
RF_API bool rf_check_collision_ray_box(rf_ray ray, rf_bounding_box box); // Detect collision between ray and box

RF_API rf_ray_hit_info rf_collision_ray_model(rf_ray ray, rf_model model); // Get collision info between ray and model
RF_API rf_ray_hit_info rf_collision_ray_triangle(rf_ray ray, rf_vec3 p1, rf_vec3 p2, rf_vec3 p3); // Get collision info between ray and triangle
RF_API rf_ray_hit_info rf_collision_ray_ground(rf_ray ray, float ground_height); // Get collision info between ray and ground plane (Y-normal plane)

#pragma endregion

#pragma endregion

#pragma region rf_gfx

#pragma region shader
RF_API rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code); // Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
RF_API void rf_gfx_unload_shader(rf_shader shader); // Unload shader from GPU memory (VRAM)
RF_API int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name); // Get shader uniform location
RF_API void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name); // Set shader uniform value
RF_API void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count); // Set shader uniform value vector
RF_API void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat); // Set shader uniform value (matrix 4x4)
RF_API void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture); // Set shader uniform value for texture
#pragma endregion

RF_API rf_mat rf_gfx_get_matrix_projection(); // Return internal rf__ctx->gl_ctx.projection matrix
RF_API rf_mat rf_gfx_get_matrix_modelview(); // Return internal rf__ctx->gl_ctx.modelview matrix
RF_API void rf_gfx_set_matrix_projection(rf_mat proj); // Set a custom projection matrix (replaces internal rf__ctx->gl_ctx.projection matrix)
RF_API void rf_gfx_set_matrix_modelview(rf_mat view); // Set a custom rf__ctx->gl_ctx.modelview matrix (replaces internal rf__ctx->gl_ctx.modelview matrix)

RF_API void rf_gfx_blend_mode(rf_blend_mode mode); // Choose the blending mode (alpha, additive, multiplied)
RF_API void rf_gfx_matrix_mode(rf_matrix_mode mode); // Choose the current matrix to be transformed
RF_API void rf_gfx_push_matrix(); // Push the current matrix to rf_global_gl_stack
RF_API void rf_gfx_pop_matrix(); // Pop lattest inserted matrix from rf_global_gl_stack
RF_API void rf_gfx_load_identity(); // Reset current matrix to identity matrix
RF_API void rf_gfx_translatef(float x, float y, float z); // Multiply the current matrix by a translation matrix
RF_API void rf_gfx_rotatef(float angleDeg, float x, float y, float z); // Multiply the current matrix by a rotation matrix
RF_API void rf_gfx_scalef(float x, float y, float z); // Multiply the current matrix by a scaling matrix
RF_API void rf_gfx_mult_matrixf(float* matf); // Multiply the current matrix by another matrix
RF_API void rf_gfx_frustum(double left, double right, double bottom, double top, double znear, double zfar);
RF_API void rf_gfx_ortho(double left, double right, double bottom, double top, double znear, double zfar);
RF_API void rf_gfx_viewport(int x, int y, int width, int height); // Set the viewport area

// Functions Declaration - Vertex level operations
RF_API void rf_gfx_begin(rf_drawing_mode mode); // Initialize drawing mode (how to organize vertex)
RF_API void rf_gfx_end(); // Finish vertex providing
RF_API void rf_gfx_vertex2i(int x, int y); // Define one vertex (position) - 2 int
RF_API void rf_gfx_vertex2f(float x, float y); // Define one vertex (position) - 2 float
RF_API void rf_gfx_vertex3f(float x, float y, float z); // Define one vertex (position) - 3 float
RF_API void rf_gfx_tex_coord2f(float x, float y); // Define one vertex (texture coordinate) - 2 float
RF_API void rf_gfx_normal3f(float x, float y, float z); // Define one vertex (normal) - 3 float
RF_API void rf_gfx_color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Define one vertex (color) - 4 unsigned char
RF_API void rf_gfx_color3f(float x, float y, float z); // Define one vertex (color) - 3 float
RF_API void rf_gfx_color4f(float x, float y, float z, float w); // Define one vertex (color) - 4 float

RF_API void rf_gfx_enable_texture(unsigned int id); // Enable texture usage
RF_API void rf_gfx_disable_texture(); // Disable texture usage
RF_API void rf_gfx_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode); // Set texture parameters (wrap mode/filter mode)
RF_API void rf_gfx_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode); // Set filter for texture
RF_API void rf_gfx_enable_render_texture(unsigned int id); // Enable render texture (fbo)
RF_API void rf_gfx_disable_render_texture(void); // Disable render texture (fbo), return to default framebuffer
RF_API void rf_gfx_enable_depth_test(void); // Enable depth test
RF_API void rf_gfx_disable_depth_test(void); // Disable depth test
RF_API void rf_gfx_enable_backface_culling(void); // Enable backface culling
RF_API void rf_gfx_disable_backface_culling(void); // Disable backface culling
RF_API void rf_gfx_enable_scissor_test(void); // Enable scissor test
RF_API void rf_gfx_disable_scissor_test(void); // Disable scissor test
RF_API void rf_gfx_scissor(int x, int y, int width, int height); // Scissor test
RF_API void rf_gfx_enable_wire_mode(void); // Enable wire mode
RF_API void rf_gfx_disable_wire_mode(void); // Disable wire mode
RF_API void rf_gfx_delete_textures(unsigned int id); // Delete OpenGL texture from GPU
RF_API void rf_gfx_delete_render_textures(rf_render_texture2d target); // Delete render textures (fbo) from GPU
RF_API void rf_gfx_delete_shader(unsigned int id); // Delete OpenGL shader program from GPU
RF_API void rf_gfx_delete_vertex_arrays(unsigned int id); // Unload vertex data (VAO) from GPU memory
RF_API void rf_gfx_delete_buffers(unsigned int id); // Unload vertex data (VBO) from GPU memory
RF_API void rf_gfx_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Clear color buffer with color
RF_API void rf_gfx_clear_screen_buffers(void); // Clear used screen buffers (color and depth)
RF_API void rf_gfx_update_buffer(int buffer_id, void* data, int data_size); // Update GPU buffer with new data
RF_API unsigned int rf_gfx_load_attrib_buffer(unsigned int vao_id, int shader_loc, void* buffer, int size, bool dynamic); // Load a new attributes buffer
RF_API void rf_gfx_init_vertex_buffer(rf_vertex_buffer* vertex_buffer);

RF_API void rf_gfx_close(); // De-inititialize rlgl (buffers, shaders, textures)
RF_API void rf_gfx_draw(); // Update and draw default internal buffers

RF_API bool rf_gfx_check_buffer_limit(int v_count); // Check internal buffer overflow for a given number of vertex
RF_API void rf_gfx_set_debug_marker(const char* text); // Set debug marker for analysis

// Textures data management
RF_API unsigned int rf_gfx_load_texture(void* data, int width, int height, rf_pixel_format format, int mipmap_count); // Load texture in GPU
RF_API unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, bool use_render_buffer); // Load depth texture/renderbuffer (to be attached to fbo)
RF_API unsigned int rf_gfx_load_texture_cubemap(void* data, int size, rf_pixel_format format); // Load texture cubemap
RF_API void rf_gfx_update_texture(unsigned int id, int width, int height, rf_pixel_format format, const void* pixels, int pixels_size); // Update GPU texture with new data
RF_API rf_gfx_pixel_format rf_gfx_get_internal_texture_formats(rf_pixel_format format); // Get OpenGL internal formats
RF_API void rf_gfx_unload_texture(unsigned int id); // Unload texture from GPU memory

RF_API void rf_gfx_generate_mipmaps(rf_texture2d* texture); // Generate mipmap data for selected texture
RF_API rf_image rf_gfx_read_texture_pixels_to_buffer(rf_texture2d texture, void* dst, int dst_size);
RF_API rf_image rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator);
RF_API void rf_gfx_read_screen_pixels(rf_color* dst, int width, int height); // Read screen pixel data (color buffer)

// Render texture management (fbo)
RF_API rf_render_texture2d rf_gfx_load_render_texture(int width, int height, rf_pixel_format format, int depth_bits, bool use_depth_texture); // Load a render texture (with color and depth attachments)
RF_API void rf_gfx_render_texture_attach(rf_render_texture2d target, unsigned int id, int attach_type); // Attach texture/renderbuffer to an fbo
RF_API bool rf_gfx_render_texture_complete(rf_render_texture2d target); // Verify render texture is complete

// Vertex data management
RF_API void rf_gfx_load_mesh(rf_mesh* mesh, bool dynamic); // Upload vertex data into GPU and provided VAO/VBO ids
RF_API void rf_gfx_update_mesh(rf_mesh mesh, int buffer, int num); // Update vertex or index data on GPU (upload new data to one buffer)
RF_API void rf_gfx_update_mesh_at(rf_mesh mesh, int buffer, int num, int index); // Update vertex or index data on GPU, at index
RF_API void rf_gfx_draw_mesh(rf_mesh mesh, rf_material material, rf_mat transform); // Draw a 3d mesh with material and transform
RF_API void rf_gfx_unload_mesh(rf_mesh mesh); // Unload mesh data from CPU and GPU

#pragma endregion

#pragma region gen textures
RF_API rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, int size); // Generate cubemap texture from HDR texture
RF_API rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, int size); // Generate irradiance texture using cubemap data
RF_API rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, int size); // Generate prefilter texture using cubemap data
RF_API rf_texture2d rf_gen_texture_brdf(rf_shader shader, int size); // Generate BRDF texture using cubemap data.
#pragma endregion

#pragma region pixel format
RF_API const char* rf_pixel_format_string(rf_pixel_format format);
RF_API bool rf_is_uncompressed_format(rf_pixel_format format);
RF_API bool rf_is_compressed_format(rf_pixel_format format);
RF_API int rf_bits_per_pixel(rf_pixel_format format);
RF_API int rf_bytes_per_pixel(rf_uncompressed_pixel_format format);
RF_API int rf_pixel_buffer_size(int width, int height, rf_pixel_format format);

RF_API bool rf_format_pixels_to_normalized(const void* src, int src_size, rf_uncompressed_pixel_format src_format, rf_vec4* dst, int dst_size);
RF_API bool rf_format_pixels_to_rgba32(const void* src, int src_size, rf_uncompressed_pixel_format src_format, rf_color* dst, int dst_size);
RF_API bool rf_format_pixels(const void* src, int src_size, rf_uncompressed_pixel_format src_format, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format);

RF_API rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_uncompressed_pixel_format src_format);
RF_API rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_uncompressed_pixel_format src_format);
RF_API void rf_format_one_pixel(const void* src, rf_uncompressed_pixel_format src_format, void* dst, rf_uncompressed_pixel_format dst_format);
#pragma endregion

#pragma region image

#pragma region extract image data functions
RF_API int rf_image_size(rf_image image);
RF_API int rf_image_size_in_format(rf_image image, rf_pixel_format format);

RF_API bool rf_image_get_pixels_as_rgba32_to_buffer(rf_image image, rf_color* dst, int dst_size);
RF_API bool rf_image_get_pixels_as_normalized_to_buffer(rf_image image, rf_vec4* dst, int dst_size);

RF_API rf_color* rf_image_pixels_to_rgba32(rf_image image, rf_allocator allocator);
RF_API rf_vec4* rf_image_compute_pixels_to_normalized(rf_image image, rf_allocator allocator);

RF_API void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, int palette_size);
RF_API rf_palette rf_image_extract_palette(rf_image image, int palette_size, rf_allocator allocator);
RF_API rf_rec rf_image_alpha_border(rf_image image, float threshold);
#pragma endregion

#pragma region loading & unloading functions
RF_API bool rf_supports_image_file_type(const char* filename);

RF_API rf_image rf_load_image_from_file_data_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
RF_API rf_image rf_load_image_from_file_data(const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
RF_API rf_image rf_load_image_from_hdr_file_data(const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_image rf_load_image_from_format_to_buffer(const void* src, int src_size, int src_width, int src_height, rf_uncompressed_pixel_format src_format, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format);
RF_API rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);

RF_API void rf_unload_image(rf_image image, rf_allocator allocator);
#pragma endregion

#pragma region image manipulation
RF_API rf_image rf_image_copy_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_copy(rf_image image, rf_allocator allocator);

RF_API rf_image rf_image_crop_to_buffer(rf_image image, rf_rec crop, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format);
RF_API rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator);

RF_API rf_image rf_image_resize_to_buffer(rf_image image, int new_width, int new_height, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_image rf_image_resize_nn_to_buffer(rf_image image, int new_width, int new_height, void* dst, int dst_size);
RF_API rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator allocator);

RF_API rf_image rf_image_format_to_buffer(rf_image image, rf_uncompressed_pixel_format dst_format, void* dst, int dst_size);
RF_API rf_image rf_image_format(rf_image image, rf_uncompressed_pixel_format new_format, rf_allocator allocator);

RF_API rf_image rf_image_alpha_mask_to_buffer(rf_image image, rf_image alpha_mask, void* dst, int dst_size);
RF_API rf_image rf_image_alpha_clear(rf_image image, rf_color color, float threshold, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_image rf_image_alpha_premultiply(rf_image image, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_rec rf_image_alpha_crop_rec(rf_image image, float threshold);
RF_API rf_image rf_image_alpha_crop(rf_image image, float threshold, rf_allocator allocator);

RF_API rf_image rf_image_dither(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator allocator, rf_allocator temp_allocator);

RF_API void rf_image_flip_vertical_in_place(rf_image* image);
RF_API rf_image rf_image_flip_vertical_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_flip_vertical(rf_image image, rf_allocator allocator);

RF_API void rf_image_flip_horizontal_in_place(rf_image* image);
RF_API rf_image rf_image_flip_horizontal_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_flip_horizontal(rf_image image, rf_allocator allocator);

RF_API rf_image rf_image_rotate_cw_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_rotate_cw(rf_image image);
RF_API rf_image rf_image_rotate_ccw_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_rotate_ccw(rf_image image);

RF_API rf_image rf_image_color_tint_to_buffer(rf_image image, rf_color color, void* dst, int dst_size);
RF_API rf_image rf_image_color_tint(rf_image image, rf_color color);
RF_API rf_image rf_image_color_invert_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_color_invert(rf_image image);
RF_API rf_image rf_image_color_grayscale_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_color_grayscale(rf_image image);
RF_API rf_image rf_image_color_contrast_to_buffer(rf_image image, float contrast, void* dst, int dst_size);
RF_API rf_image rf_image_color_contrast(rf_image image, int contrast);
RF_API rf_image rf_image_color_brightness_to_buffer(rf_image image, int brightness, void* dst, int dst_size);
RF_API rf_image rf_image_color_brightness(rf_image image, int brightness);
RF_API rf_image rf_image_color_replace_to_buffer(rf_image image, rf_color color, rf_color replace, void* dst, int dst_size);
RF_API rf_image rf_image_color_replace(rf_image image, rf_color color, rf_color replace);

RF_API rf_vec2 rf_get_seed_for_cellular_image(int seeds_per_row, int tile_size, int i, rf_rand_proc rand);

RF_API rf_image rf_gen_image_color_to_buffer(int width, int height, rf_color color, rf_color* dst, int dst_count);
RF_API rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator);
RF_API rf_image rf_gen_image_gradient_v_to_buffer(int width, int height, rf_color top, rf_color bottom, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator);
RF_API rf_image rf_gen_image_gradient_h_to_buffer(int width, int height, rf_color left, rf_color right, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator);
RF_API rf_image rf_gen_image_gradient_radial_to_buffer(int width, int height, float density, rf_color inner, rf_color outer, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator);
RF_API rf_image rf_gen_image_checked_to_buffer(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator);
RF_API rf_image rf_gen_image_white_noise_to_buffer(int width, int height, float factor, rf_rand_proc rand, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_rand_proc rand, rf_allocator allocator);
RF_API rf_image rf_gen_image_perlin_noise_to_buffer(int width, int height, int offset_x, int offset_y, float scale, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator);
RF_API rf_image rf_gen_image_cellular_to_buffer(int width, int height, int tile_size, rf_rand_proc rand, rf_color* dst, int dst_size);
RF_API rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_rand_proc rand, rf_allocator allocator);

RF_API void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator);
RF_API void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator);
RF_API void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator);
#pragma endregion

#pragma region mipmaps
RF_API int rf_mipmaps_image_size(rf_mipmaps_image image);
RF_API rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count);
RF_API rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, int dst_size, rf_allocator temp_allocator);  // Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
RF_API rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator);
RF_API void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator);
#pragma endregion

#pragma region dds
RF_API int rf_get_dds_image_size(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_dds_image_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_mipmaps_image rf_load_dds_image(const void* src, int src_size, rf_allocator allocator);
RF_API rf_mipmaps_image rf_load_dds_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region pkm
RF_API int rf_get_pkm_image_size(const void* src, int src_size);
RF_API rf_image rf_load_pkm_image_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_image rf_load_pkm_image(const void* src, int src_size, rf_allocator allocator);
RF_API rf_image rf_load_pkm_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region ktx
RF_API int rf_get_ktx_image_size(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_mipmaps_image rf_load_ktx_image(const void* src, int src_size, rf_allocator allocator);
RF_API rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma endregion

#pragma region gif
RF_API rf_gif rf_load_animated_gif(const void* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_sizei rf_gif_frame_size(rf_gif gif);
RF_API rf_image rf_get_frame_from_gif(rf_gif gif, int frame);
RF_API void rf_unload_gif(rf_gif gif, rf_allocator allocator);
#pragma endregion

#pragma region texture
RF_API rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks io); // Load texture from file into GPU memory (VRAM)
RF_API rf_texture2d rf_load_texture_from_file_data(const void* data, int dst_size, rf_allocator temp_allocator); // Load texture from an image file data using stb
RF_API rf_texture2d rf_load_texture_from_image(rf_image image); // Load texture from image data
RF_API rf_texture2d rf_load_texture_from_image_with_mipmaps(rf_mipmaps_image image); // Load texture from image data
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator); // Load cubemap from image, multiple image cubemap layouts supported
RF_API rf_render_texture2d rf_load_render_texture(int width, int height); // Load texture for rendering (framebuffer)

RF_API void rf_update_texture(rf_texture2d texture, const void* pixels, int pixels_size); // Update GPU texture with new data. Pixels data must match texture.format
RF_API void rf_gen_texture_mipmaps(rf_texture2d* texture); // Generate GPU mipmaps for a texture
RF_API void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode); // Set texture scaling filter mode
RF_API void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode); // Set texture wrapping mode
RF_API void rf_unload_texture(rf_texture2d texture); // Unload texture from GPU memory (VRAM)
RF_API void rf_unload_render_texture(rf_render_texture2d target); // Unload render texture from GPU memory (VRAM)
#pragma endregion

#pragma region font

#pragma region ttf font
RF_API rf_ttf_font_info rf_parse_ttf_font(const void* ttf_data, int font_size);
RF_API void rf_compute_ttf_font_glyph_metrics(rf_ttf_font_info* font_info, const int* codepoints, int codepoints_count, rf_glyph_info* dst, int dst_count);
RF_API int rf_compute_ttf_font_atlas_width(int padding, rf_glyph_info* glyph_metrics, int glyphs_count);
RF_API rf_image rf_generate_ttf_font_atlas(rf_ttf_font_info* font_info, int atlas_width, int padding, rf_glyph_info* glyphs, int glyphs_count, rf_font_antialias antialias, unsigned short* dst, int dst_count, rf_allocator temp_allocator);
RF_API rf_font rf_ttf_font_from_atlas(int font_size, rf_image atlas, rf_glyph_info* glyph_metrics, int glyphs_count);

RF_API rf_font rf_load_ttf_font_from_data(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, int char_count, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_font rf_load_ttf_font_from_file(const char* filename, int font_size, rf_font_antialias antialias, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region image font
RF_API bool rf_compute_glyph_metrics_from_image(rf_image image, rf_color key, const int* codepoints, rf_glyph_info* dst, int codepoints_count);
RF_API rf_font rf_load_image_font_from_data(rf_image image, rf_glyph_info* glyphs, int glyphs_count);
RF_API rf_font rf_load_image_font(rf_image image, rf_color key, rf_allocator allocator);
RF_API rf_font rf_load_image_font_from_file(const char* path, rf_color key, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region font utils
RF_API void rf_unload_font(rf_font font, rf_allocator allocator);
RF_API rf_glyph_index rf_get_glyph_index(rf_font font, int character);
RF_API int rf_font_height(rf_font font, float font_size);

RF_API rf_sizef rf_measure_text(rf_font font, const char* text, float font_size, float extra_spacing);
RF_API rf_sizef rf_measure_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float extra_spacing, bool wrap);

RF_API rf_sizef rf_measure_string(rf_font font, const char* text, int len, float font_size, float extra_spacing);
RF_API rf_sizef rf_measure_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float extra_spacing, bool wrap);
#pragma endregion

#pragma endregion

#pragma region utf8
RF_API rf_decoded_rune rf_decode_utf8_char(const char* text, int len);
RF_API rf_decoded_utf8_stats rf_count_utf8_chars(const char* text, int len);
RF_API rf_decoded_string rf_decode_utf8_to_buffer(const char* text, int len, rf_rune* dst, int dst_size);
RF_API rf_decoded_string rf_decode_utf8(const char* text, int len, rf_allocator allocator);
#pragma endregion

#pragma region drawing
RF_API void rf_clear(rf_color color); // Set background color (framebuffer clear color)

RF_API void rf_begin(); // Setup canvas (framebuffer) to start drawing
RF_API void rf_end(); // End canvas drawing and swap buffers (double buffering)

RF_API void rf_begin_2d(rf_camera2d camera); // Initialize 2D mode with custom camera (2D)
RF_API void rf_end_2d(); // Ends 2D mode with custom camera

RF_API void rf_begin_3d(rf_camera3d camera); // Initializes 3D mode with custom camera (3D)
RF_API void rf_end_3d(); // Ends 3D mode and returns to default 2D orthographic mode

RF_API void rf_begin_render_to_texture(rf_render_texture2d target); // Initializes render texture for drawing
RF_API void rf_end_render_to_texture(); // Ends drawing to render texture

RF_API void rf_begin_scissor_mode(int x, int y, int width, int height); // Begin scissor mode (define screen area for following drawing)
RF_API void rf_end_scissor_mode(); // End scissor mode

RF_API void rf_begin_shader(rf_shader shader); // Begin custom shader drawing
RF_API void rf_end_shader(); // End custom shader drawing (use default shader)

RF_API void rf_begin_blend_mode(rf_blend_mode mode); // Begin blending mode (alpha, additive, multiplied)
RF_API void rf_end_blend_mode(); // End blending mode (reset to default: alpha blending)

RF_API void rf_draw_pixel(int pos_x, int pos_y, rf_color color); // Draw a pixel
RF_API void rf_draw_pixel_v(rf_vec2 position, rf_color color); // Draw a pixel (Vector version)

RF_API void rf_draw_line(int startPosX, int startPosY, int endPosX, int endPosY, rf_color color); // Draw a line
RF_API void rf_draw_line_v(rf_vec2 startPos, rf_vec2 endPos, rf_color color); // Draw a line (Vector version)
RF_API void rf_draw_line_ex(rf_vec2 startPos, rf_vec2 endPos, float thick, rf_color color); // Draw a line defining thickness
RF_API void rf_draw_line_bezier(rf_vec2 start_pos, rf_vec2 end_pos, float thick, rf_color color); // Draw a line using cubic-bezier curves in-out
RF_API void rf_draw_line_strip(rf_vec2* points, int num_points, rf_color color); // Draw lines sequence

RF_API void rf_draw_circle(int center_x, int center_y, float radius, rf_color color); // Draw a color-filled circle
RF_API void rf_draw_circle_v(rf_vec2 center, float radius, rf_color color); // Draw a color-filled circle (Vector version)
RF_API void rf_draw_circle_sector(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color); // Draw a piece of a circle
RF_API void rf_draw_circle_sector_lines(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color); // Draw circle sector outline
RF_API void rf_draw_circle_gradient(int center_x, int center_y, float radius, rf_color color1, rf_color color2); // Draw a gradient-filled circle
RF_API void rf_draw_circle_lines(int center_x, int center_y, float radius, rf_color color); // Draw circle outline

RF_API void rf_draw_ring(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color); // Draw ring
RF_API void rf_draw_ring_lines(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color); // Draw ring outline

RF_API void rf_draw_rectangle(int posX, int posY, int width, int height, rf_color color); // Draw a color-filled rectangle
RF_API void rf_draw_rectangle_v(rf_vec2 position, rf_vec2 size, rf_color color); // Draw a color-filled rectangle (Vector version)
RF_API void rf_draw_rectangle_rec(rf_rec rec, rf_color color); // Draw a color-filled rectangle
RF_API void rf_draw_rectangle_pro(rf_rec rec, rf_vec2 origin, float rotation, rf_color color); // Draw a color-filled rectangle with pro parameters

RF_API void rf_draw_rectangle_gradient_v(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2);// Draw a vertical-gradient-filled rectangle
RF_API void rf_draw_rectangle_gradient_h(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2);// Draw a horizontal-gradient-filled rectangle
RF_API void rf_draw_rectangle_gradient(rf_rec rec, rf_color col1, rf_color col2, rf_color col3, rf_color col4); // Draw a gradient-filled rectangle with custom vertex colors

RF_API void rf_draw_rectangle_outline(rf_rec rec, int line_thick, rf_color color); // Draw rectangle outline with extended parameters
RF_API void rf_draw_rectangle_rounded(rf_rec rec, float roundness, int segments, rf_color color); // Draw rectangle with rounded edges
RF_API void rf_draw_rectangle_rounded_lines(rf_rec rec, float roundness, int segments, int line_thick, rf_color color); // Draw rectangle with rounded edges outline

RF_API void rf_draw_triangle(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color); // Draw a color-filled triangle (vertex in counter-clockwise order!)
RF_API void rf_draw_triangle_lines(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color); // Draw triangle outline (vertex in counter-clockwise order!)
RF_API void rf_draw_triangle_fan(rf_vec2* points, int num_points, rf_color color); // Draw a triangle fan defined by points (first vertex is the center)
RF_API void rf_draw_triangle_strip(rf_vec2* points, int points_count, rf_color color); // Draw a triangle strip defined by points
RF_API void rf_draw_poly(rf_vec2 center, int sides, float radius, float rotation, rf_color color); // Draw a regular polygon (Vector version)

// rf_texture2d drawing functions

RF_API void rf_draw_texture(rf_texture2d texture, int x, int y, rf_color tint); // Draw a rf_texture2d with extended parameters
RF_API void rf_draw_texture_ex(rf_texture2d texture, int x, int y, int w, int h, float rotation, rf_color tint); // Draw a rf_texture2d with extended parameters
RF_API void rf_draw_texture_region(rf_texture2d texture, rf_rec source_rec, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint); // Draw a part of a texture defined by a rectangle with 'pro' parameters
RF_API void rf_draw_texture_npatch(rf_texture2d texture, rf_npatch_info n_patch_info, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint); // Draws a texture (or part of it) that stretches or shrinks nicely

// Text drawing functions

RF_API void rf_draw_string(const char* text, int text_len, int posX, int posY, int font_size, rf_color color); // Draw text (using default font)
RF_API void rf_draw_string_ex(rf_font font, const char* text, int text_len, rf_vec2 position, float fontSize, float spacing, rf_color tint); // Draw text using font and additional parameters
RF_API void rf_draw_string_wrap(rf_font font, const char* text, int text_len, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode); // Draw text and wrap at a specific width
RF_API void rf_draw_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint); // Draw text using font inside rectangle limits

RF_API void rf_draw_text(const char* text, int posX, int posY, int font_size, rf_color color); // Draw text (using default font)
RF_API void rf_draw_text_ex(rf_font font, const char* text, rf_vec2 position, float fontSize, float spacing, rf_color tint); // Draw text using font and additional parameters
RF_API void rf_draw_text_wrap(rf_font font, const char* text, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode); // Draw text and wrap at a specific width
RF_API void rf_draw_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint); // Draw text using font inside rectangle limits

RF_API void rf_draw_line3d(rf_vec3 start_pos, rf_vec3 end_pos, rf_color color); // Draw a line in 3D world space
RF_API void rf_draw_circle3d(rf_vec3 center, float radius, rf_vec3 rotation_axis, float rotation_angle, rf_color color); // Draw a circle in 3D world space
RF_API void rf_draw_cube(rf_vec3 position, float width, float height, float length, rf_color color); // Draw cube
RF_API void rf_draw_cube_wires(rf_vec3 position, float width, float height, float length, rf_color color); // Draw cube wires
RF_API void rf_draw_cube_texture(rf_texture2d texture, rf_vec3 position, float width, float height, float length, rf_color color); // Draw cube textured
RF_API void rf_draw_sphere(rf_vec3 center_pos, float radius, rf_color color); // Draw sphere
RF_API void rf_draw_sphere_ex(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color); // Draw sphere with extended parameters
RF_API void rf_draw_sphere_wires(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color); // Draw sphere wires
RF_API void rf_draw_cylinder(rf_vec3 position, float radius_top, float radius_bottom, float height, int slices, rf_color color); // Draw a cylinder/cone
RF_API void rf_draw_cylinder_wires(rf_vec3 position, float radius_top, float radius_bottom, float height, int slices, rf_color color); // Draw a cylinder/cone wires
RF_API void rf_draw_plane(rf_vec3 center_pos, rf_vec2 size, rf_color color); // Draw a plane XZ
RF_API void rf_draw_ray(rf_ray ray, rf_color color); // Draw a ray line
RF_API void rf_draw_grid(int slices, float spacing); // Draw a grid (centered at (0, 0, 0))
RF_API void rf_draw_gizmo(rf_vec3 position); // Draw simple gizmo

// rf_model drawing functions
RF_API void rf_draw_model(rf_model model, rf_vec3 position, float scale, rf_color tint); // Draw a model (with texture if set)
RF_API void rf_draw_model_ex(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotation_angle, rf_vec3 scale, rf_color tint); // Draw a model with extended parameters
RF_API void rf_draw_model_wires(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotation_angle, rf_vec3 scale, rf_color tint); // Draw a model wires (with texture if set) with extended parameters
RF_API void rf_draw_bounding_box(rf_bounding_box box, rf_color color); // Draw bounding box (wires)
RF_API void rf_draw_billboard(rf_camera3d camera, rf_texture2d texture, rf_vec3 center, float size, rf_color tint); // Draw a billboard texture
RF_API void rf_draw_billboard_rec(rf_camera3d camera, rf_texture2d texture, rf_rec source_rec, rf_vec3 center, float size, rf_color tint); // Draw a billboard texture defined by source_rec

// rf_image draw
RF_API void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator); // Draw a source image within a destination image (tint applied to source)
RF_API void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator); // Draw rectangle within an image
RF_API void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator); // Draw rectangle lines within an image
#pragma endregion

#pragma region model & materials & animations
RF_API rf_bounding_box rf_mesh_bounding_box(rf_mesh mesh); // Compute mesh bounding box limits
RF_API void rf_mesh_compute_tangents(rf_mesh* mesh, rf_allocator allocator, rf_allocator temp_allocator); // Compute mesh tangents
RF_API void rf_mesh_compute_binormals(rf_mesh* mesh); // Compute mesh binormals
RF_API void rf_unload_mesh(rf_mesh mesh, rf_allocator allocator); // Unload mesh from memory (RAM and/or VRAM)

RF_API rf_model rf_load_model(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_model rf_load_model_from_obj(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load model from files (meshes and materials)
RF_API rf_model rf_load_model_from_iqm(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load model from files (meshes and materials)
RF_API rf_model rf_load_model_from_gltf(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load model from files (meshes and materials)
RF_API rf_model rf_load_model_from_mesh(rf_mesh mesh, rf_allocator allocator); // Load model from generated mesh. Note: The function takes ownership of the mesh in model.meshes[0]
RF_API void rf_unload_model(rf_model model, rf_allocator allocator); // Unload model from memory (RAM and/or VRAM)

RF_API rf_materials_array rf_load_materials_from_mtl(const char* filename, rf_allocator allocator, rf_io_callbacks io); // Load materials from model file
RF_API void rf_set_material_texture(rf_material* material, int map_type, rf_texture2d texture); // Set texture for a material map type (rf_map_diffuse, rf_map_specular...)
RF_API void rf_set_model_mesh_material(rf_model* model, int mesh_id, int material_id); // Set material for a mesh
RF_API void rf_unload_material(rf_material material, rf_allocator allocator); // Unload material from GPU memory (VRAM)

// Animations

RF_API rf_model_animation_array rf_load_model_animations_from_iqm_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_model_animation_array rf_load_model_animations_from_iqm(const unsigned char* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator); // Load model animations from file
RF_API void rf_update_model_animation(rf_model model, rf_model_animation anim, int frame); // Update model animation pose
RF_API bool rf_is_model_animation_valid(rf_model model, rf_model_animation anim); // Check model animation skeleton match
RF_API void rf_unload_model_animation(rf_model_animation anim, rf_allocator allocator); // Unload animation data

// mesh generation functions

RF_API rf_mesh rf_gen_mesh_cube(float width, float height, float length, rf_allocator allocator, rf_allocator temp_allocator); // Generate cuboid mesh
RF_API rf_mesh rf_gen_mesh_poly(int sides, float radius, rf_allocator allocator, rf_allocator temp_allocator); // Generate polygonal mesh
RF_API rf_mesh rf_gen_mesh_plane(float width, float length, int res_x, int res_z, rf_allocator allocator, rf_allocator temp_allocator); // Generate plane mesh (with subdivisions)
RF_API rf_mesh rf_gen_mesh_sphere(float radius, int rings, int slices, rf_allocator allocator, rf_allocator temp_allocator); // Generate sphere mesh (standard sphere)
RF_API rf_mesh rf_gen_mesh_hemi_sphere(float radius, int rings, int slices, rf_allocator allocator, rf_allocator temp_allocator); // Generate half-sphere mesh (no bottom cap)
RF_API rf_mesh rf_gen_mesh_cylinder(float radius, float height, int slices, rf_allocator allocator, rf_allocator temp_allocator); // Generate cylinder mesh
RF_API rf_mesh rf_gen_mesh_torus(float radius, float size, int rad_seg, int sides, rf_allocator allocator, rf_allocator temp_allocator); // Generate torus mesh
RF_API rf_mesh rf_gen_mesh_knot(float radius, float size, int rad_seg, int sides, rf_allocator allocator, rf_allocator temp_allocator); // Generate trefoil knot mesh
RF_API rf_mesh rf_gen_mesh_heightmap(rf_image heightmap, rf_vec3 size, rf_allocator allocator, rf_allocator temp_allocator); // Generate heightmap mesh from image data
RF_API rf_mesh rf_gen_mesh_cubicmap(rf_image cubicmap, rf_vec3 cube_size, rf_allocator allocator, rf_allocator temp_allocator); // Generate cubes-based map mesh from image data
#pragma endregion

#pragma region ez api

#ifndef RAYFORK_NO_EZ_API

RF_API rf_material rf_load_default_material_ez();
RF_API rf_image rf_get_screen_data_ez();
RF_API rf_base64_output rf_decode_base64_ez(const unsigned char* input);
RF_API rf_image rf_gfx_read_texture_pixels_ez(rf_texture2d texture);

#pragma region image
#pragma region extract image data functions
RF_API rf_color* rf_image_pixels_to_rgba32_ez(rf_image image);
RF_API rf_vec4* rf_image_compute_pixels_to_normalized_ez(rf_image image);
RF_API rf_palette rf_image_extract_palette_ez(rf_image image, int palette_size);
#pragma endregion

#pragma region loading & unloading functions
RF_API rf_image rf_load_image_from_file_data_ez(const void* src, int src_size);
RF_API rf_image rf_load_image_from_hdr_file_data_ez(const void* src, int src_size);
RF_API rf_image rf_load_image_from_file_ez(const char* filename);
RF_API void rf_unload_image_ez(rf_image image);
#pragma endregion

#pragma region image manipulation
RF_API rf_image rf_image_copy_ez(rf_image image);

RF_API rf_image rf_image_crop_ez(rf_image image, rf_rec crop);

RF_API rf_image rf_image_resize_ez(rf_image image, int new_width, int new_height);
RF_API rf_image rf_image_resize_nn_ez(rf_image image, int new_width, int new_height);

RF_API rf_image rf_image_format_ez(rf_image image, rf_uncompressed_pixel_format new_format);

RF_API rf_image rf_image_alpha_clear_ez(rf_image image, rf_color color, float threshold);
RF_API rf_image rf_image_alpha_premultiply_ez(rf_image image);
RF_API rf_image rf_image_alpha_crop_ez(rf_image image, float threshold);
RF_API rf_image rf_image_dither_ez(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp);

RF_API rf_image rf_image_flip_vertical_ez(rf_image image);
RF_API rf_image rf_image_flip_horizontal_ez(rf_image image);

RF_API rf_vec2 rf_get_seed_for_cellular_image_ez(int seeds_per_row, int tile_size, int i);

RF_API rf_image rf_gen_image_color_ez(int width, int height, rf_color color);
RF_API rf_image rf_gen_image_gradient_v_ez(int width, int height, rf_color top, rf_color bottom);
RF_API rf_image rf_gen_image_gradient_h_ez(int width, int height, rf_color left, rf_color right);
RF_API rf_image rf_gen_image_gradient_radial_ez(int width, int height, float density, rf_color inner, rf_color outer);
RF_API rf_image rf_gen_image_checked_ez(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2);
RF_API rf_image rf_gen_image_white_noise_ez(int width, int height, float factor);
RF_API rf_image rf_gen_image_perlin_noise_ez(int width, int height, int offset_x, int offset_y, float scale);
RF_API rf_image rf_gen_image_cellular_ez(int width, int height, int tile_size);
#pragma endregion

#pragma region mipmaps
RF_API rf_mipmaps_image rf_image_gen_mipmaps_ez(rf_image image, int gen_mipmaps_count);
RF_API void rf_unload_mipmaps_image_ez(rf_mipmaps_image image);
#pragma endregion

#pragma region dds
RF_API rf_mipmaps_image rf_load_dds_image_ez(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_dds_image_from_file_ez(const char* file);
#pragma endregion

#pragma region pkm
RF_API rf_image rf_load_pkm_image_ez(const void* src, int src_size);
RF_API rf_image rf_load_pkm_image_from_file_ez(const char* file);
#pragma endregion

#pragma region ktx
RF_API rf_mipmaps_image rf_load_ktx_image_ez(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_ktx_image_from_file_ez(const char* file);
#pragma endregion
#pragma endregion

#pragma region gif
RF_API rf_gif rf_load_animated_gif_ez(const void* data, int data_size);
RF_API rf_gif rf_load_animated_gif_file_ez(const char* filename);
RF_API void rf_unload_gif_ez(rf_gif gif);
#pragma endregion

#pragma region texture
RF_API rf_texture2d rf_load_texture_from_file_ez(const char* filename);
RF_API rf_texture2d rf_load_texture_from_file_data_ez(const void* data, int dst_size);
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image_ez(rf_image image, rf_cubemap_layout_type layout_type);
#pragma endregion

#pragma region font
RF_API rf_font rf_load_ttf_font_from_data_ez(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, int chars_count);
RF_API rf_font rf_load_ttf_font_from_file_ez(const char* filename, int font_size, rf_font_antialias antialias);

RF_API rf_font rf_load_image_font_ez(rf_image image, rf_color key);
RF_API rf_font rf_load_image_font_from_file_ez(const char* path, rf_color key);

RF_API void rf_unload_font_ez(rf_font font);
#pragma endregion

#pragma region utf8
RF_API rf_decoded_string rf_decode_utf8_ez(const char* text, int len);
#pragma endregion

#pragma region drawing
RF_API void rf_image_draw_ez(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint);
RF_API void rf_image_draw_rectangle_ez(rf_image* dst, rf_rec rec, rf_color color);
RF_API void rf_image_draw_rectangle_lines_ez(rf_image* dst, rf_rec rec, int thick, rf_color color);
#pragma endregion

#pragma region model & materials & animations
RF_API void rf_mesh_compute_tangents_ez(rf_mesh* mesh);
RF_API void rf_unload_mesh_ez(rf_mesh mesh);

RF_API rf_model rf_load_model_ez(const char* filename);
RF_API rf_model rf_load_model_from_obj_ez(const char* filename);
RF_API rf_model rf_load_model_from_iqm_ez(const char* filename);
RF_API rf_model rf_load_model_from_gltf_ez(const char* filename);
RF_API rf_model rf_load_model_from_mesh_ez(rf_mesh mesh);
RF_API void rf_unload_model_ez(rf_model model);

RF_API rf_materials_array rf_load_materials_from_mtl_ez(const char* filename);
RF_API void rf_unload_material_ez(rf_material material);

RF_API rf_model_animation_array rf_load_model_animations_from_iqm_file_ez(const char* filename);
RF_API rf_model_animation_array rf_load_model_animations_from_iqm_ez(const unsigned char* data, int data_size);
RF_API void rf_unload_model_animation_ez(rf_model_animation anim);

RF_API rf_mesh rf_gen_mesh_cube_ez(float width, float height, float length);
RF_API rf_mesh rf_gen_mesh_poly_ez(int sides, float radius);
RF_API rf_mesh rf_gen_mesh_plane_ez(float width, float length, int res_x, int res_z);
RF_API rf_mesh rf_gen_mesh_sphere_ez(float radius, int rings, int slices);
RF_API rf_mesh rf_gen_mesh_hemi_sphere_ez(float radius, int rings, int slices);
RF_API rf_mesh rf_gen_mesh_cylinder_ez(float radius, float height, int slices);
RF_API rf_mesh rf_gen_mesh_torus_ez(float radius, float size, int rad_seg, int sides);
RF_API rf_mesh rf_gen_mesh_knot_ez(float radius, float size, int rad_seg, int sides);
RF_API rf_mesh rf_gen_mesh_heightmap_ez(rf_image heightmap, rf_vec3 size);
RF_API rf_mesh rf_gen_mesh_cubicmap_ez(rf_image cubicmap, rf_vec3 cube_size);
#pragma endregion

#endif

#pragma endregion

#pragma endregion

#pragma region audio

#define RAYFORK_NO_AUDIO // Note(LucaSas) We defined RAYFORK_NO_AUDIO until it is properly implemented
#if !defined(RAYFORK_NO_AUDIO)

#pragma region miniaudio
#pragma region miniaudio header

/*
Audio playback and capture library. Choice of public domain or MIT-0. See license statements at the end of this file.
miniaudio - v0.10.15 - 2020-07-15

David Reid - davidreidsoftware@gmail.com

Website: https://miniaud.io
GitHub:  https://github.com/dr-soft/miniaudio
*/

/*
1. Introduction
===============
miniaudio is a single file library for audio playback and capture. To use it, do the following in one .c file:

    ```c
    #define MINIAUDIO_IMPLEMENTATION
    #include "miniaudio.h"
    ```

You can do `#include miniaudio.h` in other parts of the program just like any other header.

miniaudio uses the concept of a "device" as the abstraction for physical devices. The idea is that you choose a physical device to emit or capture audio from,
and then move data to/from the device when miniaudio tells you to. Data is delivered to and from devices asynchronously via a callback which you specify when
initializing the device.

When initializing the device you first need to configure it. The device configuration allows you to specify things like the format of the data delivered via
the callback, the size of the internal buffer and the ID of the device you want to emit or capture audio from.

Once you have the device configuration set up you can initialize the device. When initializing a device you need to allocate memory for the device object
beforehand. This gives the application complete control over how the memory is allocated. In the example below we initialize a playback device on the stack,
but you could allocate it on the heap if that suits your situation better.

    ```c
    void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        // In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
        // pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
        // frameCount frames.
    }

    ...

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = MY_FORMAT;
    config.playback.channels = MY_CHANNEL_COUNT;
    config.sampleRate        = MY_SAMPLE_RATE;
    config.dataCallback      = data_callback;
    config.pUserData         = pMyCustomData;   // Can be accessed from the device object (device.pUserData).

    ma_device device;
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        ... An error occurred ...
    }

    ma_device_start(&device);     // The device is sleeping by default so you'll need to start it manually.

    ...

    ma_device_uninit(&device);    // This will stop the device so no need to do that manually.
    ```

In the example above, `data_callback()` is where audio data is written and read from the device. The idea is in playback mode you cause sound to be emitted
from the speakers by writing audio data to the output buffer (`pOutput` in the example). In capture mode you read data from the input buffer (`pInput`) to
extract sound captured by the microphone. The `frameCount` parameter tells you how many frames can be written to the output buffer and read from the input
buffer. A "frame" is one sample for each channel. For example, in a stereo stream (2 channels), one frame is 2 samples: one for the left, one for the right.
The channel count is defined by the device config. The size in bytes of an individual sample is defined by the sample format which is also specified in the
device config. Multi-channel audio data is always interleaved, which means the samples for each frame are stored next to each other in memory. For example, in
a stereo stream the first pair of samples will be the left and right samples for the first frame, the second pair of samples will be the left and right samples
for the second frame, etc.

The configuration of the device is defined by the `ma_device_config` structure. The config object is always initialized with `ma_device_config_init()`. It's
important to always initialize the config with this function as it initializes it with logical defaults and ensures your program doesn't break when new members
are added to the `ma_device_config` structure. The example above uses a fairly simple and standard device configuration. The call to `ma_device_config_init()`
takes a single parameter, which is whether or not the device is a playback, capture, duplex or loopback device (loopback devices are not supported on all
backends). The `config.playback.format` member sets the sample format which can be one of the following (all formats are native-endian):

    +---------------+----------------------------------------+---------------------------+
    | Symbol        | Description                            | Range                     |
    +---------------+----------------------------------------+---------------------------+
    | ma_format_f32 | 32-bit floating point                  | [-1, 1]                   |
    | ma_format_s16 | 16-bit signed integer                  | [-32768, 32767]           |
    | ma_format_s24 | 24-bit signed integer (tightly packed) | [-8388608, 8388607]       |
    | ma_format_s32 | 32-bit signed integer                  | [-2147483648, 2147483647] |
    | ma_format_u8  | 8-bit unsigned integer                 | [0, 255]                  |
    +---------------+----------------------------------------+---------------------------+

The `config.playback.channels` member sets the number of channels to use with the device. The channel count cannot exceed MA_MAX_CHANNELS. The
`config.sampleRate` member sets the sample rate (which must be the same for both playback and capture in full-duplex configurations). This is usually set to
44100 or 48000, but can be set to anything. It's recommended to keep this between 8000 and 384000, however.

Note that leaving the format, channel count and/or sample rate at their default values will result in the internal device's native configuration being used
which is useful if you want to avoid the overhead of miniaudio's automatic data conversion.

In addition to the sample format, channel count and sample rate, the data callback and user data pointer are also set via the config. The user data pointer is
not passed into the callback as a parameter, but is instead set to the `pUserData` member of `ma_device` which you can access directly since all miniaudio
structures are transparent.

Initializing the device is done with `ma_device_init()`. This will return a result code telling you what went wrong, if anything. On success it will return
`MA_SUCCESS`. After initialization is complete the device will be in a stopped state. To start it, use `ma_device_start()`. Uninitializing the device will stop
it, which is what the example above does, but you can also stop the device with `ma_device_stop()`. To resume the device simply call `ma_device_start()` again.
Note that it's important to never stop or start the device from inside the callback. This will result in a deadlock. Instead you set a variable or signal an
event indicating that the device needs to stop and handle it in a different thread. The following APIs must never be called inside the callback:

    ```c
    ma_device_init()
    ma_device_init_ex()
    ma_device_uninit()
    ma_device_start()
    ma_device_stop()
    ```

You must never try uninitializing and reinitializing a device inside the callback. You must also never try to stop and start it from inside the callback. There
are a few other things you shouldn't do in the callback depending on your requirements, however this isn't so much a thread-safety thing, but rather a real-
time processing thing which is beyond the scope of this introduction.

The example above demonstrates the initialization of a playback device, but it works exactly the same for capture. All you need to do is change the device type
from `ma_device_type_playback` to `ma_device_type_capture` when setting up the config, like so:

    ```c
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format   = MY_FORMAT;
    config.capture.channels = MY_CHANNEL_COUNT;
    ```

In the data callback you just read from the input buffer (`pInput` in the example above) and leave the output buffer alone (it will be set to NULL when the
device type is set to `ma_device_type_capture`).

These are the available device types and how you should handle the buffers in the callback:

    +-------------------------+--------------------------------------------------------+
    | Device Type             | Callback Behavior                                      |
    +-------------------------+--------------------------------------------------------+
    | ma_device_type_playback | Write to output buffer, leave input buffer untouched.  |
    | ma_device_type_capture  | Read from input buffer, leave output buffer untouched. |
    | ma_device_type_duplex   | Read from input buffer, write to output buffer.        |
    | ma_device_type_loopback | Read from input buffer, leave output buffer untouched. |
    +-------------------------+--------------------------------------------------------+

You will notice in the example above that the sample format and channel count is specified separately for playback and capture. This is to support different
data formats between the playback and capture devices in a full-duplex system. An example may be that you want to capture audio data as a monaural stream (one
channel), but output sound to a stereo speaker system. Note that if you use different formats between playback and capture in a full-duplex configuration you
will need to convert the data yourself. There are functions available to help you do this which will be explained later.

The example above did not specify a physical device to connect to which means it will use the operating system's default device. If you have multiple physical
devices connected and you want to use a specific one you will need to specify the device ID in the configuration, like so:

    ```c
    config.playback.pDeviceID = pMyPlaybackDeviceID;    // Only if requesting a playback or duplex device.
    config.capture.pDeviceID = pMyCaptureDeviceID;      // Only if requesting a capture, duplex or loopback device.
    ```

To retrieve the device ID you will need to perform device enumeration, however this requires the use of a new concept called the "context". Conceptually
speaking the context sits above the device. There is one context to many devices. The purpose of the context is to represent the backend at a more global level
and to perform operations outside the scope of an individual device. Mainly it is used for performing run-time linking against backend libraries, initializing
backends and enumerating devices. The example below shows how to enumerate devices.

    ```c
    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        // Error.
    }

    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    if (ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount) != MA_SUCCESS) {
        // Error.
    }

    // Loop over each device info and do something with it. Here we just print the name with their index. You may want to give the user the
    // opportunity to choose which device they'd prefer.
    for (ma_uint32 iDevice = 0; iDevice < playbackDeviceCount; iDevice += 1) {
        printf("%d - %s\n", iDevice, pPlaybackDeviceInfos[iDevice].name);
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = &pPlaybackDeviceInfos[chosenPlaybackDeviceIndex].id;
    config.playback.format    = MY_FORMAT;
    config.playback.channels  = MY_CHANNEL_COUNT;
    config.sampleRate         = MY_SAMPLE_RATE;
    config.dataCallback       = data_callback;
    config.pUserData          = pMyCustomData;

    ma_device device;
    if (ma_device_init(&context, &config, &device) != MA_SUCCESS) {
        // Error
    }

    ...

    ma_device_uninit(&device);
    ma_context_uninit(&context);
    ```

The first thing we do in this example is initialize a `ma_context` object with `ma_context_init()`. The first parameter is a pointer to a list of `ma_backend`
values which are used to override the default backend priorities. When this is NULL, as in this example, miniaudio's default priorities are used. The second
parameter is the number of backends listed in the array pointed to by the first parameter. The third parameter is a pointer to a `ma_context_config` object
which can be NULL, in which case defaults are used. The context configuration is used for setting the logging callback, custom memory allocation callbacks,
user-defined data and some backend-specific configurations.

Once the context has been initialized you can enumerate devices. In the example above we use the simpler `ma_context_get_devices()`, however you can also use a
callback for handling devices by using `ma_context_enumerate_devices()`. When using `ma_context_get_devices()` you provide a pointer to a pointer that will,
upon output, be set to a pointer to a buffer containing a list of `ma_device_info` structures. You also provide a pointer to an unsigned integer that will
receive the number of items in the returned buffer. Do not free the returned buffers as their memory is managed internally by miniaudio.

The `ma_device_info` structure contains an `id` member which is the ID you pass to the device config. It also contains the name of the device which is useful
for presenting a list of devices to the user via the UI.

When creating your own context you will want to pass it to `ma_device_init()` when initializing the device. Passing in NULL, like we do in the first example,
will result in miniaudio creating the context for you, which you don't want to do since you've already created a context. Note that internally the context is
only tracked by it's pointer which means you must not change the location of the `ma_context` object. If this is an issue, consider using `malloc()` to
allocate memory for the context.



2. Building
===========
miniaudio should work cleanly out of the box without the need to download or install any dependencies. See below for platform-specific details.


2.1. Windows
------------
The Windows build should compile cleanly on all popular compilers without the need to configure any include paths nor link to any libraries.

2.2. macOS and iOS
------------------
The macOS build should compile cleanly without the need to download any dependencies nor link to any libraries or frameworks. The iOS build needs to be
compiled as Objective-C (sorry) and will need to link the relevant frameworks but should Just Work with Xcode. Compiling through the command line requires
linking to `-lpthread` and `-lm`.

2.3. Linux
----------
The Linux build only requires linking to `-ldl`, `-lpthread` and `-lm`. You do not need any development packages.

2.4. BSD
--------
The BSD build only requires linking to `-lpthread` and `-lm`. NetBSD uses audio(4), OpenBSD uses sndio and FreeBSD uses OSS.

2.5. Android
------------
AAudio is the highest priority backend on Android. This should work out of the box without needing any kind of compiler configuration. Support for AAudio
starts with Android 8 which means older versions will fall back to OpenSL|ES which requires API level 16+.

2.6. Emscripten
---------------
The Emscripten build emits Web Audio JavaScript directly and should Just Work without any configuration. You cannot use -std=c* compiler flags, nor -ansi.


2.7. Build Options
------------------
`#define` these options before including miniaudio.h.

    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | Option               | Description                                                                                                                      |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_WASAPI         | Disables the WASAPI backend.                                                                                                     |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_DSOUND         | Disables the DirectSound backend.                                                                                                |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_WINMM          | Disables the WinMM backend.                                                                                                      |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_ALSA           | Disables the ALSA backend.                                                                                                       |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_PULSEAUDIO     | Disables the PulseAudio backend.                                                                                                 |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_JACK           | Disables the JACK backend.                                                                                                       |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_COREAUDIO      | Disables the Core Audio backend.                                                                                                 |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_SNDIO          | Disables the sndio backend.                                                                                                      |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_AUDIO4         | Disables the audio(4) backend.                                                                                                   |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_OSS            | Disables the OSS backend.                                                                                                        |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_AAUDIO         | Disables the AAudio backend.                                                                                                     |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_OPENSL         | Disables the OpenSL|ES backend.                                                                                                  |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_WEBAUDIO       | Disables the Web Audio backend.                                                                                                  |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_NULL           | Disables the null backend.                                                                                                       |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_DECODING       | Disables decoding APIs.                                                                                                          |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_ENCODING       | Disables encoding APIs.                                                                                                          |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_WAV            | Disables the built-in WAV decoder and encoder.                                                                                   |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_FLAC           | Disables the built-in FLAC decoder.                                                                                              |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_MP3            | Disables the built-in MP3 decoder.                                                                                               |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_DEVICE_IO      | Disables playback and recording. This will disable ma_context and ma_device APIs. This is useful if you only want to use         |
    |                      | miniaudio's data conversion and/or decoding APIs.                                                                                |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_THREADING      | Disables the ma_thread, ma_mutex, ma_semaphore and ma_event APIs. This option is useful if you only need to use miniaudio for    |
    |                      | data conversion, decoding and/or encoding. Some families of APIs require threading which means the following options must also   |
    |                      | be set:                                                                                                                          |
    |                      |                                                                                                                                  |
    |                      |     ```                                                                                                                          |
    |                      |     MA_NO_DEVICE_IO                                                                                                              |
    |                      |     ```                                                                                                                          |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_GENERATION     | Disables generation APIs such a ma_waveform and ma_noise.                                                                        |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_SSE2           | Disables SSE2 optimizations.                                                                                                     |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_AVX2           | Disables AVX2 optimizations.                                                                                                     |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_AVX512         | Disables AVX-512 optimizations.                                                                                                  |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_NO_NEON           | Disables NEON optimizations.                                                                                                     |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_LOG_LEVEL [level] | Sets the logging level. Set level to one of the following:                                                                       |
    |                      |                                                                                                                                  |
    |                      |     ```                                                                                                                          |
    |                      |     MA_LOG_LEVEL_VERBOSE                                                                                                         |
    |                      |     MA_LOG_LEVEL_INFO                                                                                                            |
    |                      |     MA_LOG_LEVEL_WARNING                                                                                                         |
    |                      |     MA_LOG_LEVEL_ERROR                                                                                                           |
    |                      |     ```                                                                                                                          |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_DEBUG_OUTPUT      | Enable printf() debug output.                                                                                                    |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_COINIT_VALUE      | Windows only. The value to pass to internal calls to `CoInitializeEx()`. Defaults to `COINIT_MULTITHREADED`.                     |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_API               | Controls how public APIs should be decorated. Defaults to `extern`.                                                              |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+
    | MA_DLL               | If set, configures MA_API to either import or export APIs depending on whether or not the implementation is being defined. If    |
    |                      | defining the implementation, MA_API will be configured to export. Otherwise it will be configured to import. This has no effect  |
    |                      | if MA_API is defined externally.                                                                                                 |
    +----------------------+----------------------------------------------------------------------------------------------------------------------------------+


3. Definitions
==============
This section defines common terms used throughout miniaudio. Unfortunately there is often ambiguity in the use of terms throughout the audio space, so this
section is intended to clarify how miniaudio uses each term.

3.1. Sample
-----------
A sample is a single unit of audio data. If the sample format is f32, then one sample is one 32-bit floating point number.

3.2. Frame / PCM Frame
----------------------
A frame is a group of samples equal to the number of channels. For a stereo stream a frame is 2 samples, a mono frame is 1 sample, a 5.1 surround sound frame
is 6 samples, etc. The terms "frame" and "PCM frame" are the same thing in miniaudio. Note that this is different to a compressed frame. If ever miniaudio
needs to refer to a compressed frame, such as a FLAC frame, it will always clarify what it's referring to with something like "FLAC frame".

3.3. Channel
------------
A stream of monaural audio that is emitted from an individual speaker in a speaker system, or received from an individual microphone in a microphone system. A
stereo stream has two channels (a left channel, and a right channel), a 5.1 surround sound system has 6 channels, etc. Some audio systems refer to a channel as
a complex audio stream that's mixed with other channels to produce the final mix - this is completely different to miniaudio's use of the term "channel" and
should not be confused.

3.4. Sample Rate
----------------
The sample rate in miniaudio is always expressed in Hz, such as 44100, 48000, etc. It's the number of PCM frames that are processed per second.

3.5. Formats
------------
Throughout miniaudio you will see references to different sample formats:

    +---------------+----------------------------------------+---------------------------+
    | Symbol        | Description                            | Range                     |
    +---------------+----------------------------------------+---------------------------+
    | ma_format_f32 | 32-bit floating point                  | [-1, 1]                   |
    | ma_format_s16 | 16-bit signed integer                  | [-32768, 32767]           |
    | ma_format_s24 | 24-bit signed integer (tightly packed) | [-8388608, 8388607]       |
    | ma_format_s32 | 32-bit signed integer                  | [-2147483648, 2147483647] |
    | ma_format_u8  | 8-bit unsigned integer                 | [0, 255]                  |
    +---------------+----------------------------------------+---------------------------+

All formats are native-endian.



4. Decoding
===========
The `ma_decoder` API is used for reading audio files. Built in support is included for WAV, FLAC and MP3. Support for Vorbis is enabled via stb_vorbis which
can be enabled by including the header section before the implementation of miniaudio, like the following:

    ```c
    #define STB_VORBIS_HEADER_ONLY
    #include "extras/stb_vorbis.c"    // Enables Vorbis decoding.

    #define MINIAUDIO_IMPLEMENTATION
    #include "miniaudio.h"

    // The stb_vorbis implementation must come after the implementation of miniaudio.
    #undef STB_VORBIS_HEADER_ONLY
    #include "extras/stb_vorbis.c"
    ```

A copy of stb_vorbis is included in the "extras" folder in the miniaudio repository (https://github.com/dr-soft/miniaudio).

Built-in decoders are implemented via dr_wav, dr_flac and dr_mp3. These are amalgamated into the implementation section of miniaudio. You can disable the
built-in decoders by specifying one or more of the following options before the miniaudio implementation:

    ```c
    #define MA_NO_WAV
    #define MA_NO_FLAC
    #define MA_NO_MP3
    ```

Disabling built-in versions of dr_wav, dr_flac and dr_mp3 is useful if you use these libraries independantly of the `ma_decoder` API.

A decoder can be initialized from a file with `ma_decoder_init_file()`, a block of memory with `ma_decoder_init_memory()`, or from data delivered via callbacks
with `ma_decoder_init()`. Here is an example for loading a decoder from a file:

    ```c
    ma_decoder decoder;
    ma_result result = ma_decoder_init_file("MySong.mp3", NULL, &decoder);
    if (result != MA_SUCCESS) {
        return false;   // An error occurred.
    }

    ...

    ma_decoder_uninit(&decoder);
    ```

When initializing a decoder, you can optionally pass in a pointer to a ma_decoder_config object (the NULL argument in the example above) which allows you to
configure the output format, channel count, sample rate and channel map:

    ```c
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, 48000);
    ```

When passing in NULL for decoder config in `ma_decoder_init*()`, the output format will be the same as that defined by the decoding backend.

Data is read from the decoder as PCM frames. This will return the number of PCM frames actually read. If the return value is less than the requested number of
PCM frames it means you've reached the end:

    ```c
    ma_uint64 framesRead = ma_decoder_read_pcm_frames(pDecoder, pFrames, framesToRead);
    if (framesRead < framesToRead) {
        // Reached the end.
    }
    ```

You can also seek to a specific frame like so:

    ```c
    ma_result result = ma_decoder_seek_to_pcm_frame(pDecoder, targetFrame);
    if (result != MA_SUCCESS) {
        return false;   // An error occurred.
    }
    ```

If you want to loop back to the start, you can simply seek back to the first PCM frame:

    ```c
    ma_decoder_seek_to_pcm_frame(pDecoder, 0);
    ```

When loading a decoder, miniaudio uses a trial and error technique to find the appropriate decoding backend. This can be unnecessarily inefficient if the type
is already known. In this case you can use the `_wav`, `_mp3`, etc. varients of the aforementioned initialization APIs:

    ```c
    ma_decoder_init_wav()
    ma_decoder_init_mp3()
    ma_decoder_init_memory_wav()
    ma_decoder_init_memory_mp3()
    ma_decoder_init_file_wav()
    ma_decoder_init_file_mp3()
    etc.
    ```

The `ma_decoder_init_file()` API will try using the file extension to determine which decoding backend to prefer.



5. Encoding
===========
The `ma_encoding` API is used for writing audio files. The only supported output format is WAV which is achieved via dr_wav which is amalgamated into the
implementation section of miniaudio. This can be disabled by specifying the following option before the implementation of miniaudio:

    ```c
    #define MA_NO_WAV
    ```

An encoder can be initialized to write to a file with `ma_encoder_init_file()` or from data delivered via callbacks with `ma_encoder_init()`. Below is an
example for initializing an encoder to output to a file.

    ```c
    ma_encoder_config config = ma_encoder_config_init(ma_resource_format_wav, FORMAT, CHANNELS, SAMPLE_RATE);
    ma_encoder encoder;
    ma_result result = ma_encoder_init_file("my_file.wav", &config, &encoder);
    if (result != MA_SUCCESS) {
        // Error
    }

    ...

    ma_encoder_uninit(&encoder);
    ```

When initializing an encoder you must specify a config which is initialized with `ma_encoder_config_init()`. Here you must specify the file type, the output
sample format, output channel count and output sample rate. The following file types are supported:

    +------------------------+-------------+
    | Enum                   | Description |
    +------------------------+-------------+
    | ma_resource_format_wav | WAV         |
    +------------------------+-------------+

If the format, channel count or sample rate is not supported by the output file type an error will be returned. The encoder will not perform data conversion so
you will need to convert it before outputting any audio data. To output audio data, use `ma_encoder_write_pcm_frames()`, like in the example below:

    ```c
    framesWritten = ma_encoder_write_pcm_frames(&encoder, pPCMFramesToWrite, framesToWrite);
    ```

Encoders must be uninitialized with `ma_encoder_uninit()`.


6. Data Conversion
==================
A data conversion API is included with miniaudio which supports the majority of data conversion requirements. This supports conversion between sample formats,
channel counts (with channel mapping) and sample rates.


6.1. Sample Format Conversion
-----------------------------
Conversion between sample formats is achieved with the `ma_pcm_*_to_*()`, `ma_pcm_convert()` and `ma_convert_pcm_frames_format()` APIs. Use `ma_pcm_*_to_*()`
to convert between two specific formats. Use `ma_pcm_convert()` to convert based on a `ma_format` variable. Use `ma_convert_pcm_frames_format()` to convert
PCM frames where you want to specify the frame count and channel count as a variable instead of the total sample count.


6.1.1. Dithering
----------------
Dithering can be set using the ditherMode parameter.

The different dithering modes include the following, in order of efficiency:

    +-----------+--------------------------+
    | Type      | Enum Token               |
    +-----------+--------------------------+
    | None      | ma_dither_mode_none      |
    | Rectangle | ma_dither_mode_rectangle |
    | Triangle  | ma_dither_mode_triangle  |
    +-----------+--------------------------+

Note that even if the dither mode is set to something other than `ma_dither_mode_none`, it will be ignored for conversions where dithering is not needed.
Dithering is available for the following conversions:

    ```
    s16 -> u8
    s24 -> u8
    s32 -> u8
    f32 -> u8
    s24 -> s16
    s32 -> s16
    f32 -> s16
    ```

Note that it is not an error to pass something other than ma_dither_mode_none for conversions where dither is not used. It will just be ignored.



6.2. Channel Conversion
-----------------------
Channel conversion is used for channel rearrangement and conversion from one channel count to another. The `ma_channel_converter` API is used for channel
conversion. Below is an example of initializing a simple channel converter which converts from mono to stereo.

    ```c
    ma_channel_converter_config config = ma_channel_converter_config_init(ma_format, 1, NULL, 2, NULL, ma_channel_mix_mode_default, NULL);
    result = ma_channel_converter_init(&config, &converter);
    if (result != MA_SUCCESS) {
        // Error.
    }
    ```

To perform the conversion simply call `ma_channel_converter_process_pcm_frames()` like so:

    ```c
    ma_result result = ma_channel_converter_process_pcm_frames(&converter, pFramesOut, pFramesIn, frameCount);
    if (result != MA_SUCCESS) {
        // Error.
    }
    ```

It is up to the caller to ensure the output buffer is large enough to accomodate the new PCM frames.

The only formats supported are `ma_format_s16` and `ma_format_f32`. If you need another format you need to convert your data manually which you can do with
`ma_pcm_convert()`, etc.

Input and output PCM frames are always interleaved. Deinterleaved layouts are not supported.


6.2.1. Channel Mapping
----------------------
In addition to converting from one channel count to another, like the example above, The channel converter can also be used to rearrange channels. When
initializing the channel converter, you can optionally pass in channel maps for both the input and output frames. If the channel counts are the same, and each
channel map contains the same channel positions with the exception that they're in a different order, a simple shuffling of the channels will be performed. If,
however, there is not a 1:1 mapping of channel positions, or the channel counts differ, the input channels will be mixed based on a mixing mode which is
specified when initializing the `ma_channel_converter_config` object.

When converting from mono to multi-channel, the mono channel is simply copied to each output channel. When going the other way around, the audio of each output
channel is simply averaged and copied to the mono channel.

In more complicated cases blending is used. The `ma_channel_mix_mode_simple` mode will drop excess channels and silence extra channels. For example, converting
from 4 to 2 channels, the 3rd and 4th channels will be dropped, whereas converting from 2 to 4 channels will put silence into the 3rd and 4th channels.

The `ma_channel_mix_mode_rectangle` mode uses spacial locality based on a rectangle to compute a simple distribution between input and output. Imagine sitting
in the middle of a room, with speakers on the walls representing channel positions. The MA_CHANNEL_FRONT_LEFT position can be thought of as being in the corner
of the front and left walls.

Finally, the `ma_channel_mix_mode_custom_weights` mode can be used to use custom user-defined weights. Custom weights can be passed in as the last parameter of
`ma_channel_converter_config_init()`.

Predefined channel maps can be retrieved with `ma_get_standard_channel_map()`. This takes a `ma_standard_channel_map` enum as it's first parameter, which can
be one of the following:

    +-----------------------------------+-----------------------------------------------------------+
    | Name                              | Description                                               |
    +-----------------------------------+-----------------------------------------------------------+
    | ma_standard_channel_map_default   | Default channel map used by miniaudio. See below.         |
    | ma_standard_channel_map_microsoft | Channel map used by Microsoft's bitfield channel maps.    |
    | ma_standard_channel_map_alsa      | Default ALSA channel map.                                 |
    | ma_standard_channel_map_rfc3551   | RFC 3551. Based on AIFF.                                  |
    | ma_standard_channel_map_flac      | FLAC channel map.                                         |
    | ma_standard_channel_map_vorbis    | Vorbis channel map.                                       |
    | ma_standard_channel_map_sound4    | FreeBSD's sound(4).                                       |
    | ma_standard_channel_map_sndio     | sndio channel map. http://www.sndio.org/tips.html         |
    | ma_standard_channel_map_webaudio  | https://webaudio.github.io/web-audio-api/#ChannelOrdering |
    +-----------------------------------+-----------------------------------------------------------+

Below are the channel maps used by default in miniaudio (ma_standard_channel_map_default):

    +---------------+---------------------------------+
    | Channel Count | Mapping                         |
    +---------------+---------------------------------+
    | 1 (Mono)      | 0: MA_CHANNEL_MONO              |
    +---------------+---------------------------------+
    | 2 (Stereo)    | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT       |
    +---------------+---------------------------------+
    | 3             | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT  <br> |
    |               | 2: MA_CHANNEL_FRONT_CENTER      |
    +---------------+---------------------------------+
    | 4 (Surround)  | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT  <br> |
    |               | 2: MA_CHANNEL_FRONT_CENTER <br> |
    |               | 3: MA_CHANNEL_BACK_CENTER       |
    +---------------+---------------------------------+
    | 5             | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT  <br> |
    |               | 2: MA_CHANNEL_FRONT_CENTER <br> |
    |               | 3: MA_CHANNEL_BACK_LEFT    <br> |
    |               | 4: MA_CHANNEL_BACK_RIGHT        |
    +---------------+---------------------------------+
    | 6 (5.1)       | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT  <br> |
    |               | 2: MA_CHANNEL_FRONT_CENTER <br> |
    |               | 3: MA_CHANNEL_LFE          <br> |
    |               | 4: MA_CHANNEL_SIDE_LEFT    <br> |
    |               | 5: MA_CHANNEL_SIDE_RIGHT        |
    +---------------+---------------------------------+
    | 7             | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT  <br> |
    |               | 2: MA_CHANNEL_FRONT_CENTER <br> |
    |               | 3: MA_CHANNEL_LFE          <br> |
    |               | 4: MA_CHANNEL_BACK_CENTER  <br> |
    |               | 4: MA_CHANNEL_SIDE_LEFT    <br> |
    |               | 5: MA_CHANNEL_SIDE_RIGHT        |
    +---------------+---------------------------------+
    | 8 (7.1)       | 0: MA_CHANNEL_FRONT_LEFT   <br> |
    |               | 1: MA_CHANNEL_FRONT_RIGHT  <br> |
    |               | 2: MA_CHANNEL_FRONT_CENTER <br> |
    |               | 3: MA_CHANNEL_LFE          <br> |
    |               | 4: MA_CHANNEL_BACK_LEFT    <br> |
    |               | 5: MA_CHANNEL_BACK_RIGHT   <br> |
    |               | 6: MA_CHANNEL_SIDE_LEFT    <br> |
    |               | 7: MA_CHANNEL_SIDE_RIGHT        |
    +---------------+---------------------------------+
    | Other         | All channels set to 0. This     |
    |               | is equivalent to the same       |
    |               | mapping as the device.          |
    +---------------+---------------------------------+



6.3. Resampling
---------------
Resampling is achieved with the `ma_resampler` object. To create a resampler object, do something like the following:

    ```c
    ma_resampler_config config = ma_resampler_config_init(ma_format_s16, channels, sampleRateIn, sampleRateOut, ma_resample_algorithm_linear);
    ma_resampler resampler;
    ma_result result = ma_resampler_init(&config, &resampler);
    if (result != MA_SUCCESS) {
        // An error occurred...
    }
    ```

Do the following to uninitialize the resampler:

    ```c
    ma_resampler_uninit(&resampler);
    ```

The following example shows how data can be processed

    ```c
    ma_uint64 frameCountIn  = 1000;
    ma_uint64 frameCountOut = 2000;
    ma_result result = ma_resampler_process_pcm_frames(&resampler, pFramesIn, &frameCountIn, pFramesOut, &frameCountOut);
    if (result != MA_SUCCESS) {
        // An error occurred...
    }

    // At this point, frameCountIn contains the number of input frames that were consumed and frameCountOut contains the
    // number of output frames written.
    ```

To initialize the resampler you first need to set up a config (`ma_resampler_config`) with `ma_resampler_config_init()`. You need to specify the sample format
you want to use, the number of channels, the input and output sample rate, and the algorithm.

The sample format can be either `ma_format_s16` or `ma_format_f32`. If you need a different format you will need to perform pre- and post-conversions yourself
where necessary. Note that the format is the same for both input and output. The format cannot be changed after initialization.

The resampler supports multiple channels and is always interleaved (both input and output). The channel count cannot be changed after initialization.

The sample rates can be anything other than zero, and are always specified in hertz. They should be set to something like 44100, etc. The sample rate is the
only configuration property that can be changed after initialization.

The miniaudio resampler supports multiple algorithms:

    +-----------+------------------------------+
    | Algorithm | Enum Token                   |
    +-----------+------------------------------+
    | Linear    | ma_resample_algorithm_linear |
    | Speex     | ma_resample_algorithm_speex  |
    +-----------+------------------------------+

Because Speex is not public domain it is strictly opt-in and the code is stored in separate files. if you opt-in to the Speex backend you will need to consider
it's license, the text of which can be found in it's source files in "extras/speex_resampler". Details on how to opt-in to the Speex resampler is explained in
the Speex Resampler section below.

The algorithm cannot be changed after initialization.

Processing always happens on a per PCM frame basis and always assumes interleaved input and output. De-interleaved processing is not supported. To process
frames, use `ma_resampler_process_pcm_frames()`. On input, this function takes the number of output frames you can fit in the output buffer and the number of
input frames contained in the input buffer. On output these variables contain the number of output frames that were written to the output buffer and the
number of input frames that were consumed in the process. You can pass in NULL for the input buffer in which case it will be treated as an infinitely large
buffer of zeros. The output buffer can also be NULL, in which case the processing will be treated as seek.

The sample rate can be changed dynamically on the fly. You can change this with explicit sample rates with `ma_resampler_set_rate()` and also with a decimal
ratio with `ma_resampler_set_rate_ratio()`. The ratio is in/out.

Sometimes it's useful to know exactly how many input frames will be required to output a specific number of frames. You can calculate this with
`ma_resampler_get_required_input_frame_count()`. Likewise, it's sometimes useful to know exactly how many frames would be output given a certain number of
input frames. You can do this with `ma_resampler_get_expected_output_frame_count()`.

Due to the nature of how resampling works, the resampler introduces some latency. This can be retrieved in terms of both the input rate and the output rate
with `ma_resampler_get_input_latency()` and `ma_resampler_get_output_latency()`.


6.3.1. Resampling Algorithms
----------------------------
The choice of resampling algorithm depends on your situation and requirements. The linear resampler is the most efficient and has the least amount of latency,
but at the expense of poorer quality. The Speex resampler is higher quality, but slower with more latency. It also performs several heap allocations internally
for memory management.


6.3.1.1. Linear Resampling
--------------------------
The linear resampler is the fastest, but comes at the expense of poorer quality. There is, however, some control over the quality of the linear resampler which
may make it a suitable option depending on your requirements.

The linear resampler performs low-pass filtering before or after downsampling or upsampling, depending on the sample rates you're converting between. When
decreasing the sample rate, the low-pass filter will be applied before downsampling. When increasing the rate it will be performed after upsampling. By default
a fourth order low-pass filter will be applied. This can be configured via the `lpfOrder` configuration variable. Setting this to 0 will disable filtering.

The low-pass filter has a cutoff frequency which defaults to half the sample rate of the lowest of the input and output sample rates (Nyquist Frequency). This
can be controlled with the `lpfNyquistFactor` config variable. This defaults to 1, and should be in the range of 0..1, although a value of 0 does not make
sense and should be avoided. A value of 1 will use the Nyquist Frequency as the cutoff. A value of 0.5 will use half the Nyquist Frequency as the cutoff, etc.
Values less than 1 will result in more washed out sound due to more of the higher frequencies being removed. This config variable has no impact on performance
and is a purely perceptual configuration.

The API for the linear resampler is the same as the main resampler API, only it's called `ma_linear_resampler`.


6.3.1.2. Speex Resampling
-------------------------
The Speex resampler is made up of third party code which is released under the BSD license. Because it is licensed differently to miniaudio, which is public
domain, it is strictly opt-in and all of it's code is stored in separate files. If you opt-in to the Speex resampler you must consider the license text in it's
source files. To opt-in, you must first #include the following file before the implementation of miniaudio.h:

    ```c
    #include "extras/speex_resampler/ma_speex_resampler.h"
    ```

Both the header and implementation is contained within the same file. The implementation can be included in your program like so:

    ```c
    #define MINIAUDIO_SPEEX_RESAMPLER_IMPLEMENTATION
    #include "extras/speex_resampler/ma_speex_resampler.h"
    ```

Note that even if you opt-in to the Speex backend, miniaudio won't use it unless you explicitly ask for it in the respective config of the object you are
initializing. If you try to use the Speex resampler without opting in, initialization of the `ma_resampler` object will fail with `MA_NO_BACKEND`.

The only configuration option to consider with the Speex resampler is the `speex.quality` config variable. This is a value between 0 and 10, with 0 being
the fastest with the poorest quality and 10 being the slowest with the highest quality. The default value is 3.



6.4. General Data Conversion
----------------------------
The `ma_data_converter` API can be used to wrap sample format conversion, channel conversion and resampling into one operation. This is what miniaudio uses
internally to convert between the format requested when the device was initialized and the format of the backend's native device. The API for general data
conversion is very similar to the resampling API. Create a `ma_data_converter` object like this:

    ```c
    ma_data_converter_config config = ma_data_converter_config_init(
        inputFormat,
        outputFormat,
        inputChannels,
        outputChannels,
        inputSampleRate,
        outputSampleRate
    );

    ma_data_converter converter;
    ma_result result = ma_data_converter_init(&config, &converter);
    if (result != MA_SUCCESS) {
        // An error occurred...
    }
    ```

In the example above we use `ma_data_converter_config_init()` to initialize the config, however there's many more properties that can be configured, such as
channel maps and resampling quality. Something like the following may be more suitable depending on your requirements:

    ```c
    ma_data_converter_config config = ma_data_converter_config_init_default();
    config.formatIn = inputFormat;
    config.formatOut = outputFormat;
    config.channelsIn = inputChannels;
    config.channelsOut = outputChannels;
    config.sampleRateIn = inputSampleRate;
    config.sampleRateOut = outputSampleRate;
    ma_get_standard_channel_map(ma_standard_channel_map_flac, config.channelCountIn, config.channelMapIn);
    config.resampling.linear.lpfOrder = MA_MAX_FILTER_ORDER;
    ```

Do the following to uninitialize the data converter:

    ```c
    ma_data_converter_uninit(&converter);
    ```

The following example shows how data can be processed

    ```c
    ma_uint64 frameCountIn  = 1000;
    ma_uint64 frameCountOut = 2000;
    ma_result result = ma_data_converter_process_pcm_frames(&converter, pFramesIn, &frameCountIn, pFramesOut, &frameCountOut);
    if (result != MA_SUCCESS) {
        // An error occurred...
    }

    // At this point, frameCountIn contains the number of input frames that were consumed and frameCountOut contains the number
    // of output frames written.
    ```

The data converter supports multiple channels and is always interleaved (both input and output). The channel count cannot be changed after initialization.

Sample rates can be anything other than zero, and are always specified in hertz. They should be set to something like 44100, etc. The sample rate is the only
configuration property that can be changed after initialization, but only if the `resampling.allowDynamicSampleRate` member of `ma_data_converter_config` is
set to MA_TRUE. To change the sample rate, use `ma_data_converter_set_rate()` or `ma_data_converter_set_rate_ratio()`. The ratio must be in/out. The resampling
algorithm cannot be changed after initialization.

Processing always happens on a per PCM frame basis and always assumes interleaved input and output. De-interleaved processing is not supported. To process
frames, use `ma_data_converter_process_pcm_frames()`. On input, this function takes the number of output frames you can fit in the output buffer and the number
of input frames contained in the input buffer. On output these variables contain the number of output frames that were written to the output buffer and the
number of input frames that were consumed in the process. You can pass in NULL for the input buffer in which case it will be treated as an infinitely large
buffer of zeros. The output buffer can also be NULL, in which case the processing will be treated as seek.

Sometimes it's useful to know exactly how many input frames will be required to output a specific number of frames. You can calculate this with
`ma_data_converter_get_required_input_frame_count()`. Likewise, it's sometimes useful to know exactly how many frames would be output given a certain number of
input frames. You can do this with `ma_data_converter_get_expected_output_frame_count()`.

Due to the nature of how resampling works, the data converter introduces some latency if resampling is required. This can be retrieved in terms of both the
input rate and the output rate with `ma_data_converter_get_input_latency()` and `ma_data_converter_get_output_latency()`.



7. Filtering
============

7.1. Biquad Filtering
---------------------
Biquad filtering is achieved with the `ma_biquad` API. Example:

    ```c
    ma_biquad_config config = ma_biquad_config_init(ma_format_f32, channels, b0, b1, b2, a0, a1, a2);
    ma_result result = ma_biquad_init(&config, &biquad);
    if (result != MA_SUCCESS) {
        // Error.
    }

    ...

    ma_biquad_process_pcm_frames(&biquad, pFramesOut, pFramesIn, frameCount);
    ```

Biquad filtering is implemented using transposed direct form 2. The numerator coefficients are b0, b1 and b2, and the denominator coefficients are a0, a1 and
a2. The a0 coefficient is required and coefficients must not be pre-normalized.

Supported formats are `ma_format_s16` and `ma_format_f32`. If you need to use a different format you need to convert it yourself beforehand. When using
`ma_format_s16` the biquad filter will use fixed point arithmetic. When using `ma_format_f32`, floating point arithmetic will be used.

Input and output frames are always interleaved.

Filtering can be applied in-place by passing in the same pointer for both the input and output buffers, like so:

    ```c
    ma_biquad_process_pcm_frames(&biquad, pMyData, pMyData, frameCount);
    ```

If you need to change the values of the coefficients, but maintain the values in the registers you can do so with `ma_biquad_reinit()`. This is useful if you
need to change the properties of the filter while keeping the values of registers valid to avoid glitching. Do not use `ma_biquad_init()` for this as it will
do a full initialization which involves clearing the registers to 0. Note that changing the format or channel count after initialization is invalid and will
result in an error.


7.2. Low-Pass Filtering
-----------------------
Low-pass filtering is achieved with the following APIs:

    +---------+------------------------------------------+
    | API     | Description                              |
    +---------+------------------------------------------+
    | ma_lpf1 | First order low-pass filter              |
    | ma_lpf2 | Second order low-pass filter             |
    | ma_lpf  | High order low-pass filter (Butterworth) |
    +---------+------------------------------------------+

Low-pass filter example:

    ```c
    ma_lpf_config config = ma_lpf_config_init(ma_format_f32, channels, sampleRate, cutoffFrequency, order);
    ma_result result = ma_lpf_init(&config, &lpf);
    if (result != MA_SUCCESS) {
        // Error.
    }

    ...

    ma_lpf_process_pcm_frames(&lpf, pFramesOut, pFramesIn, frameCount);
    ```

Supported formats are `ma_format_s16` and` ma_format_f32`. If you need to use a different format you need to convert it yourself beforehand. Input and output
frames are always interleaved.

Filtering can be applied in-place by passing in the same pointer for both the input and output buffers, like so:

    ```c
    ma_lpf_process_pcm_frames(&lpf, pMyData, pMyData, frameCount);
    ```

The maximum filter order is limited to MA_MAX_FILTER_ORDER which is set to 8. If you need more, you can chain first and second order filters together.

    ```c
    for (iFilter = 0; iFilter < filterCount; iFilter += 1) {
        ma_lpf2_process_pcm_frames(&lpf2[iFilter], pMyData, pMyData, frameCount);
    }
    ```

If you need to change the configuration of the filter, but need to maintain the state of internal registers you can do so with `ma_lpf_reinit()`. This may be
useful if you need to change the sample rate and/or cutoff frequency dynamically while maintaing smooth transitions. Note that changing the format or channel
count after initialization is invalid and will result in an error.

The `ma_lpf` object supports a configurable order, but if you only need a first order filter you may want to consider using `ma_lpf1`. Likewise, if you only
need a second order filter you can use `ma_lpf2`. The advantage of this is that they're lighter weight and a bit more efficient.

If an even filter order is specified, a series of second order filters will be processed in a chain. If an odd filter order is specified, a first order filter
will be applied, followed by a series of second order filters in a chain.


7.3. High-Pass Filtering
------------------------
High-pass filtering is achieved with the following APIs:

    +---------+-------------------------------------------+
    | API     | Description                               |
    +---------+-------------------------------------------+
    | ma_hpf1 | First order high-pass filter              |
    | ma_hpf2 | Second order high-pass filter             |
    | ma_hpf  | High order high-pass filter (Butterworth) |
    +---------+-------------------------------------------+

High-pass filters work exactly the same as low-pass filters, only the APIs are called `ma_hpf1`, `ma_hpf2` and `ma_hpf`. See example code for low-pass filters
for example usage.


7.4. Band-Pass Filtering
------------------------
Band-pass filtering is achieved with the following APIs:

    +---------+-------------------------------+
    | API     | Description                   |
    +---------+-------------------------------+
    | ma_bpf2 | Second order band-pass filter |
    | ma_bpf  | High order band-pass filter   |
    +---------+-------------------------------+

Band-pass filters work exactly the same as low-pass filters, only the APIs are called `ma_bpf2` and `ma_hpf`. See example code for low-pass filters for example
usage. Note that the order for band-pass filters must be an even number which means there is no first order band-pass filter, unlike low-pass and high-pass
filters.


7.5. Notch Filtering
--------------------
Notch filtering is achieved with the following APIs:

    +-----------+------------------------------------------+
    | API       | Description                              |
    +-----------+------------------------------------------+
    | ma_notch2 | Second order notching filter             |
    +-----------+------------------------------------------+


7.6. Peaking EQ Filtering
-------------------------
Peaking filtering is achieved with the following APIs:

    +----------+------------------------------------------+
    | API      | Description                              |
    +----------+------------------------------------------+
    | ma_peak2 | Second order peaking filter              |
    +----------+------------------------------------------+


7.7. Low Shelf Filtering
------------------------
Low shelf filtering is achieved with the following APIs:

    +-------------+------------------------------------------+
    | API         | Description                              |
    +-------------+------------------------------------------+
    | ma_loshelf2 | Second order low shelf filter            |
    +-------------+------------------------------------------+

Where a high-pass filter is used to eliminate lower frequencies, a low shelf filter can be used to just turn them down rather than eliminate them entirely.


7.8. High Shelf Filtering
-------------------------
High shelf filtering is achieved with the following APIs:

    +-------------+------------------------------------------+
    | API         | Description                              |
    +-------------+------------------------------------------+
    | ma_hishelf2 | Second order high shelf filter           |
    +-------------+------------------------------------------+

The high shelf filter has the same API as the low shelf filter, only you would use `ma_hishelf` instead of `ma_loshelf`. Where a low shelf filter is used to
adjust the volume of low frequencies, the high shelf filter does the same thing for high frequencies.




8. Waveform and Noise Generation
================================

8.1. Waveforms
--------------
miniaudio supports generation of sine, square, triangle and sawtooth waveforms. This is achieved with the `ma_waveform` API. Example:

    ```c
    ma_waveform_config config = ma_waveform_config_init(FORMAT, CHANNELS, SAMPLE_RATE, ma_waveform_type_sine, amplitude, frequency);

    ma_waveform waveform;
    ma_result result = ma_waveform_init(&config, &waveform);
    if (result != MA_SUCCESS) {
        // Error.
    }

    ...

    ma_waveform_read_pcm_frames(&waveform, pOutput, frameCount);
    ```

The amplitude, frequency and sample rate can be changed dynamically with `ma_waveform_set_amplitude()`, `ma_waveform_set_frequency()` and
`ma_waveform_set_sample_rate()` respectively.

You can reverse the waveform by setting the amplitude to a negative value. You can use this to control whether or not a sawtooth has a positive or negative
ramp, for example.

Below are the supported waveform types:

    +---------------------------+
    | Enum Name                 |
    +---------------------------+
    | ma_waveform_type_sine     |
    | ma_waveform_type_square   |
    | ma_waveform_type_triangle |
    | ma_waveform_type_sawtooth |
    +---------------------------+



8.2. Noise
----------
miniaudio supports generation of white, pink and Brownian noise via the `ma_noise` API. Example:

    ```c
    ma_noise_config config = ma_noise_config_init(FORMAT, CHANNELS, ma_noise_type_white, SEED, amplitude);

    ma_noise noise;
    ma_result result = ma_noise_init(&config, &noise);
    if (result != MA_SUCCESS) {
        // Error.
    }

    ...

    ma_noise_read_pcm_frames(&noise, pOutput, frameCount);
    ```

The noise API uses simple LCG random number generation. It supports a custom seed which is useful for things like automated testing requiring reproducibility.
Setting the seed to zero will default to MA_DEFAULT_LCG_SEED.

By default, the noise API will use different values for different channels. So, for example, the left side in a stereo stream will be different to the right
side. To instead have each channel use the same random value, set the `duplicateChannels` member of the noise config to true, like so:

    ```c
    config.duplicateChannels = MA_TRUE;
    ```

Below are the supported noise types.

    +------------------------+
    | Enum Name              |
    +------------------------+
    | ma_noise_type_white    |
    | ma_noise_type_pink     |
    | ma_noise_type_brownian |
    +------------------------+



9. Audio Buffers
================
miniaudio supports reading from a buffer of raw audio data via the `ma_audio_buffer` API. This can read from both memory that's managed by the application, but
can also handle the memory management for you internally. The way memory is managed is flexible and should support most use cases.

Audio buffers are initialised using the standard configuration system used everywhere in miniaudio:

    ```c
    ma_audio_buffer_config config = ma_audio_buffer_config_init(format, channels, sizeInFrames, pExistingData, &allocationCallbacks);
    ma_audio_buffer buffer;
    result = ma_audio_buffer_init(&config, &buffer);
    if (result != MA_SUCCESS) {
        // Error.
    }

    ...

    ma_audio_buffer_uninit(&buffer);
    ```

In the example above, the memory pointed to by `pExistingData` will _not_ be copied which is how an application can handle memory allocations themselves. If
you would rather make a copy of the data, use `ma_audio_buffer_init_copy()`. To uninitialize the buffer, use `ma_audio_buffer_uninit()`.

Sometimes it can be convenient to allocate the memory for the `ma_audio_buffer` structure _and_ the raw audio data in a contiguous block of memory. That is,
the raw audio data will be located immediately after the `ma_audio_buffer` structure. To do this, use `ma_audio_buffer_alloc_and_init()`:

    ```c
    ma_audio_buffer_config config = ma_audio_buffer_config_init(format, channels, sizeInFrames, pExistingData, &allocationCallbacks);
    ma_audio_buffer* pBuffer
    result = ma_audio_buffer_alloc_and_init(&config, &pBuffer);
    if (result != MA_SUCCESS) {
        // Error
    }

    ...

    ma_audio_buffer_uninit_and_free(&buffer);
    ```

If you initialize the buffer with `ma_audio_buffer_alloc_and_init()` you should uninitialize it with `ma_audio_buffer_uninit_and_free()`. In the example above,
the memory pointed to by `pExistingData` will be copied into the buffer, which is contrary to the behavior of `ma_audio_buffer_init()`.

An audio buffer has a playback cursor just like a decoder. As you read frames from the buffer, the cursor moves forward. The last parameter (`loop`) can be
used to determine if the buffer should loop. The return value is the number of frames actually read. If this is less than the number of frames requested it
means the end has been reached. This should never happen if the `loop` parameter is set to true. If you want to manually loop back to the start, you can do so
with with `ma_audio_buffer_seek_to_pcm_frame(pAudioBuffer, 0)`. Below is an example for reading data from an audio buffer.

    ```c
    ma_uint64 framesRead = ma_audio_buffer_read_pcm_frames(pAudioBuffer, pFramesOut, desiredFrameCount, isLooping);
    if (framesRead < desiredFrameCount) {
        // If not looping, this means the end has been reached. This should never happen in looping mode with valid input.
    }
    ```

Sometimes you may want to avoid the cost of data movement between the internal buffer and the output buffer. Instead you can use memory mapping to retrieve a
pointer to a segment of data:

    ```c
    void* pMappedFrames;
    ma_uint64 frameCount = frameCountToTryMapping;
    ma_result result = ma_audio_buffer_map(pAudioBuffer, &pMappedFrames, &frameCount);
    if (result == MA_SUCCESS) {
        // Map was successful. The value in frameCount will be how many frames were _actually_ mapped, which may be
        // less due to the end of the buffer being reached.
        ma_copy_pcm_frames(pFramesOut, pMappedFrames, frameCount, pAudioBuffer->format, pAudioBuffer->channels);

        // You must unmap the buffer.
        ma_audio_buffer_unmap(pAudioBuffer, frameCount);
    }
    ```

When you use memory mapping, the read cursor is increment by the frame count passed in to `ma_audio_buffer_unmap()`. If you decide not to process every frame
you can pass in a value smaller than the value returned by `ma_audio_buffer_map()`. The disadvantage to using memory mapping is that it does not handle looping
for you. You can determine if the buffer is at the end for the purpose of looping with `ma_audio_buffer_at_end()` or by inspecting the return value of
`ma_audio_buffer_unmap()` and checking if it equals `MA_AT_END`. You should not treat `MA_AT_END` as an error when returned by `ma_audio_buffer_unmap()`.



10. Ring Buffers
================
miniaudio supports lock free (single producer, single consumer) ring buffers which are exposed via the `ma_rb` and `ma_pcm_rb` APIs. The `ma_rb` API operates
on bytes, whereas the `ma_pcm_rb` operates on PCM frames. They are otherwise identical as `ma_pcm_rb` is just a wrapper around `ma_rb`.

Unlike most other APIs in miniaudio, ring buffers support both interleaved and deinterleaved streams. The caller can also allocate their own backing memory for
the ring buffer to use internally for added flexibility. Otherwise the ring buffer will manage it's internal memory for you.

The examples below use the PCM frame variant of the ring buffer since that's most likely the one you will want to use. To initialize a ring buffer, do
something like the following:

    ```c
    ma_pcm_rb rb;
    ma_result result = ma_pcm_rb_init(FORMAT, CHANNELS, BUFFER_SIZE_IN_FRAMES, NULL, NULL, &rb);
    if (result != MA_SUCCESS) {
        // Error
    }
    ```

The `ma_pcm_rb_init()` function takes the sample format and channel count as parameters because it's the PCM varient of the ring buffer API. For the regular
ring buffer that operates on bytes you would call `ma_rb_init()` which leaves these out and just takes the size of the buffer in bytes instead of frames. The
fourth parameter is an optional pre-allocated buffer and the fifth parameter is a pointer to a `ma_allocation_callbacks` structure for custom memory allocation
routines. Passing in `NULL` for this results in `MA_MALLOC()` and `MA_FREE()` being used.

Use `ma_pcm_rb_init_ex()` if you need a deinterleaved buffer. The data for each sub-buffer is offset from each other based on the stride. To manage your sub-
buffers you can use `ma_pcm_rb_get_subbuffer_stride()`, `ma_pcm_rb_get_subbuffer_offset()` and `ma_pcm_rb_get_subbuffer_ptr()`.

Use 'ma_pcm_rb_acquire_read()` and `ma_pcm_rb_acquire_write()` to retrieve a pointer to a section of the ring buffer. You specify the number of frames you
need, and on output it will set to what was actually acquired. If the read or write pointer is positioned such that the number of frames requested will require
a loop, it will be clamped to the end of the buffer. Therefore, the number of frames you're given may be less than the number you requested.

After calling `ma_pcm_rb_acquire_read()` or `ma_pcm_rb_acquire_write()`, you do your work on the buffer and then "commit" it with `ma_pcm_rb_commit_read()` or
`ma_pcm_rb_commit_write()`. This is where the read/write pointers are updated. When you commit you need to pass in the buffer that was returned by the earlier
call to `ma_pcm_rb_acquire_read()` or `ma_pcm_rb_acquire_write()` and is only used for validation. The number of frames passed to `ma_pcm_rb_commit_read()` and
`ma_pcm_rb_commit_write()` is what's used to increment the pointers.

If you want to correct for drift between the write pointer and the read pointer you can use a combination of `ma_pcm_rb_pointer_distance()`,
`ma_pcm_rb_seek_read()` and `ma_pcm_rb_seek_write()`. Note that you can only move the pointers forward, and you should only move the read pointer forward via
the consumer thread, and the write pointer forward by the producer thread. If there is too much space between the pointers, move the read pointer forward. If
there is too little space between the pointers, move the write pointer forward.

You can use a ring buffer at the byte level instead of the PCM frame level by using the `ma_rb` API. This is exactly the same, only you will use the `ma_rb`
functions instead of `ma_pcm_rb` and instead of frame counts pass around byte counts.

The maximum size of the buffer in bytes is `0x7FFFFFFF-(MA_SIMD_ALIGNMENT-1)` due to the most significant bit being used to encode a loop flag and the internally
managed buffers always being aligned to MA_SIMD_ALIGNMENT.

Note that the ring buffer is only thread safe when used by a single consumer thread and single producer thread.



11. Backends
============
The following backends are supported by miniaudio.

    +-------------+-----------------------+--------------------------------------------------------+
    | Name        | Enum Name             | Supported Operating Systems                            |
    +-------------+-----------------------+--------------------------------------------------------+
    | WASAPI      | ma_backend_wasapi     | Windows Vista+                                         |
    | DirectSound | ma_backend_dsound     | Windows XP+                                            |
    | WinMM       | ma_backend_winmm      | Windows XP+ (may work on older versions, but untested) |
    | Core Audio  | ma_backend_coreaudio  | macOS, iOS                                             |
    | ALSA        | ma_backend_alsa       | Linux                                                  |
    | PulseAudio  | ma_backend_pulseaudio | Cross Platform (disabled on Windows, BSD and Android)  |
    | JACK        | ma_backend_jack       | Cross Platform (disabled on BSD and Android)           |
    | sndio       | ma_backend_sndio      | OpenBSD                                                |
    | audio(4)    | ma_backend_audio4     | NetBSD, OpenBSD                                        |
    | OSS         | ma_backend_oss        | FreeBSD                                                |
    | AAudio      | ma_backend_aaudio     | Android 8+                                             |
    | OpenSL ES   | ma_backend_opensl     | Android (API level 16+)                                |
    | Web Audio   | ma_backend_webaudio   | Web (via Emscripten)                                   |
    | Null        | ma_backend_null       | Cross Platform (not used on Web)                       |
    +-------------+-----------------------+--------------------------------------------------------+

Some backends have some nuance details you may want to be aware of.

11.1. WASAPI
------------
- Low-latency shared mode will be disabled when using an application-defined sample rate which is different to the device's native sample rate. To work around
  this, set `wasapi.noAutoConvertSRC` to true in the device config. This is due to IAudioClient3_InitializeSharedAudioStream() failing when the
  `AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM` flag is specified. Setting wasapi.noAutoConvertSRC will result in miniaudio's internal resampler being used instead
  which will in turn enable the use of low-latency shared mode.

11.2. PulseAudio
----------------
- If you experience bad glitching/noise on Arch Linux, consider this fix from the Arch wiki:
  https://wiki.archlinux.org/index.php/PulseAudio/Troubleshooting#Glitches,_skips_or_crackling. Alternatively, consider using a different backend such as ALSA.

11.3. Android
-------------
- To capture audio on Android, remember to add the RECORD_AUDIO permission to your manifest: `<uses-permission android:name="android.permission.RECORD_AUDIO" />`
- With OpenSL|ES, only a single ma_context can be active at any given time. This is due to a limitation with OpenSL|ES.
- With AAudio, only default devices are enumerated. This is due to AAudio not having an enumeration API (devices are enumerated through Java). You can however
  perform your own device enumeration through Java and then set the ID in the ma_device_id structure (ma_device_id.aaudio) and pass it to ma_device_init().
- The backend API will perform resampling where possible. The reason for this as opposed to using miniaudio's built-in resampler is to take advantage of any
  potential device-specific optimizations the driver may implement.

11.4. UWP
---------
- UWP only supports default playback and capture devices.
- UWP requires the Microphone capability to be enabled in the application's manifest (Package.appxmanifest):

    ```
    <Package ...>
        ...
        <Capabilities>
            <DeviceCapability Name="microphone" />
        </Capabilities>
    </Package>
    ```

11.5. Web Audio / Emscripten
----------------------------
- You cannot use `-std=c*` compiler flags, nor `-ansi`. This only applies to the Emscripten build.
- The first time a context is initialized it will create a global object called "miniaudio" whose primary purpose is to act as a factory for device objects.
- Currently the Web Audio backend uses ScriptProcessorNode's, but this may need to change later as they've been deprecated.
- Google has implemented a policy in their browsers that prevent automatic media output without first receiving some kind of user input. The following web page
  has additional details: https://developers.google.com/web/updates/2017/09/autoplay-policy-changes. Starting the device may fail if you try to start playback
  without first handling some kind of user input.



12. Miscellaneous Notes
=======================
- Automatic stream routing is enabled on a per-backend basis. Support is explicitly enabled for WASAPI and Core Audio, however other backends such as
  PulseAudio may naturally support it, though not all have been tested.
- The contents of the output buffer passed into the data callback will always be pre-initialized to zero unless the `noPreZeroedOutputBuffer` config variable
  in `ma_device_config` is set to true, in which case it'll be undefined which will require you to write something to the entire buffer.
- By default miniaudio will automatically clip samples. This only applies when the playback sample format is configured as `ma_format_f32`. If you are doing
  clipping yourself, you can disable this overhead by setting `noClip` to true in the device config.
- The sndio backend is currently only enabled on OpenBSD builds.
- The audio(4) backend is supported on OpenBSD, but you may need to disable sndiod before you can use it.
- Note that GCC and Clang requires `-msse2`, `-mavx2`, etc. for SIMD optimizations.
*/

#ifndef miniaudio_h
#define miniaudio_h

#ifdef __cplusplus
extern "C" {
#endif

#define MA_STRINGIFY(x)     #x
#define MA_XSTRINGIFY(x)    MA_STRINGIFY(x)

#define MA_VERSION_MAJOR    0
#define MA_VERSION_MINOR    10
#define MA_VERSION_REVISION 15
#define MA_VERSION_STRING   MA_XSTRINGIFY(MA_VERSION_MAJOR) "." MA_XSTRINGIFY(MA_VERSION_MINOR) "." MA_XSTRINGIFY(MA_VERSION_REVISION)

#if defined(_MSC_VER) && !defined(__clang__)
    #pragma warning(push)
    #pragma warning(disable:4201)   /* nonstandard extension used: nameless struct/union */
    #pragma warning(disable:4214)   /* nonstandard extension used: bit field types other than int */
    #pragma warning(disable:4324)   /* structure was padded due to alignment specifier */
#else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic" /* For ISO C99 doesn't support unnamed structs/unions [-Wpedantic] */
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wc11-extensions"   /* anonymous unions are a C11 extension */
    #endif
#endif

/* Platform/backend detection. */
#ifdef _WIN32
    #define MA_WIN32
    #if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        #define MA_WIN32_UWP
    #else
        #define MA_WIN32_DESKTOP
    #endif
#else
    #define MA_POSIX
    #include <pthread.h>    /* Unfortunate #include, but needed for pthread_t, pthread_mutex_t and pthread_cond_t types. */

    #ifdef __unix__
        #define MA_UNIX
        #if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
            #define MA_BSD
        #endif
    #endif
    #ifdef __linux__
        #define MA_LINUX
    #endif
    #ifdef __APPLE__
        #define MA_APPLE
    #endif
    #ifdef __ANDROID__
        #define MA_ANDROID
    #endif
    #ifdef __EMSCRIPTEN__
        #define MA_EMSCRIPTEN
    #endif
#endif

#include <stddef.h> /* For size_t. */

/* Sized types. Prefer built-in types. Fall back to stdint. */
#ifdef _MSC_VER
    #if defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wlanguage-extension-token"
        #pragma GCC diagnostic ignored "-Wlong-long"
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
    #endif
    typedef   signed __int8  ma_int8;
    typedef unsigned __int8  ma_uint8;
    typedef   signed __int16 ma_int16;
    typedef unsigned __int16 ma_uint16;
    typedef   signed __int32 ma_int32;
    typedef unsigned __int32 ma_uint32;
    typedef   signed __int64 ma_int64;
    typedef unsigned __int64 ma_uint64;
    #if defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
#else
    #define MA_HAS_STDINT
    #include <stdint.h>
    typedef int8_t   ma_int8;
    typedef uint8_t  ma_uint8;
    typedef int16_t  ma_int16;
    typedef uint16_t ma_uint16;
    typedef int32_t  ma_int32;
    typedef uint32_t ma_uint32;
    typedef int64_t  ma_int64;
    typedef uint64_t ma_uint64;
#endif

#ifdef MA_HAS_STDINT
    typedef uintptr_t ma_uintptr;
#else
    #if defined(_WIN32)
        #if defined(_WIN64)
            typedef ma_uint64 ma_uintptr;
        #else
            typedef ma_uint32 ma_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
            typedef ma_uint64 ma_uintptr;
        #else
            typedef ma_uint32 ma_uintptr;
        #endif
    #else
        typedef ma_uint64 ma_uintptr;   /* Fallback. */
    #endif
#endif

typedef ma_uint8    ma_bool8;
typedef ma_uint32   ma_bool32;
#define MA_TRUE     1
#define MA_FALSE    0

typedef void* ma_handle;
typedef void* ma_ptr;
typedef void (* ma_proc)(void);

#if defined(_MSC_VER) && !defined(_WCHAR_T_DEFINED)
typedef ma_uint16 wchar_t;
#endif

/* Define NULL for some compilers. */
#ifndef NULL
#define NULL 0
#endif

#if defined(SIZE_MAX)
    #define MA_SIZE_MAX    SIZE_MAX
#else
    #define MA_SIZE_MAX    0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif


#ifdef _MSC_VER
    #define MA_INLINE __forceinline
#elif defined(__GNUC__)
    /*
    I've had a bug report where GCC is emitting warnings about functions possibly not being inlineable. This warning happens when
    the __attribute__((always_inline)) attribute is defined without an "inline" statement. I think therefore there must be some
    case where "__inline__" is not always defined, thus the compiler emitting these warnings. When using -std=c89 or -ansi on the
    command line, we cannot use the "inline" keyword and instead need to use "__inline__". In an attempt to work around this issue
    I am using "__inline__" only when we're compiling in strict ANSI mode.
    */
    #if defined(__STRICT_ANSI__)
        #define MA_INLINE __inline__ __attribute__((always_inline))
    #else
        #define MA_INLINE inline __attribute__((always_inline))
    #endif
#else
    #define MA_INLINE
#endif

#if !defined(MA_API)
    #if defined(MA_DLL)
        #if defined(_WIN32)
            #define MA_DLL_IMPORT  __declspec(dllimport)
            #define MA_DLL_EXPORT  __declspec(dllexport)
            #define MA_DLL_PRIVATE static
        #else
            #if defined(__GNUC__) && __GNUC__ >= 4
                #define MA_DLL_IMPORT  __attribute__((visibility("default")))
                #define MA_DLL_EXPORT  __attribute__((visibility("default")))
                #define MA_DLL_PRIVATE __attribute__((visibility("hidden")))
            #else
                #define MA_DLL_IMPORT
                #define MA_DLL_EXPORT
                #define MA_DLL_PRIVATE static
            #endif
        #endif

        #if defined(MINIAUDIO_IMPLEMENTATION) || defined(MA_IMPLEMENTATION)
            #define MA_API  MA_DLL_EXPORT
        #else
            #define MA_API  MA_DLL_IMPORT
        #endif
        #define MA_PRIVATE MA_DLL_PRIVATE
    #else
        #define MA_API extern
        #define MA_PRIVATE static
    #endif
#endif

/* SIMD alignment in bytes. Currently set to 64 bytes in preparation for future AVX-512 optimizations. */
#define MA_SIMD_ALIGNMENT  64


/*
Logging Levels
==============
A log level will automatically include the lower levels. For example, verbose logging will enable everything. The warning log level will only include warnings
and errors, but will ignore informational and verbose logging. If you only want to handle a specific log level, implement a custom log callback (see
ma_context_init() for details) and interrogate the `logLevel` parameter.

By default the log level will be set to MA_LOG_LEVEL_ERROR, but you can change this by defining MA_LOG_LEVEL before the implementation of miniaudio.

MA_LOG_LEVEL_VERBOSE
    Mainly intended for debugging. This will enable all log levels and can be triggered from within the data callback so care must be taken when enabling this
    in production environments.

MA_LOG_LEVEL_INFO
    Informational logging. Useful for debugging. This will also enable warning and error logs. This will never be called from within the data callback.

MA_LOG_LEVEL_WARNING
    Warnings. You should enable this in you development builds and action them when encounted. This will also enable error logs. These logs usually indicate a
    potential problem or misconfiguration, but still allow you to keep running. This will never be called from within the data callback.

MA_LOG_LEVEL_ERROR
    Error logging. This will be fired when an operation fails and is subsequently aborted. This can be fired from within the data callback, in which case the
    device will be stopped. You should always have this log level enabled.
*/
#define MA_LOG_LEVEL_VERBOSE   4
#define MA_LOG_LEVEL_INFO      3
#define MA_LOG_LEVEL_WARNING   2
#define MA_LOG_LEVEL_ERROR     1

#ifndef MA_LOG_LEVEL
#define MA_LOG_LEVEL           MA_LOG_LEVEL_ERROR
#endif

typedef struct ma_context ma_context;
typedef struct ma_device ma_device;

typedef ma_uint8 ma_channel;
#define MA_CHANNEL_NONE                                0
#define MA_CHANNEL_MONO                                1
#define MA_CHANNEL_FRONT_LEFT                          2
#define MA_CHANNEL_FRONT_RIGHT                         3
#define MA_CHANNEL_FRONT_CENTER                        4
#define MA_CHANNEL_LFE                                 5
#define MA_CHANNEL_BACK_LEFT                           6
#define MA_CHANNEL_BACK_RIGHT                          7
#define MA_CHANNEL_FRONT_LEFT_CENTER                   8
#define MA_CHANNEL_FRONT_RIGHT_CENTER                  9
#define MA_CHANNEL_BACK_CENTER                         10
#define MA_CHANNEL_SIDE_LEFT                           11
#define MA_CHANNEL_SIDE_RIGHT                          12
#define MA_CHANNEL_TOP_CENTER                          13
#define MA_CHANNEL_TOP_FRONT_LEFT                      14
#define MA_CHANNEL_TOP_FRONT_CENTER                    15
#define MA_CHANNEL_TOP_FRONT_RIGHT                     16
#define MA_CHANNEL_TOP_BACK_LEFT                       17
#define MA_CHANNEL_TOP_BACK_CENTER                     18
#define MA_CHANNEL_TOP_BACK_RIGHT                      19
#define MA_CHANNEL_AUX_0                               20
#define MA_CHANNEL_AUX_1                               21
#define MA_CHANNEL_AUX_2                               22
#define MA_CHANNEL_AUX_3                               23
#define MA_CHANNEL_AUX_4                               24
#define MA_CHANNEL_AUX_5                               25
#define MA_CHANNEL_AUX_6                               26
#define MA_CHANNEL_AUX_7                               27
#define MA_CHANNEL_AUX_8                               28
#define MA_CHANNEL_AUX_9                               29
#define MA_CHANNEL_AUX_10                              30
#define MA_CHANNEL_AUX_11                              31
#define MA_CHANNEL_AUX_12                              32
#define MA_CHANNEL_AUX_13                              33
#define MA_CHANNEL_AUX_14                              34
#define MA_CHANNEL_AUX_15                              35
#define MA_CHANNEL_AUX_16                              36
#define MA_CHANNEL_AUX_17                              37
#define MA_CHANNEL_AUX_18                              38
#define MA_CHANNEL_AUX_19                              39
#define MA_CHANNEL_AUX_20                              40
#define MA_CHANNEL_AUX_21                              41
#define MA_CHANNEL_AUX_22                              42
#define MA_CHANNEL_AUX_23                              43
#define MA_CHANNEL_AUX_24                              44
#define MA_CHANNEL_AUX_25                              45
#define MA_CHANNEL_AUX_26                              46
#define MA_CHANNEL_AUX_27                              47
#define MA_CHANNEL_AUX_28                              48
#define MA_CHANNEL_AUX_29                              49
#define MA_CHANNEL_AUX_30                              50
#define MA_CHANNEL_AUX_31                              51
#define MA_CHANNEL_LEFT                                MA_CHANNEL_FRONT_LEFT
#define MA_CHANNEL_RIGHT                               MA_CHANNEL_FRONT_RIGHT
#define MA_CHANNEL_POSITION_COUNT                      (MA_CHANNEL_AUX_31 + 1)


typedef int ma_result;
#define MA_SUCCESS                                      0
#define MA_ERROR                                       -1   /* A generic error. */
#define MA_INVALID_ARGS                                -2
#define MA_INVALID_OPERATION                           -3
#define MA_OUT_OF_MEMORY                               -4
#define MA_OUT_OF_RANGE                                -5
#define MA_ACCESS_DENIED                               -6
#define MA_DOES_NOT_EXIST                              -7
#define MA_ALREADY_EXISTS                              -8
#define MA_TOO_MANY_OPEN_FILES                         -9
#define MA_INVALID_FILE                                -10
#define MA_TOO_BIG                                     -11
#define MA_PATH_TOO_LONG                               -12
#define MA_NAME_TOO_LONG                               -13
#define MA_NOT_DIRECTORY                               -14
#define MA_IS_DIRECTORY                                -15
#define MA_DIRECTORY_NOT_EMPTY                         -16
#define MA_END_OF_FILE                                 -17
#define MA_NO_SPACE                                    -18
#define MA_BUSY                                        -19
#define MA_IO_ERROR                                    -20
#define MA_INTERRUPT                                   -21
#define MA_UNAVAILABLE                                 -22
#define MA_ALREADY_IN_USE                              -23
#define MA_BAD_ADDRESS                                 -24
#define MA_BAD_SEEK                                    -25
#define MA_BAD_PIPE                                    -26
#define MA_DEADLOCK                                    -27
#define MA_TOO_MANY_LINKS                              -28
#define MA_NOT_IMPLEMENTED                             -29
#define MA_NO_MESSAGE                                  -30
#define MA_BAD_MESSAGE                                 -31
#define MA_NO_DATA_AVAILABLE                           -32
#define MA_INVALID_DATA                                -33
#define MA_TIMEOUT                                     -34
#define MA_NO_NETWORK                                  -35
#define MA_NOT_UNIQUE                                  -36
#define MA_NOT_SOCKET                                  -37
#define MA_NO_ADDRESS                                  -38
#define MA_BAD_PROTOCOL                                -39
#define MA_PROTOCOL_UNAVAILABLE                        -40
#define MA_PROTOCOL_NOT_SUPPORTED                      -41
#define MA_PROTOCOL_FAMILY_NOT_SUPPORTED               -42
#define MA_ADDRESS_FAMILY_NOT_SUPPORTED                -43
#define MA_SOCKET_NOT_SUPPORTED                        -44
#define MA_CONNECTION_RESET                            -45
#define MA_ALREADY_CONNECTED                           -46
#define MA_NOT_CONNECTED                               -47
#define MA_CONNECTION_REFUSED                          -48
#define MA_NO_HOST                                     -49
#define MA_IN_PROGRESS                                 -50
#define MA_CANCELLED                                   -51
#define MA_MEMORY_ALREADY_MAPPED                       -52
#define MA_AT_END                                      -53

/* General miniaudio-specific errors. */
#define MA_FORMAT_NOT_SUPPORTED                        -100
#define MA_DEVICE_TYPE_NOT_SUPPORTED                   -101
#define MA_SHARE_MODE_NOT_SUPPORTED                    -102
#define MA_NO_BACKEND                                  -103
#define MA_NO_DEVICE                                   -104
#define MA_API_NOT_FOUND                               -105
#define MA_INVALID_DEVICE_CONFIG                       -106

/* State errors. */
#define MA_DEVICE_NOT_INITIALIZED                      -200
#define MA_DEVICE_ALREADY_INITIALIZED                  -201
#define MA_DEVICE_NOT_STARTED                          -202
#define MA_DEVICE_NOT_STOPPED                          -203

/* Operation errors. */
#define MA_FAILED_TO_INIT_BACKEND                      -300
#define MA_FAILED_TO_OPEN_BACKEND_DEVICE               -301
#define MA_FAILED_TO_START_BACKEND_DEVICE              -302
#define MA_FAILED_TO_STOP_BACKEND_DEVICE               -303


/* Standard sample rates. */
#define MA_SAMPLE_RATE_8000                            8000
#define MA_SAMPLE_RATE_11025                           11025
#define MA_SAMPLE_RATE_16000                           16000
#define MA_SAMPLE_RATE_22050                           22050
#define MA_SAMPLE_RATE_24000                           24000
#define MA_SAMPLE_RATE_32000                           32000
#define MA_SAMPLE_RATE_44100                           44100
#define MA_SAMPLE_RATE_48000                           48000
#define MA_SAMPLE_RATE_88200                           88200
#define MA_SAMPLE_RATE_96000                           96000
#define MA_SAMPLE_RATE_176400                          176400
#define MA_SAMPLE_RATE_192000                          192000
#define MA_SAMPLE_RATE_352800                          352800
#define MA_SAMPLE_RATE_384000                          384000

#define MA_MIN_CHANNELS                                1
#ifndef MA_MAX_CHANNELS
#define MA_MAX_CHANNELS                                32
#endif
#define MA_MIN_SAMPLE_RATE                             MA_SAMPLE_RATE_8000
#define MA_MAX_SAMPLE_RATE                             MA_SAMPLE_RATE_384000

#ifndef MA_MAX_FILTER_ORDER
#define MA_MAX_FILTER_ORDER                            8
#endif

typedef enum
{
    ma_stream_format_pcm = 0
} ma_stream_format;

typedef enum
{
    ma_stream_layout_interleaved = 0,
    ma_stream_layout_deinterleaved
} ma_stream_layout;

typedef enum
{
    ma_dither_mode_none = 0,
    ma_dither_mode_rectangle,
    ma_dither_mode_triangle
} ma_dither_mode;

typedef enum
{
    /*
    I like to keep these explicitly defined because they're used as a key into a lookup table. When items are
    added to this, make sure there are no gaps and that they're added to the lookup table in ma_get_bytes_per_sample().
    */
    ma_format_unknown = 0,     /* Mainly used for indicating an error, but also used as the default for the output format for decoders. */
    ma_format_u8      = 1,
    ma_format_s16     = 2,     /* Seems to be the most widely supported format. */
    ma_format_s24     = 3,     /* Tightly packed. 3 bytes per sample. */
    ma_format_s32     = 4,
    ma_format_f32     = 5,
    ma_format_count
} ma_format;

typedef enum
{
    ma_channel_mix_mode_rectangular = 0,   /* Simple averaging based on the plane(s) the channel is sitting on. */
    ma_channel_mix_mode_simple,            /* Drop excess channels; zeroed out extra channels. */
    ma_channel_mix_mode_custom_weights,    /* Use custom weights specified in ma_channel_router_config. */
    ma_channel_mix_mode_planar_blend = ma_channel_mix_mode_rectangular,
    ma_channel_mix_mode_default = ma_channel_mix_mode_planar_blend
} ma_channel_mix_mode;

typedef enum
{
    ma_standard_channel_map_microsoft,
    ma_standard_channel_map_alsa,
    ma_standard_channel_map_rfc3551,   /* Based off AIFF. */
    ma_standard_channel_map_flac,
    ma_standard_channel_map_vorbis,
    ma_standard_channel_map_sound4,    /* FreeBSD's sound(4). */
    ma_standard_channel_map_sndio,     /* www.sndio.org/tips.html */
    ma_standard_channel_map_webaudio = ma_standard_channel_map_flac, /* https://webaudio.github.io/web-audio-api/#ChannelOrdering. Only 1, 2, 4 and 6 channels are defined, but can fill in the gaps with logical assumptions. */
    ma_standard_channel_map_default = ma_standard_channel_map_microsoft
} ma_standard_channel_map;

typedef enum
{
    ma_performance_profile_low_latency = 0,
    ma_performance_profile_conservative
} ma_performance_profile;


typedef struct
{
    void* pUserData;
    void* (* onMalloc)(size_t sz, void* pUserData);
    void* (* onRealloc)(void* p, size_t sz, void* pUserData);
    void  (* onFree)(void* p, void* pUserData);
} ma_allocation_callbacks;

typedef struct
{
    ma_int32 state;
} ma_lcg;


#ifndef MA_NO_THREADING
/* Thread priorties should be ordered such that the default priority of the worker thread is 0. */
typedef enum
{
    ma_thread_priority_idle     = -5,
    ma_thread_priority_lowest   = -4,
    ma_thread_priority_low      = -3,
    ma_thread_priority_normal   = -2,
    ma_thread_priority_high     = -1,
    ma_thread_priority_highest  =  0,
    ma_thread_priority_realtime =  1,
    ma_thread_priority_default  =  0
} ma_thread_priority;

typedef unsigned char ma_spinlock;

#if defined(MA_WIN32)
typedef ma_handle ma_thread;
#endif
#if defined(MA_POSIX)
typedef pthread_t ma_thread;
#endif

#if defined(MA_WIN32)
typedef ma_handle ma_mutex;
#endif
#if defined(MA_POSIX)
typedef pthread_mutex_t ma_mutex;
#endif

#if defined(MA_WIN32)
typedef ma_handle ma_event;
#endif
#if defined(MA_POSIX)
typedef struct
{
    ma_uint32 value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ma_event;
#endif  /* MA_POSIX */

#if defined(MA_WIN32)
typedef ma_handle ma_semaphore;
#endif
#if defined(MA_POSIX)
typedef struct
{
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ma_semaphore;
#endif  /* MA_POSIX */
#else
/* MA_NO_THREADING is set which means threading is disabled. Threading is required by some API families. If any of these are enabled we need to throw an error. */
#ifndef MA_NO_DEVICE_IO
#error "MA_NO_THREADING cannot be used without MA_NO_DEVICE_IO";
#endif
#endif  /* MA_NO_THREADING */


/*
Retrieves the version of miniaudio as separated integers. Each component can be NULL if it's not required.
*/
MA_API void ma_version(ma_uint32* pMajor, ma_uint32* pMinor, ma_uint32* pRevision);

/*
Retrieves the version of miniaudio as a string which can be useful for logging purposes.
*/
MA_API const char* ma_version_string();


/**************************************************************************************************************************************************************

Biquad Filtering

**************************************************************************************************************************************************************/
typedef union
{
    float    f32;
    ma_int32 s32;
} ma_biquad_coefficient;

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    double b0;
    double b1;
    double b2;
    double a0;
    double a1;
    double a2;
} ma_biquad_config;

MA_API ma_biquad_config ma_biquad_config_init(ma_format format, ma_uint32 channels, double b0, double b1, double b2, double a0, double a1, double a2);

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_biquad_coefficient b0;
    ma_biquad_coefficient b1;
    ma_biquad_coefficient b2;
    ma_biquad_coefficient a1;
    ma_biquad_coefficient a2;
    ma_biquad_coefficient r1[MA_MAX_CHANNELS];
    ma_biquad_coefficient r2[MA_MAX_CHANNELS];
} ma_biquad;

MA_API ma_result ma_biquad_init(const ma_biquad_config* pConfig, ma_biquad* pBQ);
MA_API ma_result ma_biquad_reinit(const ma_biquad_config* pConfig, ma_biquad* pBQ);
MA_API ma_result ma_biquad_process_pcm_frames(ma_biquad* pBQ, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_biquad_get_latency(ma_biquad* pBQ);


/**************************************************************************************************************************************************************

Low-Pass Filtering

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double cutoffFrequency;
    double q;
} ma_lpf1_config, ma_lpf2_config;

MA_API ma_lpf1_config ma_lpf1_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency);
MA_API ma_lpf2_config ma_lpf2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency, double q);

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_biquad_coefficient a;
    ma_biquad_coefficient r1[MA_MAX_CHANNELS];
} ma_lpf1;

MA_API ma_result ma_lpf1_init(const ma_lpf1_config* pConfig, ma_lpf1* pLPF);
MA_API ma_result ma_lpf1_reinit(const ma_lpf1_config* pConfig, ma_lpf1* pLPF);
MA_API ma_result ma_lpf1_process_pcm_frames(ma_lpf1* pLPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_lpf1_get_latency(ma_lpf1* pLPF);

typedef struct
{
    ma_biquad bq;   /* The second order low-pass filter is implemented as a biquad filter. */
} ma_lpf2;

MA_API ma_result ma_lpf2_init(const ma_lpf2_config* pConfig, ma_lpf2* pLPF);
MA_API ma_result ma_lpf2_reinit(const ma_lpf2_config* pConfig, ma_lpf2* pLPF);
MA_API ma_result ma_lpf2_process_pcm_frames(ma_lpf2* pLPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_lpf2_get_latency(ma_lpf2* pLPF);


typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double cutoffFrequency;
    ma_uint32 order;    /* If set to 0, will be treated as a passthrough (no filtering will be applied). */
} ma_lpf_config;

MA_API ma_lpf_config ma_lpf_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency, ma_uint32 order);

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 lpf1Count;
    ma_uint32 lpf2Count;
    ma_lpf1 lpf1[1];
    ma_lpf2 lpf2[MA_MAX_FILTER_ORDER/2];
} ma_lpf;

MA_API ma_result ma_lpf_init(const ma_lpf_config* pConfig, ma_lpf* pLPF);
MA_API ma_result ma_lpf_reinit(const ma_lpf_config* pConfig, ma_lpf* pLPF);
MA_API ma_result ma_lpf_process_pcm_frames(ma_lpf* pLPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_lpf_get_latency(ma_lpf* pLPF);


/**************************************************************************************************************************************************************

High-Pass Filtering

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double cutoffFrequency;
    double q;
} ma_hpf1_config, ma_hpf2_config;

MA_API ma_hpf1_config ma_hpf1_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency);
MA_API ma_hpf2_config ma_hpf2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency, double q);

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_biquad_coefficient a;
    ma_biquad_coefficient r1[MA_MAX_CHANNELS];
} ma_hpf1;

MA_API ma_result ma_hpf1_init(const ma_hpf1_config* pConfig, ma_hpf1* pHPF);
MA_API ma_result ma_hpf1_reinit(const ma_hpf1_config* pConfig, ma_hpf1* pHPF);
MA_API ma_result ma_hpf1_process_pcm_frames(ma_hpf1* pHPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_hpf1_get_latency(ma_hpf1* pHPF);

typedef struct
{
    ma_biquad bq;   /* The second order high-pass filter is implemented as a biquad filter. */
} ma_hpf2;

MA_API ma_result ma_hpf2_init(const ma_hpf2_config* pConfig, ma_hpf2* pHPF);
MA_API ma_result ma_hpf2_reinit(const ma_hpf2_config* pConfig, ma_hpf2* pHPF);
MA_API ma_result ma_hpf2_process_pcm_frames(ma_hpf2* pHPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_hpf2_get_latency(ma_hpf2* pHPF);


typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double cutoffFrequency;
    ma_uint32 order;    /* If set to 0, will be treated as a passthrough (no filtering will be applied). */
} ma_hpf_config;

MA_API ma_hpf_config ma_hpf_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency, ma_uint32 order);

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 hpf1Count;
    ma_uint32 hpf2Count;
    ma_hpf1 hpf1[1];
    ma_hpf2 hpf2[MA_MAX_FILTER_ORDER/2];
} ma_hpf;

MA_API ma_result ma_hpf_init(const ma_hpf_config* pConfig, ma_hpf* pHPF);
MA_API ma_result ma_hpf_reinit(const ma_hpf_config* pConfig, ma_hpf* pHPF);
MA_API ma_result ma_hpf_process_pcm_frames(ma_hpf* pHPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_hpf_get_latency(ma_hpf* pHPF);


/**************************************************************************************************************************************************************

Band-Pass Filtering

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double cutoffFrequency;
    double q;
} ma_bpf2_config;

MA_API ma_bpf2_config ma_bpf2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency, double q);

typedef struct
{
    ma_biquad bq;   /* The second order band-pass filter is implemented as a biquad filter. */
} ma_bpf2;

MA_API ma_result ma_bpf2_init(const ma_bpf2_config* pConfig, ma_bpf2* pBPF);
MA_API ma_result ma_bpf2_reinit(const ma_bpf2_config* pConfig, ma_bpf2* pBPF);
MA_API ma_result ma_bpf2_process_pcm_frames(ma_bpf2* pBPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_bpf2_get_latency(ma_bpf2* pBPF);


typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double cutoffFrequency;
    ma_uint32 order;    /* If set to 0, will be treated as a passthrough (no filtering will be applied). */
} ma_bpf_config;

MA_API ma_bpf_config ma_bpf_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double cutoffFrequency, ma_uint32 order);

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 bpf2Count;
    ma_bpf2 bpf2[MA_MAX_FILTER_ORDER/2];
} ma_bpf;

MA_API ma_result ma_bpf_init(const ma_bpf_config* pConfig, ma_bpf* pBPF);
MA_API ma_result ma_bpf_reinit(const ma_bpf_config* pConfig, ma_bpf* pBPF);
MA_API ma_result ma_bpf_process_pcm_frames(ma_bpf* pBPF, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_bpf_get_latency(ma_bpf* pBPF);


/**************************************************************************************************************************************************************

Notching Filter

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double q;
    double frequency;
} ma_notch2_config;

MA_API ma_notch2_config ma_notch2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double q, double frequency);

typedef struct
{
    ma_biquad bq;
} ma_notch2;

MA_API ma_result ma_notch2_init(const ma_notch2_config* pConfig, ma_notch2* pFilter);
MA_API ma_result ma_notch2_reinit(const ma_notch2_config* pConfig, ma_notch2* pFilter);
MA_API ma_result ma_notch2_process_pcm_frames(ma_notch2* pFilter, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_notch2_get_latency(ma_notch2* pFilter);


/**************************************************************************************************************************************************************

Peaking EQ Filter

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double gainDB;
    double q;
    double frequency;
} ma_peak2_config;

MA_API ma_peak2_config ma_peak2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double gainDB, double q, double frequency);

typedef struct
{
    ma_biquad bq;
} ma_peak2;

MA_API ma_result ma_peak2_init(const ma_peak2_config* pConfig, ma_peak2* pFilter);
MA_API ma_result ma_peak2_reinit(const ma_peak2_config* pConfig, ma_peak2* pFilter);
MA_API ma_result ma_peak2_process_pcm_frames(ma_peak2* pFilter, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_peak2_get_latency(ma_peak2* pFilter);


/**************************************************************************************************************************************************************

Low Shelf Filter

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double gainDB;
    double shelfSlope;
    double frequency;
} ma_loshelf2_config;

MA_API ma_loshelf2_config ma_loshelf2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double gainDB, double shelfSlope, double frequency);

typedef struct
{
    ma_biquad bq;
} ma_loshelf2;

MA_API ma_result ma_loshelf2_init(const ma_loshelf2_config* pConfig, ma_loshelf2* pFilter);
MA_API ma_result ma_loshelf2_reinit(const ma_loshelf2_config* pConfig, ma_loshelf2* pFilter);
MA_API ma_result ma_loshelf2_process_pcm_frames(ma_loshelf2* pFilter, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_loshelf2_get_latency(ma_loshelf2* pFilter);


/**************************************************************************************************************************************************************

High Shelf Filter

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    double gainDB;
    double shelfSlope;
    double frequency;
} ma_hishelf2_config;

MA_API ma_hishelf2_config ma_hishelf2_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, double gainDB, double shelfSlope, double frequency);

typedef struct
{
    ma_biquad bq;
} ma_hishelf2;

MA_API ma_result ma_hishelf2_init(const ma_hishelf2_config* pConfig, ma_hishelf2* pFilter);
MA_API ma_result ma_hishelf2_reinit(const ma_hishelf2_config* pConfig, ma_hishelf2* pFilter);
MA_API ma_result ma_hishelf2_process_pcm_frames(ma_hishelf2* pFilter, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);
MA_API ma_uint32 ma_hishelf2_get_latency(ma_hishelf2* pFilter);



/************************************************************************************************************************************************************
*************************************************************************************************************************************************************

DATA CONVERSION
===============

This section contains the APIs for data conversion. You will find everything here for channel mapping, sample format conversion, resampling, etc.

*************************************************************************************************************************************************************
************************************************************************************************************************************************************/

/**************************************************************************************************************************************************************

Resampling

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRateIn;
    ma_uint32 sampleRateOut;
    ma_uint32 lpfOrder;         /* The low-pass filter order. Setting this to 0 will disable low-pass filtering. */
    double    lpfNyquistFactor; /* 0..1. Defaults to 1. 1 = Half the sampling frequency (Nyquist Frequency), 0.5 = Quarter the sampling frequency (half Nyquest Frequency), etc. */
} ma_linear_resampler_config;

MA_API ma_linear_resampler_config ma_linear_resampler_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut);

typedef struct
{
    ma_linear_resampler_config config;
    ma_uint32 inAdvanceInt;
    ma_uint32 inAdvanceFrac;
    ma_uint32 inTimeInt;
    ma_uint32 inTimeFrac;
    union
    {
        float    f32[MA_MAX_CHANNELS];
        ma_int16 s16[MA_MAX_CHANNELS];
    } x0; /* The previous input frame. */
    union
    {
        float    f32[MA_MAX_CHANNELS];
        ma_int16 s16[MA_MAX_CHANNELS];
    } x1; /* The next input frame. */
    ma_lpf lpf;
} ma_linear_resampler;

MA_API ma_result ma_linear_resampler_init(const ma_linear_resampler_config* pConfig, ma_linear_resampler* pResampler);
MA_API void ma_linear_resampler_uninit(ma_linear_resampler* pResampler);
MA_API ma_result ma_linear_resampler_process_pcm_frames(ma_linear_resampler* pResampler, const void* pFramesIn, ma_uint64* pFrameCountIn, void* pFramesOut, ma_uint64* pFrameCountOut);
MA_API ma_result ma_linear_resampler_set_rate(ma_linear_resampler* pResampler, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut);
MA_API ma_result ma_linear_resampler_set_rate_ratio(ma_linear_resampler* pResampler, float ratioInOut);
MA_API ma_uint64 ma_linear_resampler_get_required_input_frame_count(ma_linear_resampler* pResampler, ma_uint64 outputFrameCount);
MA_API ma_uint64 ma_linear_resampler_get_expected_output_frame_count(ma_linear_resampler* pResampler, ma_uint64 inputFrameCount);
MA_API ma_uint64 ma_linear_resampler_get_input_latency(ma_linear_resampler* pResampler);
MA_API ma_uint64 ma_linear_resampler_get_output_latency(ma_linear_resampler* pResampler);

typedef enum
{
    ma_resample_algorithm_linear = 0,   /* Fastest, lowest quality. Optional low-pass filtering. Default. */
    ma_resample_algorithm_speex
} ma_resample_algorithm;

typedef struct
{
    ma_format format;   /* Must be either ma_format_f32 or ma_format_s16. */
    ma_uint32 channels;
    ma_uint32 sampleRateIn;
    ma_uint32 sampleRateOut;
    ma_resample_algorithm algorithm;
    struct
    {
        ma_uint32 lpfOrder;
        double lpfNyquistFactor;
    } linear;
    struct
    {
        int quality;    /* 0 to 10. Defaults to 3. */
    } speex;
} ma_resampler_config;

MA_API ma_resampler_config ma_resampler_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut, ma_resample_algorithm algorithm);

typedef struct
{
    ma_resampler_config config;
    union
    {
        ma_linear_resampler linear;
        struct
        {
            void* pSpeexResamplerState;   /* SpeexResamplerState* */
        } speex;
    } state;
} ma_resampler;

/*
Initializes a new resampler object from a config.
*/
MA_API ma_result ma_resampler_init(const ma_resampler_config* pConfig, ma_resampler* pResampler);

/*
Uninitializes a resampler.
*/
MA_API void ma_resampler_uninit(ma_resampler* pResampler);

/*
Converts the given input data.

Both the input and output frames must be in the format specified in the config when the resampler was initilized.

On input, [pFrameCountOut] contains the number of output frames to process. On output it contains the number of output frames that
were actually processed, which may be less than the requested amount which will happen if there's not enough input data. You can use
ma_resampler_get_expected_output_frame_count() to know how many output frames will be processed for a given number of input frames.

On input, [pFrameCountIn] contains the number of input frames contained in [pFramesIn]. On output it contains the number of whole
input frames that were actually processed. You can use ma_resampler_get_required_input_frame_count() to know how many input frames
you should provide for a given number of output frames. [pFramesIn] can be NULL, in which case zeroes will be used instead.

If [pFramesOut] is NULL, a seek is performed. In this case, if [pFrameCountOut] is not NULL it will seek by the specified number of
output frames. Otherwise, if [pFramesCountOut] is NULL and [pFrameCountIn] is not NULL, it will seek by the specified number of input
frames. When seeking, [pFramesIn] is allowed to NULL, in which case the internal timing state will be updated, but no input will be
processed. In this case, any internal filter state will be updated as if zeroes were passed in.

It is an error for [pFramesOut] to be non-NULL and [pFrameCountOut] to be NULL.

It is an error for both [pFrameCountOut] and [pFrameCountIn] to be NULL.
*/
MA_API ma_result ma_resampler_process_pcm_frames(ma_resampler* pResampler, const void* pFramesIn, ma_uint64* pFrameCountIn, void* pFramesOut, ma_uint64* pFrameCountOut);


/*
Sets the input and output sample sample rate.
*/
MA_API ma_result ma_resampler_set_rate(ma_resampler* pResampler, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut);

/*
Sets the input and output sample rate as a ratio.

The ration is in/out.
*/
MA_API ma_result ma_resampler_set_rate_ratio(ma_resampler* pResampler, float ratio);


/*
Calculates the number of whole input frames that would need to be read from the client in order to output the specified
number of output frames.

The returned value does not include cached input frames. It only returns the number of extra frames that would need to be
read from the input buffer in order to output the specified number of output frames.
*/
MA_API ma_uint64 ma_resampler_get_required_input_frame_count(ma_resampler* pResampler, ma_uint64 outputFrameCount);

/*
Calculates the number of whole output frames that would be output after fully reading and consuming the specified number of
input frames.
*/
MA_API ma_uint64 ma_resampler_get_expected_output_frame_count(ma_resampler* pResampler, ma_uint64 inputFrameCount);


/*
Retrieves the latency introduced by the resampler in input frames.
*/
MA_API ma_uint64 ma_resampler_get_input_latency(ma_resampler* pResampler);

/*
Retrieves the latency introduced by the resampler in output frames.
*/
MA_API ma_uint64 ma_resampler_get_output_latency(ma_resampler* pResampler);



/**************************************************************************************************************************************************************

Channel Conversion

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format format;
    ma_uint32 channelsIn;
    ma_uint32 channelsOut;
    ma_channel channelMapIn[MA_MAX_CHANNELS];
    ma_channel channelMapOut[MA_MAX_CHANNELS];
    ma_channel_mix_mode mixingMode;
    float weights[MA_MAX_CHANNELS][MA_MAX_CHANNELS];  /* [in][out]. Only used when mixingMode is set to ma_channel_mix_mode_custom_weights. */
} ma_channel_converter_config;

MA_API ma_channel_converter_config ma_channel_converter_config_init(ma_format format, ma_uint32 channelsIn, const ma_channel* pChannelMapIn, ma_uint32 channelsOut, const ma_channel* pChannelMapOut, ma_channel_mix_mode mixingMode);

typedef struct
{
    ma_format format;
    ma_uint32 channelsIn;
    ma_uint32 channelsOut;
    ma_channel channelMapIn[MA_MAX_CHANNELS];
    ma_channel channelMapOut[MA_MAX_CHANNELS];
    ma_channel_mix_mode mixingMode;
    union
    {
        float    f32[MA_MAX_CHANNELS][MA_MAX_CHANNELS];
        ma_int32 s16[MA_MAX_CHANNELS][MA_MAX_CHANNELS];
    } weights;
    ma_bool32 isPassthrough         : 1;
    ma_bool32 isSimpleShuffle       : 1;
    ma_bool32 isSimpleMonoExpansion : 1;
    ma_bool32 isStereoToMono        : 1;
    ma_uint8  shuffleTable[MA_MAX_CHANNELS];
} ma_channel_converter;

MA_API ma_result ma_channel_converter_init(const ma_channel_converter_config* pConfig, ma_channel_converter* pConverter);
MA_API void ma_channel_converter_uninit(ma_channel_converter* pConverter);
MA_API ma_result ma_channel_converter_process_pcm_frames(ma_channel_converter* pConverter, void* pFramesOut, const void* pFramesIn, ma_uint64 frameCount);


/**************************************************************************************************************************************************************

Data Conversion

**************************************************************************************************************************************************************/
typedef struct
{
    ma_format formatIn;
    ma_format formatOut;
    ma_uint32 channelsIn;
    ma_uint32 channelsOut;
    ma_uint32 sampleRateIn;
    ma_uint32 sampleRateOut;
    ma_channel channelMapIn[MA_MAX_CHANNELS];
    ma_channel channelMapOut[MA_MAX_CHANNELS];
    ma_dither_mode ditherMode;
    ma_channel_mix_mode channelMixMode;
    float channelWeights[MA_MAX_CHANNELS][MA_MAX_CHANNELS];  /* [in][out]. Only used when channelMixMode is set to ma_channel_mix_mode_custom_weights. */
    struct
    {
        ma_resample_algorithm algorithm;
        ma_bool32 allowDynamicSampleRate;
        struct
        {
            ma_uint32 lpfOrder;
            double lpfNyquistFactor;
        } linear;
        struct
        {
            int quality;
        } speex;
    } resampling;
} ma_data_converter_config;

MA_API ma_data_converter_config ma_data_converter_config_init_default(void);
MA_API ma_data_converter_config ma_data_converter_config_init(ma_format formatIn, ma_format formatOut, ma_uint32 channelsIn, ma_uint32 channelsOut, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut);

typedef struct
{
    ma_data_converter_config config;
    ma_channel_converter channelConverter;
    ma_resampler resampler;
    ma_bool32 hasPreFormatConversion  : 1;
    ma_bool32 hasPostFormatConversion : 1;
    ma_bool32 hasChannelConverter     : 1;
    ma_bool32 hasResampler            : 1;
    ma_bool32 isPassthrough           : 1;
} ma_data_converter;

MA_API ma_result ma_data_converter_init(const ma_data_converter_config* pConfig, ma_data_converter* pConverter);
MA_API void ma_data_converter_uninit(ma_data_converter* pConverter);
MA_API ma_result ma_data_converter_process_pcm_frames(ma_data_converter* pConverter, const void* pFramesIn, ma_uint64* pFrameCountIn, void* pFramesOut, ma_uint64* pFrameCountOut);
MA_API ma_result ma_data_converter_set_rate(ma_data_converter* pConverter, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut);
MA_API ma_result ma_data_converter_set_rate_ratio(ma_data_converter* pConverter, float ratioInOut);
MA_API ma_uint64 ma_data_converter_get_required_input_frame_count(ma_data_converter* pConverter, ma_uint64 outputFrameCount);
MA_API ma_uint64 ma_data_converter_get_expected_output_frame_count(ma_data_converter* pConverter, ma_uint64 inputFrameCount);
MA_API ma_uint64 ma_data_converter_get_input_latency(ma_data_converter* pConverter);
MA_API ma_uint64 ma_data_converter_get_output_latency(ma_data_converter* pConverter);


/************************************************************************************************************************************************************

Format Conversion

************************************************************************************************************************************************************/
MA_API void ma_pcm_u8_to_s16(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_u8_to_s24(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_u8_to_s32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_u8_to_f32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s16_to_u8(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s16_to_s24(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s16_to_s32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s16_to_f32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s24_to_u8(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s24_to_s16(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s24_to_s32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s24_to_f32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s32_to_u8(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s32_to_s16(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s32_to_s24(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_s32_to_f32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_f32_to_u8(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_f32_to_s16(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_f32_to_s24(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_f32_to_s32(void* pOut, const void* pIn, ma_uint64 count, ma_dither_mode ditherMode);
MA_API void ma_pcm_convert(void* pOut, ma_format formatOut, const void* pIn, ma_format formatIn, ma_uint64 sampleCount, ma_dither_mode ditherMode);
MA_API void ma_convert_pcm_frames_format(void* pOut, ma_format formatOut, const void* pIn, ma_format formatIn, ma_uint64 frameCount, ma_uint32 channels, ma_dither_mode ditherMode);

/*
Deinterleaves an interleaved buffer.
*/
MA_API void ma_deinterleave_pcm_frames(ma_format format, ma_uint32 channels, ma_uint64 frameCount, const void* pInterleavedPCMFrames, void** ppDeinterleavedPCMFrames);

/*
Interleaves a group of deinterleaved buffers.
*/
MA_API void ma_interleave_pcm_frames(ma_format format, ma_uint32 channels, ma_uint64 frameCount, const void** ppDeinterleavedPCMFrames, void* pInterleavedPCMFrames);

/************************************************************************************************************************************************************

Channel Maps

************************************************************************************************************************************************************/

/*
Helper for retrieving a standard channel map.

The output channel map buffer must have a capacity of at least `channels`.
*/
MA_API void ma_get_standard_channel_map(ma_standard_channel_map standardChannelMap, ma_uint32 channels, ma_channel* pChannelMap);

/*
Copies a channel map.

Both input and output channel map buffers must have a capacity of at at least `channels`.
*/
MA_API void ma_channel_map_copy(ma_channel* pOut, const ma_channel* pIn, ma_uint32 channels);


/*
Determines whether or not a channel map is valid.

A blank channel map is valid (all channels set to MA_CHANNEL_NONE). The way a blank channel map is handled is context specific, but
is usually treated as a passthrough.

Invalid channel maps:
  - A channel map with no channels
  - A channel map with more than one channel and a mono channel

The channel map buffer must have a capacity of at least `channels`.
*/
MA_API ma_bool32 ma_channel_map_valid(ma_uint32 channels, const ma_channel* pChannelMap);

/*
Helper for comparing two channel maps for equality.

This assumes the channel count is the same between the two.

Both channels map buffers must have a capacity of at least `channels`.
*/
MA_API ma_bool32 ma_channel_map_equal(ma_uint32 channels, const ma_channel* pChannelMapA, const ma_channel* pChannelMapB);

/*
Helper for determining if a channel map is blank (all channels set to MA_CHANNEL_NONE).

The channel map buffer must have a capacity of at least `channels`.
*/
MA_API ma_bool32 ma_channel_map_blank(ma_uint32 channels, const ma_channel* pChannelMap);

/*
Helper for determining whether or not a channel is present in the given channel map.

The channel map buffer must have a capacity of at least `channels`.
*/
MA_API ma_bool32 ma_channel_map_contains_channel_position(ma_uint32 channels, const ma_channel* pChannelMap, ma_channel channelPosition);


/************************************************************************************************************************************************************

Conversion Helpers

************************************************************************************************************************************************************/

/*
High-level helper for doing a full format conversion in one go. Returns the number of output frames. Call this with pOut set to NULL to
determine the required size of the output buffer. frameCountOut should be set to the capacity of pOut. If pOut is NULL, frameCountOut is
ignored.

A return value of 0 indicates an error.

This function is useful for one-off bulk conversions, but if you're streaming data you should use the ma_data_converter APIs instead.
*/
MA_API ma_uint64 ma_convert_frames(void* pOut, ma_uint64 frameCountOut, ma_format formatOut, ma_uint32 channelsOut, ma_uint32 sampleRateOut, const void* pIn, ma_uint64 frameCountIn, ma_format formatIn, ma_uint32 channelsIn, ma_uint32 sampleRateIn);
MA_API ma_uint64 ma_convert_frames_ex(void* pOut, ma_uint64 frameCountOut, const void* pIn, ma_uint64 frameCountIn, const ma_data_converter_config* pConfig);


/************************************************************************************************************************************************************

Ring Buffer

************************************************************************************************************************************************************/
typedef struct
{
    void* pBuffer;
    ma_uint32 subbufferSizeInBytes;
    ma_uint32 subbufferCount;
    ma_uint32 subbufferStrideInBytes;
    volatile ma_uint32 encodedReadOffset;  /* Most significant bit is the loop flag. Lower 31 bits contains the actual offset in bytes. */
    volatile ma_uint32 encodedWriteOffset; /* Most significant bit is the loop flag. Lower 31 bits contains the actual offset in bytes. */
    ma_bool32 ownsBuffer          : 1;     /* Used to know whether or not miniaudio is responsible for free()-ing the buffer. */
    ma_bool32 clearOnWriteAcquire : 1;     /* When set, clears the acquired write buffer before returning from ma_rb_acquire_write(). */
    ma_allocation_callbacks allocationCallbacks;
} ma_rb;

MA_API ma_result ma_rb_init_ex(size_t subbufferSizeInBytes, size_t subbufferCount, size_t subbufferStrideInBytes, void* pOptionalPreallocatedBuffer, const ma_allocation_callbacks* pAllocationCallbacks, ma_rb* pRB);
MA_API ma_result ma_rb_init(size_t bufferSizeInBytes, void* pOptionalPreallocatedBuffer, const ma_allocation_callbacks* pAllocationCallbacks, ma_rb* pRB);
MA_API void ma_rb_uninit(ma_rb* pRB);
MA_API void ma_rb_reset(ma_rb* pRB);
MA_API ma_result ma_rb_acquire_read(ma_rb* pRB, size_t* pSizeInBytes, void** ppBufferOut);
MA_API ma_result ma_rb_commit_read(ma_rb* pRB, size_t sizeInBytes, void* pBufferOut);
MA_API ma_result ma_rb_acquire_write(ma_rb* pRB, size_t* pSizeInBytes, void** ppBufferOut);
MA_API ma_result ma_rb_commit_write(ma_rb* pRB, size_t sizeInBytes, void* pBufferOut);
MA_API ma_result ma_rb_seek_read(ma_rb* pRB, size_t offsetInBytes);
MA_API ma_result ma_rb_seek_write(ma_rb* pRB, size_t offsetInBytes);
MA_API ma_int32 ma_rb_pointer_distance(ma_rb* pRB);    /* Returns the distance between the write pointer and the read pointer. Should never be negative for a correct program. Will return the number of bytes that can be read before the read pointer hits the write pointer. */
MA_API ma_uint32 ma_rb_available_read(ma_rb* pRB);
MA_API ma_uint32 ma_rb_available_write(ma_rb* pRB);
MA_API size_t ma_rb_get_subbuffer_size(ma_rb* pRB);
MA_API size_t ma_rb_get_subbuffer_stride(ma_rb* pRB);
MA_API size_t ma_rb_get_subbuffer_offset(ma_rb* pRB, size_t subbufferIndex);
MA_API void* ma_rb_get_subbuffer_ptr(ma_rb* pRB, size_t subbufferIndex, void* pBuffer);


typedef struct
{
    ma_rb rb;
    ma_format format;
    ma_uint32 channels;
} ma_pcm_rb;

MA_API ma_result ma_pcm_rb_init_ex(ma_format format, ma_uint32 channels, ma_uint32 subbufferSizeInFrames, ma_uint32 subbufferCount, ma_uint32 subbufferStrideInFrames, void* pOptionalPreallocatedBuffer, const ma_allocation_callbacks* pAllocationCallbacks, ma_pcm_rb* pRB);
MA_API ma_result ma_pcm_rb_init(ma_format format, ma_uint32 channels, ma_uint32 bufferSizeInFrames, void* pOptionalPreallocatedBuffer, const ma_allocation_callbacks* pAllocationCallbacks, ma_pcm_rb* pRB);
MA_API void ma_pcm_rb_uninit(ma_pcm_rb* pRB);
MA_API void ma_pcm_rb_reset(ma_pcm_rb* pRB);
MA_API ma_result ma_pcm_rb_acquire_read(ma_pcm_rb* pRB, ma_uint32* pSizeInFrames, void** ppBufferOut);
MA_API ma_result ma_pcm_rb_commit_read(ma_pcm_rb* pRB, ma_uint32 sizeInFrames, void* pBufferOut);
MA_API ma_result ma_pcm_rb_acquire_write(ma_pcm_rb* pRB, ma_uint32* pSizeInFrames, void** ppBufferOut);
MA_API ma_result ma_pcm_rb_commit_write(ma_pcm_rb* pRB, ma_uint32 sizeInFrames, void* pBufferOut);
MA_API ma_result ma_pcm_rb_seek_read(ma_pcm_rb* pRB, ma_uint32 offsetInFrames);
MA_API ma_result ma_pcm_rb_seek_write(ma_pcm_rb* pRB, ma_uint32 offsetInFrames);
MA_API ma_int32 ma_pcm_rb_pointer_distance(ma_pcm_rb* pRB); /* Return value is in frames. */
MA_API ma_uint32 ma_pcm_rb_available_read(ma_pcm_rb* pRB);
MA_API ma_uint32 ma_pcm_rb_available_write(ma_pcm_rb* pRB);
MA_API ma_uint32 ma_pcm_rb_get_subbuffer_size(ma_pcm_rb* pRB);
MA_API ma_uint32 ma_pcm_rb_get_subbuffer_stride(ma_pcm_rb* pRB);
MA_API ma_uint32 ma_pcm_rb_get_subbuffer_offset(ma_pcm_rb* pRB, ma_uint32 subbufferIndex);
MA_API void* ma_pcm_rb_get_subbuffer_ptr(ma_pcm_rb* pRB, ma_uint32 subbufferIndex, void* pBuffer);


/************************************************************************************************************************************************************

Miscellaneous Helpers

************************************************************************************************************************************************************/
/*
Retrieves a human readable description of the given result code.
*/
MA_API const char* ma_result_description(ma_result result);

/*
malloc(). Calls MA_MALLOC().
*/
MA_API void* ma_malloc(size_t sz, const ma_allocation_callbacks* pAllocationCallbacks);

/*
realloc(). Calls MA_REALLOC().
*/
MA_API void* ma_realloc(void* p, size_t sz, const ma_allocation_callbacks* pAllocationCallbacks);

/*
free(). Calls MA_FREE().
*/
MA_API void ma_free(void* p, const ma_allocation_callbacks* pAllocationCallbacks);

/*
Performs an aligned malloc, with the assumption that the alignment is a power of 2.
*/
MA_API void* ma_aligned_malloc(size_t sz, size_t alignment, const ma_allocation_callbacks* pAllocationCallbacks);

/*
Free's an aligned malloc'd buffer.
*/
MA_API void ma_aligned_free(void* p, const ma_allocation_callbacks* pAllocationCallbacks);

/*
Retrieves a friendly name for a format.
*/
MA_API const char* ma_get_format_name(ma_format format);

/*
Blends two frames in floating point format.
*/
MA_API void ma_blend_f32(float* pOut, float* pInA, float* pInB, float factor, ma_uint32 channels);

/*
Retrieves the size of a sample in bytes for the given format.

This API is efficient and is implemented using a lookup table.

Thread Safety: SAFE
  This API is pure.
*/
MA_API ma_uint32 ma_get_bytes_per_sample(ma_format format);
static MA_INLINE ma_uint32 ma_get_bytes_per_frame(ma_format format, ma_uint32 channels) { return ma_get_bytes_per_sample(format) * channels; }

/*
Converts a log level to a string.
*/
MA_API const char* ma_log_level_to_string(ma_uint32 logLevel);



/************************************************************************************************************************************************************
*************************************************************************************************************************************************************

DEVICE I/O
==========

This section contains the APIs for device playback and capture. Here is where you'll find ma_device_init(), etc.

*************************************************************************************************************************************************************
************************************************************************************************************************************************************/
#ifndef MA_NO_DEVICE_IO
/* Some backends are only supported on certain platforms. */
#if defined(MA_WIN32)
    #define MA_SUPPORT_WASAPI
    #if defined(MA_WIN32_DESKTOP)  /* DirectSound and WinMM backends are only supported on desktops. */
        #define MA_SUPPORT_DSOUND
        #define MA_SUPPORT_WINMM
        #define MA_SUPPORT_JACK    /* JACK is technically supported on Windows, but I don't know how many people use it in practice... */
    #endif
#endif
#if defined(MA_UNIX)
    #if defined(MA_LINUX)
        #if !defined(MA_ANDROID)   /* ALSA is not supported on Android. */
            #define MA_SUPPORT_ALSA
        #endif
    #endif
    #if !defined(MA_BSD) && !defined(MA_ANDROID) && !defined(MA_EMSCRIPTEN)
        #define MA_SUPPORT_PULSEAUDIO
        #define MA_SUPPORT_JACK
    #endif
    #if defined(MA_ANDROID)
        #define MA_SUPPORT_AAUDIO
        #define MA_SUPPORT_OPENSL
    #endif
    #if defined(__OpenBSD__)        /* <-- Change this to "#if defined(MA_BSD)" to enable sndio on all BSD flavors. */
        #define MA_SUPPORT_SNDIO    /* sndio is only supported on OpenBSD for now. May be expanded later if there's demand. */
    #endif
    #if defined(__NetBSD__) || defined(__OpenBSD__)
        #define MA_SUPPORT_AUDIO4   /* Only support audio(4) on platforms with known support. */
    #endif
    #if defined(__FreeBSD__) || defined(__DragonFly__)
        #define MA_SUPPORT_OSS      /* Only support OSS on specific platforms with known support. */
    #endif
#endif
#if defined(MA_APPLE)
    #define MA_SUPPORT_COREAUDIO
#endif
#if defined(MA_EMSCRIPTEN)
    #define MA_SUPPORT_WEBAUDIO
#endif

/* Explicitly disable the Null backend for Emscripten because it uses a background thread which is not properly supported right now. */
#if !defined(MA_EMSCRIPTEN)
#define MA_SUPPORT_NULL
#endif


#if !defined(MA_NO_WASAPI) && defined(MA_SUPPORT_WASAPI)
    #define MA_ENABLE_WASAPI
#endif
#if !defined(MA_NO_DSOUND) && defined(MA_SUPPORT_DSOUND)
    #define MA_ENABLE_DSOUND
#endif
#if !defined(MA_NO_WINMM) && defined(MA_SUPPORT_WINMM)
    #define MA_ENABLE_WINMM
#endif
#if !defined(MA_NO_ALSA) && defined(MA_SUPPORT_ALSA)
    #define MA_ENABLE_ALSA
#endif
#if !defined(MA_NO_PULSEAUDIO) && defined(MA_SUPPORT_PULSEAUDIO)
    #define MA_ENABLE_PULSEAUDIO
#endif
#if !defined(MA_NO_JACK) && defined(MA_SUPPORT_JACK)
    #define MA_ENABLE_JACK
#endif
#if !defined(MA_NO_COREAUDIO) && defined(MA_SUPPORT_COREAUDIO)
    #define MA_ENABLE_COREAUDIO
#endif
#if !defined(MA_NO_SNDIO) && defined(MA_SUPPORT_SNDIO)
    #define MA_ENABLE_SNDIO
#endif
#if !defined(MA_NO_AUDIO4) && defined(MA_SUPPORT_AUDIO4)
    #define MA_ENABLE_AUDIO4
#endif
#if !defined(MA_NO_OSS) && defined(MA_SUPPORT_OSS)
    #define MA_ENABLE_OSS
#endif
#if !defined(MA_NO_AAUDIO) && defined(MA_SUPPORT_AAUDIO)
    #define MA_ENABLE_AAUDIO
#endif
#if !defined(MA_NO_OPENSL) && defined(MA_SUPPORT_OPENSL)
    #define MA_ENABLE_OPENSL
#endif
#if !defined(MA_NO_WEBAUDIO) && defined(MA_SUPPORT_WEBAUDIO)
    #define MA_ENABLE_WEBAUDIO
#endif
#if !defined(MA_NO_NULL) && defined(MA_SUPPORT_NULL)
    #define MA_ENABLE_NULL
#endif

#ifdef MA_SUPPORT_WASAPI
/* We need a IMMNotificationClient object for WASAPI. */
typedef struct
{
    void* lpVtbl;
    ma_uint32 counter;
    ma_device* pDevice;
} ma_IMMNotificationClient;
#endif

/* Backend enums must be in priority order. */
typedef enum
{
    ma_backend_wasapi,
    ma_backend_dsound,
    ma_backend_winmm,
    ma_backend_coreaudio,
    ma_backend_sndio,
    ma_backend_audio4,
    ma_backend_oss,
    ma_backend_pulseaudio,
    ma_backend_alsa,
    ma_backend_jack,
    ma_backend_aaudio,
    ma_backend_opensl,
    ma_backend_webaudio,
    ma_backend_null    /* <-- Must always be the last item. Lowest priority, and used as the terminator for backend enumeration. */
} ma_backend;


/*
The callback for processing audio data from the device.

The data callback is fired by miniaudio whenever the device needs to have more data delivered to a playback device, or when a capture device has some data
available. This is called as soon as the backend asks for more data which means it may be called with inconsistent frame counts. You cannot assume the
callback will be fired with a consistent frame count.


Parameters
----------
pDevice (in)
    A pointer to the relevant device.

pOutput (out)
    A pointer to the output buffer that will receive audio data that will later be played back through the speakers. This will be non-null for a playback or
    full-duplex device and null for a capture and loopback device.

pInput (in)
    A pointer to the buffer containing input data from a recording device. This will be non-null for a capture, full-duplex or loopback device and null for a
    playback device.

frameCount (in)
    The number of PCM frames to process. Note that this will not necessarily be equal to what you requested when you initialized the device. The
    `periodSizeInFrames` and `periodSizeInMilliseconds` members of the device config are just hints, and are not necessarily exactly what you'll get. You must
    not assume this will always be the same value each time the callback is fired.


Remarks
-------
You cannot stop and start the device from inside the callback or else you'll get a deadlock. You must also not uninitialize the device from inside the
callback. The following APIs cannot be called from inside the callback:

    ma_device_init()
    ma_device_init_ex()
    ma_device_uninit()
    ma_device_start()
    ma_device_stop()

The proper way to stop the device is to call `ma_device_stop()` from a different thread, normally the main application thread.
*/
typedef void (* ma_device_callback_proc)(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

/*
The callback for when the device has been stopped.

This will be called when the device is stopped explicitly with `ma_device_stop()` and also called implicitly when the device is stopped through external forces
such as being unplugged or an internal error occuring.


Parameters
----------
pDevice (in)
    A pointer to the device that has just stopped.


Remarks
-------
Do not restart or uninitialize the device from the callback.
*/
typedef void (* ma_stop_proc)(ma_device* pDevice);

/*
The callback for handling log messages.


Parameters
----------
pContext (in)
    A pointer to the context the log message originated from.

pDevice (in)
    A pointer to the device the log message originate from, if any. This can be null, in which case the message came from the context.

logLevel (in)
    The log level. This can be one of the following:

    |----------------------|
    | Log Level            |
    |----------------------|
    | MA_LOG_LEVEL_VERBOSE |
    | MA_LOG_LEVEL_INFO    |
    | MA_LOG_LEVEL_WARNING |
    | MA_LOG_LEVEL_ERROR   |
    |----------------------|

message (in)
    The log message.


Remarks
-------
Do not modify the state of the device from inside the callback.
*/
typedef void (* ma_log_proc)(ma_context* pContext, ma_device* pDevice, ma_uint32 logLevel, const char* message);

typedef enum
{
    ma_device_type_playback = 1,
    ma_device_type_capture  = 2,
    ma_device_type_duplex   = ma_device_type_playback | ma_device_type_capture, /* 3 */
    ma_device_type_loopback = 4
} ma_device_type;

typedef enum
{
    ma_share_mode_shared = 0,
    ma_share_mode_exclusive
} ma_share_mode;

/* iOS/tvOS/watchOS session categories. */
typedef enum
{
    ma_ios_session_category_default = 0,        /* AVAudioSessionCategoryPlayAndRecord with AVAudioSessionCategoryOptionDefaultToSpeaker. */
    ma_ios_session_category_none,               /* Leave the session category unchanged. */
    ma_ios_session_category_ambient,            /* AVAudioSessionCategoryAmbient */
    ma_ios_session_category_solo_ambient,       /* AVAudioSessionCategorySoloAmbient */
    ma_ios_session_category_playback,           /* AVAudioSessionCategoryPlayback */
    ma_ios_session_category_record,             /* AVAudioSessionCategoryRecord */
    ma_ios_session_category_play_and_record,    /* AVAudioSessionCategoryPlayAndRecord */
    ma_ios_session_category_multi_route         /* AVAudioSessionCategoryMultiRoute */
} ma_ios_session_category;

/* iOS/tvOS/watchOS session category options */
typedef enum
{
    ma_ios_session_category_option_mix_with_others                            = 0x01,   /* AVAudioSessionCategoryOptionMixWithOthers */
    ma_ios_session_category_option_duck_others                                = 0x02,   /* AVAudioSessionCategoryOptionDuckOthers */
    ma_ios_session_category_option_allow_bluetooth                            = 0x04,   /* AVAudioSessionCategoryOptionAllowBluetooth */
    ma_ios_session_category_option_default_to_speaker                         = 0x08,   /* AVAudioSessionCategoryOptionDefaultToSpeaker */
    ma_ios_session_category_option_interrupt_spoken_audio_and_mix_with_others = 0x11,   /* AVAudioSessionCategoryOptionInterruptSpokenAudioAndMixWithOthers */
    ma_ios_session_category_option_allow_bluetooth_a2dp                       = 0x20,   /* AVAudioSessionCategoryOptionAllowBluetoothA2DP */
    ma_ios_session_category_option_allow_air_play                             = 0x40,   /* AVAudioSessionCategoryOptionAllowAirPlay */
} ma_ios_session_category_option;

typedef union
{
    ma_int64 counter;
    double counterD;
} ma_timer;

typedef union
{
    wchar_t wasapi[64];             /* WASAPI uses a wchar_t string for identification. */
    ma_uint8 dsound[16];            /* DirectSound uses a GUID for identification. */
    /*UINT_PTR*/ ma_uint32 winmm;   /* When creating a device, WinMM expects a Win32 UINT_PTR for device identification. In practice it's actually just a UINT. */
    char alsa[256];                 /* ALSA uses a name string for identification. */
    char pulse[256];                /* PulseAudio uses a name string for identification. */
    int jack;                       /* JACK always uses default devices. */
    char coreaudio[256];            /* Core Audio uses a string for identification. */
    char sndio[256];                /* "snd/0", etc. */
    char audio4[256];               /* "/dev/audio", etc. */
    char oss[64];                   /* "dev/dsp0", etc. "dev/dsp" for the default device. */
    ma_int32 aaudio;                /* AAudio uses a 32-bit integer for identification. */
    ma_uint32 opensl;               /* OpenSL|ES uses a 32-bit unsigned integer for identification. */
    char webaudio[32];              /* Web Audio always uses default devices for now, but if this changes it'll be a GUID. */
    int nullbackend;                /* The null backend uses an integer for device IDs. */
} ma_device_id;

typedef struct
{
    /* Basic info. This is the only information guaranteed to be filled in during device enumeration. */
    ma_device_id id;
    char name[256];

    /*
    Detailed info. As much of this is filled as possible with ma_context_get_device_info(). Note that you are allowed to initialize
    a device with settings outside of this range, but it just means the data will be converted using miniaudio's data conversion
    pipeline before sending the data to/from the device. Most programs will need to not worry about these values, but it's provided
    here mainly for informational purposes or in the rare case that someone might find it useful.

    These will be set to 0 when returned by ma_context_enumerate_devices() or ma_context_get_devices().
    */
    ma_uint32 formatCount;
    ma_format formats[ma_format_count];
    ma_uint32 minChannels;
    ma_uint32 maxChannels;
    ma_uint32 minSampleRate;
    ma_uint32 maxSampleRate;

    struct
    {
        ma_bool32 isDefault;
    } _private;
} ma_device_info;

typedef struct
{
    ma_device_type deviceType;
    ma_uint32 sampleRate;
    ma_uint32 periodSizeInFrames;
    ma_uint32 periodSizeInMilliseconds;
    ma_uint32 periods;
    ma_performance_profile performanceProfile;
    ma_bool32 noPreZeroedOutputBuffer;  /* When set to true, the contents of the output buffer passed into the data callback will be left undefined rather than initialized to zero. */
    ma_bool32 noClip;                   /* When set to true, the contents of the output buffer passed into the data callback will be clipped after returning. Only applies when the playback sample format is f32. */
    ma_device_callback_proc dataCallback;
    ma_stop_proc stopCallback;
    void* pUserData;
    struct
    {
        ma_resample_algorithm algorithm;
        struct
        {
            ma_uint32 lpfOrder;
        } linear;
        struct
        {
            int quality;
        } speex;
    } resampling;
    struct
    {
        const ma_device_id* pDeviceID;
        ma_format format;
        ma_uint32 channels;
        ma_channel channelMap[MA_MAX_CHANNELS];
        ma_share_mode shareMode;
    } playback;
    struct
    {
        const ma_device_id* pDeviceID;
        ma_format format;
        ma_uint32 channels;
        ma_channel channelMap[MA_MAX_CHANNELS];
        ma_share_mode shareMode;
    } capture;

    struct
    {
        ma_bool32 noAutoConvertSRC;     /* When set to true, disables the use of AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM. */
        ma_bool32 noDefaultQualitySRC;  /* When set to true, disables the use of AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY. */
        ma_bool32 noAutoStreamRouting;  /* Disables automatic stream routing. */
        ma_bool32 noHardwareOffloading; /* Disables WASAPI's hardware offloading feature. */
    } wasapi;
    struct
    {
        ma_bool32 noMMap;           /* Disables MMap mode. */
        ma_bool32 noAutoFormat;     /* Opens the ALSA device with SND_PCM_NO_AUTO_FORMAT. */
        ma_bool32 noAutoChannels;   /* Opens the ALSA device with SND_PCM_NO_AUTO_CHANNELS. */
        ma_bool32 noAutoResample;   /* Opens the ALSA device with SND_PCM_NO_AUTO_RESAMPLE. */
    } alsa;
    struct
    {
        const char* pStreamNamePlayback;
        const char* pStreamNameCapture;
    } pulse;
} ma_device_config;

typedef struct
{
    ma_log_proc logCallback;
    ma_thread_priority threadPriority;
    size_t threadStackSize;
    void* pUserData;
    ma_allocation_callbacks allocationCallbacks;
    struct
    {
        ma_bool32 useVerboseDeviceEnumeration;
    } alsa;
    struct
    {
        const char* pApplicationName;
        const char* pServerName;
        ma_bool32 tryAutoSpawn; /* Enables autospawning of the PulseAudio daemon if necessary. */
    } pulse;
    struct
    {
        ma_ios_session_category sessionCategory;
        ma_uint32 sessionCategoryOptions;
    } coreaudio;
    struct
    {
        const char* pClientName;
        ma_bool32 tryStartServer;
    } jack;
} ma_context_config;

/*
The callback for handling device enumeration. This is fired from `ma_context_enumerated_devices()`.


Parameters
----------
pContext (in)
    A pointer to the context performing the enumeration.

deviceType (in)
    The type of the device being enumerated. This will always be either `ma_device_type_playback` or `ma_device_type_capture`.

pInfo (in)
    A pointer to a `ma_device_info` containing the ID and name of the enumerated device. Note that this will not include detailed information about the device,
    only basic information (ID and name). The reason for this is that it would otherwise require opening the backend device to probe for the information which
    is too inefficient.

pUserData (in)
    The user data pointer passed into `ma_context_enumerate_devices()`.
*/
typedef ma_bool32 (* ma_enum_devices_callback_proc)(ma_context* pContext, ma_device_type deviceType, const ma_device_info* pInfo, void* pUserData);

struct ma_context
{
    ma_backend backend;                    /* DirectSound, ALSA, etc. */
    ma_log_proc logCallback;
    ma_thread_priority threadPriority;
    size_t threadStackSize;
    void* pUserData;
    ma_allocation_callbacks allocationCallbacks;
    ma_mutex deviceEnumLock;               /* Used to make ma_context_get_devices() thread safe. */
    ma_mutex deviceInfoLock;               /* Used to make ma_context_get_device_info() thread safe. */
    ma_uint32 deviceInfoCapacity;          /* Total capacity of pDeviceInfos. */
    ma_uint32 playbackDeviceInfoCount;
    ma_uint32 captureDeviceInfoCount;
    ma_device_info* pDeviceInfos;          /* Playback devices first, then capture. */
    ma_bool32 isBackendAsynchronous : 1;   /* Set when the context is initialized. Set to 1 for asynchronous backends such as Core Audio and JACK. Do not modify. */

    ma_result (* onUninit        )(ma_context* pContext);
    ma_bool32 (* onDeviceIDEqual )(ma_context* pContext, const ma_device_id* pID0, const ma_device_id* pID1);
    ma_result (* onEnumDevices   )(ma_context* pContext, ma_enum_devices_callback_proc callback, void* pUserData);    /* Return false from the callback to stop enumeration. */
    ma_result (* onGetDeviceInfo )(ma_context* pContext, ma_device_type deviceType, const ma_device_id* pDeviceID, ma_share_mode shareMode, ma_device_info* pDeviceInfo);
    ma_result (* onDeviceInit    )(ma_context* pContext, const ma_device_config* pConfig, ma_device* pDevice);
    void      (* onDeviceUninit  )(ma_device* pDevice);
    ma_result (* onDeviceStart   )(ma_device* pDevice);
    ma_result (* onDeviceStop    )(ma_device* pDevice);
    ma_result (* onDeviceMainLoop)(ma_device* pDevice);

    union
    {
#ifdef MA_SUPPORT_WASAPI
        struct
        {
            int _unused;
        } wasapi;
#endif
#ifdef MA_SUPPORT_DSOUND
        struct
        {
            ma_handle hDSoundDLL;
            ma_proc DirectSoundCreate;
            ma_proc DirectSoundEnumerateA;
            ma_proc DirectSoundCaptureCreate;
            ma_proc DirectSoundCaptureEnumerateA;
        } dsound;
#endif
#ifdef MA_SUPPORT_WINMM
        struct
        {
            ma_handle hWinMM;
            ma_proc waveOutGetNumDevs;
            ma_proc waveOutGetDevCapsA;
            ma_proc waveOutOpen;
            ma_proc waveOutClose;
            ma_proc waveOutPrepareHeader;
            ma_proc waveOutUnprepareHeader;
            ma_proc waveOutWrite;
            ma_proc waveOutReset;
            ma_proc waveInGetNumDevs;
            ma_proc waveInGetDevCapsA;
            ma_proc waveInOpen;
            ma_proc waveInClose;
            ma_proc waveInPrepareHeader;
            ma_proc waveInUnprepareHeader;
            ma_proc waveInAddBuffer;
            ma_proc waveInStart;
            ma_proc waveInReset;
        } winmm;
#endif
#ifdef MA_SUPPORT_ALSA
        struct
        {
            ma_handle asoundSO;
            ma_proc snd_pcm_open;
            ma_proc snd_pcm_close;
            ma_proc snd_pcm_hw_params_sizeof;
            ma_proc snd_pcm_hw_params_any;
            ma_proc snd_pcm_hw_params_set_format;
            ma_proc snd_pcm_hw_params_set_format_first;
            ma_proc snd_pcm_hw_params_get_format_mask;
            ma_proc snd_pcm_hw_params_set_channels_near;
            ma_proc snd_pcm_hw_params_set_rate_resample;
            ma_proc snd_pcm_hw_params_set_rate_near;
            ma_proc snd_pcm_hw_params_set_buffer_size_near;
            ma_proc snd_pcm_hw_params_set_periods_near;
            ma_proc snd_pcm_hw_params_set_access;
            ma_proc snd_pcm_hw_params_get_format;
            ma_proc snd_pcm_hw_params_get_channels;
            ma_proc snd_pcm_hw_params_get_channels_min;
            ma_proc snd_pcm_hw_params_get_channels_max;
            ma_proc snd_pcm_hw_params_get_rate;
            ma_proc snd_pcm_hw_params_get_rate_min;
            ma_proc snd_pcm_hw_params_get_rate_max;
            ma_proc snd_pcm_hw_params_get_buffer_size;
            ma_proc snd_pcm_hw_params_get_periods;
            ma_proc snd_pcm_hw_params_get_access;
            ma_proc snd_pcm_hw_params;
            ma_proc snd_pcm_sw_params_sizeof;
            ma_proc snd_pcm_sw_params_current;
            ma_proc snd_pcm_sw_params_get_boundary;
            ma_proc snd_pcm_sw_params_set_avail_min;
            ma_proc snd_pcm_sw_params_set_start_threshold;
            ma_proc snd_pcm_sw_params_set_stop_threshold;
            ma_proc snd_pcm_sw_params;
            ma_proc snd_pcm_format_mask_sizeof;
            ma_proc snd_pcm_format_mask_test;
            ma_proc snd_pcm_get_chmap;
            ma_proc snd_pcm_state;
            ma_proc snd_pcm_prepare;
            ma_proc snd_pcm_start;
            ma_proc snd_pcm_drop;
            ma_proc snd_pcm_drain;
            ma_proc snd_device_name_hint;
            ma_proc snd_device_name_get_hint;
            ma_proc snd_card_get_index;
            ma_proc snd_device_name_free_hint;
            ma_proc snd_pcm_mmap_begin;
            ma_proc snd_pcm_mmap_commit;
            ma_proc snd_pcm_recover;
            ma_proc snd_pcm_readi;
            ma_proc snd_pcm_writei;
            ma_proc snd_pcm_avail;
            ma_proc snd_pcm_avail_update;
            ma_proc snd_pcm_wait;
            ma_proc snd_pcm_info;
            ma_proc snd_pcm_info_sizeof;
            ma_proc snd_pcm_info_get_name;
            ma_proc snd_config_update_free_global;

            ma_mutex internalDeviceEnumLock;
            ma_bool32 useVerboseDeviceEnumeration;
        } alsa;
#endif
#ifdef MA_SUPPORT_PULSEAUDIO
        struct
        {
            ma_handle pulseSO;
            ma_proc pa_mainloop_new;
            ma_proc pa_mainloop_free;
            ma_proc pa_mainloop_get_api;
            ma_proc pa_mainloop_iterate;
            ma_proc pa_mainloop_wakeup;
            ma_proc pa_context_new;
            ma_proc pa_context_unref;
            ma_proc pa_context_connect;
            ma_proc pa_context_disconnect;
            ma_proc pa_context_set_state_callback;
            ma_proc pa_context_get_state;
            ma_proc pa_context_get_sink_info_list;
            ma_proc pa_context_get_source_info_list;
            ma_proc pa_context_get_sink_info_by_name;
            ma_proc pa_context_get_source_info_by_name;
            ma_proc pa_operation_unref;
            ma_proc pa_operation_get_state;
            ma_proc pa_channel_map_init_extend;
            ma_proc pa_channel_map_valid;
            ma_proc pa_channel_map_compatible;
            ma_proc pa_stream_new;
            ma_proc pa_stream_unref;
            ma_proc pa_stream_connect_playback;
            ma_proc pa_stream_connect_record;
            ma_proc pa_stream_disconnect;
            ma_proc pa_stream_get_state;
            ma_proc pa_stream_get_sample_spec;
            ma_proc pa_stream_get_channel_map;
            ma_proc pa_stream_get_buffer_attr;
            ma_proc pa_stream_set_buffer_attr;
            ma_proc pa_stream_get_device_name;
            ma_proc pa_stream_set_write_callback;
            ma_proc pa_stream_set_read_callback;
            ma_proc pa_stream_flush;
            ma_proc pa_stream_drain;
            ma_proc pa_stream_is_corked;
            ma_proc pa_stream_cork;
            ma_proc pa_stream_trigger;
            ma_proc pa_stream_begin_write;
            ma_proc pa_stream_write;
            ma_proc pa_stream_peek;
            ma_proc pa_stream_drop;
            ma_proc pa_stream_writable_size;
            ma_proc pa_stream_readable_size;

            char* pApplicationName;
            char* pServerName;
            ma_bool32 tryAutoSpawn;
        } pulse;
#endif
#ifdef MA_SUPPORT_JACK
        struct
        {
            ma_handle jackSO;
            ma_proc jack_client_open;
            ma_proc jack_client_close;
            ma_proc jack_client_name_size;
            ma_proc jack_set_process_callback;
            ma_proc jack_set_buffer_size_callback;
            ma_proc jack_on_shutdown;
            ma_proc jack_get_sample_rate;
            ma_proc jack_get_buffer_size;
            ma_proc jack_get_ports;
            ma_proc jack_activate;
            ma_proc jack_deactivate;
            ma_proc jack_connect;
            ma_proc jack_port_register;
            ma_proc jack_port_name;
            ma_proc jack_port_get_buffer;
            ma_proc jack_free;

            char* pClientName;
            ma_bool32 tryStartServer;
        } jack;
#endif
#ifdef MA_SUPPORT_COREAUDIO
        struct
        {
            ma_handle hCoreFoundation;
            ma_proc CFStringGetCString;
            ma_proc CFRelease;

            ma_handle hCoreAudio;
            ma_proc AudioObjectGetPropertyData;
            ma_proc AudioObjectGetPropertyDataSize;
            ma_proc AudioObjectSetPropertyData;
            ma_proc AudioObjectAddPropertyListener;
            ma_proc AudioObjectRemovePropertyListener;

            ma_handle hAudioUnit;  /* Could possibly be set to AudioToolbox on later versions of macOS. */
            ma_proc AudioComponentFindNext;
            ma_proc AudioComponentInstanceDispose;
            ma_proc AudioComponentInstanceNew;
            ma_proc AudioOutputUnitStart;
            ma_proc AudioOutputUnitStop;
            ma_proc AudioUnitAddPropertyListener;
            ma_proc AudioUnitGetPropertyInfo;
            ma_proc AudioUnitGetProperty;
            ma_proc AudioUnitSetProperty;
            ma_proc AudioUnitInitialize;
            ma_proc AudioUnitRender;

            /*AudioComponent*/ ma_ptr component;
        } coreaudio;
#endif
#ifdef MA_SUPPORT_SNDIO
        struct
        {
            ma_handle sndioSO;
            ma_proc sio_open;
            ma_proc sio_close;
            ma_proc sio_setpar;
            ma_proc sio_getpar;
            ma_proc sio_getcap;
            ma_proc sio_start;
            ma_proc sio_stop;
            ma_proc sio_read;
            ma_proc sio_write;
            ma_proc sio_onmove;
            ma_proc sio_nfds;
            ma_proc sio_pollfd;
            ma_proc sio_revents;
            ma_proc sio_eof;
            ma_proc sio_setvol;
            ma_proc sio_onvol;
            ma_proc sio_initpar;
        } sndio;
#endif
#ifdef MA_SUPPORT_AUDIO4
        struct
        {
            int _unused;
        } audio4;
#endif
#ifdef MA_SUPPORT_OSS
        struct
        {
            int versionMajor;
            int versionMinor;
        } oss;
#endif
#ifdef MA_SUPPORT_AAUDIO
        struct
        {
            ma_handle hAAudio; /* libaaudio.so */
            ma_proc AAudio_createStreamBuilder;
            ma_proc AAudioStreamBuilder_delete;
            ma_proc AAudioStreamBuilder_setDeviceId;
            ma_proc AAudioStreamBuilder_setDirection;
            ma_proc AAudioStreamBuilder_setSharingMode;
            ma_proc AAudioStreamBuilder_setFormat;
            ma_proc AAudioStreamBuilder_setChannelCount;
            ma_proc AAudioStreamBuilder_setSampleRate;
            ma_proc AAudioStreamBuilder_setBufferCapacityInFrames;
            ma_proc AAudioStreamBuilder_setFramesPerDataCallback;
            ma_proc AAudioStreamBuilder_setDataCallback;
            ma_proc AAudioStreamBuilder_setErrorCallback;
            ma_proc AAudioStreamBuilder_setPerformanceMode;
            ma_proc AAudioStreamBuilder_openStream;
            ma_proc AAudioStream_close;
            ma_proc AAudioStream_getState;
            ma_proc AAudioStream_waitForStateChange;
            ma_proc AAudioStream_getFormat;
            ma_proc AAudioStream_getChannelCount;
            ma_proc AAudioStream_getSampleRate;
            ma_proc AAudioStream_getBufferCapacityInFrames;
            ma_proc AAudioStream_getFramesPerDataCallback;
            ma_proc AAudioStream_getFramesPerBurst;
            ma_proc AAudioStream_requestStart;
            ma_proc AAudioStream_requestStop;
        } aaudio;
#endif
#ifdef MA_SUPPORT_OPENSL
        struct
        {
            int _unused;
        } opensl;
#endif
#ifdef MA_SUPPORT_WEBAUDIO
        struct
        {
            int _unused;
        } webaudio;
#endif
#ifdef MA_SUPPORT_NULL
        struct
        {
            int _unused;
        } null_backend;
#endif
    };

    union
    {
#ifdef MA_WIN32
        struct
        {
            /*HMODULE*/ ma_handle hOle32DLL;
            ma_proc CoInitializeEx;
            ma_proc CoUninitialize;
            ma_proc CoCreateInstance;
            ma_proc CoTaskMemFree;
            ma_proc PropVariantClear;
            ma_proc StringFromGUID2;

            /*HMODULE*/ ma_handle hUser32DLL;
            ma_proc GetForegroundWindow;
            ma_proc GetDesktopWindow;

            /*HMODULE*/ ma_handle hAdvapi32DLL;
            ma_proc RegOpenKeyExA;
            ma_proc RegCloseKey;
            ma_proc RegQueryValueExA;
        } win32;
#endif
#ifdef MA_POSIX
        struct
        {
            ma_handle pthreadSO;
            ma_proc pthread_create;
            ma_proc pthread_join;
            ma_proc pthread_mutex_init;
            ma_proc pthread_mutex_destroy;
            ma_proc pthread_mutex_lock;
            ma_proc pthread_mutex_unlock;
            ma_proc pthread_cond_init;
            ma_proc pthread_cond_destroy;
            ma_proc pthread_cond_wait;
            ma_proc pthread_cond_signal;
            ma_proc pthread_attr_init;
            ma_proc pthread_attr_destroy;
            ma_proc pthread_attr_setschedpolicy;
            ma_proc pthread_attr_getschedparam;
            ma_proc pthread_attr_setschedparam;
        } posix;
#endif
        int _unused;
    };
};

struct ma_device
{
    ma_context* pContext;
    ma_device_type type;
    ma_uint32 sampleRate;
    volatile ma_uint32 state;               /* The state of the device is variable and can change at any time on any thread, so tell the compiler as such with `volatile`. */
    ma_device_callback_proc onData;         /* Set once at initialization time and should not be changed after. */
    ma_stop_proc onStop;                    /* Set once at initialization time and should not be changed after. */
    void* pUserData;                        /* Application defined data. */
    ma_mutex lock;
    ma_event wakeupEvent;
    ma_event startEvent;
    ma_event stopEvent;
    ma_thread thread;
    ma_result workResult;                   /* This is set by the worker thread after it's finished doing a job. */
    ma_bool32 usingDefaultSampleRate  : 1;
    ma_bool32 usingDefaultBufferSize  : 1;
    ma_bool32 usingDefaultPeriods     : 1;
    ma_bool32 isOwnerOfContext        : 1;  /* When set to true, uninitializing the device will also uninitialize the context. Set to true when NULL is passed into ma_device_init(). */
    ma_bool32 noPreZeroedOutputBuffer : 1;
    ma_bool32 noClip                  : 1;
    volatile float masterVolumeFactor;      /* Volatile so we can use some thread safety when applying volume to periods. */
    struct
    {
        ma_resample_algorithm algorithm;
        struct
        {
            ma_uint32 lpfOrder;
        } linear;
        struct
        {
            int quality;
        } speex;
    } resampling;
    struct
    {
        char name[256];                     /* Maybe temporary. Likely to be replaced with a query API. */
        ma_share_mode shareMode;            /* Set to whatever was passed in when the device was initialized. */
        ma_bool32 usingDefaultFormat     : 1;
        ma_bool32 usingDefaultChannels   : 1;
        ma_bool32 usingDefaultChannelMap : 1;
        ma_format format;
        ma_uint32 channels;
        ma_channel channelMap[MA_MAX_CHANNELS];
        ma_format internalFormat;
        ma_uint32 internalChannels;
        ma_uint32 internalSampleRate;
        ma_channel internalChannelMap[MA_MAX_CHANNELS];
        ma_uint32 internalPeriodSizeInFrames;
        ma_uint32 internalPeriods;
        ma_data_converter converter;
    } playback;
    struct
    {
        char name[256];                     /* Maybe temporary. Likely to be replaced with a query API. */
        ma_share_mode shareMode;            /* Set to whatever was passed in when the device was initialized. */
        ma_bool32 usingDefaultFormat     : 1;
        ma_bool32 usingDefaultChannels   : 1;
        ma_bool32 usingDefaultChannelMap : 1;
        ma_format format;
        ma_uint32 channels;
        ma_channel channelMap[MA_MAX_CHANNELS];
        ma_format internalFormat;
        ma_uint32 internalChannels;
        ma_uint32 internalSampleRate;
        ma_channel internalChannelMap[MA_MAX_CHANNELS];
        ma_uint32 internalPeriodSizeInFrames;
        ma_uint32 internalPeriods;
        ma_data_converter converter;
    } capture;

    union
    {
#ifdef MA_SUPPORT_WASAPI
        struct
        {
            /*IAudioClient**/ ma_ptr pAudioClientPlayback;
            /*IAudioClient**/ ma_ptr pAudioClientCapture;
            /*IAudioRenderClient**/ ma_ptr pRenderClient;
            /*IAudioCaptureClient**/ ma_ptr pCaptureClient;
            /*IMMDeviceEnumerator**/ ma_ptr pDeviceEnumerator; /* Used for IMMNotificationClient notifications. Required for detecting default device changes. */
            ma_IMMNotificationClient notificationClient;
            /*HANDLE*/ ma_handle hEventPlayback;               /* Auto reset. Initialized to signaled. */
            /*HANDLE*/ ma_handle hEventCapture;                /* Auto reset. Initialized to unsignaled. */
            ma_uint32 actualPeriodSizeInFramesPlayback;        /* Value from GetBufferSize(). internalPeriodSizeInFrames is not set to the _actual_ buffer size when low-latency shared mode is being used due to the way the IAudioClient3 API works. */
            ma_uint32 actualPeriodSizeInFramesCapture;
            ma_uint32 originalPeriodSizeInFrames;
            ma_uint32 originalPeriodSizeInMilliseconds;
            ma_uint32 originalPeriods;
            ma_bool32 hasDefaultPlaybackDeviceChanged;         /* <-- Make sure this is always a whole 32-bits because we use atomic assignments. */
            ma_bool32 hasDefaultCaptureDeviceChanged;          /* <-- Make sure this is always a whole 32-bits because we use atomic assignments. */
            ma_uint32 periodSizeInFramesPlayback;
            ma_uint32 periodSizeInFramesCapture;
            ma_bool32 isStartedCapture;                        /* <-- Make sure this is always a whole 32-bits because we use atomic assignments. */
            ma_bool32 isStartedPlayback;                       /* <-- Make sure this is always a whole 32-bits because we use atomic assignments. */
            ma_bool32 noAutoConvertSRC               : 1;      /* When set to true, disables the use of AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM. */
            ma_bool32 noDefaultQualitySRC            : 1;      /* When set to true, disables the use of AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY. */
            ma_bool32 noHardwareOffloading           : 1;
            ma_bool32 allowCaptureAutoStreamRouting  : 1;
            ma_bool32 allowPlaybackAutoStreamRouting : 1;
        } wasapi;
#endif
#ifdef MA_SUPPORT_DSOUND
        struct
        {
            /*LPDIRECTSOUND*/ ma_ptr pPlayback;
            /*LPDIRECTSOUNDBUFFER*/ ma_ptr pPlaybackPrimaryBuffer;
            /*LPDIRECTSOUNDBUFFER*/ ma_ptr pPlaybackBuffer;
            /*LPDIRECTSOUNDCAPTURE*/ ma_ptr pCapture;
            /*LPDIRECTSOUNDCAPTUREBUFFER*/ ma_ptr pCaptureBuffer;
        } dsound;
#endif
#ifdef MA_SUPPORT_WINMM
        struct
        {
            /*HWAVEOUT*/ ma_handle hDevicePlayback;
            /*HWAVEIN*/ ma_handle hDeviceCapture;
            /*HANDLE*/ ma_handle hEventPlayback;
            /*HANDLE*/ ma_handle hEventCapture;
            ma_uint32 fragmentSizeInFrames;
            ma_uint32 iNextHeaderPlayback;             /* [0,periods). Used as an index into pWAVEHDRPlayback. */
            ma_uint32 iNextHeaderCapture;              /* [0,periods). Used as an index into pWAVEHDRCapture. */
            ma_uint32 headerFramesConsumedPlayback;    /* The number of PCM frames consumed in the buffer in pWAVEHEADER[iNextHeader]. */
            ma_uint32 headerFramesConsumedCapture;     /* ^^^ */
            /*WAVEHDR**/ ma_uint8* pWAVEHDRPlayback;   /* One instantiation for each period. */
            /*WAVEHDR**/ ma_uint8* pWAVEHDRCapture;    /* One instantiation for each period. */
            ma_uint8* pIntermediaryBufferPlayback;
            ma_uint8* pIntermediaryBufferCapture;
            ma_uint8* _pHeapData;                      /* Used internally and is used for the heap allocated data for the intermediary buffer and the WAVEHDR structures. */
        } winmm;
#endif
#ifdef MA_SUPPORT_ALSA
        struct
        {
            /*snd_pcm_t**/ ma_ptr pPCMPlayback;
            /*snd_pcm_t**/ ma_ptr pPCMCapture;
            ma_bool32 isUsingMMapPlayback : 1;
            ma_bool32 isUsingMMapCapture  : 1;
        } alsa;
#endif
#ifdef MA_SUPPORT_PULSEAUDIO
        struct
        {
            /*pa_mainloop**/ ma_ptr pMainLoop;
            /*pa_mainloop_api**/ ma_ptr pAPI;
            /*pa_context**/ ma_ptr pPulseContext;
            /*pa_stream**/ ma_ptr pStreamPlayback;
            /*pa_stream**/ ma_ptr pStreamCapture;
            /*pa_context_state*/ ma_uint32 pulseContextState;
            void* pMappedBufferPlayback;
            const void* pMappedBufferCapture;
            ma_uint32 mappedBufferFramesRemainingPlayback;
            ma_uint32 mappedBufferFramesRemainingCapture;
            ma_uint32 mappedBufferFramesCapacityPlayback;
            ma_uint32 mappedBufferFramesCapacityCapture;
            ma_bool32 breakFromMainLoop : 1;
        } pulse;
#endif
#ifdef MA_SUPPORT_JACK
        struct
        {
            /*jack_client_t**/ ma_ptr pClient;
            /*jack_port_t**/ ma_ptr pPortsPlayback[MA_MAX_CHANNELS];
            /*jack_port_t**/ ma_ptr pPortsCapture[MA_MAX_CHANNELS];
            float* pIntermediaryBufferPlayback; /* Typed as a float because JACK is always floating point. */
            float* pIntermediaryBufferCapture;
            ma_pcm_rb duplexRB;
        } jack;
#endif
#ifdef MA_SUPPORT_COREAUDIO
        struct
        {
            ma_uint32 deviceObjectIDPlayback;
            ma_uint32 deviceObjectIDCapture;
            /*AudioUnit*/ ma_ptr audioUnitPlayback;
            /*AudioUnit*/ ma_ptr audioUnitCapture;
            /*AudioBufferList**/ ma_ptr pAudioBufferList;  /* Only used for input devices. */
            ma_event stopEvent;
            ma_uint32 originalPeriodSizeInFrames;
            ma_uint32 originalPeriodSizeInMilliseconds;
            ma_uint32 originalPeriods;
            ma_bool32 isDefaultPlaybackDevice;
            ma_bool32 isDefaultCaptureDevice;
            ma_bool32 isSwitchingPlaybackDevice;   /* <-- Set to true when the default device has changed and miniaudio is in the process of switching. */
            ma_bool32 isSwitchingCaptureDevice;    /* <-- Set to true when the default device has changed and miniaudio is in the process of switching. */
            ma_pcm_rb duplexRB;
            void* pRouteChangeHandler;             /* Only used on mobile platforms. Obj-C object for handling route changes. */
        } coreaudio;
#endif
#ifdef MA_SUPPORT_SNDIO
        struct
        {
            ma_ptr handlePlayback;
            ma_ptr handleCapture;
            ma_bool32 isStartedPlayback;
            ma_bool32 isStartedCapture;
        } sndio;
#endif
#ifdef MA_SUPPORT_AUDIO4
        struct
        {
            int fdPlayback;
            int fdCapture;
        } audio4;
#endif
#ifdef MA_SUPPORT_OSS
        struct
        {
            int fdPlayback;
            int fdCapture;
        } oss;
#endif
#ifdef MA_SUPPORT_AAUDIO
        struct
        {
            /*AAudioStream**/ ma_ptr pStreamPlayback;
            /*AAudioStream**/ ma_ptr pStreamCapture;
            ma_pcm_rb duplexRB;
        } aaudio;
#endif
#ifdef MA_SUPPORT_OPENSL
        struct
        {
            /*SLObjectItf*/ ma_ptr pOutputMixObj;
            /*SLOutputMixItf*/ ma_ptr pOutputMix;
            /*SLObjectItf*/ ma_ptr pAudioPlayerObj;
            /*SLPlayItf*/ ma_ptr pAudioPlayer;
            /*SLObjectItf*/ ma_ptr pAudioRecorderObj;
            /*SLRecordItf*/ ma_ptr pAudioRecorder;
            /*SLAndroidSimpleBufferQueueItf*/ ma_ptr pBufferQueuePlayback;
            /*SLAndroidSimpleBufferQueueItf*/ ma_ptr pBufferQueueCapture;
            ma_bool32 isDrainingCapture;
            ma_bool32 isDrainingPlayback;
            ma_uint32 currentBufferIndexPlayback;
            ma_uint32 currentBufferIndexCapture;
            ma_uint8* pBufferPlayback;      /* This is malloc()'d and is used for storing audio data. Typed as ma_uint8 for easy offsetting. */
            ma_uint8* pBufferCapture;
            ma_pcm_rb duplexRB;
        } opensl;
#endif
#ifdef MA_SUPPORT_WEBAUDIO
        struct
        {
            int indexPlayback;              /* We use a factory on the JavaScript side to manage devices and use an index for JS/C interop. */
            int indexCapture;
            ma_pcm_rb duplexRB;             /* In external capture format. */
        } webaudio;
#endif
#ifdef MA_SUPPORT_NULL
        struct
        {
            ma_thread deviceThread;
            ma_event operationEvent;
            ma_event operationCompletionEvent;
            ma_uint32 operation;
            ma_result operationResult;
            ma_timer timer;
            double priorRunTime;
            ma_uint32 currentPeriodFramesRemainingPlayback;
            ma_uint32 currentPeriodFramesRemainingCapture;
            ma_uint64 lastProcessedFramePlayback;
            ma_uint32 lastProcessedFrameCapture;
            ma_bool32 isStarted;
        } null_device;
#endif
    };
};
#if defined(_MSC_VER) && !defined(__clang__)
    #pragma warning(pop)
#else
    #pragma GCC diagnostic pop  /* For ISO C99 doesn't support unnamed structs/unions [-Wpedantic] */
#endif

/*
Initializes a `ma_context_config` object.


Return Value
------------
A `ma_context_config` initialized to defaults.


Remarks
-------
You must always use this to initialize the default state of the `ma_context_config` object. Not using this will result in your program breaking when miniaudio
is updated and new members are added to `ma_context_config`. It also sets logical defaults.

You can override members of the returned object by changing it's members directly.


See Also
--------
ma_context_init()
*/
MA_API ma_context_config ma_context_config_init(void);

/*
Initializes a context.

The context is used for selecting and initializing an appropriate backend and to represent the backend at a more global level than that of an individual
device. There is one context to many devices, and a device is created from a context. A context is required to enumerate devices.


Parameters
----------
backends (in, optional)
    A list of backends to try initializing, in priority order. Can be NULL, in which case it uses default priority order.

backendCount (in, optional)
    The number of items in `backend`. Ignored if `backend` is NULL.

pConfig (in, optional)
    The context configuration.

pContext (in)
    A pointer to the context object being initialized.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Unsafe. Do not call this function across multiple threads as some backends read and write to global state.


Remarks
-------
When `backends` is NULL, the default priority order will be used. Below is a list of backends in priority order:

    |-------------|-----------------------|--------------------------------------------------------|
    | Name        | Enum Name             | Supported Operating Systems                            |
    |-------------|-----------------------|--------------------------------------------------------|
    | WASAPI      | ma_backend_wasapi     | Windows Vista+                                         |
    | DirectSound | ma_backend_dsound     | Windows XP+                                            |
    | WinMM       | ma_backend_winmm      | Windows XP+ (may work on older versions, but untested) |
    | Core Audio  | ma_backend_coreaudio  | macOS, iOS                                             |
    | ALSA        | ma_backend_alsa       | Linux                                                  |
    | PulseAudio  | ma_backend_pulseaudio | Cross Platform (disabled on Windows, BSD and Android)  |
    | JACK        | ma_backend_jack       | Cross Platform (disabled on BSD and Android)           |
    | sndio       | ma_backend_sndio      | OpenBSD                                                |
    | audio(4)    | ma_backend_audio4     | NetBSD, OpenBSD                                        |
    | OSS         | ma_backend_oss        | FreeBSD                                                |
    | AAudio      | ma_backend_aaudio     | Android 8+                                             |
    | OpenSL|ES   | ma_backend_opensl     | Android (API level 16+)                                |
    | Web Audio   | ma_backend_webaudio   | Web (via Emscripten)                                   |
    | Null        | ma_backend_null       | Cross Platform (not used on Web)                       |
    |-------------|-----------------------|--------------------------------------------------------|

The context can be configured via the `pConfig` argument. The config object is initialized with `ma_context_config_init()`. Individual configuration settings
can then be set directly on the structure. Below are the members of the `ma_context_config` object.

    logCallback
        Callback for handling log messages from miniaudio.

    threadPriority
        The desired priority to use for the audio thread. Allowable values include the following:

        |--------------------------------------|
        | Thread Priority                      |
        |--------------------------------------|
        | ma_thread_priority_idle              |
        | ma_thread_priority_lowest            |
        | ma_thread_priority_low               |
        | ma_thread_priority_normal            |
        | ma_thread_priority_high              |
        | ma_thread_priority_highest (default) |
        | ma_thread_priority_realtime          |
        | ma_thread_priority_default           |
        |--------------------------------------|

    pUserData
        A pointer to application-defined data. This can be accessed from the context object directly such as `context.pUserData`.

    allocationCallbacks
        Structure containing custom allocation callbacks. Leaving this at defaults will cause it to use MA_MALLOC, MA_REALLOC and MA_FREE. These allocation
        callbacks will be used for anything tied to the context, including devices.

    alsa.useVerboseDeviceEnumeration
        ALSA will typically enumerate many different devices which can be intrusive and not user-friendly. To combat this, miniaudio will enumerate only unique
        card/device pairs by default. The problem with this is that you lose a bit of flexibility and control. Setting alsa.useVerboseDeviceEnumeration makes
        it so the ALSA backend includes all devices. Defaults to false.

    pulse.pApplicationName
        PulseAudio only. The application name to use when initializing the PulseAudio context with `pa_context_new()`.

    pulse.pServerName
        PulseAudio only. The name of the server to connect to with `pa_context_connect()`.

    pulse.tryAutoSpawn
        PulseAudio only. Whether or not to try automatically starting the PulseAudio daemon. Defaults to false. If you set this to true, keep in mind that
        miniaudio uses a trial and error method to find the most appropriate backend, and this will result in the PulseAudio daemon starting which may be
        intrusive for the end user.

    coreaudio.sessionCategory
        iOS only. The session category to use for the shared AudioSession instance. Below is a list of allowable values and their Core Audio equivalents.

        |-----------------------------------------|-------------------------------------|
        | miniaudio Token                         | Core Audio Token                    |
        |-----------------------------------------|-------------------------------------|
        | ma_ios_session_category_ambient         | AVAudioSessionCategoryAmbient       |
        | ma_ios_session_category_solo_ambient    | AVAudioSessionCategorySoloAmbient   |
        | ma_ios_session_category_playback        | AVAudioSessionCategoryPlayback      |
        | ma_ios_session_category_record          | AVAudioSessionCategoryRecord        |
        | ma_ios_session_category_play_and_record | AVAudioSessionCategoryPlayAndRecord |
        | ma_ios_session_category_multi_route     | AVAudioSessionCategoryMultiRoute    |
        | ma_ios_session_category_none            | AVAudioSessionCategoryAmbient       |
        | ma_ios_session_category_default         | AVAudioSessionCategoryAmbient       |
        |-----------------------------------------|-------------------------------------|

    coreaudio.sessionCategoryOptions
        iOS only. Session category options to use with the shared AudioSession instance. Below is a list of allowable values and their Core Audio equivalents.

        |---------------------------------------------------------------------------|------------------------------------------------------------------|
        | miniaudio Token                                                           | Core Audio Token                                                 |
        |---------------------------------------------------------------------------|------------------------------------------------------------------|
        | ma_ios_session_category_option_mix_with_others                            | AVAudioSessionCategoryOptionMixWithOthers                        |
        | ma_ios_session_category_option_duck_others                                | AVAudioSessionCategoryOptionDuckOthers                           |
        | ma_ios_session_category_option_allow_bluetooth                            | AVAudioSessionCategoryOptionAllowBluetooth                       |
        | ma_ios_session_category_option_default_to_speaker                         | AVAudioSessionCategoryOptionDefaultToSpeaker                     |
        | ma_ios_session_category_option_interrupt_spoken_audio_and_mix_with_others | AVAudioSessionCategoryOptionInterruptSpokenAudioAndMixWithOthers |
        | ma_ios_session_category_option_allow_bluetooth_a2dp                       | AVAudioSessionCategoryOptionAllowBluetoothA2DP                   |
        | ma_ios_session_category_option_allow_air_play                             | AVAudioSessionCategoryOptionAllowAirPlay                         |
        |---------------------------------------------------------------------------|------------------------------------------------------------------|

    jack.pClientName
        The name of the client to pass to `jack_client_open()`.

    jack.tryStartServer
        Whether or not to try auto-starting the JACK server. Defaults to false.


It is recommended that only a single context is active at any given time because it's a bulky data structure which performs run-time linking for the
relevant backends every time it's initialized.

The location of the context cannot change throughout it's lifetime. Consider allocating the `ma_context` object with `malloc()` if this is an issue. The
reason for this is that a pointer to the context is stored in the `ma_device` structure.


Example 1 - Default Initialization
----------------------------------
The example below shows how to initialize the context using the default configuration.

```c
ma_context context;
ma_result result = ma_context_init(NULL, 0, NULL, &context);
if (result != MA_SUCCESS) {
    // Error.
}
```


Example 2 - Custom Configuration
--------------------------------
The example below shows how to initialize the context using custom backend priorities and a custom configuration. In this hypothetical example, the program
wants to prioritize ALSA over PulseAudio on Linux. They also want to avoid using the WinMM backend on Windows because it's latency is too high. They also
want an error to be returned if no valid backend is available which they achieve by excluding the Null backend.

For the configuration, the program wants to capture any log messages so they can, for example, route it to a log file and user interface.

```c
ma_backend backends[] = {
    ma_backend_alsa,
    ma_backend_pulseaudio,
    ma_backend_wasapi,
    ma_backend_dsound
};

ma_context_config config = ma_context_config_init();
config.logCallback = my_log_callback;
config.pUserData   = pMyUserData;

ma_context context;
ma_result result = ma_context_init(backends, sizeof(backends)/sizeof(backends[0]), &config, &context);
if (result != MA_SUCCESS) {
    // Error.
    if (result == MA_NO_BACKEND) {
        // Couldn't find an appropriate backend.
    }
}
```


See Also
--------
ma_context_config_init()
ma_context_uninit()
*/
MA_API ma_result ma_context_init(const ma_backend backends[], ma_uint32 backendCount, const ma_context_config* pConfig, ma_context* pContext);

/*
Uninitializes a context.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Unsafe. Do not call this function across multiple threads as some backends read and write to global state.


Remarks
-------
Results are undefined if you call this while any device created by this context is still active.


See Also
--------
ma_context_init()
*/
MA_API ma_result ma_context_uninit(ma_context* pContext);

/*
Retrieves the size of the ma_context object.

This is mainly for the purpose of bindings to know how much memory to allocate.
*/
MA_API size_t ma_context_sizeof(void);

/*
Enumerates over every device (both playback and capture).

This is a lower-level enumeration function to the easier to use `ma_context_get_devices()`. Use `ma_context_enumerate_devices()` if you would rather not incur
an internal heap allocation, or it simply suits your code better.

Note that this only retrieves the ID and name/description of the device. The reason for only retrieving basic information is that it would otherwise require
opening the backend device in order to probe it for more detailed information which can be inefficient. Consider using `ma_context_get_device_info()` for this,
but don't call it from within the enumeration callback.

Returning false from the callback will stop enumeration. Returning true will continue enumeration.


Parameters
----------
pContext (in)
    A pointer to the context performing the enumeration.

callback (in)
    The callback to fire for each enumerated device.

pUserData (in)
    A pointer to application-defined data passed to the callback.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Safe. This is guarded using a simple mutex lock.


Remarks
-------
Do _not_ assume the first enumerated device of a given type is the default device.

Some backends and platforms may only support default playback and capture devices.

In general, you should not do anything complicated from within the callback. In particular, do not try initializing a device from within the callback. Also,
do not try to call `ma_context_get_device_info()` from within the callback.

Consider using `ma_context_get_devices()` for a simpler and safer API, albeit at the expense of an internal heap allocation.


Example 1 - Simple Enumeration
------------------------------
ma_bool32 ma_device_enum_callback(ma_context* pContext, ma_device_type deviceType, const ma_device_info* pInfo, void* pUserData)
{
    printf("Device Name: %s\n", pInfo->name);
    return MA_TRUE;
}

ma_result result = ma_context_enumerate_devices(&context, my_device_enum_callback, pMyUserData);
if (result != MA_SUCCESS) {
    // Error.
}


See Also
--------
ma_context_get_devices()
*/
MA_API ma_result ma_context_enumerate_devices(ma_context* pContext, ma_enum_devices_callback_proc callback, void* pUserData);

/*
Retrieves basic information about every active playback and/or capture device.

This function will allocate memory internally for the device lists and return a pointer to them through the `ppPlaybackDeviceInfos` and `ppCaptureDeviceInfos`
parameters. If you do not want to incur the overhead of these allocations consider using `ma_context_enumerate_devices()` which will instead use a callback.


Parameters
----------
pContext (in)
    A pointer to the context performing the enumeration.

ppPlaybackDeviceInfos (out)
    A pointer to a pointer that will receive the address of a buffer containing the list of `ma_device_info` structures for playback devices.

pPlaybackDeviceCount (out)
    A pointer to an unsigned integer that will receive the number of playback devices.

ppCaptureDeviceInfos (out)
    A pointer to a pointer that will receive the address of a buffer containing the list of `ma_device_info` structures for capture devices.

pCaptureDeviceCount (out)
    A pointer to an unsigned integer that will receive the number of capture devices.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Unsafe. Since each call to this function invalidates the pointers from the previous call, you should not be calling this simultaneously across multiple
threads. Instead, you need to make a copy of the returned data with your own higher level synchronization.


Remarks
-------
It is _not_ safe to assume the first device in the list is the default device.

You can pass in NULL for the playback or capture lists in which case they'll be ignored.

The returned pointers will become invalid upon the next call this this function, or when the context is uninitialized. Do not free the returned pointers.


See Also
--------
ma_context_get_devices()
*/
MA_API ma_result ma_context_get_devices(ma_context* pContext, ma_device_info** ppPlaybackDeviceInfos, ma_uint32* pPlaybackDeviceCount, ma_device_info** ppCaptureDeviceInfos, ma_uint32* pCaptureDeviceCount);

/*
Retrieves information about a device of the given type, with the specified ID and share mode.


Parameters
----------
pContext (in)
    A pointer to the context performing the query.

deviceType (in)
    The type of the device being queried. Must be either `ma_device_type_playback` or `ma_device_type_capture`.

pDeviceID (in)
    The ID of the device being queried.

shareMode (in)
    The share mode to query for device capabilities. This should be set to whatever you're intending on using when initializing the device. If you're unsure,
    set this to `ma_share_mode_shared`.

pDeviceInfo (out)
    A pointer to the `ma_device_info` structure that will receive the device information.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Safe. This is guarded using a simple mutex lock.


Remarks
-------
Do _not_ call this from within the `ma_context_enumerate_devices()` callback.

It's possible for a device to have different information and capabilities depending on whether or not it's opened in shared or exclusive mode. For example, in
shared mode, WASAPI always uses floating point samples for mixing, but in exclusive mode it can be anything. Therefore, this function allows you to specify
which share mode you want information for. Note that not all backends and devices support shared or exclusive mode, in which case this function will fail if
the requested share mode is unsupported.

This leaves pDeviceInfo unmodified in the result of an error.
*/
MA_API ma_result ma_context_get_device_info(ma_context* pContext, ma_device_type deviceType, const ma_device_id* pDeviceID, ma_share_mode shareMode, ma_device_info* pDeviceInfo);

/*
Determines if the given context supports loopback mode.


Parameters
----------
pContext (in)
    A pointer to the context getting queried.


Return Value
------------
MA_TRUE if the context supports loopback mode; MA_FALSE otherwise.
*/
MA_API ma_bool32 ma_context_is_loopback_supported(ma_context* pContext);



/*
Initializes a device config with default settings.


Parameters
----------
deviceType (in)
    The type of the device this config is being initialized for. This must set to one of the following:

    |-------------------------|
    | Device Type             |
    |-------------------------|
    | ma_device_type_playback |
    | ma_device_type_capture  |
    | ma_device_type_duplex   |
    | ma_device_type_loopback |
    |-------------------------|


Return Value
------------
A new device config object with default settings. You will typically want to adjust the config after this function returns. See remarks.


Thread Safety
-------------
Safe.


Callback Safety
---------------
Safe, but don't try initializing a device in a callback.


Remarks
-------
The returned config will be initialized to defaults. You will normally want to customize a few variables before initializing the device. See Example 1 for a
typical configuration which sets the sample format, channel count, sample rate, data callback and user data. These are usually things you will want to change
before initializing the device.

See `ma_device_init()` for details on specific configuration options.


Example 1 - Simple Configuration
--------------------------------
The example below is what a program will typically want to configure for each device at a minimum. Notice how `ma_device_config_init()` is called first, and
then the returned object is modified directly. This is important because it ensures that your program continues to work as new configuration options are added
to the `ma_device_config` structure.

```c
ma_device_config config = ma_device_config_init(ma_device_type_playback);
config.playback.format   = ma_format_f32;
config.playback.channels = 2;
config.sampleRate        = 48000;
config.dataCallback      = ma_data_callback;
config.pUserData         = pMyUserData;
```


See Also
--------
ma_device_init()
ma_device_init_ex()
*/
MA_API ma_device_config ma_device_config_init(ma_device_type deviceType);


/*
Initializes a device.

A device represents a physical audio device. The idea is you send or receive audio data from the device to either play it back through a speaker, or capture it
from a microphone. Whether or not you should send or receive data from the device (or both) depends on the type of device you are initializing which can be
playback, capture, full-duplex or loopback. (Note that loopback mode is only supported on select backends.) Sending and receiving audio data to and from the
device is done via a callback which is fired by miniaudio at periodic time intervals.

The frequency at which data is delivered to and from a device depends on the size of it's period. The size of the period can be defined in terms of PCM frames
or milliseconds, whichever is more convenient. Generally speaking, the smaller the period, the lower the latency at the expense of higher CPU usage and
increased risk of glitching due to the more frequent and granular data deliver intervals. The size of a period will depend on your requirements, but
miniaudio's defaults should work fine for most scenarios. If you're building a game you should leave this fairly small, whereas if you're building a simple
media player you can make it larger. Note that the period size you request is actually just a hint - miniaudio will tell the backend what you want, but the
backend is ultimately responsible for what it gives you. You cannot assume you will get exactly what you ask for.

When delivering data to and from a device you need to make sure it's in the correct format which you can set through the device configuration. You just set the
format that you want to use and miniaudio will perform all of the necessary conversion for you internally. When delivering data to and from the callback you
can assume the format is the same as what you requested when you initialized the device. See Remarks for more details on miniaudio's data conversion pipeline.


Parameters
----------
pContext (in, optional)
    A pointer to the context that owns the device. This can be null, in which case it creates a default context internally.

pConfig (in)
    A pointer to the device configuration. Cannot be null. See remarks for details.

pDevice (out)
    A pointer to the device object being initialized.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Unsafe. It is not safe to call this function simultaneously for different devices because some backends depend on and mutate global state. The same applies to
calling this at the same time as `ma_device_uninit()`.


Callback Safety
---------------
Unsafe. It is not safe to call this inside any callback.


Remarks
-------
Setting `pContext` to NULL will result in miniaudio creating a default context internally and is equivalent to passing in a context initialized like so:

    ```c
    ma_context_init(NULL, 0, NULL, &context);
    ```

Do not set `pContext` to NULL if you are needing to open multiple devices. You can, however, use NULL when initializing the first device, and then use
device.pContext for the initialization of other devices.

The device can be configured via the `pConfig` argument. The config object is initialized with `ma_device_config_init()`. Individual configuration settings can
then be set directly on the structure. Below are the members of the `ma_device_config` object.

    deviceType
        Must be `ma_device_type_playback`, `ma_device_type_capture`, `ma_device_type_duplex` of `ma_device_type_loopback`.

    sampleRate
        The sample rate, in hertz. The most common sample rates are 48000 and 44100. Setting this to 0 will use the device's native sample rate.

    periodSizeInFrames
        The desired size of a period in PCM frames. If this is 0, `periodSizeInMilliseconds` will be used instead. If both are 0 the default buffer size will
        be used depending on the selected performance profile. This value affects latency. See below for details.

    periodSizeInMilliseconds
        The desired size of a period in milliseconds. If this is 0, `periodSizeInFrames` will be used instead. If both are 0 the default buffer size will be
        used depending on the selected performance profile. The value affects latency. See below for details.

    periods
        The number of periods making up the device's entire buffer. The total buffer size is `periodSizeInFrames` or `periodSizeInMilliseconds` multiplied by
        this value. This is just a hint as backends will be the ones who ultimately decide how your periods will be configured.

    performanceProfile
        A hint to miniaudio as to the performance requirements of your program. Can be either `ma_performance_profile_low_latency` (default) or
        `ma_performance_profile_conservative`. This mainly affects the size of default buffers and can usually be left at it's default value.

    noPreZeroedOutputBuffer
        When set to true, the contents of the output buffer passed into the data callback will be left undefined. When set to false (default), the contents of
        the output buffer will be cleared the zero. You can use this to avoid the overhead of zeroing out the buffer if you can guarantee that your data
        callback will write to every sample in the output buffer, or if you are doing your own clearing.

    noClip
        When set to true, the contents of the output buffer passed into the data callback will be clipped after returning. When set to false (default), the
        contents of the output buffer are left alone after returning and it will be left up to the backend itself to decide whether or not the clip. This only
        applies when the playback sample format is f32.

    dataCallback
        The callback to fire whenever data is ready to be delivered to or from the device.

    stopCallback
        The callback to fire whenever the device has stopped, either explicitly via `ma_device_stop()`, or implicitly due to things like the device being
        disconnected.

    pUserData
        The user data pointer to use with the device. You can access this directly from the device object like `device.pUserData`.

    resampling.algorithm
        The resampling algorithm to use when miniaudio needs to perform resampling between the rate specified by `sampleRate` and the device's native rate. The
        default value is `ma_resample_algorithm_linear`, and the quality can be configured with `resampling.linear.lpfOrder`.

    resampling.linear.lpfOrder
        The linear resampler applies a low-pass filter as part of it's procesing for anti-aliasing. This setting controls the order of the filter. The higher
        the value, the better the quality, in general. Setting this to 0 will disable low-pass filtering altogether. The maximum value is
        `MA_MAX_FILTER_ORDER`. The default value is `min(4, MA_MAX_FILTER_ORDER)`.

    playback.pDeviceID
        A pointer to a `ma_device_id` structure containing the ID of the playback device to initialize. Setting this NULL (default) will use the system's
        default playback device. Retrieve the device ID from the `ma_device_info` structure, which can be retrieved using device enumeration.

    playback.format
        The sample format to use for playback. When set to `ma_format_unknown` the device's native format will be used. This can be retrieved after
        initialization from the device object directly with `device.playback.format`.

    playback.channels
        The number of channels to use for playback. When set to 0 the device's native channel count will be used. This can be retrieved after initialization
        from the device object directly with `device.playback.channels`.

    playback.channelMap
        The channel map to use for playback. When left empty, the device's native channel map will be used. This can be retrieved after initialization from the
        device object direct with `device.playback.channelMap`.

    playback.shareMode
        The preferred share mode to use for playback. Can be either `ma_share_mode_shared` (default) or `ma_share_mode_exclusive`. Note that if you specify
        exclusive mode, but it's not supported by the backend, initialization will fail. You can then fall back to shared mode if desired by changing this to
        ma_share_mode_shared and reinitializing.

    capture.pDeviceID
        A pointer to a `ma_device_id` structure containing the ID of the capture device to initialize. Setting this NULL (default) will use the system's
        default capture device. Retrieve the device ID from the `ma_device_info` structure, which can be retrieved using device enumeration.

    capture.format
        The sample format to use for capture. When set to `ma_format_unknown` the device's native format will be used. This can be retrieved after
        initialization from the device object directly with `device.capture.format`.

    capture.channels
        The number of channels to use for capture. When set to 0 the device's native channel count will be used. This can be retrieved after initialization
        from the device object directly with `device.capture.channels`.

    capture.channelMap
        The channel map to use for capture. When left empty, the device's native channel map will be used. This can be retrieved after initialization from the
        device object direct with `device.capture.channelMap`.

    capture.shareMode
        The preferred share mode to use for capture. Can be either `ma_share_mode_shared` (default) or `ma_share_mode_exclusive`. Note that if you specify
        exclusive mode, but it's not supported by the backend, initialization will fail. You can then fall back to shared mode if desired by changing this to
        ma_share_mode_shared and reinitializing.

    wasapi.noAutoConvertSRC
        WASAPI only. When set to true, disables WASAPI's automatic resampling and forces the use of miniaudio's resampler. Defaults to false.

    wasapi.noDefaultQualitySRC
        WASAPI only. Only used when `wasapi.noAutoConvertSRC` is set to false. When set to true, disables the use of `AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY`.
        You should usually leave this set to false, which is the default.

    wasapi.noAutoStreamRouting
        WASAPI only. When set to true, disables automatic stream routing on the WASAPI backend. Defaults to false.

    wasapi.noHardwareOffloading
        WASAPI only. When set to true, disables the use of WASAPI's hardware offloading feature. Defaults to false.

    alsa.noMMap
        ALSA only. When set to true, disables MMap mode. Defaults to false.

    alsa.noAutoFormat
        ALSA only. When set to true, disables ALSA's automatic format conversion by including the SND_PCM_NO_AUTO_FORMAT flag. Defaults to false.

    alsa.noAutoChannels
        ALSA only. When set to true, disables ALSA's automatic channel conversion by including the SND_PCM_NO_AUTO_CHANNELS flag. Defaults to false.

    alsa.noAutoResample
        ALSA only. When set to true, disables ALSA's automatic resampling by including the SND_PCM_NO_AUTO_RESAMPLE flag. Defaults to false.

    pulse.pStreamNamePlayback
        PulseAudio only. Sets the stream name for playback.

    pulse.pStreamNameCapture
        PulseAudio only. Sets the stream name for capture.


Once initialized, the device's config is immutable. If you need to change the config you will need to initialize a new device.

After initializing the device it will be in a stopped state. To start it, use `ma_device_start()`.

If both `periodSizeInFrames` and `periodSizeInMilliseconds` are set to zero, it will default to `MA_DEFAULT_PERIOD_SIZE_IN_MILLISECONDS_LOW_LATENCY` or
`MA_DEFAULT_PERIOD_SIZE_IN_MILLISECONDS_CONSERVATIVE`, depending on whether or not `performanceProfile` is set to `ma_performance_profile_low_latency` or
`ma_performance_profile_conservative`.

If you request exclusive mode and the backend does not support it an error will be returned. For robustness, you may want to first try initializing the device
in exclusive mode, and then fall back to shared mode if required. Alternatively you can just request shared mode (the default if you leave it unset in the
config) which is the most reliable option. Some backends do not have a practical way of choosing whether or not the device should be exclusive or not (ALSA,
for example) in which case it just acts as a hint. Unless you have special requirements you should try avoiding exclusive mode as it's intrusive to the user.
Starting with Windows 10, miniaudio will use low-latency shared mode where possible which may make exclusive mode unnecessary.

When sending or receiving data to/from a device, miniaudio will internally perform a format conversion to convert between the format specified by the config
and the format used internally by the backend. If you pass in 0 for the sample format, channel count, sample rate _and_ channel map, data transmission will run
on an optimized pass-through fast path. You can retrieve the format, channel count and sample rate by inspecting the `playback/capture.format`,
`playback/capture.channels` and `sampleRate` members of the device object.

When compiling for UWP you must ensure you call this function on the main UI thread because the operating system may need to present the user with a message
asking for permissions. Please refer to the official documentation for ActivateAudioInterfaceAsync() for more information.

ALSA Specific: When initializing the default device, requesting shared mode will try using the "dmix" device for playback and the "dsnoop" device for capture.
If these fail it will try falling back to the "hw" device.


Example 1 - Simple Initialization
---------------------------------
This example shows how to initialize a simple playback device using a standard configuration. If you are just needing to do simple playback from the default
playback device this is usually all you need.

```c
ma_device_config config = ma_device_config_init(ma_device_type_playback);
config.playback.format   = ma_format_f32;
config.playback.channels = 2;
config.sampleRate        = 48000;
config.dataCallback      = ma_data_callback;
config.pMyUserData       = pMyUserData;

ma_device device;
ma_result result = ma_device_init(NULL, &config, &device);
if (result != MA_SUCCESS) {
    // Error
}
```


Example 2 - Advanced Initialization
-----------------------------------
This example shows how you might do some more advanced initialization. In this hypothetical example we want to control the latency by setting the buffer size
and period count. We also want to allow the user to be able to choose which device to output from which means we need a context so we can perform device
enumeration.

```c
ma_context context;
ma_result result = ma_context_init(NULL, 0, NULL, &context);
if (result != MA_SUCCESS) {
    // Error
}

ma_device_info* pPlaybackDeviceInfos;
ma_uint32 playbackDeviceCount;
result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, NULL, NULL);
if (result != MA_SUCCESS) {
    // Error
}

// ... choose a device from pPlaybackDeviceInfos ...

ma_device_config config = ma_device_config_init(ma_device_type_playback);
config.playback.pDeviceID       = pMyChosenDeviceID;    // <-- Get this from the `id` member of one of the `ma_device_info` objects returned by ma_context_get_devices().
config.playback.format          = ma_format_f32;
config.playback.channels        = 2;
config.sampleRate               = 48000;
config.dataCallback             = ma_data_callback;
config.pUserData                = pMyUserData;
config.periodSizeInMilliseconds = 10;
config.periods                  = 3;

ma_device device;
result = ma_device_init(&context, &config, &device);
if (result != MA_SUCCESS) {
    // Error
}
```


See Also
--------
ma_device_config_init()
ma_device_uninit()
ma_device_start()
ma_context_init()
ma_context_get_devices()
ma_context_enumerate_devices()
*/
MA_API ma_result ma_device_init(ma_context* pContext, const ma_device_config* pConfig, ma_device* pDevice);

/*
Initializes a device without a context, with extra parameters for controlling the configuration of the internal self-managed context.

This is the same as `ma_device_init()`, only instead of a context being passed in, the parameters from `ma_context_init()` are passed in instead. This function
allows you to configure the internally created context.


Parameters
----------
backends (in, optional)
    A list of backends to try initializing, in priority order. Can be NULL, in which case it uses default priority order.

backendCount (in, optional)
    The number of items in `backend`. Ignored if `backend` is NULL.

pContextConfig (in, optional)
    The context configuration.

pConfig (in)
    A pointer to the device configuration. Cannot be null. See remarks for details.

pDevice (out)
    A pointer to the device object being initialized.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Unsafe. It is not safe to call this function simultaneously for different devices because some backends depend on and mutate global state. The same applies to
calling this at the same time as `ma_device_uninit()`.


Callback Safety
---------------
Unsafe. It is not safe to call this inside any callback.


Remarks
-------
You only need to use this function if you want to configure the context differently to it's defaults. You should never use this function if you want to manage
your own context.

See the documentation for `ma_context_init()` for information on the different context configuration options.


See Also
--------
ma_device_init()
ma_device_uninit()
ma_device_config_init()
ma_context_init()
*/
MA_API ma_result ma_device_init_ex(const ma_backend backends[], ma_uint32 backendCount, const ma_context_config* pContextConfig, const ma_device_config* pConfig, ma_device* pDevice);

/*
Uninitializes a device.

This will explicitly stop the device. You do not need to call `ma_device_stop()` beforehand, but it's harmless if you do.


Parameters
----------
pDevice (in)
    A pointer to the device to stop.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Unsafe. As soon as this API is called the device should be considered undefined.


Callback Safety
---------------
Unsafe. It is not safe to call this inside any callback. Doing this will result in a deadlock.


See Also
--------
ma_device_init()
ma_device_stop()
*/
MA_API void ma_device_uninit(ma_device* pDevice);

/*
Starts the device. For playback devices this begins playback. For capture devices it begins recording.

Use `ma_device_stop()` to stop the device.


Parameters
----------
pDevice (in)
    A pointer to the device to start.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Safe. It's safe to call this from any thread with the exception of the callback thread.


Callback Safety
---------------
Unsafe. It is not safe to call this inside any callback.


Remarks
-------
For a playback device, this will retrieve an initial chunk of audio data from the client before returning. The reason for this is to ensure there is valid
audio data in the buffer, which needs to be done before the device begins playback.

This API waits until the backend device has been started for real by the worker thread. It also waits on a mutex for thread-safety.

Do not call this in any callback.


See Also
--------
ma_device_stop()
*/
MA_API ma_result ma_device_start(ma_device* pDevice);

/*
Stops the device. For playback devices this stops playback. For capture devices it stops recording.

Use `ma_device_start()` to start the device again.


Parameters
----------
pDevice (in)
    A pointer to the device to stop.


Return Value
------------
MA_SUCCESS if successful; any other error code otherwise.


Thread Safety
-------------
Safe. It's safe to call this from any thread with the exception of the callback thread.


Callback Safety
---------------
Unsafe. It is not safe to call this inside any callback. Doing this will result in a deadlock.


Remarks
-------
This API needs to wait on the worker thread to stop the backend device properly before returning. It also waits on a mutex for thread-safety. In addition, some
backends need to wait for the device to finish playback/recording of the current fragment which can take some time (usually proportionate to the buffer size
that was specified at initialization time).

Backends are required to either pause the stream in-place or drain the buffer if pausing is not possible. The reason for this is that stopping the device and
the resuming it with ma_device_start() (which you might do when your program loses focus) may result in a situation where those samples are never output to the
speakers or received from the microphone which can in turn result in de-syncs.

Do not call this in any callback.

This will be called implicitly by `ma_device_uninit()`.


See Also
--------
ma_device_start()
*/
MA_API ma_result ma_device_stop(ma_device* pDevice);

/*
Determines whether or not the device is started.


Parameters
----------
pDevice (in)
    A pointer to the device whose start state is being retrieved.


Return Value
------------
True if the device is started, false otherwise.


Thread Safety
-------------
Safe. If another thread calls `ma_device_start()` or `ma_device_stop()` at this same time as this function is called, there's a very small chance the return
value will be out of sync.


Callback Safety
---------------
Safe. This is implemented as a simple accessor.


See Also
--------
ma_device_start()
ma_device_stop()
*/
MA_API ma_bool32 ma_device_is_started(ma_device* pDevice);

/*
Sets the master volume factor for the device.

The volume factor must be between 0 (silence) and 1 (full volume). Use `ma_device_set_master_gain_db()` to use decibel notation, where 0 is full volume and
values less than 0 decreases the volume.


Parameters
----------
pDevice (in)
    A pointer to the device whose volume is being set.

volume (in)
    The new volume factor. Must be within the range of [0, 1].


Return Value
------------
MA_SUCCESS if the volume was set successfully.
MA_INVALID_ARGS if pDevice is NULL.
MA_INVALID_ARGS if the volume factor is not within the range of [0, 1].


Thread Safety
-------------
Safe. This just sets a local member of the device object.


Callback Safety
---------------
Safe. If you set the volume in the data callback, that data written to the output buffer will have the new volume applied.


Remarks
-------
This applies the volume factor across all channels.

This does not change the operating system's volume. It only affects the volume for the given `ma_device` object's audio stream.


See Also
--------
ma_device_get_master_volume()
ma_device_set_master_volume_gain_db()
ma_device_get_master_volume_gain_db()
*/
MA_API ma_result ma_device_set_master_volume(ma_device* pDevice, float volume);

/*
Retrieves the master volume factor for the device.


Parameters
----------
pDevice (in)
    A pointer to the device whose volume factor is being retrieved.

pVolume (in)
    A pointer to the variable that will receive the volume factor. The returned value will be in the range of [0, 1].


Return Value
------------
MA_SUCCESS if successful.
MA_INVALID_ARGS if pDevice is NULL.
MA_INVALID_ARGS if pVolume is NULL.


Thread Safety
-------------
Safe. This just a simple member retrieval.


Callback Safety
---------------
Safe.


Remarks
-------
If an error occurs, `*pVolume` will be set to 0.


See Also
--------
ma_device_set_master_volume()
ma_device_set_master_volume_gain_db()
ma_device_get_master_volume_gain_db()
*/
MA_API ma_result ma_device_get_master_volume(ma_device* pDevice, float* pVolume);

/*
Sets the master volume for the device as gain in decibels.

A gain of 0 is full volume, whereas a gain of < 0 will decrease the volume.


Parameters
----------
pDevice (in)
    A pointer to the device whose gain is being set.

gainDB (in)
    The new volume as gain in decibels. Must be less than or equal to 0, where 0 is full volume and anything less than 0 decreases the volume.


Return Value
------------
MA_SUCCESS if the volume was set successfully.
MA_INVALID_ARGS if pDevice is NULL.
MA_INVALID_ARGS if the gain is > 0.


Thread Safety
-------------
Safe. This just sets a local member of the device object.


Callback Safety
---------------
Safe. If you set the volume in the data callback, that data written to the output buffer will have the new volume applied.


Remarks
-------
This applies the gain across all channels.

This does not change the operating system's volume. It only affects the volume for the given `ma_device` object's audio stream.


See Also
--------
ma_device_get_master_volume_gain_db()
ma_device_set_master_volume()
ma_device_get_master_volume()
*/
MA_API ma_result ma_device_set_master_gain_db(ma_device* pDevice, float gainDB);

/*
Retrieves the master gain in decibels.


Parameters
----------
pDevice (in)
    A pointer to the device whose gain is being retrieved.

pGainDB (in)
    A pointer to the variable that will receive the gain in decibels. The returned value will be <= 0.


Return Value
------------
MA_SUCCESS if successful.
MA_INVALID_ARGS if pDevice is NULL.
MA_INVALID_ARGS if pGainDB is NULL.


Thread Safety
-------------
Safe. This just a simple member retrieval.


Callback Safety
---------------
Safe.


Remarks
-------
If an error occurs, `*pGainDB` will be set to 0.


See Also
--------
ma_device_set_master_volume_gain_db()
ma_device_set_master_volume()
ma_device_get_master_volume()
*/
MA_API ma_result ma_device_get_master_gain_db(ma_device* pDevice, float* pGainDB);


/*
Retrieves a friendly name for a backend.
*/
MA_API const char* ma_get_backend_name(ma_backend backend);

/*
Determines whether or not loopback mode is support by a backend.
*/
MA_API ma_bool32 ma_is_loopback_supported(ma_backend backend);

#endif  /* MA_NO_DEVICE_IO */


#ifndef MA_NO_THREADING

/*
Locks a spinlock.
*/
MA_API ma_result ma_spinlock_lock(ma_spinlock* pSpinlock);

/*
Locks a spinlock, but does not yield() when looping.
*/
MA_API ma_result ma_spinlock_lock_noyield(ma_spinlock* pSpinlock);

/*
Unlocks a spinlock.
*/
MA_API ma_result ma_spinlock_unlock(ma_spinlock* pSpinlock);


/*
Creates a mutex.

A mutex must be created from a valid context. A mutex is initially unlocked.
*/
MA_API ma_result ma_mutex_init(ma_mutex* pMutex);

/*
Deletes a mutex.
*/
MA_API void ma_mutex_uninit(ma_mutex* pMutex);

/*
Locks a mutex with an infinite timeout.
*/
MA_API void ma_mutex_lock(ma_mutex* pMutex);

/*
Unlocks a mutex.
*/
MA_API void ma_mutex_unlock(ma_mutex* pMutex);


/*
Initializes an auto-reset event.
*/
MA_API ma_result ma_event_init(ma_event* pEvent);

/*
Uninitializes an auto-reset event.
*/
MA_API void ma_event_uninit(ma_event* pEvent);

/*
Waits for the specified auto-reset event to become signalled.
*/
MA_API ma_result ma_event_wait(ma_event* pEvent);

/*
Signals the specified auto-reset event.
*/
MA_API ma_result ma_event_signal(ma_event* pEvent);
#endif  /* MA_NO_THREADING */


/************************************************************************************************************************************************************

Utiltities

************************************************************************************************************************************************************/

/*
Adjust buffer size based on a scaling factor.

This just multiplies the base size by the scaling factor, making sure it's a size of at least 1.
*/
MA_API ma_uint32 ma_scale_buffer_size(ma_uint32 baseBufferSize, float scale);

/*
Calculates a buffer size in milliseconds from the specified number of frames and sample rate.
*/
MA_API ma_uint32 ma_calculate_buffer_size_in_milliseconds_from_frames(ma_uint32 bufferSizeInFrames, ma_uint32 sampleRate);

/*
Calculates a buffer size in frames from the specified number of milliseconds and sample rate.
*/
MA_API ma_uint32 ma_calculate_buffer_size_in_frames_from_milliseconds(ma_uint32 bufferSizeInMilliseconds, ma_uint32 sampleRate);

/*
Copies PCM frames from one buffer to another.
*/
MA_API void ma_copy_pcm_frames(void* dst, const void* src, ma_uint64 frameCount, ma_format format, ma_uint32 channels);

/*
Copies silent frames into the given buffer.

Remarks
-------
For all formats except `ma_format_u8`, the output buffer will be filled with 0. For `ma_format_u8` it will be filled with 128. The reason for this is that it
makes more sense for the purpose of mixing to initialize it to the center point.
*/
MA_API void ma_silence_pcm_frames(void* p, ma_uint64 frameCount, ma_format format, ma_uint32 channels);
static MA_INLINE void ma_zero_pcm_frames(void* p, ma_uint64 frameCount, ma_format format, ma_uint32 channels) { ma_silence_pcm_frames(p, frameCount, format, channels); }


/*
Offsets a pointer by the specified number of PCM frames.
*/
MA_API void* ma_offset_pcm_frames_ptr(void* p, ma_uint64 offsetInFrames, ma_format format, ma_uint32 channels);
MA_API const void* ma_offset_pcm_frames_const_ptr(const void* p, ma_uint64 offsetInFrames, ma_format format, ma_uint32 channels);


/*
Clips f32 samples.
*/
MA_API void ma_clip_samples_f32(float* p, ma_uint64 sampleCount);
static MA_INLINE void ma_clip_pcm_frames_f32(float* p, ma_uint64 frameCount, ma_uint32 channels) { ma_clip_samples_f32(p, frameCount*channels); }

/*
Helper for applying a volume factor to samples.

Note that the source and destination buffers can be the same, in which case it'll perform the operation in-place.
*/
MA_API void ma_copy_and_apply_volume_factor_u8(ma_uint8* pSamplesOut, const ma_uint8* pSamplesIn, ma_uint32 sampleCount, float factor);
MA_API void ma_copy_and_apply_volume_factor_s16(ma_int16* pSamplesOut, const ma_int16* pSamplesIn, ma_uint32 sampleCount, float factor);
MA_API void ma_copy_and_apply_volume_factor_s24(void* pSamplesOut, const void* pSamplesIn, ma_uint32 sampleCount, float factor);
MA_API void ma_copy_and_apply_volume_factor_s32(ma_int32* pSamplesOut, const ma_int32* pSamplesIn, ma_uint32 sampleCount, float factor);
MA_API void ma_copy_and_apply_volume_factor_f32(float* pSamplesOut, const float* pSamplesIn, ma_uint32 sampleCount, float factor);

MA_API void ma_apply_volume_factor_u8(ma_uint8* pSamples, ma_uint32 sampleCount, float factor);
MA_API void ma_apply_volume_factor_s16(ma_int16* pSamples, ma_uint32 sampleCount, float factor);
MA_API void ma_apply_volume_factor_s24(void* pSamples, ma_uint32 sampleCount, float factor);
MA_API void ma_apply_volume_factor_s32(ma_int32* pSamples, ma_uint32 sampleCount, float factor);
MA_API void ma_apply_volume_factor_f32(float* pSamples, ma_uint32 sampleCount, float factor);

MA_API void ma_copy_and_apply_volume_factor_pcm_frames_u8(ma_uint8* pPCMFramesOut, const ma_uint8* pPCMFramesIn, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_copy_and_apply_volume_factor_pcm_frames_s16(ma_int16* pPCMFramesOut, const ma_int16* pPCMFramesIn, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_copy_and_apply_volume_factor_pcm_frames_s24(void* pPCMFramesOut, const void* pPCMFramesIn, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_copy_and_apply_volume_factor_pcm_frames_s32(ma_int32* pPCMFramesOut, const ma_int32* pPCMFramesIn, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_copy_and_apply_volume_factor_pcm_frames_f32(float* pPCMFramesOut, const float* pPCMFramesIn, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_copy_and_apply_volume_factor_pcm_frames(void* pFramesOut, const void* pFramesIn, ma_uint32 frameCount, ma_format format, ma_uint32 channels, float factor);

MA_API void ma_apply_volume_factor_pcm_frames_u8(ma_uint8* pFrames, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_apply_volume_factor_pcm_frames_s16(ma_int16* pFrames, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_apply_volume_factor_pcm_frames_s24(void* pFrames, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_apply_volume_factor_pcm_frames_s32(ma_int32* pFrames, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_apply_volume_factor_pcm_frames_f32(float* pFrames, ma_uint32 frameCount, ma_uint32 channels, float factor);
MA_API void ma_apply_volume_factor_pcm_frames(void* pFrames, ma_uint32 frameCount, ma_format format, ma_uint32 channels, float factor);


/*
Helper for converting a linear factor to gain in decibels.
*/
MA_API float ma_factor_to_gain_db(float factor);

/*
Helper for converting gain in decibels to a linear factor.
*/
MA_API float ma_gain_db_to_factor(float gain);


typedef void ma_data_source;

typedef struct
{
    ma_result (* onRead)(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead);
    ma_result (* onSeek)(ma_data_source* pDataSource, ma_uint64 frameIndex);
    ma_result (* onMap)(ma_data_source* pDataSource, void** ppFramesOut, ma_uint64* pFrameCount);   /* Returns MA_AT_END if the end has been reached. This should be considered successful. */
    ma_result (* onUnmap)(ma_data_source* pDataSource, ma_uint64 frameCount);
    ma_result (* onGetDataFormat)(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels);
} ma_data_source_callbacks;

MA_API ma_result ma_data_source_read_pcm_frames(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead, ma_bool32 loop);   /* Must support pFramesOut = NULL in which case a forward seek should be performed. */
MA_API ma_result ma_data_source_seek_pcm_frames(ma_data_source* pDataSource, ma_uint64 frameCount, ma_uint64* pFramesSeeked, ma_bool32 loop); /* Can only seek forward. Equivalent to ma_data_source_read_pcm_frames(pDataSource, NULL, frameCount); */
MA_API ma_result ma_data_source_seek_to_pcm_frame(ma_data_source* pDataSource, ma_uint64 frameIndex);
MA_API ma_result ma_data_source_map(ma_data_source* pDataSource, void** ppFramesOut, ma_uint64* pFrameCount);
MA_API ma_result ma_data_source_unmap(ma_data_source* pDataSource, ma_uint64 frameCount);   /* Returns MA_AT_END if the end has been reached. This should be considered successful. */
MA_API ma_result ma_data_source_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels);


typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint64 sizeInFrames;
    const void* pData;  /* If set to NULL, will allocate a block of memory for you. */
    ma_allocation_callbacks allocationCallbacks;
} ma_audio_buffer_config;

MA_API ma_audio_buffer_config ma_audio_buffer_config_init(ma_format format, ma_uint32 channels, ma_uint64 sizeInFrames, const void* pData, const ma_allocation_callbacks* pAllocationCallbacks);

typedef struct
{
    ma_data_source_callbacks ds;
    ma_format format;
    ma_uint32 channels;
    ma_uint64 cursor;
    ma_uint64 sizeInFrames;
    const void* pData;
    ma_allocation_callbacks allocationCallbacks;
    ma_bool32 ownsData;             /* Used to control whether or not miniaudio owns the data buffer. If set to true, pData will be freed in ma_audio_buffer_uninit(). */
    ma_uint8 _pExtraData[1];        /* For allocating a buffer with the memory located directly after the other memory of the structure. */
} ma_audio_buffer;

MA_API ma_result ma_audio_buffer_init(const ma_audio_buffer_config* pConfig, ma_audio_buffer* pAudioBuffer);
MA_API ma_result ma_audio_buffer_init_copy(const ma_audio_buffer_config* pConfig, ma_audio_buffer* pAudioBuffer);
MA_API ma_result ma_audio_buffer_alloc_and_init(const ma_audio_buffer_config* pConfig, ma_audio_buffer** ppAudioBuffer);  /* Always copies the data. Doesn't make sense to use this otherwise. Use ma_audio_buffer_uninit_and_free() to uninit. */
MA_API void ma_audio_buffer_uninit(ma_audio_buffer* pAudioBuffer);
MA_API void ma_audio_buffer_uninit_and_free(ma_audio_buffer* pAudioBuffer);
MA_API ma_uint64 ma_audio_buffer_read_pcm_frames(ma_audio_buffer* pAudioBuffer, void* pFramesOut, ma_uint64 frameCount, ma_bool32 loop);
MA_API ma_result ma_audio_buffer_seek_to_pcm_frame(ma_audio_buffer* pAudioBuffer, ma_uint64 frameIndex);
MA_API ma_result ma_audio_buffer_map(ma_audio_buffer* pAudioBuffer, void** ppFramesOut, ma_uint64* pFrameCount);
MA_API ma_result ma_audio_buffer_unmap(ma_audio_buffer* pAudioBuffer, ma_uint64 frameCount);    /* Returns MA_AT_END if the end has been reached. This should be considered successful. */
MA_API ma_result ma_audio_buffer_at_end(ma_audio_buffer* pAudioBuffer);




/************************************************************************************************************************************************************

VFS
===

The VFS object (virtual file system) is what's used to customize file access. This is useful in cases where stdio FILE* based APIs may not be entirely
appropriate for a given situation.

************************************************************************************************************************************************************/
typedef void      ma_vfs;
typedef ma_handle ma_vfs_file;

#define MA_OPEN_MODE_READ   0x00000001
#define MA_OPEN_MODE_WRITE  0x00000002

typedef enum
{
    ma_seek_origin_start,
    ma_seek_origin_current,
    ma_seek_origin_end  /* Not used by decoders. */
} ma_seek_origin;

typedef struct
{
    ma_uint64 sizeInBytes;
} ma_file_info;

typedef struct
{
    ma_result (* onOpen) (ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile);
    ma_result (* onOpenW)(ma_vfs* pVFS, const wchar_t* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile);
    ma_result (* onClose)(ma_vfs* pVFS, ma_vfs_file file);
    ma_result (* onRead) (ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead);
    ma_result (* onWrite)(ma_vfs* pVFS, ma_vfs_file file, const void* pSrc, size_t sizeInBytes, size_t* pBytesWritten);
    ma_result (* onSeek) (ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin);
    ma_result (* onTell) (ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor);
    ma_result (* onInfo) (ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo);
} ma_vfs_callbacks;

MA_API ma_result ma_vfs_open(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile);
MA_API ma_result ma_vfs_open_w(ma_vfs* pVFS, const wchar_t* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile);
MA_API ma_result ma_vfs_close(ma_vfs* pVFS, ma_vfs_file file);
MA_API ma_result ma_vfs_read(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead);
MA_API ma_result ma_vfs_write(ma_vfs* pVFS, ma_vfs_file file, const void* pSrc, size_t sizeInBytes, size_t* pBytesWritten);
MA_API ma_result ma_vfs_seek(ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin);
MA_API ma_result ma_vfs_tell(ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor);
MA_API ma_result ma_vfs_info(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo);
MA_API ma_result ma_vfs_open_and_read_file(ma_vfs* pVFS, const char* pFilePath, void** ppData, size_t* pSize, const ma_allocation_callbacks* pAllocationCallbacks);

typedef struct
{
    ma_vfs_callbacks cb;
    ma_allocation_callbacks allocationCallbacks;    /* Only used for the wchar_t version of open() on non-Windows platforms. */
} ma_default_vfs;

MA_API ma_result ma_default_vfs_init(ma_default_vfs* pVFS, const ma_allocation_callbacks* pAllocationCallbacks);




#if !defined(MA_NO_DECODING) || !defined(MA_NO_ENCODING)
typedef enum
{
    ma_resource_format_wav
} ma_resource_format;
#endif

/************************************************************************************************************************************************************

Decoding
========

Decoders are independent of the main device API. Decoding APIs can be called freely inside the device's data callback, but they are not thread safe unless
you do your own synchronization.

************************************************************************************************************************************************************/
#ifndef MA_NO_DECODING
typedef struct ma_decoder ma_decoder;

typedef size_t    (* ma_decoder_read_proc)                    (ma_decoder* pDecoder, void* pBufferOut, size_t bytesToRead);     /* Returns the number of bytes read. */
typedef ma_bool32 (* ma_decoder_seek_proc)                    (ma_decoder* pDecoder, int byteOffset, ma_seek_origin origin);    /* Origin will never be ma_seek_origin_end. */
typedef ma_uint64 (* ma_decoder_read_pcm_frames_proc)         (ma_decoder* pDecoder, void* pFramesOut, ma_uint64 frameCount);   /* Returns the number of frames read. Output data is in internal format. */
typedef ma_result (* ma_decoder_seek_to_pcm_frame_proc)       (ma_decoder* pDecoder, ma_uint64 frameIndex);
typedef ma_result (* ma_decoder_uninit_proc)                  (ma_decoder* pDecoder);
typedef ma_uint64 (* ma_decoder_get_length_in_pcm_frames_proc)(ma_decoder* pDecoder);

typedef struct
{
    ma_format format;      /* Set to 0 or ma_format_unknown to use the stream's internal format. */
    ma_uint32 channels;    /* Set to 0 to use the stream's internal channels. */
    ma_uint32 sampleRate;  /* Set to 0 to use the stream's internal sample rate. */
    ma_channel channelMap[MA_MAX_CHANNELS];
    ma_channel_mix_mode channelMixMode;
    ma_dither_mode ditherMode;
    struct
    {
        ma_resample_algorithm algorithm;
        struct
        {
            ma_uint32 lpfOrder;
        } linear;
        struct
        {
            int quality;
        } speex;
    } resampling;
    ma_allocation_callbacks allocationCallbacks;
} ma_decoder_config;

struct ma_decoder
{
    ma_data_source_callbacks ds;
    ma_decoder_read_proc onRead;
    ma_decoder_seek_proc onSeek;
    void* pUserData;
    ma_uint64  readPointer; /* Used for returning back to a previous position after analysing the stream or whatnot. */
    ma_format  internalFormat;
    ma_uint32  internalChannels;
    ma_uint32  internalSampleRate;
    ma_channel internalChannelMap[MA_MAX_CHANNELS];
    ma_format  outputFormat;
    ma_uint32  outputChannels;
    ma_uint32  outputSampleRate;
    ma_channel outputChannelMap[MA_MAX_CHANNELS];
    ma_data_converter converter;   /* <-- Data conversion is achieved by running frames through this. */
    ma_allocation_callbacks allocationCallbacks;
    ma_decoder_read_pcm_frames_proc onReadPCMFrames;
    ma_decoder_seek_to_pcm_frame_proc onSeekToPCMFrame;
    ma_decoder_uninit_proc onUninit;
    ma_decoder_get_length_in_pcm_frames_proc onGetLengthInPCMFrames;
    void* pInternalDecoder; /* <-- The drwav/drflac/stb_vorbis/etc. objects. */
    union
    {
        struct
        {
            ma_vfs* pVFS;
            ma_vfs_file file;
        } vfs;
        struct
        {
            const ma_uint8* pData;
            size_t dataSize;
            size_t currentReadPos;
        } memory;               /* Only used for decoders that were opened against a block of memory. */
    } backend;
};

MA_API ma_decoder_config ma_decoder_config_init(ma_format outputFormat, ma_uint32 outputChannels, ma_uint32 outputSampleRate);

MA_API ma_result ma_decoder_init(ma_decoder_read_proc onRead, ma_decoder_seek_proc onSeek, void* pUserData, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_wav(ma_decoder_read_proc onRead, ma_decoder_seek_proc onSeek, void* pUserData, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_flac(ma_decoder_read_proc onRead, ma_decoder_seek_proc onSeek, void* pUserData, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_mp3(ma_decoder_read_proc onRead, ma_decoder_seek_proc onSeek, void* pUserData, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vorbis(ma_decoder_read_proc onRead, ma_decoder_seek_proc onSeek, void* pUserData, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_raw(ma_decoder_read_proc onRead, ma_decoder_seek_proc onSeek, void* pUserData, const ma_decoder_config* pConfigIn, const ma_decoder_config* pConfigOut, ma_decoder* pDecoder);

MA_API ma_result ma_decoder_init_memory(const void* pData, size_t dataSize, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_memory_wav(const void* pData, size_t dataSize, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_memory_flac(const void* pData, size_t dataSize, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_memory_mp3(const void* pData, size_t dataSize, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_memory_vorbis(const void* pData, size_t dataSize, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_memory_raw(const void* pData, size_t dataSize, const ma_decoder_config* pConfigIn, const ma_decoder_config* pConfigOut, ma_decoder* pDecoder);

MA_API ma_result ma_decoder_init_vfs(ma_vfs* pVFS, const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_wav(ma_vfs* pVFS, const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_flac(ma_vfs* pVFS, const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_mp3(ma_vfs* pVFS, const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_vorbis(ma_vfs* pVFS, const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);

MA_API ma_result ma_decoder_init_vfs_w(ma_vfs* pVFS, const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_wav_w(ma_vfs* pVFS, const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_flac_w(ma_vfs* pVFS, const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_mp3_w(ma_vfs* pVFS, const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_vfs_vorbis_w(ma_vfs* pVFS, const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);

MA_API ma_result ma_decoder_init_file(const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_wav(const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_flac(const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_mp3(const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_vorbis(const char* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);

MA_API ma_result ma_decoder_init_file_w(const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_wav_w(const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_flac_w(const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_mp3_w(const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);
MA_API ma_result ma_decoder_init_file_vorbis_w(const wchar_t* pFilePath, const ma_decoder_config* pConfig, ma_decoder* pDecoder);

MA_API ma_result ma_decoder_uninit(ma_decoder* pDecoder);

/*
Retrieves the length of the decoder in PCM frames.

Do not call this on streams of an undefined length, such as internet radio.

If the length is unknown or an error occurs, 0 will be returned.

This will always return 0 for Vorbis decoders. This is due to a limitation with stb_vorbis in push mode which is what miniaudio
uses internally.

For MP3's, this will decode the entire file. Do not call this in time critical scenarios.

This function is not thread safe without your own synchronization.
*/
MA_API ma_uint64 ma_decoder_get_length_in_pcm_frames(ma_decoder* pDecoder);

/*
Reads PCM frames from the given decoder.

This is not thread safe without your own synchronization.
*/
MA_API ma_uint64 ma_decoder_read_pcm_frames(ma_decoder* pDecoder, void* pFramesOut, ma_uint64 frameCount);

/*
Seeks to a PCM frame based on it's absolute index.

This is not thread safe without your own synchronization.
*/
MA_API ma_result ma_decoder_seek_to_pcm_frame(ma_decoder* pDecoder, ma_uint64 frameIndex);

/*
Helper for opening and decoding a file into a heap allocated block of memory. Free the returned pointer with ma_free(). On input,
pConfig should be set to what you want. On output it will be set to what you got.
*/
MA_API ma_result ma_decode_from_vfs(ma_vfs* pVFS, const char* pFilePath, ma_decoder_config* pConfig, ma_uint64* pFrameCountOut, void** ppPCMFramesOut);
MA_API ma_result ma_decode_file(const char* pFilePath, ma_decoder_config* pConfig, ma_uint64* pFrameCountOut, void** ppPCMFramesOut);
MA_API ma_result ma_decode_memory(const void* pData, size_t dataSize, ma_decoder_config* pConfig, ma_uint64* pFrameCountOut, void** ppPCMFramesOut);

#endif  /* MA_NO_DECODING */


/************************************************************************************************************************************************************

Encoding
========

Encoders do not perform any format conversion for you. If your target format does not support the format, and error will be returned.

************************************************************************************************************************************************************/
#ifndef MA_NO_ENCODING
typedef struct ma_encoder ma_encoder;

typedef size_t    (* ma_encoder_write_proc)           (ma_encoder* pEncoder, const void* pBufferIn, size_t bytesToWrite);     /* Returns the number of bytes written. */
typedef ma_bool32 (* ma_encoder_seek_proc)            (ma_encoder* pEncoder, int byteOffset, ma_seek_origin origin);
typedef ma_result (* ma_encoder_init_proc)            (ma_encoder* pEncoder);
typedef void      (* ma_encoder_uninit_proc)          (ma_encoder* pEncoder);
typedef ma_uint64 (* ma_encoder_write_pcm_frames_proc)(ma_encoder* pEncoder, const void* pFramesIn, ma_uint64 frameCount);

typedef struct
{
    ma_resource_format resourceFormat;
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_allocation_callbacks allocationCallbacks;
} ma_encoder_config;

MA_API ma_encoder_config ma_encoder_config_init(ma_resource_format resourceFormat, ma_format format, ma_uint32 channels, ma_uint32 sampleRate);

struct ma_encoder
{
    ma_encoder_config config;
    ma_encoder_write_proc onWrite;
    ma_encoder_seek_proc onSeek;
    ma_encoder_init_proc onInit;
    ma_encoder_uninit_proc onUninit;
    ma_encoder_write_pcm_frames_proc onWritePCMFrames;
    void* pUserData;
    void* pInternalEncoder; /* <-- The drwav/drflac/stb_vorbis/etc. objects. */
    void* pFile;    /* FILE*. Only used when initialized with ma_encoder_init_file(). */
};

MA_API ma_result ma_encoder_init(ma_encoder_write_proc onWrite, ma_encoder_seek_proc onSeek, void* pUserData, const ma_encoder_config* pConfig, ma_encoder* pEncoder);
MA_API ma_result ma_encoder_init_file(const char* pFilePath, const ma_encoder_config* pConfig, ma_encoder* pEncoder);
MA_API ma_result ma_encoder_init_file_w(const wchar_t* pFilePath, const ma_encoder_config* pConfig, ma_encoder* pEncoder);
MA_API void ma_encoder_uninit(ma_encoder* pEncoder);
MA_API ma_uint64 ma_encoder_write_pcm_frames(ma_encoder* pEncoder, const void* pFramesIn, ma_uint64 frameCount);

#endif /* MA_NO_ENCODING */


/************************************************************************************************************************************************************

Generation

************************************************************************************************************************************************************/
#ifndef MA_NO_GENERATION
typedef enum
{
    ma_waveform_type_sine,
    ma_waveform_type_square,
    ma_waveform_type_triangle,
    ma_waveform_type_sawtooth
} ma_waveform_type;

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_waveform_type type;
    double amplitude;
    double frequency;
} ma_waveform_config;

MA_API ma_waveform_config ma_waveform_config_init(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, ma_waveform_type type, double amplitude, double frequency);

typedef struct
{
    ma_data_source_callbacks ds;
    ma_waveform_config config;
    double advance;
    double time;
} ma_waveform;

MA_API ma_result ma_waveform_init(const ma_waveform_config* pConfig, ma_waveform* pWaveform);
MA_API ma_uint64 ma_waveform_read_pcm_frames(ma_waveform* pWaveform, void* pFramesOut, ma_uint64 frameCount);
MA_API ma_result ma_waveform_seek_to_pcm_frame(ma_waveform* pWaveform, ma_uint64 frameIndex);
MA_API ma_result ma_waveform_set_amplitude(ma_waveform* pWaveform, double amplitude);
MA_API ma_result ma_waveform_set_frequency(ma_waveform* pWaveform, double frequency);
MA_API ma_result ma_waveform_set_sample_rate(ma_waveform* pWaveform, ma_uint32 sampleRate);



typedef enum
{
    ma_noise_type_white,
    ma_noise_type_pink,
    ma_noise_type_brownian
} ma_noise_type;

typedef struct
{
    ma_format format;
    ma_uint32 channels;
    ma_noise_type type;
    ma_int32 seed;
    double amplitude;
    ma_bool32 duplicateChannels;
} ma_noise_config;

MA_API ma_noise_config ma_noise_config_init(ma_format format, ma_uint32 channels, ma_noise_type type, ma_int32 seed, double amplitude);

typedef struct
{
    ma_data_source_callbacks ds;
    ma_noise_config config;
    ma_lcg lcg;
    union
    {
        struct
        {
            double bin[MA_MAX_CHANNELS][16];
            double accumulation[MA_MAX_CHANNELS];
            ma_uint32 counter[MA_MAX_CHANNELS];
        } pink;
        struct
        {
            double accumulation[MA_MAX_CHANNELS];
        } brownian;
    } state;
} ma_noise;

MA_API ma_result ma_noise_init(const ma_noise_config* pConfig, ma_noise* pNoise);
MA_API ma_uint64 ma_noise_read_pcm_frames(ma_noise* pNoise, void* pFramesOut, ma_uint64 frameCount);

#endif  /* MA_NO_GENERATION */

#ifdef __cplusplus
}
#endif
#endif  /* miniaudio_h */
#pragma endregion
#pragma endregion

#pragma region enums, constants, structs

/*
 This buffer size is defined by number of samples, independent of sample size and channels number
 After some math, considering a sampleRate of 48000, a buffer refill rate of 1/60 seconds and a
 standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
 In case of music-stalls, just increase this number.
*/
#define RF_DEFAULT_AUDIO_BUFFER_SIZE     (4096)
#define RF_DEFAULT_AUDIO_DEVICE_FORMAT   ma_format_f32
#define RF_DEFAULT_DEVICE_SAMPLE_RATE    (44100)
#define RF_DEFAULT_AUDIO_DEVICE_CHANNELS (RF_STEREO)

typedef enum rf_audio_format
{
    RF_AUDIO_FORMAT_UNKNOWN = 0,
    RF_AUDIO_FORMAT_WAV,
    RF_AUDIO_FORMAT_OGG,
    RF_AUDIO_FORMAT_FLAC,
    RF_AUDIO_FORMAT_MP3,
    RF_AUDIO_FORMAT_XM,
    RF_AUDIO_FORMAT_MOD,
} rf_audio_format;

typedef enum rf_audio_channels
{
    RF_MONO   = 1,
    RF_STEREO = 2,
} rf_audio_channels;

typedef struct rf_audio_source_base
{
    ma_uint64 (*decode) (struct rf_audio_source_base* this_source, void* frames_dst, ma_uint64 frames_count);
    ma_result (*seek)   (struct rf_audio_source_base* this_source, ma_uint64 target_frame);
} rf_audio_source_base;

typedef struct rf_audio_source
{
    rf_audio_source_base base;

    rf_audio_format format;
    const void* source;
    int source_size;

    rf_audio_channels channels;
    int sample_rate; // Frequency (samples per second)
    int sample_size; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    int size_in_frames;
    int frame_cursor_pos;
    int total_frames_processed;
    bool valid;
} rf_audio_source;

#pragma endregion

#pragma region audio loading
int rf_decoded_wav_size(const void* wav_file_contents, int wav_file_contents_size);
int rf_decoded_flac_size(const void* flac_file_contents, int flac_file_contents_size);

rf_audio_source rf_static_audio_from_memory(const void* file, rf_allocator allocator);
rf_audio_source rf_static_audio_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#endif // !defined(RAYFORK_NO_AUDIO)

#pragma endregion

#endif // #ifndef RAYFORK_H