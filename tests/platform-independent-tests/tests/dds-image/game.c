// Implementation dds images in Rayfork

#include "include/rayfork.h"
#include "glad.h"
#include "sokol_app.h"
#include "malloc.h"

rf_context rf_ctx;
rf_default_render_batch  rf_mem;
rf_default_font font_mem;

const int screen_width = 800;
const int screen_height = 450;

rf_mipmaps_image image;
rf_texture2d texture;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, screen_width, screen_height, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&font_mem);

    image = rf_load_dds_image_from_file(ASSETS_PATH "mario.dds", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);
    texture = rf_load_texture_from_image(image.image);
    int i = glGetError();
    int j = 0;
}

void on_frame(void)
{
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        rf_draw_texture_ex(texture, 0, 0, texture.width * 0.1f, texture.height * 0.1f, 1, RF_WHITE);
    }
    rf_end();
}