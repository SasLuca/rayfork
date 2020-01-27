//Implementation of the geometric shapes example from raylib using rayfork

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork_renderer.h"

rf_renderer_context rf_ctx;
rf_camera3d camera;

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, 640, 480);
    rf_load_font_default();

    camera.position = (rf_vec3) {0.0f, 10.0f, 10.0f };
    camera.target   = (rf_vec3) {0.0f, 0.0f, 0.0f };
    camera.up       = (rf_vec3) {0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.type     = RF_CAMERA_PERSPECTIVE;
}

void on_frame(void)
{
    rf_begin_drawing();

    rf_clear_background(rf_raywhite);

    rf_begin_mode3d(camera);

    rf_draw_cube((rf_vec3) {-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, rf_red);
    rf_draw_cube_wires((rf_vec3) {-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, rf_gold);
    rf_draw_cube_wires((rf_vec3) {-4.0f, 0.0f, -2.0f}, 3.0f, 6.0f, 2.0f, rf_maroon);

    rf_draw_sphere((rf_vec3) {-1.0f, 0.0f, -2.0f}, 1.0f, rf_green);
    rf_draw_sphere_wires((rf_vec3) {1.0f, 0.0f, 2.0f}, 2.0f, 16, 16, rf_lime);

    rf_draw_cylinder((rf_vec3) {4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, rf_skyblue);
    rf_draw_cylinder_wires((rf_vec3) {4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, rf_darkblue);
    rf_draw_cylinder_wires((rf_vec3) {4.5f, -1.0f, 2.0f}, 1.0f, 1.0f, 2.0f, 6, rf_brown);

    rf_draw_cylinder((rf_vec3) {1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, rf_gold);
    rf_draw_cylinder_wires((rf_vec3) {1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, rf_pink);

    rf_draw_grid(10, 1.0f); // Draw a grid

    rf_end_mode3d();

    rf_draw_fps(10, 10);

    rf_end_drawing();
}

void on_cleanup(void)
{
    //Empty
}

void on_event(const sapp_event* event)
{
    //Empty
}