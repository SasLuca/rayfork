#pragma once

#define SCREEN_WIDTH  (800)
#define SCREEN_HEIGHT (450)

typedef struct input_data input_data;
struct input_data
{
    int char_key_q[50];
    int key_count;

    int shift_pressed;
    int backspace_pressed;
    int backspace_down;

    float mouse_x;
    float mouse_y;
};

void on_init(void);
void on_frame(input_data);