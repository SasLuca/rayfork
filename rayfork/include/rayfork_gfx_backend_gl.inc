// Such files are used to declare the data structures for a specific backend.
// They are included in rayfork.h based on the graphics backend that was chosen at compile time.
// This file declares only structs and must define rf_renderer_memory_buffers and rf_gfx_context and the rf_init function

#ifndef RAYFORK_GFX_BACKEND_GL_H
#define RAYFORK_GFX_BACKEND_GL_H

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rf_dynamic_buffer rf_dynamic_buffer;
struct rf_dynamic_buffer
{
    int v_counter;  // vertex position counter to process (and draw) from full buffer
    int tc_counter; // vertex texcoord counter to process (and draw) from full buffer
    int c_counter;  // vertex color counter to process (and draw) from full buffer

    float vertices[3 * 4 * RF_MAX_BATCH_ELEMENTS];
    float texcoords[2 * 4 * RF_MAX_BATCH_ELEMENTS];
    unsigned char colors[4 * 4 * RF_MAX_BATCH_ELEMENTS];

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    unsigned int indices[6 * RF_MAX_BATCH_ELEMENTS]; // 6 int by quad (indices)
#elif defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    unsigned short indices[6 * RF_MAX_BATCH_ELEMENTS];  // 6 int by quad (indices)
#endif

    unsigned int vao_id;         // OpenGL Vertex Array Object id
    unsigned int vbo_id[4];      // OpenGL Vertex Buffer Objects id (4 types of vertex data)
};

// Draw call type
typedef struct rf_draw_call rf_draw_call;
struct rf_draw_call
{
    rf_drawing_mode mode;    // Drawing mode: RF_LINES, RF_TRIANGLES, RF_QUADS
    int vertex_count;        // Number of vertex of the draw
    int vertex_alignment;    // Number of vertex required for index alignment (LINES, TRIANGLES)
    //unsigned int vao_id;   // Vertex array id to be used on the draw
    //unsigned int shaderId; // rf_shader id to be used on the draw
    unsigned int texture_id; // rf_texture id to be used on the draw
    // TODO: Support additional texture units?

    //rf_mat projection;     // Projection matrix for this draw
    //rf_mat modelview;      // Modelview matrix for this draw
};

typedef struct rf_renderer_memory_buffers rf_renderer_memory_buffers;
struct rf_renderer_memory_buffers
{
    rf_dynamic_buffer vertex_data[RF_MAX_BATCH_BUFFERING];
    rf_draw_call      draw_calls[RF_MAX_DRAWCALL_REGISTERED];

    rf_char_info      default_font_chars[RF_DEFAULT_FONT_CHARS_COUNT];
    rf_rec            default_font_recs[RF_DEFAULT_FONT_CHARS_COUNT];
};

typedef struct rf_opengl_procs rf_opengl_procs;
struct rf_opengl_procs
{
    void (*glViewport)(int x, int y, int width, int height);
    void (*glBindTexture)(unsigned int target, unsigned int texture);
    void (*glTexParameteri)(unsigned int target, unsigned int pname, int param);
    void (*glTexParameterf)(unsigned int target, unsigned int pname, float param);
    void (*glTexParameteriv)(unsigned int target, unsigned int pname, const int* params);
    void (*glBindFramebuffer)(unsigned int target, unsigned int framebuffer);
    void (*glEnable)(unsigned int cap);
    void (*glDisable)(unsigned int cap);
    void (*glScissor)(int x, int y, int width, int height);
    void (*glDeleteTextures)(int n, const unsigned int* textures);
    void (*glDeleteRenderbuffers)(int n, const unsigned int* renderbuffers);
    void (*glDeleteFramebuffers)(int n, const unsigned int* framebuffers);
    void (*glDeleteVertexArrays)(int n, const unsigned int* arrays);
    void (*glDeleteBuffers)(int n, const unsigned int* buffers);
    void (*glClearColor)(float red, float green, float blue, float alpha);
    void (*glClear)(unsigned int mask);
    void (*glBindBuffer)(unsigned int target, unsigned int buffer);
    void (*glBufferSubData)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data);
    void (*glBindVertexArray)(unsigned int array);
    void (*glGenBuffers)(int n, unsigned int* buffers);
    void (*glBufferData)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
    void (*glVertexAttribPointer)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
    void (*glEnableVertexAttribArray)(unsigned int index);
    void (*glGenVertexArrays)(int n, unsigned int* arrays);
    void (*glVertexAttrib3f)(unsigned int index, float x, float y, float z);
    void (*glDisableVertexAttribArray)(unsigned int index);
    void (*glVertexAttrib4f)(unsigned int index, float x, float y, float z, float w);
    void (*glVertexAttrib2f)(unsigned int index, float x, float y);
    void (*glUseProgram)(unsigned int program);
    void (*glUniform4f)(int location, float v0, float v1, float v2, float v3);
    void (*glActiveTexture)(unsigned int texture);
    void (*glUniform1i)(int location, int v0);
    void (*glUniformMatrix4fv)(int location, int count, unsigned char transpose, const float* value);
    void (*glDrawElements)(unsigned int mode, int count, unsigned int type, const void* indices);
    void (*glDrawArrays)(unsigned int mode, int first, int count);
    void (*glPixelStorei)(unsigned int pname, int param);
    void (*glGenTextures)(int n, unsigned int* textures);
    void (*glTexImage2D)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
    void (*glGenRenderbuffers)(int n, unsigned int* renderbuffers);
    void (*glBindRenderbuffer)(unsigned int target, unsigned int renderbuffer);
    void (*glRenderbufferStorage)(unsigned int target, unsigned int internalformat, int width, int height);
    void (*glCompressedTexImage2D)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data);
    void (*glTexSubImage2D)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels);
    void (*glGenerateMipmap)(unsigned int target);
    void (*glReadPixels)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
    void (*glGenFramebuffers)(int n, unsigned int* framebuffers);
    void (*glFramebufferTexture2D)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
    void (*glFramebufferRenderbuffer)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
    unsigned int (*glCheckFramebufferStatus)(unsigned int target);
    unsigned int (*glCreateShader)(unsigned int type);
    void (*glShaderSource)(unsigned int shader, int count, const char** string, const int* length);
    void (*glCompileShader)(unsigned int shader);
    void (*glGetShaderiv)(unsigned int shader, unsigned int pname, int* params);
    void (*glGetShaderInfoLog)(unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int (*glCreateProgram)();
    void (*glAttachShader)(unsigned int program, unsigned int shader);
    void (*glBindAttribLocation)(unsigned int program, unsigned int index, const char* name);
    void (*glLinkProgram)(unsigned int program);
    void (*glGetProgramiv)(unsigned int program, unsigned int pname, int* params);
    void (*glGetProgramInfoLog)(unsigned int program, int bufSize, int* length, char* infoLog);
    void (*glDeleteProgram)(unsigned int program);
    int (*glGetAttribLocation)(unsigned int program, const char* name);
    int (*glGetUniformLocation)(unsigned int program, const char* name);
    void (*glDetachShader)(unsigned int program, unsigned int shader);
    void (*glDeleteShader)(unsigned int shader);

    void (*glGetTexImage)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels); // NULL for OpenGL ES3

    void (*glGetActiveUniform)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name);
    void (*glUniform1f)(int location, float v0);
    void (*glUniform1fv)(int location, int count, const float* value);
    void (*glUniform2fv)(int location, int count, const float* value);
    void (*glUniform3fv)(int location, int count, const float* value);
    void (*glUniform4fv)(int location, int count, const float* value);
    void (*glUniform1iv)(int location, int count, const int* value);
    void (*glUniform2iv)(int location, int count, const int* value);
    void (*glUniform3iv)(int location, int count, const int* value);
    void (*glUniform4iv)(int location, int count, const int* value);
    const unsigned char* (*glGetString)(unsigned int name);
    void (*glGetFloatv)(unsigned int pname, float* data);
    void (*glDepthFunc)(unsigned int func);
    void (*glBlendFunc)(unsigned int sfactor, unsigned int dfactor);
    void (*glCullFace)(unsigned int mode);
    void (*glFrontFace)(unsigned int mode);

    //On OpenGL33 we only set glClearDepth but on OpenGL ES3 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version
    void (*glClearDepth)(double depth);
    void (*glClearDepthf)(float depth);

    void (*glGetIntegerv)(unsigned int pname, int* data); //OpenGL 33 only, can be NULL in OpenGL ES3
    const unsigned char* (*glGetStringi)(unsigned int name, unsigned int index);

    void (*glPolygonMode)(unsigned int face, unsigned int mode); //OpenGL 33 only
};

typedef struct rf_gfx_context rf_gfx_context;
struct rf_gfx_context
{
    rf_opengl_procs gl;

    rf_mat stack[RF_MAX_MATRIX_STACK_SIZE];
    int stack_counter;

    rf_mat modelview;
    rf_mat projection;
    rf_mat* current_matrix;
    int current_matrix_mode;
    float current_depth;
    int current_buffer;

    rf_mat transform_matrix;
    bool use_transform_matrix;

    // Default buffers draw calls
    rf_draw_call* draws;
    int draws_counter;

    // Default texture (1px white) useful for plain color polys (required by shader)
    unsigned int default_texture_id;

    // Default shaders
    unsigned int default_vertex_shader_id;   // Default vertex shader id (used by default shader program)
    unsigned int default_frag_shader_id;     // Default fragment shader Id (used by default shader program)

    rf_shader default_shader;                // Basic shader, support vertex color and diffuse texture
    rf_shader current_shader;                // rf_shader to be used on rendering (by default, default_shader)

    // Extension supported flag: Compressed textures
    bool tex_comp_dxt_supported;             // DDS texture compression support
    bool tex_comp_etc1_supported;            // ETC1 texture compression support
    bool tex_comp_etc2_supported;            // ETC2/EAC texture compression support
    bool tex_comp_pvrt_supported;            // PVR texture compression support
    bool tex_comp_astc_supported;            // ASTC texture compression support

    // Extension supported flag: Textures format
    bool tex_npot_supported;                 // NPOT textures full support
    bool tex_float_supported;                // float textures support (32 bit per channel)
    bool tex_depth_supported;                // Depth textures supported
    int max_depth_bits;                      // Maximum bits for depth component

    // Extension supported flag: Clamp mirror wrap mode
    bool tex_mirror_clamp_supported;         // Clamp mirror wrap mode supported

    // Extension supported flag: Anisotropic filtering
    bool tex_anisotropic_filter_supported;   // Anisotropic texture filtering support
    float max_anisotropic_level;             // Maximum anisotropy level supported (minimum is 2.0f)

    bool debug_marker_supported;             // Debug marker support

    rf_blend_mode blend_mode;                          // Track current blending mode

    // Default framebuffer size
    int framebuffer_width;                   // Default framebuffer width
    int framebuffer_height;                  // Default framebuffer height

    // Default dynamic buffer for elements data. Note: A multi-buffering system is supported
    rf_renderer_memory_buffers* memory;
};

struct rf_context;
RF_API void rf_init(struct rf_context* rf_ctx, rf_renderer_memory_buffers* memory, int width, int height, rf_opengl_procs gl);

//region opengl procs macro
#define RF_CONCAT2(a, b) a##b
#define RF_CONCAT(a, b) RF_CONCAT2(a, b)
#define RF_OPENGL_PROC_EXT(ext, gl_proc_name) RF_CONCAT(ext, gl_proc_name)

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
#define RF_DEFAULT_OPENGL_PROCS_EXT(ext) (rf_opengl_procs) { \
    ((void (*)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Viewport)),\
    ((void (*)(unsigned int target, unsigned int texture))RF_OPENGL_PROC_EXT(ext, BindTexture)),\
    ((void (*)(unsigned int target, unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, TexParameteri)),\
    ((void (*)(unsigned int target, unsigned int pname, float param))RF_OPENGL_PROC_EXT(ext, TexParameterf)),\
    ((void (*)(unsigned int target, unsigned int pname, const int* params))RF_OPENGL_PROC_EXT(ext, TexParameteriv)),\
    ((void (*)(unsigned int target, unsigned int framebuffer))RF_OPENGL_PROC_EXT(ext, BindFramebuffer)),\
    ((void (*)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Enable)),\
    ((void (*)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Disable)),\
    ((void (*)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Scissor)),\
    ((void (*)(int n, const unsigned int* textures))RF_OPENGL_PROC_EXT(ext, DeleteTextures)),\
    ((void (*)(int n, const unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, DeleteRenderbuffers)),\
    ((void (*)(int n, const unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, DeleteFramebuffers)),\
    ((void (*)(int n, const unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, DeleteVertexArrays)),\
    ((void (*)(int n, const unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, DeleteBuffers)),\
    ((void (*)(float red, float green, float blue, float alpha))RF_OPENGL_PROC_EXT(ext, ClearColor)),\
    ((void (*)(unsigned int mask))RF_OPENGL_PROC_EXT(ext, Clear)),\
    ((void (*)(unsigned int target, unsigned int buffer))RF_OPENGL_PROC_EXT(ext, BindBuffer)),\
    ((void (*)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data))RF_OPENGL_PROC_EXT(ext, BufferSubData)),\
    ((void (*)(unsigned int array))RF_OPENGL_PROC_EXT(ext, BindVertexArray)),\
    ((void (*)(int n, unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, GenBuffers)),\
    ((void (*)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage))RF_OPENGL_PROC_EXT(ext, BufferData)),\
    ((void (*)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer))RF_OPENGL_PROC_EXT(ext, VertexAttribPointer)),\
    ((void (*)(unsigned int index))RF_OPENGL_PROC_EXT(ext, EnableVertexAttribArray)),\
    ((void (*)(int n, unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, GenVertexArrays)),\
    ((void (*)(unsigned int index, float x, float y, float z))RF_OPENGL_PROC_EXT(ext, VertexAttrib3f)),\
    ((void (*)(unsigned int index))RF_OPENGL_PROC_EXT(ext, DisableVertexAttribArray)),\
    ((void (*)(unsigned int index, float x, float y, float z, float w))RF_OPENGL_PROC_EXT(ext, VertexAttrib4f)),\
    ((void (*)(unsigned int index, float x, float y))RF_OPENGL_PROC_EXT(ext, VertexAttrib2f)),\
    ((void (*)(unsigned int program))RF_OPENGL_PROC_EXT(ext, UseProgram)),\
    ((void (*)(int location, float v0, float v1, float v2, float v3))RF_OPENGL_PROC_EXT(ext, Uniform4f)),\
    ((void (*)(unsigned int texture))RF_OPENGL_PROC_EXT(ext, ActiveTexture)),\
    ((void (*)(int location, int v0))RF_OPENGL_PROC_EXT(ext, Uniform1i)),\
    ((void (*)(int location, int count, unsigned char transpose, const float* value))RF_OPENGL_PROC_EXT(ext, UniformMatrix4fv)),\
    ((void (*)(unsigned int mode, int count, unsigned int type, const void* indices))RF_OPENGL_PROC_EXT(ext, DrawElements)),\
    ((void (*)(unsigned int mode, int first, int count))RF_OPENGL_PROC_EXT(ext, DrawArrays)),\
    ((void (*)(unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, PixelStorei)),\
    ((void (*)(int n, unsigned int* textures))RF_OPENGL_PROC_EXT(ext, GenTextures)),\
    ((void (*)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexImage2D)),\
    ((void (*)(int n, unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, GenRenderbuffers)),\
    ((void (*)(unsigned int target, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, BindRenderbuffer)),\
    ((void (*)(unsigned int target, unsigned int internalformat, int width, int height))RF_OPENGL_PROC_EXT(ext, RenderbufferStorage)),\
    ((void (*)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data))RF_OPENGL_PROC_EXT(ext, CompressedTexImage2D)),\
    ((void (*)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexSubImage2D)),\
    ((void (*)(unsigned int target))RF_OPENGL_PROC_EXT(ext, GenerateMipmap)),\
    ((void (*)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, ReadPixels)),\
    ((void (*)(int n, unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, GenFramebuffers)),\
    ((void (*)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level))RF_OPENGL_PROC_EXT(ext, FramebufferTexture2D)),\
    ((void (*)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, FramebufferRenderbuffer)),\
    ((unsigned int (*)(unsigned int target))RF_OPENGL_PROC_EXT(ext, CheckFramebufferStatus)),\
    ((unsigned int (*)(unsigned int type))RF_OPENGL_PROC_EXT(ext, CreateShader)),\
    ((void (*)(unsigned int shader, int count, const char** string, const int* length))RF_OPENGL_PROC_EXT(ext, ShaderSource)),\
    ((void (*)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, CompileShader)),\
    ((void (*)(unsigned int shader, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetShaderiv)),\
    ((void (*)(unsigned int shader, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetShaderInfoLog)),\
    ((unsigned int (*)())RF_OPENGL_PROC_EXT(ext, CreateProgram)),\
    ((void (*)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, AttachShader)),\
    ((void (*)(unsigned int program, unsigned int index, const char* name))RF_OPENGL_PROC_EXT(ext, BindAttribLocation)),\
    ((void (*)(unsigned int program))RF_OPENGL_PROC_EXT(ext, LinkProgram)),\
    ((void (*)(unsigned int program, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetProgramiv)),\
    ((void (*)(unsigned int program, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetProgramInfoLog)),\
    ((void (*)(unsigned int program))RF_OPENGL_PROC_EXT(ext, DeleteProgram)),\
    ((int (*)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetAttribLocation)),\
    ((int (*)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetUniformLocation)),\
    ((void (*)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, DetachShader)),\
    ((void (*)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, DeleteShader)),\
    ((void (*)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, GetTexImage)),\
    ((void (*)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name))RF_OPENGL_PROC_EXT(ext, GetActiveUniform)),\
    ((void (*)(int location, float v0))RF_OPENGL_PROC_EXT(ext, Uniform1f)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform1fv)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform2fv)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform3fv)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform4fv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform1iv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform2iv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform3iv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform4iv)),\
    ((const unsigned char* (*)(unsigned int name))RF_OPENGL_PROC_EXT(ext, GetString)),\
    ((void (*)(unsigned int pname, float* data))RF_OPENGL_PROC_EXT(ext, GetFloatv)),\
    ((void (*)(unsigned int func))RF_OPENGL_PROC_EXT(ext, DepthFunc)),\
    ((void (*)(unsigned int sfactor, unsigned int dfactor))RF_OPENGL_PROC_EXT(ext, BlendFunc)),\
    ((void (*)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, CullFace)),\
    ((void (*)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, FrontFace)),\
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/\
    ((void (*)(double depth))RF_OPENGL_PROC_EXT(ext, ClearDepth)),\
    NULL /*glClearDepthf*/,\
    ((void (*)(unsigned int pname, int* data))RF_OPENGL_PROC_EXT(ext, GetIntegerv)),\
    ((const unsigned char* (*)(unsigned int name, unsigned int index))RF_OPENGL_PROC_EXT(ext, GetStringi)),\
    ((void (*)(unsigned int face, unsigned int mode))RF_OPENGL_PROC_EXT(ext, PolygonMode)), /*OpenGL 33 only*/ \
}

#else
#define RF_DEFAULT_OPENGL_PROCS_EXT(ext) (rf_opengl_procs) { \
    ((void (*)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Viewport)),\
    ((void (*)(unsigned int target, unsigned int texture))RF_OPENGL_PROC_EXT(ext, BindTexture)),\
    ((void (*)(unsigned int target, unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, TexParameteri)),\
    ((void (*)(unsigned int target, unsigned int pname, float param))RF_OPENGL_PROC_EXT(ext, TexParameterf)),\
    NULL, /*glTexParameteri not needed for OpenGL ES3*/ \
    ((void (*)(unsigned int target, unsigned int framebuffer))RF_OPENGL_PROC_EXT(ext, BindFramebuffer)),\
    ((void (*)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Enable)),\
    ((void (*)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Disable)),\
    ((void (*)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Scissor)),\
    ((void (*)(int n, const unsigned int* textures))RF_OPENGL_PROC_EXT(ext, DeleteTextures)),\
    ((void (*)(int n, const unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, DeleteRenderbuffers)),\
    ((void (*)(int n, const unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, DeleteFramebuffers)),\
    ((void (*)(int n, const unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, DeleteVertexArrays)),\
    ((void (*)(int n, const unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, DeleteBuffers)),\
    ((void (*)(float red, float green, float blue, float alpha))RF_OPENGL_PROC_EXT(ext, ClearColor)),\
    ((void (*)(unsigned int mask))RF_OPENGL_PROC_EXT(ext, Clear)),\
    ((void (*)(unsigned int target, unsigned int buffer))RF_OPENGL_PROC_EXT(ext, BindBuffer)),\
    ((void (*)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data))RF_OPENGL_PROC_EXT(ext, BufferSubData)),\
    ((void (*)(unsigned int array))RF_OPENGL_PROC_EXT(ext, BindVertexArray)),\
    ((void (*)(int n, unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, GenBuffers)),\
    ((void (*)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage))RF_OPENGL_PROC_EXT(ext, BufferData)),\
    ((void (*)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer))RF_OPENGL_PROC_EXT(ext, VertexAttribPointer)),\
    ((void (*)(unsigned int index))RF_OPENGL_PROC_EXT(ext, EnableVertexAttribArray)),\
    ((void (*)(int n, unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, GenVertexArrays)),\
    ((void (*)(unsigned int index, float x, float y, float z))RF_OPENGL_PROC_EXT(ext, VertexAttrib3f)),\
    ((void (*)(unsigned int index))RF_OPENGL_PROC_EXT(ext, DisableVertexAttribArray)),\
    ((void (*)(unsigned int index, float x, float y, float z, float w))RF_OPENGL_PROC_EXT(ext, VertexAttrib4f)),\
    ((void (*)(unsigned int index, float x, float y))RF_OPENGL_PROC_EXT(ext, VertexAttrib2f)),\
    ((void (*)(unsigned int program))RF_OPENGL_PROC_EXT(ext, UseProgram)),\
    ((void (*)(int location, float v0, float v1, float v2, float v3))RF_OPENGL_PROC_EXT(ext, Uniform4f)),\
    ((void (*)(unsigned int texture))RF_OPENGL_PROC_EXT(ext, ActiveTexture)),\
    ((void (*)(int location, int v0))RF_OPENGL_PROC_EXT(ext, Uniform1i)),\
    ((void (*)(int location, int count, unsigned char transpose, const float* value))RF_OPENGL_PROC_EXT(ext, UniformMatrix4fv)),\
    ((void (*)(unsigned int mode, int count, unsigned int type, const void* indices))RF_OPENGL_PROC_EXT(ext, DrawElements)),\
    ((void (*)(unsigned int mode, int first, int count))RF_OPENGL_PROC_EXT(ext, DrawArrays)),\
    ((void (*)(unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, PixelStorei)),\
    ((void (*)(int n, unsigned int* textures))RF_OPENGL_PROC_EXT(ext, GenTextures)),\
    ((void (*)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexImage2D)),\
    ((void (*)(int n, unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, GenRenderbuffers)),\
    ((void (*)(unsigned int target, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, BindRenderbuffer)),\
    ((void (*)(unsigned int target, unsigned int internalformat, int width, int height))RF_OPENGL_PROC_EXT(ext, RenderbufferStorage)),\
    ((void (*)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data))RF_OPENGL_PROC_EXT(ext, CompressedTexImage2D)),\
    ((void (*)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexSubImage2D)),\
    ((void (*)(unsigned int target))RF_OPENGL_PROC_EXT(ext, GenerateMipmap)),\
    ((void (*)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, ReadPixels)),\
    ((void (*)(int n, unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, GenFramebuffers)),\
    ((void (*)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level))RF_OPENGL_PROC_EXT(ext, FramebufferTexture2D)),\
    ((void (*)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, FramebufferRenderbuffer)),\
    ((unsigned int (*)(unsigned int target))RF_OPENGL_PROC_EXT(ext, CheckFramebufferStatus)),\
    ((unsigned int (*)(unsigned int type))RF_OPENGL_PROC_EXT(ext, CreateShader)),\
    ((void (*)(unsigned int shader, int count, const char** string, const int* length))RF_OPENGL_PROC_EXT(ext, ShaderSource)),\
    ((void (*)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, CompileShader)),\
    ((void (*)(unsigned int shader, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetShaderiv)),\
    ((void (*)(unsigned int shader, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetShaderInfoLog)),\
    ((unsigned int (*)())RF_OPENGL_PROC_EXT(ext, CreateProgram)),\
    ((void (*)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, AttachShader)),\
    ((void (*)(unsigned int program, unsigned int index, const char* name))RF_OPENGL_PROC_EXT(ext, BindAttribLocation)),\
    ((void (*)(unsigned int program))RF_OPENGL_PROC_EXT(ext, LinkProgram)),\
    ((void (*)(unsigned int program, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetProgramiv)),\
    ((void (*)(unsigned int program, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetProgramInfoLog)),\
    ((void (*)(unsigned int program))RF_OPENGL_PROC_EXT(ext, DeleteProgram)),\
    ((int (*)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetAttribLocation)),\
    ((int (*)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetUniformLocation)),\
    ((void (*)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, DetachShader)),\
    ((void (*)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, DeleteShader)),\
    NULL, /*glGetTexImage is NULL for opengl es3*/ \
    ((void (*)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name))RF_OPENGL_PROC_EXT(ext, GetActiveUniform)),\
    ((void (*)(int location, float v0))RF_OPENGL_PROC_EXT(ext, Uniform1f)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform1fv)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform2fv)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform3fv)),\
    ((void (*)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform4fv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform1iv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform2iv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform3iv)),\
    ((void (*)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform4iv)),\
    ((const unsigned char* (*)(unsigned int name))RF_OPENGL_PROC_EXT(ext, GetString)),\
    ((void (*)(unsigned int pname, float* data))RF_OPENGL_PROC_EXT(ext, GetFloatv)),\
    ((void (*)(unsigned int func))RF_OPENGL_PROC_EXT(ext, DepthFunc)),\
    ((void (*)(unsigned int sfactor, unsigned int dfactor))RF_OPENGL_PROC_EXT(ext, BlendFunc)),\
    ((void (*)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, CullFace)),\
    ((void (*)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, FrontFace)),\
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/\
    NULL /*glClearDepth*/,\
    ((void (*)(float depth))RF_OPENGL_PROC_EXT(ext, ClearDepthf)),\
    NULL /*OpenGL 33 only, should be NULL in OpenGL ES3*/,\
    ((const unsigned char* (*)(unsigned int name, unsigned int index))RF_OPENGL_PROC_EXT(ext, GetStringi)),\
    NULL /*glPolygonMode is OpenGL 33 only*/ \
}
#endif

#define RF_DEFAULT_OPENGL_PROCS RF_DEFAULT_OPENGL_PROCS_EXT(gl)

//endregion

#endif