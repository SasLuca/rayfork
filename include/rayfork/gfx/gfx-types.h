#ifndef RAYFORK_GFX_TYPES_H
#define RAYFORK_GFX_TYPES_H

#include "rayfork/foundation/basicdef.h"
#include "rayfork/image/pixel-format.h"

struct rf_gfx_context;

#pragma region backend selection

#if !defined(RAYFORK_GRAPHICS_BACKEND_GL_33) && !defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) && !defined(RAYFORK_GRAPHICS_BACKEND_METAL) && !defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    #define RAYFORK_NO_GRAPHICS_BACKEND_SELECTED_BY_THE_USER (1)
#endif

// If no graphics backend was set, choose OpenGL33 on desktop and OpenGL ES3 on mobile
#if RAYFORK_NO_GRAPHICS_BACKEND_SELECTED_BY_THE_USER
    #if defined(rayfork_platform_windows) || defined(rayfork_platform_linux) || defined(rayfork_platform_macos)
        #define RAYFORK_GRAPHICS_BACKEND_GL_33 (1)
    #else // if on mobile
        #define RAYFORK_GRAPHICS_BACKEND_GL_ES3 (1)
    #endif
#endif

// Check to make sure only one graphics backend was selected
#if (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) + defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) + defined(RAYFORK_GRAPHICS_BACKEND_METAL) + defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)) != 1
    #error rayfork error: you can only set one graphics backend but 2 or more were detected.
#endif

#pragma endregion

#pragma region constants

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

#if !defined(rf_max_matrix_stack_size)
    #define rf_max_matrix_stack_size (32) // Max size of rf_mat rf__ctx->gl_ctx.stack
#endif

#if !defined(RF_DEFAULT_BATCH_DRAW_CALLS_COUNT)
    #define RF_DEFAULT_BATCH_DRAW_CALLS_COUNT (256) // Max rf__ctx->gl_ctx.draws by state changes (mode, texture)
#endif

// Shader and material limits
#if !defined(RF_MAX_SHADER_LOCATIONS)
    #define RF_MAX_SHADER_LOCATIONS (32) // Maximum number of predefined locations stored in shader struct
#endif

#if !defined(RF_MAX_MATERIAL_MAPS)
    #define RF_MAX_MATERIAL_MAPS (12) // Maximum number of texture maps stored in shader struct
#endif

#if !defined(RF_MAX_TEXT_BUFFER_LENGTH)
    #define RF_MAX_TEXT_BUFFER_LENGTH (1024) // Size of internal rf_internal buffers used on some functions:
#endif

#if !defined(RF_MAX_MESH_VBO)
    #define RF_MAX_MESH_VBO (7) // Maximum number of vbo per mesh
#endif

// Default vertex attribute names on shader to set location points
#define RF_DEFAULT_ATTRIB_POSITION_NAME  "vertex_position"    // shader-location = 0
#define RF_DEFAULT_ATTRIB_TEXCOORD_NAME  "vertex_tex_coord"   // shader-location = 1
#define RF_DEFAULT_ATTRIB_NORMAL_NAME    "vertex_normal"      // shader-location = 2
#define RF_DEFAULT_ATTRIB_COLOR_NAME     "vertex_color"       // shader-location = 3
#define RF_DEFAULT_ATTRIB_TANGENT_NAME   "vertex_tangent"     // shader-location = 4
#define RF_DEFAULT_ATTRIB_TEXCOORD2_NAME "vertex_tex_coord2"  // shader-location = 5

#pragma endregion

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

// Shader location point type
typedef enum rf_shader_location_index
{
    RF_LOC_VERTEX_POSITION   = 0,
    RF_LOC_VERTEX_TEXCOORD01 = 1,
    RF_LOC_VERTEX_TEXCOORD02 = 2,
    RF_LOC_VERTEX_NORMAL     = 3,
    RF_LOC_VERTEX_TANGENT    = 4,
    RF_LOC_VERTEX_COLOR      = 5,
    RF_LOC_MATRIX_MVP        = 6,
    RF_LOC_MATRIX_MODEL      = 7,
    RF_LOC_MATRIX_VIEW       = 8,
    RF_LOC_MATRIX_PROJECTION = 9,
    RF_LOC_VECTOR_VIEW       = 10,
    RF_LOC_COLOR_DIFFUSE     = 11,
    RF_LOC_COLOR_SPECULAR    = 12,
    RF_LOC_COLOR_AMBIENT     = 13,

    // These 2 are intentionally the same
    RF_LOC_MAP_ALBEDO        = 14,
    RF_LOC_MAP_DIFFUSE       = 14,

    // These 2 are intentionally the same
    RF_LOC_MAP_METALNESS     = 15,
    RF_LOC_MAP_SPECULAR      = 15,

    RF_LOC_MAP_NORMAL        = 16,
    RF_LOC_MAP_ROUGHNESS     = 17,
    RF_LOC_MAP_OCCLUSION     = 18,
    RF_LOC_MAP_EMISSION      = 19,
    RF_LOC_MAP_HEIGHT        = 20,
    RF_LOC_MAP_CUBEMAP       = 21,
    RF_LOC_MAP_IRRADIANCE    = 22,
    RF_LOC_MAP_PREFILTER     = 23,
    RF_LOC_MAP_BRDF          = 24,
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

typedef struct rf_shader
{
    unsigned int id; // rf_shader program id
    int locs[RF_MAX_SHADER_LOCATIONS];       // rf_shader locations array (RF_MAX_SHADER_LOCATIONS)
} rf_shader;

typedef struct rf_gfx_pixel_format
{
    unsigned int internal_format;
    unsigned int format;
    unsigned int type;
    rf_valid_t valid;
} rf_gfx_pixel_format;

typedef struct rf_texture2d
{
    unsigned int id;        // OpenGL texture id
    int width;              // rf_texture base width
    int height;             // rf_texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    rf_pixel_format format; // Data format (rf_pixel_format type)
    rf_valid_t valid;
} rf_texture2d, rf_texture_cubemap;

typedef struct rf_render_texture2d
{
    unsigned int id;            // OpenGL Framebuffer Object (FBO) id
    rf_texture2d texture;       // rf_color buffer attachment texture
    rf_texture2d depth;         // Depth buffer attachment texture
    int          depth_texture; // Track if depth attachment is a texture or renderbuffer
} rf_render_texture2d;

struct rf_vertex_buffer;
struct rf_mesh;
struct rf_material;
struct rf_gfx_backend_data;
struct rf_render_batch;

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

typedef struct rf_material_map
{
    rf_texture2d texture; // rf_material map texture
    rf_color     color;   // rf_material map color
    float        value;   // rf_material map value
} rf_material_map;

typedef struct rf_material
{
    rf_shader        shader; // rf_material shader
    rf_material_map* maps;   // rf_material maps array (RF_MAX_MATERIAL_MAPS)
    float*           params; // rf_material generic parameters (if required)
} rf_material;

typedef struct rf_transform
{
    rf_vec3 translation;    // Translation
    rf_quaternion rotation; // Rotation
    rf_vec3 scale;          // Scale
} rf_transform;

typedef struct rf_bone_info
{
    char   name[32]; // Bone name
    rf_int parent;   // Bone parent
} rf_bone_info;

typedef struct rf_model
{
    rf_mat transform;  // Local transform matrix
    rf_int mesh_count; // Number of meshes
    rf_mesh* meshes;   // Meshes array

    rf_int material_count;  // Number of materials
    rf_material* materials; // Materials array
    int* mesh_material;     // Mesh material number

    // Animation data
    rf_int bone_count;       // Number of bones
    rf_bone_info* bones;     // Bones information (skeleton)
    rf_transform* bind_pose; // Bones base transformation (pose)
} rf_model;

typedef struct rf_model_animation
{
    rf_int bone_count;          // Number of bones
    rf_bone_info* bones;        // Bones information (skeleton)
    rf_int frame_count;         // Number of animation frames
    rf_transform** frame_poses; // Poses array by frame
} rf_model_animation;

typedef struct rf_model_animation_array
{
    rf_int              size;
    rf_model_animation* anims;
} rf_model_animation_array;

typedef struct rf_materials_array
{
    rf_int       size;
    rf_material* materials;
} rf_materials_array;

#endif // RAYFORK_GFX_TYPES_H