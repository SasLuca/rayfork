#ifndef RAYFORK_GFX_TEXTURE_H
#define RAYFORK_GFX_TEXTURE_H

#include "rayfork/gfx/gfx-types.h"
#include "rayfork/foundation/io.h"
#include "rayfork/image/image.h"

/* Load texture from file into GPU memory (VRAM) */
rf_extern rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks* io);

/* Load texture from an image file data using stb */
rf_extern rf_texture2d rf_load_texture_from_file_data(const void* data, rf_int dst_size, rf_allocator temp_allocator);

/* Load texture from image data */
rf_extern rf_texture2d rf_load_texture_from_image(rf_image image);

/* Load texture from image data */
rf_extern rf_texture2d rf_load_texture_from_image_with_mipmaps(rf_mipmaps_image image);

/* Load cubemap from image, multiple image cubemap layouts supported */
rf_extern rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator);

/* Load texture for rendering (framebuffer) */
rf_extern rf_render_texture2d rf_load_render_texture(int width, int height);

/* Update GPU texture with new data. Pixels data must match texture.format */
rf_extern void rf_update_texture(rf_texture2d texture, const void* pixels, rf_int pixels_size);

/* Generate GPU mipmaps for a texture */
rf_extern void rf_gen_texture_mipmaps(rf_texture2d* texture);

/* Set texture scaling filter mode */
rf_extern void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode);

/* Set texture wrapping mode */
rf_extern void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode);

/* Unload texture from GPU memory (VRAM) */
rf_extern void rf_texture_free(rf_texture2d texture);

/* Unload render texture from GPU memory (VRAM) */
rf_extern void rf_unload_render_texture(rf_render_texture2d target);

/* Generate cubemap texture from HDR texture */
rf_extern rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, rf_int size);

/* Generate irradiance texture using cubemap data */
rf_extern rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, rf_int size);

/* Generate prefilter texture using cubemap data */
rf_extern rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, rf_int size);

/* Generate BRDF texture using cubemap data */
rf_extern rf_texture2d rf_gen_texture_brdf(rf_shader shader, rf_int size);

#endif // RAYFORK_GFX_TEXTURE_H