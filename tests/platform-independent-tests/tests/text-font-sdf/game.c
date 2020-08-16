//Implementation of the font filters example from raylib using rayfork

#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context   rf_ctx;
rf_default_render_batch    rf_mem;
rf_default_font     default_font_buffers;

const char msg[50] = "Signed Distance Fields";

rf_font font_default = { 0 };

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    rf_set_target_fps(60);

    // Default font generation from TTF font
    font_default.base_size = 16;
    font_default.chars_count = 95;
    // Parameters > font size: 16, no chars array provided (0), chars count: 95 (autogenerate chars array)
    font_default.chars_info = rf_load_ttf_font_from_data("resources/AnonymousPro-Bold.ttf", 16, 0, 95, FONT_DEFAULT);
    // Parameters > chars count: 95, font size: 16, chars padding in image: 4 px, pack method: 0 (default)
    Image atlas = GenImageFontAtlas(font_default.chars, &font_default.recs, 95, 16, 4, 0);
    font_default.texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);

    // SDF font generation from TTF font
    Font fontSDF = { 0 };
    fontSDF.baseSize = 16;
    fontSDF.charsCount = 95;
    // Parameters > font size: 16, no chars array provided (0), chars count: 0 (defaults to 95)
    fontSDF.chars = LoadFontData("resources/AnonymousPro-Bold.ttf", 16, 0, 0, FONT_SDF);
    // Parameters > chars count: 95, font size: 16, chars padding in image: 0 px, pack method: 1 (Skyline algorythm)
    atlas = GenImageFontAtlas(fontSDF.chars, &fontSDF.recs, 95, 16, 0, 1);
    fontSDF.texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);

    // Load SDF required shader (we use default vertex shader)
    Shader shader = LoadShader(0, FormatText("resources/shaders/glsl%i/sdf.fs", GLSL_VERSION));
    SetTextureFilter(fontSDF.texture, FILTER_BILINEAR);    // Required for SDF font

    Vector2 fontPosition = { 40, screenHeight/2 - 50 };
    Vector2 textSize = { 0.0f, 0.0f };
    float fontSize = 16.0f;
    int currentFont = 0;            // 0 - font_default, 1 - fontSDF
}

void on_frame(const input_data input)
{
    // Update

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);
        
    rf_end();
}