#ifndef RAYFORK_INTERNAL_GFX_BACKEND_OPENGL3_GLPROCS_H
#define RAYFORK_INTERNAL_GFX_BACKEND_OPENGL3_GLPROCS_H

#if !defined(rf_gl_calling_convention) && (defined(rayfork_platform_windows) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__))
    #define rf_gl_calling_convention __stdcall *
#else
    #define rf_gl_calling_convention *
#endif

typedef struct rf_opengl_procs
{
    void                 (rf_gl_calling_convention Viewport)                 (int x, int y, int width, int height);
    void                 (rf_gl_calling_convention BindTexture)              (unsigned int target, unsigned int texture);
    void                 (rf_gl_calling_convention TexParameteri)            (unsigned int target, unsigned int pname, int param);
    void                 (rf_gl_calling_convention TexParameterf)            (unsigned int target, unsigned int pname, float param);
    void                 (rf_gl_calling_convention TexParameteriv)           (unsigned int target, unsigned int pname, const int* params);
    void                 (rf_gl_calling_convention BindFramebuffer)          (unsigned int target, unsigned int framebuffer);
    void                 (rf_gl_calling_convention Enable)                   (unsigned int cap);
    void                 (rf_gl_calling_convention Disable)                  (unsigned int cap);
    void                 (rf_gl_calling_convention Scissor)                  (int x, int y, int width, int height);
    void                 (rf_gl_calling_convention DeleteTextures)           (int n, const unsigned int* textures);
    void                 (rf_gl_calling_convention DeleteRenderbuffers)      (int n, const unsigned int* renderbuffers);
    void                 (rf_gl_calling_convention DeleteFramebuffers)       (int n, const unsigned int* framebuffers);
    void                 (rf_gl_calling_convention DeleteVertexArrays)       (int n, const unsigned int* arrays);
    void                 (rf_gl_calling_convention DeleteBuffers)            (int n, const unsigned int* buffers);
    void                 (rf_gl_calling_convention ClearColor)               (float red, float green, float blue, float alpha);
    void                 (rf_gl_calling_convention Clear)                    (unsigned int mask);
    void                 (rf_gl_calling_convention BindBuffer)               (unsigned int target, unsigned int buffer);
    void                 (rf_gl_calling_convention BufferSubData)            (unsigned int target, ptrdiff_t offset, ptrdiff_t size, const void* data);
    void                 (rf_gl_calling_convention BindVertexArray)          (unsigned int array);
    void                 (rf_gl_calling_convention GenBuffers)               (int n, unsigned int* buffers);
    void                 (rf_gl_calling_convention BufferData)               (unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
    void                 (rf_gl_calling_convention VertexAttribPointer)      (unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
    void                 (rf_gl_calling_convention EnableVertexAttribArray)  (unsigned int index);
    void                 (rf_gl_calling_convention GenVertexArrays)          (int n, unsigned int* arrays);
    void                 (rf_gl_calling_convention VertexAttrib3f)           (unsigned int index, float x, float y, float z);
    void                 (rf_gl_calling_convention DisableVertexAttribArray) (unsigned int index);
    void                 (rf_gl_calling_convention VertexAttrib4f)           (unsigned int index, float x, float y, float z, float w);
    void                 (rf_gl_calling_convention VertexAttrib2f)           (unsigned int index, float x, float y);
    void                 (rf_gl_calling_convention UseProgram)               (unsigned int program);
    void                 (rf_gl_calling_convention Uniform4f)                (int location, float v0, float v1, float v2, float v3);
    void                 (rf_gl_calling_convention ActiveTexture)            (unsigned int texture);
    void                 (rf_gl_calling_convention Uniform1i)                (int location, int v0);
    void                 (rf_gl_calling_convention UniformMatrix4fv)         (int location, int count, unsigned char transpose, const float* value);
    void                 (rf_gl_calling_convention DrawElements)             (unsigned int mode, int count, unsigned int type, const void* indices);
    void                 (rf_gl_calling_convention DrawArrays)               (unsigned int mode, int first, int count);
    void                 (rf_gl_calling_convention PixelStorei)              (unsigned int pname, int param);
    void                 (rf_gl_calling_convention GenTextures)              (int n, unsigned int* textures);
    void                 (rf_gl_calling_convention TexImage2D)               (unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
    void                 (rf_gl_calling_convention GenRenderbuffers)         (int n, unsigned int* renderbuffers);
    void                 (rf_gl_calling_convention BindRenderbuffer)         (unsigned int target, unsigned int renderbuffer);
    void                 (rf_gl_calling_convention RenderbufferStorage)      (unsigned int target, unsigned int internalformat, int width, int height);
    void                 (rf_gl_calling_convention CompressedTexImage2D)     (unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, const void* data);
    void                 (rf_gl_calling_convention TexSubImage2D)            (unsigned int target, int level, int txoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, const void* pixels);
    void                 (rf_gl_calling_convention GenerateMipmap)           (unsigned int target);
    void                 (rf_gl_calling_convention ReadPixels)               (int x, int y, int width, int height, unsigned int format, unsigned int type, void* pixels);
    void                 (rf_gl_calling_convention GenFramebuffers)          (int n, unsigned int* framebuffers);
    void                 (rf_gl_calling_convention FramebufferTexture2D)     (unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
    void                 (rf_gl_calling_convention FramebufferRenderbuffer)  (unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
    unsigned int         (rf_gl_calling_convention CheckFramebufferStatus)   (unsigned int target);
    unsigned int         (rf_gl_calling_convention CreateShader)             (unsigned int type);
    void                 (rf_gl_calling_convention ShaderSource)             (unsigned int shader, int count, const char** string, const int* length);
    void                 (rf_gl_calling_convention CompileShader)            (unsigned int shader);
    void                 (rf_gl_calling_convention GetShaderiv)              (unsigned int shader, unsigned int pname, int* params);
    void                 (rf_gl_calling_convention GetShaderInfoLog)         (unsigned int shader, int bufSize, int* length, char* infoLog);
    unsigned int         (rf_gl_calling_convention CreateProgram)            ();
    void                 (rf_gl_calling_convention AttachShader)             (unsigned int program, unsigned int shader);
    void                 (rf_gl_calling_convention BindAttribLocation)       (unsigned int program, unsigned int index, const char* name);
    void                 (rf_gl_calling_convention LinkProgram)              (unsigned int program);
    void                 (rf_gl_calling_convention GetProgramiv)             (unsigned int program, unsigned int pname, int* params);
    void                 (rf_gl_calling_convention GetProgramInfoLog)        (unsigned int program, int bufSize, int* length, char* infoLog);
    void                 (rf_gl_calling_convention DeleteProgram)            (unsigned int program);
    int                  (rf_gl_calling_convention GetAttribLocation)        (unsigned int program, const char* name);
    int                  (rf_gl_calling_convention GetUniformLocation)       (unsigned int program, const char* name);
    void                 (rf_gl_calling_convention DetachShader)             (unsigned int program, unsigned int shader);
    void                 (rf_gl_calling_convention DeleteShader)             (unsigned int shader);
    void                 (rf_gl_calling_convention GetActiveUniform)         (unsigned int program, unsigned int index, int bufSize, int* length, int* size, unsigned int* type, char* name);
    void                 (rf_gl_calling_convention Uniform1f)                (int location, float v0);
    void                 (rf_gl_calling_convention Uniform1fv)               (int location, int count, const float* value);
    void                 (rf_gl_calling_convention Uniform2fv)               (int location, int count, const float* value);
    void                 (rf_gl_calling_convention Uniform3fv)               (int location, int count, const float* value);
    void                 (rf_gl_calling_convention Uniform4fv)               (int location, int count, const float* value);
    void                 (rf_gl_calling_convention Uniform1iv)               (int location, int count, const int* value);
    void                 (rf_gl_calling_convention Uniform2iv)               (int location, int count, const int* value);
    void                 (rf_gl_calling_convention Uniform3iv)               (int location, int count, const int* value);
    void                 (rf_gl_calling_convention Uniform4iv)               (int location, int count, const int* value);
    const unsigned char* (rf_gl_calling_convention GetString)                (unsigned int name);
    void                 (rf_gl_calling_convention GetFloatv)                (unsigned int pname, float* data);
    void                 (rf_gl_calling_convention DepthFunc)                (unsigned int func);
    void                 (rf_gl_calling_convention BlendFunc)                (unsigned int sfactor, unsigned int dfactor);
    void                 (rf_gl_calling_convention CullFace)                 (unsigned int mode);
    void                 (rf_gl_calling_convention FrontFace)                (unsigned int mode);
    const unsigned char* (rf_gl_calling_convention GetStringi)               (unsigned int name, unsigned int index);
    void                 (rf_gl_calling_convention GetTexImage)              (unsigned int target, int level, unsigned int format, unsigned int type, void* pixels); // OpenGL ES3 ONLY
    void                 (rf_gl_calling_convention ClearDepth)               (double depth);                                                                         // OpenGL 33  ONLY
    void                 (rf_gl_calling_convention ClearDepthf)              (float depth);                                                                          // OpenGL ES3 ONLY
    void                 (rf_gl_calling_convention GetIntegerv)              (unsigned int pname, int* data);                                                        // OpenGL 33  ONLY
    void                 (rf_gl_calling_convention PolygonMode)              (unsigned int face, unsigned int mode);                                                 // OpenGL 33  ONLY
} rf_opengl_procs;

#if defined(__cplusplus)
    #define rf_gl_proc_defn(ext, proc) (void*) rf_concat(ext, proc)
    #define rf_gl_proc_null(ext, proc) NULL
#else
    #define rf_gl_proc_defn(ext, proc) .proc = (void*) rf_concat(ext, proc)
    #define rf_gl_proc_null(ext, proc) .proc = NULL
#endif

#if defined(rayfork_graphics_backend_gl33)
    #define rf_gl_proc_gl33(ext, proc) rf_gl_proc_defn(ext, proc)
    #define rf_gl_proc_gles(ext, proc) rf_gl_proc_null(ext, proc)
#else
    #define rf_gl_proc_gl33(ext, proc) rf_gl_proc_null(ext, proc)
    #define rf_gl_proc_gles(ext, proc) rf_gl_proc_defn(ext, proc)
#endif

#define rf_opengl_procs_ext(ext) ((struct rf_gfx_backend_data*) &(rf_opengl_procs) {\
    rf_gl_proc_defn(ext, Viewport),\
    rf_gl_proc_defn(ext, BindTexture),\
    rf_gl_proc_defn(ext, TexParameteri),\
    rf_gl_proc_defn(ext, TexParameterf),\
    rf_gl_proc_defn(ext, TexParameteriv),\
    rf_gl_proc_defn(ext, BindFramebuffer),\
    rf_gl_proc_defn(ext, Enable),\
    rf_gl_proc_defn(ext, Disable),\
    rf_gl_proc_defn(ext, Scissor),\
    rf_gl_proc_defn(ext, DeleteTextures),\
    rf_gl_proc_defn(ext, DeleteRenderbuffers),\
    rf_gl_proc_defn(ext, DeleteFramebuffers),\
    rf_gl_proc_defn(ext, DeleteVertexArrays),\
    rf_gl_proc_defn(ext, DeleteBuffers),\
    rf_gl_proc_defn(ext, ClearColor),\
    rf_gl_proc_defn(ext, Clear),\
    rf_gl_proc_defn(ext, BindBuffer),\
    rf_gl_proc_defn(ext, BufferSubData),\
    rf_gl_proc_defn(ext, BindVertexArray),\
    rf_gl_proc_defn(ext, GenBuffers),\
    rf_gl_proc_defn(ext, BufferData),\
    rf_gl_proc_defn(ext, VertexAttribPointer),\
    rf_gl_proc_defn(ext, EnableVertexAttribArray),\
    rf_gl_proc_defn(ext, GenVertexArrays),\
    rf_gl_proc_defn(ext, VertexAttrib3f),\
    rf_gl_proc_defn(ext, DisableVertexAttribArray),\
    rf_gl_proc_defn(ext, VertexAttrib4f),\
    rf_gl_proc_defn(ext, VertexAttrib2f),\
    rf_gl_proc_defn(ext, UseProgram),\
    rf_gl_proc_defn(ext, Uniform4f),\
    rf_gl_proc_defn(ext, ActiveTexture),\
    rf_gl_proc_defn(ext, Uniform1i),\
    rf_gl_proc_defn(ext, UniformMatrix4fv),\
    rf_gl_proc_defn(ext, DrawElements),\
    rf_gl_proc_defn(ext, DrawArrays),\
    rf_gl_proc_defn(ext, PixelStorei),\
    rf_gl_proc_defn(ext, GenTextures),\
    rf_gl_proc_defn(ext, TexImage2D),\
    rf_gl_proc_defn(ext, GenRenderbuffers),\
    rf_gl_proc_defn(ext, BindRenderbuffer),\
    rf_gl_proc_defn(ext, RenderbufferStorage),\
    rf_gl_proc_defn(ext, CompressedTexImage2D),\
    rf_gl_proc_defn(ext, TexSubImage2D),\
    rf_gl_proc_defn(ext, GenerateMipmap),\
    rf_gl_proc_defn(ext, ReadPixels),\
    rf_gl_proc_defn(ext, GenFramebuffers),\
    rf_gl_proc_defn(ext, FramebufferTexture2D),\
    rf_gl_proc_defn(ext, FramebufferRenderbuffer),\
    rf_gl_proc_defn(ext, CheckFramebufferStatus),\
    rf_gl_proc_defn(ext, CreateShader),\
    rf_gl_proc_defn(ext, ShaderSource),\
    rf_gl_proc_defn(ext, CompileShader),\
    rf_gl_proc_defn(ext, GetShaderiv),\
    rf_gl_proc_defn(ext, GetShaderInfoLog),\
    rf_gl_proc_defn(ext, CreateProgram),\
    rf_gl_proc_defn(ext, AttachShader),\
    rf_gl_proc_defn(ext, BindAttribLocation),\
    rf_gl_proc_defn(ext, LinkProgram),\
    rf_gl_proc_defn(ext, GetProgramiv),\
    rf_gl_proc_defn(ext, GetProgramInfoLog),\
    rf_gl_proc_defn(ext, DeleteProgram),\
    rf_gl_proc_defn(ext, GetAttribLocation),\
    rf_gl_proc_defn(ext, GetUniformLocation),\
    rf_gl_proc_defn(ext, DetachShader),\
    rf_gl_proc_defn(ext, DeleteShader),\
    rf_gl_proc_defn(ext, GetActiveUniform),\
    rf_gl_proc_defn(ext, Uniform1f),\
    rf_gl_proc_defn(ext, Uniform1fv),\
    rf_gl_proc_defn(ext, Uniform2fv),\
    rf_gl_proc_defn(ext, Uniform3fv),\
    rf_gl_proc_defn(ext, Uniform4fv),\
    rf_gl_proc_defn(ext, Uniform1iv),\
    rf_gl_proc_defn(ext, Uniform2iv),\
    rf_gl_proc_defn(ext, Uniform3iv),\
    rf_gl_proc_defn(ext, Uniform4iv),\
    rf_gl_proc_defn(ext, GetString),\
    rf_gl_proc_defn(ext, GetFloatv),\
    rf_gl_proc_defn(ext, DepthFunc),\
    rf_gl_proc_defn(ext, BlendFunc),\
    rf_gl_proc_defn(ext, CullFace),\
    rf_gl_proc_defn(ext, FrontFace),\
    rf_gl_proc_defn(ext, GetStringi),\
    rf_gl_proc_gl33(ext, GetTexImage), /* OpenGL 33  ONLY */ \
    rf_gl_proc_gl33(ext, ClearDepth),  /* OpenGL 33  ONLY */ \
    rf_gl_proc_gles(ext, ClearDepthf), /* OpenGL ES3 ONLY */ \
    rf_gl_proc_gl33(ext, GetIntegerv), /* OpenGL 33  ONLY */ \
    rf_gl_proc_gl33(ext, PolygonMode), /* OpenGL 33  ONLY */ \
})

#endif // RAYFORK_INTERNAL_GFX_BACKEND_OPENGL3_GLPROCS_H