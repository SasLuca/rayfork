//In this file we only initialise the window using sokol_app

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#include "sokol_app.h"
#include "game.h"

input_data global_input_data;



void platform_on_frame()
{
    on_frame(global_input_data);

    // single frame input
    if (global_input_data.space_pressed) global_input_data.space_down = 0;
}

void platform_on_event(const sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
        global_input_data.mouse_x = event->mouse_x;
        global_input_data.mouse_y = event->mouse_y;
    }

    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        if (event->key_code == SAPP_KEYCODE_SPACE && !global_input_data.space_pressed)
        {
            global_input_data.space_pressed = 1;
            global_input_data.space_down = 1;
        }
    }

    if (event->type == SAPP_EVENTTYPE_KEY_UP)
    {
        if (event->key_code == SAPP_KEYCODE_SPACE)
        {
            global_input_data.space_pressed = 0;
            global_input_data.space_down = 0;
        }
    }
}

sapp_desc sokol_main(int argc, char** argv) 
{
    return (sapp_desc) 
    {
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .init_cb = on_init,
        .frame_cb = platform_on_frame,
        .event_cb = platform_on_event,
        .window_title = "rayfork [textures] example - particles blending",
    };
}