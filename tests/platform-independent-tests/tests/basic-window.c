#include "platform-common/platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - basic window"
};

rf_gfx_context ctx;
rf_render_batch batch;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_gfx_init(&ctx, window.width, window.height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(rf_default_allocator);
    rf_set_active_render_batch(&batch);
}

extern void game_update(const platform_input_state* input, float delta)
{
    rf_begin();
    {
        rf_clear(rf_raywhite);

        char*    text = "Congrats! You created your first window!";
        rf_sizef size = rf_measure_text(rf_get_builtin_bitmap_font(), text, 20, 2);
        rf_vec2  pos  = rf_center_to_screen(size.width, size.height);
        rf_draw_text(text, pos.x, pos.y, 20, rf_black);
    }
    rf_end();
}