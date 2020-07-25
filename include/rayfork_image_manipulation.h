#ifndef RAYFORK_IMAGE_MANIPULATION_H
#define RAYFORK_IMAGE_MANIPULATION_H

#include "rayfork_image.h"

RF_API rf_image rf_image_copy_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_copy(rf_image image, rf_allocator allocator);

RF_API rf_image rf_image_crop_to_buffer(rf_image image, rf_rec crop, void* dst, int dst_size, rf_uncompressed_pixel_format dst_format);
RF_API rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator);

RF_API rf_image rf_image_resize_to_buffer(rf_image image, int new_width, int new_height, void* dst, int dst_size, rf_allocator temp_allocator);
RF_API rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_image rf_image_resize_nn_to_buffer(rf_image image, int new_width, int new_height, void* dst, int dst_size);
RF_API rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator allocator);

RF_API rf_image rf_image_format_to_buffer(rf_image image, rf_uncompressed_pixel_format dst_format, void* dst, int dst_size);
RF_API rf_image rf_image_format(rf_image image, rf_uncompressed_pixel_format new_format, rf_allocator allocator);

RF_API rf_image rf_image_alpha_mask_to_buffer(rf_image image, rf_image alpha_mask, void* dst, int dst_size);
RF_API rf_image rf_image_alpha_clear(rf_image image, rf_color color, float threshold, rf_allocator allocator, rf_allocator temp_allocator);
RF_API rf_image rf_image_alpha_premultiply(rf_image image, rf_allocator allocator, rf_allocator temp_allocator);

RF_API rf_rec rf_image_alpha_crop_rec(rf_image image, float threshold);
RF_API rf_image rf_image_alpha_crop(rf_image image, float threshold, rf_allocator allocator);

RF_API rf_image rf_image_dither(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator allocator, rf_allocator temp_allocator);

RF_API void rf_image_flip_vertical_in_place(rf_image* image);
RF_API rf_image rf_image_flip_vertical_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_flip_vertical(rf_image image, rf_allocator allocator);

RF_API void rf_image_flip_horizontal_in_place(rf_image* image);
RF_API rf_image rf_image_flip_horizontal_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_flip_horizontal(rf_image image, rf_allocator allocator);

RF_API rf_image rf_image_rotate_cw_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_rotate_cw(rf_image image);
RF_API rf_image rf_image_rotate_ccw_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_rotate_ccw(rf_image image);

RF_API rf_image rf_image_color_tint_to_buffer(rf_image image, rf_color color, void* dst, int dst_size);
RF_API rf_image rf_image_color_tint(rf_image image, rf_color color);
RF_API rf_image rf_image_color_invert_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_color_invert(rf_image image);
RF_API rf_image rf_image_color_grayscale_to_buffer(rf_image image, void* dst, int dst_size);
RF_API rf_image rf_image_color_grayscale(rf_image image);
RF_API rf_image rf_image_color_contrast_to_buffer(rf_image image, float contrast, void* dst, int dst_size);
RF_API rf_image rf_image_color_contrast(rf_image image, int contrast);
RF_API rf_image rf_image_color_brightness_to_buffer(rf_image image, int brightness, void* dst, int dst_size);
RF_API rf_image rf_image_color_brightness(rf_image image, int brightness);
RF_API rf_image rf_image_color_replace_to_buffer(rf_image image, rf_color color, rf_color replace, void* dst, int dst_size);
RF_API rf_image rf_image_color_replace(rf_image image, rf_color color, rf_color replace);

RF_API void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator);
RF_API void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator);
RF_API void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator);

#endif // RAYFORK_IMAGE_MANIPULATION_H