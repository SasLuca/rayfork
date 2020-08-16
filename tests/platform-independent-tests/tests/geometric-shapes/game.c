// Implementation of the geometric shapes example from raylib using rayfork

#include "include/rayfork.h"
#include "glad/glad.h"
#include "sokol_app.h"

rf_context rf_ctx;
rf_default_render_batch rf_mem;
rf_camera3d camera;

const int screen_width = 800;
const int screen_height = 450;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, screen_width, screen_height, RF_DEFAULT_OPENGL_PROCS);

    camera.position = (rf_vec3) {0.0f, 10.0f, 10.0f };
    camera.target   = (rf_vec3) {0.0f, 0.0f, 0.0f };
    camera.up       = (rf_vec3) {0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.type     = RF_CAMERA_PERSPECTIVE;
}

void on_frame(void)
{
    rf_begin();

    rf_clear(RF_RAYWHITE);

    rf_begin_3d(camera);

    rf_draw_cube((rf_vec3) {-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, RF_RED);
    rf_draw_cube_wires((rf_vec3) {-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, RF_GOLD);
    rf_draw_cube_wires((rf_vec3) {-4.0f, 0.0f, -2.0f}, 3.0f, 6.0f, 2.0f, RF_MAROON);

    rf_draw_sphere((rf_vec3) {-1.0f, 0.0f, -2.0f}, 1.0f, RF_GREEN);
    rf_draw_sphere_wires((rf_vec3) {1.0f, 0.0f, 2.0f}, 2.0f, 16, 16, RF_LIME);

    rf_draw_cylinder((rf_vec3) {4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, RF_SKYBLUE);
    rf_draw_cylinder_wires((rf_vec3) {4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, RF_DARKBLUE);
    rf_draw_cylinder_wires((rf_vec3) {4.5f, -1.0f, 2.0f}, 1.0f, 1.0f, 2.0f, 6, RF_BROWN);

    rf_draw_cylinder((rf_vec3) {1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, RF_GOLD);
    rf_draw_cylinder_wires((rf_vec3) {1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, RF_PINK);

    rf_draw_grid(10, 1.0f);

    rf_end_3d();

    rf_end();
}

void on_cleanup(void)
{
    //Empty
}

void on_event(const sapp_event* event)
{
    //Empty
}