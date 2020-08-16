#if !defined(RAYFORK_NO_EZ_API)
#pragma region other stuff

RF_API rf_material rf_load_default_material_ez() { return rf_load_default_material(RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_get_screen_data_ez()
{
    if (rf_ctx.render_width < 0 || rf_ctx.render_height < 0) return (rf_image) {0};

    int size = rf_ctx.render_width * rf_ctx.render_height;
    rf_color* dst = RF_ALLOC(RF_DEFAULT_ALLOCATOR, size * sizeof(rf_color));
    rf_image result = rf_get_screen_data(dst, size);

    return result;
}
RF_API rf_base64_output rf_decode_base64_ez(const unsigned char* input) { return rf_decode_base64(input, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gfx_read_texture_pixels_ez(rf_texture2d texture) { return rf_gfx_read_texture_pixels(texture, RF_DEFAULT_ALLOCATOR); }

#pragma endregion

#pragma region image

#pragma region extract image data functions
RF_API rf_color* rf_image_pixels_to_rgba32_ez(rf_image image) { return rf_image_pixels_to_rgba32(image, RF_DEFAULT_ALLOCATOR); }
RF_API rf_vec4* rf_image_compute_pixels_to_normalized_ez(rf_image image) { return rf_image_compute_pixels_to_normalized(image, RF_DEFAULT_ALLOCATOR); }
RF_API rf_palette rf_image_extract_palette_ez(rf_image image, int palette_size) { return rf_image_extract_palette(image, palette_size, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region loading & unloading functions
RF_API rf_image rf_load_image_from_file_data_ez(const void* src, int src_size) { return rf_load_image_from_file_data(src, src_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_load_image_from_hdr_file_data_ez(const void* src, int src_size) { return rf_load_image_from_hdr_file_data(src, src_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_load_image_from_file_ez(const char* filename) { return rf_load_image_from_file(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API void rf_unload_image_ez(rf_image image) { rf_unload_image(image, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region image manipulation
RF_API rf_image rf_image_copy_ez(rf_image image) { return rf_image_copy(image, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_crop_ez(rf_image image, rf_rec crop) { return rf_image_crop(image, crop, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_resize_ez(rf_image image, int new_width, int new_height) { return rf_image_resize(image, new_width, new_height, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_resize_nn_ez(rf_image image, int new_width, int new_height) { return rf_image_resize_nn(image, new_width, new_height, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_format_ez(rf_image image, rf_uncompressed_pixel_format new_format) { return rf_image_format(image, new_format, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_alpha_clear_ez(rf_image image, rf_color color, float threshold) { return rf_image_alpha_clear(image, color, threshold, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_alpha_premultiply_ez(rf_image image) { return rf_image_alpha_premultiply(image, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_alpha_crop_ez(rf_image image, float threshold) { return rf_image_alpha_crop(image, threshold, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_dither_ez(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp) { return rf_image_dither(image, r_bpp, g_bpp, b_bpp, a_bpp, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }

RF_API rf_image rf_image_flip_vertical_ez(rf_image image) { return rf_image_flip_vertical(image, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_image_flip_horizontal_ez(rf_image image) { return rf_image_flip_horizontal(image, RF_DEFAULT_ALLOCATOR); }

RF_API rf_vec2 rf_get_seed_for_cellular_image_ez(int seeds_per_row, int tile_size, int i) { return rf_get_seed_for_cellular_image(
        seeds_per_row, tile_size, i, RF_DEFAULT_RAND_PROC); }

RF_API rf_image rf_gen_image_color_ez(int width, int height, rf_color color) { return rf_gen_image_color(width, height, color, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_gradient_v_ez(int width, int height, rf_color top, rf_color bottom) { return rf_gen_image_gradient_v(width, height, top, bottom, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_gradient_h_ez(int width, int height, rf_color left, rf_color right) { return rf_gen_image_gradient_h(width, height, left, right, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_gradient_radial_ez(int width, int height, float density, rf_color inner, rf_color outer) { return rf_gen_image_gradient_radial(width, height, density, inner, outer, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_checked_ez(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2) { return rf_gen_image_checked(width, height, checks_x, checks_y, col1, col2, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_white_noise_ez(int width, int height, float factor) { return rf_gen_image_white_noise(
        width, height, factor, RF_DEFAULT_RAND_PROC, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_perlin_noise_ez(int width, int height, int offset_x, int offset_y, float scale) { return rf_gen_image_perlin_noise(width, height, offset_x, offset_y, scale, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_gen_image_cellular_ez(int width, int height, int tile_size) { return rf_gen_image_cellular(width,
                                                                                                              height,
                                                                                                              tile_size,
                                                                                                              RF_DEFAULT_RAND_PROC,
                                                                                                              RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region mipmaps
RF_API rf_mipmaps_image rf_image_gen_mipmaps_ez(rf_image image, int gen_mipmaps_count) { return rf_image_gen_mipmaps(image, gen_mipmaps_count, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_unload_mipmaps_image_ez(rf_mipmaps_image image) { rf_unload_mipmaps_image(image, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region dds
RF_API rf_mipmaps_image rf_load_dds_image_ez(const void* src, int src_size) { return rf_load_dds_image(src, src_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mipmaps_image rf_load_dds_image_from_file_ez(const char* file) { return rf_load_dds_image_from_file(file, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
#pragma endregion

#pragma region pkm
RF_API rf_image rf_load_pkm_image_ez(const void* src, int src_size) { return rf_load_pkm_image(src, src_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_image rf_load_pkm_image_from_file_ez(const char* file) { return rf_load_pkm_image_from_file(file, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
#pragma endregion

#pragma region ktx
RF_API rf_mipmaps_image rf_load_ktx_image_ez(const void* src, int src_size) { return rf_load_ktx_image(src, src_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mipmaps_image rf_load_ktx_image_from_file_ez(const char* file) { return rf_load_ktx_image_from_file(file, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
#pragma endregion

#pragma endregion

#pragma region gif
RF_API rf_gif rf_load_animated_gif_ez(const void* data, int data_size) { return rf_load_animated_gif(data, data_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_gif rf_load_animated_gif_file_ez(const char* filename) { return rf_load_animated_gif_file(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API void rf_unload_gif_ez(rf_gif gif) { rf_unload_gif(gif, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region texture
RF_API rf_texture2d rf_load_texture_from_file_ez(const char* filename) { return rf_load_texture_from_file(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API rf_texture2d rf_load_texture_from_file_data_ez(const void* dst, int dst_size) { return rf_load_texture_from_file_data(dst, dst_size, RF_DEFAULT_ALLOCATOR); }
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image_ez(rf_image image, rf_cubemap_layout_type layout_type) { return rf_load_texture_cubemap_from_image(image, layout_type, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region font
RF_API rf_font rf_load_ttf_font_from_data_ez(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, int chars_count) { return rf_load_ttf_font_from_data(font_file_data, font_size, antialias, chars, chars_count, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_font rf_load_ttf_font_from_file_ez(const char* filename, int font_size, rf_font_antialias antialias) { return rf_load_ttf_font_from_file(filename, font_size, antialias, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }

RF_API rf_font rf_load_image_font_ez(rf_image image, rf_color key) { return rf_load_image_font(image, key, RF_DEFAULT_ALLOCATOR); }
RF_API rf_font rf_load_image_font_from_file_ez(const char* path, rf_color key) { return rf_load_image_font_from_file(path, key, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }

RF_API void rf_unload_font_ez(rf_font font) { rf_unload_font(font, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region utf8
RF_API rf_decoded_string rf_decode_utf8_ez(const char* text, int len) { return rf_decode_utf8(text, len, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region drawing
RF_API void rf_image_draw_ez(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint) { rf_image_draw(dst, src, src_rec, dst_rec, tint, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_image_draw_rectangle_ez(rf_image* dst, rf_rec rec, rf_color color) { rf_image_draw_rectangle(dst, rec, color, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_image_draw_rectangle_lines_ez(rf_image* dst, rf_rec rec, int thick, rf_color color) { rf_image_draw_rectangle_lines(dst, rec, thick, color, RF_DEFAULT_ALLOCATOR); }
#pragma endregion

#pragma region model & materials & animations
RF_API void rf_mesh_compute_tangents_ez(rf_mesh* mesh) { rf_mesh_compute_tangents(mesh, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_unload_mesh_ez(rf_mesh mesh) { rf_unload_mesh(mesh, RF_DEFAULT_ALLOCATOR); }

RF_API rf_model rf_load_model_ez(const char* filename) { return rf_load_model(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API rf_model rf_load_model_from_obj_ez(const char* filename) { return rf_load_model_from_obj(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API rf_model rf_load_model_from_iqm_ez(const char* filename) { return rf_load_model_from_iqm(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API rf_model rf_load_model_from_gltf_ez(const char* filename) { return rf_load_model_from_gltf(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API rf_model rf_load_model_from_mesh_ez(rf_mesh mesh) { return rf_load_model_from_mesh(mesh, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_unload_model_ez(rf_model model) { rf_unload_model(model, RF_DEFAULT_ALLOCATOR); }

RF_API rf_materials_array rf_load_materials_from_mtl_ez(const char* filename) { return rf_load_materials_from_mtl(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API void rf_unload_material_ez(rf_material material) { rf_unload_material(material, RF_DEFAULT_ALLOCATOR); }

RF_API rf_model_animation_array rf_load_model_animations_from_iqm_file_ez(const char* filename) { return rf_load_model_animations_from_iqm_file(filename, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); }
RF_API rf_model_animation_array rf_load_model_animations_from_iqm_ez(const unsigned char* data, int data_size) { return rf_load_model_animations_from_iqm(data, data_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API void rf_unload_model_animation_ez(rf_model_animation anim) { rf_unload_model_animation(anim, RF_DEFAULT_ALLOCATOR); }

RF_API rf_mesh rf_gen_mesh_cube_ez(float width, float height, float length) { return rf_gen_mesh_cube(width, height, length, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_poly_ez(int sides, float radius) { return rf_gen_mesh_poly(sides, radius, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_plane_ez(float width, float length, int res_x, int res_z) { return rf_gen_mesh_plane(width, length, res_x, res_z, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_sphere_ez(float radius, int rings, int slices) { return rf_gen_mesh_sphere(radius, rings, slices, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_hemi_sphere_ez(float radius, int rings, int slices) { return rf_gen_mesh_hemi_sphere(radius, rings, slices, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_cylinder_ez(float radius, float height, int slices) { return rf_gen_mesh_cylinder(radius, height, slices, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_torus_ez(float radius, float size, int rad_seg, int sides) { return rf_gen_mesh_torus(radius, size, rad_seg, sides, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_knot_ez(float radius, float size, int rad_seg, int sides) { return rf_gen_mesh_knot(radius, size, rad_seg, sides, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_heightmap_ez(rf_image heightmap, rf_vec3 size) { return rf_gen_mesh_heightmap(heightmap, size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
RF_API rf_mesh rf_gen_mesh_cubicmap_ez(rf_image cubicmap, rf_vec3 cube_size) { return rf_gen_mesh_cubicmap(cubicmap, cube_size, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR); }
#pragma endregion
#endif