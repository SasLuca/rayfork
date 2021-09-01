#include "rayfork/image/pixel-format.h"
#include "rayfork/foundation/logger.h"
#include "string.h"
#include "math.h"

rf_extern rf_str rf_pixel_format_to_str(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale:     return rf_cstr("rf_pixel_format_grayscale");
        case rf_pixel_format_gray_alpha:    return rf_cstr("rf_pixel_format_gray_alpha");
        case rf_pixel_format_r5g6b5:        return rf_cstr("rf_pixel_format_r5g6b5");
        case rf_pixel_format_r8g8b8:        return rf_cstr("rf_pixel_format_r8g8b8");
        case rf_pixel_format_r5g5b5a1:      return rf_cstr("rf_pixel_format_r5g5b5a1");
        case rf_pixel_format_r4g4b4a4:      return rf_cstr("rf_pixel_format_r4g4b4a4");
        case rf_pixel_format_r8g8b8a8:      return rf_cstr("rf_pixel_format_r8g8b8a8");
        case rf_pixel_format_r32:           return rf_cstr("rf_pixel_format_r32");
        case rf_pixel_format_r32g32b32:     return rf_cstr("rf_pixel_format_r32g32b32");
        case rf_pixel_format_r32g32b32a32:  return rf_cstr("rf_pixel_format_r32g32b32a32");
        case rf_pixel_format_dxt1_rgb:      return rf_cstr("rf_pixel_format_dxt1_rgb");
        case rf_pixel_format_dxt1_rgba:     return rf_cstr("rf_pixel_format_dxt1_rgba");
        case rf_pixel_format_dxt3_rgba:     return rf_cstr("rf_pixel_format_dxt3_rgba");
        case rf_pixel_format_dxt5_rgba:     return rf_cstr("rf_pixel_format_dxt5_rgba");
        case rf_pixel_format_etc1_rgb:      return rf_cstr("rf_pixel_format_etc1_rgb");
        case rf_pixel_format_etc2_rgb:      return rf_cstr("rf_pixel_format_etc2_rgb");
        case rf_pixel_format_etc2_eac_rgba: return rf_cstr("rf_pixel_format_etc2_eac_rgba");
        case rf_pixel_format_pvrt_rgb:      return rf_cstr("rf_pixel_format_pvrt_rgb");
        case rf_pixel_format_prvt_rgba:     return rf_cstr("rf_pixel_format_prvt_rgba");
        case rf_pixel_format_astc_4x4_rgba: return rf_cstr("rf_pixel_format_astc_4x4_rgba");
        case rf_pixel_format_astc_8x8_rgba: return rf_cstr("rf_pixel_format_astc_8x8_rgba");
        default:                            return (rf_str) {0};
    }
}

rf_extern bool rf_has_alpha_channel(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_gray_alpha:
        case rf_pixel_format_r5g5b5a1:
        case rf_pixel_format_r4g4b4a4:
        case rf_pixel_format_r8g8b8a8:
        case rf_pixel_format_r32g32b32a32:
        case rf_pixel_format_dxt1_rgba:
        case rf_pixel_format_dxt3_rgba:
        case rf_pixel_format_dxt5_rgba:
        case rf_pixel_format_etc2_eac_rgba:
        case rf_pixel_format_prvt_rgba:
        case rf_pixel_format_astc_4x4_rgba:
        case rf_pixel_format_astc_8x8_rgba:
            return true;
        default:
            return false;
    }
}

rf_extern bool rf_is_uncompressed_format(rf_pixel_format format)
{
    return format >= rf_pixel_format_grayscale && format <= rf_pixel_format_r32g32b32a32;
}

rf_extern bool rf_is_compressed_format(rf_pixel_format format)
{
    return format >= rf_pixel_format_dxt1_rgb && format <= rf_pixel_format_astc_8x8_rgba;
}

rf_extern rf_int rf_bits_per_pixel(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale:     return 8;      // 8 bit per pixel (no alpha)
        case rf_pixel_format_gray_alpha:    return 8 * 2;  // 8 * 2 bpp (2 channels)
        case rf_pixel_format_r5g6b5:        return 16;     // 16 bpp
        case rf_pixel_format_r8g8b8:        return 24;     // 24 bpp
        case rf_pixel_format_r5g5b5a1:      return 16;     // 16 bpp (1 bit alpha)
        case rf_pixel_format_r4g4b4a4:      return 16;     // 16 bpp (4 bit alpha)
        case rf_pixel_format_r8g8b8a8:      return 32;     // 32 bpp
        case rf_pixel_format_r32:           return 32;     // 32 bpp (1 channel - float)
        case rf_pixel_format_r32g32b32:     return 32 * 3; // 32 * 3 bpp (3 channels - float)
        case rf_pixel_format_r32g32b32a32:  return 32 * 4; // 32 * 4 bpp (4 channels - float)
        case rf_pixel_format_dxt1_rgb:      return 4;      // 4 bpp (no alpha)
        case rf_pixel_format_dxt1_rgba:     return 4;      // 4 bpp (1 bit alpha)
        case rf_pixel_format_dxt3_rgba:     return 8;      // 8 bpp
        case rf_pixel_format_dxt5_rgba:     return 8;      // 8 bpp
        case rf_pixel_format_etc1_rgb:      return 4;      // 4 bpp
        case rf_pixel_format_etc2_rgb:      return 4;      // 4 bpp
        case rf_pixel_format_etc2_eac_rgba: return 8;      // 8 bpp
        case rf_pixel_format_pvrt_rgb:      return 4;      // 4 bpp
        case rf_pixel_format_prvt_rgba:     return 4;      // 4 bpp
        case rf_pixel_format_astc_4x4_rgba: return 8;      // 8 bpp
        case rf_pixel_format_astc_8x8_rgba: return 2;      // 2 bpp
        default: return 0;
    }
}

rf_extern rf_int rf_bytes_per_pixel(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale:    return 1;
        case rf_pixel_format_gray_alpha:   return 2;
        case rf_pixel_format_r5g5b5a1:     return 2;
        case rf_pixel_format_r5g6b5:       return 2;
        case rf_pixel_format_r4g4b4a4:     return 2;
        case rf_pixel_format_r8g8b8a8:     return 4;
        case rf_pixel_format_r8g8b8:       return 3;
        case rf_pixel_format_r32:          return 4;
        case rf_pixel_format_r32g32b32:    return 12;
        case rf_pixel_format_r32g32b32a32: return 16;
        default:                           return 0;
    }
}

rf_extern rf_int rf_image_size_in_format(rf_int width, rf_int height, rf_pixel_format format)
{
    rf_int result = width * height * rf_bits_per_pixel(format) / 8;
    return result;
}

rf_extern void rf_pixel_buffer_free(rf_pixel_buffer it, rf_allocator allocator)
{
    rf_free(allocator, it.data);
}

rf_extern rf_pixel_buffer rf_format_pixels_to_normalized(rf_pixel_buffer src, rf_allocator allocator)
{
    rf_pixel_format dst_format = rf_pixel_format_r32g32b32a32;
    rf_int src_pixel_count = src.size / rf_bytes_per_pixel(src.format);
    rf_int dst_size = src_pixel_count * rf_bytes_per_pixel(dst_format);
    rf_vec4* dst = rf_alloc(allocator, dst_size);
    rf_pixel_buffer result = rf_format_pixels_to_normalized_to_buffer(src, dst, dst_size);

    return result;
}

rf_extern rf_pixel_buffer rf_format_pixels_to_rgba32(rf_pixel_buffer src, rf_allocator allocator)
{
    rf_pixel_format dst_format = rf_pixel_format_r8g8b8a8;
    rf_int src_pixel_count = src.size / rf_bytes_per_pixel(src.format);
    rf_int dst_size = src_pixel_count * rf_bytes_per_pixel(dst_format);
    rf_color* dst = rf_alloc(allocator, dst_size);
    rf_pixel_buffer result = rf_format_pixels_to_rgba32_to_buffer(src, dst, dst_size);
    
    return result;
}

rf_extern rf_pixel_buffer rf_format_pixels(rf_pixel_buffer src, rf_pixel_format dst_format, rf_allocator allocator)
{
    rf_int src_pixel_count = src.size / rf_bytes_per_pixel(src.format);
    rf_int dst_size = src_pixel_count * rf_bytes_per_pixel(dst_format);
    rf_vec4* dst = rf_alloc(allocator, dst_size);
    rf_pixel_buffer result = rf_format_pixels_to_buffer(src, dst, dst_size, dst_format);
    
    return result;
}

rf_extern rf_pixel_buffer rf_format_pixels_to_normalized_to_buffer(rf_pixel_buffer src, rf_vec4* dst, rf_int dst_size_in_bytes)
{
    if (!src.valid)
    {
        rf_log_error(rf_bad_argument, "Source pixel buffer is invalid.");
        return (rf_pixel_buffer) {0};
    }

    rf_pixel_buffer result = {0};

    rf_int src_bpp = rf_bytes_per_pixel(src.format);
    rf_int src_pixel_count = src.size / src_bpp;
    rf_int dst_pixel_count = dst_size_in_bytes / sizeof(rf_vec4);

    if (dst_pixel_count >= src_pixel_count)
    {
        if (src.format == rf_pixel_format_r32g32b32a32)
        {
            memcpy(dst, src.data, src.size);
        }
        else
        {
            #define rf_for_each_pixel_ for (rf_int dst_iter = 0, src_iter = 0; src_iter < src.size && dst_iter < dst_size_in_bytes; dst_iter++, src_iter += src_bpp)
            switch (src.format)
            {
                case rf_pixel_format_grayscale:
                    rf_for_each_pixel_
                    {
                        float value = ((unsigned char*)src.data)[src_iter] / 255.0f;

                        dst[dst_iter].x = value;
                        dst[dst_iter].y = value;
                        dst[dst_iter].z = value;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_gray_alpha:
                    rf_for_each_pixel_
                    {
                        float value0 = (float)((unsigned char*)src.data)[src_iter + 0] / 255.0f;
                        float value1 = (float)((unsigned char*)src.data)[src_iter + 1] / 255.0f;

                        dst[dst_iter].x = value0;
                        dst[dst_iter].y = value0;
                        dst[dst_iter].z = value0;
                        dst[dst_iter].w = value1;
                    }
                    break;

                case rf_pixel_format_r5g5b5a1:
                    rf_for_each_pixel_
                    {
                        unsigned short pixel = ((unsigned short*) src.data)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f/31);
                        dst[dst_iter].y = (float)((pixel & 0b0000011111000000) >>  6) * (1.0f/31);
                        dst[dst_iter].z = (float)((pixel & 0b0000000000111110) >>  1) * (1.0f/31);
                        dst[dst_iter].w = ((pixel & 0b0000000000000001) == 0) ? 0.0f : 1.0f;
                    }
                    break;

                case rf_pixel_format_r5g6b5:
                    rf_for_each_pixel_
                    {
                        unsigned short pixel = ((unsigned short*)src.data)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f / 31);
                        dst[dst_iter].y = (float)((pixel & 0b0000011111100000) >>  5) * (1.0f / 63);
                        dst[dst_iter].z = (float) (pixel & 0b0000000000011111)        * (1.0f / 31);
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r4g4b4a4:
                    rf_for_each_pixel_
                    {
                        unsigned short pixel = ((unsigned short*)src.data)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111000000000000) >> 12) * (1.0f / 15);
                        dst[dst_iter].y = (float)((pixel & 0b0000111100000000) >> 8)  * (1.0f / 15);
                        dst[dst_iter].z = (float)((pixel & 0b0000000011110000) >> 4)  * (1.0f / 15);
                        dst[dst_iter].w = (float) (pixel & 0b0000000000001111)        * (1.0f / 15);
                    }
                    break;

                case rf_pixel_format_r8g8b8a8:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].x = (float)((unsigned char*)src.data)[src_iter + 0] / 255.0f;
                        dst[dst_iter].y = (float)((unsigned char*)src.data)[src_iter + 1] / 255.0f;
                        dst[dst_iter].z = (float)((unsigned char*)src.data)[src_iter + 2] / 255.0f;
                        dst[dst_iter].w = (float)((unsigned char*)src.data)[src_iter + 3] / 255.0f;
                    }
                    break;

                case rf_pixel_format_r8g8b8:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].x = (float)((unsigned char*)src.data)[src_iter + 0] / 255.0f;
                        dst[dst_iter].y = (float)((unsigned char*)src.data)[src_iter + 1] / 255.0f;
                        dst[dst_iter].z = (float)((unsigned char*)src.data)[src_iter + 2] / 255.0f;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r32:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].x = ((float*)src.data)[src_iter];
                        dst[dst_iter].y = 0.0f;
                        dst[dst_iter].z = 0.0f;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r32g32b32:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].x = ((float*)src.data)[src_iter + 0];
                        dst[dst_iter].y = ((float*)src.data)[src_iter + 1];
                        dst[dst_iter].z = ((float*)src.data)[src_iter + 2];
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r32g32b32a32:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].x = ((float*)src.data)[src_iter + 0];
                        dst[dst_iter].y = ((float*)src.data)[src_iter + 1];
                        dst[dst_iter].z = ((float*)src.data)[src_iter + 2];
                        dst[dst_iter].w = ((float*)src.data)[src_iter + 3];
                    }
                    break;

                default: break;
            }
            #undef rf_for_each_pixel_
        }

        result = (rf_pixel_buffer) {
            .size = dst_size_in_bytes,
            .data = dst,
            .format = rf_pixel_format_r32g32b32a32,
            .valid = rf_valid,
        };
    }
    else rf_log_error(rf_bad_buffer_size, "Buffer is size %d but function expected a size of at least %d.", dst_size_in_bytes, src_pixel_count * sizeof(rf_vec4));

    return result;
}

rf_extern rf_pixel_buffer rf_format_pixels_to_rgba32_to_buffer(rf_pixel_buffer src, rf_color* dst, rf_int dst_size_in_bytes)
{
    if (!src.valid)
    {
        rf_log_error(rf_bad_argument, "Source pixel buffer is invalid.");
        return (rf_pixel_buffer) {0};
    }


    rf_pixel_buffer result = {0};

    rf_int src_bpp = rf_bytes_per_pixel(src.format);
    rf_int src_pixel_count = src.size / src_bpp;
    rf_int dst_pixel_count = dst_size_in_bytes / sizeof(rf_color);

    if (dst_pixel_count >= src_pixel_count)
    {
        if (src.format == rf_pixel_format_r8g8b8a8)
        {
            memcpy(dst, src.data, src.size);
        }
        else
        {
            #define rf_for_each_pixel_ for (rf_int dst_iter = 0, src_iter = 0; src_iter < src.size && dst_iter < dst_size_in_bytes; dst_iter++, src_iter += src_bpp)
            switch (src.format)
            {
                case rf_pixel_format_grayscale:
                    rf_for_each_pixel_
                    {
                        unsigned char value = ((unsigned char*) src.data)[src_iter];
                        dst[dst_iter].r = value;
                        dst[dst_iter].g = value;
                        dst[dst_iter].b = value;
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_gray_alpha:
                    rf_for_each_pixel_
                    {
                        unsigned char value0 = ((unsigned char*) src.data)[src_iter + 0];
                        unsigned char value1 = ((unsigned char*) src.data)[src_iter + 1];

                        dst[dst_iter].r = value0;
                        dst[dst_iter].g = value0;
                        dst[dst_iter].b = value0;
                        dst[dst_iter].a = value1;
                    }
                    break;

                case rf_pixel_format_r5g5b5a1:
                    rf_for_each_pixel_
                    {
                        unsigned short pixel = ((unsigned short*) src.data)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000011111000000) >>  6) * (255 / 31));
                        dst[dst_iter].b = (unsigned char)((float)((pixel & 0b0000000000111110) >>  1) * (255 / 31));
                        dst[dst_iter].a = (unsigned char)        ((pixel & 0b0000000000000001)        *  255);
                    }
                    break;

                case rf_pixel_format_r5g6b5:
                    rf_for_each_pixel_
                    {
                        unsigned short pixel = ((unsigned short*) src.data)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)* (255 / 31));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000011111100000) >>  5)* (255 / 63));
                        dst[dst_iter].b = (unsigned char)((float) (pixel & 0b0000000000011111)       * (255 / 31));
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r4g4b4a4:
                    rf_for_each_pixel_
                    {
                        unsigned short pixel = ((unsigned short*) src.data)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)  * (255 / 15));
                        dst[dst_iter].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)  * (255 / 15));
                        dst[dst_iter].a = (unsigned char)((float) (pixel & 0b0000000000001111)        * (255 / 15));
                    }
                    break;

                case rf_pixel_format_r8g8b8a8:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].r = ((unsigned char*) src.data)[src_iter + 0];
                        dst[dst_iter].g = ((unsigned char*) src.data)[src_iter + 1];
                        dst[dst_iter].b = ((unsigned char*) src.data)[src_iter + 2];
                        dst[dst_iter].a = ((unsigned char*) src.data)[src_iter + 3];
                    }
                    break;

                case rf_pixel_format_r8g8b8:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].r = (unsigned char)((unsigned char*) src.data)[src_iter + 0];
                        dst[dst_iter].g = (unsigned char)((unsigned char*) src.data)[src_iter + 1];
                        dst[dst_iter].b = (unsigned char)((unsigned char*) src.data)[src_iter + 2];
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r32:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src.data)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = 0;
                        dst[dst_iter].b = 0;
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r32g32b32:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src.data)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = (unsigned char)(((float*) src.data)[src_iter + 1] * 255.0f);
                        dst[dst_iter].b = (unsigned char)(((float*) src.data)[src_iter + 2] * 255.0f);
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r32g32b32a32:
                    rf_for_each_pixel_
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src.data)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = (unsigned char)(((float*) src.data)[src_iter + 1] * 255.0f);
                        dst[dst_iter].b = (unsigned char)(((float*) src.data)[src_iter + 2] * 255.0f);
                        dst[dst_iter].a = (unsigned char)(((float*) src.data)[src_iter + 3] * 255.0f);
                    }
                    break;

                default: break;
            }
            #undef rf_for_each_pixel_
        }

        result = (rf_pixel_buffer) {
            .size = dst_size_in_bytes,
            .data = dst,
            .format = rf_pixel_format_r32g32b32a32,
            .valid = rf_valid,
        };
    }
    else rf_log_error(rf_bad_buffer_size, "Buffer is size %d but function expected a size of at least %d", dst_size_in_bytes, src_pixel_count * sizeof(rf_color));

    return result;
}

rf_extern rf_pixel_buffer rf_format_pixels_to_buffer(rf_pixel_buffer src, void* dst, rf_int dst_size, rf_pixel_format dst_format)
{
    if (!src.valid)
    {
        rf_log_error(rf_bad_argument, "Source pixel buffer is invalid.");
        return (rf_pixel_buffer) {0};
    }

    rf_pixel_buffer result = {0};

    if (rf_is_uncompressed_format(src.format) && dst_format == rf_pixel_format_r32g32b32a32)
    {
        result = rf_format_pixels_to_normalized_to_buffer(src, dst, dst_size);
    }
    else if (rf_is_uncompressed_format(src.format) && dst_format == rf_pixel_format_r8g8b8a8)
    {
        result = rf_format_pixels_to_rgba32_to_buffer(src, dst, dst_size);
    }
    else if (rf_is_uncompressed_format(src.format) && rf_is_uncompressed_format(dst_format))
    {
        rf_int src_bpp = rf_bytes_per_pixel(src.format);
        rf_int dst_bpp = rf_bytes_per_pixel(dst_format);

        rf_int src_pixel_count = src.size / src_bpp;
        rf_int dst_pixel_count = dst_size / dst_bpp;

        if (dst_pixel_count >= src_pixel_count)
        {
            // Loop over both src and dst
            #define rf_for_each_pixel_ for (rf_int src_iter = 0, dst_iter = 0; src_iter < src.size && dst_iter < dst_size; src_iter += src_bpp, dst_iter += dst_bpp)
            #define rf_compute_normalized_pixel_() rf_format_one_pixel_to_normalized(((unsigned char*) src.data) + src_iter, src.format);
            if (src.format == dst_format)
            {
                memcpy(dst, src.data, src.size);
            }
            else
            {
                switch (dst_format)
                {
                    case rf_pixel_format_grayscale:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();
                            ((unsigned char*)dst)[dst_iter] = (unsigned char)((normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f) * 255.0f);
                        }
                        break;

                    case rf_pixel_format_gray_alpha:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            ((unsigned char*)dst)[dst_iter    ] = (unsigned char)((normalized.x * 0.299f + (float)normalized.y * 0.587f + (float)normalized.z * 0.114f) * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 1] = (unsigned char) (normalized.w * 255.0f);
                        }
                        break;

                    case rf_pixel_format_r5g6b5:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 63.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 31.0f));

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
                        }
                        break;

                    case rf_pixel_format_r8g8b8:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            ((unsigned char*)dst)[dst_iter    ] = (unsigned char)(normalized.x * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 1] = (unsigned char)(normalized.y * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 2] = (unsigned char)(normalized.z * 255.0f);
                        }
                        break;

                    case rf_pixel_format_r5g5b5a1:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            int ALPHA_THRESHOLD = 50;
                            unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 31.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 31.0f));
                            unsigned char a = (normalized.w > ((float)ALPHA_THRESHOLD / 255.0f)) ? 1 : 0;

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
                        }
                        break;

                    case rf_pixel_format_r4g4b4a4:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            unsigned char r = (unsigned char)(round(normalized.x * 15.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 15.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 15.0f));
                            unsigned char a = (unsigned char)(round(normalized.w * 15.0f));

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
                        }
                        break;

                    case rf_pixel_format_r32:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            ((float*)dst)[dst_iter] = (float)(normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f);
                        }
                        break;

                    case rf_pixel_format_r32g32b32:
                        rf_for_each_pixel_
                        {
                            rf_vec4 normalized = rf_compute_normalized_pixel_();

                            ((float*)dst)[dst_iter    ] = normalized.x;
                            ((float*)dst)[dst_iter + 1] = normalized.y;
                            ((float*)dst)[dst_iter + 2] = normalized.z;
                        }
                        break;

                    default: break;
                }
            }
            #undef rf_for_each_pixel_
            #undef rf_compute_normalized_pixel_

            result = (rf_pixel_buffer) {
                .size = dst_size,
                .data = dst,
                .format = rf_pixel_format_r32g32b32a32,
                .valid = rf_valid,
            };
        }
        else rf_log_error(rf_bad_buffer_size, "Buffer is size %d but function expected a size of at least %d.", dst_size, src_pixel_count * dst_bpp);
    }
    else rf_log_error(rf_bad_argument, "Function expected uncompressed pixel formats. Source format: %d, Destination format: %d.", src.format, dst_format);

    return result;
}

rf_extern rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_pixel_format src_format)
{
    rf_vec4 result = {0};

    switch (src_format)
    {
        case rf_pixel_format_grayscale:
        {
            float value = ((unsigned char*)src)[0] / 255.0f;

            result.x = value;
            result.y = value;
            result.z = value;
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_gray_alpha:
        {
            float value0 = (float)((unsigned char*)src)[0] / 255.0f;
            float value1 = (float)((unsigned char*)src)[1] / 255.0f;

            result.x = value0;
            result.y = value0;
            result.z = value0;
            result.w = value1;
        }
        break;

        case rf_pixel_format_r5g5b5a1:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f/31);
            result.y = (float)((pixel & 0b0000011111000000) >>  6) * (1.0f/31);
            result.z = (float)((pixel & 0b0000000000111110) >>  1) * (1.0f/31);
            result.w = ((pixel & 0b0000000000000001) == 0) ? 0.0f : 1.0f;
        }
        break;

        case rf_pixel_format_r5g6b5:
        {
            unsigned short pixel = ((unsigned short*)src)[0];

            result.x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f / 31);
            result.y = (float)((pixel & 0b0000011111100000) >>  5) * (1.0f / 63);
            result.z = (float) (pixel & 0b0000000000011111)        * (1.0f / 31);
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r4g4b4a4:
        {
            unsigned short pixel = ((unsigned short*)src)[0];

            result.x = (float)((pixel & 0b1111000000000000) >> 12) * (1.0f / 15);
            result.y = (float)((pixel & 0b0000111100000000) >> 8)  * (1.0f / 15);
            result.z = (float)((pixel & 0b0000000011110000) >> 4)  * (1.0f / 15);
            result.w = (float) (pixel & 0b0000000000001111)        * (1.0f / 15);
        }
        break;

        case rf_pixel_format_r8g8b8a8:
        {
            result.x = (float)((unsigned char*)src)[0] / 255.0f;
            result.y = (float)((unsigned char*)src)[1] / 255.0f;
            result.z = (float)((unsigned char*)src)[2] / 255.0f;
            result.w = (float)((unsigned char*)src)[3] / 255.0f;
        }
        break;

        case rf_pixel_format_r8g8b8:
        {
            result.x = (float)((unsigned char*)src)[0] / 255.0f;
            result.y = (float)((unsigned char*)src)[1] / 255.0f;
            result.z = (float)((unsigned char*)src)[2] / 255.0f;
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r32:
        {
            result.x = ((float*)src)[0];
            result.y = 0.0f;
            result.z = 0.0f;
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r32g32b32:
        {
            result.x = ((float*)src)[0];
            result.y = ((float*)src)[1];
            result.z = ((float*)src)[2];
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r32g32b32a32:
        {
            result.x = ((float*)src)[0];
            result.y = ((float*)src)[1];
            result.z = ((float*)src)[2];
            result.w = ((float*)src)[3];
        }
        break;

        default: break;
    }

    return result;
}

rf_extern rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_pixel_format src_format)
{
    rf_color result = {0};

    switch (src_format)
    {
        case rf_pixel_format_grayscale:
        {
            unsigned char value = ((unsigned char*) src)[0];
            result.r = value;
            result.g = value;
            result.b = value;
            result.a = 255;
        }
        break;

        case rf_pixel_format_gray_alpha:
        {
            unsigned char value0 = ((unsigned char*) src)[0];
            unsigned char value1 = ((unsigned char*) src)[1];

            result.r = value0;
            result.g = value0;
            result.b = value0;
            result.a = value1;
        }
        break;

        case rf_pixel_format_r5g5b5a1:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
            result.g = (unsigned char)((float)((pixel & 0b0000011111000000) >>  6) * (255 / 31));
            result.b = (unsigned char)((float)((pixel & 0b0000000000111110) >>  1) * (255 / 31));
            result.a = (unsigned char)        ((pixel & 0b0000000000000001)        *  255);
        }
        break;

        case rf_pixel_format_r5g6b5:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)* (255 / 31));
            result.g = (unsigned char)((float)((pixel & 0b0000011111100000) >>  5)* (255 / 63));
            result.b = (unsigned char)((float) (pixel & 0b0000000000011111)       * (255 / 31));
            result.a = 255;
        }
        break;

        case rf_pixel_format_r4g4b4a4:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
            result.g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)  * (255 / 15));
            result.b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)  * (255 / 15));
            result.a = (unsigned char)((float) (pixel & 0b0000000000001111)        * (255 / 15));
        }
        break;

        case rf_pixel_format_r8g8b8a8:
        {
            result.r = ((unsigned char*) src)[0];
            result.g = ((unsigned char*) src)[1];
            result.b = ((unsigned char*) src)[2];
            result.a = ((unsigned char*) src)[3];
        }
        break;

        case rf_pixel_format_r8g8b8:
        {
            result.r = (unsigned char)((unsigned char*) src)[0];
            result.g = (unsigned char)((unsigned char*) src)[1];
            result.b = (unsigned char)((unsigned char*) src)[2];
            result.a = 255;
        }
        break;

        case rf_pixel_format_r32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = 0;
            result.b = 0;
            result.a = 255;
        }
        break;

        case rf_pixel_format_r32g32b32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = (unsigned char)(((float*) src)[1] * 255.0f);
            result.b = (unsigned char)(((float*) src)[2] * 255.0f);
            result.a = 255;
        }
        break;

        case rf_pixel_format_r32g32b32a32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = (unsigned char)(((float*) src)[1] * 255.0f);
            result.b = (unsigned char)(((float*) src)[2] * 255.0f);
            result.a = (unsigned char)(((float*) src)[3] * 255.0f);
        }
        break;

        default: break;
    }

    return result;
}

rf_extern void rf_format_one_pixel(const void* src, rf_pixel_format src_format, void* dst, rf_pixel_format dst_format)
{
    if (src_format == dst_format && rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        memcpy(dst, src, rf_bytes_per_pixel(src_format));
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == rf_pixel_format_r32g32b32a32)
    {
        *((rf_vec4*)dst) = rf_format_one_pixel_to_normalized(src, src_format);
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == rf_pixel_format_r8g8b8a8)
    {
        *((rf_color*)dst) = rf_format_one_pixel_to_rgba32(src, src_format);
    }
    else if (rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        switch (dst_format)
        {
            case rf_pixel_format_grayscale:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0] = (unsigned char)((normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f) * 255.0f);
            }
            break;

            case rf_pixel_format_gray_alpha:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)((normalized.x * 0.299f + (float)normalized.y * 0.587f + (float)normalized.z * 0.114f) * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char) (normalized.w * 255.0f);
            }
            break;

            case rf_pixel_format_r5g6b5:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 63.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 31.0f));

                ((unsigned short*)dst)[0] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
            }
            break;

            case rf_pixel_format_r8g8b8:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)(normalized.x * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char)(normalized.y * 255.0f);
                ((unsigned char*)dst)[0 + 2] = (unsigned char)(normalized.z * 255.0f);
            }
            break;

            case rf_pixel_format_r5g5b5a1:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                int ALPHA_THRESHOLD = 50;
                unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 31.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 31.0f));
                unsigned char a = (normalized.w > ((float)ALPHA_THRESHOLD / 255.0f)) ? 1 : 0;

                ((unsigned short*)dst)[0] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
            }
            break;

            case rf_pixel_format_r4g4b4a4:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                unsigned char r = (unsigned char)(round(normalized.x * 15.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 15.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 15.0f));
                unsigned char a = (unsigned char)(round(normalized.w * 15.0f));

                ((unsigned short*)dst)[0] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
            }
            break;

            case rf_pixel_format_r8g8b8a8:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)(normalized.x * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char)(normalized.y * 255.0f);
                ((unsigned char*)dst)[0 + 2] = (unsigned char)(normalized.z * 255.0f);
                ((unsigned char*)dst)[0 + 3] = (unsigned char)(normalized.w * 255.0f);
            }
            break;

            case rf_pixel_format_r32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0] = (float)(normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f);
            }
            break;

            case rf_pixel_format_r32g32b32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0    ] = normalized.x;
                ((float*)dst)[0 + 1] = normalized.y;
                ((float*)dst)[0 + 2] = normalized.z;
            }
            break;

            case rf_pixel_format_r32g32b32a32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0    ] = normalized.x;
                ((float*)dst)[0 + 1] = normalized.y;
                ((float*)dst)[0 + 2] = normalized.z;
                ((float*)dst)[0 + 3] = normalized.w;
            }
            break;

            default: break;
        }
    }
}