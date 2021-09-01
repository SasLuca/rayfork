#include "rayfork/foundation/logger.h"
#include "rayfork/image/dds-loading.h"

/*
 Required extension:
 GL_EXT_texture_compression_s3tc

 Supported tokens (defined by extensions)
 GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
 GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
 GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
 GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3
*/

#define rf_fourcc_dxt1 (0x31545844)  // Equivalent to "DXT1" in ASCII
#define rf_fourcc_dxt3 (0x33545844)  // Equivalent to "DXT3" in ASCII
#define rf_fourcc_dxt5 (0x35545844)  // Equivalent to "DXT5" in ASCII

typedef struct rf_dds_pixel_format rf_dds_pixel_format;
struct rf_dds_pixel_format
{
    uint32_t size;
    uint32_t flags;
    uint32_t four_cc;
    uint32_t rgb_bit_count;
    uint32_t r_bit_mask;
    uint32_t g_bit_mask;
    uint32_t b_bit_mask;
    uint32_t a_bit_mask;
};

// DDS Header (must be 124 bytes)
typedef struct rf_dds_header rf_dds_header;
struct rf_dds_header
{
    char     id[4];
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitch_or_linear_size;
    uint32_t depth;
    uint32_t mipmap_count;
    uint32_t reserved_1[11];
    rf_dds_pixel_format ddspf;
    uint32_t caps;
    uint32_t caps_2;
    uint32_t caps_3;
    uint32_t caps_4;
    uint32_t reserved_2;
};

rf_extern rf_int rf_get_dds_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size >= sizeof(rf_dds_header))
    {
        rf_dds_header header = *(rf_dds_header*)src;

        // Verify the type of file
        if (rf_str_match((rf_str) { header.id, sizeof(header.id) }, rf_cstr("DDS ")))
        {
            if (header.ddspf.rgb_bit_count == 16) // 16bit mode, no compressed
            {
                if (header.ddspf.flags == 0x40) // no alpha channel
                {
                    result = header.width * header.height * sizeof(unsigned short);
                }
                else if (header.ddspf.flags == 0x41) // with alpha channel
                {
                    if (header.ddspf.a_bit_mask == 0x8000) // 1bit alpha
                    {
                        result = header.width * header.height * sizeof(unsigned short);
                    }
                    else if (header.ddspf.a_bit_mask == 0xf000) // 4bit alpha
                    {
                        result = header.width * header.height * sizeof(unsigned short);
                    }
                }
            }
            else if (header.ddspf.flags == 0x40 && header.ddspf.rgb_bit_count == 24) // DDS_RGB, no compressed
            {
                // Not sure if this case exists...
                result = header.width * header.height * 3 * sizeof(unsigned char);
            }
            else if (header.ddspf.flags == 0x41 && header.ddspf.rgb_bit_count == 32) // DDS_RGBA, no compressed
            {
                result = header.width * header.height * 4 * sizeof(unsigned char);
            }
            else if (((header.ddspf.flags == 0x04) || (header.ddspf.flags == 0x05)) && (header.ddspf.four_cc > 0)) // Compressed
            {
                int size;       // DDS result data size

                // Calculate data size, including all mipmaps
                if (header.mipmap_count > 1) size = header.pitch_or_linear_size * 2;
                else size = header.pitch_or_linear_size;

                result = size * sizeof(unsigned char);
            }
        }
    }

    return result;
}

rf_extern rf_mipmaps_image rf_load_dds_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_mipmaps_image result = { 0 };

    if (src && dst && dst_size > 0 && src_size >= sizeof(rf_dds_header))
    {
        rf_dds_header header = *(rf_dds_header*)src;

        src = ((char*)src) + sizeof(rf_dds_header);
        src_size -= sizeof(rf_dds_header);

        // Verify the type of file
        if (rf_str_match((rf_str){header.id, sizeof(header.id)}, rf_cstr("DDS ")))
        {
            result.width   = header.width;
            result.height  = header.height;
            result.mipmaps = (header.mipmap_count == 0) ? 1 : header.mipmap_count;

            if (header.ddspf.rgb_bit_count == 16) // 16bit mode, no compressed
            {
                if (header.ddspf.flags == 0x40) // no alpha channel
                {
                    int dds_result_size = header.width * header.height * sizeof(unsigned short);

                    if (src_size >= dds_result_size && dst_size >= dds_result_size)
                    {
                        memcpy(dst, src, dds_result_size);
                        result.format = rf_pixel_format_r5g6b5;
                        result.data   = dst;
                        result.valid  = 1;
                    }
                }
                else if (header.ddspf.flags == 0x41) // with alpha channel
                {
                    if (header.ddspf.a_bit_mask == 0x8000) // 1bit alpha
                    {
                        int dds_result_size = header.width * header.height * sizeof(unsigned short);

                        if (src_size >= dds_result_size && dst_size >= dds_result_size)
                        {
                            memcpy(dst, src, dds_result_size);

                            unsigned char alpha = 0;

                            // Data comes as A1R5G5B5, it must be reordered to R5G5B5A1
                            for (rf_int i = 0; i < result.width * result.height; i++)
                            {
                                alpha = ((unsigned short *)result.data)[i] >> 15;
                                ((unsigned short*)result.data)[i] = ((unsigned short *)result.data)[i] << 1;
                                ((unsigned short*)result.data)[i] += alpha;
                            }

                            result.format = rf_pixel_format_r5g5b5a1;
                            result.data   = dst;
                            result.valid  = 1;
                        }
                    }
                    else if (header.ddspf.a_bit_mask == 0xf000) // 4bit alpha
                    {
                        int dds_result_size = header.width * header.height * sizeof(unsigned short);

                        if (src_size >= dds_result_size && dst_size >= dds_result_size)
                        {
                            memcpy(dst, src, dds_result_size);

                            unsigned char alpha = 0;

                            // Data comes as A4R4G4B4, it must be reordered R4G4B4A4
                            for (rf_int i = 0; i < result.width * result.height; i++)
                            {
                                alpha = ((unsigned short*)result.data)[i] >> 12;
                                ((unsigned short*)result.data)[i] = ((unsigned short*)result.data)[i] << 4;
                                ((unsigned short*)result.data)[i] += alpha;
                            }

                            result.format = rf_pixel_format_r4g4b4a4;
                            result.data   = dst;
                            result.valid  = 1;
                        }
                    }
                }
            }
            else if (header.ddspf.flags == 0x40 && header.ddspf.rgb_bit_count == 24) // DDS_RGB, no compressed, not sure if this case exists...
            {
                int dds_result_size = header.width * header.height * 3;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);
                    result.format = rf_pixel_format_r8g8b8;
                    result.data   = dst;
                    result.valid  = 1;
                }
            }
            else if (header.ddspf.flags == 0x41 && header.ddspf.rgb_bit_count == 32) // DDS_RGBA, no compressed
            {
                int dds_result_size = header.width * header.height * 4;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);

                    unsigned char blue = 0;

                    // Data comes as A8R8G8B8, it must be reordered R8G8B8A8 (view next comment)
                    // DirecX understand ARGB as a 32bit DWORD but the actual memory byte alignment is BGRA
                    // So, we must realign B8G8R8A8 to R8G8B8A8
                    for (rf_int i = 0; i < header.width * header.height * 4; i += 4)
                    {
                        blue = ((unsigned char*)dst)[i];
                        ((unsigned char*)dst)[i + 0] = ((unsigned char*)dst)[i + 2];
                        ((unsigned char*)dst)[i + 2] = blue;
                    }

                    result.format = rf_pixel_format_r8g8b8a8;
                    result.data   = dst;
                    result.valid  = 1;
                }
            }
            else if (((header.ddspf.flags == 0x04) || (header.ddspf.flags == 0x05)) && (header.ddspf.four_cc > 0)) // Compressed
            {
                int dds_result_size = (header.mipmap_count > 1) ? (header.pitch_or_linear_size * 2) : header.pitch_or_linear_size;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);

                    switch (header.ddspf.four_cc)
                    {
                        case rf_fourcc_dxt1: result.format = header.ddspf.flags == 0x04 ? rf_pixel_format_dxt1_rgb : rf_pixel_format_dxt1_rgba; break;
                        case rf_fourcc_dxt3: result.format = rf_pixel_format_dxt3_rgba; break;
                        case rf_fourcc_dxt5: result.format = rf_pixel_format_dxt5_rgba; break;
                        default: break;
                    }

                    result.data  = dst;
                    result.valid = 1;
                }
            }
        }
        else rf_log_error(rf_bad_format, "DDS file does not seem to be a valid result");
    }

    return result;
}

rf_extern rf_mipmaps_image rf_load_dds_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_mipmaps_image result = {0};

    int dst_size = rf_get_dds_image_size(src, src_size);
    void* dst = rf_alloc(allocator, dst_size);

    result = rf_load_dds_image_to_buffer(src, src_size, dst, dst_size);

    return result;
}

rf_extern rf_mipmaps_image rf_load_dds_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_file_contents file_contents = rf_read_entire_file(file, temp_allocator, io);
    rf_mipmaps_image result = rf_load_dds_image(file_contents.data, file_contents.size, allocator);
    rf_free(temp_allocator, file_contents.data);

    return result;
}