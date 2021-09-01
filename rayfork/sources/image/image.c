#include "rayfork/image/image.h"
#include "rayfork/foundation/logger.h"

rf_extern rf_int rf_image_size(rf_image image)
{
    rf_int result = rf_image_size_in_format(image.width, image.height, image.format);
    return result;
}

// Extract color palette from image to maximum size
rf_extern void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, rf_int palette_size)
{
    if (rf_is_uncompressed_format(image.format))
    {
        if (palette_size > 0)
        {
            int img_size = rf_image_size(image);
            int img_bpp  = rf_bytes_per_pixel(image.format);
            const unsigned char* img_data = (unsigned char*) image.data;

            for (rf_int img_iter = 0, palette_iter = 0; img_iter < img_size && palette_iter < palette_size; img_iter += img_bpp)
            {
                rf_color color = rf_format_one_pixel_to_rgba32(img_data, image.format);

                bool color_found = 0;

                for (rf_int i = 0; i < palette_iter; i++)
                {
                    if (rf_color_match(palette_dst[i], color))
                    {
                        color_found = 1;
                        break;
                    }
                }

                if (!color_found)
                {
                    palette_dst[palette_iter] = color;
                    palette_iter++;
                }
            }
        }
        else rf_log(rf_log_type_warning, "Palette size was 0.");
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);
}

rf_extern rf_palette rf_image_extract_palette(rf_image image, rf_int palette_size, rf_allocator allocator)
{
    rf_palette result = {0};

    if (rf_is_uncompressed_format(image.format))
    {
        rf_color* dst = rf_alloc(allocator, sizeof(rf_color) * palette_size);
        rf_image_extract_palette_to_buffer(image, dst, palette_size);

        result.colors = dst;
        result.size   = palette_size;
    }

    return result;
}

// Get image alpha border rectangle
rf_extern rf_rec rf_image_alpha_border(rf_image image, float threshold)
{
    rf_rec crop = {0};

    if (rf_is_uncompressed_format(image.format))
    {
        int x_min = 65536; // Define a big enough number
        int x_max = 0;
        int y_min = 65536;
        int y_max = 0;

        int src_bpp = rf_bytes_per_pixel(image.format);
        int src_size = rf_image_size(image);
        unsigned char* src = image.data;

        for (rf_int y = 0; y < image.height; y++)
        {
            for (rf_int x = 0; x < image.width; x++)
            {
                int src_pos = (y * image.width + x) * src_bpp;

                rf_color rgba32_pixel = rf_format_one_pixel_to_rgba32(&src[src_pos], image.format);

                if (rgba32_pixel.a > (unsigned char)(threshold * 255.0f))
                {
                    if (x < x_min) x_min = x;
                    if (x > x_max) x_max = x;
                    if (y < y_min) y_min = y;
                    if (y > y_max) y_max = y;
                }
            }
        }

        crop = (rf_rec) { x_min, y_min, (x_max + 1) - x_min, (y_max + 1) - y_min };
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return crop;
}

rf_extern rf_pixel_buffer rf_image_to_pixel_buffer(rf_image image)
{
    rf_pixel_buffer result = {
        .data = image.data,
        .format = image.format,
        .size = rf_image_size(image),
        .valid = image.valid,
    };

    return result;
}

rf_extern rf_image rf_pixel_buffer_to_image(rf_pixel_buffer pixel_buffer, rf_int width, rf_int height)
{
    rf_image result = {
        .data = pixel_buffer.data,
        .width = width,
        .height = height,
        .format = pixel_buffer.format,
        .valid = pixel_buffer.valid,
    };

    return result;
}

rf_extern void rf_image_free(rf_image image, rf_allocator allocator)
{
    rf_free(allocator, image.data);
}

rf_extern rf_int rf_mipmaps_image_size(rf_mipmaps_image image)
{
    int size   = 0;
    int width  = image.width;
    int height = image.height;

    for (rf_int i = 0; i < image.mipmaps; i++)
    {
        size += width * height * rf_bytes_per_pixel(image.format);

        width  /= 2;
        height /= 2;

        // Security check for NPOT textures
        if (width  < 1) width  = 1;
        if (height < 1) height = 1;
    }

    return size;
}

rf_extern void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator)
{
    rf_free(allocator, image.data);
}

rf_extern rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count)
{
    if (!image.valid) return (rf_mipmaps_stats) {0};

    int possible_mip_count = 1;
    int mipmaps_size = rf_image_size(image);

    int mip_width = image.width;
    int mip_height = image.height;

    while (mip_width != 1 || mip_height != 1 || possible_mip_count == desired_mipmaps_count)
    {
        if (mip_width  != 1) mip_width  /= 2;
        if (mip_height != 1) mip_height /= 2;

        // Safety check for NPOT textures
        if (mip_width  < 1) mip_width  = 1;
        if (mip_height < 1) mip_height = 1;

        mipmaps_size += mip_width * mip_height * rf_bytes_per_pixel(image.format);

        possible_mip_count++;
    }

    return (rf_mipmaps_stats) { possible_mip_count, mipmaps_size };
}