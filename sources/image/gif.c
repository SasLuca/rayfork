#include "rayfork/image/gif.h"
#include "rayfork/image/image.h"
#include "rayfork/foundation/logger.h"
#include "stb_image.h"
#include "string.h"

// Load animated GIF data
//  - rf_image.data buffer includes all frames: [image#0][image#1][image#2][...]
//  - Number of frames is returned through 'frames' parameter
//  - Frames delay is returned through 'delays' parameter (int array)
//  - All frames are returned in RGBA format
rf_extern rf_gif rf_load_animated_gif(const void* data, rf_int data_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_gif gif = {0};

    rf_global_allocator_for_thirdparty_libraries = (temp_allocator);
    {
        int component_count = 0;
        void* loaded_gif = stbi_load_gif_from_memory(data, data_size, &gif.frame_delays, &gif.width, &gif.height, &gif.frames_count, &component_count, 4);

        if (loaded_gif && component_count == 4)
        {
            int loaded_gif_size = gif.width * gif.height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
            void* dst = rf_alloc(allocator, loaded_gif_size);

            if (dst)
            {
                memcpy(dst, loaded_gif, loaded_gif_size);

                gif.data   = dst;
                gif.format = rf_pixel_format_r8g8b8a8;
                gif.valid  = 1;
            }
        }

        rf_free(temp_allocator, loaded_gif);
    }
    rf_global_allocator_for_thirdparty_libraries = ((rf_allocator) {0});

    return gif;
}

rf_extern rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_gif result = (rf_gif) {0};

    rf_file_contents file_contents = rf_read_entire_file(filename, temp_allocator, io);

    if (file_contents.valid)
    {
        result = rf_load_animated_gif(file_contents.data, file_contents.size, allocator, temp_allocator);
    }

    rf_free(temp_allocator, file_contents.data);

    return result;
}

rf_extern rf_sizei rf_gif_frame_size(rf_gif gif)
{
    rf_sizei result = {0};

    if (gif.valid)
    {
        result = (rf_sizei) { gif.width / gif.frames_count, gif.height / gif.frames_count };
    }

    return result;
}

// Returns an image pointing to the frame in the gif
rf_extern rf_image rf_get_frame_from_gif(rf_gif gif, rf_int frame)
{
    rf_image result = {0};

    if (gif.valid)
    {
        rf_sizei size = rf_gif_frame_size(gif);

        result = (rf_image)
        {
            .data   = ((char*)gif.data) + (size.width * size.height * rf_bytes_per_pixel(gif.format)) * frame,
            .width  = size.width,
            .height = size.height,
            .format = gif.format,
            .valid  = 1,
        };
    }

    return result;
}

rf_extern void rf_unload_gif(rf_gif gif, rf_allocator allocator)
{
    if (gif.valid)
    {
        rf_free(allocator, gif.frame_delays);
        rf_free(allocator, gif.data);
    }
}