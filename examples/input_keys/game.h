#pragma once

#define screen_width 800
#define screen_height 450

typedef struct input_data input_data;
struct input_data
{
    int up_pressed;
    int down_pressed;
    int left_pressed;
    int right_pressed;
};

void on_init(void);
void on_frame(input_data);