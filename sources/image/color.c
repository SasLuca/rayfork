#include "../include/rayfork/image/color.h"

// Returns 1 if the two colors have the same values for the rgb components
rf_extern bool rf_color_match_rgb(rf_color a, rf_color b)
{
    bool result = a.r == b.r && a.g == b.g && a.b == b.b;
    return result;
}

// Returns 1 if the two colors have the same values
rf_extern bool rf_color_match(rf_color a, rf_color b)
{
    bool result = a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
    return result;
}

// Returns hexadecimal value for a rf_color
rf_extern int32_t rf_color_to_int(rf_color color)
{
    int32_t result = (((int32_t) color.r << 24) | ((int32_t) color.g << 16) | ((int32_t) color.b << 8) | (int32_t) color.a);
    return result;
}

// Returns color normalized as float [0..1]
rf_extern rf_vec4 rf_color_normalize(rf_color color)
{
    rf_vec4 result;

    result.x = (float) color.r / 255.0f;
    result.y = (float) color.g / 255.0f;
    result.z = (float) color.b / 255.0f;
    result.w = (float) color.a / 255.0f;

    return result;
}

// Returns color from normalized values [0..1]
rf_extern rf_color rf_color_from_normalized(rf_vec4 normalized)
{
    rf_color result;

    result.r = normalized.x * 255.0f;
    result.g = normalized.y * 255.0f;
    result.b = normalized.z * 255.0f;
    result.a = normalized.w * 255.0f;

    return result;
}

// Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
rf_extern rf_vec3 rf_color_to_hsv(rf_color color)
{
    rf_vec3 rgb = { (float) color.r / 255.0f, (float) color.g / 255.0f, (float) color.b / 255.0f };
    rf_vec3 hsv = { 0.0f, 0.0f, 0.0f };
    float min, max, delta;

    min = rgb.x < rgb.y ? rgb.x : rgb.y;
    min = min < rgb.z ? min : rgb.z;

    max = rgb.x > rgb.y ? rgb.x : rgb.y;
    max = max > rgb.z ? max : rgb.z;

    hsv.z = max; // Value
    delta = max - min;

    if (delta < 0.00001f)
    {
        hsv.y = 0.0f;
        hsv.x = 0.0f; // Undefined, maybe NAN?
        return hsv;
    }

    if (max > 0.0f)
    {
        // NOTE: If max is 0, this divide would cause a crash
        hsv.y = (delta / max); // Saturation
    }
    else
    {
        // NOTE: If max is 0, then r = g = b = 0, s = 0, h is undefined
        hsv.y = 0.0f;
        hsv.x = NAN; // Undefined
        return hsv;
    }

    // NOTE: Comparing float values could not work properly
    if (rgb.x >= max) hsv.x = (rgb.y - rgb.z) / delta; // Between yellow & magenta
    else
    {
        if (rgb.y >= max) hsv.x = 2.0f + (rgb.z - rgb.x) / delta; // Between cyan & yellow
        else hsv.x = 4.0f + (rgb.x - rgb.y) / delta; // Between magenta & cyan
    }

    hsv.x *= 60.0f; // Convert to degrees

    if (hsv.x < 0.0f) hsv.x += 360.0f;

    return hsv;
}

// Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
rf_extern rf_color rf_color_from_hsv(rf_vec3 hsv)
{
    rf_color color = {0, 0, 0, 255};
    float h = hsv.x, s = hsv.y, v = hsv.z;

    // Red channel
    float k = fmod((5.0f + h / 60.0f), 6);
    float t = 4.0f - k;
    k = (t < k) ? t : k;
    k = (k < 1) ? k : 1;
    k = (k > 0) ? k : 0;
    color.r = (v - v * s * k) * 255;

    // Green channel
    k = fmod((3.0f + h / 60.0f), 6);
    t = 4.0f - k;
    k = (t < k) ? t : k;
    k = (k < 1) ? k : 1;
    k = (k > 0) ? k : 0;
    color.g = (v - v * s * k) * 255;

    // Blue channel
    k = fmod((1.0f + h / 60.0f), 6);
    t = 4.0f - k;
    k = (t < k) ? t : k;
    k = (k < 1) ? k : 1;
    k = (k > 0) ? k : 0;
    color.b = (v - v * s * k) * 255;

    return color;
}

// Returns a rf_color struct from hexadecimal value
rf_extern rf_color rf_color_from_int(int32_t hex_value)
{
    rf_color color;

    color.r = (unsigned char) (hex_value >> 24) & 0xFF;
    color.g = (unsigned char) (hex_value >> 16) & 0xFF;
    color.b = (unsigned char) (hex_value >> 8) & 0xFF;
    color.a = (unsigned char) hex_value & 0xFF;

    return color;
}

// rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
rf_extern rf_color rf_fade(rf_color color, float alpha)
{
    if (alpha < 0.0f)
    {
        alpha = 0.0f;
    }
    else if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }

    return (rf_color) { color.r, color.g, color.b, (uint8_t) (255.0f * alpha) };
}