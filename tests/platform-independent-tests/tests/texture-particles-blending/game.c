// Implementation of the texture particles blending example from raylib using rayfork
#include <stdlib.h>

#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

#define MAX_PARTICLES 200

rf_context rf_ctx;
rf_default_render_batch rf_mem;

// Particle structure with basic data
typedef struct {
    rf_vec2 position;
    rf_color color;
    float alpha;
    float size;
    float rotation;
    bool active;        // NOTE: Use it to activate/deactive particle
} particle;

// Particles pool, reuse them!
particle mouse_tail[MAX_PARTICLES] = { 0 };
rf_texture2d smoke;

float gravity = 3.0f;
int blending = RF_BLEND_ALPHA;

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
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    // Initialize particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        mouse_tail[i].position = (rf_vec2){ 0, 0 };
        mouse_tail[i].color = (rf_color){ random_value_in_range(0, 255), random_value_in_range(0, 255), random_value_in_range(0, 255), 255 };
        mouse_tail[i].alpha = 1.0f;
        mouse_tail[i].size = (float)random_value_in_range(1, 30) / 20.0f;
        mouse_tail[i].rotation = (float)random_value_in_range(0, 360);
        mouse_tail[i].active = false;
    }

    smoke = rf_load_texture_from_file(ASSETS_PATH"smoke.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);
}

void on_frame(const input_data input)
{
    // Update
    // Activate one particle every frame and Update active particles
    // NOTE: Particles initial position should be mouse position when activated
    // NOTE: Particles fall down with gravity and rotation... and disappear after 2 seconds (alpha = 0)
    // NOTE: When a particle disappears, active = false and it can be reused.
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!mouse_tail[i].active)
        {
            mouse_tail[i].active = true;
            mouse_tail[i].alpha = 1.0f;
            mouse_tail[i].position = (rf_vec2){ input.mouse_x, input.mouse_y };
            i = MAX_PARTICLES;
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (mouse_tail[i].active)
        {
            mouse_tail[i].position.y += gravity;
            mouse_tail[i].alpha -= 0.01f;

            if (mouse_tail[i].alpha <= 0.0f) mouse_tail[i].active = false;

            mouse_tail[i].rotation += 5.0f;
        }
    }

    if (input.space_down)
    {
        if (blending == RF_BLEND_ALPHA) blending = RF_BLEND_ADDITIVE;
        else blending = RF_BLEND_ALPHA;
    }
    
    // Draw
    rf_begin();

        rf_clear(RF_DARKGRAY);
        rf_begin_blend_mode(blending);

        // Draw active particles
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            if (mouse_tail[i].active) rf_draw_texture_region(smoke, (rf_rec){ 0.0f, 0.0f, (float)smoke.width, (float)smoke.height },
                                                    (rf_rec){ mouse_tail[i].position.x, mouse_tail[i].position.y, smoke.width*mouse_tail[i].size, smoke.height*mouse_tail[i].size },
                                                    (rf_vec2){ (float)(smoke.width*mouse_tail[i].size/2.0f), (float)(smoke.height*mouse_tail[i].size/2.0f) }, mouse_tail[i].rotation,
                                                    rf_fade(mouse_tail[i].color, mouse_tail[i].alpha));
        }

        rf_end_blend_mode();

        rf_draw_text("PRESS SPACE to CHANGE BLENDING MODE", 180, 20, 20, RF_BLACK);

        if (blending == RF_BLEND_ALPHA) rf_draw_text("ALPHA BLENDING", 290, SCREEN_HEIGHT - 40, 20, RF_BLACK);
        else rf_draw_text("ADDITIVE BLENDING", 280, SCREEN_HEIGHT - 40, 20, RF_RAYWHITE);

    rf_end();
}