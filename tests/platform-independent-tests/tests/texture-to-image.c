#include "platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - basic shapes"
};

rf_context ctx;
rf_render_batch batch;

rf_image image;
rf_texture2d texture;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_init_context(&ctx);
    rf_init_gfx(window.width, window.height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&batch);

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    image = rf_load_image_from_file_ez(ASSETS_PATH"raylib_logo.png");  // Load image data into CPU memory (RAM)
    texture = rf_load_texture_from_image(image);                       // Image converted to texture, GPU memory (RAM -> VRAM)
    rf_unload_image_ez(image);                                         // Unload image data from CPU memory (RAM)

    image = rf_gfx_read_texture_pixels_ez(texture); // Retrieve image data from GPU memory (VRAM -> RAM)
    rf_unload_texture(texture);                     // Unload texture from GPU memory (VRAM)

    texture = rf_load_texture_from_image(image); // Recreate texture from retrieved image data (RAM -> VRAM)
    rf_unload_image_ez(image);                   // Unload retrieved image data from CPU memory (RAM)
}

extern void game_update(const platform_input_state* input)
{
    rf_begin();

    rf_clear(RF_RAYWHITE);

    rf_draw_texture(texture, window.width / 2 - texture.width / 2, window.height / 2 - texture.height / 2, RF_WHITE);

    rf_draw_text("this IS a texture loaded from an image!", 300, 370, 10, RF_GRAY);

    rf_end();
}