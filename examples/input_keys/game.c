//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "rayfork_renderer.h"
#include "game.h"

rf_context rf_ctx;

rf_vector2 ball_position = { (float) screen_width / 2, (float) screen_height / 2 };

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, screen_width, screen_height);
    rf_set_target_fps(60);
    rf_load_font_default();
}

void on_frame(const input_data input)
{
    //Update
    if (input.right_pressed) ball_position.x += 2.0f;
    if (input.left_pressed)  ball_position.x -= 2.0f;
    if (input.up_pressed)    ball_position.y -= 2.0f;
    if (input.down_pressed)  ball_position.y += 2.0f;

    //Render
    rf_begin_drawing();

    rf_clear_background(rf_raywhite);

    rf_draw_text("move the ball with arrow keys", 10, 10, 20, rf_darkgray);

    rf_draw_circle_v(ball_position, 50, rf_maroon);

    rf_end_drawing();
}