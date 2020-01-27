//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork_renderer.h"

rf_renderer_context rf_ctx;

const int screen_width = 800;
const int screen_height = 450;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, screen_width, screen_height);
    rf_set_target_fps(60);
    rf_load_font_default();
}

void on_frame(void)
{
    rf_begin_drawing();

    rf_clear_background(rf_raywhite);

    rf_draw_text("Congrats! You created your first window!", 190, 200, 20, rf_lightgray);

    rf_end_drawing();
}