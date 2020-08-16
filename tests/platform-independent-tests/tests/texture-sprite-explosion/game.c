// Implementation of the sprite explosion example from raylib using rayfork
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

#define NUM_FRAMES (8)
#define NUM_LINES  (6)

rf_context rf_ctx;
rf_default_render_batch rf_mem;

rf_texture2d explosion;

int frame_width;
int frame_height;
int current_frame = 0;
int current_line = 0;

rf_rec frame_rec;
rf_vec2 position = { 0.0f, 0.0f };

bool active = false;
int frames_counter = 0;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    // @TODO: Fix when audio is implemented
    //InitAudioDevice();

    // Load explosion sound
    //Sound fxBoom = LoadSound("resources/boom.wav");

    // Load explosion texture
    explosion = rf_load_texture_from_file(ASSETS_PATH"explosion.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);

    // Init variables for animation
    frame_width = explosion.width/NUM_FRAMES;    // Sprite one frame rectangle width
    frame_height = explosion.height/NUM_LINES;   // Sprite one frame rectangle height

    frame_rec = (rf_rec){ 0, 0, frame_width, frame_height };
}

void on_frame(const input_data input)
{
    // Update
    // Check for mouse button pressed and activate explosion (if not active)
    if (input.mouse_left_down && !active)
    {
        position = (rf_vec2){ input.mouse_x, input.mouse_y };
        active = true;

        position.x -= frame_width/2;
        position.y -= frame_height/2;

        // @TODO: Fix when audio is implemented
//        PlaySound(fxBoom);
    }

    // Compute explosion animation frames
    if (active)
    {
        frames_counter++;

        if (frames_counter > 2)
        {
            current_frame++;

            if (current_frame >= NUM_FRAMES)
            {
                current_frame = 0;
                current_line++;

                if (current_line >= NUM_LINES)
                {
                    current_line = 0;
                    active = false;
                }
            }

            frames_counter = 0;
        }
    }

    frame_rec.x = frame_width*current_frame;
    frame_rec.y = frame_height*current_line;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        // Draw explosion required frame rectangle
        if (active) rf_draw_texture_region(explosion, frame_rec, (rf_rec){ position.x, position.y, frame_rec.width, frame_rec.height }, (rf_vec2){ 0.0f, 0.0f }, 0.0f, RF_WHITE);

    rf_end();
}