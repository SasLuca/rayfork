#pragma once

#define SCREEN_WIDTH  (800)
#define SCREEN_HEIGHT (450)

typedef struct input_data input_data;
struct input_data
{
    int mouse_left_pressed;
    int mouse_left_down;

    int right_pressed;
    int right_down;
};

void on_init(void);
void on_frame(input_data);