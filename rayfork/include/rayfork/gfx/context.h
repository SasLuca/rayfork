#ifndef RAYFORK_GFX_CONTEXT_H
#define RAYFORK_GFX_CONTEXT_H

#include "rayfork/gfx/render-batch.h"
#include "rayfork/gfx/bitmap-font.h"

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
    rf_mat         stack[rf_max_matrix_stack_size];
    int            stack_counter;
    bool           transform_matrix_required;

    unsigned int default_texture_id;       // Default texture (1px white) useful for plain color polys (required by shader)
    unsigned int default_vertex_shader_id; // Default vertex shader id (used by default shader program)
    unsigned int default_frag_shader_id;   // Default fragment shader Id (used by default shader program)

    rf_shader default_shader; // Basic shader, support vertex color and diffuse texture
    rf_shader current_shader; // Shader to be used on rendering (by default, default_shader)

    rf_blend_mode blend_mode; // Track current blending mode

    rf_texture2d tex_shapes;
    rf_rec       rec_tex_shapes;

    rf_gfx_backend_data gfx_backend_data;

    rf_builtin_font_memory builtin_font_data;
    rf_bitmap_font         builtin_font;
} rf_gfx_context;

rf_extern rf_gfx_context* rf_global_gfx_context;

rf_extern void rf_gfx_backend_init_impl(rf_gfx_backend_data* gfx_data);
rf_extern void rf_gfx_init(rf_gfx_context* ctx, int screen_width, int screen_height, rf_gfx_backend_data* gfx_data);

rf_extern rf_int rf_gfx_width();
rf_extern rf_int rf_gfx_height();

rf_extern rf_sizef         rf_get_viewport();
rf_extern rf_texture2d     rf_get_shapes_texture();       // Get texture to draw shapes, the user can customize this using rf_set_shapes_texture
rf_extern rf_render_batch* rf_get_current_render_batch(); // Return a pointer to the current render batch
rf_extern rf_shader        rf_get_default_shader();       // Get default shader
rf_extern rf_texture2d     rf_get_default_texture();      // Get default internal texture (white texture)
rf_extern rf_image         rf_get_screen_data(rf_allocator allocator); // Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
rf_extern rf_image         rf_get_screen_data_to_buffer(rf_color* dst, rf_int dst_size);
rf_extern rf_bitmap_font   rf_get_builtin_bitmap_font();  // Get builtin font

rf_extern void rf_set_global_gfx_context_pointer(rf_gfx_context* ctx);     // Set the global context pointer
rf_extern void rf_set_viewport(int width, int height);                     // Set viewport for a provided width and height
rf_extern void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes

rf_extern rf_material rf_load_default_material(rf_allocator allocator); // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)

#endif // RAYFORK_GFX_CONTEXT_H