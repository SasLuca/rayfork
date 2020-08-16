//Implementation of the texture srcrec destrec example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

struct rf_texture2d scarfy;

int frame_width;
int frame_height;

// Source rectangle (part of the texture to use for drawing)
rf_rec source_rec;

// Destination rectangle (screen rectangle where drawing part of texture)
rf_rec dest_rec;

// Origin of the texture (rotation/scale point), it's relative to destination rectangle size
rf_vec2 origin;

int rotation = 0;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    scarfy = rf_load_texture_from_file(ASSETS_PATH"scarfy.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);

    frame_width = scarfy.width / 6;
    frame_height = scarfy.height;

    // Source rectangle (part of the texture to use for drawing)
    source_rec = (rf_rec){ 0.0f, 0.0f, frame_width, frame_height };

    // Destination rectangle (screen rectangle where drawing part of texture)
    dest_rec = (rf_rec){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, frame_width * 2, frame_height*2 };

    // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
    origin = (rf_vec2){ frame_width, frame_height };
}

void on_frame(const input_data input)
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

    rf_draw_line((int)dest_rec.x, 0, (int)dest_rec.x, SCREEN_HEIGHT, RF_GRAY);
    rf_draw_line(0, (int)dest_rec.y, SCREEN_WIDTH, (int)dest_rec.y, RF_GRAY);

    rf_draw_text("(c) Scarfy sprite by Eiden Marsal", SCREEN_WIDTH - 200, SCREEN_HEIGHT - 20, 10, RF_GRAY);

    rf_end();
}