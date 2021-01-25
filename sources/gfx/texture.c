#include "rayfork/gfx/texture.h"
#include "rayfork/gfx/gfx.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/image/image-manipulation.h"
#include "rayfork/image/image-loading.h"
#include "rayfork/image/image-gen.h"

// Load texture from file into GPU memory (VRAM)
rf_extern rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_texture2d result = {0};

    rf_image img = rf_load_image_from_file(filename, temp_allocator, temp_allocator, io);

    result = rf_load_texture_from_image(img);

    rf_image_free(img, temp_allocator);

    return result;
}

// Load texture from an image file data
rf_extern rf_texture2d rf_load_texture_from_file_data(const void* data, rf_int dst_size, rf_allocator temp_allocator)
{
    rf_image img = rf_load_image_from_file_data(data, dst_size, rf_desired_channels_any, temp_allocator, temp_allocator);

    rf_texture2d texture = rf_load_texture_from_image(img);

    rf_image_free(img, temp_allocator);

    return texture;
}

// Load texture from image data
rf_extern rf_texture2d rf_load_texture_from_image(rf_image image)
{
    return rf_load_texture_from_image_with_mipmaps((rf_mipmaps_image) {
        .data = image.data,
        .valid = image.valid,
        .height = image.height,
        .width = image.width,
        .format = image.format,
        .mipmaps = 1
    });
}

rf_extern rf_texture2d rf_load_texture_from_image_with_mipmaps(rf_mipmaps_image image)
{
    rf_texture2d result = {0};

    if (image.valid)
    {
        result.id = rf_gfx_load_texture(image.data, image.width, image.height, image.format, image.mipmaps);

        if (result.id != 0)
        {
            result.width  = image.width;
            result.height = image.height;
            result.format = image.format;
            result.valid  = 1;
        }
    }
    else rf_log(rf_log_type_warning, "rf_texture could not be loaded from rf_image");

    return result;
}

// Load cubemap from image, multiple image cubemap layouts supported
rf_extern rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator)
{
    rf_texture_cubemap cubemap = {0};

    if (layout_type == RF_CUBEMAP_AUTO_DETECT) // Try to automatically guess layout type
    {
        // Check image width/height to determine the type of cubemap provided
        if (image.width > image.height)
        {
            if ((image.width / 6) == image.height) { layout_type = RF_CUBEMAP_LINE_HORIZONTAL; cubemap.width = image.width / 6; }
            else if ((image.width / 4) == (image.height/3)) { layout_type = RF_CUBEMAP_CROSS_FOUR_BY_TREE; cubemap.width = image.width / 4; }
            else if (image.width >= (int)((float)image.height * 1.85f)) { layout_type = RF_CUBEMAP_PANORAMA; cubemap.width = image.width / 4; }
        }
        else if (image.height > image.width)
        {
            if ((image.height / 6) == image.width) { layout_type = RF_CUBEMAP_LINE_VERTICAL; cubemap.width = image.height / 6; }
            else if ((image.width / 3) == (image.height/4)) { layout_type = RF_CUBEMAP_CROSS_THREE_BY_FOUR; cubemap.width = image.width / 3; }
        }

        cubemap.height = cubemap.width;
    }

    if (layout_type != RF_CUBEMAP_AUTO_DETECT)
    {
        int size = cubemap.width;

        rf_image faces = {0}; // Vertical column image
        rf_rec face_recs[6] = {0}; // Face source rectangles
        for (rf_int i = 0; i < 6; i++) face_recs[i] = (rf_rec) { 0, 0, size, size };

        if (layout_type == RF_CUBEMAP_LINE_VERTICAL)
        {
            faces = image;
            for (rf_int i = 0; i < 6; i++) face_recs[i].y = size*i;
        }
        else if (layout_type == RF_CUBEMAP_PANORAMA)
        {
            // TODO: Convert panorama image to square faces...
            // Ref: https://github.com/denivip/panorama/blob/master/panorama.cpp
        }
        else
        {
            if (layout_type == RF_CUBEMAP_LINE_HORIZONTAL) { for (rf_int i = 0; i < 6; i++) { face_recs[i].x = size * i; } }
            else if (layout_type == RF_CUBEMAP_CROSS_THREE_BY_FOUR)
            {
                face_recs[0].x = size; face_recs[0].y = size;
                face_recs[1].x = size; face_recs[1].y = 3*size;
                face_recs[2].x = size; face_recs[2].y = 0;
                face_recs[3].x = size; face_recs[3].y = 2*size;
                face_recs[4].x = 0; face_recs[4].y = size;
                face_recs[5].x = 2*size; face_recs[5].y = size;
            }
            else if (layout_type == RF_CUBEMAP_CROSS_FOUR_BY_TREE)
            {
                face_recs[0].x = 2*size; face_recs[0].y = size;
                face_recs[1].x = 0; face_recs[1].y = size;
                face_recs[2].x = size; face_recs[2].y = 0;
                face_recs[3].x = size; face_recs[3].y = 2*size;
                face_recs[4].x = size; face_recs[4].y = size;
                face_recs[5].x = 3*size; face_recs[5].y = size;
            }

            // Convert image data to 6 faces in a vertical column, that's the optimum layout for loading
            rf_image faces_colors = rf_gen_image_color(size, size * 6, rf_magenta, temp_allocator);
            faces = rf_image_format(faces_colors, image.format, temp_allocator);
            rf_image_free(faces_colors, temp_allocator);

            // TODO: rf_image formating does not work with compressed textures!
        }

        for (rf_int i = 0; i < 6; i++)
        {
            rf_image_draw(&faces, image, face_recs[i], (rf_rec) {0, size * i, size, size }, rf_white, temp_allocator);
        }

        cubemap.id = rf_gfx_load_texture_cubemap(faces.data, size, faces.format);

        if (cubemap.id == 0) { rf_log(rf_log_type_warning, "Cubemap image could not be loaded."); }

        rf_image_free(faces, temp_allocator);
    }
    else rf_log(rf_log_type_warning, "Cubemap image layout can not be detected.");

    return cubemap;
}

// Load texture for rendering (framebuffer)
rf_extern rf_render_texture2d rf_load_render_texture(int width, int height)
{
    rf_render_texture2d target = rf_gfx_load_render_texture(width, height, rf_pixel_format_r8g8b8a8, 24, 0);

    return target;
}

// Update GPU texture with new data. Pixels data must match texture.format
rf_extern void rf_update_texture(rf_texture2d texture, const void* pixels, rf_int pixels_size)
{
    rf_gfx_update_texture(texture.id, texture.width, texture.height, texture.format, pixels, pixels_size);
}

// Generate GPU mipmaps for a texture
rf_extern void rf_gen_texture_mipmaps(rf_texture2d* texture)
{
    // NOTE: NPOT textures support check inside function
    // On WebGL (OpenGL ES 2.0) NPOT textures support is limited
    rf_gfx_generate_mipmaps(texture);
}

// Set texture wrapping mode
rf_extern void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode)
{
    rf_gfx_set_texture_wrap(texture, wrap_mode);
}

// Set texture scaling filter mode
rf_extern void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode)
{
    rf_gfx_set_texture_filter(texture, filter_mode);
}

// Unload texture from GPU memory (VRAM)
rf_extern void rf_texture_free(rf_texture2d texture)
{
    if (texture.id > 0)
    {
        rf_gfx_delete_textures(texture.id);

        rf_log(rf_log_type_info, "[TEX ID %i] Unloaded texture data from VRAM (GPU)", texture.id);
    }
}

// Unload render texture from GPU memory (VRAM)
rf_extern void rf_unload_render_texture(rf_render_texture2d target)
{
    if (target.id > 0)
    {
        rf_gfx_delete_render_textures(target);

        rf_log(rf_log_type_info, "[TEX ID %i] Unloaded render texture data from VRAM (GPU)", target.id);
    }
}