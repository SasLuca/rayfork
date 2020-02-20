//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "include/old_rayfork.h"

rf_context rf_ctx;
rf_memory rf_memory;

int screen_width = 800;
int screen_height = 450;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_renderer_init_context(&rf_ctx, &rf_memory, screen_width, screen_height);
}

void on_frame(void)
{
    rf_begin();

    rf_clear(RF_RAYWHITE);

    const char* text      = "Congrats! You created your first window!";
    const rf_sizef size   = rf_measure_text(rf_get_default_font(), text, strlen(text), 20, 1);
    const rf_vec2 pos     = (rf_vec2) { screen_width / 2 - size.width / 2, screen_height / 2 - size.height / 2 }; // Center the text

    rf_draw_text_cstr(rf_get_default_font(), text, pos, 20, 1, RF_LIGHTGRAY);

    rf_end();
}

void on_resize(int width, int height)
{
    screen_width = width;
    screen_height = height;

    rf_set_viewport(screen_width, screen_height);
}