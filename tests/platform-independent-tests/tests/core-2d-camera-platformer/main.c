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
    // keyboard
    if (global_input_data.c_pressed) global_input_data.c_down = 0;
    if (global_input_data.r_pressed) global_input_data.r_down = 0;

    // mouse
    if (global_input_data.mouse_scroll_y != 0) global_input_data.mouse_scroll_y = 0;
}

void platform_on_event(const sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        if (event->key_code == SAPP_KEYCODE_RIGHT) global_input_data.right_pressed = 1;
        if (event->key_code == SAPP_KEYCODE_LEFT) global_input_data.left_pressed = 1;
        if (event->key_code == SAPP_KEYCODE_SPACE) global_input_data.space_pressed = 1;

        if (event->key_code == SAPP_KEYCODE_C && !global_input_data.c_pressed){
            global_input_data.c_pressed = 1;
            global_input_data.c_down = 1;
        }

        if (event->key_code == SAPP_KEYCODE_R && !global_input_data.r_pressed)
        {
            global_input_data.r_pressed = 1;
            global_input_data.r_down = 1;
        }
    }

    if (event->type == SAPP_EVENTTYPE_KEY_UP)
    {
        if (event->key_code == SAPP_KEYCODE_RIGHT) global_input_data.right_pressed = 0;
        if (event->key_code == SAPP_KEYCODE_LEFT) global_input_data.left_pressed = 0;
        if (event->key_code == SAPP_KEYCODE_SPACE) global_input_data.space_pressed = 0;

        if (event->key_code == SAPP_KEYCODE_C)
        {
            global_input_data.c_pressed = 0;
            global_input_data.c_down = 0;
        }

        if (event->key_code == SAPP_KEYCODE_R)
        {
            global_input_data.r_pressed = 0;
            global_input_data.r_down = 0;
        }
    }

    if (event->type == SAPP_EVENTTYPE_MOUSE_SCROLL)
    {
        global_input_data.mouse_scroll_y = event->scroll_y;
    }

}

sapp_desc sokol_main(int argc, char** argv) 
{
    return (sapp_desc) 
    {
        .width = 800,
        .height = 450,
        .init_cb = on_init,
        .frame_cb = platform_on_frame,
        .event_cb = platform_on_event,
        .window_title = "raylib [core] example - 2d camera platformer",
    };
}