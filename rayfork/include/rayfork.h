#ifndef RAYFORK_H
#define RAYFORK_H

#include "stddef.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

//region macros and constants

#ifdef __glad_h_
#error Macros defined in glad will overwrite opengl proc names in rayforh.h. Include rayfork.h before glad.h to fix this issue.
#endif

// If no graphics backend was set, choose OpenGL ES3
#if !defined(RAYFORK_GRAPHICS_BACKEND_GL_33) && !defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) && !defined(RAYFORK_GRAPHICS_BACKEND_METAL) && !defined(RAYFORK_GRAPHICS_BACKEND_GL_DIRECTX)
    #define RAYFORK_GRAPHICS_BACKEND_GL_33
#endif

// Check to make sure only one graphics backend was selected
#if (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) && (defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_GL_DIRECTX))) || \
    (defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) && (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_GL_DIRECTX))) || \
    (defined(RAYFORK_GRAPHICS_BACKEND_METAL) && (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_GL_DIRECTX))) || \
    (defined(RAYFORK_GRAPHICS_BACKEND_GL_DIRECTX) && (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL)))
    #error You can only set one graphics backend but 2 were detected.
#endif

// Prefix for all the public functions
#ifndef RF_API
    #ifdef __cplusplus
        #define RF_API extern "C"
    #else
        #define RF_API extern
    #endif
#endif

// Used to make constant literals work even in C++ mode
#ifdef __cplusplus
    #define RF_LIT(type) type
#else
    #define RF_LIT(type) (type)
#endif

#ifndef RF_NO_THREAD_LOCALS
    #if __cplusplus >= 201103L
        #define RF_THREAD_LOCAL thread_local
    #elif __STDC_VERSION_ >= 201112L
        #define RF_THREAD_LOCAL _Thread_local
    #elif defined(__GNUC__)
        #define RF_THREAD_LOCAL __thread
    #elif defined(_MSC_VER)
        #define RF_THREAD_LOCAL __declspec(thread)
    #endif
#endif

#ifndef RF_LOG_IMPL
    #define RF_LOG_IMPL(type, msg)
#endif

#ifndef RF_LOG_IMPL_V
    #define RF_LOG_IMPL_V(type, msg, ...)
#endif

#define RF_LOG(type, msg) { RF_LOG_IMPL(type, "[file: " __FILE__ "][line " __LINE__ "][proc: " __FUNCTION__ "]" msg) }
#define RF_LOG_V(type, msg, ...) { RF_LOG_IMPL_V(type, "[file: " __FILE__ "][line " __LINE__ "][proc: " __FUNCTION__ "]" msg, __VA_ARGS__) }
#define RF_LOG_ERROR(error_type, msg) { RF_LOG(RF_LOG_TYPE_ERROR, "[ERROR]" msg) }
#define RF_LOG_ERROR_V(error_type, msg, ...) { RF_LOG_V(RF_LOG_TYPE_ERROR, "[ERROR]" msg, __VA_ARGS__) }

#define RF_NULL_ALLOCATOR    (RF_LIT(rf_allocator) { 0 })
#define RF_DEFAULT_ALLOCATOR (RF_LIT(rf_allocator) { NULL, rf_malloc_wrapper })

#define RF_ALLOC(allocator, amount) ((allocator).request(RF_AM_ALLOC, (amount), NULL, (allocator).user_data))
#define RF_FREE(allocator, pointer) ((allocator).request(RF_AM_FREE, 0, (pointer), (allocator).user_data))

#define RF_UNLOCKED_FPS (0)

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on RF_WHITE background
#define RF_LIGHTGRAY  (RF_LIT(rf_color) { 200, 200, 200, 255 }) // Light Gray
#define RF_GRAY       (RF_LIT(rf_color) { 130, 130, 130, 255 }) // Gray
#define RF_DARKGRAY   (RF_LIT(rf_color) {  80,  80,  80, 255 }) // Dark Gray
#define RF_YELLOW     (RF_LIT(rf_color) { 253, 249,   0, 255 }) // Yellow
#define RF_GOLD       (RF_LIT(rf_color) { 255, 203,   0, 255 }) // Gold
#define RF_ORANGE     (RF_LIT(rf_color) { 255, 161,   0, 255 }) // Orange
#define RF_PINK       (RF_LIT(rf_color) { 255, 109, 194, 255 }) // Pink
#define RF_RED        (RF_LIT(rf_color) { 230,  41,  55, 255 }) // Red
#define RF_MAROON     (RF_LIT(rf_color) { 190,  33,  55, 255 }) // Maroon
#define RF_GREEN      (RF_LIT(rf_color) {   0, 228,  48, 255 }) // Green
#define RF_LIME       (RF_LIT(rf_color) {   0, 158,  47, 255 }) // Lime
#define RF_DARKGREEN  (RF_LIT(rf_color) {   0, 117,  44, 255 }) // Dark Green
#define RF_SKYBLUE    (RF_LIT(rf_color) { 102, 191, 255, 255 }) // Sky Blue
#define RF_BLUE       (RF_LIT(rf_color) {   0, 121, 241, 255 }) // Blue
#define RF_DARKBLUE   (RF_LIT(rf_color) {   0,  82, 172, 255 }) // Dark Blue
#define RF_PURPLE     (RF_LIT(rf_color) { 200, 122, 255, 255 }) // Purple
#define RF_VIOLET     (RF_LIT(rf_color) { 135,  60, 190, 255 }) // Violet
#define RF_DARKPURPLE (RF_LIT(rf_color) { 112,  31, 126, 255 }) // Dark Purple
#define RF_BEIGE      (RF_LIT(rf_color) { 211, 176, 131, 255 }) // Beige
#define RF_BROWN      (RF_LIT(rf_color) { 127, 106,  79, 255 }) // Brown
#define RF_DARKBROWN  (RF_LIT(rf_color) {  76,  63,  47, 255 }) // Dark Brown

#define RF_WHITE      (RF_LIT(rf_color) { 255, 255, 255, 255 }) // White
#define RF_BLACK      (RF_LIT(rf_color) {   0,   0,   0, 255 }) // Black
#define RF_BLANK      (RF_LIT(rf_color) {   0,   0,   0,   0 }) // Blank (Transparent)
#define RF_MAGENTA    (RF_LIT(rf_color) { 255,   0, 255, 255 }) // Magenta
#define RF_RAYWHITE   (RF_LIT(rf_color) { 245, 245, 245, 255 }) // My own White (raylib logo)

#ifndef RF_MAX_BATCH_ELEMENTS
    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL)
        #define RF_MAX_BATCH_ELEMENTS (2048)
    #else
        #define RF_MAX_BATCH_ELEMENTS (8192)
    #endif
#endif

#if !defined(RF_MAX_BATCH_BUFFERING)
    #define RF_MAX_BATCH_BUFFERING (1) // Max number of buffers for batching (multi-buffering)
#endif

#if !defined(RF_MAX_MATRIX_STACK_SIZE)
    #define RF_MAX_MATRIX_STACK_SIZE (32) // Max size of rf_mat _rf_ctx->gl_ctx.stack
#endif

#if !defined(RF_MAX_DRAWCALL_REGISTERED)
    #define RF_MAX_DRAWCALL_REGISTERED (256) // Max _rf_ctx->gl_ctx.draws by state changes (mode, texture)
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

#define RF_MAX_TEXT_UNICODE_CHARS (512) // Maximum number of unicode codepoints

//Note(LucaSas): Check this number
#if !defined(RF_MAX_FONT_CHARS)
    #define RF_MAX_FONT_CHARS (256) //Max number of characters in a font
#endif

#define RF_DEFAULT_FONT_CHARS_COUNT (224) //Number of characters in the default font

// Default vertex attribute names on shader to set location points
#define RF_DEFAULT_ATTRIB_POSITION_NAME    "vertex_position"    // shader-location = 0
#define RF_DEFAULT_ATTRIB_TEXCOORD_NAME    "vertex_tex_coord"   // shader-location = 1
#define RF_DEFAULT_ATTRIB_NORMAL_NAME      "vertex_normal"      // shader-location = 2
#define RF_DEFAULT_ATTRIB_COLOR_NAME       "vertex_color"       // shader-location = 3
#define RF_DEFAULT_ATTRIB_TANGENT_NAME     "vertex_tangent"     // shader-location = 4
#define RF_DEFAULT_ATTRIB_TEXCOORD2_NAME   "vertex_tex_coord2"  // shader-location = 5

//endregion

//region enums

typedef enum rf_error_type
{
    RF_BAD_ARGUMENT,
    RF_BAD_ALLOC,
    RF_BAD_IO,
    RF_BAD_BUFFER_SIZE,
    RF_STBI_FAILED,
    RF_UNSUPPORTED
} rf_error_type;

typedef enum rf_log_type
{
    RF_LOG_TYPE_DEBUG, // Useful mostly to rayfork devs
    RF_LOG_TYPE_INFO, // Information
    RF_LOG_TYPE_WARNING, // Warnings about things to be careful about
    RF_LOG_TYPE_ERROR, // Errors that prevented functions from doing everything they advertised
} rf_log_type;

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
    RF_NPT_9PATCH = 0, // Npatch defined by 3x3 tiles
    RF_NPT_3PATCH_VERTICAL, // Npatch defined by 1x3 tiles
    RF_NPT_3PATCH_HORIZONTAL // Npatch defined by 3x1 tiles
} rf_ninepatch_type;

// rf_font type, defines generation method
typedef enum rf_font_type
{
    RF_FONT_DEFAULT = 0, // Default font generation, anti-aliased
    RF_FONT_BITMAP, // Bitmap font generation, no anti-aliasing
    RF_FONT_SDF // SDF font generation, requires external shader
} rf_font_type;

typedef enum rf_allocator_mode
{
    RF_AM_ALLOC,
    RF_AM_FREE
} rf_allocator_mode;
//endregion

//region structs

//region math structs

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

//rf_mat type (OpenGL style 4x4 - right handed, column major)
typedef struct rf_mat rf_mat;
struct rf_mat
{
    float m0, m4, m8, m12;
    float m1, m5, m9, m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
};

typedef struct rf_float16 rf_float16;
struct rf_float16
{
    float v[16];
};

typedef struct rf_rec rf_rec;
struct rf_rec
{
    union
    {
        rf_vec2 pos;

        struct
        {
            float x;
            float y;
        };
    };

    union
    {
        rf_sizef size;

        struct
        {
            float width;
            float height;
        };
    };
};

//endregion

typedef struct rf_io_callbacks rf_io_callbacks;
struct rf_io_callbacks
{
    int (*get_file_size_proc)(const char* filename);
    bool (*read_file_into_buffer_proc)(const char* filename, void* buffer, int buffer_size); //Returns true if operation was successful
};

typedef struct rf_allocator rf_allocator;
struct rf_allocator
{
    void* user_data;
    void* (*request) (rf_allocator_mode mode, int size_to_alloc, void* pointer_to_free, void* user_data);
};

//RGBA (32bit)
typedef union rf_color rf_color;
union rf_color
{
    unsigned char rgba[4];

    struct
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
};

typedef struct rf_image rf_image;
struct rf_image
{
    void*           data;    // image raw data
    int             width;   // image base width
    int             height;  // image base height
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    bool            valid;   // True if the image is valid and can be used
};

typedef struct rf_mipmaps_image rf_mipmaps_image;
struct rf_mipmaps_image
{
    union
    {
        rf_image image;
        struct
        {
            void* data;    //image raw data
            int   width;   //image base width
            int   height;  //image base height
            rf_pixel_format format;  //Data format (rf_pixel_format type)
        };
    };

    int mipmaps; //Mipmap levels, 1 by default
};

typedef struct rf_gif rf_gif;
struct rf_gif
{
    int frames_count;
    int* frame_delays;

    union
    {
        rf_image image;

        struct
        {
            void* data;    //rf_image raw data
            int   width;   //rf_image base width
            int   height;  //rf_image base height
            rf_pixel_format format;  //Data format (rf_pixel_format type)
        };
    };
};

typedef struct rf_texture2d rf_texture2d;
typedef struct rf_texture2d rf_texture_cubemap;
struct rf_texture2d
{
    unsigned int id; //OpenGL texture id
    int width;       //rf_texture base width
    int height;      //rf_texture base height
    int mipmaps;     //Mipmap levels, 1 by default
    int format;      //Data format (rf_pixel_format type)
};

typedef struct rf_render_texture2d rf_render_texture2d;
typedef struct rf_render_texture2d rf_render_texture;
struct rf_render_texture2d
{
    unsigned int id;      //OpenGL Framebuffer Object (FBO) id
    rf_texture2d texture; //rf_color buffer attachment texture
    rf_texture2d depth;   //Depth buffer attachment texture
    bool depth_texture;   //Track if depth attachment is a texture or renderbuffer
};

typedef struct rf_npatch_info rf_npatch_info;
struct rf_npatch_info
{
    rf_rec source_rec; //Region in the texture
    int left;            //left border offset
    int top;             //top border offset
    int right;           //right border offset
    int bottom;          //bottom border offset
    int type;            //layout of the n-patch: 3x3, 1x3 or 3x1
};

typedef struct rf_utf8_codepoint rf_utf8_codepoint;
struct rf_utf8_codepoint
{
    int value;
    int bytes_processed;
};

typedef struct rf_char_info rf_char_info;
struct rf_char_info
{
    int value;    //Character value (Unicode)
    int offset_x;  //Character offset X when drawing
    int offset_y;  //Character offset Y when drawing
    int advance_x; //Character advance position X
    rf_image image;  //Character image data
};

typedef struct rf_font rf_font;
struct rf_font
{
    int base_size;      // Base size (default chars height)
    int chars_count;    // Number of characters
    rf_texture2d texture; // Characters texture atlas
    rf_rec* recs;   // Characters rectangles in texture
    rf_char_info* chars;   // Characters info data
};

typedef struct rf_load_font_async_result rf_load_font_async_result;
struct rf_load_font_async_result
{
    rf_font font;
    rf_image atlas;
};

typedef struct rf_camera3d rf_camera3d;
struct rf_camera3d
{
    rf_vec3 position; // Camera position
    rf_vec3 target;   // Camera target it looks-at
    rf_vec3 up;       // Camera up vector (rotation over its axis)
    float fovy;       // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
    int type;         // Camera type, defines GL_PROJECTION type: RF_CAMERA_PERSPECTIVE or RF_CAMERA_ORTHOGRAPHIC
};

typedef struct rf_camera2d rf_camera2d;
struct rf_camera2d
{
    rf_vec2 offset; // Camera offset (displacement from target)
    rf_vec2 target; // Camera target (rotation and zoom origin)
    float rotation; // Camera rotation in degrees
    float zoom;     // Camera zoom (scaling), should be 1.0f by default
};

typedef struct rf_mesh rf_mesh;
struct rf_mesh
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
    int* bone_ids;        // Vertex bone ids, up to 4 bones influence by vertex (skinning)
    float* bone_weights;  // Vertex bone weight, up to 4 bones influence by vertex (skinning)

    // OpenGL identifiers
    unsigned int vao_id;  // OpenGL Vertex Array Object id
    unsigned int* vbo_id; // OpenGL Vertex Buffer Objects id (default vertex data)
};

typedef struct rf_input_state_for_update_camera rf_input_state_for_update_camera;
struct rf_input_state_for_update_camera
{
    rf_vec2     mouse_position;
    int         mouse_wheel_move;                  //mouse wheel movement Y
    bool        is_camera_pan_control_key_down;    //MOUSE_MIDDLE_BUTTON
    bool        is_camera_alt_control_key_down;    //KEY_LEFT_ALT
    bool        is_camera_smooth_zoom_control_key; //KEY_LEFT_CONTROL
    bool        direction_keys[6];                 //'W', 'S', 'D', 'A', 'E', 'Q'
};

typedef struct rf_shader rf_shader;
struct rf_shader
{
    unsigned int id; // rf_shader program id
    int locs[RF_MAX_SHADER_LOCATIONS];       // rf_shader locations array (RF_MAX_SHADER_LOCATIONS)
};

typedef struct rf_material_map rf_material_map;
struct rf_material_map
{
    rf_texture2d texture; // rf_material map texture
    rf_color color;       // rf_material map color
    float value;       // rf_material map value
};

typedef struct rf_material rf_material;
struct rf_material
{
    rf_shader shader;     // rf_material shader
    rf_material_map* maps; // rf_material maps array (RF_MAX_MATERIAL_MAPS)
    float* params;     // rf_material generic parameters (if required)
};

typedef struct rf_transform rf_transform;
struct rf_transform
{
    rf_vec3 translation; // Translation
    rf_quaternion rotation; // Rotation
    rf_vec3 scale;       // Scale
};

typedef struct rf_bone_info rf_bone_info;
struct rf_bone_info
{
    char name[32]; // Bone name
    int  parent;   // Bone parent
};

typedef struct rf_model rf_model;
struct rf_model
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
};

typedef struct rf_model_animation rf_model_animation;
struct rf_model_animation
{
    int bone_count;             // Number of bones
    rf_bone_info* bones;        // Bones information (skeleton)
    int frame_count;            // Number of animation frames
    rf_transform** frame_poses; // Poses array by frame
};

typedef struct rf_model_animation_array rf_model_animation_array;
struct rf_model_animation_array
{
    int                 anims_count;
    rf_model_animation* anims;
};

// rf_ray type (useful for raycast)
typedef struct rf_ray rf_ray;
struct rf_ray
{
    rf_vec3 position;  // rf_ray position (origin)
    rf_vec3 direction; // rf_ray direction
};

typedef struct rf_ray_hit_info rf_ray_hit_info;
struct rf_ray_hit_info
{
    bool hit; // Did the ray hit something?
    float distance; // Distance to nearest hit
    rf_vec3 position; // Position of nearest hit
    rf_vec3 normal; // Surface normal of hit
};

typedef struct rf_bounding_box rf_bounding_box;
struct rf_bounding_box
{
    rf_vec3 min; // Minimum vertex box-corner
    rf_vec3 max; // Maximum vertex box-corner
};

typedef struct rf_base64_output rf_base64_output;
struct rf_base64_output
{
    int size;
    unsigned char* buffer;
};

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
#include "rayfork_gfx_backend_gl.inc"
#endif

typedef struct rf_default_font_buffer rf_default_font_buffers;
struct rf_default_font_buffer
{
    rf_char_info   chars[RF_DEFAULT_FONT_CHARS_COUNT];
    rf_rec         recs[RF_DEFAULT_FONT_CHARS_COUNT];
    unsigned short
};

typedef struct rf_context rf_context;
struct rf_context
{
    //Display size
    union
    {
        rf_sizei display_size;
        struct
        {
            int display_width;
            int display_height;
        };
    };

    // Screen width and height (used render area)
    union
    {
        rf_sizei screen_size;
        struct
        {
            int screen_width;
            int screen_height;
        };
    };

    // Framebuffer width and height (render area, including black bars if required)
    union
    {
        rf_sizei render_size;
        struct
        {
            int render_width;
            int render_height;
        };
    };

    // Current render width and height, it could change on rf_begin_texture_mode()
    union
    {
        rf_sizei current_size;
        struct
        {
            int current_width;
            int current_height;
        };
    };

    int render_offset_x; // Offset X from render area (must be divided by 2)
    int render_offset_y; // Offset Y from render area (must be divided by 2)
    rf_mat screen_scaling; // rf_mat to scale screen

    rf_gfx_context gfx_ctx;

    double current_time; // Current time measure
    double previous_time; // Previous time measure
    double update_time; // Time measure for frame update
    double draw_time; // Time measure for frame draw
    double frame_time; // Time measure for one frame
    double target_time; // Desired time for one frame, if 0 not applied

    //Camera 3d stuff
    //Note(LucaSas): might extract into another struct
    rf_vec2 camera_angle;         // rf_camera3d angle in plane XZ
    float camera_target_distance; // rf_camera3d distance from position to target
    rf_camera3d_mode camera_mode; // Current camera mode

    //Shapes global data
    rf_texture2d tex_shapes;
    rf_rec rec_tex_shapes;

    void (*wait_proc)(float); // Wait for some milliseconds (pauses program execution).
    double (*get_time_proc)(void); // Returns elapsed time in seconds since rf_context_init.
    int (*get_random_value_proc)(int min, int max); //A callback for a function that returns a random value in a range

    rf_font default_font; // Default font provided by raylib
    unsigned short default_font_pixels[128 * 128]; // Default font buffer
};

//endregion

//region setups and defaults
RF_API void rf_setup_viewport(int width, int height); // Set viewport for a provided width and height
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes
RF_API void rf_load_default_font(rf_allocator allocator, rf_allocator temp_allocator); // Load the raylib default font
RF_API rf_material rf_load_default_material(rf_allocator allocator); // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
//endregion

//region time functions
RF_API double rf_get_time(void); // Wait for some milliseconds (pauses program execution)
RF_API void rf_wait(float duration); // Returns elapsed time in seconds since rf_context_init
//endregion

//region default io and allocator
void* rf_malloc_wrapper(rf_allocator_mode mode, int size_to_alloc, void* pointer_to_free, void* user_data);

#if !defined(RF_NO_DEFAULT_IO)
    RF_API int rf_get_file_size(const char* filename); //Get the size of the file
    RF_API bool rf_load_file_into_buffer(const char* filename, void* buffer, int buffer_size); //Load the file into a buffer

    #define RF_DEFAULT_IO (RF_LIT(rf_io_callbacks) { rf_get_file_size, rf_load_file_into_buffer })
#endif
//endregion

//region getters
RF_API float rf_get_frame_time(); // Returns time in seconds for last frame drawn. Returns 0 if time functions are set to NULL
RF_API int rf_get_fps(); // Returns current FPS. Returns 0 if time functions are set to NULL
RF_API rf_font rf_get_default_font(); // Get the default font, useful to be used with extended parameters
RF_API rf_shader rf_get_default_shader(); // Get default shader
RF_API rf_texture2d rf_get_default_texture(); // Get default internal texture (white texture)
RF_API rf_context* rf_get_context(); //Get the context pointer
RF_API rf_image rf_get_screen_data(rf_allocator allocator, rf_allocator temp_allocator); // Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
//endregion

//region setters
RF_API void rf_set_random_value_proc(int (*get_random_value_proc)(int, int)); // Set the proc used by some functions to get a random number in a range
RF_API void rf_set_global_context_pointer(rf_context* ctx); // Set the global context pointer
RF_API void rf_set_viewport(int width, int height); // Set viewport for a provided width and height
RF_API void rf_set_target_fps(int fps); // Set target FPS (maximum). Ignored if time functions are set to NULL
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes
RF_API void rf_set_time_functions(void (*wait_proc)(float), double (*get_time_proc)(void)); //Used to set custom functions
//endregion

//region math
#define RF_PI (3.14159265358979323846f)

#define RF_DEG2RAD (RF_PI / 180.0f)
#define RF_RAD2DEG (180.0f / RF_PI)

#define RF_VEC2_ZERO ((rf_vec2) { 0.0f, 0.0f })
#define RF_VEC2_ONE  ((rf_vec2) { 1.0f, 1.0f })

#define RF_VEC3_ZERO ((rf_vec3) { 0.0f, 0.0f, 0.0f })
#define RF_VEC3_ONE  ((rf_vec3) { 1.0f, 1.0f, 1.0f })

//region base64
RF_API int rf_get_size_base64(const unsigned char* input);
RF_API rf_base64_output rf_decode_base64(const unsigned char* input, rf_allocator allocator);
//endregion

//region color
RF_API bool rf_color_equal_rgb(rf_color a, rf_color b); // Returns true if the two colors have the same values for the rgb components
RF_API bool rf_color_equal(rf_color a, rf_color b); // Returns true if the two colors have the same values
RF_API int rf_color_to_int(rf_color color); // Returns hexadecimal value for a rf_color
RF_API rf_vec4 rf_color_normalize(rf_color color); // Returns color normalized as float [0..1]
RF_API rf_color rf_color_from_normalized(rf_vec4 normalized); // Returns color from normalized values [0..1]
RF_API rf_vec3 rf_color_to_hsv(rf_color color); // Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
RF_API rf_color rf_color_from_hsv(rf_vec3 hsv); // Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
RF_API rf_color rf_color_from_int(int hex_value); // Returns a rf_color struct from hexadecimal value
RF_API rf_color rf_fade(rf_color color, float alpha); // rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
//endregion

//region camera
RF_API rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view); // Get world coordinates from screen coordinates
RF_API rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera); // Returns a ray trace from mouse position
RF_API rf_mat rf_get_camera_matrix(rf_camera3d camera); // Get transform matrix for camera
RF_API rf_mat rf_get_camera_matrix2d(rf_camera2d camera); // Returns camera 2d transform matrix
RF_API rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera); // Returns the screen space position from a 3d world space position
RF_API rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera); // Returns the screen space position for a 2d camera world space position
RF_API  rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera); // Returns the world space position for a 2d camera screen space position
//endregion

//region vec and matrix math
RF_API int rf_get_buffer_size_for_pixel_format(int width, int height, int format); //Get the buffer size of an image of a specific width and height in a given format
RF_API float rf_clamp(float value, float min, float max); // Clamp float value
RF_API float rf_lerp(float start, float end, float amount); // Calculate linear interpolation between two floats

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

//endregion

//region collision detection

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

//endregion
//endregion

//region rf_gfx
//region shader
RF_API rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code); // Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
RF_API void rf_gfx_unload_shader(rf_shader shader); // Unload shader from GPU memory (VRAM)
RF_API int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name); // Get shader uniform location
RF_API void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name); // Set shader uniform value
RF_API void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count); // Set shader uniform value vector
RF_API void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat); // Set shader uniform value (matrix 4x4)
RF_API void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture); // Set shader uniform value for texture
//endregion

RF_API rf_mat rf_gfx_get_matrix_projection(); // Return internal _rf_ctx->gl_ctx.projection matrix
RF_API rf_mat rf_gfx_get_matrix_modelview(); // Return internal _rf_ctx->gl_ctx.modelview matrix
RF_API void rf_gfx_set_matrix_projection(rf_mat proj); // Set a custom projection matrix (replaces internal _rf_ctx->gl_ctx.projection matrix)
RF_API void rf_gfx_set_matrix_modelview(rf_mat view); // Set a custom _rf_ctx->gl_ctx.modelview matrix (replaces internal _rf_ctx->gl_ctx.modelview matrix)

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

RF_API void rf_gfx_close(); // De-inititialize rlgl (buffers, shaders, textures)
RF_API void rf_gfx_draw(); // Update and draw default internal buffers

RF_API bool rf_gfx_check_buffer_limit(int v_count); // Check internal buffer overflow for a given number of vertex
RF_API void rf_gfx_set_debug_marker(const char* text); // Set debug marker for analysis

// Textures data management
RF_API unsigned int rf_gfx_load_texture(void* data, int width, int height, int format, int mipmap_count); // Load texture in GPU
RF_API unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, bool use_render_buffer); // Load depth texture/renderbuffer (to be attached to fbo)
RF_API unsigned int rf_gfx_load_texture_cubemap(void* data, int size, int format); // Load texture cubemap
RF_API void rf_gfx_update_texture(unsigned int id, int width, int height, int format, const void* data); // Update GPU texture with new data
RF_API void rf_gfx_get_gl_texture_formats(int format, unsigned int* gl_internal_format, unsigned int* gl_format, unsigned int* gl_type); // Get OpenGL internal formats
RF_API void rf_gfx_unload_texture(unsigned int id); // Unload texture from GPU memory

RF_API void rf_gfx_generate_mipmaps(rf_texture2d* texture); // Generate mipmap data for selected texture
RF_API void* rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator); // Read texture pixel data
RF_API unsigned char* rf_gfx_read_screen_pixels(int width, int height, rf_allocator allocator, rf_allocator temp_allocator); // Read screen pixel data (color buffer)

// Render texture management (fbo)
RF_API rf_render_texture2d rf_gfx_load_render_texture(int width, int height, int format, int depth_bits, bool use_depth_texture); // Load a render texture (with color and depth attachments)
RF_API void rf_gfx_render_texture_attach(rf_render_texture target, unsigned int id, int attach_type); // Attach texture/renderbuffer to an fbo
RF_API bool rf_gfx_render_texture_complete(rf_render_texture target); // Verify render texture is complete

// Vertex data management
RF_API void rf_gfx_load_mesh(rf_mesh* mesh, bool dynamic); // Upload vertex data into GPU and provided VAO/VBO ids
RF_API void rf_gfx_update_mesh(rf_mesh mesh, int buffer, int num); // Update vertex or index data on GPU (upload new data to one buffer)
RF_API void rf_gfx_update_mesh_at(rf_mesh mesh, int buffer, int num, int index); // Update vertex or index data on GPU, at index
RF_API void rf_gfx_draw_mesh(rf_mesh mesh, rf_material material, rf_mat transform); // Draw a 3d mesh with material and transform
RF_API void rf_gfx_unload_mesh(rf_mesh mesh); // Unload mesh data from CPU and GPU

//region gen textures
RF_API rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, int size); // Generate cubemap texture from HDR texture
RF_API rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, int size); // Generate irradiance texture using cubemap data
RF_API rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, int size); // Generate prefilter texture using cubemap data
RF_API rf_texture2d rf_gen_texture_brdf(rf_shader shader, int size); // Generate BRDF texture using cubemap data.
//endregion
//endregion

//region pixel format
RF_API const char* rf_pixel_format_string(rf_pixel_format format);
RF_API bool rf_is_uncompressed_format(rf_pixel_format format);
RF_API bool rf_is_compressed_format(rf_pixel_format format);
RF_API int rf_bits_per_pixel(rf_pixel_format format);
RF_API int rf_bytes_per_pixel(rf_uncompressed_pixel_format format);

RF_API void rf_format_pixels_to_normalized(const void* src, int src_size, rf_uncompressed_pixel_format src_format, rf_vec4* dst, int dst_size);
RF_API void rf_format_pixels_to_rgba32(const void* src, int src_size, rf_uncompressed_pixel_format src_format, rf_color* dst, int dst_size);
RF_API void rf_format_pixels(const void* src, int src_size, rf_uncompressed_pixel_format src_format, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format);

RF_API rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_uncompressed_pixel_format src_format);
RF_API rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_uncompressed_pixel_format src_format);
RF_API void rf_format_one_pixel(const void* src, rf_uncompressed_pixel_format src_format, void* dst, rf_uncompressed_pixel_format dst_format);
//endregion

//region image
//region extract image data functions
RF_API int rf_image_size(rf_image image); // Returns the size of the image in bytes
RF_API int rf_image_size_in_format(rf_image image, rf_pixel_format format);

RF_API void rf_image_pixels_to_rgba32_in_buffer(rf_image image, rf_color* dst, int dst_size);
RF_API void rf_image_pixels_to_normalized_in_buffer(rf_image image, rf_vec4* dst, int dst_size);

RF_API rf_color* rf_image_pixels_to_rgba32(rf_image image, rf_allocator allocator); // Get pixel data from image in the form of rf_color struct array
RF_API rf_vec4* rf_image_compute_pixels_to_normalized(rf_image image, rf_allocator allocator); // Get pixel data from image as rf_vec4 array (float normalized)

RF_API void rf_image_extract_palette(rf_image image, rf_color* dst, int dst_size); // Extract color palette from image to maximum size.
RF_API rf_rec rf_image_alpha_border(rf_image image, float threshold); // Get image alpha border rectangle
//endregion

//region loading & unloading functions
RF_API bool rf_supports_image_file_type(const char* filename);

RF_API rf_image rf_load_image_from_data_into_buffer(const void* data, int data_size, void* buffer, int buffer_size, rf_desired_channels channels, rf_allocator temp_allocator);
RF_API rf_image rf_load_image_from_data(const void* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator); // Load image from file data into CPU memory (RAM)
RF_API rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load image from file into CPU memory (RAM)

RF_API void     rf_unload_image(rf_image image, rf_allocator allocator); // Unloads the image using its allocator
//endregion

//region gif
RF_API rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_gif rf_load_animated_gif(const void* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_sizei rf_gif_frame_size(rf_gif gif);
RF_API rf_image rf_get_frame_from_gif(rf_gif gif, int frame); // Returns an image pointing to the frame in the gif
RF_API void rf_unload_gif(rf_gif gif, rf_allocator allocator);
//endregion

//region image manipulation
RF_API rf_mipmaps_image rf_mipmaps_image_copy(rf_mipmaps_image image, rf_allocator allocator); // Copy an image with mipmaps

RF_API rf_image rf_image_copy(rf_image image, rf_allocator allocator); // Copy an image
RF_API rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator); // Crop an image to area defined by a rectangle

RF_API rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator); // Resize and image to new size. Note: Uses stb default scaling filters (both bicubic): STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL (high-quality Catmull-Rom)
RF_API rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator temp_allocator); // Resize and image to new size using Nearest-Neighbor scaling algorithm
RF_API void rf_image_resize_canvas(rf_image* image, int new_width, int new_height, int offset_x, int offset_y, rf_color color, rf_allocator temp_allocator); // Resize canvas and fill with color. Note: Resize offset is relative to the top-left corner of the original image

RF_API void rf_image_gen_mipmaps(rf_image* image, rf_allocator temp_allocator); // Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image

RF_API void rf_image_to_pot(rf_image* image, rf_color fill_color, rf_allocator temp_allocator); // Convert image to POT (power-of-two). Note: It could be useful on OpenGL ES 2.0 (RPI, HTML5)
RF_API rf_image rf_image_format(rf_image image, rf_pixel_format new_format, rf_allocator allocator, rf_allocator temp_allocator); // Convert image data to desired format

RF_API void rf_image_alpha_mask(rf_image* image, rf_image alpha_mask, rf_allocator temp_allocator); // Apply alpha mask to image. Note 1: Returned image is GRAY_ALPHA (16bit) or RGBA (32bit). Note 2: alphaMask should be same size as image
RF_API void rf_image_alpha_clear(rf_image* image, rf_color color, float threshold, rf_allocator temp_allocator); // Clear alpha channel to desired color. Note: Threshold defines the alpha limit, 0.0f to 1.0f
RF_API void rf_image_alpha_premultiply(rf_image* image, rf_allocator temp_allocator); // Premultiply alpha channel
RF_API void rf_image_alpha_crop(rf_image* image, float threshold, rf_allocator temp_allocator); // Crop image depending on alpha value

RF_API void rf_image_dither(rf_image* image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator temp_allocator); // Dither image data to 16bpp or lower (Floyd-Steinberg dithering) Note: In case selected bpp do not represent an known 16bit format, dithered data is stored in the LSB part of the unsigned short

RF_API rf_image rf_image_text(const char* text, int text_len, int font_size, rf_color color, rf_allocator allocator, rf_allocator temp_allocator); // Create an image from text (default font)
RF_API rf_image rf_image_text_ex(rf_font font, const char* text, int text_len, float font_size, float spacing, rf_color tint, rf_allocator allocator, rf_allocator temp_allocator); // Create an image from text (custom sprite font)

RF_API void rf_image_flip_vertical(rf_image* image, rf_allocator temp_allocator); // Flip image vertically
RF_API void rf_image_flip_horizontal(rf_image* image, rf_allocator temp_allocator); // Flip image horizontally
RF_API void rf_image_rotate_cw(rf_image* image, rf_allocator temp_allocator); // Rotate image clockwise 90deg
RF_API void rf_image_rotate_ccw(rf_image* image, rf_allocator temp_allocator); // Rotate image counter-clockwise 90deg
RF_API void rf_image_color_tint(rf_image* image, rf_color color, rf_allocator temp_allocator); // Modify image color: tint
RF_API void rf_image_color_invert(rf_image* image, rf_allocator temp_allocator); // Modify image color: invert
RF_API void rf_image_color_grayscale(rf_image* image, rf_allocator temp_allocator); // Modify image color: grayscale
RF_API void rf_image_color_contrast(rf_image* image, float contrast, rf_allocator temp_allocator); // Modify image color: contrast (-100 to 100)
RF_API void rf_image_color_brightness(rf_image* image, int brightness, rf_allocator temp_allocator); // Modify image color: brightness (-255 to 255)
RF_API void rf_image_color_replace(rf_image* image, rf_color color, rf_color replace, rf_allocator temp_allocator); // Modify image color: replace color

// rf_image generation functions

RF_API rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: plain color
RF_API rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: vertical gradient
RF_API rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: horizontal gradient
RF_API rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: radial gradient
RF_API rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: checked
RF_API rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: white noise
RF_API rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: perlin noise
RF_API rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_allocator allocator, rf_allocator temp_allocator); // Generate image: cellular algorithm. Bigger tileSize means bigger cells

//endregion
//endregion

//region texture
RF_API rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks io); // Load texture from file into GPU memory (VRAM)
RF_API rf_texture2d rf_load_texture_from_data(const void* data, int data_len, rf_allocator temp_allocator); // Load texture from an image file data using stb
RF_API rf_texture2d rf_load_texture_from_image(rf_image image); // Load texture from image data
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator); // Load cubemap from image, multiple image cubemap layouts supported
RF_API rf_render_texture2d rf_load_render_texture(int width, int height); // Load texture for rendering (framebuffer)
RF_API rf_image rf_get_texture_data(rf_texture2d texture, rf_allocator allocator); // Get pixel data from GPU texture and return an rf_image
RF_API void rf_update_texture(rf_texture2d texture, const void* pixels); // Update GPU texture with new data. Pixels data must match texture.format
RF_API void rf_gen_texture_mipmaps(rf_texture2d* texture); // Generate GPU mipmaps for a texture
RF_API void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode); // Set texture scaling filter mode
RF_API void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode); // Set texture wrapping mode
RF_API void rf_unload_texture(rf_texture2d texture); // Unload texture from GPU memory (VRAM)
RF_API void rf_unload_render_texture(rf_render_texture2d target); // Unload render texture from GPU memory (VRAM)
//endregion

//region font & text
RF_API rf_utf8_codepoint rf_get_next_utf8_codepoint(const char* text, int len); //Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found or the length is exhausted
RF_API rf_font rf_load_font_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load font from file into GPU memory (VRAM)
RF_API rf_font rf_load_font(const void* font_file_data, int font_file_data_size, int fontSize, int* fontChars, int chars_count, rf_allocator allocator, rf_allocator temp_allocator); // Load font from a font file data into GPU memory (VRAM)
RF_API rf_load_font_async_result rf_load_font_async(const unsigned char* font_file_data, int font_file_data_size, int fontSize, int* fontChars, int chars_count, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_font rf_finish_load_font_async(rf_load_font_async_result fontJobResult);
RF_API rf_char_info* rf_load_font_data(const void* font_data, int font_data_size, int font_size, int* font_chars, int chars_count, rf_font_type type, rf_allocator allocator, rf_allocator temp_allocator); // Load font data for further use
RF_API rf_font rf_load_font_from_image(rf_image image, rf_color key, int firstChar, rf_allocator allocator, rf_allocator temp_allocator); // Load font from rf_image (XNA style)
RF_API rf_image rf_gen_image_font_atlas(const rf_char_info* chars, rf_rec** char_recs, int chars_count, int font_size, int padding, bool use_skyline_rect_packing, rf_allocator allocator, rf_allocator temp_allocator); // Generate image font atlas using chars info
RF_API void rf_unload_font(rf_font font); // Unload rf_font from GPU memory (VRAM)

RF_API int rf_get_glyph_index(rf_font font, int character); // Get index position for a unicode character on font
RF_API rf_sizef rf_measure_text(rf_font font, const char* text, int len, float font_size, float spacing); // Measure string size for rf_font
RF_API rf_sizef rf_measure_text_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float extra_spacing, bool wrap);
//endregion

//region drawing
// Drawing-related functions
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
RF_API void rf_draw_texture(rf_texture2d texture, rf_vec2 position, float rotation, float scale, rf_color tint); // Draw a rf_texture2d with extended parameters
RF_API void rf_draw_texture_region(rf_texture2d texture, rf_rec source_rec, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint); // Draw a part of a texture defined by a rectangle with 'pro' parameters
RF_API void rf_draw_texture_npatch(rf_texture2d texture, rf_npatch_info n_patch_info, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint); // Draws a texture (or part of it) that stretches or shrinks nicely

// Text drawing functions
RF_API void rf_draw_fps(int posX, int posY); // Shows current FPS
RF_API void rf_draw_text(const char* text, int posX, int posY, int fontSize, rf_color color); // Draw text (using default font)
RF_API void rf_draw_text_ex(rf_font font, const char* text, int text_len, rf_vec2 position, float fontSize, float spacing, rf_color tint); // Draw text using font and additional parameters
RF_API void rf_draw_text_wrap(rf_font font, const char* text, int text_len, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode); // Draw text and wrap at a specific width
RF_API void rf_draw_text_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint); // Draw text using font inside rectangle limits
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
RF_API void rf_draw_model(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotation_angle, rf_vec3 scale, rf_color tint); // Draw a model with extended parameters
RF_API void rf_draw_model_wires(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotation_angle, rf_vec3 scale, rf_color tint); // Draw a model wires (with texture if set) with extended parameters
RF_API void rf_draw_bounding_box(rf_bounding_box box, rf_color color); // Draw bounding box (wires)
RF_API void rf_draw_billboard(rf_camera3d camera, rf_texture2d texture, rf_vec3 center, float size, rf_color tint); // Draw a billboard texture
RF_API void rf_draw_billboard_rec(rf_camera3d camera, rf_texture2d texture, rf_rec source_rec, rf_vec3 center, float size, rf_color tint); // Draw a billboard texture defined by source_rec

// rf_image draw

RF_API void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator); // Draw a source image within a destination image (tint applied to source)
RF_API void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator); // Draw rectangle within an image
RF_API void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator); // Draw rectangle lines within an image
RF_API void rf_image_draw_text(rf_image* dst, rf_vec2 position, const char* text, int text_len, int font_size, rf_color color, rf_allocator temp_allocator); // Draw text (default font) within an image (destination)
RF_API void rf_image_draw_text_ex(rf_image* dst, rf_vec2 position, rf_font font, const char* text, int text_len, float font_size, float spacing, rf_color color, rf_allocator temp_allocator); // Draw text (custom sprite font) within an image (destination)
//endregion

//region model & materials & animations
RF_API rf_bounding_box rf_mesh_bounding_box(rf_mesh mesh); // Compute mesh bounding box limits
RF_API void rf_mesh_compute_tangents(rf_mesh* mesh, rf_allocator temp_allocator); // Compute mesh tangents
RF_API void rf_mesh_compute_binormals(rf_mesh* mesh); // Compute mesh binormals
RF_API void rf_unload_mesh(rf_mesh mesh); // Unload mesh from memory (RAM and/or VRAM)

RF_API rf_model rf_load_model(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_model rf_load_model_from_obj(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load model from files (meshes and materials)
RF_API rf_model rf_load_model_from_iqm(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load model from files (meshes and materials)
RF_API rf_model rf_load_model_from_gltf(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load model from files (meshes and materials)
RF_API rf_model rf_load_model_with_mesh(rf_mesh mesh, rf_allocator allocator); // Load model from generated mesh. Note: The function takes ownership of the mesh in model.meshes[0]
RF_API void rf_unload_model(rf_model model); // Unload model from memory (RAM and/or VRAM)

RF_API rf_material* rf_load_materials_from_mtl(const char* data, int data_size, int* material_count, rf_allocator allocator); // Load materials from model file
RF_API void rf_set_material_texture(rf_material* material, int map_type, rf_texture2d texture); // Set texture for a material map type (rf_map_diffuse, rf_map_specular...)
RF_API void rf_set_model_mesh_material(rf_model* model, int mesh_id, int material_id); // Set material for a mesh
RF_API void rf_unload_material(rf_material material); // Unload material from GPU memory (VRAM)

// Animations
RF_API rf_model_animation_array rf_load_model_animations_from_iqm_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_model_animation_array rf_load_model_animations_from_iqm(const unsigned char* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator); // Load model animations from file
RF_API void rf_update_model_animation(rf_model model, rf_model_animation anim, int frame); // Update model animation pose
RF_API bool rf_is_model_animation_valid(rf_model model, rf_model_animation anim); // Check model animation skeleton match
RF_API void rf_unload_model_animation(rf_model_animation anim); // Unload animation data

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
//endregion

#endif //#ifndef RAYFORK_H