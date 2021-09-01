#include "rayfork/image/image-manipulation.h"

#include "rayfork/image/image-gen.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/internal/foundation/assert.h"
#include "rayfork/foundation/numeric-limits.h"
#include "rayfork/foundation/min-max.h"
#include "rayfork/foundation/rand.h"
#include "stb_image_resize/stb_image_resize.h"
#include "stb_perlin/stb_perlin.h"
#include "string.h"
#include "math.h"

/**
 * Copy an existing image into a buffer.
 * @param image a valid image to copy from.
 * @param dst a buffer for the resulting image.
 * @param dst_size size of the `dst` buffer.
 * @return a deep copy of the image into the provided `dst` buffer.
 */
rf_extern rf_image rf_image_copy_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        int width  = image.width;
        int height = image.height;
        int size   = width * height * rf_bytes_per_pixel(image.format);

        if (dst_size >= size)
        {
            memcpy(dst, image.data, size);

            result.data    = dst;
            result.width   = image.width;
            result.height  = image.height;
            result.format  = image.format;
            result.valid   = 1;
        }
        else rf_log_error(rf_bad_argument, "Destination buffer is too small. Expected at least %d bytes but was %d", size, dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image was invalid.");

    return result;
}

/**
 * Copy an existing image.
 * @param image a valid image to copy from.
 * @param allocator used to allocate the new buffer for the resulting image.
 * @return a deep copy of the image.
 */
rf_extern rf_image rf_image_copy(rf_image image, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int size  = image.width * image.height * rf_bytes_per_pixel(image.format);
        void* dst = rf_alloc(allocator, size);

        if (dst)
        {
            result = rf_image_copy_to_buffer(image, dst, size);
        }
        else rf_log_error(rf_bad_alloc, "Failed to allocate %d bytes", size);
    }
    else rf_log_error(rf_bad_argument, "Image was invalid.");

    return result;
}

/**
 * Crop an image and store the result in a provided buffer.
 * @param image a valid image that we crop from.
 * @param crop a rectangle representing which part of the image to crop.
 * @param dst a buffer for the resulting image. Must be of size at least `rf_image_size_in_format(RF_UNCOMPRESSED_R8G8B8A8, crop.width, crop.height)`.
 * @param dst_size size of the `dst` buffer.
 * @return a cropped image using the `dst` buffer in the same format as `image`.
 */
rf_extern rf_image rf_image_crop_to_buffer(rf_image image, rf_rec crop, void* dst, rf_int dst_size, rf_pixel_format dst_format)
{
    rf_image result = {0};

    if (image.valid)
    {
        // Security checks to validate crop rectangle
        if (crop.x < 0) {
            crop.width += crop.x; crop.x = 0;
        }
        if (crop.y < 0) {
            crop.height += crop.y; crop.y = 0;
        }
        if ((crop.x + crop.width) > image.width) {
            crop.width = image.width - crop.x;
        }
        if ((crop.y + crop.height) > image.height) {
            crop.height = image.height - crop.y;
        }

        if ((crop.x < image.width) && (crop.y < image.height))
        {
            rf_int expected_size = rf_image_size_in_format(crop.width, crop.height, dst_format);
            if (dst_size >= expected_size)
            {
                rf_pixel_format src_format = image.format;
                rf_int src_size = rf_image_size(image);

                unsigned char* src_ptr = image.data;
                unsigned char* dst_ptr = dst;

                rf_int src_bpp = rf_bytes_per_pixel(image.format);
                rf_int dst_bpp = rf_bytes_per_pixel(dst_format);

                for (rf_int y = 0; y < crop.height; y++)
                {
                    for (rf_int x = 0; x < crop.width; x++)
                    {
                        rf_int src_x = x + crop.x;
                        rf_int src_y = y + crop.y;

                        rf_int src_pixel = (src_y * image.width + src_x) * src_bpp;
                        rf_int dst_pixel = (y * crop.width + x) * src_bpp;
                        rf_assert(src_pixel < src_size);
                        rf_assert(dst_pixel < dst_size);

                        rf_format_one_pixel(&src_ptr[src_pixel], src_format, &dst_ptr[dst_pixel], dst_format);
                    }
                }

                result.data   = dst;
                result.format = dst_format;
                result.width  = crop.width;
                result.height = crop.height;
                result.valid  = 1;
            }
            else rf_log_error(rf_bad_argument, "Destination buffer is too small. Expected at least %d bytes but was %d", expected_size, dst_size);
        }
        else rf_log_error(rf_bad_argument, "Image can not be cropped, crop rectangle out of bounds.");
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

/**
 * Crop an image and store the result in a provided buffer.
 * @param image a valid image that we crop from.
 * @param crop a rectangle representing which part of the image to crop.
 * @param dst a buffer for the resulting image. Must be of size at least `rf_image_size_in_format(RF_UNCOMPRESSED_R8G8B8A8, crop.width, crop.height)`.
 * @param dst_size size of the `dst` buffer.
 * @return a cropped image using the `dst` buffer in the same format as `image`.
 */
rf_extern rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int size = rf_image_size_in_format(crop.width, crop.height, image.format);
        void* dst = rf_alloc(allocator, size);

        if (dst)
        {
            result = rf_image_crop_to_buffer(image, crop, dst, size, image.format);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

rf_internal int rf_format_to_stb_channel_count(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale: return 1;
        case rf_pixel_format_gray_alpha: return 2;
        case rf_pixel_format_r8g8b8: return 3;
        case rf_pixel_format_r8g8b8a8: return 4;
        default: return 0;
    }
}

// Resize and image to new size.
// Note: Uses stb default scaling filters (both bicubic): STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL (high-quality Catmull-Rom)
rf_extern rf_image rf_image_resize_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size, rf_allocator temp_allocator)
{
    if (!image.valid || dst_size < new_width * new_height * rf_bytes_per_pixel(image.format)) return (rf_image){0};

    rf_image result = {0};

    int stb_format = rf_format_to_stb_channel_count(image.format);

    if (stb_format)
    {
        rf_global_allocator_for_thirdparty_libraries = (temp_allocator);
        stbir_resize_uint8((unsigned char*) image.data, image.width, image.height, 0, (unsigned char*) dst, new_width, new_height, 0, stb_format);
        rf_global_allocator_for_thirdparty_libraries = ((rf_allocator) {0});

        result.data   = dst;
        result.width  = new_width;
        result.height = new_height;
        result.format = image.format;
        result.valid  = 1;
    }
    else // if the format of the image is not supported by stbir we need to first format the pixels to a supported format like rgba32
    {
        int pixels_size = image.width * image.height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
        rf_color* pixels = rf_alloc(temp_allocator, pixels_size);

        if (pixels)
        {
            rf_pixel_buffer temp_pixel_buffer = rf_format_pixels_to_rgba32_to_buffer(rf_image_to_pixel_buffer(image), pixels, pixels_size);
            rf_assert(bufer.valid);

            rf_global_allocator_for_thirdparty_libraries = (temp_allocator);
            stbir_resize_uint8((unsigned char*)pixels, image.width, image.height, 0, (unsigned char*) dst, new_width, new_height, 0, 4);
            rf_global_allocator_for_thirdparty_libraries = ((rf_allocator) {0});

            temp_pixel_buffer = rf_format_pixels_to_buffer(temp_pixel_buffer, dst, dst_size, image.format);
            rf_assert(format_success);

            result.data   = dst;
            result.width  = new_width;
            result.height = new_height;
            result.format = image.format;
            result.valid  = 1;
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", image.width * image.height * sizeof(rf_color));

        rf_free(temp_allocator, pixels);
    }

    return result;
}

rf_extern rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int dst_size = new_width * new_height * rf_bytes_per_pixel(image.format);
        void* dst = rf_alloc(allocator, dst_size);

        if (dst)
        {
            result = rf_image_resize_to_buffer(image, new_width, new_height, dst, dst_size, temp_allocator);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

/**
 * Resize and image to new size using Nearest-Neighbor scaling algorithm
 * @param image
 * @param new_width
 * @param new_height
 * @param dst
 * @param dst_size
 * @return a resized version of the `image`, with the `dst` buffer, in the same format.
 */
rf_extern rf_image rf_image_resize_nn_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        int bpp = rf_bytes_per_pixel(image.format);
        int expected_size = new_width * new_height * bpp;

        if (dst_size >= expected_size)
        {
            // EDIT: added +1 to account for an early rounding problem
            int x_ratio = (int)((image.width  << 16) / new_width ) + 1;
            int y_ratio = (int)((image.height << 16) / new_height) + 1;

            unsigned char* src = image.data;

            int x2, y2;
            for (rf_int y = 0; y < new_height; y++)
            {
                for (rf_int x = 0; x < new_width; x++)
                {
                    x2 = ((x * x_ratio) >> 16);
                    y2 = ((y * y_ratio) >> 16);

                    rf_format_one_pixel(src + ((y2 * image.width) + x2) * bpp, image.format,
                                        ((unsigned char *) dst) + ((y * new_width) + x) * bpp, image.format);
                }
            }

            result.data   = dst;
            result.width  = new_width;
            result.height = new_height;
            result.format = image.format;
            result.valid  = 1;
        }
        else rf_log_error(rf_bad_buffer_size, "Expected `dst` to be at least %d bytes but was %d bytes", expected_size, dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

rf_extern rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int dst_size = new_width * new_height * rf_bytes_per_pixel(image.format);
        void* dst = rf_alloc(allocator, dst_size);

        if (dst)
        {
            result = rf_image_resize_nn_to_buffer(image, new_width, new_height, dst, dst_size);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

// Convert image data to desired format
rf_extern rf_image rf_image_format_to_buffer(rf_image image, rf_pixel_format dst_format, void* dst, rf_int dst_size)
{
    rf_pixel_buffer buffer = rf_format_pixels_to_buffer(rf_image_to_pixel_buffer(image), dst, dst_size, dst_format);
    rf_image result = rf_pixel_buffer_to_image(buffer, image.width, image.height);

    return result;
}

rf_extern rf_image rf_image_format(rf_image image, rf_pixel_format new_format, rf_allocator allocator)
{
    rf_pixel_buffer buffer = rf_format_pixels(rf_image_to_pixel_buffer(image), new_format, allocator);
    rf_image result = rf_pixel_buffer_to_image(buffer, image.width, image.height);

    return result;
}

// Apply alpha mask to image. Note 1: Returned image is GRAY_ALPHA (16bit) or RGBA (32bit). Note 2: alphaMask should be same size as image
rf_extern rf_image rf_image_alpha_mask_to_buffer(rf_image image, rf_image alpha_mask, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid && alpha_mask.valid)
    {
        if (image.width == alpha_mask.width && image.height == alpha_mask.height)
        {
            if (rf_is_compressed_format(image.format) && alpha_mask.format == rf_pixel_format_grayscale)
            {
                if (dst_size >= rf_image_size_in_format(image.width, image.height, rf_pixel_format_gray_alpha))
                {
                    // Apply alpha mask to alpha channel
                    for (rf_int i = 0; i < image.width * image.height; i++)
                    {
                        unsigned char mask_pixel = 0;
                        rf_format_one_pixel(((unsigned char*)alpha_mask.data) + i, alpha_mask.format, &mask_pixel, rf_pixel_format_grayscale);

                        // Todo: Finish implementing this function
                        //((unsigned char*)dst)[k] = mask_pixel;
                    }

                    result.data   = dst;
                    result.width  = image.width;
                    result.height = image.height;
                    result.format = rf_pixel_format_gray_alpha;
                    result.valid  = 1;
                }
            } else rf_log_error(rf_bad_argument, "Expected compressed pixel formats. `image.format`: %d, `alpha_mask.format`: %d", image.format, alpha_mask.format);
        } else rf_log_error(rf_bad_argument, "Alpha mask must be same size as image but was w: %d, h: %d", alpha_mask.width, alpha_mask.height);
    } else rf_log_error(rf_bad_argument, "One image was invalid. `image.valid`: %d, `alpha_mask.valid`: %d", image.valid, alpha_mask.valid);

    return result;
}

// Clear alpha channel to desired color. Note: Threshold defines the alpha limit, 0.0f to 1.0f
rf_extern rf_image rf_image_alpha_clear(rf_image image, rf_color color, float threshold)
{
    rf_image result = {0};

    if (image.valid)
    {
        switch (image.format)
        {
            // formats with alpha channel
            case rf_pixel_format_gray_alpha:
            {
                uint8_t threshold_value = threshold * 255.0f;
                uint8_t* dst = image.data;
                for (int i = 1; i < image.width * image.height * 2; i += 2)
                {
                    // branchless version of: if (dst[i] <= threshold_value)
                    bool upto_threshold = dst[i] <= threshold_value;
                    dst[i - 1] = upto_threshold * color.r;
                    dst[i] = upto_threshold * color.a;
                }
            }
            case rf_pixel_format_r5g5b5a1:
            {
                uint8_t threshold_value = threshold < 0.5f;

                uint8_t r = (uint8_t)(round((float)color.r * 31.0f));
                uint8_t g = (uint8_t)(round((float)color.g * 31.0f));
                uint8_t b = (uint8_t)(round((float)color.b * 31.0f));
                uint8_t a = (color.a < 128) ? 0 : 1;

                uint16_t* dst = image.data;

                for (int i = 0; i < image.width * image.height; i++)
                {
                    // branchless version of: if ((dst[i] & 0b0000000000000001) <= threshold_value) ...
                    dst[i] = ((dst[i] & 0b0000000000000001) <= threshold_value) * (uint16_t)r << 11 | (uint16_t)g << 6 | (uint16_t)b << 1 | (uint16_t)a;
                }

                result = image;
                break;
            }
            case rf_pixel_format_r4g4b4a4:
            {
                uint8_t threshold_value = (uint8_t)(threshold * 15.0f);
                uint8_t r = (uint8_t)(round(((float)color.r) * 15.0f));
                uint8_t g = (uint8_t)(round(((float)color.g) * 15.0f));
                uint8_t b = (uint8_t)(round(((float)color.b) * 15.0f));
                uint8_t a = (uint8_t)(round(((float)color.a) * 15.0f));

                uint16_t* dst = image.data;

                for (int i = 0; i < image.width * image.height; i++)
                {
                    // branchless version of: if ((dst[i] & 0x000f) <= thresholdValue) ...
                    dst[i] = ((dst[i] & 0x000f) <= threshold_value) * (((uint16_t)r << 12) | ((uint16_t)g << 8) | ((uint16_t)b << 4) | ((uint16_t)a));
                }
                result = image;
                break;
            }
            case rf_pixel_format_r8g8b8a8:
            {
                rf_color* dst = image.data;
                for (rf_int i = 0; i < image.width * image.height; i++)
                {
                    if (dst[i].a <= threshold * 255.0f)
                    {
                        dst[i] = color;
                    }
                }
                result = image;
                break;
            }
            case rf_pixel_format_r32g32b32a32:
            {
                rf_vec4* dst = image.data;
                for (rf_int i = 0; i < image.width * image.height; i++)
                {
                    if (dst[i].z <= threshold * 255.0f)
                    {
                        dst[i] = rf_color_normalize(color);
                    }
                }
                result = image;
                break;
            }

            // compressed formats
            case rf_pixel_format_dxt1_rgb:
            case rf_pixel_format_dxt1_rgba:
            case rf_pixel_format_dxt3_rgba:
            case rf_pixel_format_dxt5_rgba:
            case rf_pixel_format_etc1_rgb:
            case rf_pixel_format_etc2_rgb:
            case rf_pixel_format_etc2_eac_rgba:
            case rf_pixel_format_pvrt_rgb:
            case rf_pixel_format_prvt_rgba:
            case rf_pixel_format_astc_4x4_rgba:
            case rf_pixel_format_astc_8x8_rgba:
            {
                rf_log_error(rf_bad_argument, "Image must be in an uncompressed format but was in a compressed format. image.format: %s", rf_pixel_format_to_str(image.format).data);
                break;
            }

            // formats without alpha channel
            default:
            {
                result = image;
                break;
            }
        }
    }
    else rf_log_error(rf_bad_argument, "Source pixel buffer is invalid.");

    return result;
}

// Premultiply alpha channel
// @Note(LucaSas): can be optimized by handling all cases in which a format has an alpha channel properly
rf_extern rf_image rf_image_alpha_premultiply(rf_image image, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (rf_has_alpha_channel(image.format))
    {
        float alpha = 0.0f;
        rf_pixel_buffer pixels = rf_format_pixels_to_rgba32(rf_image_to_pixel_buffer(image), temp_allocator);

        if (pixels.valid)
        {
            rf_color* dst = pixels.data;
            for (rf_int i = 0; i < image.width * image.height; i++)
            {
                alpha = (float)dst[i].a / 255.0f;
                dst[i].r = (uint8_t)((float)dst[i].r * alpha);
                dst[i].g = (uint8_t)((float)dst[i].g * alpha);
                dst[i].b = (uint8_t)((float)dst[i].b * alpha);
            }

            rf_image temp_image = rf_pixel_buffer_to_image(pixels, image.width, image.height);

            result = rf_image_format(temp_image, image.format, allocator);
        }

        rf_free(temp_allocator, pixels.data);
    }
    else
    {
        result = rf_image_copy(image, allocator);
    }

    return result;
}

rf_extern rf_rec rf_image_alpha_crop_rec(rf_image image, float threshold)
{
    if (!image.valid) return (rf_rec){0};

    int bpp = rf_bytes_per_pixel(image.format);

    int x_min = rf_int32_max;
    int x_max = 0;
    int y_min = rf_int32_max;
    int y_max = 0;

    char* src = image.data;

    for (rf_int y = 0; y < image.height; y++)
    {
        for (rf_int x = 0; x < image.width; x++)
        {
            int pixel = (y * image.width + x) * bpp;
            rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(&src[pixel], image.format);

            if (pixel_rgba32.a > (unsigned char)(threshold * 255.0f))
            {
                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
                if (y < y_min) y_min = y;
                if (y > y_max) y_max = y;
            }
        }
    }

    return (rf_rec) { x_min, y_min, (x_max + 1) - x_min, (y_max + 1) - y_min };
}

// Crop image depending on alpha value
rf_extern rf_image rf_image_alpha_crop(rf_image image, float threshold, rf_allocator allocator)
{
    rf_rec crop = rf_image_alpha_crop_rec(image, threshold);

    return rf_image_crop(image, crop, allocator);
}

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering) Note: In case selected bpp do not represent an known 16bit format, dithered data is stored in the LSB part of the unsigned short
rf_extern rf_image rf_image_dither(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (rf_is_compressed_format(image.format))
    {
        rf_log_error(rf_bad_argument, "Expected image in uncompressed format. image.format: %s", rf_pixel_format_to_str(image.format).data);
        return (rf_image) {0};
    }

    rf_image result = {0};

    if (image.format != rf_pixel_format_r8g8b8 && image.format != rf_pixel_format_r8g8b8a8)
    {
        rf_log(rf_log_type_warning, "Image format is already 16bpp or lower, dithering could have no effect");
    }

    rf_pixel_format result_format = 0;
    if ((r_bpp == 5) && (g_bpp == 6) && (b_bpp == 5) && (a_bpp == 0))      result_format = rf_pixel_format_r5g6b5;
    else if ((r_bpp == 5) && (g_bpp == 5) && (b_bpp == 5) && (a_bpp == 1)) result_format = rf_pixel_format_r5g5b5a1;
    else if ((r_bpp == 4) && (g_bpp == 4) && (b_bpp == 4) && (a_bpp == 4)) result_format = rf_pixel_format_r4g4b4a4;

    if (result_format)
    {
        rf_pixel_buffer temp_pixel_buffer = rf_format_pixels_to_rgba32(rf_image_to_pixel_buffer(image), temp_allocator);

        if (temp_pixel_buffer.valid)
        {
            rf_int result_size = image.width * image.height * sizeof(uint16_t);
            uint16_t* result_data = rf_alloc(allocator, result_size);
            rf_color* pixels = temp_pixel_buffer.data;

            rf_color old_pixel = rf_white;
            rf_color new_pixel = rf_white;

            int32_t  r_error = 0;
            int32_t  g_error = 0;
            int32_t  b_error = 0;
            uint16_t r_pixel = 0;
            uint16_t g_pixel = 0;
            uint16_t b_pixel = 0;
            uint16_t a_pixel = 0;

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    old_pixel = pixels[y * image.width + x];

                    // NOTE: New pixel obtained by bits truncate, it would be better to round values (check rf_image_format())
                    new_pixel.r = old_pixel.r >> (8 - r_bpp); // R bits
                    new_pixel.g = old_pixel.g >> (8 - g_bpp); // G bits
                    new_pixel.b = old_pixel.b >> (8 - b_bpp); // B bits
                    new_pixel.a = old_pixel.a >> (8 - a_bpp); // A bits (not used on dithering)

                    // NOTE: Error must be computed between new and old pixel but using same number of bits!
                    // We want to know how much color precision we have lost...
                    r_error = (int)old_pixel.r - (int)(new_pixel.r << (8 - r_bpp));
                    g_error = (int)old_pixel.g - (int)(new_pixel.g << (8 - g_bpp));
                    b_error = (int)old_pixel.b - (int)(new_pixel.b << (8 - b_bpp));

                    pixels[y*image.width + x] = new_pixel;

                    // NOTE: Some cases are out of the array and should be ignored
                    if (x < (image.width - 1))
                    {
                        pixels[y*image.width + x+1].r = rf_min_i((int)pixels[y * image.width + x + 1].r + (int)((float)r_error * 7.0f / 16), 0xff);
                        pixels[y*image.width + x+1].g = rf_min_i((int)pixels[y * image.width + x + 1].g + (int)((float)g_error * 7.0f / 16), 0xff);
                        pixels[y*image.width + x+1].b = rf_min_i((int)pixels[y * image.width + x + 1].b + (int)((float)b_error * 7.0f / 16), 0xff);
                    }

                    if ((x > 0) && (y < (image.height - 1)))
                    {
                        pixels[(y+1)*image.width + x-1].r = rf_min_i((int)pixels[(y + 1) * image.width + x - 1].r + (int)((float)r_error * 3.0f / 16), 0xff);
                        pixels[(y+1)*image.width + x-1].g = rf_min_i((int)pixels[(y + 1) * image.width + x - 1].g + (int)((float)g_error * 3.0f / 16), 0xff);
                        pixels[(y+1)*image.width + x-1].b = rf_min_i((int)pixels[(y + 1) * image.width + x - 1].b + (int)((float)b_error * 3.0f / 16), 0xff);
                    }

                    if (y < (image.height - 1))
                    {
                        pixels[(y+1)*image.width + x].r = rf_min_i((int)pixels[(y+1)*image.width + x].r + (int)((float)r_error*5.0f/16), 0xff);
                        pixels[(y+1)*image.width + x].g = rf_min_i((int)pixels[(y+1)*image.width + x].g + (int)((float)g_error*5.0f/16), 0xff);
                        pixels[(y+1)*image.width + x].b = rf_min_i((int)pixels[(y+1)*image.width + x].b + (int)((float)b_error*5.0f/16), 0xff);
                    }

                    if ((x < (image.width - 1)) && (y < (image.height - 1)))
                    {
                        pixels[(y+1)*image.width + x+1].r = rf_min_i((int)pixels[(y+1)*image.width + x+1].r + (int)((float)r_error*1.0f/16), 0xff);
                        pixels[(y+1)*image.width + x+1].g = rf_min_i((int)pixels[(y+1)*image.width + x+1].g + (int)((float)g_error*1.0f/16), 0xff);
                        pixels[(y+1)*image.width + x+1].b = rf_min_i((int)pixels[(y+1)*image.width + x+1].b + (int)((float)b_error*1.0f/16), 0xff);
                    }

                    r_pixel = (unsigned short)new_pixel.r;
                    g_pixel = (unsigned short)new_pixel.g;
                    b_pixel = (unsigned short)new_pixel.b;
                    a_pixel = (unsigned short)new_pixel.a;

                    ((unsigned short *)image.data)[y*image.width + x] = (r_pixel << (g_bpp + b_bpp + a_bpp)) | (g_pixel << (b_bpp + a_bpp)) | (b_pixel << a_bpp) | a_pixel;
                }
            }
        }

        rf_free(temp_allocator, temp_pixel_buffer.data);
    }
    else rf_log_error(rf_bad_argument, "Unsupported dithered OpenGL internal format: %ibpp (R%i_g%i_b%i_a%i)", (r_bpp + g_bpp + b_bpp + a_bpp), r_bpp, g_bpp, b_bpp, a_bpp);

    return result;
}

// Flip image vertically
rf_extern rf_image rf_image_flip_vertical_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (y * image.width + x) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + ((image.height - 1 - y) * image.width + x) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_flip_vertical(rf_image image, rf_allocator allocator)
{
    if (!image.valid) return (rf_image) {0};

    int size = rf_image_size(image);
    void* dst = rf_alloc(allocator, size);

    rf_image result = rf_image_flip_vertical_to_buffer(image, dst, size);
    if (!result.valid) rf_free(allocator, dst);

    return result;
}

// Flip image horizontally
rf_extern rf_image rf_image_flip_horizontal_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (y * image.width + x) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + (y * image.width + (image.width - 1 - x)) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_flip_horizontal(rf_image image, rf_allocator allocator)
{
    if (!image.valid) return (rf_image) {0};

    int size = rf_image_size(image);
    void* dst = rf_alloc(allocator, size);

    rf_image result = rf_image_flip_horizontal_to_buffer(image, dst, size);
    if (!result.valid) rf_free(allocator, dst);

    return result;
}

// Rotate image clockwise 90deg
rf_extern rf_image rf_image_rotate_cw_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (x * image.height + (image.height - y - 1)) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + (y * image.width + x) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_rotate_cw(rf_image image)
{
    return rf_image_rotate_cw_to_buffer(image, image.data, rf_image_size(image));
}

// Rotate image counter-clockwise 90deg
rf_extern rf_image rf_image_rotate_ccw_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (x * image.height + y) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + (y * image.width + (image.width - x - 1)) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_rotate_ccw(rf_image image)
{
    return rf_image_rotate_ccw_to_buffer(image, image.data, rf_image_size(image));
}

// Modify image color: tint
rf_extern rf_image rf_image_color_tint_to_buffer(rf_image image, rf_color color, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            float c_r = ((float) color.r) / 255.0f;
            float c_g = ((float) color.g) / 255.0f;
            float c_b = ((float) color.b) / 255.0f;
            float c_a = ((float) color.a) / 255.0f;

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int   index = y * image.width + x;
                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)image.data) + index * bpp;

                    rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

                    pixel_rgba32.r = (unsigned char) (255.f * (((float)pixel_rgba32.r) / 255.f * c_r));
                    pixel_rgba32.g = (unsigned char) (255.f * (((float)pixel_rgba32.g) / 255.f * c_g));
                    pixel_rgba32.b = (unsigned char) (255.f * (((float)pixel_rgba32.b) / 255.f * c_b));
                    pixel_rgba32.a = (unsigned char) (255.f * (((float)pixel_rgba32.a) / 255.f * c_a));

                    rf_format_one_pixel(&pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_color_tint(rf_image image, rf_color color)
{
    return rf_image_color_tint_to_buffer(image, color, image.data, rf_image_size(image));
}

// Modify image color: invert
rf_extern rf_image rf_image_color_invert_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int   index = y * image.width + x;
                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)dst) + index * bpp;

                    rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);
                    pixel_rgba32.r = 255 - pixel_rgba32.r;
                    pixel_rgba32.g = 255 - pixel_rgba32.g;
                    pixel_rgba32.b = 255 - pixel_rgba32.b;

                    rf_format_one_pixel(&pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_color_invert(rf_image image)
{
    return rf_image_color_invert_to_buffer(image, image.data, rf_image_size(image));
}

// Modify image color: grayscale
rf_extern rf_image rf_image_color_grayscale_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    return rf_image_format_to_buffer(image, rf_pixel_format_grayscale, dst, dst_size);
}

rf_extern rf_image rf_image_color_grayscale(rf_image image)
{
    rf_image result = {0};

    if (image.valid)
    {
        result = rf_image_color_grayscale_to_buffer(image, image.data, rf_image_size(image));
    }

    return result;
}

// Modify image color: contrast
// NOTE: Contrast values between -100 and 100
rf_extern rf_image rf_image_color_contrast_to_buffer(rf_image image, float contrast, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            if (contrast < -100) contrast = -100;
            if (contrast > +100) contrast = +100;

            contrast = (100.0f + contrast) / 100.0f;
            contrast *= contrast;

            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int index = y * image.width + x;
                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)dst) + index * bpp;

                    rf_color src_pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

                    float p_r = ((float)src_pixel_rgba32.r) / 255.0f;
                    p_r -= 0.5;
                    p_r *= contrast;
                    p_r += 0.5;
                    p_r *= 255;
                    if (p_r < 0) p_r = 0;
                    if (p_r > 255) p_r = 255;

                    float p_g = ((float)src_pixel_rgba32.g) / 255.0f;
                    p_g -= 0.5;
                    p_g *= contrast;
                    p_g += 0.5;
                    p_g *= 255;
                    if (p_g < 0) p_g = 0;
                    if (p_g > 255) p_g = 255;

                    float p_b = ((float)src_pixel_rgba32.b) / 255.0f;
                    p_b -= 0.5;
                    p_b *= contrast;
                    p_b += 0.5;
                    p_b *= 255;
                    if (p_b < 0) p_b = 0;
                    if (p_b > 255) p_b = 255;

                    src_pixel_rgba32.r = (unsigned char)p_r;
                    src_pixel_rgba32.g = (unsigned char)p_g;
                    src_pixel_rgba32.b = (unsigned char)p_b;

                    rf_format_one_pixel(&src_pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_color_contrast(rf_image image, int contrast)
{
    return rf_image_color_contrast_to_buffer(image, contrast, image.data, rf_image_size(image));
}

// Modify image color: brightness
// NOTE: Brightness values between -255 and 255
rf_extern rf_image rf_image_color_brightness_to_buffer(rf_image image, int brightness, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            if (brightness < -255) brightness = -255;
            if (brightness > +255) brightness = +255;

            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int index = y * image.width + x;

                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)dst) + index * bpp;

                    rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

                    int c_r = pixel_rgba32.r + brightness;
                    int c_g = pixel_rgba32.g + brightness;
                    int c_b = pixel_rgba32.b + brightness;

                    if (c_r < 0) c_r = 1;
                    if (c_r > 255) c_r = 255;

                    if (c_g < 0) c_g = 1;
                    if (c_g > 255) c_g = 255;

                    if (c_b < 0) c_b = 1;
                    if (c_b > 255) c_b = 255;

                    pixel_rgba32.r = (unsigned char) c_r;
                    pixel_rgba32.g = (unsigned char) c_g;
                    pixel_rgba32.b = (unsigned char) c_b;

                    rf_format_one_pixel(&pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_extern rf_image rf_image_color_brightness(rf_image image, int brightness)
{
    return rf_image_color_brightness_to_buffer(image, brightness, image.data, rf_image_size(image));
}

// Modify image color: replace color
rf_extern rf_image rf_image_color_replace_to_buffer(rf_image image, rf_color color, rf_color replace, void* dst, rf_int dst_size)
{
    if (image.valid && dst_size >= rf_image_size(image)) return (rf_image) {0};

    rf_image result = {0};

    int bpp = rf_bytes_per_pixel(image.format);

    for (rf_int y = 0; y < image.height; y++)
    {
        for (rf_int x = 0; x < image.width; x++)
        {
            int index = y * image.width + x;

            void* src_pixel = ((unsigned char*)image.data) + index * bpp;
            void* dst_pixel = ((unsigned char*)dst) + index * bpp;

            rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

            if (rf_color_match(pixel_rgba32, color))
            {
                rf_format_one_pixel(&replace, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
            }
        }
    }

    result = image;
    result.data = dst;

    return result;
}

rf_extern rf_image rf_image_color_replace(rf_image image, rf_color color, rf_color replace)
{
    return rf_image_color_replace_to_buffer(image, color, replace, image.data, rf_image_size(image));
}

// Draw an image (source) within an image (destination)
rf_extern void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator)
{
    if (src.valid && dst->valid)
    {
        dst->valid = 0;

        if (src_rec.x < 0) src_rec.x = 0;
        if (src_rec.y < 0) src_rec.y = 0;

        if ((src_rec.x + src_rec.width) > src.width)
        {
            src_rec.width = src.width - src_rec.x;
            rf_log(rf_log_type_warning, "Source rectangle width out of bounds, rescaled width: %i", src_rec.width);
        }

        if ((src_rec.y + src_rec.height) > src.height)
        {
            src_rec.height = src.height - src_rec.y;
            rf_log(rf_log_type_warning, "Source rectangle height out of bounds, rescaled height: %i", src_rec.height);
        }

        rf_image src_copy = rf_image_copy(src, temp_allocator); // Make a copy of source src to work with it

        // Crop source image to desired source rectangle (if required)
        if ((src.width != (int)src_rec.width) && (src.height != (int)src_rec.height))
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, src_rec, temp_allocator);
            rf_image_free(old_src_copy, temp_allocator);
        }

        // Scale source image in case destination rec size is different than source rec size
        if (((int)dst_rec.width != (int)src_rec.width) || ((int)dst_rec.height != (int)src_rec.height))
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_resize(src_copy, (int)dst_rec.width, (int)dst_rec.height, temp_allocator, temp_allocator);
            rf_image_free(old_src_copy, temp_allocator);
        }

        // Check that dstRec is inside dst image
        // Allow negative position within destination with cropping
        if (dst_rec.x < 0)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { -dst_rec.x, 0, dst_rec.width + dst_rec.x, dst_rec.height }, temp_allocator);
            dst_rec.width = dst_rec.width + dst_rec.x;
            dst_rec.x = 0;
            rf_image_free(old_src_copy, temp_allocator);
        }

        if ((dst_rec.x + dst_rec.width) > dst->width)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, 0, dst->width - dst_rec.x, dst_rec.height }, temp_allocator);
            dst_rec.width = dst->width - dst_rec.x;
            rf_image_free(old_src_copy, temp_allocator);
        }

        if (dst_rec.y < 0)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, -dst_rec.y, dst_rec.width, dst_rec.height + dst_rec.y }, temp_allocator);
            dst_rec.height = dst_rec.height + dst_rec.y;
            dst_rec.y = 0;
            rf_image_free(old_src_copy, temp_allocator);
        }

        if ((dst_rec.y + dst_rec.height) > dst->height)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, 0, dst_rec.width, dst->height - dst_rec.y }, temp_allocator);
            dst_rec.height = dst->height - dst_rec.y;
            rf_image_free(old_src_copy, temp_allocator);
        }

        if (src_copy.valid)
        {
            // Get image data as rf_color pixels array to work with it
            rf_pixel_buffer temp_dst_pixel_buffer = rf_format_pixels_to_rgba32(rf_image_to_pixel_buffer(*dst), temp_allocator);
            rf_pixel_buffer temp_src_pixel_buffer = rf_format_pixels_to_rgba32(rf_image_to_pixel_buffer(src_copy), temp_allocator);

            rf_color* temp_dst_pixels = temp_dst_pixel_buffer.data;
            rf_color* temp_src_pixels = temp_src_pixel_buffer.data;

            rf_image_free(src_copy, temp_allocator); // Source copy not required any more

            rf_vec4 fsrc, fdst, fout; // Normalized pixel data (ready for operation)
            rf_vec4 ftint = rf_color_normalize(tint); // Normalized color tint

            // Blit pixels, copy source image into destination
            // TODO: Maybe out-of-bounds blitting could be considered here instead of so much cropping
            for (rf_int j = (int)dst_rec.y; j < (int)(dst_rec.y + dst_rec.height); j++)
            {
                for (rf_int i = (int)dst_rec.x; i < (int)(dst_rec.x + dst_rec.width); i++)
                {
                    // Alpha blending (https://en.wikipedia.org/wiki/Alpha_compositing)

                    fdst = rf_color_normalize(temp_dst_pixels[j * (int)dst->width + i]);
                    fsrc = rf_color_normalize(temp_src_pixels[(j - (int)dst_rec.y) * (int)dst_rec.width + (i - (int)dst_rec.x)]);

                    // Apply color tint to source image
                    fsrc.x *= ftint.x; fsrc.y *= ftint.y; fsrc.z *= ftint.z; fsrc.w *= ftint.w;

                    fout.w = fsrc.w + fdst.w * (1.0f - fsrc.w);

                    if (fout.w <= 0.0f)
                    {
                        fout.x = 0.0f;
                        fout.y = 0.0f;
                        fout.z = 0.0f;
                    }
                    else
                    {
                        fout.x = (fsrc.x*fsrc.w + fdst.x*fdst.w*(1 - fsrc.w))/fout.w;
                        fout.y = (fsrc.y*fsrc.w + fdst.y*fdst.w*(1 - fsrc.w))/fout.w;
                        fout.z = (fsrc.z*fsrc.w + fdst.z*fdst.w*(1 - fsrc.w))/fout.w;
                    }

                    temp_dst_pixels[j * ((rf_int)dst->width) + i] = (rf_color) { (uint8_t)(fout.x * 255.0f),
                                                                                 (uint8_t)(fout.y * 255.0f),
                                                                                 (uint8_t)(fout.z * 255.0f),
                                                                                 (uint8_t)(fout.w * 255.0f) };

                    // TODO: Support other blending options
                }
            }

            // This will write to the dst image
            rf_pixel_buffer result_pixel_buffer = rf_format_pixels_to_buffer(temp_dst_pixel_buffer, dst->data, rf_image_size(*dst), dst->format);

            rf_free(temp_allocator, temp_src_pixels);
            rf_free(temp_allocator, temp_dst_pixels);

            dst->valid = result_pixel_buffer.valid;
        }
    }
}

// Draw rectangle within an image
rf_extern void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator)
{
    if (dst->valid)
    {
        dst->valid = 0;

        rf_image src = rf_gen_image_color((int)rec.width, (int)rec.height, color, temp_allocator);

        if (src.valid)
        {
            rf_rec src_rec = (rf_rec){ 0, 0, rec.width, rec.height };

            rf_image_draw(dst, src, src_rec, rec, rf_white, temp_allocator);

            rf_image_free(src, temp_allocator);
        }
    }
}

// Draw rectangle lines within an image
rf_extern void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator)
{
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y, rec.width, thick }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y + thick, thick, rec.height - thick * 2 }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x + rec.width - thick, rec.y + thick, thick, rec.height - thick * 2 }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y + rec.height - thick, rec.width, thick }, color, temp_allocator);
}