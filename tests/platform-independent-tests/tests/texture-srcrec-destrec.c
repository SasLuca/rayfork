#include "platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - texture srcrec destrec"
};

rf_gfx_context ctx;
rf_render_batch batch;

rf_texture2d scarfy;

int frame_width;
int frame_height;

// Source rectangle (part of the texture to use for drawing)
rf_rec source_rec;

// Destination rectangle (screen rectangle where drawing part of texture)
rf_rec dest_rec;

// Origin of the texture (rotation/scale point), it's relative to destination rectangle size
rf_vec2 origin;

int rotation = 0;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_gfx_init(&ctx, window.width, window.height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&batch);

    scarfy = rf_load_texture_from_file(ASSETS_PATH"scarfy.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);

    frame_width = scarfy.width / 6;
    frame_height = scarfy.height;

    // Source rectangle (part of the texture to use for drawing)
    source_rec = (rf_rec){ 0.0f, 0.0f, frame_width, frame_height };

    // Destination rectangle (screen rectangle where drawing part of texture)
    dest_rec = (rf_rec){window.width / 2, window.height / 2, frame_width * 2, frame_height * 2 };

    // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
    origin = (rf_vec2){ frame_width, frame_height };
}

extern void game_update(const platform_input_state* input)
{
    // Update
    rotation++;

    // Draw
    rf_begin();

    rf_clear(RF_RAYWHITE);

    // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
    // sourceRec defines the part of the texture we use for drawing
    // destRec defines the rectangle where our texture part will fit (scaling it to fit)
    // origin defines the point of the texture used as reference for rotation and scaling
    // rotation defines the texture rotation (using origin as rotation point)
    rf_draw_texture_region(scarfy, source_rec, dest_rec, origin, (float)rotation, RF_WHITE);

    rf_draw_line((int)dest_rec.x, 0, (int)dest_rec.x, window.height, RF_GRAY);
    rf_draw_line(0, (int)dest_rec.y, window.width, (int)dest_rec.y, RF_GRAY);

    rf_draw_text("(c) Scarfy sprite by Eiden Marsal", window.width - 200, window.height - 20, 10, RF_GRAY);

    rf_end();
}