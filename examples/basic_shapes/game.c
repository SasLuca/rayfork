//Implementation of the geometric shapes example from raylib using rayfork

#define RAYFORK_IMPLEMENTATION
#define RAYFORK_GRAPHICS_BACKEND_OPENGL_33
#define RAYFORK_MATH_IMPLEMENTATION
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork.h"

rf_context rf_ctx;
rf_memory  rf_mem;

const int screen_width = 800;
const int screen_height = 450;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_renderer_init_context(&rf_ctx, &rf_mem, screen_width, screen_height);
}

void on_frame(void)
{
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_text_cstr(rf_get_default_font(), "some basic shapes available on raylib", (rf_vec2) {20, 20 }, 20, 1, RF_DARKGRAY);

        rf_draw_circle(screen_width / 4, 120, 35, RF_DARKBLUE);

        rf_draw_rectangle((rf_rec) {screen_width / 4 * 2 - 60, 100, 120, 60 }, RF_VEC2_ZERO, 0, RF_RED);
        rf_draw_rectangle_outline((rf_rec) {screen_width / 4 * 2 - 40, 320, 80, 60 }, 0, RF_ORANGE);  // NOTE: Uses QUADS internally, not lines
        rf_draw_rectangle_gradient_h(screen_width / 4 * 2 - 90, 170, 180, 130, RF_MAROON, RF_GOLD);
        rf_draw_triangle((rf_vec2) {screen_width / 4 * 3, 80 },
                         (rf_vec2) {screen_width / 4 * 3 - 60, 150 },
                         (rf_vec2) {screen_width / 4 * 3 + 60, 150 }, RF_VIOLET);

        rf_draw_poly((rf_vec2) {screen_width / 4.0f * 3.0f, 320 }, 6, 80, 0, RF_BROWN);

        rf_draw_circle_gradient(screen_width / 4, 220, 60, RF_GREEN, RF_SKYBLUE);


        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        rf_draw_line((rf_vec2) { 18, 42 }, (rf_vec2) {screen_width - 18, 42 }, 3, RF_BLACK);

        rf_draw_circle_lines(screen_width / 4, 340, 80, RF_DARKBLUE);

        rf_draw_triangle_lines(
                (rf_vec2) {screen_width / 4 * 3, 160 },
                (rf_vec2) {screen_width / 4 * 3 - 20, 230 },
                (rf_vec2) {screen_width / 4 * 3 + 20, 230 },
                RF_DARKBLUE);

    rf_end();
}