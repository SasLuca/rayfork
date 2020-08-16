//Implementation of the geometric shapes example from raylib using rayfork
#include <stdio.h>
#include "include/rayfork.h"
#include "glad/glad.h"
#include "game.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;

const int screen_width = 800;
const int screen_height = 450;

float box_pos_y = screen_width / 2 - 40;
int scroll_speed = 4;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, screen_width, screen_height, RF_DEFAULT_OPENGL_PROCS);
}

void on_frame(const input_data input)
{
    //Update
    box_pos_y -= input.mouse_scroll_y/4 * scroll_speed; // mouse_scroll is either 4 or -4

    //Render
    rf_begin();

    rf_clear(RF_RAYWHITE);


    rf_draw_rectangle(screen_width/2 - 40, box_pos_y, 80, 80, RF_MAROON);

    rf_draw_text("Use mouse wheel to move the cube up and down!", 10, 10, 20, RF_GRAY);
    char scroll_pos_y_text_buff[1024];
    snprintf(scroll_pos_y_text_buff, 1024, "Box position Y: %f", box_pos_y);
    rf_draw_text(scroll_pos_y_text_buff, 10, 40, 20, RF_LIGHTGRAY);

    rf_end();
}