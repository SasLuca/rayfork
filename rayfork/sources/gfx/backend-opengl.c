#if defined(rayfork_graphics_backend_gl33) || defined(rayfork_graphics_backend_gl_es3)

#include "rayfork/gfx/backends/gl/backend-opengl.h"
#include "rayfork/gfx/backends/gl/glconstants.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/foundation/str.h"
#include "rayfork/math/maths.h"
#include "rayfork/gfx/context.h"
#include "rayfork/gfx/gfx.h"
#include "rayfork/internal/gfx/macros.h"

// Use this for glClearDepth
#if defined(rayfork_graphics_backend_gl33)
    #define rf_gl_ClearDepth rf_gl.ClearDepth
#else
    #define rf_gl_ClearDepth rf_gl.ClearDepthf
#endif

#pragma region internal functions

// Compile custom shader and return shader id
rf_internal unsigned int rf_compile_shader(const char* shader_str, int type)
{
    unsigned int shader = rf_gl.CreateShader(type);
    rf_gl.ShaderSource(shader, 1, &shader_str, NULL);

    int success = 0;
    rf_gl.CompileShader(shader);
    rf_gl.GetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE)
    {
        rf_log(rf_log_type_warning, "[SHDR ID %i] Failed to compile shader...", shader);
        int max_len = 0;
        int length;
        rf_gl.GetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_len);

        //@Note: Buffer may not be big enough for some messages
        char log[1024];

        rf_gl.GetShaderInfoLog(shader, 1024, &length, log);

        rf_log(rf_log_type_info, "%s", log);
    }
    else rf_log(rf_log_type_info, "[SHDR ID %i] rf_shader compiled successfully", shader);

    return shader;
}

// Load custom shader strings and return program id
rf_internal unsigned int rf_load_shader_program(unsigned int v_shader_id, unsigned int f_shader_id)
{
    unsigned int program = 0;

    int success = 0;
    program = rf_gl.CreateProgram();

    rf_gl.AttachShader(program, v_shader_id);
    rf_gl.AttachShader(program, f_shader_id);

    // NOTE: Default attribute shader locations must be binded before linking
    rf_gl.BindAttribLocation(program, 0, RF_DEFAULT_ATTRIB_POSITION_NAME);
    rf_gl.BindAttribLocation(program, 1, RF_DEFAULT_ATTRIB_TEXCOORD_NAME);
    rf_gl.BindAttribLocation(program, 2, RF_DEFAULT_ATTRIB_NORMAL_NAME);
    rf_gl.BindAttribLocation(program, 3, RF_DEFAULT_ATTRIB_COLOR_NAME);
    rf_gl.BindAttribLocation(program, 4, RF_DEFAULT_ATTRIB_TANGENT_NAME);
    rf_gl.BindAttribLocation(program, 5, RF_DEFAULT_ATTRIB_TEXCOORD2_NAME);

    // NOTE: If some attrib name is no found on the shader, it locations becomes -1

    rf_gl.LinkProgram(program);

    // NOTE: All uniform variables are intitialised to 0 when a program links

    rf_gl.GetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        rf_log(rf_log_type_warning, "[SHDR ID %i] Failed to link shader program...", program);

        int max_len = 0;
        int length;

        rf_gl.GetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);

        char log[1024];

        rf_gl.GetProgramInfoLog(program, 1024, &length, log);

        rf_log(rf_log_type_info, "%s", log);

        rf_gl.DeleteProgram(program);

        program = 0;
    }
    else rf_log(rf_log_type_info, "[SHDR ID %i] rf_shader program loaded successfully", program);

    return program;
}

// Get location handlers to for shader attributes and uniforms. Note: If any location is not found, loc point becomes -1
rf_internal void rf_set_shader_default_locations(rf_shader* shader)
{
    // NOTE: Default shader attrib locations have been fixed before linking:
    //          vertex position location    = 0
    //          vertex texcoord location    = 1
    //          vertex normal location      = 2
    //          vertex color location       = 3
    //          vertex tangent location     = 4
    //          vertex texcoord2 location   = 5

    // Get handles to GLSL input attibute locations
    shader->locs[RF_LOC_VERTEX_POSITION] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_POSITION_NAME);
    shader->locs[RF_LOC_VERTEX_TEXCOORD01] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_TEXCOORD_NAME);
    shader->locs[RF_LOC_VERTEX_TEXCOORD02] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_TEXCOORD2_NAME);
    shader->locs[RF_LOC_VERTEX_NORMAL] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_NORMAL_NAME);
    shader->locs[RF_LOC_VERTEX_TANGENT] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_TANGENT_NAME);
    shader->locs[RF_LOC_VERTEX_COLOR] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_COLOR_NAME);

    // Get handles to GLSL uniform locations (vertex shader)
    shader->locs[RF_LOC_MATRIX_MVP]  = rf_gl.GetUniformLocation(shader->id, "mvp");
    shader->locs[RF_LOC_MATRIX_PROJECTION]  = rf_gl.GetUniformLocation(shader->id, "projection");
    shader->locs[RF_LOC_MATRIX_VIEW]  = rf_gl.GetUniformLocation(shader->id, "view");

    // Get handles to GLSL uniform locations (fragment shader)
    shader->locs[RF_LOC_COLOR_DIFFUSE] = rf_gl.GetUniformLocation(shader->id, "col_diffuse");
    shader->locs[RF_LOC_MAP_DIFFUSE] = rf_gl.GetUniformLocation(shader->id, "texture0");
    shader->locs[RF_LOC_MAP_SPECULAR] = rf_gl.GetUniformLocation(shader->id, "texture1");
    shader->locs[RF_LOC_MAP_NORMAL] = rf_gl.GetUniformLocation(shader->id, "texture2");
}

// Unload default shader
rf_internal void rf_unlock_shader_default()
{
    rf_gl.UseProgram(0);

    rf_gl.DetachShader(rf_ctx.default_shader.id, rf_ctx.default_vertex_shader_id);
    rf_gl.DetachShader(rf_ctx.default_shader.id, rf_ctx.default_frag_shader_id);
    rf_gl.DeleteShader(rf_ctx.default_vertex_shader_id);
    rf_gl.DeleteShader(rf_ctx.default_frag_shader_id);

    rf_gl.DeleteProgram(rf_ctx.default_shader.id);
}

// Draw default internal buffers vertex data
rf_internal void rf_draw_buffers_default()
{

}

// Unload default internal buffers vertex data from CPU and GPU
rf_internal void rf_unload_buffers_default()
{
    // Unbind everything
    rf_gl.BindVertexArray(0);
    rf_gl.DisableVertexAttribArray(0);
    rf_gl.DisableVertexAttribArray(1);
    rf_gl.DisableVertexAttribArray(2);
    rf_gl.DisableVertexAttribArray(3);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, 0);
    rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (rf_int i = 0; i < RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT; i++)
    {
        // Delete VBOs from GPU (VRAM)
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[0]);
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[1]);
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[2]);
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[3]);

        // Delete VAOs from GPU (VRAM)
        rf_gl.DeleteVertexArrays(1, &rf_batch.vertex_buffers[i].vao_id);
    }
}

// Renders a 1x1 XY quad in NDC
rf_internal void rf_gen_draw_quad(void)
{
    unsigned int quad_vao = 0;
    unsigned int quad_vbo = 0;

    float vertices[] = {
            // Positions        // rf_texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // Set up plane VAO
    rf_gl.GenVertexArrays(1, &quad_vao);
    rf_gl.GenBuffers(1, &quad_vbo);
    rf_gl.BindVertexArray(quad_vao);

    // Fill buffer
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    rf_gl.EnableVertexAttribArray(0);
    rf_gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void* )0);
    rf_gl.EnableVertexAttribArray(1);
    rf_gl.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void* )(3 * sizeof(float)));

    // Draw quad
    rf_gl.BindVertexArray(quad_vao);
    rf_gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    rf_gl.BindVertexArray(0);

    rf_gl.DeleteBuffers(1, &quad_vbo);
    rf_gl.DeleteVertexArrays(1, &quad_vao);
}

// Renders a 1x1 3D cube in NDC
rf_internal void rf_gen_draw_cube(void)
{
    unsigned int cube_vao = 0;
    unsigned int cube_vbo = 0;

    static float vertices[] = {
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f , 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    // Set up cube VAO
    rf_gl.GenVertexArrays(1, &cube_vao);
    rf_gl.GenBuffers(1, &cube_vbo);

    // Fill buffer
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    rf_gl.BindVertexArray(cube_vao);
    rf_gl.EnableVertexAttribArray(0);
    rf_gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )0);
    rf_gl.EnableVertexAttribArray(1);
    rf_gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )(3 * sizeof(float)));
    rf_gl.EnableVertexAttribArray(2);
    rf_gl.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )(6 * sizeof(float)));
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, 0);
    rf_gl.BindVertexArray(0);

    // Draw cube
    rf_gl.BindVertexArray(cube_vao);
    rf_gl.DrawArrays(GL_TRIANGLES, 0, 36);
    rf_gl.BindVertexArray(0);

    rf_gl.DeleteBuffers(1, &cube_vbo);
    rf_gl.DeleteVertexArrays(1, &cube_vao);
}

rf_internal void rf_set_gl_extension_if_available(rf_str gl_ext)
{
    #if defined(rayfork_graphics_backend_gl_es3)
        // Check NPOT textures support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
        if (rf_match_str_cstr(gl_ext, len, "GL_OES_texture_npot")) {
            rf_gfx.extensions.tex_npot_supported = 1;
        }

        // Check texture float support
        if (rf_match_str_cstr(gl_ext, len, "GL_OES_texture_float")) {
            rf_gfx.extensions.tex_float_supported = 1;
        }

        // Check depth texture support
        if ((rf_match_str_cstr(gl_ext, len, "GL_OES_depth_texture")) ||
            (rf_match_str_cstr(gl_ext, len, "GL_WEBGL_depth_texture"))) {
            rf_gfx.extensions.tex_depth_supported = 1;
        }

        if (rf_match_str_cstr(gl_ext, len, "GL_OES_depth24")) {
            rf_gfx.extensions.max_depth_bits = 24;
        }

        if (rf_match_str_cstr(gl_ext, len, "GL_OES_depth32")) {
            rf_gfx.extensions.max_depth_bits = 32;
        }
    #endif

    // DDS texture compression support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_EXT_texture_compression_s3tc")) ||
        rf_str_match_prefix(gl_ext, rf_cstr("GL_WEBGL_compressed_texture_s3tc")) ||
        rf_str_match_prefix(gl_ext, rf_cstr("GL_WEBKIT_WEBGL_compressed_texture_s3tc"))) {
        rf_gfx.extensions.tex_comp_dxt_supported = 1;
    }

    // ETC1 texture compression support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_OES_compressed_ETC1_RGB8_texture")) ||
        rf_str_match_prefix(gl_ext, rf_cstr("GL_WEBGL_compressed_texture_etc1"))) {
        rf_gfx.extensions.tex_comp_etc1_supported = 1;
    }

    // ETC2/EAC texture compression support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_ARB_ES3_compatibility"))) {
        rf_gfx.extensions.tex_comp_etc2_supported = 1;
    }

    // PVR texture compression support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_IMG_texture_compression_pvrtc"))) {
        rf_gfx.extensions.tex_comp_pvrt_supported = 1;
    }

    // ASTC texture compression support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_KHR_texture_compression_astc_hdr"))) {
        rf_gfx.extensions.tex_comp_astc_supported = 1;
    }

    // Anisotropic texture filter support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_EXT_texture_filter_anisotropic"))) {
        rf_gfx.extensions.tex_anisotropic_filter_supported = 1;
        rf_gl.GetFloatv(0x84FF, &rf_gfx.extensions.max_anisotropic_level); // GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    }

    // Clamp mirror wrap mode supported
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_EXT_texture_mirror_clamp"))) {
        rf_gfx.extensions.tex_mirror_clamp_supported = 1;
    }

    // Debug marker support
    if (rf_str_match_prefix(gl_ext, rf_cstr("GL_EXT_debug_marker"))) {
        rf_gfx.extensions.debug_marker_supported = 1;
    }
}

#pragma endregion

#pragma region init

rf_extern void rf_gfx_backend_init_impl(rf_gfx_backend_data* gfx_data)
{
    rf_gfx.gl = *((rf_opengl_procs*) gfx_data);
    rf_gfx.extensions.max_depth_bits = 16;

    // Check for extensions
    {
        int num_ext = 0;

        #if defined(rayfork_graphics_backend_gl33)
        {
            // Multiple texture extensions supported by default
            rf_gfx.extensions.tex_npot_supported  = 1;
            rf_gfx.extensions.tex_float_supported = 1;
            rf_gfx.extensions.tex_depth_supported = 1;

            rf_gl.GetIntegerv(GL_NUM_EXTENSIONS, &num_ext);

            for (rf_int i = 0; i < num_ext; i++)
            {
                const char* ext = (const char *) rf_gl.GetStringi(GL_EXTENSIONS, i);
                rf_set_gl_extension_if_available(rf_cstr(ext));
            }
        }
        #elif defined(rayfork_graphics_backend_gl_es3)
        {
            //Note: How this works is that we get one big string formated like "gl_ext_func_name1 gl_ext_func_name2 ..."
            //All function names are separated by a space, so we just take a pointer to the begin and advance until we hit a space
            //At which point we get the length, do the comparison, and set the ptr of the next extension to the next element.
            const char* extensions_str = (const char*) rf_gl.GetString(GL_EXTENSIONS);  // One big const string
            if (extensions_str)
            {
                const char* curr_ext = extensions_str;

                while (*extensions_str)
                {
                    //If we get to a space that means we got a new extension name
                    if (*extensions_str == ' ')
                    {
                        num_ext++;
                        const int curr_ext_len = (int) (extensions_str - curr_ext);
                        rf_set_gl_extension_if_available(curr_ext, curr_ext_len);
                        curr_ext = extensions_str + 1;
                    }

                    extensions_str++;
                }

                if (rf_gfx.extensions.tex_npot_supported) rf_log(rf_log_type_info, "[EXTENSION] NPOT textures extension detected, full NPOT textures supported");
                else rf_log(rf_log_type_warning, "[EXTENSION] NPOT textures extension not found, limited NPOT support (no-mipmaps, no-repeat)");
            }
        }
        #endif

        rf_log(rf_log_type_info, "Number of supported extensions: %i.", num_ext);

        if (rf_gfx.extensions.tex_comp_dxt_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] DXT compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_etc1_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] ETC1 compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_etc2_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] ETC2/EAC compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_pvrt_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] PVRT compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_astc_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] ASTC compressed textures supported");
        }

        if (rf_gfx.extensions.tex_anisotropic_filter_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] Anisotropic textures filtering supported (max: %.0fX)", rf_gfx.extensions.max_anisotropic_level);
        }

        if (rf_gfx.extensions.tex_mirror_clamp_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] Mirror clamp wrap texture mode supported");
        }

        if (rf_gfx.extensions.debug_marker_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] Debug Marker supported");
        }
    }

    // Initialize OpenGL default states
    {
        // Init state: Depth test
        rf_gl.DepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
        rf_gl.Disable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

        // Init state: Blending mode
        rf_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // rf_color blending function (how colors are mixed)
        rf_gl.Enable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

        // Init state: Culling
        // NOTE: All shapes/models triangles are drawn CCW
        rf_gl.CullFace(GL_BACK);                                    // Cull the back face (default)
        rf_gl.FrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
        rf_gl.Enable(GL_CULL_FACE);                                 // Enable backface culling

        // Init state: rf_color/Depth buffers clear
        rf_gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
        rf_gl_ClearDepth(1.0f);                                     // Set clear depth value (default)
        rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)
    }
}

#pragma endregion

#pragma region shader

// Load default shader (just vertex positioning and texture coloring). Note: This shader program is used for internal buffers
rf_extern rf_shader rf_gfx_load_default_shader()
{
    rf_shader shader = { 0 };
    memset(shader.locs, 0, sizeof(shader.locs));

    // NOTE: All locations must be reseted to -1 (no location)
    for (rf_int i = 0; i < RF_MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;

    // Vertex shader directly defined, no external file required
    const char* default_vertex_shader_str =
            #if defined(rayfork_graphics_backend_gl_es3)
            "#version 100\n"
            "attribute vec3 vertex_position;"
            "attribute vec2 vertex_tex_coord;"
            "attribute vec4 vertex_color;"
            "varying vec2 frag_tex_coord;"
            "varying vec4 frag_color;"
            #elif defined(rayfork_graphics_backend_gl33)
            "#version 330\n"
            "in vec3 vertex_position;"
            "in vec2 vertex_tex_coord;"
            "in vec4 vertex_color;"
            "out vec2 frag_tex_coord;"
            "out vec4 frag_color;"
            #endif
            "uniform mat4 mvp;"
            "void main()"
            "{"
            "    frag_tex_coord = vertex_tex_coord;"
            "    frag_color = vertex_color;"
            "    gl_Position = mvp*vec4(vertex_position, 1.0);"
            "}";

    // Fragment shader directly defined, no external file required
    const char* default_fragment_shader_str =
            #if defined(rayfork_graphics_backend_gl_es3)
            "#version 100\n"
            "precision mediump float;"
            "varying vec2 frag_tex_coord;"
            "varying vec4 frag_color;"
            #elif defined(rayfork_graphics_backend_gl33)
            "#version 330\n"
            "precision mediump float;"
            "in vec2 frag_tex_coord;"
            "in vec4 frag_color;"
            "out vec4 final_color;"
            #endif
            "uniform sampler2D texture0;"
            "uniform vec4 col_diffuse;"
            "void main()"
            "{"
            #if defined(rayfork_graphics_backend_gl_es3)
            "    vec4 texel_color = texture2D(texture0, frag_tex_coord);" // NOTE: texture2D() is deprecated on OpenGL 3.3 and ES 3.0
            "    frag_color = texel_color*col_diffuse*frag_color;"
            #elif defined(rayfork_graphics_backend_gl33)
            "    vec4 texel_color = texture(texture0, frag_tex_coord);"
            "    final_color = texel_color*col_diffuse*frag_color;"
            #endif
            "}";

    // NOTE: Compiled vertex/fragment shaders are kept for re-use
    rf_ctx.default_vertex_shader_id = rf_compile_shader(default_vertex_shader_str, GL_VERTEX_SHADER);     // Compile default vertex shader
    rf_ctx.default_frag_shader_id = rf_compile_shader(default_fragment_shader_str, GL_FRAGMENT_SHADER);   // Compile default fragment shader

    shader.id = rf_load_shader_program(rf_ctx.default_vertex_shader_id, rf_ctx.default_frag_shader_id);

    if (shader.id > 0)
    {
        rf_log(rf_log_type_info, "[SHDR ID %i] Default shader loaded successfully", shader.id);

        // Set default shader locations: attributes locations
        shader.locs[RF_LOC_VERTEX_POSITION] = rf_gl.GetAttribLocation(shader.id, "vertex_position");
        shader.locs[RF_LOC_VERTEX_TEXCOORD01] = rf_gl.GetAttribLocation(shader.id, "vertex_tex_coord");
        shader.locs[RF_LOC_VERTEX_COLOR] = rf_gl.GetAttribLocation(shader.id, "vertex_color");

        // Set default shader locations: uniform locations
        shader.locs[RF_LOC_MATRIX_MVP]  = rf_gl.GetUniformLocation(shader.id, "mvp");
        shader.locs[RF_LOC_COLOR_DIFFUSE] = rf_gl.GetUniformLocation(shader.id, "col_diffuse");
        shader.locs[RF_LOC_MAP_DIFFUSE] = rf_gl.GetUniformLocation(shader.id, "texture0");

        // NOTE: We could also use below function but in case RF_DEFAULT_ATTRIB_* points are
        // changed for external custom shaders, we just use direct bindings above
        //rf_set_shader_default_locations(&shader);
    }
    else rf_log(rf_log_type_warning, "[SHDR ID %i] Default shader could not be loaded", shader.id);

    return shader;
}

// Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
rf_extern rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code)
{
    rf_shader shader = { 0 };
    memset(shader.locs, -1, sizeof(shader.locs));

    unsigned int vertex_shader_id   = rf_ctx.default_vertex_shader_id;
    unsigned int fragment_shader_id = rf_ctx.default_frag_shader_id;

    if (vs_code != NULL) vertex_shader_id   = rf_compile_shader(vs_code, GL_VERTEX_SHADER);
    if (fs_code != NULL) fragment_shader_id = rf_compile_shader(fs_code, GL_FRAGMENT_SHADER);

    if ((vertex_shader_id == rf_ctx.default_vertex_shader_id) && (fragment_shader_id == rf_ctx.default_frag_shader_id)) shader = rf_ctx.default_shader;
    else
    {
        shader.id = rf_load_shader_program(vertex_shader_id, fragment_shader_id);

        if (vertex_shader_id != rf_ctx.default_vertex_shader_id) rf_gl.DeleteShader(vertex_shader_id);
        if (fragment_shader_id != rf_ctx.default_frag_shader_id) rf_gl.DeleteShader(fragment_shader_id);

        if (shader.id == 0)
        {
            rf_log(rf_log_type_warning, "Custom shader could not be loaded");
            shader = rf_ctx.default_shader;
        }

        // After shader loading, we TRY to set default location names
        if (shader.id > 0) rf_set_shader_default_locations(&shader);
    }

    // Get available shader uniforms
    // NOTE: This information is useful for debug...
    int uniform_count = -1;

    rf_gl.GetProgramiv(shader.id, GL_ACTIVE_UNIFORMS, &uniform_count);

    for (rf_int i = 0; i < uniform_count; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        unsigned int type = GL_ZERO;

        // Get the name of the uniforms
        rf_gl.GetActiveUniform(shader.id, i,sizeof(name) - 1, &namelen, &num, &type, name);

        name[namelen] = 0;

        // Get the location of the named uniform
        unsigned int location = rf_gl.GetUniformLocation(shader.id, name);

        rf_log(rf_log_type_debug, "[SHDR ID %i] Active uniform [%s] set at location: %i", shader.id, name, location);
    }

    return shader;
}

// Unload shader from GPU memory (VRAM)
rf_extern void rf_gfx_unload_shader(rf_shader shader)
{
    if (shader.id > 0)
    {
        rf_gfx_delete_shader(shader.id);
        rf_log(rf_log_type_info, "[SHDR ID %i] Unloaded shader program data", shader.id);
    }
}

// Get shader uniform location
rf_extern int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name)
{
    int location = -1;
    location = rf_gl.GetUniformLocation(shader.id, uniform_name);

    if (location == -1) rf_log(rf_log_type_warning, "[SHDR ID %i][%s] rf_shader uniform could not be found", shader.id, uniform_name);
    else rf_log(rf_log_type_info, "[SHDR ID %i][%s] rf_shader uniform set at location: %i", shader.id, uniform_name, location);

    return location;
}

// Set shader uniform value
rf_extern void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name)
{
    rf_gfx_set_shader_value_v(shader, uniform_loc, value, uniform_name, 1);
}

// Set shader uniform value vector
rf_extern void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count)
{
    rf_gl.UseProgram(shader.id);

    switch (uniform_name)
    {
        case RF_UNIFORM_FLOAT: rf_gl.Uniform1fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_VEC2: rf_gl.Uniform2fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_VEC3: rf_gl.Uniform3fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_VEC4: rf_gl.Uniform4fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_INT: rf_gl.Uniform1iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_IVEC2: rf_gl.Uniform2iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_IVEC3: rf_gl.Uniform3iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_IVEC4: rf_gl.Uniform4iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_SAMPLER2D: rf_gl.Uniform1iv(uniform_loc, count, (int* )value); break;
        default: rf_log(rf_log_type_warning, "rf_shader uniform could not be set data type not recognized");
    }

    //rf_gl.UseProgram(0);      // Avoid reseting current shader program, in case other uniforms are set
}

// Set shader uniform value (matrix 4x4)
rf_extern void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat)
{
    rf_gl.UseProgram(shader.id);

    rf_gl.UniformMatrix4fv(uniform_loc, 1, 0, rf_mat_to_float16(mat).v);

    //rf_gl.UseProgram(0);
}

// Set shader uniform value for texture
rf_extern void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture)
{
    rf_gl.UseProgram(shader.id);

    rf_gl.Uniform1i(uniform_loc, texture.id);

    //rf_gl.UseProgram(0);
}

// Return internal rf_ctx->gl_ctx.projection matrix
rf_extern rf_mat rf_gfx_get_matrix_projection() {
    return rf_ctx.projection;
}

// Return internal rf_ctx->gl_ctx.modelview matrix
rf_extern rf_mat rf_gfx_get_matrix_modelview()
{
    rf_mat matrix = rf_mat_identity();
    matrix = rf_ctx.modelview;
    return matrix;
}

// Set a custom projection matrix (replaces internal rf_ctx->gl_ctx.projection matrix)
rf_extern void rf_gfx_set_matrix_projection(rf_mat proj)
{
    rf_ctx.projection = proj;
}

// Set a custom rf_ctx->gl_ctx.modelview matrix (replaces internal rf_ctx->gl_ctx.modelview matrix)
rf_extern void rf_gfx_set_matrix_modelview(rf_mat view)
{
    rf_ctx.modelview = view;
}

#pragma endregion

#pragma region gfx

// Choose the blending mode (alpha, additive, multiplied)
rf_extern void rf_gfx_blend_mode(rf_blend_mode mode)
{
    rf_gfx_draw();

    switch (mode)
    {
        case RF_BLEND_ALPHA: rf_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
        case RF_BLEND_ADDITIVE: rf_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE); break; // Alternative: glBlendFunc(GL_ONE, GL_ONE);
        case RF_BLEND_MULTIPLIED: rf_gl.BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); break;
        default: break;
    }

    rf_ctx.blend_mode = mode;
}

// Choose the current matrix to be transformed
rf_extern void rf_gfx_matrix_mode(rf_matrix_mode mode)
{
    if (mode == GL_PROJECTION) rf_ctx.current_matrix = &rf_ctx.projection;
    else if (mode == GL_MODELVIEW) rf_ctx.current_matrix = &rf_ctx.modelview;
    //else if (mode == GL_TEXTURE) // Not supported

    rf_ctx.current_matrix_mode = mode;
}

// Push the current matrix into rf_ctx->gl_ctx.stack
rf_extern void rf_gfx_push_matrix()
{
    if (rf_ctx.stack_counter >= rf_max_matrix_stack_size) rf_log_error(rf_limit_reached, "Matrix stack limit reached.");

    if (rf_ctx.current_matrix_mode == GL_MODELVIEW)
    {
        rf_ctx.transform_matrix_required = 1;
        rf_ctx.current_matrix = &rf_ctx.transform;
    }

    rf_ctx.stack[rf_ctx.stack_counter] = *rf_ctx.current_matrix;
    rf_ctx.stack_counter++;
}

// Pop lattest inserted matrix from rf_ctx->gl_ctx.stack
rf_extern void rf_gfx_pop_matrix()
{
    if (rf_ctx.stack_counter > 0)
    {
        rf_mat mat = rf_ctx.stack[rf_ctx.stack_counter - 1];
        *rf_ctx.current_matrix = mat;
        rf_ctx.stack_counter--;
    }

    if ((rf_ctx.stack_counter == 0) && (rf_ctx.current_matrix_mode == GL_MODELVIEW))
    {
        rf_ctx.current_matrix = &rf_ctx.modelview;
        rf_ctx.transform_matrix_required = 0;
    }
}

// Reset current matrix to identity matrix
rf_extern void rf_gfx_load_identity()
{
    *rf_ctx.current_matrix = rf_mat_identity();
}

// Multiply the current matrix by a translation matrix
rf_extern void rf_gfx_translatef(float x, float y, float z)
{
    rf_mat mat_translation = rf_mat_translate(x, y, z);

    // NOTE: We transpose matrix with multiplication order
    *rf_ctx.current_matrix = rf_mat_mul(mat_translation, *rf_ctx.current_matrix);
}

// Multiply the current matrix by a rotation matrix
rf_extern void rf_gfx_rotatef(float angleDeg, float x, float y, float z)
{
    rf_mat mat_rotation = rf_mat_identity();

    rf_vec3 axis = (rf_vec3){x, y, z };
    mat_rotation = rf_mat_rotate(rf_vec3_normalize(axis), angleDeg * rf_deg2rad);

    // NOTE: We transpose matrix with multiplication order
    *rf_ctx.current_matrix = rf_mat_mul(mat_rotation, *rf_ctx.current_matrix);
}

// Multiply the current matrix by a scaling matrix
rf_extern void rf_gfx_scalef(float x, float y, float z)
{
    rf_mat mat_scale = rf_mat_scale(x, y, z);

    // NOTE: We transpose matrix with multiplication order
    *rf_ctx.current_matrix = rf_mat_mul(mat_scale, *rf_ctx.current_matrix);
}

// Multiply the current matrix by another matrix
rf_extern void rf_gfx_mult_matrixf(float* matf)
{
    // rf_mat creation from array
    rf_mat mat = {matf[0], matf[4], matf[8], matf[12],
                  matf[1], matf[5], matf[9], matf[13],
                  matf[2], matf[6], matf[10], matf[14],
                  matf[3], matf[7], matf[11], matf[15] };

    *rf_ctx.current_matrix = rf_mat_mul(*rf_ctx.current_matrix, mat);
}

// Multiply the current matrix by a perspective matrix generated by parameters
rf_extern void rf_gfx_frustum(double left, double right, double bottom, double top, double znear, double zfar)
{
    rf_mat mat_perps = rf_mat_frustum(left, right, bottom, top, znear, zfar);

    *rf_ctx.current_matrix = rf_mat_mul(*rf_ctx.current_matrix, mat_perps);
}

// Multiply the current matrix by an orthographic matrix generated by parameters
rf_extern void rf_gfx_ortho(double left, double right, double bottom, double top, double znear, double zfar)
{
    rf_mat mat_ortho = rf_mat_ortho(left, right, bottom, top, znear, zfar);

    *rf_ctx.current_matrix = rf_mat_mul(*rf_ctx.current_matrix, mat_ortho);
}

// Set the viewport area (transformation from normalized device coordinates to window coordinates)
void rf_gfx_viewport(int x, int y, int width, int height)
{
    rf_gl.Viewport(x, y, width, height);
}

// Initialize drawing mode (how to organize vertex)
rf_extern void rf_gfx_begin(rf_drawing_mode mode)
{
    // Draw mode can be GL_LINES, GL_TRIANGLES and GL_QUADS
    // NOTE: In all three cases, vertex are accumulated over default internal vertex buffer
    if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode != mode)
    {
        if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count > 0)
        {
            // Make sure current rf_ctx->gl_ctx.draws[i].vertex_count is aligned a multiple of 4,
            // that way, following QUADS drawing will keep aligned with index processing
            // It implies adding some extra alignment vertex at the end of the draw,
            // those vertex are not processed but they are considered as an additional offset
            // for the next set of vertex to be drawn
            if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == GL_LINES) rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count : rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4);
            else if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == GL_TRIANGLES) rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? 1 : (4 - (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4)));

            else rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = 0;

            if (rf_gfx_check_buffer_limit(rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment)) rf_gfx_draw();
            else
            {
                rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;

                rf_batch.draw_calls_counter++;
            }
        }

        if (rf_batch.draw_calls_counter >= RF_DEFAULT_BATCH_DRAW_CALLS_COUNT) rf_gfx_draw();

        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode = mode;
        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count = 0;
        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].texture_id = rf_ctx.default_texture_id;
    }
}

// Finish vertex providing
rf_extern void rf_gfx_end()
{
    // Make sure vertex_count is the same for vertices, texcoords, colors and normals
    // NOTE: In OpenGL 1.1, one glColor call can be made for all the subsequent glVertex calls

    // Make sure colors count match vertex count
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter != rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter)
    {
        int add_colors = rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter - rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter;

        for (rf_int i = 0; i < add_colors; i++)
        {
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 4];
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 1] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 3];
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 2] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 2];
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 3] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 1];
            rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter++;
        }
    }

    // Make sure texcoords count match vertex count
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter != rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter)
    {
        int add_tex_coords = rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter - rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter;

        for (rf_int i = 0; i < add_tex_coords; i++)
        {
            rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter] = 0.0f;
            rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter + 1] = 0.0f;
            rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter++;
        }
    }

    // TODO: Make sure normals count match vertex count... if normals support is added in a future... :P

    // NOTE: Depth increment is dependant on rf_gfx_ortho(): z-near and z-far values,
    // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
    // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bits)
    rf_batch.current_depth += (1.0f/20000.0f);

    // Verify internal buffers limits
    // NOTE: This check is combined with usage of rf_gfx_check_buffer_limit()
    if ((rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter) >= (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4 - 4))
    {
        // WARNING: If we are between rf_gfx_push_matrix() and rf_gfx_pop_matrix() and we need to force a rf_gfx_draw(),
        // we need to call rf_gfx_pop_matrix() before to recover *rf_ctx->gl_ctx.current_matrix (rf_ctx->gl_ctx.modelview) for the next forced draw call!
        // If we have multiple matrix pushed, it will require "rf_ctx->gl_ctx.stack_counter" pops before launching the draw
        for (rf_int i = rf_ctx.stack_counter; i >= 0; i--) rf_gfx_pop_matrix();
        rf_gfx_draw();
    }
}

// Define one vertex (position)
rf_extern void rf_gfx_vertex2i(int x, int y)
{
    rf_gfx_vertex3f((float)x, (float)y, rf_batch.current_depth);
}

// Define one vertex (position)
rf_extern void rf_gfx_vertex2f(float x, float y)
{
    rf_gfx_vertex3f(x, y, rf_batch.current_depth);
}

// Define one vertex (position)
// NOTE: Vertex position data is the basic information required for drawing
rf_extern void rf_gfx_vertex3f(float x, float y, float z)
{
    rf_vec3 vec = {x, y, z };

    // rf_transform provided vector if required
    if (rf_ctx.transform_matrix_required) vec = rf_vec3_transform(vec, rf_ctx.transform);

    // Verify that rf_max_batch_elements limit not reached
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter < (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4))
    {
        rf_batch.vertex_buffers[rf_batch.current_buffer].vertices[3*rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter] = vec.x;
        rf_batch.vertex_buffers[rf_batch.current_buffer].vertices[3*rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter + 1] = vec.y;
        rf_batch.vertex_buffers[rf_batch.current_buffer].vertices[3*rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter + 2] = vec.z;
        rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter++;

        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count++;
    }
    else rf_log_error(rf_limit_reached, "Render batch elements limit reached. Max bacht elements: %d", rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4);
}

// Define one vertex (texture coordinate)
// NOTE: rf_texture coordinates are limited to QUADS only
rf_extern void rf_gfx_tex_coord2f(float x, float y)
{
    rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter] = x;
    rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter + 1] = y;
    rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter++;
}

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
rf_extern void rf_gfx_normal3f(float x, float y, float z)
{
    // TODO: Normals usage...
}

// Define one vertex (color)
rf_extern void rf_gfx_color3f(float x, float y, float z)
{
    rf_gfx_color4ub((unsigned char)(x*255), (unsigned char)(y*255), (unsigned char)(z*255), 255);
}

// Define one vertex (color)
rf_extern void rf_gfx_color4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter] = x;
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 1] = y;
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 2] = z;
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 3] = w;
    rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter++;
}

// Define one vertex (color)
rf_extern void rf_gfx_color4f(float r, float g, float b, float a)
{
    rf_gfx_color4ub((unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255));
}

// Enable texture usage
rf_extern void rf_gfx_enable_texture(unsigned int id)
{
    if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].texture_id != id)
    {
        if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count > 0)
        {
            // Make sure current rf_ctx->gl_ctx.draws[i].vertex_count is aligned a multiple of 4,
            // that way, following QUADS drawing will keep aligned with index processing
            // It implies adding some extra alignment vertex at the end of the draw,
            // those vertex are not processed but they are considered as an additional offset
            // for the next set of vertex to be drawn
            if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == RF_LINES)
            {
                rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count : rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4);
            }
            else if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == RF_TRIANGLES)
            {
                rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? 1 : (4 - (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4)));
            }
            else
            {
                rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = 0;
            }

            if (rf_gfx_check_buffer_limit(rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment))
            {
                rf_gfx_draw();
            }
            else
            {
                rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;

                rf_batch.draw_calls_counter++;
            }
        }

        if (rf_batch.draw_calls_counter >= RF_DEFAULT_BATCH_DRAW_CALLS_COUNT)
        {
            rf_gfx_draw();
        }

        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].texture_id = id;
        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count = 0;
    }
}

// Disable texture usage
rf_extern void rf_gfx_disable_texture()
{
    // NOTE: If quads batch limit is reached,
    // we force a draw call and next batch starts
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter >= (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4))
    {
        rf_gfx_draw();
    }
}

// Set texture parameters (wrap mode/filter mode)
rf_extern void rf_gfx_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, texture.id);

    switch (wrap_mode)
    {
        case RF_WRAP_REPEAT:
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        } break;

        case RF_WRAP_CLAMP:
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } break;

        case RF_WRAP_MIRROR_REPEAT:
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        } break;

        case RF_WRAP_MIRROR_CLAMP:
        {
            if (rf_gfx.extensions.tex_mirror_clamp_supported)
            {
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_EXT);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_EXT);
            }
            else
            {
                rf_log(rf_log_type_warning, "Clamp mirror wrap mode not supported");
            }
        } break;

        default: break;
    }

    rf_gl.BindTexture(GL_TEXTURE_2D, 0);
}

// Set filter for texture
rf_extern void rf_gfx_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, texture.id);

    // Used only in case of anisotropic filters
    float anisotropic_value = 0;

    switch (filter_mode)
    {
        case RF_FILTER_POINT:
        {
            if (texture.mipmaps > 1)
            {
                // GL_NEAREST_MIPMAP_NEAREST - tex filter: POINT, mipmaps filter: POINT (sharp switching between mipmaps)
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

                // GL_NEAREST - tex filter: POINT (no filter), no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            else
            {
                // GL_NEAREST - tex filter: POINT (no filter), no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
        } break;

        case RF_FILTER_BILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // GL_LINEAR_MIPMAP_NEAREST - tex filter: BILINEAR, mipmaps filter: POINT (sharp switching between mipmaps)
                // Alternative: GL_NEAREST_MIPMAP_LINEAR - tex filter: POINT, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        } break;

        case RF_FILTER_TRILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // GL_LINEAR_MIPMAP_LINEAR - tex filter: BILINEAR, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                rf_log(rf_log_type_warning, "No mipmaps available for TRILINEAR texture filtering. Texture id: %d", texture.id);

                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        } break;

        case RF_FILTER_ANISOTROPIC_4x:  anisotropic_value = 4;
        case RF_FILTER_ANISOTROPIC_8x:  anisotropic_value = 8;
        case RF_FILTER_ANISOTROPIC_16x: anisotropic_value = 16;
        {
            if (anisotropic_value <= rf_gfx.extensions.max_anisotropic_level)
            {
                rf_gl.TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic_value);
            }
            else if (rf_gfx.extensions.max_anisotropic_level > 0.0f)
            {
                rf_log(rf_log_type_warning, "Maximum anisotropic filter level supported is %i_x. Texture id: %d", rf_gfx.extensions.max_anisotropic_level, texture.id);
                rf_gl.TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic_value);
            }
            else
            {
                rf_log(rf_log_type_warning, "Anisotropic filtering not supported");
            }
        } break;
    }

    rf_gl.BindTexture(GL_TEXTURE_2D, 0);
}

// Enable rendering to texture (fbo)
rf_extern void rf_gfx_enable_render_texture(unsigned int id)
{
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, id);

    //rf_gl.Disable(GL_CULL_FACE);    // Allow double side drawing for texture flipping
    //glCullFace(GL_FRONT);
}

// Disable rendering to texture
rf_extern void rf_gfx_disable_render_texture(void)
{
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    //rf_gl.Enable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}

// Enable depth test
rf_extern void rf_gfx_enable_depth_test(void) { rf_gl.Enable(GL_DEPTH_TEST); }

// Disable depth test
rf_extern void rf_gfx_disable_depth_test(void) { rf_gl.Disable(GL_DEPTH_TEST); }

// Enable backface culling
rf_extern void rf_gfx_enable_backface_culling(void) { rf_gl.Enable(GL_CULL_FACE); }

// Disable backface culling
rf_extern void rf_gfx_disable_backface_culling(void) { rf_gl.Disable(GL_CULL_FACE); }

// Enable scissor test
rf_extern void rf_gfx_enable_scissor_test(void) { rf_gl.Enable(GL_SCISSOR_TEST); }

// Disable scissor test
rf_extern void rf_gfx_disable_scissor_test(void) { rf_gl.Disable(GL_SCISSOR_TEST); }

// Scissor test
rf_extern void rf_gfx_scissor(int x, int y, int width, int height) { rf_gl.Scissor(x, y, width, height); }

// Enable wire mode
rf_extern void rf_gfx_enable_wire_mode(void)
{
#if defined(rayfork_graphics_backend_gl33)
    // NOTE: glPolygonMode() not available on OpenGL ES
    rf_gl.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}

// Disable wire mode
rf_extern void rf_gfx_disable_wire_mode(void)
{
#if defined(rayfork_graphics_backend_gl33)
    // NOTE: glPolygonMode() not available on OpenGL ES
    rf_gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

// Unload texture from GPU memory
rf_extern void rf_gfx_delete_textures(unsigned int id)
{
    if (id > 0) rf_gl.DeleteTextures(1, &id);
}

// Unload render texture from GPU memory
rf_extern void rf_gfx_delete_render_textures(rf_render_texture2d target)
{
    if (target.texture.id > 0) rf_gl.DeleteTextures(1, &target.texture.id);
    if (target.depth.id > 0)
    {
        if (target.depth_texture) rf_gl.DeleteTextures(1, &target.depth.id);
        else rf_gl.DeleteRenderbuffers(1, &target.depth.id);
    }

    if (target.id > 0) rf_gl.DeleteFramebuffers(1, &target.id);

    rf_log(rf_log_type_info, "[FBO ID %i] Unloaded render texture data from VRAM (GPU)", target.id);
}

// Unload shader from GPU memory
rf_extern void rf_gfx_delete_shader(unsigned int id)
{
    if (id != 0) rf_gl.DeleteProgram(id);
}

// Unload vertex data (VAO) from GPU memory
rf_extern void rf_gfx_delete_vertex_arrays(unsigned int id)
{
    if (id != 0) rf_gl.DeleteVertexArrays(1, &id);
    rf_log(rf_log_type_info, "[VAO ID %i] Unloaded model data from VRAM (GPU)", id);
}

// Unload vertex data (VBO) from GPU memory
rf_extern void rf_gfx_delete_buffers(unsigned int id)
{
    if (id != 0)
    {
        rf_gl.DeleteBuffers(1, &id);
        rf_log(rf_log_type_info, "[VBO ID %i] Unloaded model vertex data from VRAM (GPU)", id);
    }
}

// Clear color buffer with color
rf_extern void rf_gfx_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // rf_color values clamp to 0.0f(0) and 1.0f(255)
    float cr = (float)r/255;
    float cg = (float)g/255;
    float cb = (float)b/255;
    float ca = (float)a/255;

    rf_gl.ClearColor(cr, cg, cb, ca);
}

// Clear used screen buffers (color and depth)
rf_extern void rf_gfx_clear_screen_buffers(void)
{
    rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear used buffers: rf_color and Depth (Depth is used for 3D)
    //rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);     // Stencil buffer not used...
}

// Update GPU buffer with new data
rf_extern void rf_gfx_update_buffer(int buffer_id, void* data, int data_size)
{
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, buffer_id);
    rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);
}

// Load a new attributes buffer
rf_extern unsigned int rf_gfx_load_attrib_buffer(unsigned int vao_id, int shader_loc, void* buffer, int size, bool dynamic)
{
    unsigned int id = 0;

    int draw_hint = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    rf_gl.BindVertexArray(vao_id);

    rf_gl.GenBuffers(1, &id);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, id);
    rf_gl.BufferData(GL_ARRAY_BUFFER, size, buffer, draw_hint);
    rf_gl.VertexAttribPointer(shader_loc, 2, GL_FLOAT, 0, 0, 0);
    rf_gl.EnableVertexAttribArray(shader_loc);

    rf_gl.BindVertexArray(0);

    return id;
}

rf_extern void rf_gfx_init_vertex_buffer(rf_vertex_buffer* vertex_buffer)
{
    int elements_count = vertex_buffer->elements_count;

    // Initialize Quads VAO
    rf_gl.GenVertexArrays(1, &vertex_buffer->vao_id);
    rf_gl.BindVertexArray(vertex_buffer->vao_id);

    // Quads - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[0]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo_id[0]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, elements_count * rf_gfx_vertex_component_count, vertex_buffer->vertices, GL_DYNAMIC_DRAW);
    rf_gl.EnableVertexAttribArray(rf_ctx.current_shader.locs[RF_LOC_VERTEX_POSITION]);
    rf_gl.VertexAttribPointer(rf_ctx.current_shader.locs[RF_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

    // Vertex texcoord buffer (shader-location = 1)
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[1]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo_id[1]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, elements_count * rf_gdx_texcoord_component_count, vertex_buffer->texcoords, GL_DYNAMIC_DRAW);
    rf_gl.EnableVertexAttribArray(rf_ctx.current_shader.locs[RF_LOC_VERTEX_TEXCOORD01]);
    rf_gl.VertexAttribPointer(rf_ctx.current_shader.locs[RF_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[2]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo_id[2]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, elements_count * rf_gfx_color_component_count, vertex_buffer->colors, GL_DYNAMIC_DRAW);
    rf_gl.EnableVertexAttribArray(rf_ctx.current_shader.locs[RF_LOC_VERTEX_COLOR]);
    rf_gl.VertexAttribPointer(rf_ctx.current_shader.locs[RF_LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

    // Fill index buffer
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[3]);
    rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer->vbo_id[3]);
    rf_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, elements_count * rf_gfx_vertex_index_component_count, vertex_buffer->indices, GL_STATIC_DRAW);

    // Unbind
    rf_gl.BindVertexArray(0);
}

// Vertex Buffer Object deinitialization (memory free)
rf_extern void rf_gfx_close()
{
    rf_unlock_shader_default();              // Unload default shader
    rf_unload_buffers_default();             // Unload default buffers

    rf_gl.DeleteTextures(1, &rf_ctx.default_texture_id); // Unload default texture

    rf_log(rf_log_type_info, "[TEX ID %i] Unloaded texture data (base white texture) from VRAM", rf_ctx.default_texture_id);
}

// Update and draw internal buffers
rf_extern void rf_gfx_draw()
{
    // Only process data if we have data to process
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter > 0)
    {
        // Update default internal buffers (VAOs/VBOs) with vertex array data
        // NOTE: If there is not vertex data, buffers doesn't need to be updated (vertex_count > 0)
        // TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (change flag required)
        {
            // Update vertex buffers data
            // Activate elements VAO
            rf_gl.BindVertexArray(rf_batch.vertex_buffers[rf_batch.current_buffer].vao_id);

            // Vertex positions buffer
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, rf_batch.vertex_buffers[rf_batch.current_buffer].vbo_id[0]);
            rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter, rf_batch.vertex_buffers[rf_batch.current_buffer].vertices);
            //rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 4 * rf_max_batch_elements, rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].vertices, GL_DYNAMIC_DRAW);  // Update all buffer

            // rf_texture coordinates buffer
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, rf_batch.vertex_buffers[rf_batch.current_buffer].vbo_id[1]);
            rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter, rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords);
            //rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4 * rf_max_batch_elements, rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].texcoords, GL_DYNAMIC_DRAW); // Update all buffer

            // Colors buffer
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, rf_batch.vertex_buffers[rf_batch.current_buffer].vbo_id[2]);
            rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char) * 4 * rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter, rf_batch.vertex_buffers[rf_batch.current_buffer].colors);
            //rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4 * rf_max_batch_elements, rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].colors, GL_DYNAMIC_DRAW);    // Update all buffer

            // NOTE: glMap_buffer() causes sync issue.
            // If GPU is working with this buffer, glMap_buffer() will wait(stall) until GPU to finish its job.
            // To avoid waiting (idle), you can call first rf_gl.BufferData() with NULL pointer before glMap_buffer().
            // If you do that, the previous data in PBO will be discarded and glMap_buffer() returns a new
            // allocated pointer immediately even if GPU is still working with the previous data.

            // Another option: map the buffer object into client's memory
            // Probably this code could be moved somewhere else...
            // rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].vertices = (float* )glMap_buffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            // if (rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].vertices)
            // {
            // Update vertex data
            // }
            // glUnmap_buffer(GL_ARRAY_BUFFER);

            // Unbind the current VAO
            rf_gl.BindVertexArray(0);
        }

        // NOTE: Stereo rendering is checked inside
        //rf_draw_buffers_default();
        {
            // Draw default internal buffers vertex data
            rf_mat mat_projection = rf_ctx.projection;
            rf_mat mat_model_view = rf_ctx.modelview;

            // Draw buffers
            if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter > 0)
            {
                // Set current shader and upload current MVP matrix
                rf_gl.UseProgram(rf_ctx.current_shader.id);

                // Create rf_ctx->gl_ctx.modelview-rf_ctx->gl_ctx.projection matrix
                rf_mat mat_mvp = rf_mat_mul(rf_ctx.modelview, rf_ctx.projection);

                rf_gl.UniformMatrix4fv(rf_ctx.current_shader.locs[RF_LOC_MATRIX_MVP], 1, 0, rf_mat_to_float16(mat_mvp).v);
                rf_gl.Uniform4f(rf_ctx.current_shader.locs[RF_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
                rf_gl.Uniform1i(rf_ctx.current_shader.locs[RF_LOC_MAP_DIFFUSE], 0);    // Provided value refers to the texture unit (active)

                // TODO: Support additional texture units on custom shader
                //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_SPECULAR] > 0) rf_gl.Uniform1i(rf_ctx->gl_ctx.current_shader.locs[RF_LOC_MAP_SPECULAR], 1);
                //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_NORMAL] > 0) rf_gl.Uniform1i(rf_ctx->gl_ctx.current_shader.locs[RF_LOC_MAP_NORMAL], 2);

                // NOTE: Right now additional map textures not considered for default buffers drawing

                int vertex_offset = 0;

                rf_gl.BindVertexArray(rf_batch.vertex_buffers[rf_batch.current_buffer].vao_id);

                rf_gl.ActiveTexture(GL_TEXTURE0);

                for (rf_int i = 0; i < rf_batch.draw_calls_counter; i++)
                {
                    rf_gl.BindTexture(GL_TEXTURE_2D, rf_batch.draw_calls[i].texture_id);

                    // TODO: Find some way to bind additional textures --> Use global texture IDs? Register them on draw[i]?
                    //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_SPECULAR] > 0) { rf_gl.ActiveTexture(GL_TEXTURE1); rf_gl.BindTexture(GL_TEXTURE_2D, textureUnit1_id); }
                    //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_SPECULAR] > 0) { rf_gl.ActiveTexture(GL_TEXTURE2); rf_gl.BindTexture(GL_TEXTURE_2D, textureUnit2_id); }

                    if ((rf_batch.draw_calls[i].mode == RF_LINES) || (rf_batch.draw_calls[i].mode == RF_TRIANGLES))
                    {
                        rf_gl.DrawArrays(rf_batch.draw_calls[i].mode, vertex_offset, rf_batch.draw_calls[i].vertex_count);
                    }
                    else
                    {
                        #if defined(rayfork_graphics_backend_gl33)
                        // We need to define the number of indices to be processed: quadsCount*6
                        // NOTE: The final parameter tells the GPU the offset in bytes from the
                        // start of the index buffer to the location of the first index to process
                        rf_gl.DrawElements(GL_TRIANGLES, rf_batch.draw_calls[i].vertex_count / 4 * 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * vertex_offset / 4 * 6));
                        #elif defined(rayfork_graphics_backend_gl_es3)
                        rf_gl.DrawElements(GL_TRIANGLES, rf_batch.draw_calls[i].vertex_count / 4 * 6, GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * vertex_offset / 4 * 6));
                        #endif
                    }

                    vertex_offset += (rf_batch.draw_calls[i].vertex_count + rf_batch.draw_calls[i].vertex_alignment);
                }

                rf_gl.BindTexture(GL_TEXTURE_2D, 0);
            }

            rf_gl.BindVertexArray(0);
            rf_gl.UseProgram(0);

            // Reset vertex counters for next frame
            rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter  = 0;
            rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter = 0;
            rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter  = 0;

            // Reset depth for next draw
            rf_batch.current_depth = -1.0f;

            // Restore rf_ctx->gl_ctx.projection/rf_ctx->gl_ctx.modelview matrices
            rf_ctx.projection = mat_projection;
            rf_ctx.modelview  = mat_model_view;

            // Reset rf_ctx->gl_ctx.draws array
            for (rf_int i = 0; i < RF_DEFAULT_BATCH_DRAW_CALLS_COUNT; i++)
            {
                rf_batch.draw_calls[i].mode = RF_QUADS;
                rf_batch.draw_calls[i].vertex_count = 0;
                rf_batch.draw_calls[i].texture_id = rf_ctx.default_texture_id;
            }

            rf_batch.draw_calls_counter = 1;

            // Change to next buffer in the list
            rf_batch.current_buffer++;
            if (rf_batch.current_buffer >= RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT) rf_batch.current_buffer = 0;
        }
    }
}

// Check internal buffer overflow for a given number of vertex
rf_extern bool rf_gfx_check_buffer_limit(int v_count)
{
    return (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter + v_count) >= (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4);
}

// Set debug marker
rf_extern void rf_gfx_set_debug_marker(const char* text)
{
#if defined(rayfork_graphics_backend_gl33)
    //if (rf_ctx->gl_ctx.debug_marker_supported) glInsertEventMarkerEXT(0, text);
#endif
}

// Convert image data to OpenGL texture (returns OpenGL valid Id)
rf_extern unsigned int rf_gfx_load_texture(void* data, int width, int height, rf_pixel_format format, int mipmap_count)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, 0); // Free any old binding

    unsigned int id = 0;

    // Check texture format support by OpenGL 1.1 (compressed textures not supported)
    if ((!rf_gfx.extensions.tex_comp_dxt_supported) && ((format == rf_pixel_format_dxt1_rgb) || (format == rf_pixel_format_dxt1_rgba) || (format == rf_pixel_format_dxt3_rgba) || (format == rf_pixel_format_dxt5_rgba)))
    {
        rf_log(rf_log_type_warning, "DXT compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_etc1_supported) && (format == rf_pixel_format_etc1_rgb))
    {
        rf_log(rf_log_type_warning, "ETC1 compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_etc2_supported) && ((format == rf_pixel_format_etc2_rgb) || (format == rf_pixel_format_etc2_eac_rgba)))
    {
        rf_log(rf_log_type_warning, "ETC2 compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_pvrt_supported) && ((format == rf_pixel_format_pvrt_rgb) || (format == rf_pixel_format_prvt_rgba)))
    {
        rf_log(rf_log_type_warning, "PVRT compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_astc_supported) && ((format == rf_pixel_format_astc_4x4_rgba) || (format == rf_pixel_format_astc_8x8_rgba)))
    {
        rf_log(rf_log_type_warning, "ASTC compressed texture format not supported");
        return id;
    }

    rf_gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    rf_gl.GenTextures(1, &id);              // Generate texture id

    //rf_gl.ActiveTexture(GL_TEXTURE0);     // If not defined, using GL_TEXTURE0 by default (shader texture)

    rf_gl.BindTexture(GL_TEXTURE_2D, id);

    int mip_width = width;
    int mip_height = height;
    int mip_offset = 0;          // Mipmap data offset

    rf_log(rf_log_type_debug, "Load texture from data memory address: 0x%x", data);

    // Load the different mipmap levels
    for (rf_int i = 0; i < mipmap_count; i++)
    {
        int mip_size = rf_image_size_in_format(mip_width, mip_height, format);

        rf_gfx_pixel_format glformat = rf_gfx_get_internal_texture_formats(format);

        rf_log(rf_log_type_debug, "Load mipmap level %i (%i x %i), size: %i, offset: %i", i, mip_width, mip_height, mip_size, mip_offset);

        if (glformat.valid)
        {
            if (rf_is_uncompressed_format(format))
            {
                rf_gl.TexImage2D(GL_TEXTURE_2D, i, glformat.internal_format, mip_width, mip_height, 0, glformat.format, glformat.type, (unsigned char* )data + mip_offset);
            }
            else
            {
                rf_gl.CompressedTexImage2D(GL_TEXTURE_2D, i, glformat.internal_format, mip_width, mip_height, 0, mip_size, (unsigned char *)data + mip_offset);
            }

            #if defined(rayfork_graphics_backend_gl33)
                if (format == rf_pixel_format_grayscale)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
                    rf_gl.TexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
                else if (format == rf_pixel_format_gray_alpha)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
                    rf_gl.TexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
            #endif
        }

        mip_width /= 2;
        mip_height /= 2;
        mip_offset += mip_size;

        // Security check for NPOT textures
        if (mip_width < 1) mip_width = 1;
        if (mip_height < 1) mip_height = 1;
    }

    // rf_texture parameters configuration
    // NOTE: rf_gl.TexParameteri does NOT affect texture uploading, just the way it's used
    #if defined(rayfork_graphics_backend_gl_es3)
        // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
        if (rf_gfx.extensions.tex_npot_supported)
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
        }
        else
        {
            // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis
        }
    #else
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    #endif

    // Magnification and minification filters
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR

    #if defined(rayfork_graphics_backend_gl33)
    if (mipmap_count > 1)
    {
        // Activate Trilinear filtering if mipmaps are available
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    #endif

    // At this point we have the texture loaded in GPU and texture parameters configured

    // NOTE: If mipmaps were not in data, they are not generated automatically

    // Unbind current texture
    rf_gl.BindTexture(GL_TEXTURE_2D, 0);

    if (id > 0) rf_log(rf_log_type_info, "[TEX ID %i] rf_texture created successfully (%ix%i - %i mipmaps)", id, width, height, mipmap_count);
    else rf_log(rf_log_type_warning, "rf_texture could not be created");

    return id;
}

// Load depth texture/renderbuffer (to be attached to fbo)
// WARNING: OpenGL ES 2.0 requires GL_OES_depth_texture/WEBGL_depth_texture extensions
rf_extern unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, bool use_render_buffer)
{
    unsigned int id = 0;

    unsigned int glInternalFormat = GL_DEPTH_COMPONENT16;

    if ((bits != 16) && (bits != 24) && (bits != 32)) bits = 16;

    if (bits == 24)
    {
        glInternalFormat = GL_DEPTH_COMPONENT24;
    }

    if (bits == 32 && rf_gfx.extensions.max_depth_bits == 32) // Check max_depth_bits to make sure its ok on mobile
    {
        glInternalFormat = GL_DEPTH_COMPONENT32;
    }

    if (!use_render_buffer && rf_gfx.extensions.tex_depth_supported)
    {
        rf_gl.GenTextures(1, &id);
        rf_gl.BindTexture(GL_TEXTURE_2D, id);
        rf_gl.TexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        rf_gl.BindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        // Create the renderbuffer that will serve as the depth attachment for the framebuffer
        // NOTE: A renderbuffer is simpler than a texture and could offer better performance on embedded devices
        rf_gl.GenRenderbuffers(1, &id);
        rf_gl.BindRenderbuffer(GL_RENDERBUFFER, id);
        rf_gl.RenderbufferStorage(GL_RENDERBUFFER, glInternalFormat, width, height);

        rf_gl.BindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    return id;
}

// Load texture cubemap
// NOTE: Cubemap data is expected to be 6 images in a single column,
// expected the following convention: +X, -X, +Y, -Y, +Z, -Z
rf_extern unsigned int rf_gfx_load_texture_cubemap(void* data, int size, rf_pixel_format format)
{
    unsigned int cubemap_id = 0;
    unsigned int data_size = size * size * rf_bytes_per_pixel(format);

    rf_gl.GenTextures(1, &cubemap_id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);

    rf_gfx_pixel_format glformat = rf_gfx_get_internal_texture_formats(format);

    if (glformat.valid)
    {
        // Load cubemap faces
        for (unsigned int i = 0; i < 6; i++)
        {
            if (rf_is_uncompressed_format(format)) rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glformat.internal_format, size, size, 0, glformat.format, glformat.type, (unsigned char* )data + i * data_size);
            else rf_gl.CompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glformat.internal_format, size, size, 0, data_size, (unsigned char* )data + i*data_size);

            #if defined(rayfork_graphics_backend_gl33)
                if (format == rf_pixel_format_grayscale)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
                    rf_gl.TexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
                else if (format == rf_pixel_format_gray_alpha)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
                    rf_gl.TexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
            #endif
        }
    }

    // Set cubemap texture sampling parameters
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #if defined(rayfork_graphics_backend_gl33)
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  // Flag not supported on OpenGL ES 2.0
    #endif // defined(rayfork_graphics_backend_gl33)

    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubemap_id;
}

// Update already loaded texture in GPU with new data
// NOTE: We don't know safely if internal texture format is the expected one...
rf_extern void rf_gfx_update_texture(unsigned int id, int width, int height, rf_pixel_format format, const void* pixels, int pixels_size)
{
    if (width * height * rf_bytes_per_pixel(format) <= pixels_size) return;

    rf_gl.BindTexture(GL_TEXTURE_2D, id);

    rf_gfx_pixel_format gfx_format = rf_gfx_get_internal_texture_formats(format);

    if (gfx_format.valid && rf_is_uncompressed_format(format))
    {
        rf_gl.TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, gfx_format.internal_format, gfx_format.type, (unsigned char*) pixels);
    }
    else rf_log(rf_log_type_warning, "rf_texture format updating not supported");
}

// Get OpenGL internal formats and data type from raylib rf_pixel_format
rf_extern rf_gfx_pixel_format rf_gfx_get_internal_texture_formats(rf_pixel_format format)
{
    rf_gfx_pixel_format result = {.valid = 1};

    switch (format)
    {
        #if defined(rayfork_graphics_backend_gl_es3)
        // NOTE: on OpenGL ES 2.0 (WebGL), internalFormat must match format and options allowed are: GL_LUMINANCE, GL_RGB, GL_RGBA
        case rf_pixel_format_grayscale: result.internal_format = GL_LUMINANCE; result.format = GL_LUMINANCE; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_gray_alpha: result.internal_format = GL_LUMINANCE_ALPHA; result.format = GL_LUMINANCE_ALPHA; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r5g6b5: result.internal_format = GL_RGB; result.format = GL_RGB; result.type = GL_UNSIGNED_SHORT_5_6_5; break;
        case rf_pixel_format_r8g8b8: result.internal_format = GL_RGB; result.format = GL_RGB; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r5g5b5a1: result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case rf_pixel_format_r4g4b4a4: result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case rf_pixel_format_r8g8b8a8: result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r32:          if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_LUMINANCE; result.format = GL_LUMINANCE; result.type = GL_FLOAT; break;   // NOTE: Requires extension OES_texture_float
        case rf_pixel_format_r32g32b32:    if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGB; result.format = GL_RGB; result.type = GL_FLOAT; break;         // NOTE: Requires extension OES_texture_float
        case rf_pixel_format_r32g32b32a32: if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_FLOAT; break;    // NOTE: Requires extension OES_texture_float

        #elif defined(rayfork_graphics_backend_gl33)
        //case rf_pixel_format_r5g6b5:       result.internal_format = GL_RGB565; result.format = GL_RGB; result.type = GL_UNSIGNED_SHORT_5_6_5; break;
        case rf_pixel_format_grayscale:    result.internal_format = GL_R8; result.format = GL_RED; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_gray_alpha:   result.internal_format = GL_RG8; result.format = GL_RG; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r8g8b8:       result.internal_format = GL_RGB8; result.format = GL_RGB; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r5g5b5a1:     result.internal_format = GL_RGB5_A1; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case rf_pixel_format_r4g4b4a4:     result.internal_format = GL_RGBA4; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case rf_pixel_format_r8g8b8a8:     result.internal_format = GL_RGBA8; result.format = GL_RGBA; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r32:          if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_R32F; result.format = GL_RED; result.type = GL_FLOAT; break;
        case rf_pixel_format_r32g32b32:    if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGB32F; result.format = GL_RGB; result.type = GL_FLOAT; break;
        case rf_pixel_format_r32g32b32a32: if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGBA32F; result.format = GL_RGBA; result.type = GL_FLOAT; break;
        #endif

        case rf_pixel_format_dxt1_rgb:      if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
        case rf_pixel_format_dxt1_rgba:     if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
        case rf_pixel_format_dxt3_rgba:     if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        case rf_pixel_format_dxt5_rgba:     if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
        case rf_pixel_format_etc1_rgb:      if (rf_gfx.extensions.tex_comp_etc1_supported) result.internal_format = GL_ETC1_RGB8_OES; break;                      // NOTE: Requires OpenGL ES 2.0 or OpenGL 4.3
        case rf_pixel_format_etc2_rgb:      if (rf_gfx.extensions.tex_comp_etc2_supported) result.internal_format = GL_COMPRESSED_RGB8_ETC2; break;               // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
        case rf_pixel_format_etc2_eac_rgba: if (rf_gfx.extensions.tex_comp_etc2_supported) result.internal_format = GL_COMPRESSED_RGBA8_ETC2_EAC; break;     // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
        case rf_pixel_format_pvrt_rgb:      if (rf_gfx.extensions.tex_comp_pvrt_supported) result.internal_format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; break;    // NOTE: Requires PowerVR GPU
        case rf_pixel_format_prvt_rgba:     if (rf_gfx.extensions.tex_comp_pvrt_supported) result.internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; break;  // NOTE: Requires PowerVR GPU
        case rf_pixel_format_astc_4x4_rgba: if (rf_gfx.extensions.tex_comp_astc_supported) result.internal_format = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3
        case rf_pixel_format_astc_8x8_rgba: if (rf_gfx.extensions.tex_comp_astc_supported) result.internal_format = GL_COMPRESSED_RGBA_ASTC_8x8_KHR; break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3

        default:
            rf_log(rf_log_type_warning, "rf_texture format not supported");
            result.valid = 0;
            break;
    }

    return result;
}

// Unload texture from GPU memory
rf_extern void rf_gfx_unload_texture(unsigned int id)
{
    if (id > 0) rf_gl.DeleteTextures(1, &id);
}

// Generate mipmap data for selected texture
rf_extern void rf_gfx_generate_mipmaps(rf_texture2d* texture)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, texture->id);

    // Check if texture is power-of-two (POT)
    bool tex_is_pot = 0;

    if (((texture->width > 0) && ((texture->width & (texture->width - 1)) == 0)) &&
        ((texture->height > 0) && ((texture->height & (texture->height - 1)) == 0))) tex_is_pot = 1;

    if ((tex_is_pot) || (rf_gfx.extensions.tex_npot_supported))
    {
        //glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);   // Hint for mipmaps generation algorythm: GL_FASTEST, GL_NICEST, GL_DONT_CARE
        rf_gl.GenerateMipmap(GL_TEXTURE_2D);    // Generate mipmaps automatically
        rf_log(rf_log_type_info, "[TEX ID %i] Mipmaps generated automatically", texture->id);

        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);   // Activate Trilinear filtering for mipmaps

        texture->mipmaps =  1 + (int)floor(log(texture->width > texture->height ? texture->width : texture->height)/log(2));
    }
    else rf_log(rf_log_type_warning, "[TEX ID %i] Mipmaps can not be generated", texture->id);

    rf_gl.BindTexture(GL_TEXTURE_2D, 0);
}

// Read texture pixel data
rf_extern rf_image rf_gfx_read_texture_pixels_to_buffer(rf_texture2d texture, void* dst, int dst_size)
{
    if (!texture.valid || !dst || !dst_size) return (rf_image) {0};

    rf_image result = {0};

    #if defined(rayfork_graphics_backend_gl33)
    {
        rf_gl.BindTexture(GL_TEXTURE_2D, texture.id);

        /*
        NOTE: Using texture.id, we can retrieve some texture info (but not on OpenGL ES 2.0)
        Possible texture info: GL_TEXTURE_RED_SIZE, GL_TEXTURE_GREEN_SIZE, GL_TEXTURE_BLUE_SIZE, GL_TEXTURE_ALPHA_SIZE
        int width, height, format;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

        NOTE: Each row written to or read from by OpenGL pixel operations like rf_gl.GetTexImage are aligned to a 4 unsigned char boundary by default, which may add some padding.
        Use rf_gl.PixelStorei to modify padding with the GL_[UN]PACK_ALIGNMENT setting.
        GL_PACK_ALIGNMENT affects operations that read from OpenGL memory (rf_gl.ReadPixels, rf_gl.GetTexImage, etc.)
        GL_UNPACK_ALIGNMENT affects operations that write to OpenGL memory (glTexImage, etc.)
        */

        rf_gl.PixelStorei(GL_PACK_ALIGNMENT, 1);

        rf_gfx_pixel_format format = rf_gfx_get_internal_texture_formats(texture.format);
        int size = texture.width * texture.height * rf_bytes_per_pixel(texture.format);

        if (format.valid && rf_is_uncompressed_format(texture.format) && size <= dst_size)
        {
            rf_gl.GetTexImage(GL_TEXTURE_2D, 0, format.format, format.type, dst);

            result = (rf_image) {
                .width = texture.width,
                .height = texture.height,
                .format = texture.format,
                .data = dst,
                .valid = 1,
            };
        }
        else rf_log(rf_log_type_warning, "rf_texture data retrieval not suported for pixel format");

        rf_gl.BindTexture(GL_TEXTURE_2D, 0);
    }
    #elif defined(rayfork_graphics_backend_gl_es3)
    {
        /*
        rf_gl.GetTexImage() is not available on OpenGL ES 2.0
        rf_texture2d width and height are required on OpenGL ES 2.0. There is no way to get it from texture id.
        Two possible Options:
        1 - Bind texture to color fbo attachment and rf_gl.ReadPixels()
        2 - Create an fbo, activate it, render quad with texture, rf_gl.ReadPixels()
        We are using Option 1, just need to care for texture format on retrieval
        NOTE: This behaviour could be conditioned by graphic driver...
        */
        rf_render_texture2d fbo = rf_gfx_load_render_texture(texture.width, texture.height, rf_pixel_format_r8g8b8a8, 16, 0);

        rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo.id);
        rf_gl.BindTexture(GL_TEXTURE_2D, 0);

        // Attach our texture to FBO
        // NOTE: Previoust attached texture is automatically detached
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id, 0);

        // We read data as RGBA because FBO texture is configured as RGBA, despite binding another texture format
        if (texture.width * texture.height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8) <= dst_size)
        {
            rf_gl.ReadPixels(0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, dst);

            result = (rf_image) {
                .width = texture.width,
                .height = texture.height,
                .format = texture.format,
                .data = dst,
                .valid = 1,
            };
        }

        // Re-attach internal FBO color texture before deleting it
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture.id, 0);

        rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clean up temporal fbo
        rf_gfx_delete_render_textures(fbo);
    }
    #endif

    return result;
}

rf_extern rf_image rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator)
{
    if (!texture.valid) return (rf_image) {0};

    int size = texture.width * texture.height * rf_bytes_per_pixel(texture.format);
    void* dst = rf_alloc(allocator, size);
    rf_image result = rf_gfx_read_texture_pixels_to_buffer(texture, dst, size);

    if (!result.valid) rf_free(allocator, dst);

    return result;
}

// Read screen pixel data (color buffer)
rf_extern void rf_gfx_read_screen_pixels(rf_color* dst, int width, int height)
{
    // NOTE 1: glReadPixels returns image flipped vertically -> (0,0) is the bottom left corner of the framebuffer
    // NOTE 2: We are getting alpha channel! Be careful, it can be transparent if not cleared properly!
    rf_gl.ReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, dst);

    for (rf_int y = height - 1; y >= 0; y--)
    {
        for (rf_int x = 0; x < width; x++)
        {
            dst[((height - 1) - y) * width + x] = dst[(y * width) + x]; // Flip line

            // Set alpha component value to 255 (no trasparent image retrieval)
            // NOTE: Alpha value has already been applied to RGB in framebuffer, we don't need it!
            if (((x + 1) % 4) == 0) dst[((height - 1) - y) * width + x].a = 255;
        }
    }
}

// Load a texture to be used for rendering (fbo with default color and depth attachments)
// NOTE: If colorFormat or depth_bits are no supported, no attachment is done
rf_extern rf_render_texture2d rf_gfx_load_render_texture(int width, int height, rf_pixel_format format, int depth_bits, bool use_depth_texture)
{
    rf_render_texture2d target = { 0 };

    if (use_depth_texture && rf_gfx.extensions.tex_depth_supported) target.depth_texture = 1;

    // Create the framebuffer object
    rf_gl.GenFramebuffers(1, &target.id);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, target.id);

    // Create fbo color texture attachment
    //-----------------------------------------------------------------------------------------------------
    if (rf_is_uncompressed_format(format))
    {
        // WARNING: Some texture formats are not supported for fbo color attachment
        target.texture.id = rf_gfx_load_texture(NULL, width, height, format, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = format;
        target.texture.mipmaps = 1;
    }
    //-----------------------------------------------------------------------------------------------------

    // Create fbo depth renderbuffer/texture
    //-----------------------------------------------------------------------------------------------------
    if (depth_bits > 0)
    {
        target.depth.id = rf_gfx_load_texture_depth(width, height, depth_bits, !use_depth_texture);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;
    }
    //-----------------------------------------------------------------------------------------------------

    // Attach color texture and depth renderbuffer to FBO
    //-----------------------------------------------------------------------------------------------------
    rf_gfx_render_texture_attach(target, target.texture.id, 0);    // COLOR attachment
    rf_gfx_render_texture_attach(target, target.depth.id, 1);      // DEPTH attachment
    //-----------------------------------------------------------------------------------------------------

    // Check if fbo is complete with attachments (valid)
    //-----------------------------------------------------------------------------------------------------
    if (rf_gfx_render_texture_complete(target)) rf_log(rf_log_type_info, "[FBO ID %i] Framebuffer object created successfully", target.id);
    //-----------------------------------------------------------------------------------------------------

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    return target;
}

// Attach color buffer texture to an fbo (unloads previous attachment)
// NOTE: Attach type: 0-rf_color, 1-Depth renderbuffer, 2-Depth texture
rf_extern void rf_gfx_render_texture_attach(rf_render_texture2d target, unsigned int id, int attach_type)
{
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, target.id);

    if (attach_type == 0) rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
    else if (attach_type == 1)
    {
        if (target.depth_texture) rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
        else rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);
    }

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Verify render texture is complete
rf_extern bool rf_gfx_render_texture_complete(rf_render_texture2d target)
{
    bool result = 0;

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, target.id);

    unsigned int status = rf_gl.CheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (status)
        {
            case GL_FRAMEBUFFER_UNSUPPORTED: rf_log(rf_log_type_warning, "Framebuffer is unsupported"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: rf_log(rf_log_type_warning, "Framebuffer has incomplete attachment"); break;

            #if defined(rayfork_graphics_backend_gl_es3)
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: rf_log(rf_log_type_warning, "Framebuffer has incomplete dimensions"); break;
            #endif

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: rf_log(rf_log_type_warning, "Framebuffer has a missing attachment"); break;
            default: break;
        }
    }

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    result = (status == GL_FRAMEBUFFER_COMPLETE);

    return result;
}

// Upload vertex data into a VAO (if supported) and VBO
rf_extern void rf_gfx_load_mesh(rf_mesh* mesh, bool dynamic)
{
    if (mesh->vao_id > 0)
    {
        // Check if mesh has already been loaded in GPU
        rf_log(rf_log_type_warning, "Trying to re-load an already loaded mesh");
        return;
    }

    mesh->vao_id = 0;        // Vertex Array Object
    mesh->vbo_id[0] = 0;     // Vertex positions VBO
    mesh->vbo_id[1] = 0;     // Vertex texcoords VBO
    mesh->vbo_id[2] = 0;     // Vertex normals VBO
    mesh->vbo_id[3] = 0;     // Vertex colors VBO
    mesh->vbo_id[4] = 0;     // Vertex tangents VBO
    mesh->vbo_id[5] = 0;     // Vertex texcoords2 VBO
    mesh->vbo_id[6] = 0;     // Vertex indices VBO

    int draw_hint = GL_STATIC_DRAW;
    if (dynamic) draw_hint = GL_DYNAMIC_DRAW;

    // Initialize Quads VAO (Buffer A)
    rf_gl.GenVertexArrays(1, &mesh->vao_id);
    rf_gl.BindVertexArray(mesh->vao_id);


    // NOTE: Attributes must be uploaded considering default locations points

    // Enable vertex attributes: position (shader-location = 0)
    rf_gl.GenBuffers(1, &mesh->vbo_id[0]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[0]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*mesh->vertex_count, mesh->vertices, draw_hint);
    rf_gl.VertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    rf_gl.EnableVertexAttribArray(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    rf_gl.GenBuffers(1, &mesh->vbo_id[1]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[1]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*mesh->vertex_count, mesh->texcoords, draw_hint);
    rf_gl.VertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    rf_gl.EnableVertexAttribArray(1);

    // Enable vertex attributes: normals (shader-location = 2)
    if (mesh->normals != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[2]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[2]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*mesh->vertex_count, mesh->normals, draw_hint);
        rf_gl.VertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
        rf_gl.EnableVertexAttribArray(2);
    }
    else
    {
        // Default color vertex attribute set to RF_WHITE
        rf_gl.VertexAttrib3f(2, 1.0f, 1.0f, 1.0f);
        rf_gl.DisableVertexAttribArray(2);
    }

    // Default color vertex attribute (shader-location = 3)
    if (mesh->colors != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[3]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[3]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(unsigned char) * 4 * mesh->vertex_count, mesh->colors, draw_hint);
        rf_gl.VertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        rf_gl.EnableVertexAttribArray(3);
    }
    else
    {
        // Default color vertex attribute set to RF_WHITE
        rf_gl.VertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);
        rf_gl.DisableVertexAttribArray(3);
    }

    // Default tangent vertex attribute (shader-location = 4)
    if (mesh->tangents != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[4]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[4]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * mesh->vertex_count, mesh->tangents, draw_hint);
        rf_gl.VertexAttribPointer(4, 4, GL_FLOAT, 0, 0, 0);
        rf_gl.EnableVertexAttribArray(4);
    }
    else
    {
        // Default tangents vertex attribute
        rf_gl.VertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);
        rf_gl.DisableVertexAttribArray(4);
    }

    // Default texcoord2 vertex attribute (shader-location = 5)
    if (mesh->texcoords2 != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[5]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[5]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*mesh->vertex_count, mesh->texcoords2, draw_hint);
        rf_gl.VertexAttribPointer(5, 2, GL_FLOAT, 0, 0, 0);
        rf_gl.EnableVertexAttribArray(5);
    }
    else
    {
        // Default texcoord2 vertex attribute
        rf_gl.VertexAttrib2f(5, 0.0f, 0.0f);
        rf_gl.DisableVertexAttribArray(5);
    }

    if (mesh->indices != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[6]);
        rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_id[6]);
        rf_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*mesh->triangle_count * 3, mesh->indices, draw_hint);
    }

    if (mesh->vao_id > 0) rf_log(rf_log_type_info, "[VAO ID %i] rf_mesh uploaded successfully to VRAM (GPU)", mesh->vao_id);
    else rf_log(rf_log_type_warning, "rf_mesh could not be uploaded to VRAM (GPU)");
}

// Update vertex or index data on GPU (upload new data to one buffer)
rf_extern void rf_gfx_update_mesh(rf_mesh mesh, int buffer, int num)
{
    rf_gfx_update_mesh_at(mesh, buffer, num, 0);
}

// Update vertex or index data on GPU, at index
// WARNING: error checking is in place that will cause the data to not be
//          updated if offset + size exceeds what the buffer can hold
rf_extern void rf_gfx_update_mesh_at(rf_mesh mesh, int buffer, int num, int index)
{
    // Activate mesh VAO
    rf_gl.BindVertexArray(mesh.vao_id);

    switch (buffer)
    {
        case 0:     // Update vertices (vertex position)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[0]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*num, mesh.vertices, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 3*index, sizeof(float) * 3*num, mesh.vertices);

        } break;
        case 1:     // Update texcoords (vertex texture coordinates)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[1]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*num, mesh.texcoords, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2*index, sizeof(float) * 2*num, mesh.texcoords);

        } break;
        case 2:     // Update normals (vertex normals)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[2]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*num, mesh.normals, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 3*index, sizeof(float) * 3*num, mesh.normals);

        } break;
        case 3:     // Update colors (vertex colors)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[3]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, mesh.colors, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(unsigned char) * 4 * index, sizeof(unsigned char) * 4 * num, mesh.colors);

        } break;
        case 4:     // Update tangents (vertex tangents)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[4]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, mesh.tangents, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 4 * index, sizeof(float) * 4 * num, mesh.tangents);
        } break;
        case 5:     // Update texcoords2 (vertex second texture coordinates)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[5]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*num, mesh.texcoords2, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2*index, sizeof(float) * 2*num, mesh.texcoords2);
        } break;
        case 6:     // Update indices (triangle index buffer)
        {
            // the * 3 is because each triangle has 3 indices
            unsigned short *indices = mesh.indices;
            rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbo_id[6]);
            if (index == 0 && num >= mesh.triangle_count)
                rf_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices)*num * 3, indices, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.triangle_count)
                break;
            else
                rf_gl.BufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices)*index * 3, sizeof(*indices)*num * 3, indices);
        } break;
        default: break;
    }

    // Unbind the current VAO
    rf_gl.BindVertexArray(0);

    // Another option would be using buffer mapping...
    //mesh.vertices = glMap_buffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    // Now we can modify vertices
    //glUnmap_buffer(GL_ARRAY_BUFFER);
}

// Draw a 3d mesh with material and transform
rf_extern void rf_gfx_draw_mesh(rf_mesh mesh, rf_material material, rf_mat transform)
{
    // Bind shader program
    rf_gl.UseProgram(material.shader.id);

    // Matrices and other values required by shader
    //-----------------------------------------------------
    // Calculate and send to shader model matrix (used by PBR shader)
    if (material.shader.locs[RF_LOC_MATRIX_MODEL] != -1)
        rf_gfx_set_shader_value_matrix(material.shader, material.shader.locs[RF_LOC_MATRIX_MODEL], transform);

    // Upload to shader material.col_diffuse
    if (material.shader.locs[RF_LOC_COLOR_DIFFUSE] != -1)
        rf_gl.Uniform4f(material.shader.locs[RF_LOC_COLOR_DIFFUSE], (float)material.maps[RF_MAP_DIFFUSE].color.r / 255.0f,
                    (float)material.maps[RF_MAP_DIFFUSE].color.g / 255.0f,
                    (float)material.maps[RF_MAP_DIFFUSE].color.b / 255.0f,
                    (float)material.maps[RF_MAP_DIFFUSE].color.a / 255.0f);

    // Upload to shader material.colSpecular (if available)
    if (material.shader.locs[RF_LOC_COLOR_SPECULAR] != -1)
        rf_gl.Uniform4f(material.shader.locs[RF_LOC_COLOR_SPECULAR], (float)material.maps[RF_MAP_SPECULAR].color.r / 255.0f,
                    (float)material.maps[RF_MAP_SPECULAR].color.g / 255.0f,
                    (float)material.maps[RF_MAP_SPECULAR].color.b / 255.0f,
                    (float)material.maps[RF_MAP_SPECULAR].color.a / 255.0f);

    if (material.shader.locs[RF_LOC_MATRIX_VIEW] != -1)
        rf_gfx_set_shader_value_matrix(material.shader, material.shader.locs[RF_LOC_MATRIX_VIEW],
                                       rf_ctx.modelview);
    if (material.shader.locs[RF_LOC_MATRIX_PROJECTION] != -1)
        rf_gfx_set_shader_value_matrix(material.shader, material.shader.locs[RF_LOC_MATRIX_PROJECTION],
                                       rf_ctx.projection);

    // At this point the rf_ctx->gl_ctx.modelview matrix just contains the view matrix (camera)
    // That's because rf_begin_mode3d() sets it an no model-drawing function modifies it, all use rf_gfx_push_matrix() and rf_gfx_pop_matrix()
    rf_mat mat_view = rf_ctx.modelview;         // View matrix (camera)
    rf_mat mat_projection = rf_ctx.projection;  // Projection matrix (perspective)

    // TODO: Consider possible transform matrices in the rf_ctx->gl_ctx.stack
    // Is this the right order? or should we start with the first stored matrix instead of the last one?
    //rf_mat matStackTransform = rf_mat_identity();
    //for (rf_int i = rf_ctx->gl_ctx.stack_counter; i > 0; i--) matStackTransform = rf_mat_mul(rf_ctx->gl_ctx.stack[i], matStackTransform);

    // rf_transform to camera-space coordinates
    rf_mat mat_model_view = rf_mat_mul(transform, rf_mat_mul(rf_ctx.transform, mat_view));
    //-----------------------------------------------------

    // Bind active texture maps (if available)
    for (rf_int i = 0; i < RF_MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id > 0)
        {
            rf_gl.ActiveTexture(GL_TEXTURE0 + i);
            if ((i == RF_MAP_IRRADIANCE) || (i == RF_MAP_PREFILTER) || (i == RF_MAP_CUBEMAP)) rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, material.maps[i].texture.id);
            else rf_gl.BindTexture(GL_TEXTURE_2D, material.maps[i].texture.id);

            rf_gl.Uniform1i(material.shader.locs[RF_LOC_MAP_DIFFUSE + i], i);
        }
    }

    // Bind vertex array objects (or VBOs)
    rf_gl.BindVertexArray(mesh.vao_id);

    rf_ctx.modelview = mat_model_view;

    // Calculate model-view-rf_ctx->gl_ctx.projection matrix (MVP)
    rf_mat mat_mvp = rf_mat_mul(rf_ctx.modelview, rf_ctx.projection); // rf_transform to screen-space coordinates

    // Send combined model-view-rf_ctx->gl_ctx.projection matrix to shader
    rf_gl.UniformMatrix4fv(material.shader.locs[RF_LOC_MATRIX_MVP], 1, 0, rf_mat_to_float16(mat_mvp).v);

    // Draw call!
    if (mesh.indices != NULL) rf_gl.DrawElements(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT, 0); // Indexed vertices draw
    else rf_gl.DrawArrays(GL_TRIANGLES, 0, mesh.vertex_count);

    // Unbind all binded texture maps
    for (rf_int i = 0; i < RF_MAX_MATERIAL_MAPS; i++)
    {
        rf_gl.ActiveTexture(GL_TEXTURE0 + i);       // Set shader active texture
        if ((i == RF_MAP_IRRADIANCE) || (i == RF_MAP_PREFILTER) || (i == RF_MAP_CUBEMAP)) rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
        else rf_gl.BindTexture(GL_TEXTURE_2D, 0);   // Unbind current active texture
    }

    // Unind vertex array objects (or VBOs)
    rf_gl.BindVertexArray(0);

    // Unbind shader program
    rf_gl.UseProgram(0);

    // Restore rf_ctx->gl_ctx.projection/rf_ctx->gl_ctx.modelview matrices
    // NOTE: In stereo rendering matrices are being modified to fit every eye
    rf_ctx.projection = mat_projection;
    rf_ctx.modelview = mat_view;
}

// Unload mesh data from the GPU
rf_extern void rf_gfx_unload_mesh(rf_mesh mesh)
{
    rf_gfx_delete_buffers(mesh.vbo_id[0]);   // vertex
    rf_gfx_delete_buffers(mesh.vbo_id[1]);   // texcoords
    rf_gfx_delete_buffers(mesh.vbo_id[2]);   // normals
    rf_gfx_delete_buffers(mesh.vbo_id[3]);   // colors
    rf_gfx_delete_buffers(mesh.vbo_id[4]);   // tangents
    rf_gfx_delete_buffers(mesh.vbo_id[5]);   // texcoords2
    rf_gfx_delete_buffers(mesh.vbo_id[6]);   // indices

    rf_gfx_delete_vertex_arrays(mesh.vao_id);
}

#pragma endregion

#pragma region gen textures
// Generate cubemap texture from HDR texture
rf_extern rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, rf_int size)
{
    rf_texture2d cubemap = { 0 };
    // NOTE: rf_set_shader_default_locations() already setups locations for rf_ctx->gl_ctx.projection and view rf_mat in shader
    // Other locations should be setup externally in shader before calling the function

    // Set up depth face culling and cubemap seamless
    rf_gl.Disable(GL_CULL_FACE);
    rf_gl.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);     // Flag not supported on OpenGL ES 2.0

    // Setup framebuffer
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Set up cubemap to render and attach to framebuffer
    // NOTE: Faces are stored as 32 bit floating point values
    rf_gl.GenTextures(1, &cubemap.id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    for (unsigned int i = 0; i < 6; i++)
    {
        rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    }

    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create rf_ctx->gl_ctx.projection and different views for each face
    rf_mat fbo_projection = rf_mat_perspective(90.0 * rf_deg2rad, 1.0, 0.01, 1000.0);
    rf_mat fbo_views[6] = {
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {-1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f})
    };

    // Convert HDR equirectangular environment map to cubemap equivalent
    rf_gl.UseProgram(shader.id);
    rf_gl.ActiveTexture(GL_TEXTURE0);
    rf_gl.BindTexture(GL_TEXTURE_2D, sky_hdr.id);
    rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_PROJECTION], fbo_projection);

    // Note: don't forget to configure the viewport to the capture dimensions
    rf_gl.Viewport(0, 0, size, size);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (rf_int i = 0; i < 6; i++)
    {
        rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_VIEW], fbo_views[i]);
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap.id, 0);
        rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rf_gen_draw_cube();
    }

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);
    //glEnable(GL_CULL_FACE);

    // NOTE: rf_texture2d is a GL_TEXTURE_CUBE_MAP, not a GL_TEXTURE_2D!
    cubemap.width = size;
    cubemap.height = size;
    cubemap.mipmaps = 1;
    cubemap.format = rf_pixel_format_r32g32b32;

    return cubemap;
}

// Generate irradiance texture using cubemap data
rf_extern rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, rf_int size)
{
    rf_texture2d irradiance = { 0 };

    // NOTE: rf_set_shader_default_locations() already setups locations for rf_ctx->gl_ctx.projection and view rf_mat in shader
    // Other locations should be setup externally in shader before calling the function

    // Setup framebuffer
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Create an irradiance cubemap, and re-scale capture FBO to irradiance scale
    rf_gl.GenTextures(1, &irradiance.id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, irradiance.id);
    for (unsigned int i = 0; i < 6; i++)
    {
        rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    }

    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create rf_ctx->gl_ctx.projection (transposed) and different views for each face
    rf_mat fbo_projection = rf_mat_perspective(90.0 * rf_deg2rad, 1.0, 0.01, 1000.0);
    rf_mat fbo_views[6] = {
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {-1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f})
    };

    // Solve diffuse integral by convolution to create an irradiance cubemap
    rf_gl.UseProgram(shader.id);
    rf_gl.ActiveTexture(GL_TEXTURE0);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_PROJECTION], fbo_projection);

    // Note: don't forget to configure the viewport to the capture dimensions
    rf_gl.Viewport(0, 0, size, size);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (rf_int i = 0; i < 6; i++)
    {
        rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_VIEW], fbo_views[i]);
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance.id, 0);
        rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rf_gen_draw_cube();
    }

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);

    irradiance.width = size;
    irradiance.height = size;
    irradiance.mipmaps = 1;
    //irradiance.format = UNCOMPRESSED_R16G16B16;

    return irradiance;
}

// Generate prefilter texture using cubemap data
rf_extern rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, rf_int size)
{
    rf_texture2d prefilter = { 0 };

    // NOTE: rf_set_shader_default_locations() already setups locations for projection and view rf_mat in shader
    // Other locations should be setup externally in shader before calling the function
    // TODO: Locations should be taken out of this function... too shader dependant...
    int roughness_loc = rf_gfx_get_shader_location(shader, "roughness");

    // Setup framebuffer
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Create a prefiltered HDR environment map
    rf_gl.GenTextures(1, &prefilter.id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, prefilter.id);
    for (unsigned int i = 0; i < 6; i++)
    {
        rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    }

    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps for the prefiltered HDR texture
    rf_gl.GenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Create rf_ctx->gl_ctx.projection (transposed) and different views for each face
    rf_mat fbo_projection = rf_mat_perspective(90.0 * rf_deg2rad, 1.0, 0.01, 1000.0);
    rf_mat fbo_views[6] = {
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {-1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f})
    };

    // Prefilter HDR and store data into mipmap levels
    rf_gl.UseProgram(shader.id);
    rf_gl.ActiveTexture(GL_TEXTURE0);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_PROJECTION], fbo_projection);

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);

#define MAX_MIPMAP_LEVELS   5   // Max number of prefilter texture mipmaps

    for (rf_int mip = 0; mip < MAX_MIPMAP_LEVELS; mip++)
    {
        // Resize framebuffer according to mip-level size.
        unsigned int mip_width  = size*(int)powf(0.5f, (float)mip);
        unsigned int mip_height = size*(int)powf(0.5f, (float)mip);

        rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
        rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
        rf_gl.Viewport(0, 0, mip_width, mip_height);

        float roughness = (float)mip/(float)(MAX_MIPMAP_LEVELS - 1);
        rf_gl.Uniform1f(roughness_loc, roughness);

        for (rf_int i = 0; i < 6; i++)
        {
            rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_VIEW], fbo_views[i]);
            rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter.id, mip);
            rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rf_gen_draw_cube();
        }
    }

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);

    prefilter.width = size;
    prefilter.height = size;
    //prefilter.mipmaps = 1 + (int)floor(log(size)/log(2));
    //prefilter.format = UNCOMPRESSED_R16G16B16;

    return prefilter;
}

// Generate BRDF texture using cubemap data. Todo: Review implementation: https://github.com/HectorMF/BRDFGenerator
rf_extern rf_texture2d rf_gen_texture_brdf(rf_shader shader, rf_int size)
{
    rf_texture2d brdf = { 0 };
    // Generate BRDF convolution texture
    rf_gl.GenTextures(1, &brdf.id);
    rf_gl.BindTexture(GL_TEXTURE_2D, brdf.id);
    rf_gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, NULL);

    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Render BRDF LUT into a quad using FBO
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf.id, 0);

    rf_gl.Viewport(0, 0, size, size);
    rf_gl.UseProgram(shader.id);
    rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rf_gen_draw_quad();

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Unload framebuffer but keep color texture
    rf_gl.DeleteRenderbuffers(1, &rbo);
    rf_gl.DeleteFramebuffers(1, &fbo);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);

    brdf.width = size;
    brdf.height = size;
    brdf.mipmaps = 1;
    brdf.format = rf_pixel_format_r32g32b32;

    return brdf;
}

#pragma endregion

#endif