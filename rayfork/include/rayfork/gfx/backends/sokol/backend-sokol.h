#ifndef RAYFORK_BACKEND_SOKOL_H
#define RAYFORK_BACKEND_SOKOL_H

#include "rayfork-low-level-renderer.h"

#define rf_default_gfx_backend_init_data (NULL)

typedef float rf_gfx_vertex_data_type;
typedef float rf_gfx_texcoord_data_type;
typedef unsigned char rf_gfx_color_data_type;
#if defined(rayfork_graphics_backend_gl33)
typedef unsigned int rf_gfx_vertex_index_data_type;
#else
typedef unsigned short rf_gfx_vertex_index_data_type;
#endif

#define rf_gfx_vertex_component_count       (3 * 4) // 3 float by vertex, 4 vertex by quad
#define rf_gdx_texcoord_component_count     (2 * 4) // 2 float by texcoord, 4 texcoord by quad
#define rf_gfx_color_component_count        (4 * 4) // 4 float by color, 4 colors by quad
#define rf_gfx_vertex_index_component_count (6)     // 6 int by quad (indices)

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

typedef struct rf_gfx_backend_data
{
    struct {
        rf_bool tex_comp_dxt_supported;           // DDS texture compression support
        rf_bool tex_comp_etc1_supported;          // ETC1 texture compression support
        rf_bool tex_comp_etc2_supported;          // ETC2/EAC texture compression support
        rf_bool tex_comp_pvrt_supported;          // PVR texture compression support
        rf_bool tex_comp_astc_supported;          // ASTC texture compression support
        rf_bool tex_npot_supported;               // NPOT textures full support
        rf_bool tex_float_supported;              // float textures support (32 bit per channel)
        rf_bool tex_depth_supported;              // Depth textures supported
        int     max_depth_bits;                   // Maximum bits for depth component
        rf_bool tex_mirror_clamp_supported;       // Clamp mirror wrap mode supported
        rf_bool tex_anisotropic_filter_supported; // Anisotropic texture filtering support
        float   max_anisotropic_level;            // Maximum anisotropy level supported (minimum is 2.0f)
        rf_bool debug_marker_supported;           // Debug marker support
    } extensions;
} rf_gfx_backend_data;

#endif // RAYFORK_BACKEND_SOKOL_H