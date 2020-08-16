//Implementation of the font loading example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

// Define characters to draw
// NOTE: raylib supports UTF-8 encoding, following list is actually codified as UTF8 internally
const char msg[256] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHI\nJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmn\nopqrstuvwxyz{|}~¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓ\nÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷\nøùúûüýþÿ";

rf_font font_bm;
rf_font font_ttf;
bool use_ttf = false;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)

    // BMFont (AngelCode) : Font data and image atlas have been generated using external program
    // font_bm = rf_load_fnt_font_from_file("resources/pixantiqua.fnt");

    // TTF font : Font data and atlas are generated directly from TTF
    // NOTE: We define a font base size of 32 pixels tall and up-to 250 characters
    font_ttf = rf_load_ttf_font_from_file_ez(ASSETS_PATH"pixantiqua.ttf", RF_DEFAULT_FONT_SIZE, RF_FONT_ANTIALIAS);
}

void on_frame(const input_data input)
{
    // Update
    if (input.space_pressed) use_ttf = true;
    else use_ttf = false;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_text("Hold SPACE to use TTF generated font", 20, 20, 20, RF_LIGHTGRAY);

        if (!use_ttf)
        {
//            rf_draw_text_ex(font_bm, msg, strlen(msg), (rf_vec2){ 20.0f, 100.0f }, font_bm.base_size, 2, RF_MAROON);
            rf_draw_text("Using BMFont (Angelcode) imported", 20, SCREEN_HEIGHT - 30, 20, RF_GRAY);
        }
        else
        {
            rf_draw_text_ex(font_ttf, msg, strlen(msg), (rf_vec2){ 20.0f, 100.0f }, font_ttf.base_size, 2, RF_LIME);
            rf_draw_text("Using TTF font generated", 20, SCREEN_HEIGHT - 30, 20, RF_GRAY);
        }


    rf_end();
}