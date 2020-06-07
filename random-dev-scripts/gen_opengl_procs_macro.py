code = """void (*glViewport)(int x, int y, int width, int height);
    void (*glBindTexture)(unsigned int target, unsigned int texture);
    void (*glTexParameteri)(unsigned int target, unsigned int pname, int param);
    void (*glTexParameterf)(unsigned int target, unsigned int pname, float param);
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
    void (*glGetTexImage)(unsigned int target, int level, unsigned int format, unsigned int type, void* pixels);
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
    void (*glClearDepth)(double depth);
    void (*glClearDepthf)(float depth);
    void (*glGetIntegerv)(unsigned int pname, int* data); 
    const unsigned char* (*glGetStringi)(unsigned int name, unsigned int index);
"""

def str_inbetween(s, start, end):
    return s[s.find(start) + len(start):s.rfind(end)]


lines = [it.strip() for it in code.splitlines()]
result = ""

for l in lines:
    l = l.replace(';', '')  # Remove ;
    procname = str_inbetween(l, " (*gl", ")(")
    proc_type = l.replace("gl" + procname, "")
    result += f"(({proc_type})RF_OPENGL_PROC_EXT(ext, {procname})),\\\n"

f = open("./test.txt", "w")
f.write(result)