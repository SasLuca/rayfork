#ifndef RAYFORK_IMAGE_H
#define RAYFORK_IMAGE_H

#include "rayfork_common.h"
#include "rayfork_allocator.h"
#include "rayfork_io.h"
#include "rayfork_color.h"

typedef enum rf_desired_channels
{
    RF_ANY_CHANNELS = 0,
    RF_1BYTE_GRAYSCALE = 1,
    RF_2BYTE_GRAY_ALPHA = 2,
    RF_3BYTE_R8G8B8 = 3,
    RF_4BYTE_R8G8B8A8 = 4,
} rf_desired_channels;

typedef struct rf_image
{
    void*           data;    // image raw data
    int             width;   // image base width
    int             height;  // image base height
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    bool            valid;   // True if the image is valid and can be used
} rf_image;

typedef struct rf_mipmaps_stats
{
    int possible_mip_counts;
    int mipmaps_buffer_size;
} rf_mipmaps_stats;

typedef struct rf_mipmaps_image
{
    union
    {
        rf_image image;
        struct
        {
            void*           data;    // image raw data
            int             width;   // image base width
            int             height;  // image base height
            rf_pixel_format format;  // Data format (rf_pixel_format type)
            bool            valid;
        };
    };

    int mipmaps; // Mipmap levels, 1 by default
} rf_mipmaps_image;

typedef struct rf_gif
{
    int frames_count;
    int* frame_delays;

    union
    {
        rf_image image;

        struct
        {
            void*           data;    // rf_image raw data
            int             width;   // rf_image base width
            int             height;  // rf_image base height
            rf_pixel_format format;  // Data format (rf_pixel_format type)
            bool         valid;
        };
    };
} rf_gif;

#pragma region extract image data functions
RF_API int rf_image_size(rf_image image);
RF_API int rf_image_size_in_format(rf_image image, rf_pixel_format format);

RF_API bool rf_image_get_pixels_as_rgba32_to_buffer(rf_image image, rf_color* dst, int dst_size);
RF_API bool rf_image_get_pixels_as_normalized_to_buffer(rf_image image, rf_vec4* dst, int dst_size);

RF_API rf_color* rf_image_pixels_to_rgba32(rf_image image, rf_allocator allocator);
RF_API rf_vec4* rf_image_compute_pixels_to_normalized(rf_image image, rf_allocator allocator);

RF_API void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, int palette_size);
RF_API rf_palette rf_image_extract_palette(rf_image image, int palette_size, rf_allocator allocator);
RF_API rf_rec rf_image_alpha_border(rf_image image, float threshold);
#pragma endregion

#pragma region loading & unloading functions
RF_API bool rf_supports_image_file_type(const char* filename);

RF_API rf_image rf_load_image_from_file_data_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
RF_API rf_image rf_load_image_from_file_data(const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, int src_size, void* dst, int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
RF_API rf_image rf_load_image_from_hdr_file_data(const void* src, int src_size, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_image rf_load_image_from_format_to_buffer(const void* src, int src_size, int src_width, int src_height, rf_uncompressed_pixel_format src_format, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format);
RF_API rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);

RF_API void rf_unload_image(rf_image image, rf_allocator allocator);
#pragma endregion

#pragma region mipmaps
RF_API int rf_mipmaps_image_size(rf_mipmaps_image image);
RF_API rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count);
RF_API rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, int dst_size, rf_allocator temp_allocator);  // Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
RF_API rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator);
RF_API void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator);
#pragma endregion

#pragma region dds
RF_API int rf_get_dds_image_size(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_dds_image_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_mipmaps_image rf_load_dds_image(const void* src, int src_size, rf_allocator allocator);
RF_API rf_mipmaps_image rf_load_dds_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region pkm
RF_API int rf_get_pkm_image_size(const void* src, int src_size);
RF_API rf_image rf_load_pkm_image_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_image rf_load_pkm_image(const void* src, int src_size, rf_allocator allocator);
RF_API rf_image rf_load_pkm_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region ktx
RF_API int rf_get_ktx_image_size(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, int src_size, void* dst, int dst_size);
RF_API rf_mipmaps_image rf_load_ktx_image(const void* src, int src_size, rf_allocator allocator);
RF_API rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region gif
RF_API rf_gif rf_load_animated_gif(const void* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
RF_API rf_sizei rf_gif_frame_size(rf_gif gif);
RF_API rf_image rf_get_frame_from_gif(rf_gif gif, int frame);
RF_API void rf_unload_gif(rf_gif gif, rf_allocator allocator);
#pragma endregion

#endif // RAYFORK_IMAGE_H