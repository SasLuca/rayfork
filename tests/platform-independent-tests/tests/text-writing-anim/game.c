//Implementation of the input box example from raylib using rayfork

#include <stdio.h>
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"


rf_context rf_ctx;
rf_default_render_batch rf_mem;

const char message[128] = "This sample illustrates a text writing\nanimation effect! Check it out! ;)";

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
    if (input.space_pressed) frames_counter += 8;
    else frames_counter++;

    if (frames_counter > 1280)
        frames_counter = 1280;

    if (input.enter_down) frames_counter = 0;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        char sub_text[128];
        memcpy(sub_text, message, frames_counter/10);
        sub_text[frames_counter/10] = '\0';

        rf_draw_text(sub_text, 210, 160, 20, RF_MAROON);

        rf_draw_text("PRESS [ENTER] to RESTART!", 240, 260, 20, RF_LIGHTGRAY);
        rf_draw_text("PRESS [SPACE] to SPEED UP!", 239, 300, 20, RF_LIGHTGRAY);

    rf_end();
}