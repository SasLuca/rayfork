//Implementation of the geometric shapes example from raylib using rayfork

#include "rayfork.h"
#include "glad/glad.h"
#include "sokol_app.h"

rf_context rf_ctx;
rf_renderer_memory_buffers rf_mem;

int screen_width  = 800;
int screen_height = 450;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, screen_width, screen_height, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR);
}

void on_frame(void)
{
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        const char* text      = "Congrats! You created your first window!";
        const rf_sizef size   = rf_measure_text(rf_get_default_font(), text, strlen(text), 20, 1);
        const rf_vec2 pos     = (rf_vec2) { screen_width / 2 - size.width / 2, screen_height / 2 - size.height / 2 }; // Center the text

        rf_draw_text(text, pos.x, pos.y, 20, RF_LIGHTGRAY);
    }
    rf_end();
}

void on_resize(int width, int height)
{
    screen_width = width;
    screen_height = height;

    rf_set_viewport(screen_width, screen_height);
}