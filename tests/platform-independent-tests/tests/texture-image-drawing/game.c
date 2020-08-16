//Implementation of the texture image drawing example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

rf_image cat;
rf_image parrots;

rf_texture2d texture;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    // We let the temp buffers leak since we dont care :P (maybe a good idea to demonstrate an allocator here)
    cat = rf_load_image_from_file_ez(ASSETS_PATH"cat.png"); // Load image in CPU memory (RAM)
    cat = rf_image_crop_ez(cat, (rf_rec) { 100, 10, 280, 380 }); // Crop an image piece
    cat = rf_image_flip_horizontal_ez(cat); // Flip cropped image horizontally
    cat = rf_image_resize_ez(cat, 150, 200); // Resize flipped-cropped image

    // Load image in CPU memory (RAM)
    parrots = rf_load_image_from_file(ASSETS_PATH"parrots.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);

    // Draw one image over the other with a scaling of 1.5f
    rf_image_draw(&parrots, cat, (rf_rec){ 0, 0, cat.width, cat.height }, (rf_rec){ 30, 40, cat.width*1.5f, cat.height*1.5f }, RF_WHITE, RF_DEFAULT_ALLOCATOR);

    // Crop resulting image
    rf_image_crop(parrots, (rf_rec){ 0, 50, parrots.width, parrots.height - 100 }, RF_DEFAULT_ALLOCATOR);

    rf_unload_image_ez(cat);

    // Image converted to texture, uploaded to GPU memory (VRAM)
    texture = rf_load_texture_from_image(parrots);

    // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM
    rf_unload_image_ez(parrots);
}

void on_frame(const input_data input)
{
    rf_begin();

    rf_clear(RF_RAYWHITE);

    rf_draw_texture(texture, SCREEN_WIDTH / 2 - texture.width / 2, SCREEN_HEIGHT / 2 - texture.height / 2 - 40, RF_WHITE);
    rf_draw_rectangle_outline((rf_rec){SCREEN_WIDTH/2 - texture.width/2, SCREEN_HEIGHT/2 - texture.height/2 - 40, texture.width, texture.height }, 1.0f, RF_DARKGRAY);

    rf_draw_text("We are drawing only one texture from various images composed!", 240, 400, 10, RF_DARKGRAY);
    rf_draw_text("Source images have been cropped, scaled, flipped and copied one over the other.", 190, 420, 10, RF_DARKGRAY);

    rf_end();
}