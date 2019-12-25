//Implementation of the geometric shapes example from raylib using rayfork

#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork.h"

rf_context rf_ctx;
const int screen_width = 800;
const int screen_height = 450;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, screen_width, screen_height);
    rf_set_target_fps(&rf_ctx, 60);
    rf_load_font_default(&rf_ctx);
}

void on_frame(void)
{
    rf_begin_drawing(&rf_ctx);

        rf_clear_background(rf_raywhite);

        rf_draw_text(&rf_ctx, "some basic shapes available on raylib", 20, 20, 20, rf_darkgray);

        rf_draw_circle(&rf_ctx, screen_width / 4, 120, 35, rf_darkblue);

        rf_draw_rectangle(&rf_ctx, screen_width / 4 * 2 - 60, 100, 120, 60, rf_red);
        rf_draw_rectangle_lines(&rf_ctx, screen_width / 4 * 2 - 40, 320, 80, 60, rf_orange);  // NOTE: Uses QUADS internally, not lines
        rf_draw_rectangle_gradient_h(&rf_ctx, screen_width / 4 * 2 - 90, 170, 180, 130, rf_maroon, rf_gold);

        rf_draw_triangle(&rf_ctx, (rf_vector2){(float) screen_width / 4 * 3, 80},
                         (rf_vector2){(float) screen_width / 4 * 3 - 60, 150},
                         (rf_vector2){(float) screen_width / 4 * 3 + 60, 150}, rf_violet);

        rf_draw_poly(&rf_ctx, (rf_vector2){(float)screen_width / 4.0f * 3.0f, 320}, 6, 80, 0, rf_brown);

        rf_draw_circle_gradient(&rf_ctx, screen_width / 4, 220, 60, rf_green, rf_skyblue);

        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        rf_draw_line(&rf_ctx, 18, 42, screen_width - 18, 42, rf_black);
        rf_draw_circle_lines(&rf_ctx, screen_width / 4, 340, 80, rf_darkblue);
        rf_draw_triangle_lines(&rf_ctx,
                               (rf_vector2){(float)screen_width / 4 * 3, 160},
                               (rf_vector2){(float)screen_width / 4 * 3 - 20, 230},
                               (rf_vector2){(float)screen_width / 4 * 3 + 20, 230}, rf_darkblue);

    rf_end_drawing(&rf_ctx);
}

void on_cleanup(void)
{
    //Empty
}

void on_event(const sapp_event* event)
{
    //Empty
}