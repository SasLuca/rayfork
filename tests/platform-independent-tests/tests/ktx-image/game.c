// Implementation ktx images in Rayfork

#include "include/rayfork.h"
#include "glad.h"
#include "sokol_app.h"
#include "malloc.h"

rf_context rayfork;
rf_default_render_batch renderer_buffers;

const int screen_width = 800;
const int screen_height = 450;

rf_mipmaps_image image;
rf_texture2d texture;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rayfork, &renderer_buffers, screen_width, screen_height, RF_DEFAULT_OPENGL_PROCS);

    const char* path = ASSETS_PATH "ktx_test.ktx";

    image = rf_load_ktx_image_from_file_ez(path);
    texture = rf_load_texture_from_image_with_mipmaps(image);
}

void on_frame(void)
{
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        rf_draw_texture_ex(texture, 0, 0, texture.width * 0.1, texture.height * 0.1, 0, RF_WHITE);
    }
    rf_end();
}