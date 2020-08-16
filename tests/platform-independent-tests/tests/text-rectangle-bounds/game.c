//Implementation of the rectangle bounds example from raylib using rayfork

#include <stdio.h>
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"


rf_context rf_ctx;
rf_default_render_batch rf_mem;

const char text[] = "Text cannot escape\tthis container\t...word wrap also works when active so here's \
a long text for testing.\n\nLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod \
tempor incididunt ut labore et dolore magna aliqua. Nec ullamcorper sit amet risus nullam eget felis eget.";

bool resizing = false;
bool word_wrap = true;

rf_rec container = { 25, 25, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 250};

// Minimum width and heigh for the container rectangle
const int minWidth = 60;
const int minHeight = 60;
const int maxWidth = SCREEN_WIDTH - 50;
const int maxHeight = SCREEN_HEIGHT - 160;

rf_vec2 last_mouse = { 0.0f, 0.0f }; // Stores last mouse coordinates
rf_rec resizer;
rf_color border_color = RF_MAROON;         // Container border color
rf_font font;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    resizer = (rf_rec){ container.x + container.width - 17, container.y + container.height - 17, 14, 14 };
    
    font = rf_get_default_font();       // Get default system font
}

void on_frame(const input_data input)
{
    // Update
    if (input.space_down) word_wrap = !word_wrap;

    rf_vec2 mouse = { input.mouse_x, input. mouse_y };
    
    // Check if the mouse is inside the container and toggle border color
    if (rf_check_collision_point_rec(mouse, container)) border_color = rf_fade(RF_MAROON, 0.4f);
    else if (!resizing) border_color = RF_MAROON;

    // Container resizing logic
    if (resizing)
    {
        if (input.mouse_left_up) resizing = false;

        int width = container.width + (mouse.x - last_mouse.x);
        container.width = (width > minWidth)? ((width < maxWidth)? width : maxWidth) : minWidth;

        int height = container.height + (mouse.y - last_mouse.y);
        container.height = (height > minHeight)? ((height < maxHeight)? height : maxHeight) : minHeight;
    }
    else
    {
        // Check if we're resizing
        if (input.mouse_left_pressed && rf_check_collision_point_rec(mouse, resizer)) resizing = true;
    }

    // Move resizer rectangle properly
    resizer.x = container.x + container.width - 17;
    resizer.y = container.y + container.height - 17;

    last_mouse = mouse; // Update mouse


    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);
        rf_draw_rectangle_outline(container, 3, border_color); // Draw container border
    
        // Draw text in container (add some padding)
        rf_draw_text_rec(font, text, strlen(text),
                    (rf_rec){ container.x + 4, container.y + 4, container.width - 4, container.height - 4 },
                    20.0f, 2.0f, word_wrap, RF_GRAY);
    
        rf_draw_rectangle_rec(resizer, border_color);         // Draw the resize box
    
        // Draw bottom info
        rf_draw_rectangle(0, SCREEN_HEIGHT - 54, SCREEN_WIDTH, 54, RF_GRAY);
        rf_draw_rectangle_rec((rf_rec){ 382, SCREEN_HEIGHT - 34, 12, 12 }, RF_MAROON);
    
        rf_draw_text("Word Wrap: ", 313, SCREEN_HEIGHT-115, 20, RF_BLACK);
        if (word_wrap) rf_draw_text("ON", 447, SCREEN_HEIGHT - 115, 20, RF_RED);
        else rf_draw_text("OFF", 447, SCREEN_HEIGHT - 115, 20, RF_BLACK);
    
        rf_draw_text("Press [SPACE] to toggle word wrap", 218, SCREEN_HEIGHT - 86, 20, RF_GRAY);
    
        rf_draw_text("Click hold & drag the    to resize the container", 155, SCREEN_HEIGHT - 38, 20, RF_RAYWHITE);

    rf_end();
}