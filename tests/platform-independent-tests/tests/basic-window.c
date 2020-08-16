#include "platform.h"

const char* window_title = "rayfork - basic window";

int screen_width  = 800;
int screen_height = 450;

rf_context ctx;
rf_render_batch batch;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_init_context(&ctx);
    rf_init_gfx(screen_width, screen_height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&batch);
}

extern void game_update(const input_t* input)
{
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        char*    text = "Congrats! You created your first window!";
        rf_sizef size = rf_measure_text(rf_get_default_font(), text, 20, 2);
        rf_vec2  pos  = rf_center_to_screen(size.width, size.height);
        rf_draw_text(text, pos.x, pos.y, 20, RF_BLACK);
    }
    rf_end();
}

extern void game_window_resize(int width, int height)
{
    screen_width  = width;
    screen_height = height;

    rf_set_viewport(screen_width, screen_height);
}