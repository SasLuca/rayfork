#ifndef RAYFORK_COLOR_H
#define RAYFORK_COLOR_H

#include "rayfork-core.h"

#define rf_lightgray   (rf_lit(rf_color) { 200, 200, 200, 255 })
#define rf_gray        (rf_lit(rf_color) { 130, 130, 130, 255 })
#define rf_dark_gray   (rf_lit(rf_color) {  80,  80,  80, 255 })
#define rf_yellow      (rf_lit(rf_color) { 253, 249,   0, 255 })
#define rf_gold        (rf_lit(rf_color) { 255, 203,   0, 255 })
#define rf_orange      (rf_lit(rf_color) { 255, 161,   0, 255 })
#define rf_pink        (rf_lit(rf_color) { 255, 109, 194, 255 })
#define rf_red         (rf_lit(rf_color) { 230,  41,  55, 255 })
#define rf_maroon      (rf_lit(rf_color) { 190,  33,  55, 255 })
#define rf_green       (rf_lit(rf_color) {   0, 228,  48, 255 })
#define rf_lime        (rf_lit(rf_color) {   0, 158,  47, 255 })
#define rf_dark_green  (rf_lit(rf_color) {   0, 117,  44, 255 })
#define rf_sky_blue    (rf_lit(rf_color) { 102, 191, 255, 255 })
#define rf_blue        (rf_lit(rf_color) {   0, 121, 241, 255 })
#define rf_dark_blue   (rf_lit(rf_color) {   0,  82, 172, 255 })
#define rf_purple      (rf_lit(rf_color) { 200, 122, 255, 255 })
#define rf_violet      (rf_lit(rf_color) { 135,  60, 190, 255 })
#define rf_dark_purple (rf_lit(rf_color) { 112,  31, 126, 255 })
#define rf_beige       (rf_lit(rf_color) { 211, 176, 131, 255 })
#define rf_brown       (rf_lit(rf_color) { 127, 106,  79, 255 })
#define rf_dark_brown  (rf_lit(rf_color) {  76,  63,  47, 255 })

#define rf_white      (rf_lit(rf_color) { 255, 255, 255, 255 })
#define rf_black      (rf_lit(rf_color) {   0,   0,   0, 255 })
#define rf_blank      (rf_lit(rf_color) {   0,   0,   0,   0 })
#define rf_magenta    (rf_lit(rf_color) { 255,   0, 255, 255 })
#define rf_raywhite   (rf_lit(rf_color) { 245, 245, 245, 255 })

#define rf_default_key_color (rf_magenta)

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
    rf_pixel_format_r32g32b32a32,                              // 32 * 4 bpp (4 channels - float)
    rf_pixel_format_normalized = rf_pixel_format_r32g32b32a32, // 32 * 4 bpp (4 channels - float)
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

typedef enum rf_pixel_format rf_compressed_pixel_format;
typedef enum rf_pixel_format rf_uncompressed_pixel_format;

// R8G8B8A8 format
typedef struct rf_color
{
    unsigned char r, g, b, a;
} rf_color;

typedef struct rf_palette
{
    rf_color* colors;
    int count;
} rf_palette;

#pragma region pixel format
rf_public const char* rf_pixel_format_string(rf_pixel_format format);
rf_public rf_bool rf_is_uncompressed_format(rf_pixel_format format);
rf_public rf_bool rf_is_compressed_format(rf_pixel_format format);
rf_public int rf_bits_per_pixel(rf_pixel_format format);
rf_public int rf_bytes_per_pixel(rf_uncompressed_pixel_format format);
rf_public int rf_pixel_buffer_size(int width, int height, rf_pixel_format format);

rf_public rf_bool rf_format_pixels_to_normalized(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, rf_vec4* dst, rf_int dst_size);
rf_public rf_bool rf_format_pixels_to_rgba32(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, rf_color* dst, rf_int dst_size);
rf_public rf_bool rf_format_pixels(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format);

rf_public rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_uncompressed_pixel_format src_format);
rf_public rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_uncompressed_pixel_format src_format);
rf_public void rf_format_one_pixel(const void* src, rf_uncompressed_pixel_format src_format, void* dst, rf_uncompressed_pixel_format dst_format);
#pragma endregion

#pragma region color
rf_public rf_bool rf_color_match_rgb(rf_color a, rf_color b); // Returns true if the two colors have the same values for the rgb components
rf_public rf_bool rf_color_match(rf_color a, rf_color b); // Returns true if the two colors have the same values
rf_public int rf_color_to_int(rf_color color); // Returns hexadecimal value for a rf_color
rf_public rf_vec4 rf_color_normalize(rf_color color); // Returns color normalized as float [0..1]
rf_public rf_color rf_color_from_normalized(rf_vec4 normalized); // Returns color from normalized values [0..1]
rf_public rf_vec3 rf_color_to_hsv(rf_color color); // Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
rf_public rf_color rf_color_from_hsv(rf_vec3 hsv); // Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
rf_public rf_color rf_color_from_int(int hex_value); // Returns a rf_color struct from hexadecimal value
rf_public rf_color rf_fade(rf_color color, float alpha); // rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
#pragma endregion

#endif // RAYFORK_COLOR_H