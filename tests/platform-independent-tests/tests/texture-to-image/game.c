//Implementation of the texture to imageexample from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context   rf_ctx;
rf_default_render_batch    rf_mem;
rf_default_font     default_font_buffers;

rf_image image;
rf_texture2d texture;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    image = rf_load_image_from_file_ez(ASSETS_PATH"raylib_logo.png");  // Load image data into CPU memory (RAM)
    texture = rf_load_texture_from_image(image);  // Image converted to texture, GPU memory (RAM -> VRAM)
    rf_unload_image_ez(image); // Unload image data from CPU memory (RAM)

    image = rf_gfx_read_texture_pixels_ez(texture); // Retrieve image data from GPU memory (VRAM -> RAM)
    rf_unload_texture(texture);                                 // Unload texture from GPU memory (VRAM)

    texture = rf_load_texture_from_image(image);                // Recreate texture from retrieved image data (RAM -> VRAM)
    rf_unload_image_ez(image);               // Unload retrieved image data from CPU memory (RAM)
}

void on_frame(const input_data input)
{
    // Update

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_texture(texture, SCREEN_WIDTH / 2 - texture.width / 2, SCREEN_HEIGHT / 2 - texture.height / 2, RF_WHITE);

        rf_draw_text("this IS a texture loaded from an image!", 300, 370, 10, RF_GRAY);

    rf_end();
}