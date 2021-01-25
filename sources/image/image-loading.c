#include "rayfork/image/image-loading.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/internal/foundation/assert.h"
#include "stb_image.h"
#include "string.h"

rf_extern bool rf_supports_image_file_type(rf_str filename)
{
    return       rf_str_match_suffix(filename, rf_cstr(".png"))
              || rf_str_match_suffix(filename, rf_cstr(".bmp"))
              || rf_str_match_suffix(filename, rf_cstr(".tga"))
              || rf_str_match_suffix(filename, rf_cstr(".pic"))
              || rf_str_match_suffix(filename, rf_cstr(".psd"))
              || rf_str_match_suffix(filename, rf_cstr(".hdr"));
}

rf_extern rf_image rf_load_image_from_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator)
{
    if (src == NULL || src_size <= 0) { rf_log_error(rf_bad_argument, "Argument `image` was invalid."); return (rf_image){0}; }

    rf_image result = {0};

    int img_width  = 0;
    int img_height = 0;
    int img_bpp    = 0;

    rf_global_allocator_for_thirdparty_libraries = temp_allocator;
    void* output_buffer = stbi_load_from_memory(src, src_size, &img_width, &img_height, &img_bpp, channels);
    rf_global_allocator_for_thirdparty_libraries = (rf_allocator) {0};

    if (output_buffer)
    {
        int output_buffer_size = img_width * img_height * img_bpp;

        if (dst_size >= output_buffer_size)
        {
            result.data   = dst;
            result.width  = img_width;
            result.height = img_height;
            result.valid  = 1;

            switch (img_bpp)
            {
                case 1: result.format = rf_pixel_format_grayscale; break;
                case 2: result.format = rf_pixel_format_gray_alpha; break;
                case 3: result.format = rf_pixel_format_r8g8b8; break;
                case 4: result.format = rf_pixel_format_r8g8b8a8; break;
                default: break;
            }

            memcpy(dst, output_buffer, output_buffer_size);
        }
        else rf_log_error(rf_bad_argument, "Buffer is not big enough", img_width, img_height, img_bpp);

        rf_free(temp_allocator, output_buffer);
    }
    else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", img_width, img_height, img_bpp);

    return result;
}

rf_extern rf_image rf_load_image_from_file_data(const void* src, rf_int src_size, rf_desired_channels desired_channels, rf_allocator allocator, rf_allocator temp_allocator)
{
    // Preconditions
    if (!src || src_size <= 0)
    {
        rf_log_error(rf_bad_argument, "Argument `src` was null.");
        return (rf_image) {0};
    }

    // Compute the result
    rf_image result = {0};

    // Use stb image with the `temp_allocator` to decompress the image and get it's data
    int width = 0, height = 0, channels = 0;
    rf_global_allocator_for_thirdparty_libraries = temp_allocator;
    void* stbi_result = stbi_load_from_memory(src, src_size, &width, &height, &channels, desired_channels);
    rf_global_allocator_for_thirdparty_libraries = (rf_allocator) {0};

    if (stbi_result && channels)
    {
        // Allocate a result buffer using the `allocator` and copy the data to it
        int stbi_result_size = width * height * channels;
        void* result_buffer = rf_alloc(allocator, stbi_result_size);

        if (result_buffer)
        {
            result.data   = result_buffer;
            result.width  = width;
            result.height = height;
            result.valid  = 1;

            // Set the format appropriately depending on the `channels` count
            switch (channels)
            {
                case 1: result.format = rf_pixel_format_grayscale; break;
                case 2: result.format = rf_pixel_format_gray_alpha; break;
                case 3: result.format = rf_pixel_format_r8g8b8; break;
                case 4: result.format = rf_pixel_format_r8g8b8a8; break;
                default: break;
            }

            memcpy(result_buffer, stbi_result, stbi_result_size);
        }
        else rf_log_error(rf_bad_alloc, "Buffer is not big enough", width, height, channels);

        // Free the temp buffer allocated by stbi
        rf_free(temp_allocator, stbi_result);
    }
    else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", width, height, channels);

    return result;
}

rf_extern rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (src && src_size > 0)
    {
        int img_width = 0, img_height = 0, img_bpp = 0;

        // NOTE: Using stb_image to load images (Supports multiple image formats)
        rf_global_allocator_for_thirdparty_libraries = temp_allocator;
        void* output_buffer = stbi_load_from_memory(src, src_size, &img_width, &img_height, &img_bpp, channels);
        rf_global_allocator_for_thirdparty_libraries = (rf_allocator) {0};

        if (output_buffer)
        {
            int output_buffer_size = img_width * img_height * img_bpp;

            if (dst_size >= output_buffer_size)
            {
                result.data   = dst;
                result.width  = img_width;
                result.height = img_height;
                result.valid  = 1;

                     if (img_bpp == 1) result.format = rf_pixel_format_r32;
                else if (img_bpp == 3) result.format = rf_pixel_format_r32g32b32;
                else if (img_bpp == 4) result.format = rf_pixel_format_r32g32b32a32;

                memcpy(dst, output_buffer, output_buffer_size);
            }
            else rf_log_error(rf_bad_argument, "Buffer is not big enough", img_width, img_height, img_bpp);

            rf_free(temp_allocator, output_buffer);
        }
        else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", img_width, img_height, img_bpp);
    }
    else rf_log_error(rf_bad_argument, "Argument `image` was invalid.");

    return result;
}

rf_extern rf_image rf_load_image_from_hdr_file_data(const void* src, rf_int src_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (src && src_size)
    {
        int width = 0;
        int height = 0;
        int bpp = 0;

        // NOTE: Using stb_image to load images (Supports multiple image formats)
        rf_global_allocator_for_thirdparty_libraries = temp_allocator;
        void* stbi_result = stbi_load_from_memory(src, src_size, &width, &height, &bpp, rf_desired_channels_any);
        rf_global_allocator_for_thirdparty_libraries = (rf_allocator) {0};

        if (stbi_result && bpp)
        {
            int stbi_result_size = width * height * bpp;
            void* result_buffer = rf_alloc(allocator, stbi_result_size);

            if (result_buffer)
            {
                result.data   = result_buffer;
                result.width  = width;
                result.height = height;
                result.valid  = 1;

                     if (bpp == 1) result.format = rf_pixel_format_r32;
                else if (bpp == 3) result.format = rf_pixel_format_r32g32b32;
                else if (bpp == 4) result.format = rf_pixel_format_r32g32b32a32;

                memcpy(result_buffer, stbi_result, stbi_result_size);
            }
            else rf_log_error(rf_bad_alloc, "Buffer is not big enough", width, height, bpp);

            rf_free(temp_allocator, stbi_result);
        }
        else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", width, height, bpp);
    }
    else rf_log_error(rf_bad_argument, "Argument `image` was invalid.");

    return result;
}

rf_extern rf_image rf_load_image_from_format_to_buffer(const void* src, rf_int src_size, int src_width, int src_height, rf_pixel_format src_format, void* dst, rf_int dst_size, rf_pixel_format dst_format)
{
    rf_image result = {0};

    if (rf_is_uncompressed_format(dst_format))
    {
        result = (rf_image) {
            .data   = dst,
            .width  = src_width,
            .height = src_height,
            .format = dst_format,
        };

        rf_pixel_buffer result_buffer = rf_format_pixels_to_buffer((rf_pixel_buffer) { .data = (void*) src, .size = src_size, .format = src_format, .valid = rf_valid }, dst, dst_size, dst_format);
        rf_assert(result_buffer.valid);
    }

    return result;
}

rf_extern rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_image image = {0};

    if (rf_supports_image_file_type(rf_cstr(filename)))
    {
        rf_file_handle file = rf_file_open(io, filename, rf_file_mode_read);
        rf_file_contents contents = rf_read_entire_file(filename, temp_allocator, io);
        if (rf_str_match_suffix(rf_cstr(filename), rf_cstr(".hdr")))
        {
            image = rf_load_image_from_hdr_file_data(contents.data, contents.size, allocator, temp_allocator);
        }
        else
        {
            image = rf_load_image_from_file_data(contents.data, contents.size, rf_desired_channels_any, allocator, temp_allocator);
        }
        rf_free(temp_allocator, contents.data);
    }
    else rf_log_error(rf_unsupported, "Image fileformat not supported", filename);

    return image;
}