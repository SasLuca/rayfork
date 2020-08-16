//Implementation of the texture image text example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context   rf_ctx;
rf_default_render_batch rf_mem;
rf_default_font    default_font_buffers;

rf_font font;
rf_texture2d texture;
rf_vec2 position;

bool show_font = false;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    //rf_set_target_fps(60);

    font = rf_load_ttf_font_from_file_ez(ASSETS_PATH"KAISG.ttf", RF_DEFAULT_FONT_SIZE, RF_FONT_ANTIALIAS);

    texture = rf_load_texture_from_file_ez(ASSETS_PATH"parrots.png");

    position = (rf_vec2) { (float)(SCREEN_WIDTH / 2 - texture.width / 2), (float)(SCREEN_HEIGHT / 2 - texture.height / 2 - 20) };

    show_font = false;
}

void on_frame(const input_data input)
{
    // Update
    if (input.space_pressed) show_font = true;
    else show_font = false;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        if (!show_font)
        {
            rf_draw_texture(texture, position.x, position.y, RF_WHITE);

            const char* text = "[Parrots font drawing]";
            rf_draw_text_ex(font, text, strlen(text), (rf_vec2) { position.x + 20, position.y + 20 + 280 }, (float) font.base_size, 0, RF_WHITE);

            rf_draw_text("PRESS SPACE to SEE USED SPRITEFONT ", 290, 420, 10, RF_DARKGRAY);
        }
        else rf_draw_texture(font.texture, SCREEN_WIDTH / 2 - font.texture.width / 2, 50, RF_BLACK);

    rf_end();
}