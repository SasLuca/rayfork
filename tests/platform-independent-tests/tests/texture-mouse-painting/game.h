#pragma once

#define SCREEN_WIDTH  (800)
#define SCREEN_HEIGHT (450)

typedef struct input_data input_data;
struct input_data
{
    float mouse_x;
    float mouse_y;

    int mouse_left_pressed;
    int mouse_left_down;
    int mouse_left_up;

    int mouse_right_pressed;
    int mouse_right_down;
    int mouse_right_up;

    float scroll_y;

    int c_pressed;
    int c_down;

    int s_pressed;
    int s_down;

    int left_down;
    int left_pressed;

    int right_pressed;
    int right_down;
};

void on_init(void);
void on_frame(input_data);