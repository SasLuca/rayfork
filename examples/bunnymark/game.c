//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#define rf_max_batch_elements (8192*2)
#include "glad/glad.h"
#include "rayfork_renderer.h"
#include "game.h"

rf_renderer_context rf_ctx;

#define MAX_BUNNIES 50000  // 50K bunnies limit

// This is the maximum amount of elements (quads) per batch
// NOTE: This value is defined in [rlgl] module and can be changed there
typedef struct bunny bunny;
struct bunny
{
    rf_vec2 position;
    rf_vec2 speed;
    rf_color color;
};

rf_texture2d bunny_texture;
bunny*       bunnies;
int          bunnies_count;

int random_value_in_range(int min, int max)
{
    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }

    return (rand() % (abs(max - min) + 1) + min);
}

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, screen_width, screen_height);
    rf_set_target_fps(60);
    rf_load_font_default();

    bunny_texture = rf_load_texture("../../../examples/assets/wabbit_alpha.png");

    bunnies = (bunny*) malloc(MAX_BUNNIES * sizeof(bunny)); // Bunnies array
}

void on_frame(const input_data input)
{
    // Update
    if (input.left_mouse_button_pressed)
    {
        // Create more bunnies
        for (int i = 0; i < 100; i++)
        {
            if (bunnies_count < MAX_BUNNIES)
            {
                bunnies[bunnies_count].position = (rf_vec2) {(float) input.mouse_x, (float) input.mouse_y };
                bunnies[bunnies_count].speed.x = (float) random_value_in_range(-250, 250) / 60.0f;
                bunnies[bunnies_count].speed.y = (float) random_value_in_range(-250, 250) / 60.0f;
                bunnies[bunnies_count].color = (rf_color) { random_value_in_range(50, 240),random_value_in_range(80, 240), random_value_in_range(100, 240), 255 };
                bunnies_count++;
            }
        }
    }

    // Update bunnies
    for (int i = 0; i < bunnies_count; i++)
    {
        bunnies[i].position.x += bunnies[i].speed.x;
        bunnies[i].position.y += bunnies[i].speed.y;

        if (((bunnies[i].position.x + bunny_texture.width / 2) > screen_width) ||
            ((bunnies[i].position.x + bunny_texture.width / 2) < 0)) bunnies[i].speed.x *= -1;
        if (((bunnies[i].position.y + bunny_texture.height / 2) > screen_height) ||
            ((bunnies[i].position.y + bunny_texture.height / 2 - 40) < 0)) bunnies[i].speed.y *= -1;
    }

    // Draw
    rf_begin_drawing();

    rf_clear_background(rf_raywhite);

    for (int i = 0; i < bunnies_count; i++)
    {
        // NOTE: When internal batch buffer limit is reached (MAX_BATCH_ELEMENTS),
        // a draw call is launched and buffer starts being filled again;
        // before issuing a draw call, updated vertex data from internal CPU buffer is send to GPU...
        // Process of sending data is costly and it could happen that GPU data has not been completely
        // processed for drawing while new data is tried to be sent (updating current in-use buffers)
        // it could generates a stall and consequently a frame drop, limiting the number of drawn bunnies
        rf_draw_texture(bunny_texture, bunnies[i].position.x, bunnies[i].position.y, bunnies[i].color);
    }

    rf_draw_rectangle(0, 0, screen_width, 40, rf_black);

    char text[1024];
    snprintf(text, sizeof(text), "bunnies: %i", bunnies_count);
    rf_draw_text(text, 120, 10, 20, rf_green);
    snprintf(text, sizeof(text), "batched draw calls: %i", 1 + bunnies_count / RF_MAX_BATCH_ELEMENTS);
    rf_draw_text(text, 320, 10, 20, rf_maroon);

    rf_draw_fps(10, 10);

    rf_end_drawing();
}