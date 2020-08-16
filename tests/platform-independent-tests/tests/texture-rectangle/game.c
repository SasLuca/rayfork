//Implementation of the texture rectangle example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"
#include <stdio.h>

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1

rf_context   rf_ctx;
rf_default_render_batch    rf_mem;
rf_default_font    default_font_buffers;

int screen_width = 800;
int screen_height = 450;

rf_texture2d scarfy;
rf_vec2 position = { 350.0f, 280.0f };
rf_rec frame_rec;

int current_frame = 0;

int frames_counter = 0;
int frames_speed = 8;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    scarfy = rf_load_texture_from_file(ASSETS_PATH"scarfy.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);
    frame_rec.pos = (rf_vec2){ 0.0f, 0.0f};
    frame_rec.size = (rf_sizef){ (float)scarfy.width/6, (float)scarfy.height };

}

void on_frame(const input_data input)
{
    // Update
    frames_counter++;

    if (frames_counter >= (60 / frames_speed))
    {
        frames_counter = 0;
        current_frame++;

        if (current_frame > 5) current_frame = 0;

        frame_rec.x = (float)current_frame * (float)scarfy.width / 6;
    }

    // @Note: gotta fix input
    if (input.right_down) frames_speed++;
    else if (input.left_down) frames_speed--;

    if (frames_speed > MAX_FRAME_SPEED) frames_speed = MAX_FRAME_SPEED;
    else if (frames_speed < MIN_FRAME_SPEED) frames_speed = MIN_FRAME_SPEED;
    // Draw
    rf_begin();

    rf_clear(RF_RAYWHITE);

    rf_draw_texture(scarfy, 15, 40, RF_WHITE);
    rf_draw_rectangle_outline((rf_rec){ 15, 40, scarfy.width, scarfy.height }, 1, RF_LIME);
    rf_draw_rectangle_outline((rf_rec){ 15 + frame_rec.x, 40 + frame_rec.y, frame_rec.width, frame_rec.height }, 1, RF_RED);

    rf_draw_text("FRAME SPEED: ", 165, 210, 10, RF_DARKGRAY);

    char text[1024];
    snprintf(text, sizeof(text), "%02i FPS", frames_speed);
    rf_draw_text(text, 575, 210, 10, RF_DARKGRAY);

    rf_draw_text("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, RF_DARKGRAY);

    for (int i = 0; i < MAX_FRAME_SPEED; i++)
    {
        if (i < frames_speed) rf_draw_rectangle(250 + 21 * i, 205, 20, 20, RF_RED);
        rf_draw_rectangle_outline((rf_rec){ 250 + 21 * i, 205, 20, 20 }, 1, RF_MAROON);
    }

    rf_draw_texture_region(scarfy, frame_rec, (rf_rec) { position.x, position.y, frame_rec.width, frame_rec.height }, (rf_vec2){ 0.0f, 0.0f }, 0.0f, RF_WHITE);  // Draw part of the texture

    rf_draw_text("(c) Scarfy sprite by Eiden Marsal", screen_width - 200, screen_height - 20, 10, RF_GRAY);

    rf_end();
}