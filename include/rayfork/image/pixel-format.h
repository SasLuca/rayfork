#ifndef RAYFORK_PIXEL_FORMAT_H
#define RAYFORK_PIXEL_FORMAT_H

#include "rayfork/math/math-types.h"
#include "rayfork/foundation/str.h"
#include "color.h"

typedef enum rf_pixel_format
{
    rf_pixel_format_grayscale = 1,                             // 8 bit per pixel (no alpha)
    rf_pixel_format_gray_alpha,                                // 8 * 2 bpp (2 channels)
    rf_pixel_format_r5g6b5,                                    // 16 bpp
    rf_pixel_format_r8g8b8,                                    // 24 bpp
    rf_pixel_format_r5g5b5a1,                                  // 16 bpp (1 bit alpha)
    rf_pixel_format_r4g4b4a4,                                  // 16 bpp (4 bit alpha)
    rf_pixel_format_r8g8b8a8,                                  // 32 bpp
    rf_pixel_format_r32,                                       // 32 bpp (1 channel - float)
    rf_pixel_format_r32g32b32,                                 // 32 * 3 bpp (3 channels - float)
    rf_pixel_format_r32g32b32a32,                              // 32 * 4 bpp (4 channels - float) (normalized)
    rf_pixel_format_dxt1_rgb,                                  // 4 bpp (no alpha)
    rf_pixel_format_dxt1_rgba,                                 // 4 bpp (1 bit alpha)
    rf_pixel_format_dxt3_rgba,                                 // 8 bpp
    rf_pixel_format_dxt5_rgba,                                 // 8 bpp
    rf_pixel_format_etc1_rgb,                                  // 4 bpp
    rf_pixel_format_etc2_rgb,                                  // 4 bpp
    rf_pixel_format_etc2_eac_rgba,                             // 8 bpp
    rf_pixel_format_pvrt_rgb,                                  // 4 bpp
    rf_pixel_format_prvt_rgba,                                 // 4 bpp
    rf_pixel_format_astc_4x4_rgba,                             // 8 bpp
    rf_pixel_format_astc_8x8_rgba                              // 2 bpp
} rf_pixel_format;

typedef struct rf_pixel_buffer
{
    rf_int          size;
    void*           data;
    rf_pixel_format format;
    rf_valid_t      valid;
} rf_pixel_buffer;

rf_extern rf_str rf_pixel_format_to_str(rf_pixel_format format);
rf_extern bool rf_has_alpha_channel(rf_pixel_format format);
rf_extern bool rf_is_uncompressed_format(rf_pixel_format format);
rf_extern bool rf_is_compressed_format(rf_pixel_format format);
rf_extern rf_int rf_bits_per_pixel(rf_pixel_format format);
rf_extern rf_int rf_bytes_per_pixel(rf_pixel_format format);
rf_extern rf_int rf_image_size_in_format(rf_int width, rf_int height, rf_pixel_format format);
rf_extern void rf_pixel_buffer_free(rf_pixel_buffer it, rf_allocator allocator);

rf_extern rf_pixel_buffer rf_format_pixels_to_normalized(rf_pixel_buffer src, rf_allocator allocator);
rf_extern rf_pixel_buffer rf_format_pixels_to_rgba32(rf_pixel_buffer src, rf_allocator allocator);
rf_extern rf_pixel_buffer rf_format_pixels(rf_pixel_buffer src, rf_pixel_format dst_format, rf_allocator allocator);

rf_extern rf_pixel_buffer rf_format_pixels_to_normalized_to_buffer(rf_pixel_buffer src, rf_vec4* dst, rf_int dst_size_in_bytes);
rf_extern rf_pixel_buffer rf_format_pixels_to_rgba32_to_buffer(rf_pixel_buffer src, rf_color* dst, rf_int dst_size_in_bytes);
rf_extern rf_pixel_buffer rf_format_pixels_to_buffer(rf_pixel_buffer src, void* dst, rf_int dst_size, rf_pixel_format dst_format);

rf_extern rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_pixel_format src_format);
rf_extern rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_pixel_format src_format);
rf_extern void rf_format_one_pixel(const void* src, rf_pixel_format src_format, void* dst, rf_pixel_format dst_format);

#endif // RAYFORK_PIXEL_FORMAT_H