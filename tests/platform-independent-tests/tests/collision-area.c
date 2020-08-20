#include "platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - collision area"
};

rf_context ctx;
rf_render_batch batch;

// Box A: Moving box
rf_rec box_a;
int box_a_speed_x = 4;

// Box B: Mouse moved box
rf_rec box_b;

// Collision rectangle
rf_rec box_collision;

// Top menu limits
const int screen_upper_limit = 40;

// Movement pause
bool pause;

// Collision detection
bool collision;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    rf_init_context(&ctx);
    rf_init_gfx(window.width, window.height, gfx_data);

    batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&batch);

    box_a = (rf_rec) {10, (float) window.height / 2 - 50, 200, 100 };
    box_b = (rf_rec) {(float) window.width / 2 - 30, (float) window.height / 2 - 30, 60, 60 };
}

extern void game_update(const platform_input_state* input)
{
    if (input->any_key_pressed) { pause = !pause; }

    // Move box if not paused
    if (!pause) { box_a.x += box_a_speed_x; }

    // Bounce box on x screen limits
    if (((box_a.x + box_a.width) >= window.width) || (box_a.x <= 0))
    {
        box_a_speed_x *= -1;
    }

    // Update player-controlled-box (box02)
    box_b.x = input->mouse_x - box_b.width  / 2;
    box_b.y = input->mouse_y - box_b.height / 2;

    // Make sure Box B does not go out of move area limits
    if ((box_b.x + box_b.width) >= window.width) box_b.x = window.width - box_b.width;
    else if (box_b.x <= 0) box_b.x = 0;

    if ((box_b.y + box_b.height) >= window.height) box_b.y = window.height - box_b.height;
    else if (box_b.y <= screen_upper_limit) box_b.y = screen_upper_limit;

    // Check boxes collision
    collision = rf_check_collision_recs(box_a, box_b);

    // Get collision rectangle (only on collision)
    if (collision) box_collision = rf_get_collision_rec(box_a, box_b);
    
    // Draw
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        rf_draw_rectangle(0, 0, window.width, screen_upper_limit, collision ? RF_RED : RF_BLACK);

        rf_draw_rectangle_rec(box_a, RF_GOLD);
        rf_draw_rectangle_rec(box_b, RF_BLUE);

        if (collision)
        {
            // Draw collision area
            rf_draw_rectangle_rec(box_collision, RF_LIME);

            // Draw collision message
            const int text_width = rf_measure_text(rf_get_default_font(), "COLLISION!", 20, 1).width;
            rf_draw_text("COLLISION!", window.width / 2 - text_width / 2, screen_upper_limit / 2 - 10, 20, RF_BLACK);

            // Draw collision area
            char text_buff[512] = {0};
            snprintf(text_buff, 512, "Collision Area: %i", (int) box_collision.width * (int) box_collision.height);
            rf_draw_text(text_buff, window.width / 2 - 100, screen_upper_limit + 10, 20, RF_BLACK);
        }
    }
    rf_end();
}