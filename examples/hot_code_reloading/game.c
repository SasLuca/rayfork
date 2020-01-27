//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "game.h"
#include "stdio.h"

game_data* global_game_data;

extern void game_init(game_data* game_data)
{
    //Reset game data pointer
    global_game_data = game_data;

    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&game_data->rf_ctx, global_game_data->screen_width, global_game_data->screen_height);
    rf_set_target_fps(60);
    rf_load_font_default();
}

extern void game_refresh(game_data* game_data)
{
    //Load opengl with glad
    gladLoadGL();

    global_game_data = game_data;
    global_game_data->reload_count++;
    rf_set_global_context_ptr(&global_game_data->rf_ctx);
}

extern void game_update(void)
{
    const int screen_width = global_game_data->screen_width;

    rf_begin_drawing();

        rf_clear_background(rf_raywhite);

        if (global_game_data->reload_count > 0)
        {
            char temp_str[512];
            snprintf(temp_str, 512, "Some basic shapes available on raylib\nCode reloaded %d times", global_game_data->reload_count);
            rf_draw_text(temp_str, 20, 20, 20, rf_darkgray);
        }
        else
        {
            rf_draw_text("some basic shapes available on raylib", 20, 20, 20, rf_darkgray);
        }

        rf_draw_circle(screen_width / 4, 120, 35, rf_darkblue);

        rf_draw_rectangle(screen_width / 4 * 2 - 60, 100, 120, 60, rf_red);
        rf_draw_rectangle_lines(screen_width / 4 * 2 - 40, 320, 80, 60, rf_orange);  // NOTE: Uses QUADS internally, not lines
        rf_draw_rectangle_gradient_h(screen_width / 4 * 2 - 90, 170, 180, 130, rf_maroon, rf_gold);

        rf_draw_triangle((rf_vec2){(float) screen_width / 4 * 3, 80},
                         (rf_vec2){(float) screen_width / 4 * 3 - 60, 150},
                         (rf_vec2){(float) screen_width / 4 * 3 + 60, 150}, rf_violet);

        rf_draw_poly((rf_vec2){(float)screen_width / 4.0f * 3.0f, 320}, 6, 80, 0, rf_brown);

        rf_draw_circle_gradient(screen_width / 4, 220, 60, rf_green, rf_skyblue);

        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        rf_draw_line(18, 42, screen_width - 18, 42, rf_black);
        rf_draw_circle_lines(screen_width / 4, 340, 80, rf_darkblue);
        rf_draw_triangle_lines(
                (rf_vec2){(float)screen_width / 4 * 3, 160},
                (rf_vec2){(float)screen_width / 4 * 3 - 20, 230},
                (rf_vec2){(float)screen_width / 4 * 3 + 20, 230}, rf_darkblue);

    rf_end_drawing();
}