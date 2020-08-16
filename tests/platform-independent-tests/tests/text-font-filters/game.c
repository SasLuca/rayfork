//Implementation of the font filters example from raylib using rayfork

#include <stdio.h>
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

const char msg[50] = "Loaded Font";

rf_font font;

float font_size;
rf_sizef text_size;
rf_vec2 font_position = { 40, SCREEN_HEIGHT/2 - 80 };

int current_font_filter = 0;      // FILTER_POINT

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)

    // TTF Font loading with custom generation parameters
    font = rf_load_ttf_font_from_file_ez(ASSETS_PATH"KAISG.ttf", RF_DEFAULT_FONT_SIZE, RF_FONT_ANTIALIAS);

    // Generate mipmap levels to use trilinear filtering
    // NOTE: On 2D drawing it won't be noticeable, it looks like FILTER_BILINEAR
    rf_gen_texture_mipmaps(&font.texture);

    font_size = font.base_size;

    // Setup texture scaling filter
    rf_set_texture_filter(font.texture, RF_FILTER_POINT);
}

void on_frame(const input_data input)
{
    // Update
    font_size += input.mouse_scroll_y * 0.1f;

    // Choose font texture filter method
    if (input.one_down)
    {
        rf_set_texture_filter(font.texture, RF_FILTER_POINT);
        current_font_filter = 0;
    }
    else if (input.two_down)
    {
        rf_set_texture_filter(font.texture, RF_FILTER_BILINEAR);
        current_font_filter = 1;
    }
    else if (input.three_down)
    {
        // NOTE: Trilinear filter won't be noticed on 2D drawing
        rf_set_texture_filter(font.texture, RF_FILTER_TRILINEAR);
        current_font_filter = 2;
    }

    text_size = rf_measure_text(font, msg, strlen(msg), font_size, 0);

    if (input.left_pressed) font_position.x -= 10;
    else if (input.right_pressed) font_position.x += 10;

    // Rayfork doesn't support drag and dropcd
    // Load a dropped TTF file dynamically (at current font_size)
//    if (IsFileDropped())
//    {
//        int count = 0;
//        char **droppedFiles = GetDroppedFiles(&count);
//
//        // NOTE: We only support first ttf file dropped
//        if (IsFileExtension(droppedFiles[0], ".ttf"))
//        {
//            UnloadFont(font);
//            font = LoadFontEx(droppedFiles[0], font_size, 0, 0);
//            ClearDroppedFiles();
//        }
//    }

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_text("Use mouse wheel to change font size", 20, 20, 10, RF_GRAY);
        rf_draw_text("Use KEY_RIGHT and KEY_LEFT to move text", 20, 40, 10, RF_GRAY);
        rf_draw_text("Use 1, 2, 3 to change texture filter", 20, 60, 10, RF_GRAY);
        rf_draw_text("Drop a new TTF font for dynamic loading", 20, 80, 10, RF_DARKGRAY);

        rf_draw_text_ex(font, msg, strlen(msg), font_position, font_size, 0, RF_BLACK);

        // TODO: It seems texSize measurement is not accurate due to chars offsets...
        //DrawRectangleLines(fontPosition.x, fontPosition.y, textSize.x, textSize.y, RED);

        rf_draw_rectangle(0, SCREEN_HEIGHT - 80, SCREEN_WIDTH, 80, RF_LIGHTGRAY);

        char text[100];
        snprintf(text, 100, "Font size: %02.02f", font_size);
        rf_draw_text(text, 20, SCREEN_HEIGHT - 50, 10, RF_DARKGRAY);

        snprintf(text, 100, "Text size: [%02.02f, %02.02f]", text_size.width, text_size.height);
        rf_draw_text(text, 20, SCREEN_HEIGHT - 30, 10, RF_DARKGRAY);

        rf_draw_text("CURRENT TEXTURE FILTER:", 250, 400, 20, RF_GRAY);

        if (current_font_filter == 0) rf_draw_text("POINT", 570, 400, 20, RF_BLACK);
        else if (current_font_filter == 1) rf_draw_text("BILINEAR", 570, 400, 20, RF_BLACK);
        else if (current_font_filter == 2) rf_draw_text("TRILINEAR", 570, 400, 20, RF_BLACK);

    rf_end();
}