//In this file we only initialise the window using sokol_app

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER

#include "sokol_app.h"
#include "game.h"

//Functions we call from the dll
typedef void (*game_init_func_t)(game_data_t* game_data);
typedef void (*game_refresh_func_t)(game_data_t* game_data);
typedef void (*game_update_func_t)(void);

//Stubs for when we the dll functions are not loaded
void game_init_stub(game_data_t* it) {}
void game_refresh_stub(game_data_t* it) {}
void game_update_stub(void) {}

typedef struct win32_game_code win32_game_code;
struct win32_game_code
{
    HMODULE  game_code_dll;

    game_init_func_t game_init;
    game_refresh_func_t game_refresh;
    game_update_func_t game_update;

    bool valid;
};

win32_game_code game_code;
game_data_t game;

win32_game_code win32_load_game_code()
{
    win32_game_code result = {0};

    assert(CopyFile("./libhot-code-reloading-dynamic.dll", "game.dll", FALSE));

    result.game_code_dll = LoadLibraryA("game.dll");
    assert(result.game_code_dll);
    result.game_init = (game_init_func_t) GetProcAddress(result.game_code_dll, "game_init");
    result.game_update = (game_update_func_t) GetProcAddress(result.game_code_dll, "game_update");
    result.game_refresh = (game_refresh_func_t) GetProcAddress(result.game_code_dll, "game_refresh");
    assert(result.game_init && result.game_update && result.game_refresh);

    result.valid = true;

    return result;
}

void win32_unload_game_code()
{
    if (game_code.game_code_dll)
    {
        FreeLibrary(game_code.game_code_dll);
        game_code.game_code_dll = NULL;
    }

    game_code.valid = false;
    game_code.game_init = game_init_stub;
    game_code.game_update = game_update_stub;
    game_code.game_refresh = game_refresh_stub;
}

void on_init(void)
{
    game_code = win32_load_game_code();
    assert(game_code.valid);
    game.alloc = malloc;
    game.free = free;
    game.screen_width = 800;
    game.screen_height = 450;
    game_code.game_init(&game);
}

void on_frame(void)
{
    if (game_code.valid)
    {
        game_code.game_update();
    }
}

void on_event(const sapp_event* event)
{
    // When the user presses R we try to hot reload the code.
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN && event->key_code == SAPP_KEYCODE_R)
    {
        win32_unload_game_code();

        game_code = win32_load_game_code();
        game_code.game_refresh(&game);
    }
}

void on_cleanup(void)
{

}

sapp_desc sokol_main(int argc, char** argv) 
{
    return (sapp_desc) 
    {
        .width      = 800,
        .height     = 450,
        .init_cb    = on_init,
        .frame_cb   = on_frame,
        .cleanup_cb = on_cleanup,
        .event_cb   = on_event,
    };
}