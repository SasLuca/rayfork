#include "stdio.h"
#include "platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - texture rectangle"
};

rf_gfx_context ctx;
rf_render_batch batch;

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED  1

rf_texture2d scarfy;
rf_vec2 position = { 350.0f, 280.0f };
rf_rec frame_rec;

int current_frame;
int frames_counter;
int frames_speed = 8;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_gfx_init(&ctx, window.width, window.height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&batch);

    scarfy = rf_load_texture_from_file(ASSETS_PATH"scarfy.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);
    frame_rec = (rf_rec) {
        .width = scarfy.width / 6,
        .height = scarfy.height
    };
}

extern void game_update(const platform_input_state* input)
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

    if (input->keys[KEYCODE_RIGHT] & BTN_PRESSED_DOWN)     frames_speed++;
    else if (input->keys[KEYCODE_LEFT] & BTN_PRESSED_DOWN) frames_speed--;

    if (frames_speed > MAX_FRAME_SPEED) frames_speed = MAX_FRAME_SPEED;
    else if (frames_speed < MIN_FRAME_SPEED) frames_speed = MIN_FRAME_SPEED;
    // Draw
    rf_begin();

    rf_clear(RF_RAYWHITE);

    rf_draw_texture(scarfy, 15, 40, RF_WHITE);
    rf_draw_rectangle_outline((rf_rec){ 15, 40, scarfy.width, scarfy.height }, 1, RF_LIME);
    rf_draw_rectangle_outline((rf_rec){ 15 + frame_rec.x, 40 + frame_rec.y, frame_rec.width, frame_rec.height }, 1, RF_RED);

    rf_draw_text("FRAME SPEED: ", 165, 210, 10, RF_DARKGRAY);

    char text[1024] = {0};
    snprintf(text, sizeof(text), "%02i FPS", frames_speed);
    rf_draw_text(text, 575, 210, 10, RF_DARKGRAY);

    rf_draw_text("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, RF_DARKGRAY);

    for (int i = 0; i < MAX_FRAME_SPEED; i++)
    {
        if (i < frames_speed) rf_draw_rectangle(250 + 21 * i, 205, 20, 20, RF_RED);
        rf_draw_rectangle_outline((rf_rec){ 250 + 21 * i, 205, 20, 20 }, 1, RF_MAROON);
    }

    rf_draw_texture_region(scarfy, frame_rec, (rf_rec) { position.x, position.y, frame_rec.width, frame_rec.height }, (rf_vec2){ 0.0f, 0.0f }, 0.0f, RF_WHITE);  // Draw part of the texture

    rf_draw_text("(c) Scarfy sprite by Eiden Marsal", window.width - 200, window.height - 20, 10, RF_GRAY);

    rf_end();
}