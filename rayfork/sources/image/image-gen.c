#include "rayfork/image/image-gen.h"
#include "stb_perlin/stb_perlin.h"

rf_extern rf_vec2 rf_get_seed_for_cellular_image(int seeds_per_row, int tile_size, int i, rf_rand_proc rand)
{
    rf_vec2 result = {0};

    int y = (i / seeds_per_row) * tile_size + rand(0, tile_size - 1);
    int x = (i % seeds_per_row) * tile_size + rand(0, tile_size - 1);
    result = (rf_vec2) { (float) x, (float) y };

    return result;
}

// Generate image: plain color
rf_extern rf_image rf_gen_image_color_to_buffer(int width, int height, rf_color color, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    for (rf_int i = 0; i < dst_size; i++)
    {
        dst[i] = color;
    }

    return (rf_image) {
        .data   = dst,
        .width  = width,
        .height = height,
        .format = rf_pixel_format_r8g8b8a8,
        .valid  = 1,
    };
}

rf_extern rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    void* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_color_to_buffer(width, height, color, dst, width * height);
    }

    return result;
}

// Generate image: vertical gradient
rf_extern rf_image rf_gen_image_gradient_v_to_buffer(int width, int height, rf_color top, rf_color bottom, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int j = 0; j < height; j++)
        {
            float factor = ((float)j) / ((float)height);

            for (rf_int i = 0; i < width; i++)
            {
                ((rf_color*)dst)[j * width + i].r = (int)((float)bottom.r * factor + (float)top.r * (1.f - factor));
                ((rf_color*)dst)[j * width + i].g = (int)((float)bottom.g * factor + (float)top.g * (1.f - factor));
                ((rf_color*)dst)[j * width + i].b = (int)((float)bottom.b * factor + (float)top.b * (1.f - factor));
                ((rf_color*)dst)[j * width + i].a = (int)((float)bottom.a * factor + (float)top.a * (1.f - factor));
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1
        };
    }

    return result;
}

rf_extern rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_v_to_buffer(width, height, top, bottom, dst, dst_size);
    }

    return result;
}

// Generate image: horizontal gradient
rf_extern rf_image rf_gen_image_gradient_h_to_buffer(int width, int height, rf_color left, rf_color right, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int i = 0; i < width; i++)
        {
            float factor = ((float)i) / ((float)width);

            for (rf_int j = 0; j < height; j++)
            {
                ((rf_color*)dst)[j * width + i].r = (int)((float)right.r * factor + (float)left.r * (1.f - factor));
                ((rf_color*)dst)[j * width + i].g = (int)((float)right.g * factor + (float)left.g * (1.f - factor));
                ((rf_color*)dst)[j * width + i].b = (int)((float)right.b * factor + (float)left.b * (1.f - factor));
                ((rf_color*)dst)[j * width + i].a = (int)((float)right.a * factor + (float)left.a * (1.f - factor));
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1
        };
    }

    return result;
}

rf_extern rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_h_to_buffer(width, height, left, right, dst, dst_size);
    }

    return result;
}

// Generate image: radial gradient
rf_extern rf_image rf_gen_image_gradient_radial_to_buffer(int width, int height, float density, rf_color inner, rf_color outer, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        float radius = (width < height) ? ((float)width) / 2.0f : ((float)height) / 2.0f;

        float center_x = ((float)width ) / 2.0f;
        float center_y = ((float)height) / 2.0f;

        for (rf_int y = 0; y < height; y++)
        {
            for (rf_int x = 0; x < width; x++)
            {
                float dist   = hypotf((float)x - center_x, (float)y - center_y);
                float factor = (dist - radius * density) / (radius * (1.0f - density));

                factor = (float)fmax(factor, 0.f);
                factor = (float)fmin(factor, 1.f); // dist can be bigger than radius so we have to check

                dst[y * width + x].r = (int)((float)outer.r * factor + (float)inner.r * (1.0f - factor));
                dst[y * width + x].g = (int)((float)outer.g * factor + (float)inner.g * (1.0f - factor));
                dst[y * width + x].b = (int)((float)outer.b * factor + (float)inner.b * (1.0f - factor));
                dst[y * width + x].a = (int)((float)outer.a * factor + (float)inner.a * (1.0f - factor));
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_extern rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_radial_to_buffer(width, height, density, inner, outer, dst, dst_size);
    }

    return result;
}

// Generate image: checked
rf_extern rf_image rf_gen_image_checked_to_buffer(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        float radius = (width < height) ? ((float)width) / 2.0f : ((float)height) / 2.0f;

        float center_x = ((float)width ) / 2.0f;
        float center_y = ((float)height) / 2.0f;

        for (rf_int y = 0; y < height; y++)
        {
            for (rf_int x = 0; x < width; x++)
            {
                if ((x / checks_x + y / checks_y) % 2 == 0) dst[y * width + x] = col1;
                else dst[y * width + x] = col2;
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_extern rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_checked_to_buffer(width, height, checks_x, checks_y, col1, col2, dst, dst_size);
    }

    return result;
}

// Generate image: white noise
rf_extern rf_image rf_gen_image_white_noise_to_buffer(int width, int height, float factor, rf_rand_proc rand, rf_color* dst, rf_int dst_size)
{
    int result_image_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_image result = {0};

    if (dst_size < result_image_size || !rand || result_image_size <= 0) return result;

    for (rf_int i = 0; i < width * height; i++)
    {
        if (rand(0, 99) < (int)(factor * 100.0f))
        {
            dst[i] = rf_white;
        }
        else
        {
            dst[i] = rf_black;
        }
    }

    result = (rf_image)
    {
        .data = dst,
        .width = width,
        .height = height,
        .format = rf_pixel_format_r8g8b8a8,
        .valid = 1,
    };

    return result;
}

rf_extern rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_rand_proc rand, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);

    if (!rand || dst_size <= 0) return result;

    rf_color* dst = rf_alloc(allocator, dst_size);
    result = rf_gen_image_white_noise_to_buffer(width, height, factor, rand, dst, dst_size);

    return result;
}

// Generate image: perlin noise
rf_extern rf_image rf_gen_image_perlin_noise_to_buffer(int width, int height, int offset_x, int offset_y, float scale, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int y = 0; y < height; y++)
        {
            for (rf_int x = 0; x < width; x++)
            {
                float nx = (float)(x + offset_x)*scale/(float)width;
                float ny = (float)(y + offset_y)*scale/(float)height;

                // Typical values to start playing with:
                //   lacunarity = ~2.0   -- spacing between successive octaves (use exactly 2.0 for wrapping output)
                //   gain       =  0.5   -- relative weighting applied to each successive octave
                //   octaves    =  6     -- number of "octaves" of noise3() to sum

                // NOTE: We need to translate the data from [-1..1] to [0..1]
                float p = (stb_perlin_fbm_noise3(nx, ny, 1.0f, 2.0f, 0.5f, 6) + 1.0f) / 2.0f;

                int intensity = (int)(p * 255.0f);
                dst[y * width + x] = (rf_color){ intensity, intensity, intensity, 255 };
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_extern rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_perlin_noise_to_buffer(width, height, offset_x, offset_y, scale, dst, dst_size);
    }

    return result;
}

// Generate image: cellular algorithm. Bigger tileSize means bigger cells
rf_extern rf_image rf_gen_image_cellular_to_buffer(int width, int height, int tile_size, rf_rand_proc rand, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    int seeds_per_row = width / tile_size;
    int seeds_per_col = height / tile_size;
    int seeds_count = seeds_per_row * seeds_per_col;

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int y = 0; y < height; y++)
        {
            int tile_y = y / tile_size;

            for (rf_int x = 0; x < width; x++)
            {
                int tile_x = x / tile_size;

                float min_distance = INFINITY;

                // Check all adjacent tiles
                for (rf_int i = -1; i < 2; i++)
                {
                    if ((tile_x + i < 0) || (tile_x + i >= seeds_per_row)) continue;

                    for (rf_int j = -1; j < 2; j++)
                    {
                        if ((tile_y + j < 0) || (tile_y + j >= seeds_per_col)) continue;

                        rf_vec2 neighbor_seed = rf_get_seed_for_cellular_image(seeds_per_row, tile_size, (tile_y + j) * seeds_per_row + tile_x + i, rand);

                        float dist = (float)hypot(x - (int)neighbor_seed.x, y - (int)neighbor_seed.y);
                        min_distance = (float)fmin(min_distance, dist);
                    }
                }

                // I made this up but it seems to give good results at all tile sizes
                int intensity = (int)(min_distance * 256.0f / tile_size);
                if (intensity > 255) intensity = 255;

                dst[y * width + x] = (rf_color) { intensity, intensity, intensity, 255 };
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_extern rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_rand_proc rand, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);

    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_cellular_to_buffer(width, height, tile_size, rand, dst, dst_size);
    }

    return result;
}
