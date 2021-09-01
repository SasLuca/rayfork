#ifndef RAYFORK_IMAGE_H
#define RAYFORK_IMAGE_H

#include "rayfork/foundation/basicdef.h"
#include "rayfork/image/pixel-format.h"
#include "rayfork/math/math-types.h"

typedef struct rf_image
{
    void*           data;    // image raw data
    int             width;   // image base width
    int             height;  // image base height
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    rf_valid_t      valid;   // True if the image is valid and can be used
} rf_image;

typedef struct rf_mipmaps_stats
{
    int possible_mip_counts;
    int mipmaps_buffer_size;
} rf_mipmaps_stats;

typedef struct rf_mipmaps_image
{
    void*           data;    // image raw data
    int             width;   // image base width
    int             height;  // image base height
    int             mipmaps; // Mipmap levels, 1 by default
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    rf_valid_t      valid;
} rf_mipmaps_image;

rf_extern void rf_image_free(rf_image image, rf_allocator allocator);
rf_extern rf_int rf_image_size(rf_image image);
rf_extern void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, rf_int palette_size);
rf_extern rf_palette rf_image_extract_palette(rf_image image, rf_int palette_size, rf_allocator allocator);
rf_extern rf_rec rf_image_alpha_border(rf_image image, float threshold);
rf_extern rf_pixel_buffer rf_image_to_pixel_buffer(rf_image image);
rf_extern rf_image rf_pixel_buffer_to_image(rf_pixel_buffer pixel_buffer, rf_int width, rf_int height);

rf_extern rf_int rf_mipmaps_image_size(rf_mipmaps_image image);
rf_extern void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator);
rf_extern rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count);

#endif // RAYFORK_IMAGE_H