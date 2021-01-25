#ifndef RAYFORK_GFX_RENDERER_H
#define RAYFORK_GFX_RENDERER_H

#include "rayfork/gfx/camera.h"
#include "rayfork/gfx/bitmap-font.h"

typedef struct rf_model rf_model;

typedef enum rf_text_wrap_mode
{
    RF_CHAR_WRAP,
    RF_WORD_WRAP,
} rf_text_wrap_mode;

typedef enum rf_ninepatch_type
{
    RF_NPT_9PATCH = 0,       // Npatch defined by 3x3 tiles
    RF_NPT_3PATCH_VERTICAL,  // Npatch defined by 1x3 tiles
    RF_NPT_3PATCH_HORIZONTAL // Npatch defined by 3x1 tiles
} rf_ninepatch_type;

typedef struct rf_npatch_info
{
    rf_rec source_rec;   // Region in the texture
    int left;            // left border offset
    int top;             // top border offset
    int right;           // right border offset
    int bottom;          // bottom border offset
    int type;            // layout of the n-patch: 3x3, 1x3 or 3x1
} rf_npatch_info;

rf_extern rf_vec2 rf_center_to_screen(float w, float h); // Returns the position of an object such that it will be centered to the screen

rf_extern void rf_clear(rf_color color); // Set background color (framebuffer clear color)

rf_extern void rf_begin(); // Setup canvas (framebuffer) to start drawing
rf_extern void rf_end(); // End canvas drawing and swap buffers (double buffering)

rf_extern void rf_begin_2d(rf_camera2d camera); // Initialize 2D mode with custom camera (2D)
rf_extern void rf_end_2d(); // Ends 2D mode with custom camera

rf_extern void rf_begin_3d(rf_camera3d camera); // Initializes 3D mode with custom camera (3D)
rf_extern void rf_end_3d(); // Ends 3D mode and returns to default 2D orthographic mode

rf_extern void rf_begin_render_to_texture(rf_render_texture2d target); // Initializes render texture for drawing
rf_extern void rf_end_render_to_texture(); // Ends drawing to render texture

rf_extern void rf_begin_scissor_mode(int x, int y, int width, int height); // Begin scissor mode (define screen area for following drawing)
rf_extern void rf_end_scissor_mode(); // End scissor mode

rf_extern void rf_begin_shader(rf_shader shader); // Begin custom shader drawing
rf_extern void rf_end_shader(); // End custom shader drawing (use default shader)

rf_extern void rf_begin_blend_mode(rf_blend_mode mode); // Begin blending mode (alpha, additive, multiplied)
rf_extern void rf_end_blend_mode(); // End blending mode (reset to default: alpha blending)

rf_extern void rf_draw_pixel(int pos_x, int pos_y, rf_color color); // Draw a pixel
rf_extern void rf_draw_pixel_v(rf_vec2 position, rf_color color); // Draw a pixel (Vector version)

rf_extern void rf_draw_line(int startPosX, int startPosY, int endPosX, int endPosY, rf_color color); // Draw a line
rf_extern void rf_draw_line_v(rf_vec2 startPos, rf_vec2 endPos, rf_color color); // Draw a line (Vector version)
rf_extern void rf_draw_line_ex(rf_vec2 startPos, rf_vec2 endPos, float thick, rf_color color); // Draw a line defining thickness
rf_extern void rf_draw_line_bezier(rf_vec2 start_pos, rf_vec2 end_pos, float thick, rf_color color); // Draw a line using cubic-bezier curves in-out
rf_extern void rf_draw_line_strip(rf_vec2* points, int num_points, rf_color color); // Draw lines sequence

rf_extern void rf_draw_circle(int center_x, int center_y, float radius, rf_color color); // Draw a color-filled circle
rf_extern void rf_draw_circle_v(rf_vec2 center, float radius, rf_color color); // Draw a color-filled circle (Vector version)
rf_extern void rf_draw_circle_sector(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color); // Draw a piece of a circle
rf_extern void rf_draw_circle_sector_lines(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color); // Draw circle sector outline
rf_extern void rf_draw_circle_gradient(int center_x, int center_y, float radius, rf_color color1, rf_color color2); // Draw a gradient-filled circle
rf_extern void rf_draw_circle_lines(int center_x, int center_y, float radius, rf_color color); // Draw circle outline

rf_extern void rf_draw_ring(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color); // Draw ring
rf_extern void rf_draw_ring_lines(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color); // Draw ring outline

rf_extern void rf_draw_rectangle(int posX, int posY, int width, int height, rf_color color); // Draw a color-filled rectangle
rf_extern void rf_draw_rectangle_v(rf_vec2 position, rf_vec2 size, rf_color color); // Draw a color-filled rectangle (Vector version)
rf_extern void rf_draw_rectangle_rec(rf_rec rec, rf_color color); // Draw a color-filled rectangle
rf_extern void rf_draw_rectangle_pro(rf_rec rec, rf_vec2 origin, float rotation, rf_color color); // Draw a color-filled rectangle with pro parameters

rf_extern void rf_draw_rectangle_gradient_v(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2);// Draw a vertical-gradient-filled rectangle
rf_extern void rf_draw_rectangle_gradient_h(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2);// Draw a horizontal-gradient-filled rectangle
rf_extern void rf_draw_rectangle_gradient(rf_rec rec, rf_color col1, rf_color col2, rf_color col3, rf_color col4); // Draw a gradient-filled rectangle with custom vertex colors

rf_extern void rf_draw_rectangle_outline(rf_rec rec, int line_thick, rf_color color); // Draw rectangle outline with extended parameters
rf_extern void rf_draw_rectangle_rounded(rf_rec rec, float roundness, int segments, rf_color color); // Draw rectangle with rounded edges
rf_extern void rf_draw_rectangle_rounded_lines(rf_rec rec, float roundness, int segments, int line_thick, rf_color color); // Draw rectangle with rounded edges outline

rf_extern void rf_draw_triangle(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color); // Draw a color-filled triangle (vertex in counter-clockwise order!)
rf_extern void rf_draw_triangle_lines(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color); // Draw triangle outline (vertex in counter-clockwise order!)
rf_extern void rf_draw_triangle_fan(rf_vec2* points, int num_points, rf_color color); // Draw a triangle fan defined by points (first vertex is the center)
rf_extern void rf_draw_triangle_strip(rf_vec2* points, int points_count, rf_color color); // Draw a triangle strip defined by points
rf_extern void rf_draw_poly(rf_vec2 center, int sides, float radius, float rotation, rf_color color); // Draw a regular polygon (Vector version)

// rf_texture2d drawing functions

rf_extern void rf_draw_texture(rf_texture2d texture, int x, int y, rf_color tint); // Draw a rf_texture2d with extended parameters
rf_extern void rf_draw_texture_ex(rf_texture2d texture, int x, int y, int w, int h, float rotation, rf_color tint); // Draw a rf_texture2d with extended parameters
rf_extern void rf_draw_texture_region(rf_texture2d texture, rf_rec source_rec, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint); // Draw a part of a texture defined by a rectangle with 'pro' parameters
rf_extern void rf_draw_texture_npatch(rf_texture2d texture, rf_npatch_info n_patch_info, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint); // Draws a texture (or part of it) that stretches or shrinks nicely

// Text drawing functions

rf_extern void rf_draw_string(const char* string, int string_len, int posX, int posY, int font_size, rf_color color); // Draw text (using default font)
rf_extern void rf_draw_string_ex(rf_bitmap_font font, const char* string, int string_len, rf_vec2 position, float fontSize, float spacing, rf_color tint); // Draw text using font and additional parameters
rf_extern void rf_draw_string_wrap(rf_bitmap_font font, const char* string, int string_len, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode); // Draw text and wrap at a specific width
rf_extern void rf_draw_string_rec(rf_bitmap_font font, const char* string, int string_len, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint); // Draw text using font inside rectangle limits

rf_extern void rf_draw_text(const char* text, int posX, int posY, int font_size, rf_color color); // Draw text (using default font)
rf_extern void rf_draw_text_ex(rf_bitmap_font font, const char* text, rf_vec2 position, float fontSize, float spacing, rf_color tint); // Draw text using font and additional parameters
rf_extern void rf_draw_text_wrap(rf_bitmap_font font, const char* text, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode); // Draw text and wrap at a specific width
rf_extern void rf_draw_text_rec(rf_bitmap_font font, const char* text, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint); // Draw text using font inside rectangle limits

rf_extern void rf_draw_line3d(rf_vec3 start_pos, rf_vec3 end_pos, rf_color color); // Draw a line in 3D world space
rf_extern void rf_draw_circle3d(rf_vec3 center, float radius, rf_vec3 rotation_axis, float rotation_angle, rf_color color); // Draw a circle in 3D world space
rf_extern void rf_draw_cube(rf_vec3 position, float width, float height, float length, rf_color color); // Draw cube
rf_extern void rf_draw_cube_wires(rf_vec3 position, float width, float height, float length, rf_color color); // Draw cube wires
rf_extern void rf_draw_cube_texture(rf_texture2d texture, rf_vec3 position, float width, float height, float length, rf_color color); // Draw cube textured
rf_extern void rf_draw_sphere(rf_vec3 center_pos, float radius, rf_color color); // Draw sphere
rf_extern void rf_draw_sphere_ex(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color); // Draw sphere with extended parameters
rf_extern void rf_draw_sphere_wires(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color); // Draw sphere wires
rf_extern void rf_draw_cylinder(rf_vec3 position, float radius_top, float radius_bottom, float height, int slices, rf_color color); // Draw a cylinder/cone
rf_extern void rf_draw_cylinder_wires(rf_vec3 position, float radius_top, float radius_bottom, float height, int slices, rf_color color); // Draw a cylinder/cone wires
rf_extern void rf_draw_plane(rf_vec3 center_pos, rf_vec2 size, rf_color color); // Draw a plane XZ
rf_extern void rf_draw_ray(rf_ray ray, rf_color color); // Draw a ray line
rf_extern void rf_draw_grid(int slices, float spacing); // Draw a grid (centered at (0, 0, 0))
rf_extern void rf_draw_gizmo(rf_vec3 position); // Draw simple gizmo

// rf_model drawing functions
rf_extern void rf_draw_model(rf_model model, rf_vec3 position, float scale, rf_color tint); // Draw a model (with texture if set)
rf_extern void rf_draw_model_ex(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotation_angle, rf_vec3 scale, rf_color tint); // Draw a model with extended parameters
rf_extern void rf_draw_model_wires(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotation_angle, rf_vec3 scale, rf_color tint); // Draw a model wires (with texture if set) with extended parameters
rf_extern void rf_draw_bounding_box(rf_bounding_box box, rf_color color); // Draw bounding box (wires)
rf_extern void rf_draw_billboard(rf_camera3d camera, rf_texture2d texture, rf_vec3 center, float size, rf_color tint); // Draw a billboard texture
rf_extern void rf_draw_billboard_rec(rf_camera3d camera, rf_texture2d texture, rf_rec source_rec, rf_vec3 center, float size, rf_color tint); // Draw a billboard texture defined by source_rec

// rf_image draw

#endif // RAYFORK_GFX_RENDERER_H