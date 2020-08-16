#define RAYFORK_IMPLEMENTATION
#define RF_WINDOW_WIDTH  500
#define RF_WINDOW_HEIGHT 500
#define RF_WINDOW_TITLE "rayfork es example"
#include "rayfork_es.h"

void rf_on_init(void) { }
void rf_on_event(const sapp_event* event) { }

void rf_on_frame(float delta)
{
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        rf_draw_text("some basic shapes available in rayfork", 20, 20, 20, RF_DARKGRAY);

        rf_draw_circle(RF_WINDOW_WIDTH / 4, 120, 35, RF_DARKBLUE);

        rf_draw_rectangle(RF_WINDOW_WIDTH / 4 * 2 - 60, 100, 120, 60, RF_RED);

        rf_draw_rectangle_outline((rf_rec) { RF_WINDOW_WIDTH / 4 * 2 - 40, 320, 80, 60 }, 0, RF_ORANGE);  // NOTE: Uses QUADS internally, not lines

        rf_draw_rectangle_gradient_h(RF_WINDOW_WIDTH / 4 * 2 - 90, 170, 180, 130, RF_MAROON, RF_GOLD);

        rf_draw_triangle((rf_vec2) { RF_WINDOW_WIDTH / 4 * 3, 80 },
                         (rf_vec2) { RF_WINDOW_WIDTH / 4 * 3 - 60, 150 },
                         (rf_vec2) { RF_WINDOW_WIDTH / 4 * 3 + 60, 150 }, RF_VIOLET);

        rf_draw_poly((rf_vec2) { RF_WINDOW_WIDTH / 4.0f * 3.0f, 320 }, 6, 80, 0, RF_BROWN);

        rf_draw_circle_gradient(RF_WINDOW_WIDTH / 4, 220, 60, RF_GREEN, RF_SKYBLUE);

        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        rf_draw_line(18, 42, RF_WINDOW_WIDTH - 18, 42, RF_BLACK);

        rf_draw_circle_lines(RF_WINDOW_WIDTH / 4, 340, 80, RF_DARKBLUE);

        rf_draw_triangle_lines(
                (rf_vec2) { RF_WINDOW_WIDTH / 4 * 3, 160 },
                (rf_vec2) { RF_WINDOW_WIDTH / 4 * 3 - 20, 230 },
                (rf_vec2) { RF_WINDOW_WIDTH / 4 * 3 + 20, 230 },
                RF_DARKBLUE);
    }
    rf_end();
}