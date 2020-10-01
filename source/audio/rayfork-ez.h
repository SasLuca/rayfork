#ifndef RAYFORK_EZ_H
#define RAYFORK_EZ_H

#if !defined(RAYFORK_NO_EZ_API)

#include "rayfork-model.h"
#include "font.h"
#include "unicode.h"
#include "rayfork-render-batch.h"

RF_API rf_material rf_load_default_material_ez();
RF_API rf_image rf_get_screen_data_ez();
RF_API rf_base64_output rf_decode_base64_ez(const unsigned char* input);
RF_API rf_image rf_gfx_read_texture_pixels_ez(rf_texture2d texture);

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