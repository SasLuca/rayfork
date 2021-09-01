#ifndef RAYFORK_IMAGE_KTX_LOADING_H
#define RAYFORK_IMAGE_KTX_LOADING_H

#include "rayfork/foundation/io.h"
#include "rayfork/image/image.h"

rf_extern rf_int rf_get_ktx_image_size(const void* src, rf_int src_size);
rf_extern rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size);
rf_extern rf_mipmaps_image rf_load_ktx_image(const void* src, rf_int src_size, rf_allocator allocator);
rf_extern rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io);

#endif // RAYFORK_IMAGE_KTX_LOADING_H