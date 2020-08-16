#ifndef RAYFORK_EZ_H
#define RAYFORK_EZ_H

#if !defined(RAYFORK_NO_EZ_API)

#include "rayfork-render-batch.h"
#include "rayfork_base64.h"
#include "rayfork-font.h"
#include "rayfork-unicode.h"
#include "rayfork-3d.h"

RF_API rf_material rf_load_default_material_ez();
RF_API rf_image rf_get_screen_data_ez();
RF_API rf_base64_output rf_decode_base64_ez(const unsigned char* input);
RF_API rf_image rf_gfx_read_texture_pixels_ez(rf_texture2d texture);

#pragma region image
#pragma region extract image data functions
RF_API rf_color* rf_image_pixels_to_rgba32_ez(rf_image image);
RF_API rf_vec4* rf_image_compute_pixels_to_normalized_ez(rf_image image);
RF_API rf_palette rf_image_extract_palette_ez(rf_image image, int palette_size);
#pragma endregion

#pragma region loading & unloading functions
RF_API rf_image rf_load_image_from_file_data_ez(const void* src, int src_size);
RF_API rf_image rf_load_image_from_hdr_file_data_ez(const void* src, int src_size);
RF_API rf_image rf_load_image_from_file_ez(const char* filename);
RF_API void rf_unload_image_ez(rf_image image);
#pragma endregion

#pragma region image manipulation
RF_API rf_image rf_image_copy_ez(rf_image image);

RF_API rf_image rf_image_crop_ez(rf_image image, rf_rec crop);

RF_API rf_image rf_image_resize_ez(rf_image image, int new_width, int new_height);
RF_API rf_image rf_image_resize_nn_ez(rf_image image, int new_width, int new_height);

RF_API rf_image rf_image_format_ez(rf_image image, rf_uncompressed_pixel_format new_format);

RF_API rf_image rf_image_alpha_clear_ez(rf_image image, rf_color color, float threshold);
RF_API rf_image rf_image_alpha_premultiply_ez(rf_image image);
RF_API rf_image rf_image_alpha_crop_ez(rf_image image, float threshold);
RF_API rf_image rf_image_dither_ez(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp);

RF_API rf_image rf_image_flip_vertical_ez(rf_image image);
RF_API rf_image rf_image_flip_horizontal_ez(rf_image image);

RF_API rf_vec2 rf_get_seed_for_cellular_image_ez(int seeds_per_row, int tile_size, int i);

RF_API rf_image rf_gen_image_color_ez(int width, int height, rf_color color);
RF_API rf_image rf_gen_image_gradient_v_ez(int width, int height, rf_color top, rf_color bottom);
RF_API rf_image rf_gen_image_gradient_h_ez(int width, int height, rf_color left, rf_color right);
RF_API rf_image rf_gen_image_gradient_radial_ez(int width, int height, float density, rf_color inner, rf_color outer);
RF_API rf_image rf_gen_image_checked_ez(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2);
RF_API rf_image rf_gen_image_white_noise_ez(int width, int height, float factor);
RF_API rf_image rf_gen_image_perlin_noise_ez(int width, int height, int offset_x, int offset_y, float scale);
RF_API rf_image rf_gen_image_cellular_ez(int width, int height, int tile_size);
#pragma endregion

#pragma region mipmaps
RF_API rf_mipmaps_image rf_image_gen_mipmaps_ez(rf_image image, int gen_mipmaps_count);
RF_API void rf_unload_mipmaps_image_ez(rf_mipmaps_image image);
#pragma endregion

#pragma region dds
RF_API rf_mipmaps_image rf_load_dds_image_ez(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_dds_image_from_file_ez(const char* file);
#pragma endregion

#pragma region pkm
RF_API rf_image rf_load_pkm_image_ez(const void* src, int src_size);
RF_API rf_image rf_load_pkm_image_from_file_ez(const char* file);
#pragma endregion

#pragma region ktx
RF_API rf_mipmaps_image rf_load_ktx_image_ez(const void* src, int src_size);
RF_API rf_mipmaps_image rf_load_ktx_image_from_file_ez(const char* file);
#pragma endregion
#pragma endregion

#pragma region gif
RF_API rf_gif rf_load_animated_gif_ez(const void* data, int data_size);
RF_API rf_gif rf_load_animated_gif_file_ez(const char* filename);
RF_API void rf_unload_gif_ez(rf_gif gif);
#pragma endregion

#pragma region texture
RF_API rf_texture2d rf_load_texture_from_file_ez(const char* filename);
RF_API rf_texture2d rf_load_texture_from_file_data_ez(const void* data, int dst_size);
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image_ez(rf_image image, rf_cubemap_layout_type layout_type);
#pragma endregion

#pragma region font
RF_API rf_font rf_load_ttf_font_from_data_ez(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, int chars_count);
RF_API rf_font rf_load_ttf_font_from_file_ez(const char* filename, int font_size, rf_font_antialias antialias);

RF_API rf_font rf_load_image_font_ez(rf_image image, rf_color key);
RF_API rf_font rf_load_image_font_from_file_ez(const char* path, rf_color key);

RF_API void rf_unload_font_ez(rf_font font);
#pragma endregion

#pragma region utf8
RF_API rf_decoded_string rf_decode_utf8_ez(const char* text, int len);
#pragma endregion

#pragma region drawing
RF_API void rf_image_draw_ez(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint);
RF_API void rf_image_draw_rectangle_ez(rf_image* dst, rf_rec rec, rf_color color);
RF_API void rf_image_draw_rectangle_lines_ez(rf_image* dst, rf_rec rec, int thick, rf_color color);
#pragma endregion

#pragma region model & materials & animations
RF_API void rf_mesh_compute_tangents_ez(rf_mesh* mesh);
RF_API void rf_unload_mesh_ez(rf_mesh mesh);

RF_API rf_model rf_load_model_ez(const char* filename);
RF_API rf_model rf_load_model_from_obj_ez(const char* filename);
RF_API rf_model rf_load_model_from_iqm_ez(const char* filename);
RF_API rf_model rf_load_model_from_gltf_ez(const char* filename);
RF_API rf_model rf_load_model_from_mesh_ez(rf_mesh mesh);
RF_API void rf_unload_model_ez(rf_model model);

RF_API rf_materials_array rf_load_materials_from_mtl_ez(const char* filename);
RF_API void rf_unload_material_ez(rf_material material);

RF_API rf_model_animation_array rf_load_model_animations_from_iqm_file_ez(const char* filename);
RF_API rf_model_animation_array rf_load_model_animations_from_iqm_ez(const unsigned char* data, int data_size);
RF_API void rf_unload_model_animation_ez(rf_model_animation anim);

RF_API rf_mesh rf_gen_mesh_cube_ez(float width, float height, float length);
RF_API rf_mesh rf_gen_mesh_poly_ez(int sides, float radius);
RF_API rf_mesh rf_gen_mesh_plane_ez(float width, float length, int res_x, int res_z);
RF_API rf_mesh rf_gen_mesh_sphere_ez(float radius, int rings, int slices);
RF_API rf_mesh rf_gen_mesh_hemi_sphere_ez(float radius, int rings, int slices);
RF_API rf_mesh rf_gen_mesh_cylinder_ez(float radius, float height, int slices);
RF_API rf_mesh rf_gen_mesh_torus_ez(float radius, float size, int rad_seg, int sides);
RF_API rf_mesh rf_gen_mesh_knot_ez(float radius, float size, int rad_seg, int sides);
RF_API rf_mesh rf_gen_mesh_heightmap_ez(rf_image heightmap, rf_vec3 size);
RF_API rf_mesh rf_gen_mesh_cubicmap_ez(rf_image cubicmap, rf_vec3 cube_size);
#pragma endregion

#endif

#endif // RAYFORK_EZ_H