//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "game.h"
#include "glad/glad.h"
#include "stdio.h"

game_data_t* global_game_data;

extern void game_init(game_data_t* game_data)
{
    //Reset game data pointer
    global_game_data = game_data;

    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&game_data->rf_ctx, &game_data->rf_mem, global_game_data->screen_width, global_game_data->screen_height, RF_DEFAULT_OPENGL_PROCS);

}

extern void game_refresh(game_data_t* game_data)
{
    //Load opengl with glad
    gladLoadGL();

    global_game_data = game_data;
    global_game_data->reload_count++;
    rf_set_global_context_pointer(&global_game_data->rf_ctx);
}

extern void game_update(void)
{
    const int screen_width = global_game_data->screen_width;

    rf_begin();

        rf_clear(RF_RAYWHITE);

        if (global_game_data->reload_count > 0)
        {
            char temp_str[512];
            snprintf(temp_str, 512, "Press `R` to reload\nCode reloaded %d times", global_game_data->reload_count);
            rf_draw_text(temp_str, 20, 20, 20, RF_DARKGRAY);
        }
        else
        {
            rf_draw_text("Press `R` to reload", 20, 20, 20, RF_DARKGRAY);
        }

        rf_draw_circle(screen_width / 4, 120, 35, RF_DARKBLUE);

        rf_draw_rectangle(screen_width / 4 * 2 - 60, 100, 120, 60, RF_RED);
        rf_draw_rectangle_outline((rf_rec) { screen_width / 4 * 2 - 40, 320, 80, 60 }, 1, RF_ORANGE);
        rf_draw_rectangle_gradient_h(screen_width / 4 * 2 - 90, 170, 180, 130, RF_MAROON, RF_GOLD);

        rf_draw_triangle((rf_vec2){(float) screen_width / 4 * 3, 80},
                         (rf_vec2){(float) screen_width / 4 * 3 - 60, 150},
                         (rf_vec2){(float) screen_width / 4 * 3 + 60, 150}, RF_VIOLET);

        rf_draw_poly((rf_vec2){(float)screen_width / 4.0f * 3.0f, 320}, 6, 80, 0, RF_BROWN);

        rf_draw_circle_gradient(screen_width / 4, 220, 60, RF_GREEN, RF_SKYBLUE);

        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        rf_draw_line(18, 42, screen_width - 18, 42, RF_BLACK);
        rf_draw_circle_lines(screen_width / 4, 340, 80, RF_DARKBLUE);
        rf_draw_triangle_lines(
                (rf_vec2){(float)screen_width / 4 * 3, 160},
                (rf_vec2){(float)screen_width / 4 * 3 - 20, 230},
                (rf_vec2){(float)screen_width / 4 * 3 + 20, 230}, RF_DARKBLUE);

    rf_end();
}