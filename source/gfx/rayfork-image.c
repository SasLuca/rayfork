#include "rayfork-image.h"
#include "rayfork-gfx-internal-string-utils.h"

#pragma region dependencies

#pragma region stb_image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)                     RF_ALLOC(rf__global_allocator_for_dependencies, sz)
#define STBI_FREE(p)                        RF_FREE(rf__global_allocator_for_dependencies, p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) rf_default_realloc(rf__global_allocator_for_dependencies, p, oldsz, newsz)
#define STBI_ASSERT(it)                     RF_ASSERT(it)
#define STBIDEF                             RF_INTERNAL
#include "stb_image.h"
#pragma endregion

#pragma region stb_image_resize
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(sz,c)   ((void)(c), RF_ALLOC(rf__global_allocator_for_dependencies, sz))
#define STBIR_FREE(p,c)      ((void)(c), RF_FREE(rf__global_allocator_for_dependencies, p))
#define STBIR_ASSERT(it)     RF_ASSERT(it)
#define STBIRDEF RF_INTERNAL
#include "stb_image_resize.h"
#pragma endregion

#pragma region stb_rect_pack
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_ASSERT RF_ASSERT
#define STBRP_STATIC
#include "stb_rect_pack.h"
#pragma endregion

#pragma region stb_perlin
#define STB_PERLIN_IMPLEMENTATION
#define STBPDEF RF_INTERNAL
#include "stb_perlin.h"
#pragma endregion

#pragma endregion

#pragma region extract image data functions

RF_API int rf_image_size(rf_image image)
{
    return rf_pixel_buffer_size(image.width, image.height, image.format);
}

RF_API int rf_image_size_in_format(rf_image image, rf_pixel_format format)
{
    return image.width * image.height * rf_bytes_per_pixel(format);
}

RF_API rf_bool rf_image_get_pixels_as_rgba32_to_buffer(rf_image image, rf_color* dst, rf_int dst_size)
{
    rf_bool success = 0;

    if (rf_is_uncompressed_format(image.format))
    {
        if (image.format == RF_UNCOMPRESSED_R32 || image.format == RF_UNCOMPRESSED_R32G32B32 || image.format == RF_UNCOMPRESSED_R32G32B32A32)
        {
            RF_LOG(RF_LOG_TYPE_WARNING, "32bit pixel format converted to 8bit per channel.");
        }

        success = rf_format_pixels_to_rgba32(image.data, rf_image_size(image), image.format, dst, dst_size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return success;
}

RF_API rf_bool rf_image_get_pixels_as_normalized_to_buffer(rf_image image, rf_vec4* dst, rf_int dst_size)
{
    rf_bool success = 0;

    if (rf_is_uncompressed_format(image.format))
    {
        if ((image.format == RF_UNCOMPRESSED_R32) || (image.format == RF_UNCOMPRESSED_R32G32B32) || (image.format == RF_UNCOMPRESSED_R32G32B32A32))
        {
            RF_LOG(RF_LOG_TYPE_WARNING, "32bit pixel format converted to 8bit per channel");
        }

        success = rf_format_pixels_to_normalized(image.data, rf_image_size(image), RF_UNCOMPRESSED_R32G32B32A32, dst, dst_size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return success;
}

// Get pixel data from image in the form of rf_color struct array
RF_API rf_color* rf_image_pixels_to_rgba32(rf_image image, rf_allocator allocator)
{
    rf_color* result = NULL;

    if (rf_is_uncompressed_format(image.format))
    {
        int size = image.width * image.height * sizeof(rf_color);
        result = RF_ALLOC(allocator, size);

        if (result)
        {
            rf_bool success = rf_image_get_pixels_as_rgba32_to_buffer(image, result, size);
            RF_ASSERT(success);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return result;
}

// Get pixel data from image as rf_vec4 array (float normalized)
RF_API rf_vec4* rf_image_compute_pixels_to_normalized(rf_image image, rf_allocator allocator)
{
    rf_vec4* result = NULL;

    if (rf_is_compressed_format(image.format))
    {
        int size = image.width * image.height * sizeof(rf_color);
        result = RF_ALLOC(allocator, size);

        if (result)
        {
            rf_bool success = rf_image_get_pixels_as_normalized_to_buffer(image, result, size);
            RF_ASSERT(success);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return result;
}

// Extract color palette from image to maximum size
RF_API void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, rf_int palette_size)
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

                rf_bool color_found = 0;

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
        else RF_LOG(RF_LOG_TYPE_WARNING, "Palette size was 0.");
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Function only works for uncompressed formats but was called with format %d.", image.format);
}

RF_API rf_palette rf_image_extract_palette(rf_image image, rf_int palette_size, rf_allocator allocator)
{
    rf_palette result = {0};

    if (rf_is_uncompressed_format(image.format))
    {
        rf_color* dst = RF_ALLOC(allocator, sizeof(rf_color) * palette_size);
        rf_image_extract_palette_to_buffer(image, dst, palette_size);

        result.colors = dst;
        result.count = palette_size;
    }

    return result;
}

// Get image alpha border rectangle
RF_API rf_rec rf_image_alpha_border(rf_image image, float threshold)
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
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return crop;
}

#pragma endregion

#pragma region loading and unloading functions

RF_API rf_bool rf_supports_image_file_type(const char* filename)
{
    return       rf_is_file_extension(filename, ".png")
              || rf_is_file_extension(filename, ".bmp")
              || rf_is_file_extension(filename, ".tga")
              || rf_is_file_extension(filename, ".pic")
              || rf_is_file_extension(filename, ".psd")
              || rf_is_file_extension(filename, ".hdr");
}

RF_API rf_image rf_load_image_from_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator)
{
    if (src == NULL || src_size <= 0) { RF_LOG_ERROR(RF_BAD_ARGUMENT, "Argument `image` was invalid."); return (rf_image){0}; }

    rf_image result = {0};

    int img_width  = 0;
    int img_height = 0;
    int img_bpp    = 0;

    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
    void* output_buffer = stbi_load_from_memory(src, src_size, &img_width, &img_height, &img_bpp, channels);
    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

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
                case 1: result.format = RF_UNCOMPRESSED_GRAYSCALE; break;
                case 2: result.format = RF_UNCOMPRESSED_GRAY_ALPHA; break;
                case 3: result.format = RF_UNCOMPRESSED_R8G8B8; break;
                case 4: result.format = RF_UNCOMPRESSED_R8G8B8A8; break;
                default: break;
            }

            memcpy(dst, output_buffer, output_buffer_size);
        }
        else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Buffer is not big enough", img_width, img_height, img_bpp);

        RF_FREE(temp_allocator, output_buffer);
    }
    else RF_LOG_ERROR(RF_STBI_FAILED, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", img_width, img_height, img_bpp);

    return result;
}

RF_API rf_image rf_load_image_from_file_data(const void* src, rf_int src_size, rf_desired_channels desired_channels, rf_allocator allocator, rf_allocator temp_allocator)
{
    // Preconditions
    if (!src || src_size <= 0)
    {
        RF_LOG_ERROR(RF_BAD_ARGUMENT, "Argument `src` was null.");
        return (rf_image) {0};
    }

    // Compute the result
    rf_image result = {0};

    // Use stb image with the `temp_allocator` to decompress the image and get it's data
    int width = 0, height = 0, channels = 0;
    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
    void* stbi_result = stbi_load_from_memory(src, src_size, &width, &height, &channels, desired_channels);
    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

    if (stbi_result && channels)
    {
        // Allocate a result buffer using the `allocator` and copy the data to it
        int stbi_result_size = width * height * channels;
        void* result_buffer = RF_ALLOC(allocator, stbi_result_size);

        if (result_buffer)
        {
            result.data   = result_buffer;
            result.width  = width;
            result.height = height;
            result.valid  = 1;

            // Set the format appropriately depending on the `channels` count
            switch (channels)
            {
                case 1: result.format = RF_UNCOMPRESSED_GRAYSCALE; break;
                case 2: result.format = RF_UNCOMPRESSED_GRAY_ALPHA; break;
                case 3: result.format = RF_UNCOMPRESSED_R8G8B8; break;
                case 4: result.format = RF_UNCOMPRESSED_R8G8B8A8; break;
                default: break;
            }

            memcpy(result_buffer, stbi_result, stbi_result_size);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Buffer is not big enough", width, height, channels);

        // Free the temp buffer allocated by stbi
        RF_FREE(temp_allocator, stbi_result);
    }
    else RF_LOG_ERROR(RF_STBI_FAILED, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", width, height, channels);

    return result;
}

RF_API rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (src && src_size > 0)
    {
        int img_width = 0, img_height = 0, img_bpp = 0;

        // NOTE: Using stb_image to load images (Supports multiple image formats)
        RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
        void* output_buffer = stbi_load_from_memory(src, src_size, &img_width, &img_height, &img_bpp, channels);
        RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

        if (output_buffer)
        {
            int output_buffer_size = img_width * img_height * img_bpp;

            if (dst_size >= output_buffer_size)
            {
                result.data   = dst;
                result.width  = img_width;
                result.height = img_height;
                result.valid  = 1;

                     if (img_bpp == 1) result.format = RF_UNCOMPRESSED_R32;
                else if (img_bpp == 3) result.format = RF_UNCOMPRESSED_R32G32B32;
                else if (img_bpp == 4) result.format = RF_UNCOMPRESSED_R32G32B32A32;

                memcpy(dst, output_buffer, output_buffer_size);
            }
            else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Buffer is not big enough", img_width, img_height, img_bpp);

            RF_FREE(temp_allocator, output_buffer);
        }
        else RF_LOG_ERROR(RF_STBI_FAILED, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", img_width, img_height, img_bpp);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Argument `image` was invalid.");

    return result;
}

RF_API rf_image rf_load_image_from_hdr_file_data(const void* src, rf_int src_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (src && src_size)
    {
        int width = 0, height = 0, bpp = 0;

        // NOTE: Using stb_image to load images (Supports multiple image formats)
        RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
        void* stbi_result = stbi_load_from_memory(src, src_size, &width, &height, &bpp, RF_ANY_CHANNELS);
        RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

        if (stbi_result && bpp)
        {
            int stbi_result_size = width * height * bpp;
            void* result_buffer = RF_ALLOC(allocator, stbi_result_size);

            if (result_buffer)
            {
                result.data   = result_buffer;
                result.width  = width;
                result.height = height;
                result.valid  = 1;

                     if (bpp == 1) result.format = RF_UNCOMPRESSED_R32;
                else if (bpp == 3) result.format = RF_UNCOMPRESSED_R32G32B32;
                else if (bpp == 4) result.format = RF_UNCOMPRESSED_R32G32B32A32;

                memcpy(result_buffer, stbi_result, stbi_result_size);
            }
            else RF_LOG_ERROR(RF_BAD_ALLOC, "Buffer is not big enough", width, height, bpp);

            RF_FREE(temp_allocator, stbi_result);
        }
        else RF_LOG_ERROR(RF_STBI_FAILED, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", width, height, bpp);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Argument `image` was invalid.");

    return result;
}

RF_API rf_image rf_load_image_from_format_to_buffer(const void* src, rf_int src_size, int src_width, int src_height, rf_uncompressed_pixel_format src_format, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format)
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

        rf_bool success = rf_format_pixels(src, src_size, src_format, dst, dst_size, dst_format);
        RF_ASSERT(success);
    }

    return result;
}

RF_API rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_image image = {0};

    if (rf_supports_image_file_type(filename))
    {
        int file_size = RF_FILE_SIZE(io, filename);

        if (file_size > 0)
        {
            unsigned char* image_file_buffer = RF_ALLOC(temp_allocator, file_size);

            if (image_file_buffer)
            {
                if (RF_READ_FILE(io, filename, image_file_buffer, file_size))
                {
                    if (rf_is_file_extension(filename, ".hdr"))
                    {
                        image = rf_load_image_from_hdr_file_data(image_file_buffer, file_size, allocator, temp_allocator);
                    }
                    else
                    {
                        image = rf_load_image_from_file_data(image_file_buffer, file_size, RF_ANY_CHANNELS, allocator, temp_allocator);
                    }
                }
                else RF_LOG_ERROR(RF_BAD_IO, "File size for %s is 0", filename);

                RF_FREE(temp_allocator, image_file_buffer);
            }
            else RF_LOG_ERROR(RF_BAD_ALLOC, "Temporary allocation of size %d failed", file_size);
        }
        else RF_LOG_ERROR(RF_BAD_IO, "File size for %s is 0", filename);
    }
    else RF_LOG_ERROR(RF_UNSUPPORTED, "Image fileformat not supported", filename);

    return image;
}

RF_API void rf_unload_image(rf_image image, rf_allocator allocator)
{
    RF_FREE(allocator, image.data);
}

#pragma endregion

#pragma region image manipulation

/**
 * Copy an existing image into a buffer.
 * @param image a valid image to copy from.
 * @param dst a buffer for the resulting image.
 * @param dst_size size of the `dst` buffer.
 * @return a deep copy of the image into the provided `dst` buffer.
 */
RF_API rf_image rf_image_copy_to_buffer(rf_image image, void* dst, rf_int dst_size)
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
        else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Destination buffer is too small. Expected at least %d bytes but was %d", size, dst_size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image was invalid.");

    return result;
}

/**
 * Copy an existing image.
 * @param image a valid image to copy from.
 * @param allocator used to allocate the new buffer for the resulting image.
 * @return a deep copy of the image.
 */
RF_API rf_image rf_image_copy(rf_image image, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int size  = image.width * image.height * rf_bytes_per_pixel(image.format);
        void* dst = RF_ALLOC(allocator, size);

        if (dst)
        {
            result = rf_image_copy_to_buffer(image, dst, size);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Failed to allocate %d bytes", size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image was invalid.");

    return result;
}

/**
 * Crop an image and store the result in a provided buffer.
 * @param image a valid image that we crop from.
 * @param crop a rectangle representing which part of the image to crop.
 * @param dst a buffer for the resulting image. Must be of size at least `rf_pixel_buffer_size(RF_UNCOMPRESSED_R8G8B8A8, crop.width, crop.height)`.
 * @param dst_size size of the `dst` buffer.
 * @return a cropped image using the `dst` buffer in the same format as `image`.
 */
RF_API rf_image rf_image_crop_to_buffer(rf_image image, rf_rec crop, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format)
{
    rf_image result = {0};

    if (image.valid)
    {
        // Security checks to validate crop rectangle
        if (crop.x < 0) { crop.width += crop.x; crop.x = 0; }
        if (crop.y < 0) { crop.height += crop.y; crop.y = 0; }
        if ((crop.x + crop.width) > image.width) { crop.width = image.width - crop.x; }
        if ((crop.y + crop.height) > image.height) { crop.height = image.height - crop.y; }

        if ((crop.x < image.width) && (crop.y < image.height))
        {
            int expected_size = rf_pixel_buffer_size(crop.width, crop.height, dst_format);
            if (dst_size >= expected_size)
            {
                rf_pixel_format src_format = image.format;
                int src_size = rf_image_size(image);

                unsigned char* src_ptr = image.data;
                unsigned char* dst_ptr = dst;

                int src_bpp = rf_bytes_per_pixel(image.format);
                int dst_bpp = rf_bytes_per_pixel(dst_format);

                int crop_y = crop.y;
                int crop_h = crop.height;
                int crop_x = crop.x;
                int crop_w = crop.width;

                for (rf_int y = 0; y < crop_h; y++)
                {
                    for (rf_int x = 0; x < crop_w; x++)
                    {
                        int src_x = x + crop_x;
                        int src_y = y + crop_y;

                        int src_pixel = (src_y * image.width + src_x) * src_bpp;
                        int dst_pixel = (y * crop_w + x) * src_bpp;
                        RF_ASSERT(src_pixel < src_size);
                        RF_ASSERT(dst_pixel < dst_size);

                        rf_format_one_pixel(&src_ptr[src_pixel], src_format, &dst_ptr[dst_pixel], dst_format);
                    }
                }

                result.data   = dst;
                result.format = dst_format;
                result.width  = crop.width;
                result.height = crop.height;
                result.valid  = 1;
            }
            else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Destination buffer is too small. Expected at least %d bytes but was %d", expected_size, dst_size);
        }
        else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image can not be cropped, crop rectangle out of bounds.");
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

/**
 * Crop an image and store the result in a provided buffer.
 * @param image a valid image that we crop from.
 * @param crop a rectangle representing which part of the image to crop.
 * @param dst a buffer for the resulting image. Must be of size at least `rf_pixel_buffer_size(RF_UNCOMPRESSED_R8G8B8A8, crop.width, crop.height)`.
 * @param dst_size size of the `dst` buffer.
 * @return a cropped image using the `dst` buffer in the same format as `image`.
 */
RF_API rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int size = rf_pixel_buffer_size(crop.width, crop.height, image.format);
        void* dst = RF_ALLOC(allocator, size);

        if (dst)
        {
            result = rf_image_crop_to_buffer(image, crop, dst, size, image.format);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

RF_INTERNAL int rf_format_to_stb_channel_count(rf_pixel_format format)
{
    switch (format)
    {
        case RF_UNCOMPRESSED_GRAYSCALE: return 1;
        case RF_UNCOMPRESSED_GRAY_ALPHA: return 2;
        case RF_UNCOMPRESSED_R8G8B8: return 3;
        case RF_UNCOMPRESSED_R8G8B8A8: return 4;
        default: return 0;
    }
}

// Resize and image to new size.
// Note: Uses stb default scaling filters (both bicubic): STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL (high-quality Catmull-Rom)
RF_API rf_image rf_image_resize_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size, rf_allocator temp_allocator)
{
    if (!image.valid || dst_size < new_width * new_height * rf_bytes_per_pixel(image.format)) return (rf_image){0};

    rf_image result = {0};

    int stb_format = rf_format_to_stb_channel_count(image.format);

    if (stb_format)
    {
        RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
        stbir_resize_uint8((unsigned char*) image.data, image.width, image.height, 0, (unsigned char*) dst, new_width, new_height, 0, stb_format);
        RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

        result.data   = dst;
        result.width  = new_width;
        result.height = new_height;
        result.format = image.format;
        result.valid  = 1;
    }
    else // if the format of the image is not supported by stbir
    {
        int pixels_size = image.width * image.height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
        rf_color* pixels = RF_ALLOC(temp_allocator, pixels_size);

        if (pixels)
        {
            rf_bool format_success = rf_format_pixels_to_rgba32(image.data, rf_image_size(image), image.format, pixels, pixels_size);
            RF_ASSERT(format_success);

            RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
            stbir_resize_uint8((unsigned char*)pixels, image.width, image.height, 0, (unsigned char*) dst, new_width, new_height, 0, 4);
            RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

            format_success = rf_format_pixels(pixels, pixels_size, RF_UNCOMPRESSED_R8G8B8A8, dst, dst_size, image.format);
            RF_ASSERT(format_success);

            result.data   = dst;
            result.width  = new_width;
            result.height = new_height;
            result.format = image.format;
            result.valid  = 1;
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", image.width * image.height * sizeof(rf_color));

        RF_FREE(temp_allocator, pixels);
    }

    return result;
}

RF_API rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int dst_size = new_width * new_height * rf_bytes_per_pixel(image.format);
        void* dst = RF_ALLOC(allocator, dst_size);

        if (dst)
        {
            result = rf_image_resize_to_buffer(image, new_width, new_height, dst, dst_size, temp_allocator);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", dst_size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

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
RF_API rf_image rf_image_resize_nn_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size)
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
        else RF_LOG_ERROR(RF_BAD_BUFFER_SIZE, "Expected `dst` to be at least %d bytes but was %d bytes", expected_size, dst_size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

RF_API rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int dst_size = new_width * new_height * rf_bytes_per_pixel(image.format);
        void* dst = RF_ALLOC(allocator, dst_size);

        if (dst)
        {
            result = rf_image_resize_nn_to_buffer(image, new_width, new_height, dst, dst_size);
        }
        else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", dst_size);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

// Convert image data to desired format
RF_API rf_image rf_image_format_to_buffer(rf_image image, rf_uncompressed_pixel_format dst_format, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (rf_is_uncompressed_format(dst_format) && rf_is_uncompressed_format(image.format))
        {
            rf_bool success = rf_format_pixels(image.data, rf_image_size(image), image.format, dst, dst_size, dst_format);
            RF_ASSERT(success);

            result = (rf_image)
            {
                .data = dst,
                .width = image.width,
                .height = image.height,
                .format = dst_format,
                .valid = 1,
            };
        }
        else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Cannot format compressed pixel formats. Image format: %d, Destination format: %d.", image.format, dst_format);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

RF_API rf_image rf_image_format(rf_image image, rf_uncompressed_pixel_format new_format, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (rf_is_uncompressed_format(new_format) && rf_is_uncompressed_format(image.format))
        {
            int dst_size = image.width * image.height * rf_bytes_per_pixel(image.format);
            void* dst = RF_ALLOC(allocator, dst_size);

            if (dst)
            {
                rf_bool format_success = rf_format_pixels(image.data, rf_image_size(image), image.format, dst, dst_size, new_format);
                RF_ASSERT(format_success);

                result = (rf_image)
                {
                    .data = dst,
                    .width = image.width,
                    .height = image.height,
                    .format = new_format,
                    .valid = 1,
                };
            }
            else RF_LOG_ERROR(RF_BAD_ALLOC, "Allocation of size %d failed.", dst_size);
        }
        else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Cannot format compressed pixel formats. `image.format`: %d, `dst_format`: %d", image.format, new_format);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

// Apply alpha mask to image. Note 1: Returned image is GRAY_ALPHA (16bit) or RGBA (32bit). Note 2: alphaMask should be same size as image
RF_API rf_image rf_image_alpha_mask_to_buffer(rf_image image, rf_image alpha_mask, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid && alpha_mask.valid)
    {
        if (image.width == alpha_mask.width && image.height == alpha_mask.height)
        {
            if (rf_is_compressed_format(image.format) && alpha_mask.format == RF_UNCOMPRESSED_GRAYSCALE)
            {
                if (dst_size >= rf_image_size_in_format(image, RF_UNCOMPRESSED_GRAY_ALPHA))
                {
                    // Apply alpha mask to alpha channel
                    for (rf_int i = 0; i < image.width * image.height; i++)
                    {
                        unsigned char mask_pixel = 0;
                        rf_format_one_pixel(((unsigned char*)alpha_mask.data) + i, alpha_mask.format, &mask_pixel, RF_UNCOMPRESSED_GRAYSCALE);

                        // Todo: Finish implementing this function
                        //((unsigned char*)dst)[k] = mask_pixel;
                    }

                    result.data   = dst;
                    result.width  = image.width;
                    result.height = image.height;
                    result.format = RF_UNCOMPRESSED_GRAY_ALPHA;
                    result.valid  = 1;
                }
            } else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Expected compressed pixel formats. `image.format`: %d, `alpha_mask.format`: %d", image.format, alpha_mask.format);
        } else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Alpha mask must be same size as image but was w: %d, h: %d", alpha_mask.width, alpha_mask.height);
    } else RF_LOG_ERROR(RF_BAD_ARGUMENT, "One image was invalid. `image.valid`: %d, `alpha_mask.valid`: %d", image.valid, alpha_mask.valid);

    return result;
}

// Clear alpha channel to desired color. Note: Threshold defines the alpha limit, 0.0f to 1.0f
RF_API rf_image rf_image_alpha_clear(rf_image image, rf_color color, float threshold, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

        if (pixels)
        {
            for (rf_int i = 0; i < image.width * image.height; i++)
            {
                if (pixels[i].a <= (unsigned char)(threshold * 255.0f))
                {
                    pixels[i] = color;
                }
            }

            rf_image temp_image = {
                .data = pixels,
                .width = image.width,
                .height = image.height,
                .format = RF_UNCOMPRESSED_R8G8B8A8,
                .valid = 1
            };

            result = rf_image_format(temp_image, image.format, allocator);
        }

        RF_FREE(temp_allocator, pixels);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

// Premultiply alpha channel
RF_API rf_image rf_image_alpha_premultiply(rf_image image, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        float alpha = 0.0f;
        rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

        if (pixels)
        {
            for (rf_int i = 0; i < image.width * image.height; i++)
            {
                alpha = (float)pixels[i].a / 255.0f;
                pixels[i].r = (unsigned char)((float)pixels[i].r*alpha);
                pixels[i].g = (unsigned char)((float)pixels[i].g*alpha);
                pixels[i].b = (unsigned char)((float)pixels[i].b*alpha);
            }

            rf_image temp_image = {
                .data = pixels,
                .width = image.width,
                .height = image.height,
                .format = RF_UNCOMPRESSED_RGBA32,
                .valid = 1,
            };

            result = rf_image_format(temp_image, image.format, allocator);
        }

        RF_FREE(temp_allocator, pixels);
    }
    else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Image is invalid.");

    return result;
}

RF_API rf_rec rf_image_alpha_crop_rec(rf_image image, float threshold)
{
    if (!image.valid) return (rf_rec){0};

    int bpp = rf_bytes_per_pixel(image.format);

    int x_min = INT_MAX;
    int x_max = 0;
    int y_min = INT_MAX;
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
RF_API rf_image rf_image_alpha_crop(rf_image image, float threshold, rf_allocator allocator)
{
    rf_rec crop = rf_image_alpha_crop_rec(image, threshold);

    return rf_image_crop(image, crop, allocator);
}

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering) Note: In case selected bpp do not represent an known 16bit format, dithered data is stored in the LSB part of the unsigned short
RF_API rf_image rf_image_dither(const rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (image.format == RF_UNCOMPRESSED_R8G8B8)
        {
            if (image.format == RF_UNCOMPRESSED_R8G8B8 && (r_bpp + g_bpp + b_bpp + a_bpp) < 16)
            {
                rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

                if ((image.format != RF_UNCOMPRESSED_R8G8B8) && (image.format != RF_UNCOMPRESSED_R8G8B8A8))
                {
                    RF_LOG(RF_LOG_TYPE_WARNING, "rf_image format is already 16bpp or lower, dithering could have no effect");
                }

                // Todo: Finish implementing this function
                // // Define new image format, check if desired bpp match internal known format
                // if ((r_bpp == 5) && (g_bpp == 6) && (b_bpp == 5) && (a_bpp == 0)) image.format = RF_UNCOMPRESSED_R5G6B5;
                // else if ((r_bpp == 5) && (g_bpp == 5) && (b_bpp == 5) && (a_bpp == 1)) image.format = RF_UNCOMPRESSED_R5G5B5A1;
                // else if ((r_bpp == 4) && (g_bpp == 4) && (b_bpp == 4) && (a_bpp == 4)) image.format = RF_UNCOMPRESSED_R4G4B4A4;
                // else
                // {
                //     image.format = 0;
                //     RF_LOG(RF_LOG_TYPE_WARNING, "Unsupported dithered OpenGL internal format: %ibpp (R%i_g%i_b%i_a%i)", (r_bpp + g_bpp + b_bpp + a_bpp), r_bpp, g_bpp, b_bpp, a_bpp);
                // }
                //
                // // NOTE: We will store the dithered data as unsigned short (16bpp)
                // image.data = (unsigned short*) RF_ALLOC(image.allocator, image.width * image.height * sizeof(unsigned short));

                rf_color old_pixel = RF_WHITE;
                rf_color new_pixel  = RF_WHITE;

                int r_error, g_error, b_error;
                unsigned short r_pixel, g_pixel, b_pixel, a_pixel; // Used for 16bit pixel composition

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

                RF_FREE(temp_allocator, pixels);
            }
            else RF_LOG_ERROR(RF_BAD_ARGUMENT, "Unsupported dithering bpps (%ibpp), only 16bpp or lower modes supported", (r_bpp + g_bpp + b_bpp + a_bpp));
        }
    }

    return result;
}

// Flip image vertically
RF_API rf_image rf_image_flip_vertical_to_buffer(rf_image image, void* dst, rf_int dst_size)
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

RF_API rf_image rf_image_flip_vertical(rf_image image, rf_allocator allocator)
{
    if (!image.valid) return (rf_image) {0};

    int size = rf_image_size(image);
    void* dst = RF_ALLOC(allocator, size);

    rf_image result = rf_image_flip_vertical_to_buffer(image, dst, size);
    if (!result.valid) RF_FREE(allocator, dst);

    return result;
}

// Flip image horizontally
RF_API rf_image rf_image_flip_horizontal_to_buffer(rf_image image, void* dst, rf_int dst_size)
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

RF_API rf_image rf_image_flip_horizontal(rf_image image, rf_allocator allocator)
{
    if (!image.valid) return (rf_image) {0};

    int size = rf_image_size(image);
    void* dst = RF_ALLOC(allocator, size);

    rf_image result = rf_image_flip_horizontal_to_buffer(image, dst, size);
    if (!result.valid) RF_FREE(allocator, dst);

    return result;
}

// Rotate image clockwise 90deg
RF_API rf_image rf_image_rotate_cw_to_buffer(rf_image image, void* dst, rf_int dst_size)
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

RF_API rf_image rf_image_rotate_cw(rf_image image)
{
    return rf_image_rotate_cw_to_buffer(image, image.data, rf_image_size(image));
}

// Rotate image counter-clockwise 90deg
RF_API rf_image rf_image_rotate_ccw_to_buffer(rf_image image, void* dst, rf_int dst_size)
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

RF_API rf_image rf_image_rotate_ccw(rf_image image)
{
    return rf_image_rotate_ccw_to_buffer(image, image.data, rf_image_size(image));
}

// Modify image color: tint
RF_API rf_image rf_image_color_tint_to_buffer(rf_image image, rf_color color, void* dst, rf_int dst_size)
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

                    rf_format_one_pixel(&pixel_rgba32, RF_UNCOMPRESSED_R8G8B8A8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

RF_API rf_image rf_image_color_tint(rf_image image, rf_color color)
{
    return rf_image_color_tint_to_buffer(image, color, image.data, rf_image_size(image));
}

// Modify image color: invert
RF_API rf_image rf_image_color_invert_to_buffer(rf_image image, void* dst, rf_int dst_size)
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

                    rf_format_one_pixel(&pixel_rgba32, RF_UNCOMPRESSED_R8G8B8A8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

RF_API rf_image rf_image_color_invert(rf_image image)
{
    return rf_image_color_invert_to_buffer(image, image.data, rf_image_size(image));
}

// Modify image color: grayscale
RF_API rf_image rf_image_color_grayscale_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    return rf_image_format_to_buffer(image, RF_UNCOMPRESSED_GRAYSCALE, dst, dst_size);
}

RF_API rf_image rf_image_color_grayscale(rf_image image)
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
RF_API rf_image rf_image_color_contrast_to_buffer(rf_image image, float contrast, void* dst, rf_int dst_size)
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

                    rf_format_one_pixel(&src_pixel_rgba32, RF_UNCOMPRESSED_R8G8B8A8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

RF_API rf_image rf_image_color_contrast(rf_image image, int contrast)
{
    return rf_image_color_contrast_to_buffer(image, contrast, image.data, rf_image_size(image));
}

// Modify image color: brightness
// NOTE: Brightness values between -255 and 255
RF_API rf_image rf_image_color_brightness_to_buffer(rf_image image, int brightness, void* dst, rf_int dst_size)
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

                    rf_format_one_pixel(&pixel_rgba32, RF_UNCOMPRESSED_R8G8B8A8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

RF_API rf_image rf_image_color_brightness(rf_image image, int brightness)
{
    return rf_image_color_brightness_to_buffer(image, brightness, image.data, rf_image_size(image));
}

// Modify image color: replace color
RF_API rf_image rf_image_color_replace_to_buffer(rf_image image, rf_color color, rf_color replace, void* dst, rf_int dst_size)
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
                rf_format_one_pixel(&replace, RF_UNCOMPRESSED_R8G8B8A8, dst_pixel, image.format);
            }
        }
    }

    result = image;
    result.data = dst;

    return result;
}

RF_API rf_image rf_image_color_replace(rf_image image, rf_color color, rf_color replace)
{
    return rf_image_color_replace_to_buffer(image, color, replace, image.data, rf_image_size(image));
}

// Generate image: plain color
RF_API rf_image rf_gen_image_color_to_buffer(int width, int height, rf_color color, rf_color* dst, rf_int dst_size)
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
        .format = RF_UNCOMPRESSED_R8G8B8A8,
        .valid  = 1,
    };
}

RF_API rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    void* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_color_to_buffer(width, height, color, dst, width * height);
    }

    return result;
}

// Generate image: vertical gradient
RF_API rf_image rf_gen_image_gradient_v_to_buffer(int width, int height, rf_color top, rf_color bottom, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8))
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
            .format = RF_UNCOMPRESSED_R8G8B8A8,
            .valid = 1
        };
    }

    return result;
}

RF_API rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    rf_color* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_v_to_buffer(width, height, top, bottom, dst, dst_size);
    }

    return result;
}

// Generate image: horizontal gradient
RF_API rf_image rf_gen_image_gradient_h_to_buffer(int width, int height, rf_color left, rf_color right, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8))
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
            .format = RF_UNCOMPRESSED_R8G8B8A8,
            .valid = 1
        };
    }

    return result;
}

RF_API rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    rf_color* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_h_to_buffer(width, height, left, right, dst, dst_size);
    }

    return result;
}

// Generate image: radial gradient
RF_API rf_image rf_gen_image_gradient_radial_to_buffer(int width, int height, float density, rf_color inner, rf_color outer, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8))
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
            .format = RF_UNCOMPRESSED_R8G8B8A8,
            .valid = 1,
        };
    }

    return result;
}

RF_API rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    rf_color* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_radial_to_buffer(width, height, density, inner, outer, dst, dst_size);
    }

    return result;
}

// Generate image: checked
RF_API rf_image rf_gen_image_checked_to_buffer(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8))
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
            .format = RF_UNCOMPRESSED_R8G8B8A8,
            .valid = 1,
        };
    }

    return result;
}

RF_API rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    rf_color* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_checked_to_buffer(width, height, checks_x, checks_y, col1, col2, dst, dst_size);
    }

    return result;
}

// Generate image: white noise
RF_API rf_image rf_gen_image_white_noise_to_buffer(int width, int height, float factor, rf_rand_proc rand, rf_color* dst, rf_int dst_size)
{
    int result_image_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    rf_image result = {0};

    if (dst_size < result_image_size || !rand || result_image_size <= 0) return result;

    for (rf_int i = 0; i < width * height; i++)
    {
        if (rand(0, 99) < (int)(factor * 100.0f))
        {
            dst[i] = RF_WHITE;
        }
        else
        {
            dst[i] = RF_BLACK;
        }
    }

    result = (rf_image)
    {
        .data = dst,
        .width = width,
        .height = height,
        .format = RF_UNCOMPRESSED_R8G8B8A8,
        .valid = 1,
    };

    return result;
}

RF_API rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_rand_proc rand, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);

    if (!rand || dst_size <= 0) return result;

    rf_color* dst = RF_ALLOC(allocator, dst_size);
    result = rf_gen_image_white_noise_to_buffer(width, height, factor, rand, dst, dst_size);

    return result;
}

// Generate image: perlin noise
RF_API rf_image rf_gen_image_perlin_noise_to_buffer(int width, int height, int offset_x, int offset_y, float scale, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8))
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
            .format = RF_UNCOMPRESSED_R8G8B8A8,
            .valid = 1,
        };
    }

    return result;
}

RF_API rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
    rf_color* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_perlin_noise_to_buffer(width, height, offset_x, offset_y, scale, dst, dst_size);
    }

    return result;
}

RF_API rf_vec2 rf_get_seed_for_cellular_image(int seeds_per_row, int tile_size, int i, rf_rand_proc rand)
{
    rf_vec2 result = {0};

    int y = (i / seeds_per_row) * tile_size + rand(0, tile_size - 1);
    int x = (i % seeds_per_row) * tile_size + rand(0, tile_size - 1);
    result = (rf_vec2) { (float) x, (float) y };

    return result;
}

// Generate image: cellular algorithm. Bigger tileSize means bigger cells
RF_API rf_image rf_gen_image_cellular_to_buffer(int width, int height, int tile_size, rf_rand_proc rand, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    int seeds_per_row = width / tile_size;
    int seeds_per_col = height / tile_size;
    int seeds_count = seeds_per_row * seeds_per_col;

    if (dst_size >= width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8))
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
            .format = RF_UNCOMPRESSED_R8G8B8A8,
            .valid = 1,
        };
    }

    return result;
}

RF_API rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_rand_proc rand, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);

    rf_color* dst = RF_ALLOC(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_cellular_to_buffer(width, height, tile_size, rand, dst, dst_size);
    }

    return result;
}

// Draw an image (source) within an image (destination)
// NOTE: rf_color tint is applied to source image
RF_API void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator)
{
    if (src.valid && dst->valid)
    {
        dst->valid = 0;

        if (src_rec.x < 0) src_rec.x = 0;
        if (src_rec.y < 0) src_rec.y = 0;

        if ((src_rec.x + src_rec.width) > src.width)
        {
            src_rec.width = src.width - src_rec.x;
            RF_LOG(RF_LOG_TYPE_WARNING, "Source rectangle width out of bounds, rescaled width: %i", src_rec.width);
        }

        if ((src_rec.y + src_rec.height) > src.height)
        {
            src_rec.height = src.height - src_rec.y;
            RF_LOG(RF_LOG_TYPE_WARNING, "Source rectangle height out of bounds, rescaled height: %i", src_rec.height);
        }

        rf_image src_copy = rf_image_copy(src, temp_allocator); // Make a copy of source src to work with it

        // Crop source image to desired source rectangle (if required)
        if ((src.width != (int)src_rec.width) && (src.height != (int)src_rec.height))
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, src_rec, temp_allocator);
            rf_unload_image(old_src_copy, temp_allocator);
        }

        // Scale source image in case destination rec size is different than source rec size
        if (((int)dst_rec.width != (int)src_rec.width) || ((int)dst_rec.height != (int)src_rec.height))
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_resize(src_copy, (int)dst_rec.width, (int)dst_rec.height, temp_allocator, temp_allocator);
            rf_unload_image(old_src_copy, temp_allocator);
        }

        // Check that dstRec is inside dst image
        // Allow negative position within destination with cropping
        if (dst_rec.x < 0)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { -dst_rec.x, 0, dst_rec.width + dst_rec.x, dst_rec.height }, temp_allocator);
            dst_rec.width = dst_rec.width + dst_rec.x;
            dst_rec.x = 0;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if ((dst_rec.x + dst_rec.width) > dst->width)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, 0, dst->width - dst_rec.x, dst_rec.height }, temp_allocator);
            dst_rec.width = dst->width - dst_rec.x;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if (dst_rec.y < 0)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, -dst_rec.y, dst_rec.width, dst_rec.height + dst_rec.y }, temp_allocator);
            dst_rec.height = dst_rec.height + dst_rec.y;
            dst_rec.y = 0;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if ((dst_rec.y + dst_rec.height) > dst->height)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, 0, dst_rec.width, dst->height - dst_rec.y }, temp_allocator);
            dst_rec.height = dst->height - dst_rec.y;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if (src_copy.valid)
        {
            // Get image data as rf_color pixels array to work with it
            rf_color* dst_pixels = rf_image_pixels_to_rgba32(*dst, temp_allocator);
            rf_color* src_pixels = rf_image_pixels_to_rgba32(src_copy, temp_allocator);

            rf_unload_image(src_copy, temp_allocator); // Source copy not required any more

            rf_vec4 fsrc, fdst, fout; // Normalized pixel data (ready for operation)
            rf_vec4 ftint = rf_color_normalize(tint); // Normalized color tint

            // Blit pixels, copy source image into destination
            // TODO: Maybe out-of-bounds blitting could be considered here instead of so much cropping
            for (rf_int j = (int)dst_rec.y; j < (int)(dst_rec.y + dst_rec.height); j++)
            {
                for (rf_int i = (int)dst_rec.x; i < (int)(dst_rec.x + dst_rec.width); i++)
                {
                    // Alpha blending (https://en.wikipedia.org/wiki/Alpha_compositing)

                    fdst = rf_color_normalize(dst_pixels[j * (int)dst->width + i]);
                    fsrc = rf_color_normalize(src_pixels[(j - (int)dst_rec.y) * (int)dst_rec.width + (i - (int)dst_rec.x)]);

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

                    dst_pixels[j*(int)dst->width + i] = (rf_color) { (unsigned char)(fout.x * 255.0f),
                                                                    (unsigned char)(fout.y * 255.0f),
                                                                    (unsigned char)(fout.z * 255.0f),
                                                                    (unsigned char)(fout.w * 255.0f) };

                    // TODO: Support other blending options
                }
            }

            rf_bool format_success = rf_format_pixels(dst_pixels, rf_image_size_in_format(*dst, RF_UNCOMPRESSED_R8G8B8A8), RF_UNCOMPRESSED_R8G8B8A8,
                             dst->data, rf_image_size(*dst), dst->format);
            RF_ASSERT(format_success);

            RF_FREE(temp_allocator, src_pixels);
            RF_FREE(temp_allocator, dst_pixels);

            dst->valid = 1;
        }
    }
}

// Draw rectangle within an image
RF_API void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator)
{
    if (dst->valid)
    {
        dst->valid = 0;

        rf_image src = rf_gen_image_color((int)rec.width, (int)rec.height, color, temp_allocator);

        if (src.valid)
        {
            rf_rec src_rec = (rf_rec){ 0, 0, rec.width, rec.height };

            rf_image_draw(dst, src, src_rec, rec, RF_WHITE, temp_allocator);

            rf_unload_image(src, temp_allocator);
        }
    }
}

// Draw rectangle lines within an image
RF_API void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator)
{
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y, rec.width, thick }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y + thick, thick, rec.height - thick * 2 }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x + rec.width - thick, rec.y + thick, thick, rec.height - thick * 2 }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y + rec.height - thick, rec.width, thick }, color, temp_allocator);
}

#pragma endregion

#pragma region mipmaps

RF_API int rf_mipmaps_image_size(rf_mipmaps_image image)
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

RF_API rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count)
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

// Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
RF_API rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, rf_int dst_size, rf_allocator temp_allocator)
{
    if (image.valid) return (rf_mipmaps_image) {0};

    rf_mipmaps_image result = {0};
    rf_mipmaps_stats mipmap_stats = rf_compute_mipmaps_stats(image, gen_mipmaps_count);

    if (mipmap_stats.possible_mip_counts <= gen_mipmaps_count)
    {
        if (dst_size == mipmap_stats.mipmaps_buffer_size)
        {
            // Pointer to current mip location in the dst buffer
            unsigned char* dst_iter = dst;

            // Copy the image to the dst as the first mipmap level
            memcpy(dst_iter, image.data, rf_image_size(image));
            dst_iter += rf_image_size(image);

            // Create a rgba32 buffer for the mipmap result, half the image size is enough for any mipmap level
            int temp_mipmap_buffer_size = (image.width / 2) * (image.height / 2) * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
            void* temp_mipmap_buffer = RF_ALLOC(temp_allocator, temp_mipmap_buffer_size);

            if (temp_mipmap_buffer)
            {
                int mip_width  = image.width  / 2;
                int mip_height = image.height / 2;
                int mip_count = 1;
                for (; mip_count < gen_mipmaps_count; mip_count++)
                {
                    rf_image mipmap = rf_image_resize_to_buffer(image, mip_width, mip_height, temp_mipmap_buffer, temp_mipmap_buffer_size, temp_allocator);

                    if (mipmap.valid)
                    {
                        int dst_iter_size = dst_size - ((int)(dst_iter - ((unsigned char*)(dst))));

                        rf_bool success = rf_format_pixels(mipmap.data, rf_image_size(mipmap), mipmap.format, dst_iter, dst_iter_size, image.format);
                        RF_ASSERT(success);
                    }
                    else break;

                    mip_width  /= 2;
                    mip_height /= 2;

                    // Security check for NPOT textures
                    if (mip_width  < 1) mip_width  = 1;
                    if (mip_height < 1) mip_height = 1;

                    // Compute next mipmap location in the dst buffer
                    dst_iter += mip_width * mip_height * rf_bytes_per_pixel(image.format);
                }

                if (mip_count == gen_mipmaps_count)
                {
                    result = (rf_mipmaps_image){
                        .data = dst,
                        .width = image.width,
                        .height = image.height,
                        .mipmaps = gen_mipmaps_count,
                        .format = image.format,
                        .valid = 1
                    };
                }
            }

            RF_FREE(temp_allocator, temp_mipmap_buffer);
        }
    }
    else RF_LOG(RF_LOG_TYPE_WARNING, "rf_image mipmaps already available");

    return result;
}

RF_API rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (!image.valid) return (rf_mipmaps_image) {0};

    rf_mipmaps_image result = {0};
    rf_mipmaps_stats mipmap_stats = rf_compute_mipmaps_stats(image, desired_mipmaps_count);

    if (mipmap_stats.possible_mip_counts <= desired_mipmaps_count)
    {
        void* dst = RF_ALLOC(allocator, mipmap_stats.mipmaps_buffer_size);

        if (dst)
        {
            result = rf_image_gen_mipmaps_to_buffer(image, desired_mipmaps_count, dst, mipmap_stats.mipmaps_buffer_size, temp_allocator);
            if (!result.valid)
            {
                RF_FREE(allocator, dst);
            }
        }
    }

    return result;
}

RF_API void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator)
{
    RF_FREE(allocator, image.data);
}

#pragma endregion

#pragma region dds

/*
 Required extension:
 GL_EXT_texture_compression_s3tc

 Supported tokens (defined by extensions)
 GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
 GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
 GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
 GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3
*/

#define RF_FOURCC_DXT1 (0x31545844)  // Equivalent to "DXT1" in ASCII
#define RF_FOURCC_DXT3 (0x33545844)  // Equivalent to "DXT3" in ASCII
#define RF_FOURCC_DXT5 (0x35545844)  // Equivalent to "DXT5" in ASCII

typedef struct rf_dds_pixel_format rf_dds_pixel_format;
struct rf_dds_pixel_format
{
    unsigned int size;
    unsigned int flags;
    unsigned int four_cc;
    unsigned int rgb_bit_count;
    unsigned int r_bit_mask;
    unsigned int g_bit_mask;
    unsigned int b_bit_mask;
    unsigned int a_bit_mask;
};

// DDS Header (must be 124 bytes)
typedef struct rf_dds_header rf_dds_header;
struct rf_dds_header
{
    char id[4];
    unsigned int size;
    unsigned int flags;
    unsigned int height;
    unsigned int width;
    unsigned int pitch_or_linear_size;
    unsigned int depth;
    unsigned int mipmap_count;
    unsigned int reserved_1[11];
    rf_dds_pixel_format ddspf;
    unsigned int caps;
    unsigned int caps_2;
    unsigned int caps_3;
    unsigned int caps_4;
    unsigned int reserved_2;
};

RF_API rf_int rf_get_dds_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size >= sizeof(rf_dds_header))
    {
        rf_dds_header header = *(rf_dds_header*)src;

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "DDS "))
        {
            if (header.ddspf.rgb_bit_count == 16) // 16bit mode, no compressed
            {
                if (header.ddspf.flags == 0x40) // no alpha channel
                {
                    result = header.width * header.height * sizeof(unsigned short);
                }
                else if (header.ddspf.flags == 0x41) // with alpha channel
                {
                    if (header.ddspf.a_bit_mask == 0x8000) // 1bit alpha
                    {
                        result = header.width * header.height * sizeof(unsigned short);
                    }
                    else if (header.ddspf.a_bit_mask == 0xf000) // 4bit alpha
                    {
                        result = header.width * header.height * sizeof(unsigned short);
                    }
                }
            }
            else if (header.ddspf.flags == 0x40 && header.ddspf.rgb_bit_count == 24) // DDS_RGB, no compressed
            {
                // Not sure if this case exists...
                result = header.width * header.height * 3 * sizeof(unsigned char);
            }
            else if (header.ddspf.flags == 0x41 && header.ddspf.rgb_bit_count == 32) // DDS_RGBA, no compressed
            {
                result = header.width * header.height * 4 * sizeof(unsigned char);
            }
            else if (((header.ddspf.flags == 0x04) || (header.ddspf.flags == 0x05)) && (header.ddspf.four_cc > 0)) // Compressed
            {
                int size;       // DDS result data size

                // Calculate data size, including all mipmaps
                if (header.mipmap_count > 1) size = header.pitch_or_linear_size * 2;
                else size = header.pitch_or_linear_size;

                result = size * sizeof(unsigned char);
            }
        }
    }

    return result;
}

RF_API rf_mipmaps_image rf_load_dds_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_mipmaps_image result = { 0 };

    if (src && dst && dst_size > 0 && src_size >= sizeof(rf_dds_header))
    {
        rf_dds_header header = *(rf_dds_header*)src;

        src = ((char*)src) + sizeof(rf_dds_header);
        src_size -= sizeof(rf_dds_header);

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "DDS "))
        {
            result.width   = header.width;
            result.height  = header.height;
            result.mipmaps = (header.mipmap_count == 0) ? 1 : header.mipmap_count;

            if (header.ddspf.rgb_bit_count == 16) // 16bit mode, no compressed
            {
                if (header.ddspf.flags == 0x40) // no alpha channel
                {
                    int dds_result_size = header.width * header.height * sizeof(unsigned short);

                    if (src_size >= dds_result_size && dst_size >= dds_result_size)
                    {
                        memcpy(dst, src, dds_result_size);
                        result.format = RF_UNCOMPRESSED_R5G6B5;
                        result.data   = dst;
                        result.valid  = 1;
                    }
                }
                else if (header.ddspf.flags == 0x41) // with alpha channel
                {
                    if (header.ddspf.a_bit_mask == 0x8000) // 1bit alpha
                    {
                        int dds_result_size = header.width * header.height * sizeof(unsigned short);

                        if (src_size >= dds_result_size && dst_size >= dds_result_size)
                        {
                            memcpy(dst, src, dds_result_size);

                            unsigned char alpha = 0;

                            // Data comes as A1R5G5B5, it must be reordered to R5G5B5A1
                            for (rf_int i = 0; i < result.width * result.height; i++)
                            {
                                alpha = ((unsigned short *)result.data)[i] >> 15;
                                ((unsigned short*)result.data)[i] = ((unsigned short *)result.data)[i] << 1;
                                ((unsigned short*)result.data)[i] += alpha;
                            }

                            result.format = RF_UNCOMPRESSED_R5G5B5A1;
                            result.data   = dst;
                            result.valid  = 1;
                        }
                    }
                    else if (header.ddspf.a_bit_mask == 0xf000) // 4bit alpha
                    {
                        int dds_result_size = header.width * header.height * sizeof(unsigned short);

                        if (src_size >= dds_result_size && dst_size >= dds_result_size)
                        {
                            memcpy(dst, src, dds_result_size);

                            unsigned char alpha = 0;

                            // Data comes as A4R4G4B4, it must be reordered R4G4B4A4
                            for (rf_int i = 0; i < result.width * result.height; i++)
                            {
                                alpha = ((unsigned short*)result.data)[i] >> 12;
                                ((unsigned short*)result.data)[i] = ((unsigned short*)result.data)[i] << 4;
                                ((unsigned short*)result.data)[i] += alpha;
                            }

                            result.format = RF_UNCOMPRESSED_R4G4B4A4;
                            result.data   = dst;
                            result.valid  = 1;
                        }
                    }
                }
            }
            else if (header.ddspf.flags == 0x40 && header.ddspf.rgb_bit_count == 24) // DDS_RGB, no compressed, not sure if this case exists...
            {
                int dds_result_size = header.width * header.height * 3;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);
                    result.format = RF_UNCOMPRESSED_R8G8B8;
                    result.data   = dst;
                    result.valid  = 1;
                }
            }
            else if (header.ddspf.flags == 0x41 && header.ddspf.rgb_bit_count == 32) // DDS_RGBA, no compressed
            {
                int dds_result_size = header.width * header.height * 4;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);

                    unsigned char blue = 0;

                    // Data comes as A8R8G8B8, it must be reordered R8G8B8A8 (view next comment)
                    // DirecX understand ARGB as a 32bit DWORD but the actual memory byte alignment is BGRA
                    // So, we must realign B8G8R8A8 to R8G8B8A8
                    for (rf_int i = 0; i < header.width * header.height * 4; i += 4)
                    {
                        blue = ((unsigned char*)dst)[i];
                        ((unsigned char*)dst)[i + 0] = ((unsigned char*)dst)[i + 2];
                        ((unsigned char*)dst)[i + 2] = blue;
                    }

                    result.format = RF_UNCOMPRESSED_R8G8B8A8;
                    result.data   = dst;
                    result.valid  = 1;
                }
            }
            else if (((header.ddspf.flags == 0x04) || (header.ddspf.flags == 0x05)) && (header.ddspf.four_cc > 0)) // Compressed
            {
                int dds_result_size = (header.mipmap_count > 1) ? (header.pitch_or_linear_size * 2) : header.pitch_or_linear_size;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);

                    switch (header.ddspf.four_cc)
                    {
                        case RF_FOURCC_DXT1: result.format = header.ddspf.flags == 0x04 ? RF_COMPRESSED_DXT1_RGB : RF_COMPRESSED_DXT1_RGBA; break;
                        case RF_FOURCC_DXT3: result.format = RF_COMPRESSED_DXT3_RGBA; break;
                        case RF_FOURCC_DXT5: result.format = RF_COMPRESSED_DXT5_RGBA; break;
                        default: break;
                    }

                    result.data  = dst;
                    result.valid = 1;
                }
            }
        }
        else RF_LOG_ERROR(RF_BAD_FORMAT, "DDS file does not seem to be a valid result");
    }

    return result;
}

RF_API rf_mipmaps_image rf_load_dds_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_mipmaps_image result = {0};

    int dst_size = rf_get_dds_image_size(src, src_size);
    void* dst = RF_ALLOC(allocator, dst_size);

    result = rf_load_dds_image_to_buffer(src, src_size, dst, dst_size);

    return result;
}

RF_API rf_mipmaps_image rf_load_dds_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_mipmaps_image result = {0};

    int src_size = RF_FILE_SIZE(io, file);
    void* src = RF_ALLOC(temp_allocator, src_size);

    if (RF_READ_FILE(io, file, src, src_size))
    {
        result = rf_load_dds_image(src, src_size, allocator);
    }

    RF_FREE(temp_allocator, src);

    return result;
}
#pragma endregion

#pragma region pkm

/*
 Required extensions:
 GL_OES_compressed_ETC1_RGB8_texture  (ETC1) (OpenGL ES 2.0)
 GL_ARB_ES3_compatibility  (ETC2/EAC) (OpenGL ES 3.0)

 Supported tokens (defined by extensions)
 GL_ETC1_RGB8_OES                 0x8D64
 GL_COMPRESSED_RGB8_ETC2          0x9274
 GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

 Formats list
 version 10: format: 0=ETC1_RGB, [1=ETC1_RGBA, 2=ETC1_RGB_MIP, 3=ETC1_RGBA_MIP] (not used)
 version 20: format: 0=ETC1_RGB, 1=ETC2_RGB, 2=ETC2_RGBA_OLD, 3=ETC2_RGBA, 4=ETC2_RGBA1, 5=ETC2_R, 6=ETC2_RG, 7=ETC2_SIGNED_R, 8=ETC2_SIGNED_R

 NOTE: The extended width and height are the widths rounded up to a multiple of 4.
 NOTE: ETC is always 4bit per pixel (64 bit for each 4x4 block of pixels)
*/

// PKM file (ETC1) Header (16 bytes)
typedef struct rf_pkm_header rf_pkm_header;
struct rf_pkm_header
{
    char id[4];                 // "PKM "
    char version[2];            // "10" or "20"
    unsigned short format;      // Data format (big-endian) (Check list below)
    unsigned short width;       // Texture width (big-endian) (origWidth rounded to multiple of 4)
    unsigned short height;      // Texture height (big-endian) (origHeight rounded to multiple of 4)
    unsigned short orig_width;  // Original width (big-endian)
    unsigned short orig_height; // Original height (big-endian)
};

RF_API rf_int rf_get_pkm_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size > sizeof(rf_pkm_header))
    {
        rf_pkm_header header = *(rf_pkm_header*)src;

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "PKM "))
        {
            // Note: format, width and height come as big-endian, data must be swapped to little-endian
            header.format = ((header.format & 0x00FF) << 8) | ((header.format & 0xFF00) >> 8);
            header.width  = ((header.width  & 0x00FF) << 8) | ((header.width  & 0xFF00) >> 8);
            header.height = ((header.height & 0x00FF) << 8) | ((header.height & 0xFF00) >> 8);

            int bpp = 4;
            if (header.format == 3) bpp = 8;

            result = header.width * header.height * bpp / 8;
        }
        else RF_LOG_ERROR(RF_BAD_FORMAT, "PKM file does not seem to be a valid image");
    }

    return result;
}

// Load image from .pkm
RF_API rf_image rf_load_pkm_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (src && src_size >= sizeof(rf_pkm_header))
    {
        rf_pkm_header header = *(rf_pkm_header*)src;

        src = (char*)src + sizeof(rf_pkm_header);
        src_size -= sizeof(rf_pkm_header);

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "PKM "))
        {
            // Note: format, width and height come as big-endian, data must be swapped to little-endian
            result.format = ((header.format & 0x00FF) << 8) | ((header.format & 0xFF00) >> 8);
            result.width  = ((header.width  & 0x00FF) << 8) | ((header.width  & 0xFF00) >> 8);
            result.height = ((header.height & 0x00FF) << 8) | ((header.height & 0xFF00) >> 8);

            int bpp = (result.format == 3) ? 8 : 4;
            int size = result.width * result.height * bpp / 8;  // Total data size in bytes

            if (dst_size >= size && src_size >= size)
            {
                memcpy(dst, src, size);

                     if (header.format == 0) result.format = RF_COMPRESSED_ETC1_RGB;
                else if (header.format == 1) result.format = RF_COMPRESSED_ETC2_RGB;
                else if (header.format == 3) result.format = RF_COMPRESSED_ETC2_EAC_RGBA;

                result.valid = 1;
            }
        }
        else RF_LOG_ERROR(RF_BAD_FORMAT, "PKM file does not seem to be a valid image");
    }

    return result;
}

RF_API rf_image rf_load_pkm_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_image result = {0};

    if (src && src_size > 0)
    {
        int dst_size = rf_get_pkm_image_size(src, src_size);
        void* dst = RF_ALLOC(allocator, dst_size);

        result = rf_load_pkm_image_to_buffer(src, src_size, dst, dst_size);
    }

    return result;
}

RF_API rf_image rf_load_pkm_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_image result = {0};

    int src_size = RF_FILE_SIZE(io, file);
    void* src = RF_ALLOC(temp_allocator, src_size);

    if (RF_READ_FILE(io, file, src, src_size))
    {
        result = rf_load_pkm_image(src, src_size, allocator);
    }

    RF_FREE(temp_allocator, src);

    return result;
}

#pragma endregion

#pragma region ktx

/*
 Required extensions:
 GL_OES_compressed_ETC1_RGB8_texture  (ETC1)
 GL_ARB_ES3_compatibility  (ETC2/EAC)

 Supported tokens (defined by extensions)
 GL_ETC1_RGB8_OES                 0x8D64
 GL_COMPRESSED_RGB8_ETC2          0x9274
 GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

 KTX file Header (64 bytes)
 v1.1 - https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
 v2.0 - http://github.khronos.org/KTX-Specification/

 NOTE: Before start of every mipmap data block, we have: unsigned int dataSize
 TODO: Support KTX 2.2 specs!
*/

typedef struct rf_ktx_header rf_ktx_header;
struct rf_ktx_header
{
    char id[12];                          // Identifier: "«KTX 11»\r\n\x1A\n"
    unsigned int endianness;              // Little endian: 0x01 0x02 0x03 0x04
    unsigned int gl_type;                 // For compressed textures, gl_type must equal 0
    unsigned int gl_type_size;            // For compressed texture data, usually 1
    unsigned int gl_format;               // For compressed textures is 0
    unsigned int gl_internal_format;      // Compressed internal format
    unsigned int gl_base_internal_format; // Same as gl_format (RGB, RGBA, ALPHA...)
    unsigned int width;                   // Texture image width in pixels
    unsigned int height;                  // Texture image height in pixels
    unsigned int depth;                   // For 2D textures is 0
    unsigned int elements;                // Number of array elements, usually 0
    unsigned int faces;                   // Cubemap faces, for no-cubemap = 1
    unsigned int mipmap_levels;           // Non-mipmapped textures = 1
    unsigned int key_value_data_size;     // Used to encode any arbitrary data...
};

RF_API rf_int rf_get_ktx_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size >= sizeof(rf_ktx_header))
    {
        rf_ktx_header header = *(rf_ktx_header*)src;
        src = (char*)src + sizeof(rf_ktx_header) + header.key_value_data_size;
        src_size -= sizeof(rf_ktx_header) + header.key_value_data_size;

        if (rf_match_str_cstr(header.id + 1, 6, "KTX 11"))
        {
            if (src_size > sizeof(unsigned int))
            {
                memcpy(&result, src, sizeof(unsigned int));
            }
        }
    }

    return result;
}

RF_API rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_mipmaps_image result = {0};

    if (src && src_size > sizeof(rf_ktx_header))
    {
        rf_ktx_header header = *(rf_ktx_header*)src;
        src = (char*)src + sizeof(rf_ktx_header) + header.key_value_data_size;
        src_size -= sizeof(rf_ktx_header) + header.key_value_data_size;

        if (rf_match_str_cstr(header.id + 1, 6, "KTX 11"))
        {
            result.width = header.width;
            result.height = header.height;
            result.mipmaps = header.mipmap_levels;

            int image_size = 0;
            if (src_size > sizeof(unsigned int))
            {
                memcpy(&image_size, src, sizeof(unsigned int));
                src = (char*)src + sizeof(unsigned int);
                src_size -= sizeof(unsigned int);

                if (image_size >= src_size && dst_size >= image_size)
                {
                    memcpy(dst, src, image_size);
                    result.data = dst;

                    switch (header.gl_internal_format)
                    {
                        case 0x8D64: result.format = RF_COMPRESSED_ETC1_RGB; break;
                        case 0x9274: result.format = RF_COMPRESSED_ETC2_RGB; break;
                        case 0x9278: result.format = RF_COMPRESSED_ETC2_EAC_RGBA; break;
                        default: return result;
                    }

                    result.valid = 1;
                }
            }
        }
    }

    return result;
}

RF_API rf_mipmaps_image rf_load_ktx_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_mipmaps_image result = {0};

    if (src && src_size > 0)
    {
        int dst_size = rf_get_ktx_image_size(src, src_size);
        void* dst    = RF_ALLOC(allocator, dst_size);

        result = rf_load_ktx_image_to_buffer(src, src_size, dst, dst_size);
    }

    return result;
}

RF_API rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_mipmaps_image result = {0};

    int src_size = RF_FILE_SIZE(io, file);
    void* src = RF_ALLOC(temp_allocator, src_size);

    if (RF_READ_FILE(io, file, src, src_size))
    {
        result = rf_load_ktx_image(src, src_size, allocator);
    }

    RF_FREE(temp_allocator, src);

    return result;
}

#pragma endregion

#pragma region gif

// Load animated GIF data
//  - rf_image.data buffer includes all frames: [image#0][image#1][image#2][...]
//  - Number of frames is returned through 'frames' parameter
//  - Frames delay is returned through 'delays' parameter (int array)
//  - All frames are returned in RGBA format
RF_API rf_gif rf_load_animated_gif(const void* data, rf_int data_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_gif gif = {0};

    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
    {
        int component_count = 0;
        void* loaded_gif = stbi_load_gif_from_memory(data, data_size, &gif.frame_delays, &gif.width, &gif.height, &gif.frames_count, &component_count, 4);

        if (loaded_gif && component_count == 4)
        {
            int loaded_gif_size = gif.width * gif.height * rf_bytes_per_pixel(RF_UNCOMPRESSED_R8G8B8A8);
            void* dst = RF_ALLOC(allocator, loaded_gif_size);

            if (dst)
            {
                memcpy(dst, loaded_gif, loaded_gif_size);

                gif.data   = dst;
                gif.format = RF_UNCOMPRESSED_R8G8B8A8;
                gif.valid  = 1;
            }
        }

        RF_FREE(temp_allocator, loaded_gif);
    }
    RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR((rf_allocator) {0});

    return gif;
}

RF_API rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_gif result = (rf_gif) {0};

    int file_size = RF_FILE_SIZE(io, filename);
    unsigned char* buffer = RF_ALLOC(temp_allocator, file_size);

    if (RF_READ_FILE(io, filename, buffer, file_size))
    {
        result = rf_load_animated_gif(buffer, file_size, allocator, temp_allocator);
    }

    RF_FREE(temp_allocator, buffer);

    return result;
}

RF_API rf_sizei rf_gif_frame_size(rf_gif gif)
{
    rf_sizei result = {0};

    if (gif.valid)
    {
        result = (rf_sizei) { gif.width / gif.frames_count, gif.height / gif.frames_count };
    }

    return result;
}

// Returns an image pointing to the frame in the gif
RF_API rf_image rf_get_frame_from_gif(rf_gif gif, int frame)
{
    rf_image result = {0};

    if (gif.valid)
    {
        rf_sizei size = rf_gif_frame_size(gif);

        result = (rf_image)
        {
            .data   = ((unsigned char*)gif.data) + (size.width * size.height * rf_bytes_per_pixel(gif.format)) * frame,
            .width  = size.width,
            .height = size.height,
            .format = gif.format,
            .valid  = 1,
        };
    }

    return result;
}

RF_API void rf_unload_gif(rf_gif gif, rf_allocator allocator)
{
    if (gif.valid)
    {
        RF_FREE(allocator, gif.frame_delays);
        rf_unload_image(gif.image, allocator);
    }
}

#pragma endregion

#pragma region ez
#ifdef RAYFORK_EZ

#pragma region extract image data functions
RF_API rf_color* rf_image_pixels_to_rgba32_ez(rf_image image) { return rf_image_pixels_to_rgba32(image, RF_DEFAULT_ALLOCATOR); }
RF_API rf_vec4* rf_image_compute_pixels_to_normalized_ez(rf_image image) { return rf_image_compute_pixels_to_normalized(image, RF_DEFAULT_ALLOCATOR); }
RF_API rf_palette rf_image_extract_palette_ez(rf_image image, int palette_size) { return rf_image_extract_palette(image, palette_size, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region loading & unloading functions
RF_API rf_image rf_load_image_from_file_data_ez(const void* src, int src_size) { return rf_load_image_from_file_data(src, src_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_load_image_from_hdr_file_data_ez(const void* src, int src_size) { return rf_load_image_from_hdr_file_data(src, src_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_load_image_from_file_ez(const char* filename) { return rf_load_image_from_file(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API void rf_unload_image_ez(rf_image image) { rf_unload_image(image, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region image manipulation
RF_API rf_image rf_image_copy_ez(rf_image image) { return rf_image_copy(image, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_crop_ez(rf_image image, rf_rec crop) { return rf_image_crop(image, crop, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_resize_ez(rf_image image, int new_width, int new_height) { return rf_image_resize(image, new_width, new_height, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_resize_nn_ez(rf_image image, int new_width, int new_height) { return rf_image_resize_nn(image, new_width, new_height, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_format_ez(rf_image image, rf_uncompressed_pixel_format new_format) { return rf_image_format(image, new_format, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_alpha_clear_ez(rf_image image, rf_color color, float threshold) { return rf_image_alpha_clear(image, color, threshold, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_alpha_premultiply_ez(rf_image image) { return rf_image_alpha_premultiply(image, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_alpha_crop_ez(rf_image image, float threshold) { return rf_image_alpha_crop(image, threshold, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_dither_ez(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp) { return rf_image_dither(image, r_bpp, g_bpp, b_bpp, a_bpp, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_flip_vertical_ez(rf_image image) { return rf_image_flip_vertical(image, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_flip_horizontal_ez(rf_image image) { return rf_image_flip_horizontal(image, RF_DEFAULT_ALLOCATOR); }

RF_API rf_vec2 rf_get_seed_for_cellular_image_ez(int seeds_per_row, int tile_size, int i) { return rf_get_seed_for_cellular_image(
        seeds_per_row, tile_size, i, RF_DEFAULT_RAND_PROC); }

RF_API rf_image rf_gen_image_color_ez(int width, int height, rf_color color) { return rf_gen_image_color(width, height, color, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_gradient_v_ez(int width, int height, rf_color top, rf_color bottom) { return rf_gen_image_gradient_v(width, height, top, bottom, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_gradient_h_ez(int width, int height, rf_color left, rf_color right) { return rf_gen_image_gradient_h(width, height, left, right, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_gradient_radial_ez(int width, int height, float density, rf_color inner, rf_color outer) { return rf_gen_image_gradient_radial(width, height, density, inner, outer, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_checked_ez(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2) { return rf_gen_image_checked(width, height, checks_x, checks_y, col1, col2, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_white_noise_ez(int width, int height, float factor) { return rf_gen_image_white_noise(
        width, height, factor, RF_DEFAULT_RAND_PROC, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_perlin_noise_ez(int width, int height, int offset_x, int offset_y, float scale) { return rf_gen_image_perlin_noise(width, height, offset_x, offset_y, scale, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_cellular_ez(int width, int height, int tile_size) { return rf_gen_image_cellular(width,
                                                                                                              height,
                                                                                                              tile_size,
                                                                                                              RF_DEFAULT_RAND_PROC,
                                                                                                              RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region mipmaps
RF_API rf_mipmaps_image rf_image_gen_mipmaps_ez(rf_image image, int gen_mipmaps_count) { return rf_image_gen_mipmaps(image, gen_mipmaps_count, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_unload_mipmaps_image_ez(rf_mipmaps_image image) { rf_unload_mipmaps_image(image, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region dds
RF_API rf_mipmaps_image rf_load_dds_image_ez(const void* src, int src_size) { return rf_load_dds_image(src, src_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mipmaps_image rf_load_dds_image_from_file_ez(const char* file) { return rf_load_dds_image_from_file(file, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
#pragma endregion

#pragma region pkm
RF_API rf_image rf_load_pkm_image_ez(const void* src, int src_size) { return rf_load_pkm_image(src, src_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_load_pkm_image_from_file_ez(const char* file) { return rf_load_pkm_image_from_file(file, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
#pragma endregion

#pragma region ktx
RF_API rf_mipmaps_image rf_load_ktx_image_ez(const void* src, int src_size) { return rf_load_ktx_image(src, src_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mipmaps_image rf_load_ktx_image_from_file_ez(const char* file) { return rf_load_ktx_image_from_file(file, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
#pragma endregion

#endif // RAYFORK_EZ
#pragma endregion