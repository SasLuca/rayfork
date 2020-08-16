//In this file we only initialise the window using sokol_app

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#include "sokol_app.h"
#include "game.h"

input_data global_input_data;
float prev_scroll_y = 0;


void platform_on_frame()
{
    on_frame(global_input_data);

    // used for single frame input

    // keyboard
    if (global_input_data.left_pressed) global_input_data.left_down = 0;
    if (global_input_data.right_pressed) global_input_data.right_down = 0;
    if (global_input_data.c_pressed) global_input_data.c_down = 0;
    if (global_input_data.s_pressed) global_input_data.s_down = 0;

    //mosue
    if (global_input_data.mouse_left_pressed) global_input_data.mouse_left_down = 0;
    if (global_input_data.mouse_right_pressed) global_input_data.mouse_right_down = 0;
    if (global_input_data.mouse_left_up) global_input_data.mouse_left_up = 0;
    if (global_input_data.mouse_right_up) global_input_data.mouse_right_up = 0;
    if (global_input_data.scroll_y != 0) global_input_data.scroll_y = 0;
}

void platform_on_event(const sapp_event* event)
{
    // keyboard
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        if (event->key_code == SAPP_KEYCODE_LEFT && !global_input_data.left_pressed)
        {
            global_input_data.left_down = 1;
            global_input_data.left_pressed = 1;
        }

        if (event->key_code == SAPP_KEYCODE_RIGHT && !global_input_data.right_pressed)
        {
            global_input_data.right_down = 1;
            global_input_data.right_pressed = 1;
        }

        if (event->key_code == SAPP_KEYCODE_C && !global_input_data.c_pressed)
        {
            global_input_data.c_down = 1;
            global_input_data.c_pressed = 1;
        }

        if (event->key_code == SAPP_KEYCODE_S && !global_input_data.s_pressed)
        {
            global_input_data.s_down = 1;
            global_input_data.s_pressed = 1;
        }
    }

    if (event->type == SAPP_EVENTTYPE_KEY_UP)
    {
        if (event->key_code == SAPP_KEYCODE_LEFT)
        {
            global_input_data.left_down = 0;
            global_input_data.left_pressed = 0;
        }

        if (event->key_code == SAPP_KEYCODE_RIGHT)
        {
            global_input_data.right_down = 0;
            global_input_data.right_pressed = 0;
        }

        if (event->key_code == SAPP_KEYCODE_S)
        {
            global_input_data.s_down = 0;
            global_input_data.s_pressed = 0;
        }

        if (event->key_code == SAPP_KEYCODE_C)
        {
            global_input_data.c_down = 0;
            global_input_data.c_pressed = 0;
        }
    }

    // mouse
    if (event->type == SAPP_EVENTTYPE_MOUSE_DOWN)
    {
        if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT && !global_input_data.mouse_left_pressed)
        {
            global_input_data.mouse_left_down = 1;
            global_input_data.mouse_left_pressed = 1;
            global_input_data.mouse_left_up = 0;
        }

        if (event->mouse_button == SAPP_MOUSEBUTTON_RIGHT && !global_input_data.mouse_right_pressed)
        {
            global_input_data.mouse_right_down = 1;
            global_input_data.mouse_right_pressed = 1;
            global_input_data.mouse_right_down = 0;
        }
    }

    if (event->type == SAPP_EVENTTYPE_MOUSE_UP)
    {
        if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT)
        {
            global_input_data.mouse_left_down = 0;
            global_input_data.mouse_left_pressed = 0;
            global_input_data.mouse_left_up = 1;
        }

        if (event->mouse_button == SAPP_MOUSEBUTTON_RIGHT)
        {
            global_input_data.mouse_right_down = 0;
            global_input_data.mouse_right_pressed = 0;
            global_input_data.mouse_right_down = 1;
        }
    }

    if (event->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
        global_input_data.mouse_x = event->mouse_x;
        global_input_data.mouse_y = event->mouse_y;
    }

    if (event->type == SAPP_EVENTTYPE_MOUSE_SCROLL)
    {
        global_input_data.scroll_y = event->scroll_y;
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
        .window_title = "rayfork [textures] example - mouse painting",
    };
}