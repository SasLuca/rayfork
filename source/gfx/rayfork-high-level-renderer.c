#include "rayfork-gfx-internal.h"
#include "rayfork-camera.h"
#include "rayfork-high-level-renderer.h"

#pragma region internal functions

// Get texture to draw shapes, the user can customize this using rf_set_shapes_texture
RF_INTERNAL rf_texture2d rf_get_shapes_texture()
{
    if (rf_ctx.tex_shapes.id == 0)
    {
        rf_ctx.tex_shapes = rf_get_default_texture();
        rf_ctx.rec_tex_shapes = (rf_rec) {0.0f, 0.0f, 1.0f, 1.0f };
    }

    return rf_ctx.tex_shapes;
}

// Cubic easing in-out. Note: Required for rf_draw_line_bezier()
RF_INTERNAL float rf_shapes_ease_cubic_in_out(float t, float b, float c, float d)
{
    if ((t /= 0.5f*d) < 1) return 0.5f*c*t*t*t + b;

    t -= 2;

    return 0.5f*c*(t*t*t + 2.0f) + b;
}

#pragma endregion

RF_API rf_vec2 rf_center_to_screen(float w, float h)
{
    rf_vec2 result = { rf_ctx.render_width / 2 - w / 2, rf_ctx.render_height / 2 - h / 2 };
    return result;
}

// Set background color (framebuffer clear color)
RF_API void rf_clear(rf_color color)
{
    rf_gfx_clear_color(color.r, color.g, color.b, color.a); // Set clear color
    rf_gfx_clear_screen_buffers(); // Clear current framebuffers
}

// Setup canvas (framebuffer) to start drawing
RF_API void rf_begin()
{
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v); // Apply screen scaling

    //rf_gfx_translatef(0.375, 0.375, 0);    // HACK to have 2D pixel-perfect drawing on OpenGL 1.1, not required with OpenGL 3.3+
}

// End canvas drawing and swap buffers (double buffering)
RF_API void rf_end()
{
    rf_gfx_draw();
}

// Initialize 2D mode with custom camera (2D)
RF_API void rf_begin_2d(rf_camera2d camera)
{
    rf_gfx_draw();

    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    // Apply screen scaling if required
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v);

    // Apply 2d camera transformation to rf_gfxobal_model_view
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_get_camera_matrix2d(camera)).v);
}

// Ends 2D mode with custom camera
RF_API void rf_end_2d()
{
    rf_gfx_draw();

    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v); // Apply screen scaling if required
}

// Initializes 3D mode with custom camera (3D)
RF_API void rf_begin_3d(rf_camera3d camera)
{
    rf_gfx_draw();

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to GL_PROJECTION matrix
    rf_gfx_push_matrix(); // Save previous matrix, which contains the settings for the 2d ortho GL_PROJECTION
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    float aspect = (float) rf_ctx.current_width / (float)rf_ctx.current_height;

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        // Setup perspective GL_PROJECTION
        double top = 0.01 * tan(camera.fovy*0.5*RF_DEG2RAD);
        double right = top*aspect;

        rf_gfx_frustum(-right, right, -top, top, 0.01, 1000.0);
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic GL_PROJECTION
        double top = camera.fovy/2.0;
        double right = top*aspect;

        rf_gfx_ortho(-right,right,-top,top, 0.01, 1000.0);
    }

    // NOTE: zNear and zFar values are important when computing depth buffer values

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to rf_gfxobal_model_view matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    // Setup rf_camera3d view
    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);
    rf_gfx_mult_matrixf(rf_mat_to_float16(mat_view).v); // Multiply MODELVIEW matrix by view matrix (camera)

    rf_gfx_enable_depth_test(); // Enable DEPTH_TEST for 3D
}

// Ends 3D mode and returns to default 2D orthographic mode
RF_API void rf_end_3d()
{
    rf_gfx_draw(); // Process internal buffers (update + draw)

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to GL_PROJECTION matrix
    rf_gfx_pop_matrix(); // Restore previous matrix (PROJECTION) from matrix rf_gfxobal_gl_stack

    rf_gfx_matrix_mode(RF_MODELVIEW); // Get back to rf_gfxobal_model_view matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v); // Apply screen scaling if required

    rf_gfx_disable_depth_test(); // Disable DEPTH_TEST for 2D
}

// Initializes render texture for drawing
RF_API void rf_begin_render_to_texture(rf_render_texture2d target)
{
    rf_gfx_draw();

    rf_gfx_enable_render_texture(target.id); // Enable render target

    // Set viewport to framebuffer size
    rf_gfx_viewport(0, 0, target.texture.width, target.texture.height);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rf_gfx_ortho(0, target.texture.width, target.texture.height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    //rf_gfx_scalef(0.0f, -1.0f, 0.0f);      // Flip Y-drawing (?)

    // Setup current width/height for proper aspect ratio
    // calculation when using rf_begin_mode3d()
    rf_ctx.current_width = target.texture.width;
    rf_ctx.current_height = target.texture.height;
}

// Ends drawing to render texture
RF_API void rf_end_render_to_texture()
{
    rf_gfx_draw();

    rf_gfx_disable_render_texture(); // Disable render target

    // Set viewport to default framebuffer size
    rf_set_viewport(rf_ctx.render_width, rf_ctx.render_height);

    // Reset current screen size
    rf_ctx.current_width = rf_ctx.render_width;
    rf_ctx.current_height = rf_ctx.render_height;
}

// Begin scissor mode (define screen area for following drawing)
// NOTE: Scissor rec refers to bottom-left corner, we change it to upper-left
RF_API void rf_begin_scissor_mode(int x, int y, int width, int height)
{
    rf_gfx_draw(); // Force drawing elements

    rf_gfx_enable_scissor_test();
    rf_gfx_scissor(x, rf_ctx.render_width - (y + height), width, height);
}

// End scissor mode
RF_API void rf_end_scissor_mode()
{
    rf_gfx_draw(); // Force drawing elements
    rf_gfx_disable_scissor_test();
}

// Begin custom shader mode
RF_API void rf_begin_shader(rf_shader shader)
{
    if (rf_ctx.current_shader.id != shader.id)
    {
        rf_gfx_draw();
        rf_ctx.current_shader = shader;
    }
}

// End custom shader mode (returns to default shader)
RF_API void rf_end_shader()
{
    rf_begin_shader(rf_ctx.default_shader);
}

// Begin blending mode (alpha, additive, multiplied). Default blend mode is alpha
RF_API void rf_begin_blend_mode(rf_blend_mode mode)
{
    rf_gfx_blend_mode(mode);
}

// End blending mode (reset to default: alpha blending)
RF_API void rf_end_blend_mode()
{
    rf_gfx_blend_mode(RF_BLEND_ALPHA);
}

// Draw a pixel
RF_API void rf_draw_pixel(int pos_x, int pos_y, rf_color color)
{
    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2i(pos_x, pos_y);
    rf_gfx_vertex2i(pos_x + 1, pos_y + 1);
    rf_gfx_end();
}

// Draw a pixel (Vector version)
RF_API void rf_draw_pixel_v(rf_vec2 position, rf_color color)
{
    rf_draw_pixel(position.x, position.y, color);
}

// Draw a line
RF_API void rf_draw_line(int startPosX, int startPosY, int endPosX, int endPosY, rf_color color)
{
    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2i(startPosX, startPosY);
    rf_gfx_vertex2i(endPosX, endPosY);
    rf_gfx_end();
}

// Draw a line (Vector version)
RF_API void rf_draw_line_v(rf_vec2 startPos, rf_vec2 endPos, rf_color color)
{
    rf_draw_line(startPos.x, startPos.y, endPos.x, endPos.y, color);
}

// Draw a line defining thickness
RF_API void rf_draw_line_ex(rf_vec2 start_pos, rf_vec2 end_pos, float thick, rf_color color)
{
    if (start_pos.x > end_pos.x)
    {
        rf_vec2 temp_pos = start_pos;
        start_pos = end_pos;
        end_pos = temp_pos;
    }

    float dx = end_pos.x - start_pos.x;
    float dy = end_pos.y - start_pos.y;

    float d = sqrtf(dx*dx + dy*dy);
    float angle = asinf(dy/d);

    rf_gfx_enable_texture(rf_get_shapes_texture().id);

    rf_gfx_push_matrix();
    rf_gfx_translatef((float)start_pos.x, (float)start_pos.y, 0.0f);
    rf_gfx_rotatef(RF_RAD2DEG * angle, 0.0f, 0.0f, 1.0f);
    rf_gfx_translatef(0, (thick > 1.0f)? -thick/2.0f : -1.0f, 0.0f);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, 0.0f);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, thick);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(d, thick);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(d, 0.0f);
    rf_gfx_end();
    rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw line using cubic-bezier curves in-out
RF_API void rf_draw_line_bezier(rf_vec2 start_pos, rf_vec2 end_pos, float thick, rf_color color)
{
#define RF_LINE_DIVISIONS 24 // Bezier line divisions

    rf_vec2 previous = start_pos;
    rf_vec2 current;

    for (rf_int i = 1; i <= RF_LINE_DIVISIONS; i++)
    {
        // Cubic easing in-out
        // NOTE: Easing is calculated only for y position value
        current.y = rf_shapes_ease_cubic_in_out((float)i, start_pos.y, end_pos.y - start_pos.y, (float)RF_LINE_DIVISIONS);
        current.x = previous.x + (end_pos.x - start_pos.x)/ (float)RF_LINE_DIVISIONS;

        rf_draw_line_ex(previous, current, thick, color);

        previous = current;
    }

#undef RF_LINE_DIVISIONS
}

// Draw lines sequence
RF_API void rf_draw_line_strip(rf_vec2 *points, int points_count, rf_color color)
{
    if (points_count >= 2)
    {
        if (rf_gfx_check_buffer_limit(points_count)) rf_gfx_draw();

        rf_gfx_begin(RF_LINES);
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        for (rf_int i = 0; i < points_count - 1; i++)
        {
            rf_gfx_vertex2f(points[i].x, points[i].y);
            rf_gfx_vertex2f(points[i + 1].x, points[i + 1].y);
        }
        rf_gfx_end();
    }
}

// Draw a color-filled circle
RF_API void rf_draw_circle(int center_x, int center_y, float radius, rf_color color)
{
    rf_draw_circle_sector((rf_vec2) {center_x, center_y }, radius, 0, 360, 36, color);
}

// Draw a color-filled circle (Vector version)
RF_API void rf_draw_circle_v(rf_vec2 center, float radius, rf_color color)
{
    rf_draw_circle(center.x, center.y, radius, color);
}

// Draw a piece of a circle
RF_API void rf_draw_circle_sector(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (radius <= 0.0f) radius = 0.1f; // Avoid div by zero

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088
        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * RF_PI / th) / 360;

        if (segments <= 0) segments = 4;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;
    if (rf_gfx_check_buffer_limit(3*segments)) rf_gfx_draw();

    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x, center.y);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*radius, center.y + cosf(RF_DEG2RAD*angle)*radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*radius);

        angle += step_length;
    }
    rf_gfx_end();
}

RF_API void rf_draw_circle_sector_lines(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (radius <= 0.0f) radius = 0.1f; // Avoid div by zero issue

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;


        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * RF_PI / th) / 360;

        if (segments <= 0) segments = 4;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;

    // Hide the cap lines when the circle is full
    rf_bool show_cap_lines = true;
    int limit = 2*(segments + 2);
    if ((end_angle - start_angle)%360 == 0) { limit = 2*segments; show_cap_lines = false; }

    if (rf_gfx_check_buffer_limit(limit)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x, center.y);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*radius, center.y + cosf(RF_DEG2RAD*angle)*radius);
    }

    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*radius, center.y + cosf(RF_DEG2RAD*angle)*radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*radius);

        angle += step_length;
    }

    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x, center.y);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*radius, center.y + cosf(RF_DEG2RAD*angle)*radius);
    }
    rf_gfx_end();
}

// Draw a gradient-filled circle
// NOTE: Gradient goes from center (color1) to border (color2)
RF_API void rf_draw_circle_gradient(int center_x, int center_y, float radius, rf_color color1, rf_color color2)
{
    if (rf_gfx_check_buffer_limit(3 * 36)) rf_gfx_draw();

    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < 360; i += 10)
    {
        rf_gfx_color4ub(color1.r, color1.g, color1.b, color1.a);
        rf_gfx_vertex2f(center_x, center_y);
        rf_gfx_color4ub(color2.r, color2.g, color2.b, color2.a);
        rf_gfx_vertex2f(center_x + sinf(RF_DEG2RAD*i)*radius, center_y + cosf(RF_DEG2RAD*i)*radius);
        rf_gfx_color4ub(color2.r, color2.g, color2.b, color2.a);
        rf_gfx_vertex2f(center_x + sinf(RF_DEG2RAD*(i + 10))*radius, center_y + cosf(RF_DEG2RAD*(i + 10))*radius);
    }
    rf_gfx_end();
}

// Draw circle outline
RF_API void rf_draw_circle_lines(int center_x, int center_y, float radius, rf_color color)
{
    if (rf_gfx_check_buffer_limit(2 * 36)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
    for (rf_int i = 0; i < 360; i += 10)
    {
        rf_gfx_vertex2f(center_x + sinf(RF_DEG2RAD*i)*radius, center_y + cosf(RF_DEG2RAD*i)*radius);
        rf_gfx_vertex2f(center_x + sinf(RF_DEG2RAD*(i + 10))*radius, center_y + cosf(RF_DEG2RAD*(i + 10))*radius);
    }
    rf_gfx_end();
}

RF_API void rf_draw_ring(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (start_angle == end_angle) return;

    // Function expects (outerRadius > innerRadius)
    if (outer_radius < inner_radius)
    {
        float tmp = outer_radius;
        outer_radius = inner_radius;
        inner_radius = tmp;

        if (outer_radius <= 0.0f) outer_radius = 0.1f;
    }

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;


        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/outer_radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * RF_PI / th) / 360;

        if (segments <= 0) segments = 4;
    }

    // Not a ring
    if (inner_radius <= 0.0f)
    {
        rf_draw_circle_sector(center, outer_radius, start_angle, end_angle, segments, color);
        return;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;
    if (rf_gfx_check_buffer_limit(6*segments)) rf_gfx_draw();

    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*inner_radius, center.y + cosf(RF_DEG2RAD*angle)*inner_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*inner_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*inner_radius);

        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*inner_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*inner_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*outer_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*outer_radius);

        angle += step_length;
    }
    rf_gfx_end();

}

RF_API void rf_draw_ring_lines(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (start_angle == end_angle) return;

    // Function expects (outerRadius > innerRadius)
    if (outer_radius < inner_radius)
    {
        float tmp = outer_radius;
        outer_radius = inner_radius;
        inner_radius = tmp;

        if (outer_radius <= 0.0f) outer_radius = 0.1f;
    }

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/outer_radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * RF_PI / th) / 360;

        if (segments <= 0) segments = 4;
    }

    if (inner_radius <= 0.0f)
    {
        rf_draw_circle_sector_lines(center, outer_radius, start_angle, end_angle, segments, color);
        return;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;

    rf_bool show_cap_lines = true;
    int limit = 4 * (segments + 1);
    if ((end_angle - start_angle)%360 == 0) { limit = 4 * segments; show_cap_lines = false; }

    if (rf_gfx_check_buffer_limit(limit)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*inner_radius, center.y + cosf(RF_DEG2RAD*angle)*inner_radius);
    }

    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*outer_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*outer_radius);

        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*inner_radius, center.y + cosf(RF_DEG2RAD*angle)*inner_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*inner_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*inner_radius);

        angle += step_length;
    }

    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
        rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*inner_radius, center.y + cosf(RF_DEG2RAD*angle)*inner_radius);
    }
    rf_gfx_end();
}

// Draw a color-filled rectangle
RF_API void rf_draw_rectangle(int posX, int posY, int width, int height, rf_color color)
{
    rf_draw_rectangle_v((rf_vec2){ (float)posX, (float)posY }, (rf_vec2){ (float)width, (float)height }, color);
}

// Draw a color-filled rectangle (Vector version)
RF_API void rf_draw_rectangle_v(rf_vec2 position, rf_vec2 size, rf_color color)
{
    rf_draw_rectangle_pro((rf_rec) { position.x, position.y, size.x, size.y }, (rf_vec2){ 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle
RF_API void rf_draw_rectangle_rec(rf_rec rec, rf_color color)
{
    rf_draw_rectangle_pro(rec, (rf_vec2){ 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle with pro parameters
RF_API void rf_draw_rectangle_pro(rf_rec rec, rf_vec2 origin, float rotation, rf_color color)
{
    rf_gfx_enable_texture(rf_get_shapes_texture().id);

    rf_gfx_push_matrix();
    rf_gfx_translatef(rec.x, rec.y, 0.0f);
    rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
    rf_gfx_translatef(-origin.x, -origin.y, 0.0f);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, 0.0f);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, rec.height);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.width, rec.height);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.width, 0.0f);
    rf_gfx_end();
    rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw a vertical-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
RF_API void rf_draw_rectangle_gradient_v(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2)
{
    rf_draw_rectangle_gradient((rf_rec) {(float)pos_x, (float)pos_y, (float)width, (float)height }, color1, color2, color2, color1);
}

// Draw a horizontal-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
RF_API void rf_draw_rectangle_gradient_h(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2)
{
    rf_draw_rectangle_gradient((rf_rec) {(float)pos_x, (float)pos_y, (float)width, (float)height }, color1, color1, color2, color2);
}

// Draw a gradient-filled rectangle
// NOTE: Colors refer to corners, starting at top-lef corner and counter-clockwise
RF_API void rf_draw_rectangle_gradient(rf_rec rec, rf_color col1, rf_color col2, rf_color col3, rf_color col4)
{
    rf_gfx_enable_texture(rf_get_shapes_texture().id);

    rf_gfx_push_matrix();
    rf_gfx_begin(RF_QUADS);
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f);

    // NOTE: Default raylib font character 95 is a white square
    rf_gfx_color4ub(col1.r, col1.g, col1.b, col1.a);
    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x, rec.y);

    rf_gfx_color4ub(col2.r, col2.g, col2.b, col2.a);
    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x, rec.y + rec.height);

    rf_gfx_color4ub(col3.r, col3.g, col3.b, col3.a);
    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x + rec.width, rec.y + rec.height);

    rf_gfx_color4ub(col4.r, col4.g, col4.b, col4.a);
    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x + rec.width, rec.y);
    rf_gfx_end();
    rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw rectangle outline with extended parameters
RF_API void rf_draw_rectangle_outline(rf_rec rec, int line_thick, rf_color color)
{
    if (line_thick > rec.width || line_thick > rec.height)
    {
        if (rec.width > rec.height) line_thick = (int)rec.height/2;
        else if (rec.width < rec.height) line_thick = (int)rec.width/2;
    }

    rf_draw_rectangle_pro((rf_rec) {(int)rec.x, (int)rec.y, (int)rec.width, line_thick }, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
    rf_draw_rectangle_pro((rf_rec) {(int)(rec.x - line_thick + rec.width), (int)(rec.y + line_thick), line_thick, (int)(rec.height - line_thick * 2.0f) }, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
    rf_draw_rectangle_pro((rf_rec) {(int)rec.x, (int)(rec.y + rec.height - line_thick), (int)rec.width, line_thick }, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
    rf_draw_rectangle_pro((rf_rec) {(int)rec.x, (int)(rec.y + line_thick), line_thick, (int)(rec.height - line_thick * 2) }, (rf_vec2) {0.0f, 0.0f }, 0.0f, color);
}

// Draw rectangle with rounded edges
RF_API void rf_draw_rectangle_rounded(rf_rec rec, float roundness, int segments, rf_color color)
{
    // Not a rounded rectangle
    if ((roundness <= 0.0f) || (rec.width < 1) || (rec.height < 1 ))
    {
        rf_draw_rectangle_pro(rec, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
        return;
    }

    if (roundness >= 1.0f) roundness = 1.0f;

    // Calculate corner radius
    float radius = (rec.width > rec.height)? (rec.height*roundness)/2 : (rec.width*roundness)/2;
    if (radius <= 0.0f) return;

    // Calculate number of segments to use for the corners
    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = ceilf(2 * RF_PI / th) / 4;
        if (segments <= 0) segments = 4;
    }

    float step_length = 90.0f/(float)segments;

    /*  Quick sketch to make sense of all of this (there are 9 parts to draw, also mark the 12 points we'll use below)
     *  Not my best attempt at ASCII art, just preted it's rounded rectangle :)
     *     P0                    P1
     *       ____________________
     *     /|                    |\
     *    /1|          2         |3\
     *P7 /__|____________________|__\ P2
     *  /   |                    |  _\ P2
     *  |   |P8                P9|   |
     *  | 8 |          9         | 4 |
     *  | __|____________________|__ |
     *P6 \  |P11              P10|  / P3
     *    \7|          6         |5/
     *     \|____________________|/
     *     P5                    P4
     */

    const rf_vec2 point[12] = { // coordinates of the 12 points that define the rounded rect (the idea here is to make things easier)
            {(float)rec.x + radius, rec.y}, {(float)(rec.x + rec.width) - radius, rec.y}, { rec.x + rec.width, (float)rec.y + radius }, // PO, P1, P2
            {rec.x + rec.width, (float)(rec.y + rec.height) - radius}, {(float)(rec.x + rec.width) - radius, rec.y + rec.height}, // P3, P4
            {(float)rec.x + radius, rec.y + rec.height}, { rec.x, (float)(rec.y + rec.height) - radius}, {rec.x, (float)rec.y + radius}, // P5, P6, P7
            {(float)rec.x + radius, (float)rec.y + radius}, {(float)(rec.x + rec.width) - radius, (float)rec.y + radius}, // P8, P9
            {(float)(rec.x + rec.width) - radius, (float)(rec.y + rec.height) - radius}, {(float)rec.x + radius, (float)(rec.y + rec.height) - radius} // P10, P11
    };

    const rf_vec2 centers[4] = {point[8], point[9], point[10], point[11] };
    const float angles[4] = { 180.0f, 90.0f, 0.0f, 270.0f };
    if (rf_gfx_check_buffer_limit(12*segments + 5*6)) rf_gfx_draw(); // 4 corners with 3 vertices per segment + 5 rectangles with 6 vertices each

    rf_gfx_begin(RF_TRIANGLES);
    // Draw all of the 4 corners: [1] Upper Left Corner, [3] Upper Right Corner, [5] Lower Right Corner, [7] Lower Left Corner
    for (rf_int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
    {
        float angle = angles[k];
        const rf_vec2 center = centers[k];
        for (rf_int i = 0; i < segments; i++)
        {
            rf_gfx_color4ub(color.r, color.g, color.b, color.a);
            rf_gfx_vertex2f(center.x, center.y);
            rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*radius, center.y + cosf(RF_DEG2RAD*angle)*radius);
            rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*radius);
            angle += step_length;
        }
    }

    // [2] Upper rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[0].x, point[0].y);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[1].x, point[1].y);
    rf_gfx_vertex2f(point[0].x, point[0].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);

    // [4] Right rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_vertex2f(point[3].x, point[3].y);
    rf_gfx_vertex2f(point[2].x, point[2].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[3].x, point[3].y);

    // [6] Bottom rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[5].x, point[5].y);
    rf_gfx_vertex2f(point[4].x, point[4].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[4].x, point[4].y);

    // [8] Left rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[7].x, point[7].y);
    rf_gfx_vertex2f(point[6].x, point[6].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[7].x, point[7].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);

    // [9] Middle rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_end();

}

// Draw rectangle with rounded edges outline
RF_API void rf_draw_rectangle_rounded_lines(rf_rec rec, float roundness, int segments, int line_thick, rf_color color)
{
    if (line_thick < 0) line_thick = 0;

    // Not a rounded rectangle
    if (roundness <= 0.0f)
    {
        rf_draw_rectangle_outline((rf_rec) {rec.x - line_thick, rec.y - line_thick, rec.width + 2 * line_thick, rec.height + 2 * line_thick}, line_thick, color);
        return;
    }

    if (roundness >= 1.0f) roundness = 1.0f;

    // Calculate corner radius
    float radius = (rec.width > rec.height)? (rec.height*roundness)/2 : (rec.width*roundness)/2;
    if (radius <= 0.0f) return;

    // Calculate number of segments to use for the corners
    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = ceilf(2 * RF_PI / th) / 2;
        if (segments <= 0) segments = 4;
    }

    float step_length = 90.0f/(float)segments;
    const float outer_radius = radius + (float)line_thick, inner_radius = radius;

    /*  Quick sketch to make sense of all of this (mark the 16 + 4(corner centers P16-19) points we'll use below)
     *  Not my best attempt at ASCII art, just preted it's rounded rectangle :)
     *     P0                     P1
     *        ====================
     *     // P8                P9 \
     *    //                        \
     *P7 // P15                  P10 \\ P2
        \\ P2
     *  ||   *P16             P17*    ||
     *  ||                            ||
     *  || P14                   P11  ||
     *P6 \\  *P19             P18*   // P3
     *    \\                        //
     *     \\ P13              P12 //
     *        ====================
     *     P5                     P4

     */
    const rf_vec2 point[16] =
            {
                    {(float)rec.x + inner_radius, rec.y - line_thick}, {(float)(rec.x + rec.width) - inner_radius, rec.y - line_thick}, { rec.x + rec.width + line_thick, (float)rec.y + inner_radius }, // PO, P1, P2
                    {rec.x + rec.width + line_thick, (float)(rec.y + rec.height) - inner_radius}, {(float)(rec.x + rec.width) - inner_radius, rec.y + rec.height + line_thick}, // P3, P4
                    {(float)rec.x + inner_radius, rec.y + rec.height + line_thick}, { rec.x - line_thick, (float)(rec.y + rec.height) - inner_radius}, {rec.x - line_thick, (float)rec.y + inner_radius}, // P5, P6, P7
                    {(float)rec.x + inner_radius, rec.y}, {(float)(rec.x + rec.width) - inner_radius, rec.y}, // P8, P9
                    { rec.x + rec.width, (float)rec.y + inner_radius }, {rec.x + rec.width, (float)(rec.y + rec.height) - inner_radius}, // P10, P11
                    {(float)(rec.x + rec.width) - inner_radius, rec.y + rec.height}, {(float)rec.x + inner_radius, rec.y + rec.height}, // P12, P13
                    { rec.x, (float)(rec.y + rec.height) - inner_radius}, {rec.x, (float)rec.y + inner_radius} // P14, P15
            };

    const rf_vec2 centers[4] =
            {
                    {(float)rec.x + inner_radius, (float)rec.y + inner_radius}, {(float)(rec.x + rec.width) - inner_radius, (float)rec.y + inner_radius}, // P16, P17
                    {(float)(rec.x + rec.width) - inner_radius, (float)(rec.y + rec.height) - inner_radius}, {(float)rec.x + inner_radius, (float)(rec.y + rec.height) - inner_radius} // P18, P19
            };

    const float angles[4] = { 180.0f, 90.0f, 0.0f, 270.0f };

    if (line_thick > 1)
    {
        if (rf_gfx_check_buffer_limit(4 * 6*segments + 4 * 6)) rf_gfx_draw(); // 4 corners with 6(2 * 3) vertices for each segment + 4 rectangles with 6 vertices each

        rf_gfx_begin(RF_TRIANGLES);

        // Draw all of the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (rf_int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const rf_vec2 center = centers[k];

            for (rf_int i = 0; i < segments; i++)
            {
                rf_gfx_color4ub(color.r, color.g, color.b, color.a);

                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*inner_radius, center.y + cosf(RF_DEG2RAD*angle)*inner_radius);
                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*inner_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*inner_radius);

                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*inner_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*inner_radius);
                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*outer_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*outer_radius);

                angle += step_length;
            }
        }

        // Upper rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[0].x, point[0].y);
        rf_gfx_vertex2f(point[8].x, point[8].y);
        rf_gfx_vertex2f(point[9].x, point[9].y);
        rf_gfx_vertex2f(point[1].x, point[1].y);
        rf_gfx_vertex2f(point[0].x, point[0].y);
        rf_gfx_vertex2f(point[9].x, point[9].y);

        // Right rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[10].x, point[10].y);
        rf_gfx_vertex2f(point[11].x, point[11].y);
        rf_gfx_vertex2f(point[3].x, point[3].y);
        rf_gfx_vertex2f(point[2].x, point[2].y);
        rf_gfx_vertex2f(point[10].x, point[10].y);
        rf_gfx_vertex2f(point[3].x, point[3].y);

        // Lower rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[13].x, point[13].y);
        rf_gfx_vertex2f(point[5].x, point[5].y);
        rf_gfx_vertex2f(point[4].x, point[4].y);
        rf_gfx_vertex2f(point[12].x, point[12].y);
        rf_gfx_vertex2f(point[13].x, point[13].y);
        rf_gfx_vertex2f(point[4].x, point[4].y);

        // Left rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[7].x, point[7].y);
        rf_gfx_vertex2f(point[6].x, point[6].y);
        rf_gfx_vertex2f(point[14].x, point[14].y);
        rf_gfx_vertex2f(point[15].x, point[15].y);
        rf_gfx_vertex2f(point[7].x, point[7].y);
        rf_gfx_vertex2f(point[14].x, point[14].y);
        rf_gfx_end();

    }
    else
    {
        // Use LINES to draw the outline
        if (rf_gfx_check_buffer_limit(8*segments + 4 * 2)) rf_gfx_draw(); // 4 corners with 2 vertices for each segment + 4 rectangles with 2 vertices each

        rf_gfx_begin(RF_LINES);

        // Draw all of the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (rf_int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const rf_vec2 center = centers[k];

            for (rf_int i = 0; i < segments; i++)
            {
                rf_gfx_color4ub(color.r, color.g, color.b, color.a);
                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*angle)*outer_radius, center.y + cosf(RF_DEG2RAD*angle)*outer_radius);
                rf_gfx_vertex2f(center.x + sinf(RF_DEG2RAD*(angle + step_length))*outer_radius, center.y + cosf(RF_DEG2RAD*(angle + step_length))*outer_radius);
                angle += step_length;
            }
        }
        // And now the remaining 4 lines
        for(int i = 0; i < 8; i += 2)
        {
            rf_gfx_color4ub(color.r, color.g, color.b, color.a);
            rf_gfx_vertex2f(point[i].x, point[i].y);
            rf_gfx_vertex2f(point[i + 1].x, point[i + 1].y);
        }
        rf_gfx_end();
    }
}

// Draw a triangle
// NOTE: Vertex must be provided in counter-clockwise order
RF_API void rf_draw_triangle(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color)
{
    if (rf_gfx_check_buffer_limit(4)) rf_gfx_draw();
    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(v1.x, v1.y);
    rf_gfx_vertex2f(v2.x, v2.y);
    rf_gfx_vertex2f(v3.x, v3.y);
    rf_gfx_end();

}

// Draw a triangle using lines
// NOTE: Vertex must be provided in counter-clockwise order
RF_API void rf_draw_triangle_lines(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color)
{
    if (rf_gfx_check_buffer_limit(6)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(v1.x, v1.y);
    rf_gfx_vertex2f(v2.x, v2.y);

    rf_gfx_vertex2f(v2.x, v2.y);
    rf_gfx_vertex2f(v3.x, v3.y);

    rf_gfx_vertex2f(v3.x, v3.y);
    rf_gfx_vertex2f(v1.x, v1.y);
    rf_gfx_end();
}

// Draw a triangle fan defined by points
// NOTE: First vertex provided is the center, shared by all triangles
RF_API void rf_draw_triangle_fan(rf_vec2 *points, int points_count, rf_color color)
{
    if (points_count >= 3)
    {
        if (rf_gfx_check_buffer_limit((points_count - 2) * 4)) rf_gfx_draw();

        rf_gfx_enable_texture(rf_get_shapes_texture().id);
        rf_gfx_begin(RF_QUADS);
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        for (rf_int i = 1; i < points_count - 1; i++)
        {
            rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[0].x, points[0].y);

            rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[i].x, points[i].y);

            rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[i + 1].x, points[i + 1].y);

            rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[i + 1].x, points[i + 1].y);
        }
        rf_gfx_end();
        rf_gfx_disable_texture();
    }
}

// Draw a triangle strip defined by points
// NOTE: Every new vertex connects with previous two
RF_API void rf_draw_triangle_strip(rf_vec2 *points, int points_count, rf_color color)
{
    if (points_count >= 3)
    {
        if (rf_gfx_check_buffer_limit(points_count)) rf_gfx_draw();

        rf_gfx_begin(RF_TRIANGLES);
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        for (rf_int i = 2; i < points_count; i++)
        {
            if ((i%2) == 0)
            {
                rf_gfx_vertex2f(points[i].x, points[i].y);
                rf_gfx_vertex2f(points[i - 2].x, points[i - 2].y);
                rf_gfx_vertex2f(points[i - 1].x, points[i - 1].y);
            }
            else
            {
                rf_gfx_vertex2f(points[i].x, points[i].y);
                rf_gfx_vertex2f(points[i - 1].x, points[i - 1].y);
                rf_gfx_vertex2f(points[i - 2].x, points[i - 2].y);
            }
        }
        rf_gfx_end();
    }
}

// Draw a regular polygon of n sides (Vector version)
RF_API void rf_draw_poly(rf_vec2 center, int sides, float radius, float rotation, rf_color color)
{
    if (sides < 3) sides = 3;
    float centralAngle = 0.0f;

    if (rf_gfx_check_buffer_limit(4 * (360/sides))) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(center.x, center.y, 0.0f);
    rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < sides; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(0, 0);
        rf_gfx_vertex2f(sinf(RF_DEG2RAD*centralAngle)*radius, cosf(RF_DEG2RAD*centralAngle)*radius);

        centralAngle += 360.0f/(float)sides;
        rf_gfx_vertex2f(sinf(RF_DEG2RAD*centralAngle)*radius, cosf(RF_DEG2RAD*centralAngle)*radius);
    }
    rf_gfx_end();

    rf_gfx_pop_matrix();
}

// Draw a rf_texture2d with extended parameters
RF_API void rf_draw_texture(rf_texture2d texture, int x, int y, rf_color tint)
{
    rf_draw_texture_ex(texture, x, y, texture.width, texture.height, 0, tint);
}

// Draw a rf_texture2d with extended parameters
RF_API void rf_draw_texture_ex(rf_texture2d texture, int x, int y, int w, int h, float rotation, rf_color tint)
{
    rf_rec source_rec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    rf_rec dest_rec = { x, y, w, h };
    rf_vec2 origin = { 0.0f, 0.0f };

    rf_draw_texture_region(texture, source_rec, dest_rec, origin, rotation, tint);
}

// Draw a part of a texture (defined by a rectangle) with 'pro' parameters. Note: origin is relative to destination rectangle size
RF_API void rf_draw_texture_region(rf_texture2d texture, rf_rec source_rec, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint)
{
    // Check if texture is valid
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        rf_bool flip_x = false;

        if (source_rec.width < 0) { flip_x = true; source_rec.width *= -1; }
        if (source_rec.height < 0) source_rec.y -= source_rec.height;

        rf_gfx_enable_texture(texture.id);

        rf_gfx_push_matrix();
            rf_gfx_translatef(dest_rec.x, dest_rec.y, 0.0f);
            rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
            rf_gfx_translatef(-origin.x, -origin.y, 0.0f);

            rf_gfx_begin(RF_QUADS);
                rf_gfx_color4ub(tint.r, tint.g, tint.b, tint.a);
                rf_gfx_normal3f(0.0f, 0.0f, 1.0f); // Normal vector pointing towards viewer

                // Bottom-left corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, source_rec.y / height);
                else rf_gfx_tex_coord2f(source_rec.x / width, source_rec.y / height);
                rf_gfx_vertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, (source_rec.y + source_rec.height) / height);
                else rf_gfx_tex_coord2f(source_rec.x / width, (source_rec.y + source_rec.height) / height);
                rf_gfx_vertex2f(0.0f, dest_rec.height);

                // Top-right corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f(source_rec.x / width, (source_rec.y + source_rec.height) / height);
                else rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, (source_rec.y + source_rec.height) / height);
                rf_gfx_vertex2f(dest_rec.width, dest_rec.height);

                // Top-left corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f(source_rec.x / width, source_rec.y / height);
                else rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, source_rec.y / height);
                rf_gfx_vertex2f(dest_rec.width, 0.0f);
            rf_gfx_end();
        rf_gfx_pop_matrix();

        rf_gfx_disable_texture();
    }
}

// Draws a texture (or part of it) that stretches or shrinks nicely using n-patch info
RF_API void rf_draw_texture_npatch(rf_texture2d texture, rf_npatch_info n_patch_info, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint)
{
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        float patch_width = (dest_rec.width <= 0.0f)? 0.0f : dest_rec.width;
        float patch_height = (dest_rec.height <= 0.0f)? 0.0f : dest_rec.height;

        if (n_patch_info.source_rec.width < 0) n_patch_info.source_rec.x -= n_patch_info.source_rec.width;
        if (n_patch_info.source_rec.height < 0) n_patch_info.source_rec.y -= n_patch_info.source_rec.height;
        if (n_patch_info.type == RF_NPT_3PATCH_HORIZONTAL) patch_height = n_patch_info.source_rec.height;
        if (n_patch_info.type == RF_NPT_3PATCH_VERTICAL) patch_width = n_patch_info.source_rec.width;

        rf_bool draw_center = true;
        rf_bool draw_middle = true;
        float left_border = (float)n_patch_info.left;
        float top_border = (float)n_patch_info.top;
        float right_border = (float)n_patch_info.right;
        float bottom_border = (float)n_patch_info.bottom;

        // adjust the lateral (left and right) border widths in case patch_width < texture.width
        if (patch_width <= (left_border + right_border) && n_patch_info.type != RF_NPT_3PATCH_VERTICAL)
        {
            draw_center = false;
            left_border = (left_border / (left_border + right_border)) * patch_width;
            right_border = patch_width - left_border;
        }
        // adjust the lateral (top and bottom) border heights in case patch_height < texture.height
        if (patch_height <= (top_border + bottom_border) && n_patch_info.type != RF_NPT_3PATCH_HORIZONTAL)
        {
            draw_middle = false;
            top_border = (top_border / (top_border + bottom_border)) * patch_height;
            bottom_border = patch_height - top_border;
        }

        rf_vec2 vert_a, vert_b, vert_c, vert_d;
        vert_a.x = 0.0f; // outer left
        vert_a.y = 0.0f; // outer top
        vert_b.x = left_border; // inner left
        vert_b.y = top_border; // inner top
        vert_c.x = patch_width - right_border; // inner right
        vert_c.y = patch_height - bottom_border; // inner bottom
        vert_d.x = patch_width; // outer right
        vert_d.y = patch_height; // outer bottom

        rf_vec2 coord_a, coord_b, coord_c, coord_d;
        coord_a.x = n_patch_info.source_rec.x / width;
        coord_a.y = n_patch_info.source_rec.y / height;
        coord_b.x = (n_patch_info.source_rec.x + left_border) / width;
        coord_b.y = (n_patch_info.source_rec.y + top_border) / height;
        coord_c.x = (n_patch_info.source_rec.x + n_patch_info.source_rec.width - right_border) / width;
        coord_c.y = (n_patch_info.source_rec.y + n_patch_info.source_rec.height - bottom_border) / height;
        coord_d.x = (n_patch_info.source_rec.x + n_patch_info.source_rec.width) / width;
        coord_d.y = (n_patch_info.source_rec.y + n_patch_info.source_rec.height) / height;

        rf_gfx_enable_texture(texture.id);

        rf_gfx_push_matrix();
        rf_gfx_translatef(dest_rec.x, dest_rec.y, 0.0f);
        rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
        rf_gfx_translatef(-origin.x, -origin.y, 0.0f);

        rf_gfx_begin(RF_QUADS);
        rf_gfx_color4ub(tint.r, tint.g, tint.b, tint.a);
        rf_gfx_normal3f(0.0f, 0.0f, 1.0f); // Normal vector pointing towards viewer

        if (n_patch_info.type == RF_NPT_9PATCH)
        {
            // ------------------------------------------------------------
            // TOP-LEFT QUAD
            rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_a.y); rf_gfx_vertex2f(vert_a.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // TOP-CENTER QUAD
                rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-left corner for texture and quad
            }
            // TOP-RIGHT QUAD
            rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_a.y); rf_gfx_vertex2f(vert_d.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_middle)
            {
                // ------------------------------------------------------------
                // MIDDLE-LEFT QUAD
                rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Top-left corner for texture and quad
                if (draw_center)
                {
                    // MIDDLE-CENTER QUAD
                    rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Bottom-left corner for texture and quad
                    rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Bottom-right corner for texture and quad
                    rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Top-right corner for texture and quad
                    rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Top-left corner for texture and quad
                }

                // MIDDLE-RIGHT QUAD
                rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Top-left corner for texture and quad
            }

            // ------------------------------------------------------------
            // BOTTOM-LEFT QUAD
            rf_gfx_tex_coord2f(coord_a.x, coord_d.y); rf_gfx_vertex2f(vert_a.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // BOTTOM-CENTER QUAD
                rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Top-left corner for texture and quad
            }

            // BOTTOM-RIGHT QUAD
            rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_d.y); rf_gfx_vertex2f(vert_d.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Top-left corner for texture and quad
        }
        else if (n_patch_info.type == RF_NPT_3PATCH_VERTICAL)
        {
            // TOP QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_a.y); rf_gfx_vertex2f(vert_d.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_a.y); rf_gfx_vertex2f(vert_a.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // MIDDLE QUAD
                // -----------------------------------------------------------
                // rf_texture coords                 Vertices
                rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Top-left corner for texture and quad
            }
            // BOTTOM QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_a.x, coord_d.y); rf_gfx_vertex2f(vert_a.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_d.y); rf_gfx_vertex2f(vert_d.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Top-left corner for texture and quad
        }
        else if (n_patch_info.type == RF_NPT_3PATCH_HORIZONTAL)
        {
            // LEFT QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_a.x, coord_d.y); rf_gfx_vertex2f(vert_a.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_a.y); rf_gfx_vertex2f(vert_a.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // CENTER QUAD
                // -----------------------------------------------------------
                // rf_texture coords                 Vertices
                rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-left corner for texture and quad
            }
            // RIGHT QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_d.y); rf_gfx_vertex2f(vert_d.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_a.y); rf_gfx_vertex2f(vert_d.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-left corner for texture and quad
        }
        rf_gfx_end();
        rf_gfx_pop_matrix();

        rf_gfx_disable_texture();

    }
}

// Draw text (using default font)
RF_API void rf_draw_string(const char* text, int text_len, int posX, int posY, int fontSize, rf_color color)
{
    // Check if default font has been loaded
    if (rf_get_default_font().texture.id == 0 || text_len == 0) return;

    rf_vec2 position = { (float)posX, (float)posY };

    int defaultFontSize = 10;   // Default Font chars height in pixel
    if (fontSize < defaultFontSize) fontSize = defaultFontSize;
    int spacing = fontSize/defaultFontSize;

    rf_draw_string_ex(rf_get_default_font(), text, text_len, position, (float)fontSize, (float)spacing, color);
}

// Draw text with custom font
RF_API void rf_draw_string_ex(rf_font font, const char* text, int text_len, rf_vec2 position, float font_size, float spacing, rf_color tint)
{
    int text_offset_y = 0; // Required for line break!
    float text_offset_x = 0.0f; // Offset between characters
    float scale_factor = 0.0f;

    int letter = 0; // Current character
    int index  = 0; // Index position in sprite font

    scale_factor = font_size / font.base_size;

    for (rf_int i = 0; i < text_len; i++)
    {
        rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], text_len - i);
        letter = decoded_rune.codepoint;
        index = rf_get_glyph_index(font, letter);

        // NOTE: Normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set 'next = 1'
        if (letter == 0x3f) decoded_rune.bytes_processed = 1;
        i += (decoded_rune.bytes_processed - 1);

        if (letter == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 lines
            text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
            text_offset_x = 0.0f;
        }
        else
        {
            if (letter != ' ')
            {
                rf_rec src_rec = font.glyphs[index].rec;
                rf_rec dst_rec = {  position.x + text_offset_x + font.glyphs[index].offset_x * scale_factor,
                                    position.y + text_offset_y + font.glyphs[index].offset_y * scale_factor,
                                    font.glyphs[index].rec.width  * scale_factor,
                                    font.glyphs[index].rec.height * scale_factor };
                rf_draw_texture_region(font.texture, src_rec, dst_rec, (rf_vec2){0}, 0.0f, tint);
            }

            if (font.glyphs[index].advance_x == 0) text_offset_x += ((float)font.glyphs[index].rec.width * scale_factor + spacing);
            else text_offset_x += ((float)font.glyphs[index].advance_x * scale_factor + spacing);
        }
    }
}

// Draw text wrapped
RF_API void rf_draw_string_wrap(rf_font font, const char* text, int text_len, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode)
{
    rf_rec rec = { 0, 0, wrap_width, FLT_MAX };
    rf_draw_string_rec(font, text, text_len, rec, font_size, spacing, mode, tint);
}

// Draw text using font inside rectangle limits
RF_API void rf_draw_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint)
{
    int   text_offset_x = 0; // Offset between characters
    int   text_offset_y = 0; // Required for line break!
    float scale_factor  = 0.0f;

    int letter = 0; // Current character
    int index  = 0; // Index position in sprite font

    scale_factor = font_size/font.base_size;

    enum
    {
        MEASURE_WORD_WRAP_STATE = 0,
        MEASURE_RESULT_STATE = 1
    };

    int state      = wrap == RF_WORD_WRAP ? MEASURE_WORD_WRAP_STATE : MEASURE_RESULT_STATE;
    int start_line = -1; // Index where to begin drawing (where a line begins)
    int end_line   = -1; // Index where to stop drawing (where a line ends)
    int lastk      = -1; // Holds last value of the character position

    for (rf_int i = 0, k = 0; i < text_len; i++, k++)
    {
        int glyph_width = 0;

        rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], text_len - i);
        letter = decoded_rune.codepoint;
        index  = rf_get_glyph_index(font, letter);

        // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) decoded_rune.bytes_processed = 1;
        i += decoded_rune.bytes_processed - 1;

        if (letter != '\n')
        {
            glyph_width = (font.glyphs[index].advance_x == 0)?
                          (int)(font.glyphs[index].rec.width * scale_factor + spacing):
                          (int)(font.glyphs[index].advance_x * scale_factor + spacing);
        }

        // NOTE: When wrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in start_line and end_line, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_WORD_WRAP_STATE)
        {
            // TODO: there are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // See: http://jkorpela.fi/chars/spaces.html
            if ((letter == ' ') || (letter == '\t') || (letter == '\n')) end_line = i;

            if ((text_offset_x + glyph_width + 1) >= rec.width)
            {
                end_line = (end_line < 1)? i : end_line;
                if (i == end_line) end_line -= decoded_rune.bytes_processed;
                if ((start_line + decoded_rune.bytes_processed) == end_line) end_line = i - decoded_rune.bytes_processed;
                state = !state;
            }
            else if ((i + 1) == text_len)
            {
                end_line = i;
                state = !state;
            }
            else if (letter == '\n')
            {
                state = !state;
            }

            if (state == MEASURE_RESULT_STATE)
            {
                text_offset_x = 0;
                i = start_line;
                glyph_width = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (letter == '\n')
            {
                if (!wrap)
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                }
            }
            else
            {
                if (!wrap && ((text_offset_x + glyph_width + 1) >= rec.width))
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                }

                if ((text_offset_y + (int)(font.base_size*scale_factor)) > rec.height) break;

                // Draw glyph
                if ((letter != ' ') && (letter != '\t'))
                {
                    rf_draw_texture_region(font.texture, font.glyphs[index].rec,
                                           (rf_rec) {
                                               rec.x + text_offset_x + font.glyphs[index].offset_x * scale_factor,
                                               rec.y + text_offset_y + font.glyphs[index].offset_y * scale_factor,
                                               font.glyphs[index].rec.width  * scale_factor,
                                               font.glyphs[index].rec.height * scale_factor
                                           },
                                           (rf_vec2){ 0, 0 }, 0.0f, tint);
                }
            }

            if (wrap && (i == end_line))
            {
                text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                text_offset_x = 0;
                start_line = end_line;
                end_line = -1;
                glyph_width = 0;
                k = lastk;
                state = !state;
            }
        }

        text_offset_x += glyph_width;
    }
}

RF_API void rf_draw_text(const char* text, int posX, int posY, int font_size, rf_color color) { rf_draw_string(text, strlen(text), posX, posY, font_size, color); }

RF_API void rf_draw_text_ex(rf_font font, const char* text, rf_vec2 position, float fontSize, float spacing, rf_color tint) { rf_draw_string_ex(font, text, strlen(text), position, fontSize, spacing, tint); }

RF_API void rf_draw_text_wrap(rf_font font, const char* text, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode) { rf_draw_string_wrap(font, text, strlen(text), position, font_size, spacing, tint, wrap_width, mode); }

RF_API void rf_draw_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint) { rf_draw_string_rec(font, text, strlen(text), rec, font_size, spacing, wrap, tint); }

RF_API void rf_draw_line3d(rf_vec3 start_pos, rf_vec3 end_pos, rf_color color)
{
    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex3f(start_pos.x, start_pos.y, start_pos.z);
    rf_gfx_vertex3f(end_pos.x, end_pos.y, end_pos.z);
    rf_gfx_end();
}

// Draw a circle in 3D world space
RF_API void rf_draw_circle3d(rf_vec3 center, float radius, rf_vec3 rotation_axis, float rotationAngle, rf_color color)
{
    if (rf_gfx_check_buffer_limit(2 * 36)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(center.x, center.y, center.z);
    rf_gfx_rotatef(rotationAngle, rotation_axis.x, rotation_axis.y, rotation_axis.z);

    rf_gfx_begin(RF_LINES);
    for (rf_int i = 0; i < 360; i += 10)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius, cosf(RF_DEG2RAD*i)*radius, 0.0f);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 10))*radius, cosf(RF_DEG2RAD*(i + 10))*radius, 0.0f);
    }
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw cube
// NOTE: Cube position is the center position
RF_API void rf_draw_cube(rf_vec3 position, float width, float height, float length, rf_color color)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    if (rf_gfx_check_buffer_limit(36)) rf_gfx_draw();

    rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
    rf_gfx_translatef(position.x, position.y, position.z);
    //rf_gfx_rotatef(45, 0, 1, 0);
    //rf_gfx_scalef(1.0f, 1.0f, 1.0f);   // NOTE: Vertices are directly scaled on definition

    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    // Front face
    rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left

    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right

    // Back face
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right

    rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left

    // Top face
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Bottom Right

    rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Bottom Right

    // Bottom face
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left

    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Top Left

    // Right face
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Left

    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Left

    // Left face
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Right

    rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw cube wires
RF_API void rf_draw_cube_wires(rf_vec3 position, float width, float height, float length, rf_color color)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    if (rf_gfx_check_buffer_limit(36)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    // Front Face -----------------------------------------------------
    // Bottom Line
    rf_gfx_vertex3f(x-width/2, y-height/2, z+length/2); // Bottom Left
    rf_gfx_vertex3f(x+width/2, y-height/2, z+length/2); // Bottom Right

    // Left Line
    rf_gfx_vertex3f(x+width/2, y-height/2, z+length/2); // Bottom Right
    rf_gfx_vertex3f(x+width/2, y+height/2, z+length/2); // Top Right

    // Top Line
    rf_gfx_vertex3f(x+width/2, y+height/2, z+length/2); // Top Right
    rf_gfx_vertex3f(x-width/2, y+height/2, z+length/2); // Top Left

    // Right Line
    rf_gfx_vertex3f(x-width/2, y+height/2, z+length/2); // Top Left
    rf_gfx_vertex3f(x-width/2, y-height/2, z+length/2); // Bottom Left

    // Back Face ------------------------------------------------------
    // Bottom Line
    rf_gfx_vertex3f(x-width/2, y-height/2, z-length/2); // Bottom Left
    rf_gfx_vertex3f(x+width/2, y-height/2, z-length/2); // Bottom Right

    // Left Line
    rf_gfx_vertex3f(x+width/2, y-height/2, z-length/2); // Bottom Right
    rf_gfx_vertex3f(x+width/2, y+height/2, z-length/2); // Top Right

    // Top Line
    rf_gfx_vertex3f(x+width/2, y+height/2, z-length/2); // Top Right
    rf_gfx_vertex3f(x-width/2, y+height/2, z-length/2); // Top Left

    // Right Line
    rf_gfx_vertex3f(x-width/2, y+height/2, z-length/2); // Top Left
    rf_gfx_vertex3f(x-width/2, y-height/2, z-length/2); // Bottom Left

    // Top Face -------------------------------------------------------
    // Left Line
    rf_gfx_vertex3f(x-width/2, y+height/2, z+length/2); // Top Left Front
    rf_gfx_vertex3f(x-width/2, y+height/2, z-length/2); // Top Left Back

    // Right Line
    rf_gfx_vertex3f(x+width/2, y+height/2, z+length/2); // Top Right Front
    rf_gfx_vertex3f(x+width/2, y+height/2, z-length/2); // Top Right Back

    // Bottom Face  ---------------------------------------------------
    // Left Line
    rf_gfx_vertex3f(x-width/2, y-height/2, z+length/2); // Top Left Front
    rf_gfx_vertex3f(x-width/2, y-height/2, z-length/2); // Top Left Back

    // Right Line
    rf_gfx_vertex3f(x+width/2, y-height/2, z+length/2); // Top Right Front
    rf_gfx_vertex3f(x+width/2, y-height/2, z-length/2); // Top Right Back
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw cube
// NOTE: Cube position is the center position
RF_API void rf_draw_cube_texture(rf_texture2d texture, rf_vec3 position, float width, float height, float length, rf_color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    if (rf_gfx_check_buffer_limit(36)) rf_gfx_draw();

    rf_gfx_enable_texture(texture.id);

    //rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
    //rf_gfx_translatef(2.0f, 0.0f, 0.0f);
    //rf_gfx_rotatef(45, 0, 1, 0);
    //rf_gfx_scalef(2.0f, 2.0f, 2.0f);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    // Front Face
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left Of The rf_texture and Quad
    // Back Face
    rf_gfx_normal3f(0.0f, 0.0f, - 1.0f); // Normal Pointing Away From Viewer
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Left Of The rf_texture and Quad
    // Top Face
    rf_gfx_normal3f(0.0f, 1.0f, 0.0f); // Normal Pointing Up
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right Of The rf_texture and Quad
    // Bottom Face
    rf_gfx_normal3f(0.0f, - 1.0f, 0.0f); // Normal Pointing Down
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    // Right face
    rf_gfx_normal3f(1.0f, 0.0f, 0.0f); // Normal Pointing Right
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    // Left Face
    rf_gfx_normal3f(-1.0f, 0.0f, 0.0f); // Normal Pointing Left
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_end();
    //rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw sphere
RF_API void rf_draw_sphere(rf_vec3 center_pos, float radius, rf_color color)
{
    rf_draw_sphere_ex(center_pos, radius, 16, 16, color);
}

// Draw sphere with extended parameters
RF_API void rf_draw_sphere_ex(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color)
{
    int num_vertex = (rings + 2)*slices*6;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> translate)
    rf_gfx_translatef(center_pos.x, center_pos.y, center_pos.z);
    rf_gfx_scalef(radius, radius, radius);

    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    for (rf_int i = 0; i < (rings + 2); i++)
    {
        for (rf_int j = 0; j < slices; j++)
        {
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*i)),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*cosf(RF_DEG2RAD*(j * 360/slices)));
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*((j+1) * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*((j+1) * 360/slices)));
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*(j * 360/slices)));

            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*i)),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*cosf(RF_DEG2RAD*(j * 360/slices)));
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i)))*sinf(RF_DEG2RAD*((j+1) * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i)))*cosf(RF_DEG2RAD*((j+1) * 360/slices)));
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*((j+1) * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*((j+1) * 360/slices)));
        }
    }
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw sphere wires
RF_API void rf_draw_sphere_wires(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color)
{
    int num_vertex = (rings + 2)*slices*6;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> translate)
    rf_gfx_translatef(center_pos.x, center_pos.y, center_pos.z);
    rf_gfx_scalef(radius, radius, radius);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    for (rf_int i = 0; i < (rings + 2); i++)
    {
        for (rf_int j = 0; j < slices; j++)
        {
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*i)),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*cosf(RF_DEG2RAD*(j * 360/slices)));
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*((j+1) * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*((j+1) * 360/slices)));

            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*((j+1) * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*((j+1) * 360/slices)));
            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*(j * 360/slices)));

            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1))),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*(i+1)))*cosf(RF_DEG2RAD*(j * 360/slices)));

            rf_gfx_vertex3f(cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*sinf(RF_DEG2RAD*(j * 360/slices)),
                           sinf(RF_DEG2RAD*(270+(180/(rings + 1))*i)),
                           cosf(RF_DEG2RAD*(270+(180/(rings + 1))*i))*cosf(RF_DEG2RAD*(j * 360/slices)));
        }
    }

    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a cylinder
// NOTE: It could be also used for pyramid and cone
RF_API void rf_draw_cylinder(rf_vec3 position, float radius_top, float radius_bottom, float height, int sides, rf_color color)
{
    if (sides < 3) sides = 3;

    int num_vertex = sides*6;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);

    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    if (radius_top > 0)
    {
        // Draw Body -------------------------------------------------------------------------------------
        for (rf_int i = 0; i < 360; i += 360 / sides)
        {
            rf_gfx_vertex3f(sinf(RF_DEG2RAD * i) * radius_bottom, 0, cosf(RF_DEG2RAD * i) * radius_bottom);                                   // Bottom Left
            rf_gfx_vertex3f(sinf(RF_DEG2RAD * (i + 360 / sides)) * radius_bottom, 0,   cosf(RF_DEG2RAD * (i + 360 / sides)) * radius_bottom); // Bottom Right
            rf_gfx_vertex3f(sinf(RF_DEG2RAD * (i + 360 / sides)) * radius_top, height, cosf(RF_DEG2RAD * (i + 360 / sides)) * radius_top);    // Top Right

            rf_gfx_vertex3f(sinf(RF_DEG2RAD * i) * radius_top, height, cosf(RF_DEG2RAD * i) * radius_top);                                 // Top Left
            rf_gfx_vertex3f(sinf(RF_DEG2RAD * i) * radius_bottom, 0,   cosf(RF_DEG2RAD * i) * radius_bottom);                              // Bottom Left
            rf_gfx_vertex3f(sinf(RF_DEG2RAD * (i + 360 / sides)) * radius_top, height, cosf(RF_DEG2RAD * (i + 360 / sides)) * radius_top); // Top Right
        }

        // Draw Cap --------------------------------------------------------------------------------------
        for (rf_int i = 0; i < 360; i += 360/sides)
        {
            rf_gfx_vertex3f(0, height, 0);
            rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_top, height, cosf(RF_DEG2RAD*i)*radius_top);
            rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_top, height, cosf(RF_DEG2RAD*(i + 360/sides))*radius_top);
        }
    }
    else
    {
        // Draw Cone -------------------------------------------------------------------------------------
        for (rf_int i = 0; i < 360; i += 360/sides)
        {
            rf_gfx_vertex3f(0, height, 0);
            rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_bottom, 0, cosf(RF_DEG2RAD*i)*radius_bottom);
            rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_bottom, 0, cosf(RF_DEG2RAD*(i + 360/sides))*radius_bottom);
        }
    }

    // Draw Base -----------------------------------------------------------------------------------------
    for (rf_int i = 0; i < 360; i += 360/sides)
    {
        rf_gfx_vertex3f(0, 0, 0);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_bottom, 0, cosf(RF_DEG2RAD*(i + 360/sides))*radius_bottom);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_bottom, 0, cosf(RF_DEG2RAD*i)*radius_bottom);
    }

    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a wired cylinder
// NOTE: It could be also used for pyramid and cone
RF_API void rf_draw_cylinder_wires(rf_vec3 position, float radius_top, float radius_bottom, float height, int sides, rf_color color)
{
    if (sides < 3) sides = 3;

    int num_vertex = sides*8;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    for (rf_int i = 0; i < 360; i += 360/sides)
    {
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_bottom, 0, cosf(RF_DEG2RAD*i)*radius_bottom);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_bottom, 0, cosf(RF_DEG2RAD*(i + 360/sides))*radius_bottom);

        rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_bottom, 0, cosf(RF_DEG2RAD*(i + 360/sides))*radius_bottom);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_top, height, cosf(RF_DEG2RAD*(i + 360/sides))*radius_top);

        rf_gfx_vertex3f(sinf(RF_DEG2RAD*(i + 360/sides))*radius_top, height, cosf(RF_DEG2RAD*(i + 360/sides))*radius_top);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_top, height, cosf(RF_DEG2RAD*i)*radius_top);

        rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_top, height, cosf(RF_DEG2RAD*i)*radius_top);
        rf_gfx_vertex3f(sinf(RF_DEG2RAD*i)*radius_bottom, 0, cosf(RF_DEG2RAD*i)*radius_bottom);
    }
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a plane
RF_API void rf_draw_plane(rf_vec3 center_pos, rf_vec2 size, rf_color color)
{
    if (rf_gfx_check_buffer_limit(4)) rf_gfx_draw();

    // NOTE: Plane is always created on XZ ground
    rf_gfx_push_matrix();
    rf_gfx_translatef(center_pos.x, center_pos.y, center_pos.z);
    rf_gfx_scalef(size.x, 1.0f, size.y);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_normal3f(0.0f, 1.0f, 0.0f);

    rf_gfx_vertex3f(-0.5f, 0.0f, -0.5f);
    rf_gfx_vertex3f(-0.5f, 0.0f, 0.5f);
    rf_gfx_vertex3f(0.5f, 0.0f, 0.5f);
    rf_gfx_vertex3f(0.5f, 0.0f, -0.5f);
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a ray line
RF_API void rf_draw_ray(rf_ray ray, rf_color color)
{
    float scale = 10000;

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    rf_gfx_vertex3f(ray.position.x, ray.position.y, ray.position.z);
    rf_gfx_vertex3f(ray.position.x + ray.direction.x*scale, ray.position.y + ray.direction.y*scale, ray.position.z + ray.direction.z*scale);
    rf_gfx_end();
}

// Draw a grid centered at (0, 0, 0)
RF_API void rf_draw_grid(int slices, float spacing)
{
    int half_slices = slices/2;

    if (rf_gfx_check_buffer_limit(slices * 4)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    for (rf_int i = -half_slices; i <= half_slices; i++)
    {
        if (i == 0)
        {
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
        }

        rf_gfx_vertex3f((float)i*spacing, 0.0f, (float)-half_slices*spacing);
        rf_gfx_vertex3f((float)i*spacing, 0.0f, (float)half_slices*spacing);

        rf_gfx_vertex3f((float)-half_slices*spacing, 0.0f, (float)i*spacing);
        rf_gfx_vertex3f((float)half_slices*spacing, 0.0f, (float)i*spacing);
    }
    rf_gfx_end();
}

// Draw gizmo
RF_API void rf_draw_gizmo(rf_vec3 position)
{
    // NOTE: RGB = XYZ
    float length = 1.0f;

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);
    rf_gfx_scalef(length, length, length);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color3f(1.0f, 0.0f, 0.0f); rf_gfx_vertex3f(0.0f, 0.0f, 0.0f);
    rf_gfx_color3f(1.0f, 0.0f, 0.0f); rf_gfx_vertex3f(1.0f, 0.0f, 0.0f);

    rf_gfx_color3f(0.0f, 1.0f, 0.0f); rf_gfx_vertex3f(0.0f, 0.0f, 0.0f);
    rf_gfx_color3f(0.0f, 1.0f, 0.0f); rf_gfx_vertex3f(0.0f, 1.0f, 0.0f);

    rf_gfx_color3f(0.0f, 0.0f, 1.0f); rf_gfx_vertex3f(0.0f, 0.0f, 0.0f);
    rf_gfx_color3f(0.0f, 0.0f, 1.0f); rf_gfx_vertex3f(0.0f, 0.0f, 1.0f);
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a model (with texture if set)
RF_API void rf_draw_model(rf_model model, rf_vec3 position, float scale, rf_color tint)
{
    rf_vec3 vScale = { scale, scale, scale };
    rf_vec3 rotationAxis = { 0.0f, 1.0f, 0.0f };

    rf_draw_model_ex(model, position, rotationAxis, 0.0f, vScale, tint);
}

// Draw a model with extended parameters
RF_API void rf_draw_model_ex(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotationAngle, rf_vec3 scale, rf_color tint)
{
    // Calculate transformation matrix from function parameters
    // Get transform matrix (rotation -> scale -> translation)
    rf_mat mat_scale = rf_mat_scale(scale.x, scale.y, scale.z);
    rf_mat mat_rotation = rf_mat_rotate(rotation_axis, rotationAngle * RF_DEG2RAD);
    rf_mat mat_translation = rf_mat_translate(position.x, position.y, position.z);

    rf_mat mat_transform = rf_mat_mul(rf_mat_mul(mat_scale, mat_rotation), mat_translation);

    // Combine model transformation matrix (model.transform) with matrix generated by function parameters (mat_transform)
    model.transform = rf_mat_mul(model.transform, mat_transform);

    for (rf_int i = 0; i < model.mesh_count; i++)
    {
        // TODO: Review color + tint premultiplication mechanism
        rf_color color = model.materials[model.mesh_material[i]].maps[RF_MAP_DIFFUSE].color;

        rf_color color_tint = RF_WHITE;
        color_tint.r = (((float)color.r/255.0)*((float)tint.r/255.0))*255;
        color_tint.g = (((float)color.g/255.0)*((float)tint.g/255.0))*255;
        color_tint.b = (((float)color.b/255.0)*((float)tint.b/255.0))*255;
        color_tint.a = (((float)color.a/255.0)*((float)tint.a/255.0))*255;

        model.materials[model.mesh_material[i]].maps[RF_MAP_DIFFUSE].color = color_tint;
        rf_gfx_draw_mesh(model.meshes[i], model.materials[model.mesh_material[i]], model.transform);
        model.materials[model.mesh_material[i]].maps[RF_MAP_DIFFUSE].color = color;
    }
}

// Draw a model wires (with texture if set) with extended parameters
RF_API void rf_draw_model_wires(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotationAngle, rf_vec3 scale, rf_color tint)
{
    rf_gfx_enable_wire_mode();

    rf_draw_model_ex(model, position, rotation_axis, rotationAngle, scale, tint);

    rf_gfx_disable_wire_mode();
}

// Draw a bounding box with wires
RF_API void rf_draw_bounding_box(rf_bounding_box box, rf_color color)
{
    rf_vec3 size;

    size.x = (float)fabs(box.max.x - box.min.x);
    size.y = (float)fabs(box.max.y - box.min.y);
    size.z = (float)fabs(box.max.z - box.min.z);

    rf_vec3 center = {box.min.x + size.x / 2.0f, box.min.y + size.y / 2.0f, box.min.z + size.z / 2.0f };

    rf_draw_cube_wires(center, size.x, size.y, size.z, color);
}

// Draw a billboard
RF_API void rf_draw_billboard(rf_camera3d camera, rf_texture2d texture, rf_vec3 center, float size, rf_color tint)
{
    rf_rec source_rec = {0.0f, 0.0f, (float)texture.width, (float)texture.height };

    rf_draw_billboard_rec(camera, texture, source_rec, center, size, tint);
}

// Draw a billboard (part of a texture defined by a rectangle)
RF_API void rf_draw_billboard_rec(rf_camera3d camera, rf_texture2d texture, rf_rec source_rec, rf_vec3 center, float size, rf_color tint)
{
    // NOTE: Billboard size will maintain source_rec aspect ratio, size will represent billboard width
    rf_vec2 size_ratio = {size, size * (float)source_rec.height / source_rec.width };

    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);

    rf_vec3 right = {mat_view.m0, mat_view.m4, mat_view.m8 };
    //rf_vec3 up = { mat_view.m1, mat_view.m5, mat_view.m9 };

    // NOTE: Billboard locked on axis-Y
    rf_vec3 up = {0.0f, 1.0f, 0.0f };
    /*
        a-------b
        |       |
        |   *   |
        |       |
        d-------c
    */
    right = rf_vec3_scale(right, size_ratio.x / 2);
    up = rf_vec3_scale(up, size_ratio.y / 2);

    rf_vec3 p1 = rf_vec3_add(right, up);
    rf_vec3 p2 = rf_vec3_sub(right, up);

    rf_vec3 a = rf_vec3_sub(center, p2);
    rf_vec3 b = rf_vec3_add(center, p1);
    rf_vec3 c = rf_vec3_add(center, p2);
    rf_vec3 d = rf_vec3_sub(center, p1);

    if (rf_gfx_check_buffer_limit(4)) rf_gfx_draw();

    rf_gfx_enable_texture(texture.id);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(tint.r, tint.g, tint.b, tint.a);

    // Bottom-left corner for texture and quad
    rf_gfx_tex_coord2f((float)source_rec.x/texture.width, (float)source_rec.y/texture.height);
    rf_gfx_vertex3f(a.x, a.y, a.z);

    // Top-left corner for texture and quad
    rf_gfx_tex_coord2f((float)source_rec.x/texture.width, (float)(source_rec.y + source_rec.height)/texture.height);
    rf_gfx_vertex3f(d.x, d.y, d.z);

    // Top-right corner for texture and quad
    rf_gfx_tex_coord2f((float)(source_rec.x + source_rec.width)/texture.width, (float)(source_rec.y + source_rec.height)/texture.height);
    rf_gfx_vertex3f(c.x, c.y, c.z);

    // Bottom-right corner for texture and quad
    rf_gfx_tex_coord2f((float)(source_rec.x + source_rec.width)/texture.width, (float)source_rec.y/texture.height);
    rf_gfx_vertex3f(b.x, b.y, b.z);
    rf_gfx_end();

    rf_gfx_disable_texture();
}