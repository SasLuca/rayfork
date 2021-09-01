#include "platform-common/platform.h"

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - core 2d camera"
};

rf_gfx_context ctx;
rf_render_batch batch;

int max_buildings = 100;

rf_rec   buildings[100];
rf_color buildColors[100];

rf_rec player = { 400, 280, 40, 40 };

int spacing = 0;

rf_camera2d camera;

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    rf_gfx_init(&ctx, window.width, window.height, gfx_data);

    batch = rf_create_default_render_batch(rf_default_allocator);
    rf_set_active_render_batch(&batch);

    camera.target = (rf_vec2) { player.x + 20, player.y + 20 };
    camera.offset = (rf_vec2) {window.width / 2, window.height / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for (int i = 0; i < max_buildings; i++)
    {
        buildings[i].width = rf_default_rand_in_range(50, 100);
        buildings[i].height = rf_default_rand_in_range(100, 800);
        buildings[i].y = window.height - 130 - buildings[i].height;
        buildings[i].x = -6000 + spacing;

        spacing += buildings[i].width;

        buildColors[i] = (rf_color) { rf_default_rand_in_range(200, 240), rf_default_rand_in_range(200, 240), rf_default_rand_in_range(200, 250), 255 };
    }
}

extern void game_update(const platform_input_state* input, float delta)
{
    // Player movement
    if (input->keys[KEYCODE_RIGHT] & (KEY_PRESSED_DOWN | KEY_HOLD_DOWN)) { player.x += 2; }
    else if (input->keys[KEYCODE_LEFT] & (KEY_PRESSED_DOWN | KEY_HOLD_DOWN)) { player.x -= 2; }

    // Camera target follows player
    camera.target = (rf_vec2) { player.x + 20, player.y + 20 };

    // Camera rotation controls
    if (input->keys[KEYCODE_A] & (KEY_PRESSED_DOWN | KEY_HOLD_DOWN)) { camera.rotation--; }
    else if (input->keys[KEYCODE_S] & (KEY_PRESSED_DOWN | KEY_HOLD_DOWN)) { camera.rotation++; }

    // Limit camera rotation to 80 degrees (-40 to 40)
    if (camera.rotation > 40) { camera.rotation = 40; }
    else if (camera.rotation < -40) { camera.rotation = -40; }

    // Camera zoom controls
    if (input->mouse_scroll_y) { camera.zoom += input->mouse_scroll_y / 12; }

    if (camera.zoom > 3.0f) { camera.zoom = 3.0f; }
    else if (camera.zoom < 0.1f) { camera.zoom = 0.1f; }

    // Camera reset (zoom and rotation)
    if (input->keys[KEYCODE_R] & KEY_PRESSED_DOWN)
    {
        camera.zoom = 1.0f;
        camera.rotation = 0.0f;
    }

    //Render
    rf_begin();

    rf_clear(rf_raywhite);

    rf_begin_2d(camera);

        rf_draw_rectangle(-6000, 320, 13000, 8000, rf_gray);

        for (int i = 0; i < max_buildings; i++) rf_draw_rectangle_rec(buildings[i], buildColors[i]);

        rf_draw_rectangle_rec(player, rf_red);

        rf_draw_line(camera.target.x, -window.height * 10, camera.target.x, window.height * 10, rf_green);
        rf_draw_line(-window.width * 10, camera.target.y, window.width * 10, camera.target.y, rf_green);

    rf_end_2d();

    rf_draw_text("SCREEN AREA", 640, 10, 20, rf_red);

    rf_draw_rectangle(0, 0, window.width, 5, rf_red);
    rf_draw_rectangle(0, 5, 5, window.height - 10, rf_red);
    rf_draw_rectangle(window.width - 5, 5, 5, window.height - 10, rf_red);
    rf_draw_rectangle(0, window.height - 5, window.width, 5, rf_red);

    rf_draw_rectangle( 10, 10, 250, 113, rf_fade(rf_sky_blue, 0.5f));
    rf_draw_rectangle_outline((rf_rec){ 10, 10, 250, 113 }, 1, rf_blue);

    rf_draw_text("Free 2d camera controls:", 20, 20, 10, rf_black);
    rf_draw_text("- Right/Left to move Offset", 40, 40, 10, rf_dark_gray);
    rf_draw_text("- Mouse Wheel to Zoom in-out", 40, 60, 10, rf_dark_gray);
    rf_draw_text("- A / S to Rotate", 40, 80, 10, rf_dark_gray);
    rf_draw_text("- R to reset Zoom and Rotation", 40, 100, 10, rf_dark_gray);

    rf_end();
}