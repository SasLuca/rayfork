#include "rayfork/image/ktx-loading.h"

/*
 Required extensions:
 GL_OES_compressed_ETC1_RGB8_texture  (ETC1)
 GL_ARB_ES3_compatibility  (ETC2/EAC)

 Supported tokens (defined by extensions)
 GL_ETC1_RGB8_OES                 0x8D64
 GL_COMPRESSED_RGB8_ETC2          0x9274
 GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

 KTX file Header (64 bytes)
 v1.1 - https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
 v2.0 - http://github.khronos.org/KTX-Specification/

 NOTE: Before start of every mipmap data block, we have: uint32_t dataSize
 TODO: Support KTX 2.2 specs!
*/

typedef struct rf_ktx_header
{
    char     id[12];                  // Identifier: "«KTX 11»\r\n\x1A\n"
    uint32_t endianness;              // Little endian: 0x01 0x02 0x03 0x04
    uint32_t gl_type;                 // For compressed textures, gl_type must equal 0
    uint32_t gl_type_size;            // For compressed texture data, usually 1
    uint32_t gl_format;               // For compressed textures is 0
    uint32_t gl_internal_format;      // Compressed internal format
    uint32_t gl_base_internal_format; // Same as gl_format (RGB, RGBA, ALPHA...)
    uint32_t width;                   // Texture image width in pixels
    uint32_t height;                  // Texture image height in pixels
    uint32_t depth;                   // For 2D textures is 0
    uint32_t elements;                // Number of array elements, usually 0
    uint32_t faces;                   // Cubemap faces, for no-cubemap = 1
    uint32_t mipmap_levels;           // Non-mipmapped textures = 1
    uint32_t key_value_data_size;     // Used to encode any arbitrary data...
} rf_ktx_header;

rf_extern rf_int rf_get_ktx_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size >= sizeof(rf_ktx_header))
    {
        rf_ktx_header header = *(rf_ktx_header*)src;
        src = (char*)src + sizeof(rf_ktx_header) + header.key_value_data_size;
        src_size -= sizeof(rf_ktx_header) + header.key_value_data_size;

        if (rf_str_match((rf_str){header.id + 1, 6}, rf_cstr("KTX 11")))
        {
            if (src_size > sizeof(uint32_t))
            {
                memcpy(&result, src, sizeof(uint32_t));
            }
        }
    }

    return result;
}

rf_extern rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_mipmaps_image result = {0};

    if (src && src_size > sizeof(rf_ktx_header))
    {
        rf_ktx_header header = *(rf_ktx_header*)src;
        src = (char*)src + sizeof(rf_ktx_header) + header.key_value_data_size;
        src_size -= sizeof(rf_ktx_header) + header.key_value_data_size;

        if (rf_str_match((rf_str){header.id + 1, 6}, rf_cstr("KTX 11")))
        {
            result.width = header.width;
            result.height = header.height;
            result.mipmaps = header.mipmap_levels;

            int image_size = 0;
            if (src_size > sizeof(uint32_t))
            {
                memcpy(&image_size, src, sizeof(uint32_t));
                src = (char*)src + sizeof(uint32_t);
                src_size -= sizeof(uint32_t);

                if (image_size >= src_size && dst_size >= image_size)
                {
                    memcpy(dst, src, image_size);
                    result.data = dst;

                    switch (header.gl_internal_format)
                    {
                        case 0x8D64: result.format = rf_pixel_format_etc1_rgb; break;
                        case 0x9274: result.format = rf_pixel_format_etc2_rgb; break;
                        case 0x9278: result.format = rf_pixel_format_etc2_eac_rgba; break;
                        default: return result;
                    }

                    result.valid = 1;
                }
            }
        }
    }

    return result;
}

rf_extern rf_mipmaps_image rf_load_ktx_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_mipmaps_image result = {0};

    if (src && src_size > 0)
    {
        int dst_size = rf_get_ktx_image_size(src, src_size);
        void* dst    = rf_alloc(allocator, dst_size);

        result = rf_load_ktx_image_to_buffer(src, src_size, dst, dst_size);
    }

    return result;
}

rf_extern rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_file_contents file_contents = rf_read_entire_file(file, temp_allocator, io);
    rf_mipmaps_image result = rf_load_ktx_image(file_contents.data, file_contents.size, allocator);
    rf_free(temp_allocator, file_contents.data);

    return result;
}