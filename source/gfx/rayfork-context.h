#ifndef RAYFORK_CONTEXT_H
#define RAYFORK_CONTEXT_H

#include "rayfork-low-level-renderer.h"
#include "rayfork-font.h"
#include "rayfork-model.h"

typedef struct rf_default_font
{
    unsigned short  pixels       [128 * 128];
    rf_glyph_info   chars        [RF_BUILTIN_FONT_CHARS_COUNT];
    unsigned short  chars_pixels [128 * 128];
} rf_default_font;

typedef struct rf_gfx_context
{
    int current_width;
    int current_height;

    int render_width;
    int render_height;

    int framebuffer_width;
    int framebuffer_height;

    rf_mat           screen_scaling;
    rf_render_batch* current_batch;

    rf_matrix_mode current_matrix_mode;
    rf_mat*        current_matrix;
    rf_mat         modelview;
    rf_mat         projection;
    rf_mat         transform;
    rf_bool        transform_matrix_required;
    rf_mat         stack[RF_MAX_MATRIX_STACK_SIZE];
    int            stack_counter;

    unsigned int default_texture_id;       // Default texture (1px white) useful for plain color polys (required by shader)
    unsigned int default_vertex_shader_id; // Default vertex shader id (used by default shader program)
    unsigned int default_frag_shader_id;   // Default fragment shader Id (used by default shader program)

    rf_shader default_shader; // Basic shader, support vertex color and diffuse texture
    rf_shader current_shader; // Shader to be used on rendering (by default, default_shader)

    rf_blend_mode blend_mode; // Track current blending mode

    rf_texture2d tex_shapes;
    rf_rec       rec_tex_shapes;

    rf_font             default_font;
    rf_default_font     default_font_buffers;
    rf_gfx_backend_data gfx_backend_data;

    rf_logger   logger;
    rf_log_type logger_filter;
} rf_gfx_context;

RF_API void rf_gfx_init(rf_gfx_context* ctx, int screen_width, int screen_height, rf_gfx_backend_data* gfx_data);

RF_API rf_material rf_load_default_material(rf_allocator allocator); // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
RF_API rf_shader   rf_load_default_shader();

RF_API rf_render_batch* rf_get_current_render_batch();                      // Return a pointer to the current render batch
RF_API rf_font          rf_get_default_font();                              // Get the default font, useful to be used with extended parameters
RF_API rf_shader        rf_get_default_shader();                            // Get default shader
RF_API rf_texture2d     rf_get_default_texture();                           // Get default internal texture (white texture)
RF_API rf_gfx_context*  rf_get_gfx_context();                               // Get the context pointer
RF_API rf_image         rf_get_screen_data(rf_color* dst, rf_int dst_size); // Get pixel data from GPU frontbuffer and return an rf_image (screenshot)

RF_API void rf_set_global_gfx_context_pointer(rf_gfx_context* ctx);     // Set the global context pointer
RF_API void rf_set_viewport(int width, int height);                     // Set viewport for a provided width and height
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes

#endif // RAYFORK_CONTEXT_H