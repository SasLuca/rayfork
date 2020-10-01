#ifndef RAYFORK_TEXTURE_H
#define RAYFORK_TEXTURE_H

#include "rayfork-low-level-renderer.h"

RF_API rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks io); // Load texture from file into GPU memory (VRAM)
RF_API rf_texture2d rf_load_texture_from_file_data(const void* data, rf_int dst_size, rf_allocator temp_allocator); // Load texture from an image file data using stb
RF_API rf_texture2d rf_load_texture_from_image(rf_image image); // Load texture from image data
RF_API rf_texture2d rf_load_texture_from_image_with_mipmaps(rf_mipmaps_image image); // Load texture from image data
RF_API rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator); // Load cubemap from image, multiple image cubemap layouts supported
RF_API rf_render_texture2d rf_load_render_texture(int width, int height); // Load texture for rendering (framebuffer)

RF_API void rf_update_texture(rf_texture2d texture, const void* pixels, rf_int pixels_size); // Update GPU texture with new data. Pixels data must match texture.format
RF_API void rf_gen_texture_mipmaps(rf_texture2d* texture); // Generate GPU mipmaps for a texture
RF_API void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode); // Set texture scaling filter mode
RF_API void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode); // Set texture wrapping mode
RF_API void rf_unload_texture(rf_texture2d texture); // Unload texture from GPU memory (VRAM)
RF_API void rf_unload_render_texture(rf_render_texture2d target); // Unload render texture from GPU memory (VRAM)

RF_API rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, rf_int size); // Generate cubemap texture from HDR texture
RF_API rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, rf_int size); // Generate irradiance texture using cubemap data
RF_API rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, rf_int size); // Generate prefilter texture using cubemap data
RF_API rf_texture2d rf_gen_texture_brdf(rf_shader shader, rf_int size); // Generate BRDF texture using cubemap data.

#endif // RAYFORK_TEXTURE_H