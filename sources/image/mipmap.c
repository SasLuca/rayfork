#include "rayfork/image/mipmap.h"
#include "rayfork/image/image-manipulation.h"
#include "rayfork/foundation/logger.h"
#include "string.h"

// Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
rf_extern rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, rf_int dst_size, rf_allocator temp_allocator)
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
            int temp_mipmap_buffer_size = (image.width / 2) * (image.height / 2) * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
            void* temp_mipmap_buffer = rf_alloc(temp_allocator, temp_mipmap_buffer_size);

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

                        rf_pixel_buffer formatted_pixels = rf_format_pixels_to_buffer(rf_image_to_pixel_buffer(mipmap), dst_iter, dst_iter_size, image.format);
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

            rf_free(temp_allocator, temp_mipmap_buffer);
        }
    }
    else rf_log(rf_log_type_warning, "rf_image mipmaps already available");

    return result;
}

rf_extern rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (!image.valid) return (rf_mipmaps_image) {0};

    rf_mipmaps_image result = {0};
    rf_mipmaps_stats mipmap_stats = rf_compute_mipmaps_stats(image, desired_mipmaps_count);

    if (mipmap_stats.possible_mip_counts <= desired_mipmaps_count)
    {
        void* dst = rf_alloc(allocator, mipmap_stats.mipmaps_buffer_size);

        if (dst)
        {
            result = rf_image_gen_mipmaps_to_buffer(image, desired_mipmaps_count, dst, mipmap_stats.mipmaps_buffer_size, temp_allocator);
            if (!result.valid)
            {
                rf_free(allocator, dst);
            }
        }
    }

    return result;
}