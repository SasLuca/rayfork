// Implementation of the sprite button example from raylib using rayfork
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

#define NUM_FRAMES 3 // Number of frames (rectangles) for the button sprite texture

rf_context rf_ctx;
rf_default_render_batch rf_mem;

// @TODO: fix this after rayaudio is implemented in rayfork
//rf_sound fxButton = LoadSound("resources/buttonfx.wav");   // Load button sound

rf_texture2d button;

// Define frame rectangle for drawing
int frameHeight;
rf_rec sourceRec;

// Define button bounds on screen
rf_rec btnBounds;

int btnState = 0;       // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
bool btnAction = false; // Button action should be activated

rf_vec2 mousePoint = { 0.0f, 0.0f };

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    // @TODO: fix when rayaudio is implemented in rayfrok
    //rf_init_audio_device();

    button = rf_load_texture_from_file(ASSETS_PATH"button.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); // Load button texture

    frameHeight = button.height/NUM_FRAMES;

    sourceRec = (rf_rec){ 0, 0, button.width, frameHeight };
    btnBounds = (rf_rec){ SCREEN_WIDTH / 2 - button.width/2, SCREEN_HEIGHT/2 - button.height/NUM_FRAMES/2, button.width, frameHeight };
}

void on_frame(const input_data input)
{
    // Update
    mousePoint = (rf_vec2){ input.mouse_x, input.mouse_y };
    btnAction = false;

    // Check button state
    if (rf_check_collision_point_rec(mousePoint, btnBounds))
    {
        if (input.mouse_left_pressed) btnState = 2;
        else btnState = 1;

        if (input.mouse_left_up) btnAction = true;
    }
    else btnState = 0;

    if (btnAction)
    {
        // @TODO: fix once rayaudio is implemented raufork
//        rf_play_sound(fx_button);

        // TODO: Any desired action
    }

    // Calculate button frame rectangle to draw depending on button state
    sourceRec.y = btnState*frameHeight;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_draw_texture_region(button, sourceRec, btnBounds, (rf_vec2){ 0.0f, 0.0f }, 0.0f, RF_WHITE); // Draw button frame

    rf_end();
}