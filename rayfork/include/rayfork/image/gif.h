#ifndef RAYFORK_GIF_H
#define RAYFORK_GIF_H

#include "rayfork/image/image.h"
#include "rayfork/foundation/io.h"

typedef struct rf_gif
{
    int* frame_delays;
    int frames_count;

    void*           data;    // rf_image raw data
    int             width;   // rf_image base width
    int             height;  // rf_image base height
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    rf_valid_t      valid;
} rf_gif;

rf_extern rf_gif rf_load_animated_gif(const void* data, rf_int data_size, rf_allocator allocator, rf_allocator temp_allocator);
rf_extern rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io);
rf_extern rf_sizei rf_gif_frame_size(rf_gif gif);
rf_extern rf_image rf_get_frame_from_gif(rf_gif gif, rf_int frame);
rf_extern void rf_unload_gif(rf_gif gif, rf_allocator allocator);

#endif // RAYFORK_GIF_H