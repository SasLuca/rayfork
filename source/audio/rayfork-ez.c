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