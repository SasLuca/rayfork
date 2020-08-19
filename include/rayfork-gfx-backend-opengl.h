#if !defined(RAYFORK_GFX_BACKEND_OPENGL_H) && (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3))
#define RAYFORK_GFX_BACKEND_OPENGL_H

#include "rayfork-std.h"

#if !defined(RF_GL_CALLING_CONVENTION) && (defined(RAYFORK_PLATFORM_WINDOWS) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__))
    #define RF_GL_CALLING_CONVENTION __stdcall *
#else
    #define RF_GL_CALLING_CONVENTION *
#endif

typedef struct rf_opengl_procs
{
    void (RF_GL_CALLING_CONVENTION Viewport)(int x, int y, int width, int height);
    void (RF_GL_CALLING_CONVENTION BindTexture)(unsigned int target, unsigned int texture);
    void (RF_GL_CALLING_CONVENTION TexParameteri)(unsigned int target, unsigned int pname, int param);
    void (RF_GL_CALLING_CONVENTION TexParameterf)(unsigned int target, unsigned int pname, float param);
    void (RF_GL_CALLING_CONVENTION TexParameteriv)(unsigned int target, unsigned int pname, const int* params);
    void (RF_GL_CALLING_CONVENTION BindFramebuffer)(unsigned int target, unsigned int framebuffer);
    void (RF_GL_CALLING_CONVENTION Enable)(unsigned int cap);
    void (RF_GL_CALLING_CONVENTION Disable)(unsigned int cap);
    void (RF_GL_CALLING_CONVENTION Scissor)(int x, int y, int width, int height);
    void (RF_GL_CALLING_CONVENTION DeleteTextures)(int n, const unsigned int* textures);
    void (RF_GL_CALLING_CONVENTION DeleteRenderbuffers)(int n, const unsigned int* renderbuffers);
    void (RF_GL_CALLING_CONVENTION DeleteFramebuffers)(int n, const unsigned int* framebuffers);
    void (RF_GL_CALLING_CONVENTION DeleteVertexArrays)(int n, const unsigned int* arrays);
    void (RF_GL_CALLING_CONVENTION DeleteBuffers)(int n, const unsigned int* buffers);
    void (RF_GL_CALLING_CONVENTION ClearColor)(float red, float green, float blue, float alpha);
    void (RF_GL_CALLING_CONVENTION Clear)(unsigned int mask);
    void (RF_GL_CALLING_CONVENTION BindBuffer)(unsigned int target, unsigned int buffer);
    void (RF_GL_CALLING_CONVENTION BufferSubData)(unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data);
    void (RF_GL_CALLING_CONVENTION BindVertexArray)(unsigned int array);
    void (RF_GL_CALLING_CONVENTION GenBuffers)(int n, unsigned int* buffers);
    void (RF_GL_CALLING_CONVENTION BufferData)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
    void (RF_GL_CALLING_CONVENTION VertexAttribPointer)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
    void (RF_GL_CALLING_CONVENTION EnableVertexAttribArray)(unsigned int index);
    void (RF_GL_CALLING_CONVENTION GenVertexArrays)(int n, unsigned int* arrays);
    void (RF_GL_CALLING_CONVENTION VertexAttrib3f)(unsigned int index, float x, float y, float z);
    void (RF_GL_CALLING_CONVENTION DisableVertexAttribArray)(unsigned int index);
    void (RF_GL_CALLING_CONVENTION VertexAttrib4f)(unsigned int index, float x, float y, float z, float w);
    void (RF_GL_CALLING_CONVENTION VertexAttrib2f)(unsigned int index, float x, float y);
    void (RF_GL_CALLING_CONVENTION UseProgram)(unsigned int program);
    void (RF_GL_CALLING_CONVENTION Uniform4f)(int location, float v0, float v1, float v2, float v3);
    void (RF_GL_CALLING_CONVENTION ActiveTexture)(unsigned int texture);
    void (RF_GL_CALLING_CONVENTION Uniform1i)(int location, int v0);
    void (RF_GL_CALLING_CONVENTION UniformMatrix4fv)(int location, int count, unsigned char transpose, const float* value);
    void (RF_GL_CALLING_CONVENTION DrawElements)(unsigned int mode, int count, unsigned int type, const void* indices);
    void (RF_GL_CALLING_CONVENTION DrawArrays)(unsigned int mode, int first, int count);
    void (RF_GL_CALLING_CONVENTION PixelStorei)(unsigned int pname, int param);
    void (RF_GL_CALLING_CONVENTION GenTextures)(int n, unsigned int* textures);
    void (RF_GL_CALLING_CONVENTION TexImage2D)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
    void (RF_GL_CALLING_CONVENTION GenRenderbuffers)(int n, unsigned int* renderbuffers);
    void (RF_GL_CALLING_CONVENTION BindRenderbuffer)(unsigned int target, unsigned int renderbuffer);
    void (RF_GL_CALLING_CONVENTION RenderbufferStorage)(unsigned int target, unsigned int internalformat, int width, int height);
    void (RF_GL_CALLING_CONVENTION CompressedTexImage2D)(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data);
    void (RF_GL_CALLING_CONVENTION TexSubImage2D)(unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels);
    void (RF_GL_CALLING_CONVENTION GenerateMipmap)(unsigned int target);
    void (RF_GL_CALLING_CONVENTION ReadPixels)(int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
    void (RF_GL_CALLING_CONVENTION GenFramebuffers)(int n, unsigned int* framebuffers);
    void (RF_GL_CALLING_CONVENTION FramebufferTexture2D)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
    void (RF_GL_CALLING_CONVENTION FramebufferRenderbuffer)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
    unsigned int (RF_GL_CALLING_CONVENTION CheckFramebufferStatus)(unsigned int target);
    unsigned int (RF_GL_CALLING_CONVENTION CreateShader)(unsigned int type);
    void (RF_GL_CALLING_CONVENTION ShaderSource)(unsigned int shader, int count, const char** string, const int* length);
    void (RF_GL_CALLING_CONVENTION CompileShader)(unsigned int shader);
    void (RF_GL_CALLING_CONVENTION GetShaderiv)(unsigned int shader, unsigned int pname, int* params);
    void (RF_GL_CALLING_CONVENTION GetShaderInfoLog)(unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int (RF_GL_CALLING_CONVENTION CreateProgram)();
    void (RF_GL_CALLING_CONVENTION AttachShader)(unsigned int program, unsigned int shader);
    void (RF_GL_CALLING_CONVENTION BindAttribLocation)(unsigned int program, unsigned int index, const char* name);
    void (RF_GL_CALLING_CONVENTION LinkProgram)(unsigned int program);
    void (RF_GL_CALLING_CONVENTION GetProgramiv)(unsigned int program, unsigned int pname, int* params);
    void (RF_GL_CALLING_CONVENTION GetProgramInfoLog)(unsigned int program, int bufSize, int* length, char* infoLog);
    void (RF_GL_CALLING_CONVENTION DeleteProgram)(unsigned int program);
    int (RF_GL_CALLING_CONVENTION GetAttribLocation)(unsigned int program, const char* name);
    int (RF_GL_CALLING_CONVENTION GetUniformLocation)(unsigned int program, const char* name);
    void (RF_GL_CALLING_CONVENTION DetachShader)(unsigned int program, unsigned int shader);
    void (RF_GL_CALLING_CONVENTION DeleteShader)(unsigned int shader);

    void (RF_GL_CALLING_CONVENTION GetTexImage)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels); // NULL for OpenGL ES3

    void (RF_GL_CALLING_CONVENTION GetActiveUniform)(unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name);
    void (RF_GL_CALLING_CONVENTION Uniform1f)(int location, float v0);
    void (RF_GL_CALLING_CONVENTION Uniform1fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform2fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform3fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform4fv)(int location, int count, const float* value);
    void (RF_GL_CALLING_CONVENTION Uniform1iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION Uniform2iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION Uniform3iv)(int location, int count, const int* value);
    void (RF_GL_CALLING_CONVENTION Uniform4iv)(int location, int count, const int* value);
    const unsigned char* (RF_GL_CALLING_CONVENTION GetString)(unsigned int name);
    void (RF_GL_CALLING_CONVENTION GetFloatv)(unsigned int pname, float* data);
    void (RF_GL_CALLING_CONVENTION DepthFunc)(unsigned int func);
    void (RF_GL_CALLING_CONVENTION BlendFunc)(unsigned int sfactor, unsigned int dfactor);
    void (RF_GL_CALLING_CONVENTION CullFace)(unsigned int mode);
    void (RF_GL_CALLING_CONVENTION FrontFace)(unsigned int mode);

    //On OpenGL33 we only set glClearDepth but on OpenGL ES3 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version
    void (RF_GL_CALLING_CONVENTION ClearDepth)(double depth);
    void (RF_GL_CALLING_CONVENTION ClearDepthf)(float depth);

    void (RF_GL_CALLING_CONVENTION GetIntegerv)(unsigned int pname, int* data); // OpenGL 33 only, can be NULL in OpenGL ES3
    const unsigned char* (RF_GL_CALLING_CONVENTION GetStringi)(unsigned int name, unsigned int index);

    void (RF_GL_CALLING_CONVENTION PolygonMode)(unsigned int face, unsigned int mode); // OpenGL 33 only
} rf_opengl_procs;

#define RF_GL_PROC_DEF(ext, gl_proc_name)  .gl_proc_name = (void*) RF_CONCAT(ext, gl_proc_name)
#define RF_GL_PROC_NULL(ext, gl_proc_name) .gl_proc_name = NULL

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)

#define RF_DEFAULT_OPENGL_PROCS_EXT(ext) &(rf_opengl_procs) {\
    RF_GL_PROC_DEF(ext, Viewport),\
    RF_GL_PROC_DEF(ext, BindTexture),\
    RF_GL_PROC_DEF(ext, TexParameteri),\
    RF_GL_PROC_DEF(ext, TexParameterf),\
    RF_GL_PROC_DEF(ext, TexParameteriv),\
    RF_GL_PROC_DEF(ext, BindFramebuffer),\
    RF_GL_PROC_DEF(ext, Enable),\
    RF_GL_PROC_DEF(ext, Disable),\
    RF_GL_PROC_DEF(ext, Scissor),\
    RF_GL_PROC_DEF(ext, DeleteTextures),\
    RF_GL_PROC_DEF(ext, DeleteRenderbuffers),\
    RF_GL_PROC_DEF(ext, DeleteFramebuffers),\
    RF_GL_PROC_DEF(ext, DeleteVertexArrays),\
    RF_GL_PROC_DEF(ext, DeleteBuffers),\
    RF_GL_PROC_DEF(ext, ClearColor),\
    RF_GL_PROC_DEF(ext, Clear),\
    RF_GL_PROC_DEF(ext, BindBuffer),\
    RF_GL_PROC_DEF(ext, BufferSubData),\
    RF_GL_PROC_DEF(ext, BindVertexArray),\
    RF_GL_PROC_DEF(ext, GenBuffers),\
    RF_GL_PROC_DEF(ext, BufferData),\
    RF_GL_PROC_DEF(ext, VertexAttribPointer),\
    RF_GL_PROC_DEF(ext, EnableVertexAttribArray),\
    RF_GL_PROC_DEF(ext, GenVertexArrays),\
    RF_GL_PROC_DEF(ext, VertexAttrib3f),\
    RF_GL_PROC_DEF(ext, DisableVertexAttribArray),\
    RF_GL_PROC_DEF(ext, VertexAttrib4f),\
    RF_GL_PROC_DEF(ext, VertexAttrib2f),\
    RF_GL_PROC_DEF(ext, UseProgram),\
    RF_GL_PROC_DEF(ext, Uniform4f),\
    RF_GL_PROC_DEF(ext, ActiveTexture),\
    RF_GL_PROC_DEF(ext, Uniform1i),\
    RF_GL_PROC_DEF(ext, UniformMatrix4fv),\
    RF_GL_PROC_DEF(ext, DrawElements),\
    RF_GL_PROC_DEF(ext, DrawArrays),\
    RF_GL_PROC_DEF(ext, PixelStorei),\
    RF_GL_PROC_DEF(ext, GenTextures),\
    RF_GL_PROC_DEF(ext, TexImage2D),\
    RF_GL_PROC_DEF(ext, GenRenderbuffers),\
    RF_GL_PROC_DEF(ext, BindRenderbuffer),\
    RF_GL_PROC_DEF(ext, RenderbufferStorage),\
    RF_GL_PROC_DEF(ext, CompressedTexImage2D),\
    RF_GL_PROC_DEF(ext, TexSubImage2D),\
    RF_GL_PROC_DEF(ext, GenerateMipmap),\
    RF_GL_PROC_DEF(ext, ReadPixels),\
    RF_GL_PROC_DEF(ext, GenFramebuffers),\
    RF_GL_PROC_DEF(ext, FramebufferTexture2D),\
    RF_GL_PROC_DEF(ext, FramebufferRenderbuffer),\
    RF_GL_PROC_DEF(ext, CheckFramebufferStatus),\
    RF_GL_PROC_DEF(ext, CreateShader),\
    RF_GL_PROC_DEF(ext, ShaderSource),\
    RF_GL_PROC_DEF(ext, CompileShader),\
    RF_GL_PROC_DEF(ext, GetShaderiv),\
    RF_GL_PROC_DEF(ext, GetShaderInfoLog),\
    RF_GL_PROC_DEF(ext, CreateProgram),\
    RF_GL_PROC_DEF(ext, AttachShader),\
    RF_GL_PROC_DEF(ext, BindAttribLocation),\
    RF_GL_PROC_DEF(ext, LinkProgram),\
    RF_GL_PROC_DEF(ext, GetProgramiv),\
    RF_GL_PROC_DEF(ext, GetProgramInfoLog),\
    RF_GL_PROC_DEF(ext, DeleteProgram),\
    RF_GL_PROC_DEF(ext, GetAttribLocation),\
    RF_GL_PROC_DEF(ext, GetUniformLocation),\
    RF_GL_PROC_DEF(ext, DetachShader),\
    RF_GL_PROC_DEF(ext, DeleteShader),\
    RF_GL_PROC_DEF(ext, GetTexImage),\
    RF_GL_PROC_DEF(ext, GetActiveUniform),\
    RF_GL_PROC_DEF(ext, Uniform1f),\
    RF_GL_PROC_DEF(ext, Uniform1fv),\
    RF_GL_PROC_DEF(ext, Uniform2fv),\
    RF_GL_PROC_DEF(ext, Uniform3fv),\
    RF_GL_PROC_DEF(ext, Uniform4fv),\
    RF_GL_PROC_DEF(ext, Uniform1iv),\
    RF_GL_PROC_DEF(ext, Uniform2iv),\
    RF_GL_PROC_DEF(ext, Uniform3iv),\
    RF_GL_PROC_DEF(ext, Uniform4iv),\
    RF_GL_PROC_DEF(ext, GetString),\
    RF_GL_PROC_DEF(ext, GetFloatv),\
    RF_GL_PROC_DEF(ext, DepthFunc),\
    RF_GL_PROC_DEF(ext, BlendFunc),\
    RF_GL_PROC_DEF(ext, CullFace),\
    RF_GL_PROC_DEF(ext, FrontFace),\
    RF_GL_PROC_DEF(ext, ClearDepth),  \
    RF_GL_PROC_NULL(ext, ClearDepthf),\
    RF_GL_PROC_DEF(ext, GetIntegerv), \
    RF_GL_PROC_DEF(ext, GetStringi),  \
    RF_GL_PROC_DEF(ext, PolygonMode), \
}
    /*On OpenGL33 we only set glClearDepth but on OpenGLES2 we set glClearDepthf. In the gl backend we use a macro to choose the correct glClearDepth function depending on the gl version*/ \

#elif defined(RAYFORK_GRAPHICS_BACKEND_GL_33)


#endif

#define RF_DEFAULT_GFX_BACKEND_INIT_DATA (RF_DEFAULT_OPENGL_PROCS_EXT(gl))

typedef float rf_gfx_vertex_data_type;
typedef float rf_gfx_texcoord_data_type;
typedef unsigned char rf_gfx_color_data_type;
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
typedef unsigned int rf_gfx_vertex_index_data_type;
#else
typedef unsigned short rf_gfx_vertex_index_data_type;
#endif

#define RF_GFX_VERTEX_COMPONENT_COUNT       (3 * 4) // 3 float by vertex, 4 vertex by quad
#define RF_GFX_TEXCOORD_COMPONENT_COUNT     (2 * 4) // 2 float by texcoord, 4 texcoord by quad
#define RF_GFX_COLOR_COMPONENT_COUNT        (4 * 4) // 4 float by color, 4 colors by quad
#define RF_GFX_VERTEX_INDEX_COMPONENT_COUNT (6)     // 6 int by quad (indices)

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rf_vertex_buffer
{
    int elements_count; // Number of elements in the buffer (QUADS)
    int v_counter;      // Vertex position counter to process (and draw) from full buffer
    int tc_counter;     // Vertex texcoord counter to process (and draw) from full buffer
    int c_counter;      // Vertex color counter to process (and draw) from full buffer

    unsigned int vao_id;    // OpenGL Vertex Array Object id
    unsigned int vbo_id[4]; // OpenGL Vertex Buffer Objects id (4 types of vertex data)

    rf_gfx_vertex_data_type*       vertices;   // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    rf_gfx_texcoord_data_type*     texcoords;  // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    rf_gfx_color_data_type*        colors;     // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    rf_gfx_vertex_index_data_type* indices;    // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
} rf_vertex_buffer;

typedef struct rf_draw_call
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
} rf_draw_call;

typedef struct rf_gfx_context
{
    rf_opengl_procs gl;

    struct {
        bool tex_comp_dxt_supported;             // DDS texture compression support
        bool tex_comp_etc1_supported;            // ETC1 texture compression support
        bool tex_comp_etc2_supported;            // ETC2/EAC texture compression support
        bool tex_comp_pvrt_supported;            // PVR texture compression support
        bool tex_comp_astc_supported;            // ASTC texture compression support
        bool tex_npot_supported;                 // NPOT textures full support
        bool tex_float_supported;                // float textures support (32 bit per channel)
        bool tex_depth_supported;                // Depth textures supported
        int max_depth_bits;                      // Maximum bits for depth component
        bool tex_mirror_clamp_supported;         // Clamp mirror wrap mode supported
        bool tex_anisotropic_filter_supported;   // Anisotropic texture filtering support
        float max_anisotropic_level;             // Maximum anisotropy level supported (minimum is 2.0f)
        bool debug_marker_supported;             // Debug marker support
    } extensions;
} rf_gfx_context;

#endif // !defined(RAYFORK_GFX_BACKEND_OPENGL_H) && (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3))