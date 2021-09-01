#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#define SOKOL_WIN32_FORCE_MAIN
#include "glad/glad.h"
#include "sokol_app/sokol_app.h"
#include "sokol_time/sokol_time.h"
#include "rayfork/rayfork.h"
#include "platform-common/platform.h"

//#ifdef RAYFORK_PLATFORM_MACOS
//int gladLoadGL(void);
//#else
//#include "glad/glad.h"
//#endif

static platform_input_state input_state;
static uint64_t time_begin;
static float delta_time = 0.16f;

static void sokol_on_init(void)
{
    gladLoadGL();

    stm_setup();
    time_begin = 0;
    delta_time = 0.16f;

    game_init(rf_default_gfx_backend_init_data);
}

static void sokol_on_frame(void)
{
    game_update(&input_state, delta_time);

    for (int i = 0; i < sizeof(input_state.keys) / sizeof(input_state.keys[0]); i++)
    {
        if (input_state.keys[i] == KEY_RELEASE)
        {
            input_state.keys[i] = KEY_DEFAULT_STATE;
        }

        if (input_state.keys[i] == KEY_PRESSED_DOWN)
        {
            input_state.keys[i] = KEY_HOLD_DOWN;
        }
    }

    input_state.any_key_pressed = false;

    if (input_state.mouse_scroll_y != 0) input_state.mouse_scroll_y = 0;
    if (input_state.left_mouse_btn  == BTN_RELEASE)      input_state.left_mouse_btn  = BTN_DEFAULT_STATE;
    if (input_state.left_mouse_btn  == BTN_PRESSED_DOWN) input_state.left_mouse_btn  = BTN_HOLD_DOWN;
    if (input_state.right_mouse_btn == BTN_RELEASE)      input_state.right_mouse_btn = BTN_DEFAULT_STATE;
    if (input_state.right_mouse_btn == BTN_PRESSED_DOWN) input_state.right_mouse_btn = BTN_HOLD_DOWN;

    delta_time = stm_diff(stm_now(), time_begin);
}

static void sokol_on_event(const sapp_event* event)
{
    switch (event->type)
    {
        case SAPP_EVENTTYPE_RESIZED:
            window.width  = event->window_width;
            window.height = event->window_height;
            rf_set_viewport(window.width, window.height);
            break;

        case SAPP_EVENTTYPE_MOUSE_MOVE:
            input_state.mouse_x = event->mouse_x;
            input_state.mouse_y = event->mouse_y;
            break;

        case SAPP_EVENTTYPE_KEY_DOWN:
            if (event->key_code == KEYCODE_ESCAPE) sapp_quit();

            if (input_state.keys[event->key_code] == KEY_DEFAULT_STATE)
            {
                input_state.keys[event->key_code] = KEY_PRESSED_DOWN;
                input_state.any_key_pressed = true;
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            input_state.keys[event->key_code] = KEY_RELEASE;
            break;

        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            input_state.mouse_scroll_y = event->scroll_y;
            break;

        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT  && input_state.left_mouse_btn  == BTN_DEFAULT_STATE) input_state.left_mouse_btn  = BTN_PRESSED_DOWN;
            if (event->mouse_button == SAPP_MOUSEBUTTON_RIGHT && input_state.right_mouse_btn == BTN_DEFAULT_STATE) input_state.right_mouse_btn = BTN_PRESSED_DOWN;
            break;

        case SAPP_EVENTTYPE_MOUSE_UP:
            if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT)  input_state.left_mouse_btn  = BTN_RELEASE;
            if (event->mouse_button == SAPP_MOUSEBUTTON_RIGHT) input_state.right_mouse_btn = BTN_RELEASE;
            break;

        default:
            break;
    }
}

sapp_desc sokol_main(int argc, char** argv)
{
    return (sapp_desc)
    {
        .window_title = window.title,
        .width        = window.width,
        .height       = window.height,
        .init_cb      = sokol_on_init,
        .frame_cb     = sokol_on_frame,
        .event_cb     = sokol_on_event,
    };
}