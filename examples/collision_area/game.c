//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork.h"
#include "stdio.h"

rf_context rf_ctx;
#define SCREEN_WIDTH 800
#define screen_height 450

float mouse_x;
float mouse_y;

// Box A: Moving box
rf_rectangle box_a = { 10, (float) screen_height / 2 - 50, 200, 100 };
int box_a_speed_x = 4;

// Box B: Mouse moved box
rf_rectangle box_b = {(float) SCREEN_WIDTH / 2 - 30, (float) screen_height / 2 - 30, 60, 60 };

// Collision rectangle
rf_rectangle box_collision = { 0 };

// Top menu limits
const int screen_upper_limit = 40;

// Movement pause
bool pause = false;

// Collision detection
bool collision = false;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, SCREEN_WIDTH, screen_height);
    rf_set_target_fps(60);
    rf_load_font_default();
}

void on_frame(void)
{
    // Move box if not paused
    if (!pause) box_a.x += box_a_speed_x;

    // Bounce box on x screen limits
    if (((box_a.x + box_a.width) >= SCREEN_WIDTH) || (box_a.x <= 0)) box_a_speed_x *= -1;

    // Update player-controlled-box (box02)
    box_b.x = mouse_x - box_b.width/2;
    box_b.y = mouse_y - box_b.height/2;

    // Make sure Box B does not go out of move area limits
    if ((box_b.x + box_b.width) >= SCREEN_WIDTH) box_b.x = SCREEN_WIDTH - box_b.width;
    else if (box_b.x <= 0) box_b.x = 0;

    if ((box_b.y + box_b.height) >= screen_height) box_b.y = screen_height - box_b.height;
    else if (box_b.y <= screen_upper_limit) box_b.y = screen_upper_limit;

    // Check boxes collision
    collision = rf_check_collision_recs(box_a, box_b);

    // Get collision rectangle (only on collision)
    if (collision) box_collision = rf_get_collision_rec(box_a, box_b);
    
    // Draw
    //-----------------------------------------------------
    rf_begin_drawing();

    rf_clear_background(rf_raywhite);

    rf_draw_rectangle(0, 0, SCREEN_WIDTH, screen_upper_limit, collision ? rf_red : rf_black);

    rf_draw_rectangle_rec(box_a, rf_gold);
    rf_draw_rectangle_rec(box_b, rf_blue);

    if (collision)
    {
        // Draw collision area
        rf_draw_rectangle_rec(box_collision, rf_lime);

        // Draw collision message
        rf_draw_text("COLLISION!", SCREEN_WIDTH / 2 - rf_measure_text("COLLISION!", 20) / 2, screen_upper_limit / 2 - 10, 20, rf_black);

        // Draw collision area
        char text_buff[512];
        snprintf(text_buff, 512, "Collision Area: %i", (int) box_collision.width * (int) box_collision.height);
        rf_draw_text(text_buff, SCREEN_WIDTH / 2 - 100, screen_upper_limit + 10, 20, rf_black);
    }

    rf_draw_fps(10, 10);

    rf_end_drawing();
}

void on_event(const sapp_event* event)
{
    switch (event->type)
    {
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            mouse_x = event->mouse_x;
            mouse_y = event->mouse_y;
            break;
            
        case SAPP_EVENTTYPE_KEY_DOWN:
            pause = !pause;

        default:
            break;
    }
}

void on_cleanup(void)
{
    //Empty
}