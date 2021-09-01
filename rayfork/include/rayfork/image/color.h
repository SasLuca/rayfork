#ifndef RAYFORK_FOUNDATION_COLOR_H
#define RAYFORK_FOUNDATION_COLOR_H

#include "rayfork/foundation/basicdef.h"
#include "rayfork/math/math-types.h"

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

// R8G8B8A8 format
typedef struct rf_color
{
    uint8_t r, g, b, a;
} rf_color;

typedef struct rf_palette
{
    rf_color* colors;
    rf_int    size;
} rf_palette;

rf_extern bool rf_color_match_rgb(rf_color a, rf_color b); // Returns true if the two colors have the same values for the rgb components
rf_extern bool rf_color_match(rf_color a, rf_color b); // Returns true if the two colors have the same values
rf_extern int32_t rf_color_to_int(rf_color color); // Returns hexadecimal value for a rf_color
rf_extern rf_vec4 rf_color_normalize(rf_color color); // Returns color normalized as float [0..1]
rf_extern rf_color rf_color_from_normalized(rf_vec4 normalized); // Returns color from normalized values [0..1]
rf_extern rf_vec3 rf_color_to_hsv(rf_color color); // Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
rf_extern rf_color rf_color_from_hsv(rf_vec3 hsv); // Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
rf_extern rf_color rf_color_from_int(int32_t hex_value); // Returns a rf_color struct from hexadecimal value
rf_extern rf_color rf_fade(rf_color color, float alpha); // rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f

#endif // RAYFORK_FOUNDATION_COLOR_H