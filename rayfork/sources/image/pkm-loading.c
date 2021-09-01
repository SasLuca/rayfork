#include "rayfork/image/pkm-loading.h"
#include "rayfork/foundation/str.h"
#include "rayfork/foundation/logger.h"

/*
 Required extensions:
 GL_OES_compressed_ETC1_RGB8_texture  (ETC1) (OpenGL ES 2.0)
 GL_ARB_ES3_compatibility  (ETC2/EAC) (OpenGL ES 3.0)

 Supported tokens (defined by extensions)
 GL_ETC1_RGB8_OES                 0x8D64
 GL_COMPRESSED_RGB8_ETC2          0x9274
 GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

 Formats list
 version 10: format: 0=ETC1_RGB, [1=ETC1_RGBA, 2=ETC1_RGB_MIP, 3=ETC1_RGBA_MIP] (not used)
 version 20: format: 0=ETC1_RGB, 1=ETC2_RGB, 2=ETC2_RGBA_OLD, 3=ETC2_RGBA, 4=ETC2_RGBA1, 5=ETC2_R, 6=ETC2_RG, 7=ETC2_SIGNED_R, 8=ETC2_SIGNED_R

 NOTE: The extended width and height are the widths rounded up to a multiple of 4.
 NOTE: ETC is always 4bit per pixel (64 bit for each 4x4 block of pixels)
*/

// PKM file (ETC1) Header (16 bytes)
typedef struct rf_pkm_header
{
    char     id[4];       // "PKM "
    uint8_t  version[2];  // "10" or "20"
    uint16_t format;      // Data format (big-endian) (Check list below)
    uint16_t width;       // Texture width (big-endian) (origWidth rounded to multiple of 4)
    uint16_t height;      // Texture height (big-endian) (origHeight rounded to multiple of 4)
    uint16_t orig_width;  // Original width (big-endian)
    uint16_t orig_height; // Original height (big-endian)
} rf_pkm_header;

rf_extern rf_int rf_get_pkm_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size > sizeof(rf_pkm_header))
    {
        rf_pkm_header header = *(rf_pkm_header*)src;

        // Verify the type of file
        if (rf_str_match((rf_str){header.id, sizeof(header.id)}, rf_cstr("PKM ")))
        {
            // Note: format, width and height come as big-endian, data must be swapped to little-endian
            header.format = ((header.format & 0x00FF) << 8) | ((header.format & 0xFF00) >> 8);
            header.width  = ((header.width  & 0x00FF) << 8) | ((header.width  & 0xFF00) >> 8);
            header.height = ((header.height & 0x00FF) << 8) | ((header.height & 0xFF00) >> 8);

            int bpp = 4;
            if (header.format == 3) bpp = 8;

            result = header.width * header.height * bpp / 8;
        }
        else rf_log_error(rf_bad_format, "PKM file does not seem to be a valid image");
    }

    return result;
}

// Load image from .pkm
rf_extern rf_image rf_load_pkm_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (src && src_size >= sizeof(rf_pkm_header))
    {
        rf_pkm_header header = *(rf_pkm_header*)src;

        src = (char*)src + sizeof(rf_pkm_header);
        src_size -= sizeof(rf_pkm_header);

        // Verify the type of file
        if (rf_str_match((rf_str){header.id, sizeof(header.id)}, rf_cstr("PKM ")))
        {
            // Note: format, width and height come as big-endian, data must be swapped to little-endian
            result.format = ((header.format & 0x00FF) << 8) | ((header.format & 0xFF00) >> 8);
            result.width  = ((header.width  & 0x00FF) << 8) | ((header.width  & 0xFF00) >> 8);
            result.height = ((header.height & 0x00FF) << 8) | ((header.height & 0xFF00) >> 8);

            int bpp = (result.format == 3) ? 8 : 4;
            int size = result.width * result.height * bpp / 8;  // Total data size in bytes

            if (dst_size >= size && src_size >= size)
            {
                memcpy(dst, src, size);

                     if (header.format == 0) result.format = rf_pixel_format_etc1_rgb;
                else if (header.format == 1) result.format = rf_pixel_format_etc2_rgb;
                else if (header.format == 3) result.format = rf_pixel_format_etc2_eac_rgba;

                result.valid = 1;
            }
        }
        else rf_log_error(rf_bad_format, "PKM file does not seem to be a valid image");
    }

    return result;
}

rf_extern rf_image rf_load_pkm_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_image result = {0};

    if (src && src_size > 0)
    {
        int dst_size = rf_get_pkm_image_size(src, src_size);
        void* dst = rf_alloc(allocator, dst_size);

        result = rf_load_pkm_image_to_buffer(src, src_size, dst, dst_size);
    }

    return result;
}

rf_extern rf_image rf_load_pkm_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_file_contents file_contents = rf_read_entire_file(file, temp_allocator, io);
    rf_image result = rf_load_pkm_image(file_contents.data, file_contents.size, allocator);
    rf_free(temp_allocator, file_contents.data);

    return result;
}