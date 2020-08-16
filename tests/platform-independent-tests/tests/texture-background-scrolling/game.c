//Implementation of the texture npatch drawing example from raylib using rayfork
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;
rf_default_font default_font_buffers;

// NOTE: Be careful, background width must be equal or bigger than screen width
// if not, texture should be draw more than two times for scrolling effect
rf_texture2d background;
rf_texture2d midground;
rf_texture2d foreground;

float scrolling_back = 0.0f;
float scrolling_mid = 0.0f;
float scrolling_fore = 0.0f;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    background = rf_load_texture_from_file_ez(ASSETS_PATH"cyberpunk_street_background.png");
    midground = rf_load_texture_from_file_ez(ASSETS_PATH"cyberpunk_street_midground.png");
    foreground = rf_load_texture_from_file_ez(ASSETS_PATH"cyberpunk_street_foreground.png");
}

void on_frame(const input_data input)
{
    // Update
    scrolling_back -= 0.1f;
    scrolling_mid -= 0.5f;
    scrolling_fore -= 1.0f;

    // NOTE: Texture is scaled twice its size, so it sould be considered on scrolling
    if (scrolling_back <= -background.width * 2) scrolling_back = 0;
    if (scrolling_mid <= -midground.width * 2) scrolling_mid = 0;
    if (scrolling_fore <= -foreground.width * 2) scrolling_fore = 0;

    // Draw
    rf_begin();

        rf_clear(rf_color_from_int(0x052c46ff));

        // Draw background image twice
        // NOTE: Texture is scaled twice its size
        rf_draw_texture_ex(background, scrolling_back, 20, background.width * 2, background.height * 2, 1, RF_WHITE);
        rf_draw_texture_ex(background, background.width*2 + scrolling_back, 20, background.width * 2, background.height * 2, 1, RF_WHITE);

        // Draw midground image twice
        rf_draw_texture_ex(midground, scrolling_mid, 20, midground.width * 2, midground.height * 2, 1, RF_WHITE);
        rf_draw_texture_ex(midground, midground.width*2 + scrolling_mid, 20, midground.width * 2, midground.height * 2, 1, RF_WHITE);

        // Draw foreground image twice
        rf_draw_texture_ex(foreground, scrolling_fore, 70, foreground.width * 2, foreground.height * 2, 1, RF_WHITE);
        rf_draw_texture_ex(foreground, foreground.width*2 + scrolling_fore, 70, foreground.width * 2, foreground.height * 2, 1, RF_WHITE);

        rf_draw_text("BACKGROUND SCROLLING & PARALLAX", 10, 10, 20, RF_RED);
        rf_draw_text("(c) Cyberpunk Street Environment by Luis Zuno (@ansimuz)", SCREEN_WIDTH - 330, SCREEN_HEIGHT - 20, 10, RF_RAYWHITE);

    rf_end();
}