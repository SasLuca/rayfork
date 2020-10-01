#ifndef RAYFORK_COLOR_H
#define RAYFORK_COLOR_H

#include "rayfork-core.h"

#define RF_LIGHTGRAY  (RF_LIT(rf_color) { 200, 200, 200, 255 })
#define RF_GRAY       (RF_LIT(rf_color) { 130, 130, 130, 255 })
#define RF_DARKGRAY   (RF_LIT(rf_color) {  80,  80,  80, 255 })
#define RF_YELLOW     (RF_LIT(rf_color) { 253, 249,   0, 255 })
#define RF_GOLD       (RF_LIT(rf_color) { 255, 203,   0, 255 })
#define RF_ORANGE     (RF_LIT(rf_color) { 255, 161,   0, 255 })
#define RF_PINK       (RF_LIT(rf_color) { 255, 109, 194, 255 })
#define RF_RED        (RF_LIT(rf_color) { 230,  41,  55, 255 })
#define RF_MAROON     (RF_LIT(rf_color) { 190,  33,  55, 255 })
#define RF_GREEN      (RF_LIT(rf_color) {   0, 228,  48, 255 })
#define RF_LIME       (RF_LIT(rf_color) {   0, 158,  47, 255 })
#define RF_DARKGREEN  (RF_LIT(rf_color) {   0, 117,  44, 255 })
#define RF_SKYBLUE    (RF_LIT(rf_color) { 102, 191, 255, 255 })
#define RF_BLUE       (RF_LIT(rf_color) {   0, 121, 241, 255 })
#define RF_DARKBLUE   (RF_LIT(rf_color) {   0,  82, 172, 255 })
#define RF_PURPLE     (RF_LIT(rf_color) { 200, 122, 255, 255 })
#define RF_VIOLET     (RF_LIT(rf_color) { 135,  60, 190, 255 })
#define RF_DARKPURPLE (RF_LIT(rf_color) { 112,  31, 126, 255 })
#define RF_BEIGE      (RF_LIT(rf_color) { 211, 176, 131, 255 })
#define RF_BROWN      (RF_LIT(rf_color) { 127, 106,  79, 255 })
#define RF_DARKBROWN  (RF_LIT(rf_color) {  76,  63,  47, 255 })

#define RF_WHITE      (RF_LIT(rf_color) { 255, 255, 255, 255 })
#define RF_BLACK      (RF_LIT(rf_color) {   0,   0,   0, 255 })
#define RF_BLANK      (RF_LIT(rf_color) {   0,   0,   0,   0 })
#define RF_MAGENTA    (RF_LIT(rf_color) { 255,   0, 255, 255 })
#define RF_RAYWHITE   (RF_LIT(rf_color) { 245, 245, 245, 255 })

#define RF_DEFAULT_KEY_COLOR (RF_MAGENTA)

typedef enum rf_pixel_format
{
    RF_UNCOMPRESSED_GRAYSCALE = 1,                             // 8 bit per pixel (no alpha)
    RF_UNCOMPRESSED_GRAY_ALPHA,                                // 8 * 2 bpp (2 channels)
    RF_UNCOMPRESSED_R5G6B5,                                    // 16 bpp
    RF_UNCOMPRESSED_R8G8B8,                                    // 24 bpp
    RF_UNCOMPRESSED_R5G5B5A1,                                  // 16 bpp (1 bit alpha)
    RF_UNCOMPRESSED_R4G4B4A4,                                  // 16 bpp (4 bit alpha)
    RF_UNCOMPRESSED_R8G8B8A8,                                  // 32 bpp
    RF_UNCOMPRESSED_RGBA32 = RF_UNCOMPRESSED_R8G8B8A8,         // 32 bpp
    RF_UNCOMPRESSED_R32,                                       // 32 bpp (1 channel - float)
    RF_UNCOMPRESSED_R32G32B32,                                 // 32 * 3 bpp (3 channels - float)
    RF_UNCOMPRESSED_R32G32B32A32,                              // 32 * 4 bpp (4 channels - float)
    RF_UNCOMPRESSED_NORMALIZED = RF_UNCOMPRESSED_R32G32B32A32, // 32 * 4 bpp (4 channels - float)
    RF_COMPRESSED_DXT1_RGB,                                    // 4 bpp (no alpha)
    RF_COMPRESSED_DXT1_RGBA,                                   // 4 bpp (1 bit alpha)
    RF_COMPRESSED_DXT3_RGBA,                                   // 8 bpp
    RF_COMPRESSED_DXT5_RGBA,                                   // 8 bpp
    RF_COMPRESSED_ETC1_RGB,                                    // 4 bpp
    RF_COMPRESSED_ETC2_RGB,                                    // 4 bpp
    RF_COMPRESSED_ETC2_EAC_RGBA,                               // 8 bpp
    RF_COMPRESSED_PVRT_RGB,                                    // 4 bpp
    RF_COMPRESSED_PVRT_RGBA,                                   // 4 bpp
    RF_COMPRESSED_ASTC_4x4_RGBA,                               // 8 bpp
    RF_COMPRESSED_ASTC_8x8_RGBA                                // 2 bpp
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
RF_API const char* rf_pixel_format_string(rf_pixel_format format);
RF_API rf_bool rf_is_uncompressed_format(rf_pixel_format format);
RF_API rf_bool rf_is_compressed_format(rf_pixel_format format);
RF_API int rf_bits_per_pixel(rf_pixel_format format);
RF_API int rf_bytes_per_pixel(rf_uncompressed_pixel_format format);
RF_API int rf_pixel_buffer_size(int width, int height, rf_pixel_format format);

RF_API rf_bool rf_format_pixels_to_normalized(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, rf_vec4* dst, rf_int dst_size);
RF_API rf_bool rf_format_pixels_to_rgba32(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, rf_color* dst, rf_int dst_size);
RF_API rf_bool rf_format_pixels(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format);

RF_API rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_uncompressed_pixel_format src_format);
RF_API rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_uncompressed_pixel_format src_format);
RF_API void rf_format_one_pixel(const void* src, rf_uncompressed_pixel_format src_format, void* dst, rf_uncompressed_pixel_format dst_format);
#pragma endregion

#pragma region color
RF_API rf_bool rf_color_match_rgb(rf_color a, rf_color b); // Returns true if the two colors have the same values for the rgb components
RF_API rf_bool rf_color_match(rf_color a, rf_color b); // Returns true if the two colors have the same values
RF_API int rf_color_to_int(rf_color color); // Returns hexadecimal value for a rf_color
RF_API rf_vec4 rf_color_normalize(rf_color color); // Returns color normalized as float [0..1]
RF_API rf_color rf_color_from_normalized(rf_vec4 normalized); // Returns color from normalized values [0..1]
RF_API rf_vec3 rf_color_to_hsv(rf_color color); // Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
RF_API rf_color rf_color_from_hsv(rf_vec3 hsv); // Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
RF_API rf_color rf_color_from_int(int hex_value); // Returns a rf_color struct from hexadecimal value
RF_API rf_color rf_fade(rf_color color, float alpha); // rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
#pragma endregion

#endif // RAYFORK_COLOR_H