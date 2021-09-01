#ifndef RAYFORK_MIPMAP_H
#define RAYFORK_MIPMAP_H

#include "rayfork/image/image.h"

rf_extern rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, rf_int dst_size, rf_allocator temp_allocator);  // Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
rf_extern rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator);

#endif // RAYFORK_MIPMAP_H