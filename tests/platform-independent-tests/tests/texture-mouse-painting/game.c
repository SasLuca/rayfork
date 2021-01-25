// Implementation of the texture mouse painting example from raylib using rayfork

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

#define MAX_COLORS_COUNT 23

rf_context rf_ctx;
rf_default_render_batch rf_mem;

// Colours to choose from
rf_color colors[MAX_COLORS_COUNT] = {
        RF_RAYWHITE, RF_YELLOW, RF_GOLD, RF_ORANGE, RF_PINK, RF_RED, RF_MAROON, RF_GREEN, RF_LIME, RF_DARKGREEN,
        RF_SKYBLUE, RF_BLUE, RF_DARKBLUE, RF_PURPLE, RF_VIOLET, RF_DARKPURPLE, RF_BEIGE, RF_BROWN, RF_DARKBROWN,
        RF_LIGHTGRAY, RF_GRAY, RF_DARKGRAY, RF_BLACK };

// Define colors_recs data (for every rectangle)
rf_rec colors_recs[MAX_COLORS_COUNT] = { 0 };

int color_selected = 0;
int color_selected_prev = 0;
int color_mouse_hover = 0;
int brush_size = 20;

rf_rec btn_save_rec = { 750, 10, 40, 30 };
bool btn_save_mouse_hover = false;
bool show_save_message = false;
int save_message_counter = 0;

rf_render_texture2d target;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    for (int i = 0; i < MAX_COLORS_COUNT; i++)
    {
        colors_recs[i].x = 10 + 30*i + 2*i;
        colors_recs[i].y = 10;
        colors_recs[i].width = 30;
        colors_recs[i].height = 30;
    }

    // Create a RenderTexture2D to use as a canvas
     target = rf_load_render_texture(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Clear render texture before entering the game loop
    rf_begin_render_to_texture(target);
    rf_clear(colors[0]);
    rf_end_render_to_texture();
}

void on_frame(const input_data input)
{
    // Update

    // Move between colors with keys
    if (input.right_down) {
        color_selected++;
        color_selected_prev = color_selected;
    }
    else if (input.left_down)
    {
        color_selected--;
        color_selected_prev = color_selected;
    }

    if (color_selected >= MAX_COLORS_COUNT) color_selected = MAX_COLORS_COUNT - 1;
    else if (color_selected < 0) color_selected = 0;

    // Choose color with mouse
    for (int i = 0; i < MAX_COLORS_COUNT; i++)
    {
        if (rf_check_collision_point_rec((rf_vec2){ input.mouse_x, input.mouse_y }, colors_recs[i]))
        {
            color_mouse_hover = i;
            break;
        }
        else color_mouse_hover = -1;
    }

    if ((color_mouse_hover >= 0) && input.mouse_left_down)
    {
        color_selected = color_mouse_hover;
        color_selected_prev = color_selected;
    }

    // Change brush size
    // Working on mouse_wheel
    brush_size += input.scroll_y;

    if (brush_size < 2) brush_size = 2;
    if (brush_size > 50) brush_size = 50;

    if (input.c_down)
    {
        // Clear render texture to clear color
        rf_begin_render_to_texture(target);
        rf_clear(colors[0]);
        rf_end_render_to_texture();
    }

    if (input.mouse_left_pressed /*|| (GetGestureDetected() == GESTURE_DRAG)*/)
    {
        // Paint circle into render texture
        // NOTE: To avoid discontinuous circles, we could store
        // previous-next mouse points and just draw a line using brush size
        rf_begin_render_to_texture(target);
        if (input.mouse_y > 50) rf_draw_circle(input.mouse_x, input.mouse_y, brush_size, colors[color_selected]);
        rf_end_render_to_texture();
    }

    if (input.mouse_right_pressed)
    {
        color_selected = 0;

        // Erase circle from render texture
        rf_begin_render_to_texture(target);
        if (input.mouse_y > 50) rf_draw_circle(input.mouse_x, input.mouse_y, brush_size, colors[0]);
        rf_end_render_to_texture();
    }
    else color_selected = color_selected_prev;

    // Check mouse hover save button
    if (rf_check_collision_point_rec((rf_vec2){ input.mouse_x, input.mouse_y }, btn_save_rec)) btn_save_mouse_hover = true;
    else btn_save_mouse_hover = false;

    // Image saving logic
    // NOTE: Saving painted texture to a default named image
    if ((btn_save_mouse_hover && input.mouse_left_up) || input.s_pressed)
    {
//        rf_image image = rf_get_texture_data(target.texture, RF_DEFAULT_ALLOCATOR);
//        rf_image_flip_vertical(&image, RF_DEFAULT_ALLOCATOR);
//        rf_export_image(image, "my_amazing_texture_painting.png");
//        rf_image_free(image);
        show_save_message = true;
    }

    if (show_save_message)
    {
        // On saving, show a full screen message for 2 seconds
        save_message_counter++;
        if (save_message_counter > 240)
        {
            show_save_message = false;
            save_message_counter = 0;
        }
    }

    // Draw
    rf_begin();

    rf_clear(RF_RAYWHITE);

    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
    rf_draw_texture_region(target.texture, (rf_rec){ 0, 0, target.texture.width, -target.texture.height },
            (rf_rec){ 0, 0, target.texture.width, target.texture.height }, (rf_vec2){ 0, 0 }, 0.0f, RF_WHITE);

    // Draw drawing circle for reference
    if (input.mouse_y > 50)
    {
        if (input.mouse_right_pressed) rf_draw_circle_lines(input.mouse_x, input.mouse_y, brush_size, RF_GRAY);
        else rf_draw_circle(input.mouse_x, input.mouse_y, brush_size, colors[color_selected]);
    }

    // Draw top panel
    rf_draw_rectangle(0, 0, SCREEN_WIDTH, 50, RF_RAYWHITE);
    rf_draw_line(0, 50, SCREEN_HEIGHT, 50, RF_LIGHTGRAY);

    // Draw color selection rectangles
    for (int i = 0; i < MAX_COLORS_COUNT; i++) rf_draw_rectangle_rec(colors_recs[i], colors[i]);
    rf_draw_rectangle_outline((rf_rec){10, 10, 30, 30}, 1.0f, RF_LIGHTGRAY);

    if (color_mouse_hover >= 0) rf_draw_rectangle_rec(colors_recs[color_mouse_hover], rf_fade(RF_WHITE, 0.6f));

    rf_draw_rectangle_outline((rf_rec){ colors_recs[color_selected].x - 2, colors_recs[color_selected].y - 2,
                                      colors_recs[color_selected].width + 4, colors_recs[color_selected].height + 4 }, 2, RF_BLACK);

    // Draw save image button
    rf_draw_rectangle_outline(btn_save_rec, 2, btn_save_mouse_hover ? RF_RED : RF_BLACK);
    rf_draw_text("SAVE!", 755, 20, 10, btn_save_mouse_hover ? RF_RED : RF_BLACK);

    // Draw save image message
    if (show_save_message) {
        rf_draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, rf_fade(RF_RAYWHITE, 0.8f));
        rf_draw_rectangle(0, 150, SCREEN_WIDTH, 80, RF_BLACK);
        rf_draw_text("RayFork doesn't support image exporting :)", 150, 180, 20, RF_RAYWHITE);
    }

    rf_end();
}