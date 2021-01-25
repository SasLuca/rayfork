//Implementation of the input box example from raylib using rayfork

#include <stdio.h>
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

#define MAX_INPUT_CHARS 9

rf_context rf_ctx;
rf_default_render_batch rf_mem;

char name[MAX_INPUT_CHARS + 1]; // NOTE: One extra space required for line ending char '\0'
int letter_count = 0;

rf_rec text_box = { SCREEN_WIDTH/2 - 100, 180, 225, 50 };
bool mouse_on_text = false;

int frames_counter = 0;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
}

void on_frame(const input_data input)
{
    // Update
    if (rf_check_collision_point_rec((rf_vec2){ input.mouse_x, input.mouse_y }, text_box)) mouse_on_text = true;
    else mouse_on_text = false;

    if (mouse_on_text)
    {
        // Check if more characters have been pressed on the same frame
        for (int i =0; i < input.key_count; i++)
        {
            if (letter_count < MAX_INPUT_CHARS)
            {
                name[letter_count] = (char)input.char_key_q[i];
                letter_count++;
            }
        }

        if (input.backspace_down)
        {
            letter_count--;
            name[letter_count] = '\0';

            if (letter_count < 0) letter_count = 0;
        }
    }

    if (mouse_on_text) frames_counter++;
    else frames_counter = 0;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_text("PLACE MOUSE OVER INPUT BOX!", 240, 140, 20, RF_GRAY);
    
        rf_draw_rectangle_rec(text_box, RF_LIGHTGRAY);
        if (mouse_on_text) rf_draw_rectangle_outline((rf_rec){ text_box.x, text_box.y, text_box.width, text_box.height }, 1.0f, RF_RED);
        else rf_draw_rectangle_outline((rf_rec){ text_box.x, text_box.y, text_box.width, text_box.height }, 1.0f, RF_DARKGRAY);
    
        rf_draw_text(name, text_box.x + 5, text_box.y + 8, 40, RF_MAROON);

        char* text[100];
        snprintf(text, 100, "INPUT CHARS: %i/%i", letter_count, MAX_INPUT_CHARS);
        rf_draw_text(text, 315, 250, 20, RF_DARKGRAY);
    
        if (mouse_on_text) {
            if (letter_count < MAX_INPUT_CHARS) {
                // Draw blinking underscore char
                if (((frames_counter / 20) % 2) == 0)
                    rf_draw_text("_", text_box.x + 8 + rf_measure_text(rf_get_builtin_bitmap_font(), name, strlen(name), 40, 1.0f).width, text_box.y + 12, 40, RF_MAROON);
            }
            else rf_draw_text("Press BACKSPACE to delete chars...", 230, 300, 20, RF_GRAY);
        }


    rf_end();
}