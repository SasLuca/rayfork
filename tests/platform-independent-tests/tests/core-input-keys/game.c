//Implementation of the input keys example from raylib using rayfork

#include "include/rayfork.h"
#include "glad/glad.h"
#include "game.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

const int screen_width = 800;
const int screen_height = 450;

rf_vec2 ball_position = {(float) screen_width / 2, (float) screen_height / 2 };

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, screen_width, screen_height, RF_DEFAULT_OPENGL_PROCS);
}

void on_frame(const input_data input)
{
    //Update
    if (input.right_pressed) ball_position.x += 2.0f;
    if (input.left_pressed)  ball_position.x -= 2.0f;
    if (input.up_pressed)    ball_position.y -= 2.0f;
    if (input.down_pressed)  ball_position.y += 2.0f;

    //Render
    rf_begin();

    rf_clear(RF_RAYWHITE);

    rf_draw_text("move the ball with arrow keys", 10, 10, 20, RF_DARKGRAY);

    rf_draw_circle_v(ball_position, 50, RF_MAROON);

    rf_end();
}