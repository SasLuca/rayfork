#include "rayfork/gfx/context.h"
#include "rayfork/math/maths.h"
#include "rayfork/gfx/gfx.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/gfx/bitmap-font.h"

rf_gfx_context* rf_global_gfx_context;

rf_extern void rf_gfx_init(rf_gfx_context* ctx, int screen_width, int screen_height, rf_gfx_backend_data* gfx_data)
{
    *ctx = (rf_gfx_context) {0};
    rf_set_global_gfx_context_pointer(ctx);

    ctx->current_matrix_mode = -1;
    ctx->screen_scaling      = rf_mat_identity();
    ctx->framebuffer_width   = screen_width;
    ctx->framebuffer_height  = screen_height;
    ctx->render_width        = screen_width;
    ctx->render_height       = screen_height;
    ctx->current_width       = screen_width;
    ctx->current_height      = screen_height;

    rf_gfx_backend_init_impl(gfx_data);

    // Initialize default shaders and default textures
    {
        // Init default white texture
        unsigned char pixels[4] = { 255, 255, 255, 255 }; // 1 pixel RGBA (4 bytes)
        ctx->default_texture_id = rf_gfx_load_texture(pixels, 1, 1, rf_pixel_format_r8g8b8a8, 1);

        if (ctx->default_texture_id != 0)
        {
            rf_log(rf_log_type_info, "Base white texture loaded successfully. [ Texture ID: %d ]", ctx->default_texture_id);
        }
        else
        {
            rf_log(rf_log_type_warning, "Base white texture could not be loaded");
        }

        // Init default shader (customized for GL 3.3 and ES2)
        ctx->default_shader = rf_gfx_load_default_shader();
        ctx->current_shader = ctx->default_shader;

        // Init transformations matrix accumulator
        ctx->transform = rf_mat_identity();

        // Init internal matrix stack (emulating OpenGL 1)
        for (rf_int i = 0; i < rf_max_matrix_stack_size; i++)
        {
            ctx->stack[i] = rf_mat_identity();
        }

        // Init internal matrices
        ctx->projection     = rf_mat_identity();
        ctx->modelview      = rf_mat_identity();
        ctx->current_matrix = &ctx->modelview;
    }

    // Setup default viewport
    rf_set_viewport(screen_width, screen_height);

    // Load default font
    #if !defined(rayfork_dont_load_builtin_font)
    ctx->builtin_font = rf_load_builtin_font(&ctx->builtin_font_data);
    #endif
}

rf_extern rf_int rf_gfx_width() { return rf_global_gfx_context->render_width; }

rf_extern rf_int rf_gfx_height() { return rf_global_gfx_context->render_height; }

rf_extern rf_sizef rf_get_viewport() { return (rf_sizef) { rf_gfx_width(), rf_gfx_height() }; }

// Get texture to draw shapes, the user can customize this using rf_set_shapes_texture
rf_extern rf_texture2d rf_get_shapes_texture()
{
    if (rf_global_gfx_context->tex_shapes.id == 0)
    {
        rf_global_gfx_context->tex_shapes = rf_get_default_texture();
        rf_global_gfx_context->rec_tex_shapes = (rf_rec) {0.0f, 0.0f, 1.0f, 1.0f };
    }

    return rf_global_gfx_context->tex_shapes;
}

// Get default shader
rf_extern rf_shader rf_get_default_shader()
{
    return rf_global_gfx_context->default_shader;
}

// Get default internal texture (white texture)
rf_extern rf_texture2d rf_get_default_texture()
{
    rf_texture2d texture = {0};
    texture.id = rf_global_gfx_context->default_texture_id;
    texture.width = 1;
    texture.height = 1;
    texture.mipmaps = 1;
    texture.format = rf_pixel_format_r8g8b8a8;

    return texture;
}

rf_extern rf_image rf_get_screen_data(rf_allocator allocator)
{
    rf_int dst_size = rf_gfx_width() * rf_gfx_height();
    rf_color* dst = rf_alloc(allocator, sizeof(rf_color) * dst_size);
    rf_image result = rf_get_screen_data_to_buffer(dst, dst_size);
    if (!result.valid)
    {
        rf_free(allocator, dst);
    }

    return result;
}

// Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
rf_extern rf_image rf_get_screen_data_to_buffer(rf_color* dst, rf_int dst_size)
{
    rf_image image = {0};

    if (dst && dst_size == rf_gfx_width() * rf_gfx_height())
    {
        rf_gfx_read_screen_pixels(dst, rf_gfx_width(), rf_gfx_height());

        image.data   = dst;
        image.width  = rf_gfx_width();
        image.height = rf_gfx_height();
        image.format = rf_pixel_format_r8g8b8a8;
        image.valid  = true;
    }

    return image;
}

rf_extern rf_bitmap_font rf_get_builtin_bitmap_font()
{
    return rf_global_gfx_context->builtin_font;
}

// Set the global context pointer
rf_extern void rf_set_global_gfx_context_pointer(rf_gfx_context* ctx)
{
    rf_global_gfx_context = ctx;
}

// Set viewport for a provided width and height
rf_extern void rf_set_viewport(int width, int height)
{
    rf_global_gfx_context->render_width = width;
    rf_global_gfx_context->render_height = height;

    // Set viewport width and height
    rf_gfx_viewport(0, 0, rf_gfx_width(), rf_gfx_height());

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size, top-left corner is (0, 0)
    rf_gfx_ortho(0, rf_gfx_width(), rf_gfx_height(), 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}

// Define default texture used to draw shapes
rf_extern void rf_set_shapes_texture(rf_texture2d texture, rf_rec source)
{
    rf_global_gfx_context->tex_shapes = texture;
    rf_global_gfx_context->rec_tex_shapes = source;
}

// Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
rf_extern rf_material rf_load_default_material(rf_allocator allocator)
{
    rf_material material = {0};
    material.maps = (rf_material_map*) rf_alloc(allocator, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));
    memset(material.maps, 0, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));

    material.shader = rf_get_default_shader();
    material.maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // White texture (1x1 pixel)
    //material.maps[RF_MAP_NORMAL].texture;         // NOTE: By default, not set
    //material.maps[RF_MAP_SPECULAR].texture;       // NOTE: By default, not set

    material.maps[RF_MAP_DIFFUSE].color = rf_white; // Diffuse color
    material.maps[RF_MAP_SPECULAR].color = rf_white; // Specular color

    return material;
}