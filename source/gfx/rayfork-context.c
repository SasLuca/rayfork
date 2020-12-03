#include "rayfork-gfx-internal.h"
#include "rayfork-texture.h"

rf_internal void rf__gfx_backend_internal_init(rf_gfx_backend_data* gfx_data);

rf_public void rf_gfx_init(rf_gfx_context* ctx, int screen_width, int screen_height, rf_gfx_backend_data* gfx_data)
{
    *ctx = (rf_gfx_context) {0};
    rf_set_global_gfx_context_pointer(ctx);

    rf_ctx.current_matrix_mode = -1;
    rf_ctx.screen_scaling = rf_mat_identity();

    rf_ctx.framebuffer_width  = screen_width;
    rf_ctx.framebuffer_height = screen_height;
    rf_ctx.render_width       = screen_width;
    rf_ctx.render_height      = screen_height;
    rf_ctx.current_width      = screen_width;
    rf_ctx.current_height     = screen_height;

    rf__gfx_backend_internal_init(gfx_data);

    // Initialize default shaders and default textures
    {
        // Init default white texture
        unsigned char pixels[4] = { 255, 255, 255, 255 }; // 1 pixel RGBA (4 bytes)
        rf_ctx.default_texture_id = rf_gfx_load_texture(pixels, 1, 1, rf_pixel_format_r8g8b8a8, 1);

        if (rf_ctx.default_texture_id != 0)
        {
            rf_log(rf_log_type_info, "Base white texture loaded successfully. [ Texture ID: %d ]", rf_ctx.default_texture_id);
        }
        else
        {
            rf_log(rf_log_type_warning, "Base white texture could not be loaded");
        }

        // Init default shader (customized for GL 3.3 and ES2)
        rf_ctx.default_shader = rf_load_default_shader();
        rf_ctx.current_shader = rf_ctx.default_shader;

        // Init transformations matrix accumulator
        rf_ctx.transform = rf_mat_identity();

        // Init internal matrix stack (emulating OpenGL 1)
        for (rf_int i = 0; i < RF_MAX_MATRIX_STACK_SIZE; i++)
        {
            rf_ctx.stack[i] = rf_mat_identity();
        }

        // Init internal matrices
        rf_ctx.projection     = rf_mat_identity();
        rf_ctx.modelview      = rf_mat_identity();
        rf_ctx.current_matrix = &rf_ctx.modelview;
    }

    // Setup default viewport
    rf_set_viewport(screen_width, screen_height);

    // Load default font
    #if !defined(RAYFORK_NO_DEFAULT_FONT)
    {
        // NOTE: Using UTF8 encoding table for Unicode U+0000..U+00FF Basic Latin + Latin-1 Supplement
        // http://www.utf8-chartable.de/unicode-utf8-table.pl

        rf_ctx.default_font.glyphs_count = 224; // Number of chars included in our default font

        // Default font is directly defined here (data generated from a sprite font image)
        // This way, we reconstruct rf_font without creating large global variables
        // This data is automatically allocated to Stack and automatically deallocated at the end of this function
        static unsigned int default_font_data[512] = {
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
            0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
            0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
            0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
            0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
            0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
            0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
            0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
            0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
            0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
            0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
            0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000f01, 0x00000000, 0x06000000, 0x24000000, 0x00000f01, 0x00000000, 0x09108000,
            0x24fa28a2, 0x00000f01, 0x00000000, 0x013e0000, 0x2242252a, 0x00000f52, 0x00000000, 0x038a8000, 0x2422222a, 0x00000f29, 0x00000000, 0x010a8000,
            0x2412252a, 0x00000f01, 0x00000000, 0x010a8000, 0x24fbe8be, 0x00000f01, 0x00000000, 0x0ebe8000, 0xac020000, 0x00000f01, 0x00000000, 0x00048000,
            0x0003e000, 0x00000f00, 0x00000000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000038, 0x8443b80e, 0x00203a03,
            0x02bea080, 0xf0000020, 0xc452208a, 0x04202b02, 0xf8029122, 0x07f0003b, 0xe44b388e, 0x02203a02, 0x081e8a1c, 0x0411e92a, 0xf4420be0, 0x01248202,
            0xe8140414, 0x05d104ba, 0xe7c3b880, 0x00893a0a, 0x283c0e1c, 0x04500902, 0xc4400080, 0x00448002, 0xe8208422, 0x04500002, 0x80400000, 0x05200002,
            0x083e8e00, 0x04100002, 0x804003e0, 0x07000042, 0xf8008400, 0x07f00003, 0x80400000, 0x04000022, 0x00000000, 0x00000000, 0x80400000, 0x04000002,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800702, 0x1848a0c2, 0x84010000, 0x02920921, 0x01042642, 0x00005121, 0x42023f7f, 0x00291002,
            0xefc01422, 0x7efdfbf7, 0xefdfa109, 0x03bbbbf7, 0x28440f12, 0x42850a14, 0x20408109, 0x01111010, 0x28440408, 0x42850a14, 0x2040817f, 0x01111010,
            0xefc78204, 0x7efdfbf7, 0xe7cf8109, 0x011111f3, 0x2850a932, 0x42850a14, 0x2040a109, 0x01111010, 0x2850b840, 0x42850a14, 0xefdfbf79, 0x03bbbbf7,
            0x001fa020, 0x00000000, 0x00001000, 0x00000000, 0x00002070, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x08022800, 0x00012283, 0x02430802, 0x01010001, 0x8404147c, 0x20000144, 0x80048404, 0x00823f08, 0xdfbf4284, 0x7e03f7ef, 0x142850a1, 0x0000210a,
            0x50a14684, 0x528a1428, 0x142850a1, 0x03efa17a, 0x50a14a9e, 0x52521428, 0x142850a1, 0x02081f4a, 0x50a15284, 0x4a221428, 0xf42850a1, 0x03efa14b,
            0x50a16284, 0x4a521428, 0x042850a1, 0x0228a17a, 0xdfbf427c, 0x7e8bf7ef, 0xf7efdfbf, 0x03efbd0b, 0x00000000, 0x04000000, 0x00000000, 0x00000008,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200508, 0x00840400, 0x11458122, 0x00014210,
            0x00514294, 0x51420800, 0x20a22a94, 0x0050a508, 0x00200000, 0x00000000, 0x00050000, 0x08000000, 0xfefbefbe, 0xfbefbefb, 0xfbeb9114, 0x00fbefbe,
            0x20820820, 0x8a28a20a, 0x8a289114, 0x3e8a28a2, 0xfefbefbe, 0xfbefbe0b, 0x8a289114, 0x008a28a2, 0x228a28a2, 0x08208208, 0x8a289114, 0x088a28a2,
            0xfefbefbe, 0xfbefbefb, 0xfa2f9114, 0x00fbefbe, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00210100, 0x00000004, 0x00000000, 0x00000000, 0x14508200, 0x00001402, 0x00000000, 0x00000000,
            0x00000010, 0x00000020, 0x00000000, 0x00000000, 0xa28a28be, 0x00002228, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000,
            0xa28a28aa, 0x000022a8, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000, 0xbefbefbe, 0x00003e2f, 0x00000000, 0x00000000,
            0x00000004, 0x00002028, 0x00000000, 0x00000000, 0x80000000, 0x00003e0f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
        };

        int chars_height  = 10;
        int chars_divisor = 1; // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

        int chars_width[224] = {
            3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
            7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
            2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 5, 5, 5, 7, 1, 5, 3, 7, 3, 5, 4, 1, 7, 4, 3, 5, 3, 3, 2, 5, 6, 1, 2, 2, 3, 5, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 3, 3, 3, 3, 7, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 6,
            5, 5, 5, 5, 5, 5, 9, 5, 5, 5, 5, 5, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5
        };

        // Re-construct image from rf_ctx->default_font_data and generate a texture
        //----------------------------------------------------------------------
        {
            rf_color font_pixels[128 * 128] = {0};

            int counter = 0; // rf_font data elements counter

            // Fill with default_font_data (convert from bit to pixel!)
            for (rf_int i = 0; i < 128 * 128; i += 32)
            {
                for (rf_int j = 31; j >= 0; j--)
                {
                    const int bit_check = (default_font_data[counter]) & (1u << j);
                    if (bit_check) font_pixels[i + j] = rf_white;
                }

                counter++;

                if (counter > 512) counter = 0; // Security check...
            }

            rf_bool format_success = rf_format_pixels(font_pixels, 128 * 128 * sizeof(rf_color), rf_pixel_format_r8g8b8a8,
                                                      rf_ctx.default_font_buffers.pixels, 128 * 128 * 2, rf_pixel_format_gray_alpha);

            rf_assert(format_success);
        }

        rf_image font_image = {
            .data = rf_ctx.default_font_buffers.pixels,
            .format = rf_pixel_format_gray_alpha,
            .width = 128,
            .height = 128,
            .valid = true,
        };

        rf_ctx.default_font.texture = rf_load_texture_from_image(font_image);

        // Allocate space for our characters info data
        rf_ctx.default_font.glyphs = rf_ctx.default_font_buffers.chars;

        int current_line     = 0;
        int current_pos_x    = chars_divisor;
        int test_pos_x       = chars_divisor;
        int char_pixels_iter = 0;

        for (rf_int i = 0; i < rf_ctx.default_font.glyphs_count; i++)
        {
            rf_ctx.default_font.glyphs[i].codepoint = 32 + i; // First char is 32

            rf_ctx.default_font.glyphs[i].rec.x      = (float) current_pos_x;
            rf_ctx.default_font.glyphs[i].rec.y      = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
            rf_ctx.default_font.glyphs[i].rec.width  = (float) chars_width[i];
            rf_ctx.default_font.glyphs[i].rec.height = (float) chars_height;

            test_pos_x += (int) (rf_ctx.default_font.glyphs[i].rec.width + (float)chars_divisor);

            if (test_pos_x >= rf_ctx.default_font.texture.width)
            {
                current_line++;
                current_pos_x = 2 * chars_divisor + chars_width[i];
                test_pos_x = current_pos_x;

                rf_ctx.default_font.glyphs[i].rec.x = (float) (chars_divisor);
                rf_ctx.default_font.glyphs[i].rec.y = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
            }
            else current_pos_x = test_pos_x;

            // NOTE: On default font character offsets and xAdvance are not required
            rf_ctx.default_font.glyphs[i].offset_x = 0;
            rf_ctx.default_font.glyphs[i].offset_y = 0;
            rf_ctx.default_font.glyphs[i].advance_x = 0;
        }

        rf_ctx.default_font.base_size = (int)rf_ctx.default_font.glyphs[0].rec.height;
        rf_ctx.default_font.valid = true;

        rf_log(rf_log_type_info, "[TEX ID %i] Default font loaded successfully", rf_ctx.default_font.texture.id);
    }
    #endif
}

#pragma region getters

// Get the default font, useful to be used with extended parameters
rf_public rf_font rf_get_default_font()
{
    return rf_ctx.default_font;
}

// Get default shader
rf_public rf_shader rf_get_default_shader()
{
    return rf_ctx.default_shader;
}

// Get default internal texture (white texture)
rf_public rf_texture2d rf_get_default_texture()
{
    rf_texture2d texture = {0};
    texture.id = rf_ctx.default_texture_id;
    texture.width = 1;
    texture.height = 1;
    texture.mipmaps = 1;
    texture.format = rf_pixel_format_r8g8b8a8;

    return texture;
}

//Get the context pointer
rf_public rf_gfx_context* rf_get_gfx_context()
{
    return &rf_ctx;
}

// Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
rf_public rf_image rf_get_screen_data(rf_color* dst, rf_int dst_size)
{
    rf_image image = {0};

    if (dst && dst_size == rf_ctx.render_width * rf_ctx.render_height)
    {
        rf_gfx_read_screen_pixels(dst, rf_ctx.render_width, rf_ctx.render_height);

        image.data   = dst;
        image.width  = rf_ctx.render_width;
        image.height = rf_ctx.render_height;
        image.format = rf_pixel_format_r8g8b8a8;
        image.valid  = true;
    }

    return image;
}

rf_public rf_log_type rf_get_current_log_filter()
{
    return rf_ctx.logger_filter;
}

#pragma endregion

#pragma region setters

// Define default texture used to draw shapes
rf_public void rf_set_shapes_texture(rf_texture2d texture, rf_rec source)
{
    rf_ctx.tex_shapes = texture;
    rf_ctx.rec_tex_shapes = source;
}

// Set the global context pointer
rf_public void rf_set_global_gfx_context_pointer(rf_gfx_context* ctx)
{
    rf__global_gfx_context_ptr = ctx;
}

// Set viewport for a provided width and height
rf_public void rf_set_viewport(int width, int height)
{
    rf_ctx.render_width = width;
    rf_ctx.render_height = height;

    // Set viewport width and height
    rf_gfx_viewport(0, 0, rf_ctx.render_width, rf_ctx.render_height);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size, top-left corner is (0, 0)
    rf_gfx_ortho(0, rf_ctx.render_width, rf_ctx.render_height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}

rf_public inline rf_int rf_libc_rand_wrapper(rf_int min, rf_int max)
{
    return rand() % (max + 1 - min) + min;
}

#pragma endregion