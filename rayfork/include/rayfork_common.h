#ifndef RAYFORK_COMMON_H
#define RAYFORK_COMMON_H

#include "stddef.h"
#include "stdbool.h"

//region macros and constants

//If no graphics backend was set, choose OpenGL ES3
#if !defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33) && !defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3) && !defined(RAYFORK_GRAPHICS_BACKEND_METAL) && !defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_DIRECTX)
    #define RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3
#endif

//Check to make sure only one graphics backend was selected
#if (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33) && (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_DIRECTX))) || \
    (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3) && (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33) || defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_DIRECTX))) || \
    (defined(RAYFORK_GRAPHICS_BACKEND_METAL) && (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33) || defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_DIRECTX))) || \
    (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_DIRECTX) && (defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33) || defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL)))
    #error You can only set one graphics backend but 2 were detected.
#endif

//Prefix for all the public functions
#ifndef RF_API
    #ifdef __cplusplus
        #define RF_API extern "C"
    #else
        #define RF_API extern
    #endif
#endif

//Prefix for all the private functions
#ifndef RF_INTERNAL
    #define RF_INTERNAL static
#endif

//Used to make constant literals work even in C++ mode
#ifdef __cplusplus
    #define RF_LIT(type) type
#else
    #define RF_LIT(type) (type)
#endif

#define RF_NO_ALLOCATOR      (RF_LIT(rf_allocator) { 0 })
#define RF_DEFAULT_ALLOCATOR (RF_LIT(rf_allocator) { NULL, rf_malloc_wrapper })

#define RF_ALLOC(allocator, amount) ((allocator).request(RF_AM_ALLOC, (amount), NULL, (allocator).user_data))
#define RF_FREE(allocator, pointer) ((allocator).request(RF_AM_FREE, 0, (pointer), (allocator).user_data))

#define RF_UNLOCKED_FPS (0)

#define RF_LOG_TRACE   (0)
#define RF_LOG_DEBUG   (1)
#define RF_LOG_INFO    (2)
#define RF_LOG_WARNING (3)
#define RF_LOG_ERROR   (4)
#define RF_LOG_FATAL   (5)

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
    #if defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL)
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
    RF_LOC_VERTEX_TEXCOORD01,
    RF_LOC_VERTEX_TEXCOORD02,
    RF_LOC_VERTEX_NORMAL,
    RF_LOC_VERTEX_TANGENT,
    RF_LOC_VERTEX_COLOR,
    RF_LOC_MATRIX_MVP,
    RF_LOC_MATRIX_MODEL,
    RF_LOC_MATRIX_VIEW,
    RF_LOC_MATRIX_PROJECTION,
    RF_LOC_VECTOR_VIEW,
    RF_LOC_COLOR_DIFFUSE,
    RF_LOC_COLOR_SPECULAR,
    RF_LOC_COLOR_AMBIENT,
    RF_LOC_MAP_ALBEDO, // RF_LOC_MAP_DIFFUSE
    RF_LOC_MAP_METALNESS, // RF_LOC_MAP_SPECULAR
    RF_LOC_MAP_NORMAL,
    RF_LOC_MAP_ROUGHNESS,
    RF_LOC_MAP_OCCLUSION,
    RF_LOC_MAP_EMISSION,
    RF_LOC_MAP_HEIGHT,
    RF_LOC_MAP_CUBEMAP,
    RF_LOC_MAP_IRRADIANCE,
    RF_LOC_MAP_PREFILTER,
    RF_LOC_MAP_BRDF,
} rf_shader_location_index;

#define RF_LOC_MAP_DIFFUSE RF_LOC_MAP_ALBEDO
#define RF_LOC_MAP_SPECULAR RF_LOC_MAP_METALNESS

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
    RF_MAP_ALBEDO = 0,    // RF_MAP_DIFFUSE
    RF_MAP_METALNESS = 1, // RF_MAP_SPECULAR
    RF_MAP_NORMAL = 2,
    RF_MAP_ROUGHNESS = 3,
    RF_MAP_OCCLUSION,
    RF_MAP_EMISSION,
    RF_MAP_HEIGHT,
    RF_MAP_CUBEMAP,     // NOTE: Uses GL_TEXTURE_CUBE_MAP
    RF_MAP_IRRADIANCE,  // NOTE: Uses GL_TEXTURE_CUBE_MAP
    RF_MAP_PREFILTER,   // NOTE: Uses GL_TEXTURE_CUBE_MAP
    RF_MAP_BRDF
} rf_material_map_type;

#define RF_MAP_DIFFUSE RF_MAP_ALBEDO
#define RF_MAP_SPECULAR RF_MAP_METALNESS

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum rf_pixel_format
{
    RF_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    RF_UNCOMPRESSED_GRAY_ALPHA, // 8 * 2 bpp (2 channels)
    RF_UNCOMPRESSED_R5G6B5, // 16 bpp
    RF_UNCOMPRESSED_R8G8B8, // 24 bpp
    RF_UNCOMPRESSED_R5G5B5A1, // 16 bpp (1 bit alpha)
    RF_UNCOMPRESSED_R4G4B4A4, // 16 bpp (4 bit alpha)
    RF_UNCOMPRESSED_R8G8B8A8, // 32 bpp
    RF_UNCOMPRESSED_R32, // 32 bpp (1 channel - float)
    RF_UNCOMPRESSED_R32G32B32, // 32 * 3 bpp (3 channels - float)
    RF_UNCOMPRESSED_R32G32B32A32, // 32 * 4 bpp (4 channels - float)
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
    int (*get_file_size_cb)(const char* filename);
    bool (*read_file_into_buffer_cb)(const char* filename, void* buffer, int buffer_size); //Returns true if operation was successful
};

typedef struct rf_allocator rf_allocator;
struct rf_allocator
{
    void* user_data;
    void* (*request) (rf_allocator_mode mode, int size_to_alloc, void* pointer_to_free, void* user_data);
};

//RGBA (32bit)
typedef struct rf_color rf_color;
struct rf_color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

typedef struct rf_image rf_image;
struct rf_image
{
    void* data;    //rf_image raw data
    int   width;   //rf_image base width
    int   height;  //rf_image base height
    int   mipmaps; //Mipmap levels, 1 by default
    rf_pixel_format   format;  //Data format (rf_pixel_format type)
    rf_allocator allocator; //Allocator used for the image
};

// rf_texture2d type
// NOTE: Data stored in GPU memory
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

//rf_render_texture2d type, for texture rendering
typedef struct rf_render_texture2d rf_render_texture2d;
typedef struct rf_render_texture2d rf_render_texture;
struct rf_render_texture2d
{
    unsigned int id;      //OpenGL Framebuffer Object (FBO) id
    rf_texture2d texture; //rf_color buffer attachment texture
    rf_texture2d depth;   //Depth buffer attachment texture
    bool depth_texture;   //Track if depth attachment is a texture or renderbuffer
};

//N-Patch layout info
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

//rf_font character info
typedef struct rf_char_info rf_char_info;
struct rf_char_info
{
    int value;    //Character value (Unicode)
    int offset_x;  //Character offset X when drawing
    int offset_y;  //Character offset Y when drawing
    int advance_x; //Character advance position X
    rf_image image;  //Character image data
};

// rf_font type, includes texture and charSet array data
typedef struct rf_font rf_font;
struct rf_font
{
    int base_size;      // Base size (default chars height)
    int chars_count;    // Number of characters
    rf_texture2d texture; // Characters texture atlas
    rf_rec* recs;   // Characters rectangles in texture
    rf_char_info* chars;   // Characters info data

    rf_allocator allocator;
};

typedef struct rf_load_font_async_result rf_load_font_async_result;
struct rf_load_font_async_result
{
    rf_font font;
    rf_image atlas;
};

// Camera type, defines a camera position/orientation in 3d space
typedef struct rf_camera3d rf_camera3d;
struct rf_camera3d
{
    rf_vec3 position; // Camera position
    rf_vec3 target;   // Camera target it looks-at
    rf_vec3 up;       // Camera up vector (rotation over its axis)
    float fovy;       // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
    int type;         // Camera type, defines GL_PROJECTION type: RF_CAMERA_PERSPECTIVE or RF_CAMERA_ORTHOGRAPHIC
};

// rf_camera2d type, defines a 2d camera
typedef struct rf_camera2d rf_camera2d;
struct rf_camera2d
{
    rf_vec2 offset; // Camera offset (displacement from target)
    rf_vec2 target; // Camera target (rotation and zoom origin)
    float rotation; // Camera rotation in degrees
    float zoom;     // Camera zoom (scaling), should be 1.0f by default
};

// Vertex data definning a mesh
// NOTE: Data stored in CPU memory (and GPU)
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

    rf_allocator allocator;
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

// rf_shader type (generic)
typedef struct rf_shader rf_shader;
struct rf_shader
{
    unsigned int id; // rf_shader program id
    int locs[RF_MAX_SHADER_LOCATIONS];       // rf_shader locations array (RF_MAX_SHADER_LOCATIONS)
};

// rf_material texture map
typedef struct rf_material_map rf_material_map;
struct rf_material_map
{
    rf_texture2d texture; // rf_material map texture
    rf_color color;       // rf_material map color
    float value;       // rf_material map value
};

// rf_material type (generic)
typedef struct rf_material rf_material;
struct rf_material
{
    rf_shader shader;     // rf_material shader
    rf_material_map* maps; // rf_material maps array (RF_MAX_MATERIAL_MAPS)
    float* params;     // rf_material generic parameters (if required)

    rf_allocator allocator;
};

// Transformation properties
typedef struct rf_transform rf_transform;
struct rf_transform
{
    rf_vec3 translation; // Translation
    rf_quaternion rotation; // Rotation
    rf_vec3 scale;       // Scale
};

// Bone information
typedef struct rf_bone_info rf_bone_info;
struct rf_bone_info
{
    char name[32]; // Bone name
    int  parent;   // Bone parent
};

// rf_model type
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

    rf_allocator allocator;
};

// rf_model animation
typedef struct rf_model_animation rf_model_animation;
struct rf_model_animation
{
    int bone_count;             // Number of bones
    rf_bone_info* bones;        // Bones information (skeleton)
    int frame_count;            // Number of animation frames
    rf_transform** frame_poses; // Poses array by frame
    rf_allocator allocator;
};

// rf_ray type (useful for raycast)
typedef struct rf_ray rf_ray;
struct rf_ray
{
    rf_vec3 position;  // rf_ray position (origin)
    rf_vec3 direction; // rf_ray direction
};

// Raycast hit information
typedef struct rf_ray_hit_info rf_ray_hit_info;
struct rf_ray_hit_info
{
    bool hit; // Did the ray hit something?
    float distance; // Distance to nearest hit
    rf_vec3 position; // Position of nearest hit
    rf_vec3 normal; // Surface normal of hit
};

// Bounding box type
typedef struct rf_bounding_box rf_bounding_box;
struct rf_bounding_box
{
    rf_vec3 min; // Minimum vertex box-corner
    rf_vec3 max; // Maximum vertex box-corner
};

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rf_dynamic_buffer rf_dynamic_buffer;
struct rf_dynamic_buffer
{
    int v_counter;  // vertex position counter to process (and draw) from full buffer
    int tc_counter; // vertex texcoord counter to process (and draw) from full buffer
    int c_counter;  // vertex color counter to process (and draw) from full buffer

    float vertices[3 * 4 * RF_MAX_BATCH_ELEMENTS];
    float texcoords[2 * 4 * RF_MAX_BATCH_ELEMENTS];
    unsigned char colors[4 * 4 * RF_MAX_BATCH_ELEMENTS];

#if defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33)
    unsigned int indices[6 * RF_MAX_BATCH_ELEMENTS]; // 6 int by quad (indices)
#elif defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3)
    unsigned short indices[6 * RF_MAX_BATCH_ELEMENTS];  // 6 int by quad (indices)
#endif

    unsigned int vao_id;         // OpenGL Vertex Array Object id
    unsigned int vbo_id[4];      // OpenGL Vertex Buffer Objects id (4 types of vertex data)
};

// Draw call type
typedef struct rf_draw_call rf_draw_call;
struct rf_draw_call
{
    int mode;                   // Drawing mode: LINES, TRIANGLES, QUADS
    int vertex_count;            // Number of vertex of the draw
    int vertex_alignment;        // Number of vertex required for index alignment (LINES, TRIANGLES)
    //unsigned int vao_id;         // Vertex array id to be used on the draw
    //unsigned int shaderId;      // rf_shader id to be used on the draw
    unsigned int texture_id;     // rf_texture id to be used on the draw
    // TODO: Support additional texture units?

    //rf_mat _rf_ctx->gl_ctx.projection;        // Projection matrix for this draw
    //rf_mat _rf_ctx->gl_ctx.modelview;         // Modelview matrix for this draw
};

typedef struct rf_memory rf_memory;
struct rf_memory
{
    rf_dynamic_buffer vertex_data[RF_MAX_BATCH_BUFFERING];
    rf_draw_call draw_calls[RF_MAX_DRAWCALL_REGISTERED];

    rf_char_info default_font_chars[RF_DEFAULT_FONT_CHARS_COUNT];
    rf_rec default_font_recs[RF_DEFAULT_FONT_CHARS_COUNT];
};

typedef struct rf_opengl_procs rf_opengl_procs;
struct rf_opengl_procs
{
    void (*glViewport)(int x, int y, int width, int height);
    void (*glBindTexture)(unsigned int target, unsigned int texture);
    void (*glTexParameteri)(unsigned int target, unsigned int pname, int param);
    void (*glTexParameterf)(unsigned int target, unsigned int pname, float param);
    void (*glBindFramebuffer)(unsigned int target, unsigned int framebuffer);
    void (*glEnable)(unsigned int cap);
    void (*glDisable)(unsigned int cap);
    void (*glScissor)(int x, int y, int width, int height);
    void (*glDeleteTextures)(int n, const unsigned int* textures);
    void (*glDeleteRenderbuffers)(int n, const unsigned int* renderbuffers);
    void (*glDeleteFramebuffers)(int n, const unsigned int* framebuffers);
    void (*glDeleteVertexArrays)(int n, const unsigned int* arrays);
    void (*glDeleteBuffers)(int n, const unsigned int* buffers);
    void (*glClearColor)(float red, float green, float blue, float alpha);
    void (*glClear)(unsigned int mask);
    void (*glBindBuffer)(unsigned int target, unsigned int buffer);
    void (*glBufferSubData)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data);
    void (*glBindVertexArray)(unsigned int array);
    void (*glGenBuffers)(int n, unsigned int* buffers);
    void (*glBufferData)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
    void (*glVertexAttribPointer)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
    void (*glEnableVertexAttribArray)(unsigned int index);
    void (*glGenVertexArrays)(int n, unsigned int* arrays);
    void (*glVertexAttrib3f)(unsigned int index, float x, float y, float z);
    void (*glDisableVertexAttribArray)(unsigned int index);
    void (*glVertexAttrib4f)(unsigned int index, float x, float y, float z, float w);
    void (*glVertexAttrib2f)(unsigned int index, float x, float y);
    void (*glUseProgram)(unsigned int program);
    void (*glUniform4f)(int location, float v0, float v1, float v2, float v3);
    void (*glActiveTexture)(unsigned int texture);
    void (*glUniform1i)(int location, int v0);
    void (*glUniformMatrix4fv)(int location, int count, unsigned char transpose, const float* value);
    void (*glDrawElements)(unsigned int mode, int count, unsigned int type, const void* indices);
    void (*glDrawArrays)(unsigned int mode, int first, int count);
    void (*glPixelStorei)(unsigned int pname, int param);
    void (*glGenTextures)(int n, unsigned int* textures);
    void (*glTexImage2D)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
    void (*glGenRenderbuffers)(int n, unsigned int* renderbuffers);
    void (*glBindRenderbuffer)(unsigned int target, unsigned int renderbuffer);
    void (*glRenderbufferStorage)(unsigned int target, unsigned int internalformat, int width, int height);
    void (*glCompressedTexImage2D)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data);
    void (*glTexSubImage2D)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels);
    void (*glGenerateMipmap)(unsigned int target);
    void (*glReadPixels)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
    void (*glGenFramebuffers)(int n, unsigned int* framebuffers);
    void (*glFramebufferTexture2D)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
    void (*glFramebufferRenderbuffer)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
    unsigned int (*glCheckFramebufferStatus)(unsigned int target);
    unsigned int (*glCreateShader)(unsigned int type);
    void (*glShaderSource)(unsigned int shader, int count, const char** string, const int* length);
    void (*glCompileShader)(unsigned int shader);
    void (*glGetShaderiv)(unsigned int shader, unsigned int pname, int* params);
    void (*glGetShaderInfoLog)(unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int (*glCreateProgram)();
    void (*glAttachShader)(unsigned int program, unsigned int shader);
    void (*glBindAttribLocation)(unsigned int program, unsigned int index, const char* name);
    void (*glLinkProgram)(unsigned int program);
    void (*glGetProgramiv)(unsigned int program, unsigned int pname, int* params);
    void (*glGetProgramInfoLog)(unsigned int program, int bufSize, int* length, char* infoLog);
    void (*glDeleteProgram)(unsigned int program);
    int (*glGetAttribLocation)(unsigned int program, const char* name);
    int (*glGetUniformLocation)(unsigned int program, const char* name);
    void (*glDetachShader)(unsigned int program, unsigned int shader);
    void (*glDeleteShader)(unsigned int shader);

    void (*glGetTexImage)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels); // NULL for OpenGLES3

    void (*glGetActiveUniform)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name);
    void (*glUniform1fv)(int location, int count, const float* value);
    void (*glUniform2fv)(int location, int count, const float* value);
    void (*glUniform3fv)(int location, int count, const float* value);
    void (*glUniform4fv)(int location, int count, const float* value);
    void (*glUniform1iv)(int location, int count, const int* value);
    void (*glUniform2iv)(int location, int count, const int* value);
    void (*glUniform3iv)(int location, int count, const int* value);
    void (*glUniform4iv)(int location, int count, const int* value);
    const unsigned char* (*glGetString)(unsigned int name);
    void (*glGetFloatv)(unsigned int pname, float* data);
    void (*glDepthFunc)(unsigned int func);
    void (*glBlendFunc)(unsigned int sfactor, unsigned int dfactor);
    void (*glCullFace)(unsigned int mode);
    void (*glFrontFace)(unsigned int mode);

    //On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version
    void (*glClearDepth)(double depth);
    void (*glClearDepthf)(float depth);

    void (*glGetIntegerv)(unsigned int pname, int* data); //OpenGL 33 only, should be NULL in OpenGL ES2
    const unsigned char* (*glGetStringi)(unsigned int name, unsigned int index);
};

typedef struct rf_renderer_context rf_renderer_context;
struct rf_renderer_context
{
    rf_mat stack[RF_MAX_MATRIX_STACK_SIZE];
    int stack_counter;

    rf_mat modelview;
    rf_mat projection;
    rf_mat* current_matrix;
    int current_matrix_mode;
    float current_depth;
    int current_buffer;

    // transform matrix to be used with rlTranslate, rlRotate, rlScale
    rf_mat transform_matrix;
    bool use_transform_matrix;

    // Default buffers draw calls
    rf_draw_call *draws;
    int draws_counter;

    // Default texture (1px white) useful for plain color polys (required by shader)
    unsigned int default_texture_id;

    // Default shaders
    unsigned int default_vertex_shader_id;   // Default vertex shader id (used by default shader program)
    unsigned int default_frag_shader_id;   // Default fragment shader Id (used by default shader program)

    rf_shader default_shader;        // Basic shader, support vertex color and diffuse texture
    rf_shader current_shader;        // rf_shader to be used on rendering (by default, default_shader)

    // Extension supported flag: Compressed textures
    bool tex_comp_dxt_supported;    // DDS texture compression support
    bool tex_comp_etc1_supported;   // ETC1 texture compression support
    bool tex_comp_etc2_supported;   // ETC2/EAC texture compression support
    bool tex_comp_pvrt_supported;   // PVR texture compression support
    bool tex_comp_astc_supported;   // ASTC texture compression support

    // Extension supported flag: Textures format
    bool tex_npot_supported;       // NPOT textures full support
    bool tex_float_supported;      // float textures support (32 bit per channel)
    bool tex_depth_supported;      // Depth textures supported
    int max_depth_bits;        // Maximum bits for depth component

    // Extension supported flag: Clamp mirror wrap mode
    bool tex_mirror_clamp_supported;        // Clamp mirror wrap mode supported

    // Extension supported flag: Anisotropic filtering
    bool tex_anisotropic_filter_supported;  // Anisotropic texture filtering support
    float max_anisotropic_level;            // Maximum anisotropy level supported (minimum is 2.0f)

    bool debug_marker_supported;   // Debug marker support

    int blend_mode;                   // Track current blending mode

    // Default framebuffer size
    int framebuffer_width;            // Default framebuffer width
    int framebuffer_height;           // Default framebuffer height

    //@Note(LucaSas): Camera 3d stuff, might extract into another struct
    rf_vec2 camera_angle; // rf_camera3d angle in plane XZ
    float camera_target_distance; // rf_camera3d distance from position to target

    int camera_mode; // Current camera mode

    //@Note(LucaSas): shapes global data
    rf_texture2d tex_shapes;
    rf_rec rec_tex_shapes;

    // Default dynamic buffer for elements data. Note: A multi-buffering system is supported
    rf_memory* memory;
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

    rf_font default_font; // Default font provided by raylib

    rf_renderer_context gfx_ctx;

    #if defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_33) || defined(RAYFORK_GRAPHICS_BACKEND_OPENGL_ES3)
        rf_opengl_procs gl;
    #endif

    double current_time; // Current time measure
    double previous_time; // Previous time measure
    double update_time; // Time measure for frame update
    double draw_time; // Time measure for frame draw
    double frame_time; // Time measure for one frame
    double target_time; // Desired time for one frame, if 0 not applied

    void (*wait_func)(float); // Wait for some milliseconds (pauses program execution). Only used if RF_CUSTOM_TIME is defined. Can be null in which case it just won't have any effect
    double (*get_time_func)(void); // Returns elapsed time in seconds since rf_context_init. Only used if RF_CUSTOM_TIME is defined. Can be null in which case it just won't have any effect
};

//endregion

#endif //#ifndef RAYFORK_COMMON_H