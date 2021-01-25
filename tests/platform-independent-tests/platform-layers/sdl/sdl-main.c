#include "SDL.h"
#include "glad/glad.h"
#include "platform.h"

platform_input_state input_state;

static int sdl_key(SDL_Event event)
{
    switch (event.key.keysym.sym)
    {
        default: return KEYCODE_INVALID;
        case SDLK_SPACE: return KEYCODE_SPACE;
        case SDLK_QUOTE: return KEYCODE_APOSTROPHE;
        case SDLK_COMMA: return KEYCODE_COMMA;
        case SDLK_MINUS: return KEYCODE_MINUS;
        case SDLK_PERIOD: return KEYCODE_PERIOD;
        case SDLK_SLASH: return KEYCODE_SLASH;
        case SDLK_0: return KEYCODE_0;
        case SDLK_1: return KEYCODE_1;
        case SDLK_2: return KEYCODE_2;
        case SDLK_3: return KEYCODE_3;
        case SDLK_4: return KEYCODE_4;
        case SDLK_5: return KEYCODE_5;
        case SDLK_6: return KEYCODE_6;
        case SDLK_7: return KEYCODE_7;
        case SDLK_8: return KEYCODE_8;
        case SDLK_9: return KEYCODE_9;
        case SDLK_SEMICOLON: return KEYCODE_SEMICOLON;
        case SDLK_EQUALS: return KEYCODE_EQUAL;
        case SDLK_a: return KEYCODE_A;
        case SDLK_b: return KEYCODE_B;
        case SDLK_c: return KEYCODE_C;
        case SDLK_d: return KEYCODE_D;
        case SDLK_e: return KEYCODE_E;
        case SDLK_f: return KEYCODE_F;
        case SDLK_g: return KEYCODE_G;
        case SDLK_h: return KEYCODE_H;
        case SDLK_i: return KEYCODE_I;
        case SDLK_j: return KEYCODE_J;
        case SDLK_k: return KEYCODE_K;
        case SDLK_l: return KEYCODE_L;
        case SDLK_m: return KEYCODE_M;
        case SDLK_n: return KEYCODE_N;
        case SDLK_o: return KEYCODE_O;
        case SDLK_p: return KEYCODE_P;
        case SDLK_q: return KEYCODE_Q;
        case SDLK_r: return KEYCODE_R;
        case SDLK_s: return KEYCODE_S;
        case SDLK_t: return KEYCODE_T;
        case SDLK_u: return KEYCODE_U;
        case SDLK_v: return KEYCODE_V;
        case SDLK_w: return KEYCODE_W;
        case SDLK_x: return KEYCODE_X;
        case SDLK_y: return KEYCODE_Y;
        case SDLK_z: return KEYCODE_Z;
        case SDLK_LEFTBRACKET: return KEYCODE_LEFT_BRACKET;
        case SDLK_BACKSLASH: return KEYCODE_BACKSLASH;
        case SDLK_RIGHTBRACKET: return KEYCODE_RIGHT_BRACKET;
        case SDLK_BACKQUOTE: return KEYCODE_GRAVE_ACCENT;
        case SDLK_ESCAPE: return KEYCODE_ESCAPE;
        case SDL_SCANCODE_RETURN: return KEYCODE_ENTER;
        case SDLK_TAB: return KEYCODE_TAB;
        case SDLK_BACKSPACE: return KEYCODE_BACKSPACE;
        case SDLK_INSERT: return KEYCODE_INSERT;
        case SDLK_DELETE: return KEYCODE_DELETE;
        case SDLK_RIGHT: return KEYCODE_RIGHT;
        case SDLK_LEFT: return KEYCODE_LEFT;
        case SDLK_DOWN: return KEYCODE_DOWN;
        case SDLK_UP: return KEYCODE_UP;
        case SDLK_PAGEUP: return KEYCODE_PAGE_UP;
        case SDLK_PAGEDOWN: return KEYCODE_PAGE_DOWN;
        case SDLK_HOME: return KEYCODE_HOME;
        case SDLK_END: return KEYCODE_END;
        case SDLK_CAPSLOCK: return KEYCODE_CAPS_LOCK;
        case SDLK_SCROLLLOCK: return KEYCODE_SCROLL_LOCK;
        case SDLK_NUMLOCKCLEAR: return KEYCODE_NUM_LOCK;
        case SDLK_PRINTSCREEN: return KEYCODE_PRINT_SCREEN;
        case SDLK_PAUSE: return KEYCODE_PAUSE;
        case SDLK_F1: return KEYCODE_F1;
        case SDLK_F2: return KEYCODE_F2;
        case SDLK_F3: return KEYCODE_F3;
        case SDLK_F4: return KEYCODE_F4;
        case SDLK_F5: return KEYCODE_F5;
        case SDLK_F6: return KEYCODE_F6;
        case SDLK_F7: return KEYCODE_F7;
        case SDLK_F8: return KEYCODE_F8;
        case SDLK_F9: return KEYCODE_F9;
        case SDLK_F10: return KEYCODE_F10;
        case SDLK_F11: return KEYCODE_F11;
        case SDLK_F12: return KEYCODE_F12;
        case SDLK_F13: return KEYCODE_F13;
        case SDLK_F14: return KEYCODE_F14;
        case SDLK_F15: return KEYCODE_F15;
        case SDLK_F16: return KEYCODE_F16;
        case SDLK_F17: return KEYCODE_F17;
        case SDLK_F18: return KEYCODE_F18;
        case SDLK_F19: return KEYCODE_F19;
        case SDLK_F20: return KEYCODE_F20;
        case SDLK_F21: return KEYCODE_F21;
        case SDLK_F22: return KEYCODE_F22;
        case SDLK_F23: return KEYCODE_F23;
        case SDLK_F24: return KEYCODE_F24;
        case SDLK_KP_0: return KEYCODE_KP_0;
        case SDLK_KP_1: return KEYCODE_KP_1;
        case SDLK_KP_2: return KEYCODE_KP_2;
        case SDLK_KP_3: return KEYCODE_KP_3;
        case SDLK_KP_4: return KEYCODE_KP_4;
        case SDLK_KP_5: return KEYCODE_KP_5;
        case SDLK_KP_6: return KEYCODE_KP_6;
        case SDLK_KP_7: return KEYCODE_KP_7;
        case SDLK_KP_8: return KEYCODE_KP_8;
        case SDLK_KP_9: return KEYCODE_KP_9;
        case SDLK_KP_DECIMAL: return KEYCODE_KP_DECIMAL;
        case SDLK_KP_DIVIDE: return KEYCODE_KP_DIVIDE;
        case SDLK_KP_MULTIPLY: return KEYCODE_KP_MULTIPLY;
        case SDLK_KP_MINUS: return KEYCODE_KP_SUBTRACT;
        case SDLK_KP_PLUS: return KEYCODE_KP_ADD;
        case SDLK_KP_ENTER: return KEYCODE_KP_ENTER;
        case SDLK_KP_EQUALS: return KEYCODE_KP_EQUAL;
        case SDLK_LSHIFT: return KEYCODE_LEFT_SHIFT;
        case SDLK_LCTRL: return KEYCODE_LEFT_CONTROL;
        case SDLK_LALT: return KEYCODE_LEFT_ALT;
        case SDLK_LGUI: return KEYCODE_LEFT_SUPER;
        case SDLK_RSHIFT: return KEYCODE_RIGHT_SHIFT;
        case SDLK_RCTRL: return KEYCODE_RIGHT_CONTROL;
        case SDLK_RALT: return KEYCODE_RIGHT_ALT;
        case SDLK_RGUI: return KEYCODE_RIGHT_SUPER;
        case SDLK_MENU: return KEYCODE_MENU;
    }
}

int main(int argc, char* argv[])
{
    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_LoadLibrary(NULL);

    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Also request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window* sdl_window = SDL_CreateWindow(window.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window.width, window.height, SDL_WINDOW_OPENGL);
	SDL_SetWindowResizable(sdl_window, SDL_TRUE);

	SDL_GLContext gl_ctx = SDL_GL_CreateContext(sdl_window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	SDL_GL_SetSwapInterval(1); // Use v-sync

	SDL_Event event = {0};

	game_init(rf_default_gfx_backend_data);

    bool run = true;
	while (run)
	{
	    SDL_GL_SwapWindow(sdl_window);

	    // SDL event handling
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT ||
			   (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdl_window)) ||
			   (event.type == SDL_KEYDOWN && sdl_key(event) == KEYCODE_ESCAPE))
			{
			    run = false;
			}

            if (event.type == SDL_KEYDOWN && input_state.keys[sdl_key(event)] == KEY_DEFAULT_STATE)
            {
                input_state.keys[sdl_key(event)] = KEY_PRESSED_DOWN;
                input_state.any_key_pressed = true;
            }

            if (event.type == SDL_KEYUP)
            {
                input_state.keys[sdl_key(event)] = KEY_RELEASE;
            }

			if (event.type == SDL_MOUSEWHEEL)
            {
                input_state.mouse_scroll_y = event.wheel.y;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT  && input_state.left_mouse_btn  == BTN_DEFAULT_STATE) input_state.left_mouse_btn  = BTN_PRESSED_DOWN;
                if (event.button.button == SDL_BUTTON_RIGHT && input_state.right_mouse_btn == BTN_DEFAULT_STATE) input_state.right_mouse_btn = BTN_PRESSED_DOWN;
            }

            if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT ) input_state.left_mouse_btn  = BTN_RELEASE;
                if (event.button.button == SDL_BUTTON_RIGHT) input_state.right_mouse_btn = BTN_RELEASE;
            }

            if(event.type == SDL_MOUSEMOTION)
            {
                SDL_GetMouseState(&input_state.mouse_x, &input_state.mouse_y);
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                SDL_GetWindowSize(sdl_window, &window.width, &window.height);

                rf_set_viewport(window.width, window.height);
            }
		}

        // Game Update
		game_update(&input_state);

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
	}

	SDL_DestroyWindow(sdl_window);
	SDL_Quit();

	return 0;
}