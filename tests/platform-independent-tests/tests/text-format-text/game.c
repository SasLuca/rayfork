//Implementation of the format text example from raylib using rayfork

#include <stdio.h>
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

int score = 100020;
int hiscore = 200450;
int lives = 5;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
}

void on_frame(const input_data input)
{
    rf_begin();

        rf_clear(RF_RAYWHITE);

        char text[100];
        snprintf(text, 100, "Score: %08i", score);
        rf_draw_text(text, 200, 80, 20, RF_RED);

        snprintf(text, 100, "HiScore: %08i", hiscore);
        rf_draw_text(text, 200, 120, 20, RF_GREEN);

        snprintf(text, 100, "Lives: %02i", lives);
        rf_draw_text(text, 200, 160, 40, RF_BLUE);

    rf_end();
}