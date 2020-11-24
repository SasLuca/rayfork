#include "platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - texture particles blending"
};

rf_gfx_context ctx;
rf_render_batch batch;

#define MAX_PARTICLES 200

// Particle structure with basic data
typedef struct {
    rf_vec2 position;
    rf_color color;
    float alpha;
    float size;
    float rotation;
    rf_bool active;        // NOTE: Use it to activate/deactive particle
} particle;

// Particles pool, reuse them!
particle mouse_tail[MAX_PARTICLES];
rf_texture2d smoke;

float gravity = 3.0f;
int blending = RF_BLEND_ALPHA;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_gfx_init(&ctx, window.width, window.height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&batch);

    // Initialize particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        mouse_tail[i].position = (rf_vec2){ 0, 0 };
        mouse_tail[i].color    = (rf_color){ rf_libc_rand_wrapper(0, 255), rf_libc_rand_wrapper(0, 255), rf_libc_rand_wrapper(0, 255), 255 };
        mouse_tail[i].alpha    = 1.0f;
        mouse_tail[i].size     = (float)rf_libc_rand_wrapper(1, 30) / 20.0f;
        mouse_tail[i].rotation = (float)rf_libc_rand_wrapper(0, 360);
        mouse_tail[i].active   = 0;
    }

    smoke = rf_load_texture_from_file(ASSETS_PATH"smoke.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);
}

extern void game_update(const platform_input_state* input)
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
            mouse_tail[i].active = 1;
            mouse_tail[i].alpha = 1.0f;
            mouse_tail[i].position = (rf_vec2){ input->mouse_x, input->mouse_y };
            i = MAX_PARTICLES;
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (mouse_tail[i].active)
        {
            mouse_tail[i].position.y += gravity;
            mouse_tail[i].alpha -= 0.01f;

            if (mouse_tail[i].alpha <= 0.0f) mouse_tail[i].active = 0;

            mouse_tail[i].rotation += 5.0f;
        }
    }

    if (input->keys[KEYCODE_SPACE] & BTN_PRESSED_DOWN)
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

        if (blending == RF_BLEND_ALPHA) rf_draw_text("ALPHA BLENDING", 290, window.height - 40, 20, RF_BLACK);
        else rf_draw_text("ADDITIVE BLENDING", 280, window.height - 40, 20, RF_RAYWHITE);

    rf_end();
}