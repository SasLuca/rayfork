#pragma once

typedef struct input_data input_data;
struct input_data
{
    float mouse_x;
    float mouse_y;
    int mouse_left_pressed;
    int mouse_middle_pressed;
    int mouse_right_pressed;
};

void on_init(void);
void on_frame(input_data);