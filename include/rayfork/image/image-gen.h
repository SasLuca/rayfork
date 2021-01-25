#ifndef RAYFORK_IMAGE_GEN_H
#define RAYFORK_IMAGE_GEN_H

#include "rayfork/image/image.h"
#include "rayfork/foundation/rand.h"

rf_extern rf_vec2 rf_get_seed_for_cellular_image(int seeds_per_row, int tile_size, int i, rf_rand_proc rand);

rf_extern rf_image rf_gen_image_color_to_buffer(int width, int height, rf_color color, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator);
rf_extern rf_image rf_gen_image_gradient_v_to_buffer(int width, int height, rf_color top, rf_color bottom, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator);
rf_extern rf_image rf_gen_image_gradient_h_to_buffer(int width, int height, rf_color left, rf_color right, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator);
rf_extern rf_image rf_gen_image_gradient_radial_to_buffer(int width, int height, float density, rf_color inner, rf_color outer, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator);
rf_extern rf_image rf_gen_image_checked_to_buffer(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator);
rf_extern rf_image rf_gen_image_white_noise_to_buffer(int width, int height, float factor, rf_rand_proc rand, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_rand_proc rand, rf_allocator allocator);
rf_extern rf_image rf_gen_image_perlin_noise_to_buffer(int width, int height, int offset_x, int offset_y, float scale, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator);
rf_extern rf_image rf_gen_image_cellular_to_buffer(int width, int height, int tile_size, rf_rand_proc rand, rf_color* dst, rf_int dst_size);
rf_extern rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_rand_proc rand, rf_allocator allocator);

#endif // RAYFORK_IMAGE_GEN_H