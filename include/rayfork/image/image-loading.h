#include "rayfork/image/image.h"
#include "rayfork/foundation/allocator.h"
#include "rayfork/foundation/io.h"

typedef enum rf_desired_channels
{
    rf_desired_channels_any = 0,
    rf_desired_channels_1byte_grayscale = 1,
    rf_desired_channels_2byte_grayalpha = 2,
    rf_desired_channels_3byte_r8g8b8    = 3,
    rf_desired_channels_4byte_r88g8b8a8 = 4,
} rf_desired_channels;

rf_extern bool rf_supports_image_file_type(rf_str filename);

rf_extern rf_image rf_load_image_from_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
rf_extern rf_image rf_load_image_from_file_data(const void* src, rf_int src_size, rf_desired_channels channels, rf_allocator allocator, rf_allocator temp_allocator);

rf_extern rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
rf_extern rf_image rf_load_image_from_hdr_file_data(const void* src, rf_int src_size, rf_allocator allocator, rf_allocator temp_allocator);

rf_extern rf_image rf_load_image_from_format_to_buffer(const void* src, rf_int src_size, int src_width, int src_height, rf_pixel_format src_format, void* dst, rf_int dst_size, rf_pixel_format dst_format);
rf_extern rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io);