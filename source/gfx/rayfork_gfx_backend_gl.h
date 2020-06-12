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
    unsigned int indices[6 * RF_MAX_BATCH_ELEMENTS];   // 6 int by quad (indices)
#elif defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    unsigned short indices[6 * RF_MAX_BATCH_ELEMENTS]; // 6 int by quad (indices)
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
};

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
    #define APIENTRY __stdcall
#endif

#ifndef APIENTRY
    #define APIENTRY
#endif

#ifndef RF_GL_CALLING_CONVENTION
    #define RF_GL_CALLING_CONVENTION APIENTRY *
#endif

typedef struct rf_opengl_procs rf_opengl_procs;
struct rf_opengl_procs
{
    void (RF_GL_CALLING_CONVENTION glViewport)(int x, int y, int width, int height);
    void (RF_GL_CALLING_CONVENTION glBindTexture)(unsigned int target, unsigned int texture);
    void (RF_GL_CALLING_CONVENTION glTexParameteri)(unsigned int target, unsigned int pname, int param);
    void (RF_GL_CALLING_CONVENTION glTexParameterf)(unsigned int target, unsigned int pname, float param);
    void (RF_GL_CALLING_CONVENTION glTexParameteriv)(unsigned int target, unsigned int pname, const int* params);
    void (RF_GL_CALLING_CONVENTION glBindFramebuffer)(unsigned int target, unsigned int framebuffer);
    void (RF_GL_CALLING_CONVENTION glEnable)(unsigned int cap);
    void (RF_GL_CALLING_CONVENTION glDisable)(unsigned int cap);
    void (RF_GL_CALLING_CONVENTION glScissor)(int x, int y, int width, int height);
    void (RF_GL_CALLING_CONVENTION glDeleteTextures)(int n, const unsigned int* textures);
    void (RF_GL_CALLING_CONVENTION glDeleteRenderbuffers)(int n, const unsigned int* renderbuffers);
    void (RF_GL_CALLING_CONVENTION glDeleteFramebuffers)(int n, const unsigned int* framebuffers);
    void (RF_GL_CALLING_CONVENTION glDeleteVertexArrays)(int n, const unsigned int* arrays);
    void (RF_GL_CALLING_CONVENTION glDeleteBuffers)(int n, const unsigned int* buffers);
    void (RF_GL_CALLING_CONVENTION glClearColor)(float red, float green, float blue, float alpha);
    void (RF_GL_CALLING_CONVENTION glClear)(unsigned int mask);
    void (RF_GL_CALLING_CONVENTION glBindBuffer)(unsigned int target, unsigned int buffer);
    void (RF_GL_CALLING_CONVENTION glBufferSubData)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data);
    void (RF_GL_CALLING_CONVENTION glBindVertexArray)(unsigned int array);
    void (RF_GL_CALLING_CONVENTION glGenBuffers)(int n, unsigned int* buffers);
    void (RF_GL_CALLING_CONVENTION glBufferData)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
    void (RF_GL_CALLING_CONVENTION glVertexAttribPointer)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
    void (RF_GL_CALLING_CONVENTION glEnableVertexAttribArray)(unsigned int index);
    void (RF_GL_CALLING_CONVENTION glGenVertexArrays)(int n, unsigned int* arrays);
    void (RF_GL_CALLING_CONVENTION glVertexAttrib3f)(unsigned int index, float x, float y, float z);
    void (RF_GL_CALLING_CONVENTION glDisableVertexAttribArray)(unsigned int index);
    void (RF_GL_CALLING_CONVENTION glVertexAttrib4f)(unsigned int index, float x, float y, float z, float w);
    void (RF_GL_CALLING_CONVENTION glVertexAttrib2f)(unsigned int index, float x, float y);
    void (RF_GL_CALLING_CONVENTION glUseProgram)(unsigned int program);
    void (RF_GL_CALLING_CONVENTION glUniform4f)(int location, float v0, float v1, float v2, float v3);
    void (RF_GL_CALLING_CONVENTION glActiveTexture)(unsigned int texture);
    void (RF_GL_CALLING_CONVENTION glUniform1i)(int location, int v0);
    void (RF_GL_CALLING_CONVENTION glUniformMatrix4fv)(int location, int count, unsigned char transpose, const float* value);
    void (RF_GL_CALLING_CONVENTION glDrawElements)(unsigned int mode, int count, unsigned int type, const void* indices);
    void (RF_GL_CALLING_CONVENTION glDrawArrays)(unsigned int mode, int first, int count);
    void (RF_GL_CALLING_CONVENTION glPixelStorei)(unsigned int pname, int param);
    void (RF_GL_CALLING_CONVENTION glGenTextures)(int n, unsigned int* textures);
    void (RF_GL_CALLING_CONVENTION glTexImage2D)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
    void (RF_GL_CALLING_CONVENTION glGenRenderbuffers)(int n, unsigned int* renderbuffers);
    void (RF_GL_CALLING_CONVENTION glBindRenderbuffer)(unsigned int target, unsigned int renderbuffer);
    void (RF_GL_CALLING_CONVENTION glRenderbufferStorage)(unsigned int target, unsigned int internalformat, int width, int height);
    void (RF_GL_CALLING_CONVENTION glCompressedTexImage2D)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data);
    void (RF_GL_CALLING_CONVENTION glTexSubImage2D)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels);
    void (RF_GL_CALLING_CONVENTION glGenerateMipmap)(unsigned int target);
    void (RF_GL_CALLING_CONVENTION glReadPixels)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
    void (RF_GL_CALLING_CONVENTION glGenFramebuffers)(int n, unsigned int* framebuffers);
    void (RF_GL_CALLING_CONVENTION glFramebufferTexture2D)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
    void (RF_GL_CALLING_CONVENTION glFramebufferRenderbuffer)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
    unsigned int (RF_GL_CALLING_CONVENTION glCheckFramebufferStatus)(unsigned int target);
    unsigned int (RF_GL_CALLING_CONVENTION glCreateShader)(unsigned int type);
    void (RF_GL_CALLING_CONVENTION glShaderSource)(unsigned int shader, int count, const char** string, const int* length);
    void (RF_GL_CALLING_CONVENTION glCompileShader)(unsigned int shader);
    void (RF_GL_CALLING_CONVENTION glGetShaderiv)(unsigned int shader, unsigned int pname, int* params);
    void (RF_GL_CALLING_CONVENTION glGetShaderInfoLog)(unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int (RF_GL_CALLING_CONVENTION glCreateProgram)();
    void (RF_GL_CALLING_CONVENTION glAttachShader)(unsigned int program, unsigned int shader);
    void (RF_GL_CALLING_CONVENTION glBindAttribLocation)(unsigned int program, unsigned int index, const char* name);
    void (RF_GL_CALLING_CONVENTION glLinkProgram)(unsigned int program);
    void (RF_GL_CALLING_CONVENTION glGetProgramiv)(unsigned int program, unsigned int pname, int* params);
    void (RF_GL_CALLING_CONVENTION glGetProgramInfoLog)(unsigned int program, int bufSize, int* length, char* infoLog);
    void (RF_GL_CALLING_CONVENTION glDeleteProgram)(unsigned int program);
    int (RF_GL_CALLING_CONVENTION glGetAttribLocation)(unsigned int program, const char* name);
    int (RF_GL_CALLING_CONVENTION glGetUniformLocation)(unsigned int program, const char* name);
    void (RF_GL_CALLING_CONVENTION glDetachShader)(unsigned int program, unsigned int shader);
    void (RF_GL_CALLING_CONVENTION glDeleteShader)(unsigned int shader);

    void (RF_GL_CALLING_CONVENTION glGetTexImage)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels); // NULL for OpenGL ES3

    void (RF_GL_CALLING_CONVENTION glGetActiveUniform)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name);
    void (RF_GL_CALLING_CONVENTION glUniform1f)(int location, float v0);
    void (RF_GL_CALLING_CONVENTION glUniform1fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION glUniform2fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION glUniform3fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION glUniform4fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION glUniform1iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION glUniform2iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION glUniform3iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION glUniform4iv)(int location, int count, const int* value);
    const unsigned char* (RF_GL_CALLING_CONVENTION glGetString)(unsigned int name);
    void (RF_GL_CALLING_CONVENTION glGetFloatv)(unsigned int pname, float* data);
    void (RF_GL_CALLING_CONVENTION glDepthFunc)(unsigned int func);
    void (RF_GL_CALLING_CONVENTION glBlendFunc)(unsigned int sfactor, unsigned int dfactor);
    void (RF_GL_CALLING_CONVENTION glCullFace)(unsigned int mode);
    void (RF_GL_CALLING_CONVENTION glFrontFace)(unsigned int mode);

    //On OpenGL33 we only set glClearDepth but on OpenGL ES3 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version
    void (RF_GL_CALLING_CONVENTION glClearDepth)(double depth);
    void (RF_GL_CALLING_CONVENTION glClearDepthf)(float depth);

    void (RF_GL_CALLING_CONVENTION glGetIntegerv)(unsigned int pname, int* data); //OpenGL 33 only, can be NULL in OpenGL ES3
    const unsigned char* (RF_GL_CALLING_CONVENTION glGetStringi)(unsigned int name, unsigned int index);

    void (RF_GL_CALLING_CONVENTION glPolygonMode)(unsigned int face, unsigned int mode); //OpenGL 33 only
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
    ((void (RF_GL_CALLING_CONVENTION )(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Viewport)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int texture))RF_OPENGL_PROC_EXT(ext, BindTexture)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, TexParameteri)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int pname, float param))RF_OPENGL_PROC_EXT(ext, TexParameterf)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int pname, const int* params))RF_OPENGL_PROC_EXT(ext, TexParameteriv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int framebuffer))RF_OPENGL_PROC_EXT(ext, BindFramebuffer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Enable)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Disable)),\
    ((void (RF_GL_CALLING_CONVENTION )(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Scissor)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* textures))RF_OPENGL_PROC_EXT(ext, DeleteTextures)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, DeleteRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, DeleteFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, DeleteVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, const unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, DeleteBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(float red, float green, float blue, float alpha))RF_OPENGL_PROC_EXT(ext, ClearColor)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mask))RF_OPENGL_PROC_EXT(ext, Clear)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int buffer))RF_OPENGL_PROC_EXT(ext, BindBuffer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data))RF_OPENGL_PROC_EXT(ext, BufferSubData)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int array))RF_OPENGL_PROC_EXT(ext, BindVertexArray)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, GenBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage))RF_OPENGL_PROC_EXT(ext, BufferData)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer))RF_OPENGL_PROC_EXT(ext, VertexAttribPointer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index))RF_OPENGL_PROC_EXT(ext, EnableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, GenVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, float x, float y, float z))RF_OPENGL_PROC_EXT(ext, VertexAttrib3f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index))RF_OPENGL_PROC_EXT(ext, DisableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, float x, float y, float z, float w))RF_OPENGL_PROC_EXT(ext, VertexAttrib4f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int index, float x, float y))RF_OPENGL_PROC_EXT(ext, VertexAttrib2f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program))RF_OPENGL_PROC_EXT(ext, UseProgram)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, float v0, float v1, float v2, float v3))RF_OPENGL_PROC_EXT(ext, Uniform4f)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int texture))RF_OPENGL_PROC_EXT(ext, ActiveTexture)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int v0))RF_OPENGL_PROC_EXT(ext, Uniform1i)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, unsigned char transpose, const float* value))RF_OPENGL_PROC_EXT(ext, UniformMatrix4fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode, int count, unsigned int type, const void* indices))RF_OPENGL_PROC_EXT(ext, DrawElements)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode, int first, int count))RF_OPENGL_PROC_EXT(ext, DrawArrays)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, PixelStorei)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* textures))RF_OPENGL_PROC_EXT(ext, GenTextures)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, GenRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, BindRenderbuffer)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int internalformat, int width, int height))RF_OPENGL_PROC_EXT(ext, RenderbufferStorage)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data))RF_OPENGL_PROC_EXT(ext, CompressedTexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexSubImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target))RF_OPENGL_PROC_EXT(ext, GenerateMipmap)),\
    ((void (RF_GL_CALLING_CONVENTION )(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, ReadPixels)),\
    ((void (RF_GL_CALLING_CONVENTION )(int n, unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, GenFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level))RF_OPENGL_PROC_EXT(ext, FramebufferTexture2D)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, FramebufferRenderbuffer)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION )(unsigned int target))RF_OPENGL_PROC_EXT(ext, CheckFramebufferStatus)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION )(unsigned int type))RF_OPENGL_PROC_EXT(ext, CreateShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader, int count, const char** string, const int* length))RF_OPENGL_PROC_EXT(ext, ShaderSource)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader))RF_OPENGL_PROC_EXT(ext, CompileShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetShaderiv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetShaderInfoLog)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION )())RF_OPENGL_PROC_EXT(ext, CreateProgram)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, AttachShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int index, const char* name))RF_OPENGL_PROC_EXT(ext, BindAttribLocation)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program))RF_OPENGL_PROC_EXT(ext, LinkProgram)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetProgramiv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetProgramInfoLog)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program))RF_OPENGL_PROC_EXT(ext, DeleteProgram)),\
    ((int (RF_GL_CALLING_CONVENTION )(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetAttribLocation)),\
    ((int (RF_GL_CALLING_CONVENTION )(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetUniformLocation)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, DetachShader)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int shader))RF_OPENGL_PROC_EXT(ext, DeleteShader)),\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, GetTexImage)),\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name))RF_OPENGL_PROC_EXT(ext, GetActiveUniform)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, float v0))RF_OPENGL_PROC_EXT(ext, Uniform1f)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform1fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform2fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform3fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform4fv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform1iv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform2iv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform3iv)),\
    ((void (RF_GL_CALLING_CONVENTION )(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform4iv)),\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION )(unsigned int name))RF_OPENGL_PROC_EXT(ext, GetString)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int pname, float* data))RF_OPENGL_PROC_EXT(ext, GetFloatv)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int func))RF_OPENGL_PROC_EXT(ext, DepthFunc)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int sfactor, unsigned int dfactor))RF_OPENGL_PROC_EXT(ext, BlendFunc)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode))RF_OPENGL_PROC_EXT(ext, CullFace)),\
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int mode))RF_OPENGL_PROC_EXT(ext, FrontFace)),\
    \
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/\
    ((void (RF_GL_CALLING_CONVENTION )(double depth))RF_OPENGL_PROC_EXT(ext, ClearDepth)),\
    NULL /*glClearDepthf*/,\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int pname, int* data))RF_OPENGL_PROC_EXT(ext, GetIntegerv)),\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION )(unsigned int name, unsigned int index))RF_OPENGL_PROC_EXT(ext, GetStringi)),\
    \
    ((void (RF_GL_CALLING_CONVENTION )(unsigned int face, unsigned int mode))RF_OPENGL_PROC_EXT(ext, PolygonMode)), /*OpenGL 33 only*/ \
}

#else
#define RF_DEFAULT_OPENGL_PROCS_EXT(ext) (rf_opengl_procs) { \
    ((void (RF_GL_CALLING_CONVENTION)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Viewport)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int texture))RF_OPENGL_PROC_EXT(ext, BindTexture)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, TexParameteri)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int pname, float param))RF_OPENGL_PROC_EXT(ext, TexParameterf)),\
    NULL, /*glTexParameteri not needed for OpenGL ES3*/ \
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int framebuffer))RF_OPENGL_PROC_EXT(ext, BindFramebuffer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Enable)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int cap))RF_OPENGL_PROC_EXT(ext, Disable)),\
    ((void (RF_GL_CALLING_CONVENTION)(int x, int y, int width, int height))RF_OPENGL_PROC_EXT(ext, Scissor)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* textures))RF_OPENGL_PROC_EXT(ext, DeleteTextures)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, DeleteRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, DeleteFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, DeleteVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, const unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, DeleteBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(float red, float green, float blue, float alpha))RF_OPENGL_PROC_EXT(ext, ClearColor)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mask))RF_OPENGL_PROC_EXT(ext, Clear)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int buffer))RF_OPENGL_PROC_EXT(ext, BindBuffer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data))RF_OPENGL_PROC_EXT(ext, BufferSubData)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int array))RF_OPENGL_PROC_EXT(ext, BindVertexArray)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* buffers))RF_OPENGL_PROC_EXT(ext, GenBuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage))RF_OPENGL_PROC_EXT(ext, BufferData)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer))RF_OPENGL_PROC_EXT(ext, VertexAttribPointer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index))RF_OPENGL_PROC_EXT(ext, EnableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* arrays))RF_OPENGL_PROC_EXT(ext, GenVertexArrays)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, float x, float y, float z))RF_OPENGL_PROC_EXT(ext, VertexAttrib3f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index))RF_OPENGL_PROC_EXT(ext, DisableVertexAttribArray)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, float x, float y, float z, float w))RF_OPENGL_PROC_EXT(ext, VertexAttrib4f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int index, float x, float y))RF_OPENGL_PROC_EXT(ext, VertexAttrib2f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program))RF_OPENGL_PROC_EXT(ext, UseProgram)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, float v0, float v1, float v2, float v3))RF_OPENGL_PROC_EXT(ext, Uniform4f)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int texture))RF_OPENGL_PROC_EXT(ext, ActiveTexture)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int v0))RF_OPENGL_PROC_EXT(ext, Uniform1i)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, unsigned char transpose, const float* value))RF_OPENGL_PROC_EXT(ext, UniformMatrix4fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode, int count, unsigned int type, const void* indices))RF_OPENGL_PROC_EXT(ext, DrawElements)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode, int first, int count))RF_OPENGL_PROC_EXT(ext, DrawArrays)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int pname, int param))RF_OPENGL_PROC_EXT(ext, PixelStorei)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* textures))RF_OPENGL_PROC_EXT(ext, GenTextures)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* renderbuffers))RF_OPENGL_PROC_EXT(ext, GenRenderbuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, BindRenderbuffer)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int internalformat, int width, int height))RF_OPENGL_PROC_EXT(ext, RenderbufferStorage)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data))RF_OPENGL_PROC_EXT(ext, CompressedTexImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels))RF_OPENGL_PROC_EXT(ext, TexSubImage2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target))RF_OPENGL_PROC_EXT(ext, GenerateMipmap)),\
    ((void (RF_GL_CALLING_CONVENTION)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels))RF_OPENGL_PROC_EXT(ext, ReadPixels)),\
    ((void (RF_GL_CALLING_CONVENTION)(int n, unsigned int* framebuffers))RF_OPENGL_PROC_EXT(ext, GenFramebuffers)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level))RF_OPENGL_PROC_EXT(ext, FramebufferTexture2D)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer))RF_OPENGL_PROC_EXT(ext, FramebufferRenderbuffer)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION)(unsigned int target))RF_OPENGL_PROC_EXT(ext, CheckFramebufferStatus)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION)(unsigned int type))RF_OPENGL_PROC_EXT(ext, CreateShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader, int count, const char** string, const int* length))RF_OPENGL_PROC_EXT(ext, ShaderSource)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, CompileShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetShaderiv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetShaderInfoLog)),\
    ((unsigned int (RF_GL_CALLING_CONVENTION)())RF_OPENGL_PROC_EXT(ext, CreateProgram)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, AttachShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int index, const char* name))RF_OPENGL_PROC_EXT(ext, BindAttribLocation)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program))RF_OPENGL_PROC_EXT(ext, LinkProgram)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int pname, int* params))RF_OPENGL_PROC_EXT(ext, GetProgramiv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, int bufSize, int* length, char* infoLog))RF_OPENGL_PROC_EXT(ext, GetProgramInfoLog)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program))RF_OPENGL_PROC_EXT(ext, DeleteProgram)),\
    ((int (RF_GL_CALLING_CONVENTION)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetAttribLocation)),\
    ((int (RF_GL_CALLING_CONVENTION)(unsigned int program, const char* name))RF_OPENGL_PROC_EXT(ext, GetUniformLocation)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int shader))RF_OPENGL_PROC_EXT(ext, DetachShader)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int shader))RF_OPENGL_PROC_EXT(ext, DeleteShader)),\
    NULL, /*glGetTexImage is NULL for opengl es3*/ \
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name))RF_OPENGL_PROC_EXT(ext, GetActiveUniform)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, float v0))RF_OPENGL_PROC_EXT(ext, Uniform1f)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform1fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform2fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform3fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const float* value))RF_OPENGL_PROC_EXT(ext, Uniform4fv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform1iv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform2iv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform3iv)),\
    ((void (RF_GL_CALLING_CONVENTION)(int location, int count, const int* value))RF_OPENGL_PROC_EXT(ext, Uniform4iv)),\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION)(unsigned int name))RF_OPENGL_PROC_EXT(ext, GetString)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int pname, float* data))RF_OPENGL_PROC_EXT(ext, GetFloatv)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int func))RF_OPENGL_PROC_EXT(ext, DepthFunc)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int sfactor, unsigned int dfactor))RF_OPENGL_PROC_EXT(ext, BlendFunc)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, CullFace)),\
    ((void (RF_GL_CALLING_CONVENTION)(unsigned int mode))RF_OPENGL_PROC_EXT(ext, FrontFace)),\
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/\
    NULL /*glClearDepth*/,\
    ((void (RF_GL_CALLING_CONVENTION)(float depth))RF_OPENGL_PROC_EXT(ext, ClearDepthf)),\
    NULL /*OpenGL 33 only, should be NULL in OpenGL ES3*/,\
    ((const unsigned char* (RF_GL_CALLING_CONVENTION)(unsigned int name, unsigned int index))RF_OPENGL_PROC_EXT(ext, GetStringi)),\
    NULL /*glPolygonMode is OpenGL 33 only*/ \
}
#endif

#define RF_DEFAULT_OPENGL_PROCS RF_DEFAULT_OPENGL_PROCS_EXT(gl)

//endregion

#endif