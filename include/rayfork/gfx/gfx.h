#ifndef RAYFORK_GFX_H
#define RAYFORK_GFX_H

#include "gfx-types.h"
#include "rayfork/image/image.h"

rf_extern rf_shader rf_gfx_load_default_shader();
rf_extern rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code); // Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
rf_extern void rf_gfx_unload_shader(rf_shader shader); // Unload shader from GPU memory (VRAM)
rf_extern int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name); // Get shader uniform location
rf_extern void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name); // Set shader uniform value
rf_extern void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count); // Set shader uniform value vector
rf_extern void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat); // Set shader uniform value (matrix 4x4)
rf_extern void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture); // Set shader uniform value for texture

rf_extern rf_mat rf_gfx_get_matrix_projection(); // Return internal rf__ctx->gl_ctx.projection matrix
rf_extern rf_mat rf_gfx_get_matrix_modelview(); // Return internal rf__ctx->gl_ctx.modelview matrix
rf_extern void rf_gfx_set_matrix_projection(rf_mat proj); // Set a custom projection matrix (replaces internal rf__ctx->gl_ctx.projection matrix)
rf_extern void rf_gfx_set_matrix_modelview(rf_mat view); // Set a custom rf__ctx->gl_ctx.modelview matrix (replaces internal rf__ctx->gl_ctx.modelview matrix)

rf_extern void rf_gfx_blend_mode(rf_blend_mode mode); // Choose the blending mode (alpha, additive, multiplied)
rf_extern void rf_gfx_matrix_mode(rf_matrix_mode mode); // Choose the current matrix to be transformed
rf_extern void rf_gfx_push_matrix(); // Push the current matrix to rf_global_gl_stack
rf_extern void rf_gfx_pop_matrix(); // Pop lattest inserted matrix from rf_global_gl_stack
rf_extern void rf_gfx_load_identity(); // Reset current matrix to identity matrix
rf_extern void rf_gfx_translatef(float x, float y, float z); // Multiply the current matrix by a translation matrix
rf_extern void rf_gfx_rotatef(float angleDeg, float x, float y, float z); // Multiply the current matrix by a rotation matrix
rf_extern void rf_gfx_scalef(float x, float y, float z); // Multiply the current matrix by a scaling matrix
rf_extern void rf_gfx_mult_matrixf(float* matf); // Multiply the current matrix by another matrix
rf_extern void rf_gfx_frustum(double left, double right, double bottom, double top, double znear, double zfar);
rf_extern void rf_gfx_ortho(double left, double right, double bottom, double top, double znear, double zfar);
rf_extern void rf_gfx_viewport(int x, int y, int width, int height); // Set the viewport area

rf_extern void rf_gfx_begin(rf_drawing_mode mode); // Initialize drawing mode (how to organize vertex)
rf_extern void rf_gfx_end(); // Finish vertex providing
rf_extern void rf_gfx_vertex2i(int x, int y); // Define one vertex (position) - 2 int
rf_extern void rf_gfx_vertex2f(float x, float y); // Define one vertex (position) - 2 float
rf_extern void rf_gfx_vertex3f(float x, float y, float z); // Define one vertex (position) - 3 float
rf_extern void rf_gfx_tex_coord2f(float x, float y); // Define one vertex (texture coordinate) - 2 float
rf_extern void rf_gfx_normal3f(float x, float y, float z); // Define one vertex (normal) - 3 float
rf_extern void rf_gfx_color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Define one vertex (color) - 4 unsigned char
rf_extern void rf_gfx_color3f(float x, float y, float z); // Define one vertex (color) - 3 float
rf_extern void rf_gfx_color4f(float x, float y, float z, float w); // Define one vertex (color) - 4 float

rf_extern void rf_gfx_enable_texture(unsigned int id); // Enable texture usage
rf_extern void rf_gfx_disable_texture(); // Disable texture usage
rf_extern void rf_gfx_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode); // Set texture parameters (wrap mode/filter mode)
rf_extern void rf_gfx_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode); // Set filter for texture
rf_extern void rf_gfx_enable_render_texture(unsigned int id); // Enable render texture (fbo)
rf_extern void rf_gfx_disable_render_texture(void); // Disable render texture (fbo), return to default framebuffer
rf_extern void rf_gfx_enable_depth_test(void); // Enable depth test
rf_extern void rf_gfx_disable_depth_test(void); // Disable depth test
rf_extern void rf_gfx_enable_backface_culling(void); // Enable backface culling
rf_extern void rf_gfx_disable_backface_culling(void); // Disable backface culling
rf_extern void rf_gfx_enable_scissor_test(void); // Enable scissor test
rf_extern void rf_gfx_disable_scissor_test(void); // Disable scissor test
rf_extern void rf_gfx_scissor(int x, int y, int width, int height); // Scissor test
rf_extern void rf_gfx_enable_wire_mode(void); // Enable wire mode
rf_extern void rf_gfx_disable_wire_mode(void); // Disable wire mode
rf_extern void rf_gfx_delete_textures(unsigned int id); // Delete OpenGL texture from GPU
rf_extern void rf_gfx_delete_render_textures(rf_render_texture2d target); // Delete render textures (fbo) from GPU
rf_extern void rf_gfx_delete_shader(unsigned int id); // Delete OpenGL shader program from GPU
rf_extern void rf_gfx_delete_vertex_arrays(unsigned int id); // Unload vertex data (VAO) from GPU memory
rf_extern void rf_gfx_delete_buffers(unsigned int id); // Unload vertex data (VBO) from GPU memory
rf_extern void rf_gfx_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Clear color buffer with color
rf_extern void rf_gfx_clear_screen_buffers(void); // Clear used screen buffers (color and depth)
rf_extern void rf_gfx_update_buffer(int buffer_id, void* data, int data_size); // Update GPU buffer with new data
rf_extern unsigned int rf_gfx_load_attrib_buffer(unsigned int vao_id, int shader_loc, void* buffer, int size, bool dynamic); // Load a new attributes buffer
rf_extern void rf_gfx_init_vertex_buffer(struct rf_vertex_buffer* vertex_buffer);

rf_extern void rf_gfx_close(); // De-inititialize rf gfx (buffers, shaders, textures)
rf_extern void rf_gfx_draw(); // Update and draw default internal buffers

rf_extern bool rf_gfx_check_buffer_limit(int v_count); // Check internal buffer overflow for a given number of vertex
rf_extern void rf_gfx_set_debug_marker(const char* text); // Set debug marker for analysis

// Textures data management
rf_extern unsigned int rf_gfx_load_texture(void* data, int width, int height, rf_pixel_format format, int mipmap_count); // Load texture in GPU
rf_extern unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, bool use_render_buffer); // Load depth texture/renderbuffer (to be attached to fbo)
rf_extern unsigned int rf_gfx_load_texture_cubemap(void* data, int size, rf_pixel_format format); // Load texture cubemap
rf_extern void rf_gfx_update_texture(unsigned int id, int width, int height, rf_pixel_format format, const void* pixels, int pixels_size); // Update GPU texture with new data
rf_extern rf_gfx_pixel_format rf_gfx_get_internal_texture_formats(rf_pixel_format format); // Get OpenGL internal formats
rf_extern void rf_gfx_unload_texture(unsigned int id); // Unload texture from GPU memory

rf_extern void rf_gfx_generate_mipmaps(rf_texture2d* texture); // Generate mipmap data for selected texture
rf_extern rf_image rf_gfx_read_texture_pixels_to_buffer(rf_texture2d texture, void* dst, int dst_size);
rf_extern rf_image rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator);
rf_extern void rf_gfx_read_screen_pixels(rf_color* dst, int width, int height); // Read screen pixel data (color buffer)

// Render texture management (fbo)
rf_extern rf_render_texture2d rf_gfx_load_render_texture(int width, int height, rf_pixel_format format, int depth_bits, bool use_depth_texture); // Load a render texture (with color and depth attachments)
rf_extern void rf_gfx_render_texture_attach(rf_render_texture2d target, unsigned int id, int attach_type); // Attach texture/renderbuffer to an fbo
rf_extern bool rf_gfx_render_texture_complete(rf_render_texture2d target); // Verify render texture is complete

// Vertex data management
rf_extern void rf_gfx_load_mesh(struct rf_mesh* mesh, bool dynamic); // Upload vertex data into GPU and provided VAO/VBO ids
rf_extern void rf_gfx_update_mesh(struct rf_mesh mesh, int buffer, int num); // Update vertex or index data on GPU (upload new data to one buffer)
rf_extern void rf_gfx_update_mesh_at(struct rf_mesh mesh, int buffer, int num, int index); // Update vertex or index data on GPU, at index
rf_extern void rf_gfx_draw_mesh(struct rf_mesh mesh, struct rf_material material, rf_mat transform); // Draw a 3d mesh with material and transform
rf_extern void rf_gfx_unload_mesh(struct rf_mesh mesh); // Unload mesh data from CPU and GPU

#endif // RAYFORK_GFX_H