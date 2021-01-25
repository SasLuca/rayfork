//Implementation of the texture image loading example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context   rf_ctx;
rf_default_render_batch    rf_mem;
rf_default_font default_font_buffers;

rf_image image;
rf_texture2d texture;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    // Textures MUST be loaded after Window initialization (OpenGL context is required)

    // Loaded in CPU memory (RAM)
    image = rf_load_image_from_file_ez(ASSETS_PATH"raylib_logo.png");

    // Image converted to texture, GPU memory (VRAM)
    texture = rf_load_texture_from_image(image);

    // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM
    rf_image_free(image, RF_DEFAULT_ALLOCATOR);
}

void on_frame(const input_data input)
{
    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_texture(texture, SCREEN_WIDTH / 2 - texture.width / 2, SCREEN_HEIGHT / 2 - texture.height / 2, RF_WHITE);

        rf_draw_text("this IS a texture loaded from an image!", 300, 370, 10, RF_GRAY);

    rf_end();
}