//Implementation of the geometric shapes example from raylib using rayfork

#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork.h"

rf_context rf_ctx;
rf_camera3d camera;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, 640, 480);
    rf_load_font_default(&rf_ctx);

    camera.position = (rf_vector3) { 0.0f, 10.0f, 10.0f };
    camera.target   = (rf_vector3) { 0.0f, 0.0f, 0.0f };
    camera.up       = (rf_vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.type     = rf_camera_perspective;
}

void on_frame(void)
{
    rf_begin_drawing(&rf_ctx);

    rf_clear_background(rf_raywhite);

    rf_begin_mode3d(&rf_ctx, camera);

    rf_draw_cube(&rf_ctx, (rf_vector3) {-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, rf_red);
    rf_draw_cube_wires(&rf_ctx, (rf_vector3) {-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, rf_gold);
    rf_draw_cube_wires(&rf_ctx, (rf_vector3) {-4.0f, 0.0f, -2.0f}, 3.0f, 6.0f, 2.0f, rf_maroon);

    rf_draw_sphere(&rf_ctx, (rf_vector3) {-1.0f, 0.0f, -2.0f}, 1.0f, rf_green);
    rf_draw_sphere_wires(&rf_ctx, (rf_vector3) {1.0f, 0.0f, 2.0f}, 2.0f, 16, 16, rf_lime);

    rf_draw_cylinder(&rf_ctx, (rf_vector3) {4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, rf_skyblue);
    rf_draw_cylinder_wires(&rf_ctx, (rf_vector3) {4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, rf_darkblue);
    rf_draw_cylinder_wires(&rf_ctx, (rf_vector3) {4.5f, -1.0f, 2.0f}, 1.0f, 1.0f, 2.0f, 6, rf_brown);

    rf_draw_cylinder(&rf_ctx, (rf_vector3) {1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, rf_gold);
    rf_draw_cylinder_wires(&rf_ctx, (rf_vector3) {1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, rf_pink);

    rf_draw_grid(&rf_ctx, 10, 1.0f); // Draw a grid

    rf_end_mode3d(&rf_ctx);

    rf_draw_fps(&rf_ctx, 10, 10);

    rf_end_drawing(&rf_ctx);
}

void on_cleanup(void)
{
    //Empty
}

void on_event(const sapp_event* event)
{
    //Empty
}