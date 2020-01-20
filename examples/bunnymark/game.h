#pragma once

#define screen_width 800
#define screen_height 450

typedef struct input_data input_data;
struct input_data
{
    int left_mouse_button_pressed;
    int right_mouse_button_pressed;

    int mouse_x;
    int mouse_y;
};

void on_init(void);
void on_frame(input_data);