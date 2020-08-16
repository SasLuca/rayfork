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

    global_input_data.key_count = 0; // reset number of key pressed everyframe

    //single frame input
    if (global_input_data.backspace_pressed) global_input_data.backspace_down = 0;
    // reset key_q
    for (int i = 0; i < global_input_data.key_count; i++) global_input_data.char_key_q[i] = 0;
}

void platform_on_event(const sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        if (event->key_code >= SAPP_KEYCODE_SPACE && event->key_code <= SAPP_KEYCODE_GRAVE_ACCENT)
        {
           global_input_data.char_key_q[global_input_data.key_count] = event->key_code + (global_input_data.shift_pressed ? 0 : 32);
           global_input_data.key_count++;
        }

        if (event->key_code == SAPP_KEYCODE_BACKSPACE)
        {
            global_input_data.backspace_down = 1;
            global_input_data.backspace_pressed = 1;
        }

        if (event->key_code == SAPP_KEYCODE_LEFT_SHIFT || event->key_code == SAPP_KEYCODE_RIGHT_SHIFT)
        {
            global_input_data.shift_pressed = 1;
        }
    }

    if (event->type == SAPP_EVENTTYPE_KEY_UP)
    {
        if (event->key_code == SAPP_KEYCODE_BACKSPACE)
        {
            global_input_data.backspace_down = 0;
            global_input_data.backspace_pressed = 0;
        }

        if (event->key_code == SAPP_KEYCODE_LEFT_SHIFT || event->key_code == SAPP_KEYCODE_RIGHT_SHIFT)
        {
            global_input_data.shift_pressed = 0;
        }
    }

    if (event->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
        global_input_data.mouse_x = event->mouse_x;
        global_input_data.mouse_y = event->mouse_y;
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
        .window_title = "rayfork [textures] example - input box",
    };
}