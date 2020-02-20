#ifndef RAYFORK_IMAGE_H
#define RAYFORK_IMAGE_H

#include "rayfork_common.h"

//region extract image data functions
RF_API int       rf_image_size(rf_image image); // Returns the size of the image in bytes
RF_API rf_color* rf_get_image_pixel_data(rf_image image, rf_allocator allocator); // Get pixel data from image in the form of rf_color struct array
RF_API rf_vec4*  rf_get_image_data_normalized(rf_image image, rf_allocator allocator); // Get pixel data from image as rf_vec4 array (float normalized)
RF_API rf_color* rf_image_extract_palette(rf_image image, int max_palette_size, int* extract_count, rf_allocator allocator, rf_allocator temp_allocator); // Extract color palette from image to maximum size.
//endregion

//region loading & unloading functions
RF_API rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io); // Load image from file into CPU memory (RAM)
RF_API rf_image rf_load_image_from_data(void* data, int data_size, rf_allocator allocator); // Load image from file data into CPU memory (RAM)
RF_API rf_image rf_load_image_from_data_in_format(void* data, int data_size,  rf_allocator allocator); // Load image from file data into CPU memory (RAM)
RF_API rf_image rf_load_image_from_pixels(rf_color* pixels, int width, int height, rf_allocator allocator); // Load image from rf_color array data (RGBA - 32bit)
RF_API rf_image rf_load_image_from_data_with_params(void* data, int data_size, int width, int height, rf_pixel_format format, rf_allocator allocator); // Load image from raw data with parameters
RF_API void     rf_unload_image(rf_image); // Unloads the image using its allocator
//endregion

//region basic image manipulation
RF_API rf_image rf_image_copy(rf_image image, rf_allocator allocator); // Copy an image to a new image
RF_API rf_image rf_image_from_image(rf_image image, rf_rec rec, rf_allocator allocator, rf_allocator temp_allocator); // Create an image from another image piece

RF_API void rf_image_resize(rf_image* image, int new_width, int new_height, rf_allocator temp_allocator); // Resize and image to new size. Note: Uses stb default scaling filters (both bicubic): STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL (high-quality Catmull-Rom)
RF_API void rf_image_resize_nn(rf_image* image, int new_width, int new_height, rf_allocator temp_allocator); // Resize and image to new size using Nearest-Neighbor scaling algorithm
RF_API void rf_image_resize_canvas(rf_image* image, int new_width, int new_height, int offset_x, int offset_y, rf_color color, rf_allocator temp_allocator); // Resize canvas and fill with color. Note: Resize offset is relative to the top-left corner of the original image

RF_API void rf_image_gen_mipmaps(rf_image* image, rf_allocator temp_allocator); // Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image

RF_API void rf_image_to_pot(rf_image* image, rf_color fill_color, rf_allocator temp_allocator); // Convert image to POT (power-of-two). Note: It could be useful on OpenGL ES 2.0 (RPI, HTML5)
RF_API void rf_image_format(rf_image* image, int new_format, rf_allocator temp_allocator); // Convert image data to desired format

RF_API void rf_image_alpha_mask(rf_image* image, rf_image alpha_mask, rf_allocator temp_allocator); // Apply alpha mask to image. Note 1: Returned image is GRAY_ALPHA (16bit) or RGBA (32bit). Note 2: alphaMask should be same size as image
RF_API void rf_image_alpha_clear(rf_image* image, rf_color color, float threshold, rf_allocator temp_allocator); // Clear alpha channel to desired color. Note: Threshold defines the alpha limit, 0.0f to 1.0f
RF_API void rf_image_alpha_premultiply(rf_image* image, rf_allocator temp_allocator); // Premultiply alpha channel
RF_API void rf_image_alpha_crop(rf_image* image, float threshold, rf_allocator temp_allocator); // Crop image depending on alpha value

RF_API void rf_image_crop(rf_image* image, rf_rec crop, rf_allocator temp_allocator); // Crop an image to area defined by a rectangle
RF_API void rf_image_dither(rf_image* image, int r_bpp, int g_bpp, int b_bpp, int a_bpp); // Dither image data to 16bpp or lower (Floyd-Steinberg dithering) Note: In case selected bpp do not represent an known 16bit format, dithered data is stored in the LSB part of the unsigned short


#endif //#ifndef RAYFORK_IMAGE_H