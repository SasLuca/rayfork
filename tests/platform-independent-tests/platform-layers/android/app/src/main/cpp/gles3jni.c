#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl32.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define RAYFORK_IMPLEMENTATION
#include "../../../../../../../../generated/single-header/rayfork.h"

//region test code

const char* gVertexShader =
        "attribute vec4 vPosition;\n"
        "void main() {\n"
        "  gl_Position = vPosition;\n"
        "}\n";

const char* gFragmentShader =
        "precision mediump float;\n"
        "void main() {\n"
        "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";

GLuint loadShader(GLenum shaderType, const char* pSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }

    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) { return 0; }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) { return 0; }

    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;

bool setupGraphics(int w, int h)
{
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) { return false; }

    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");

    glViewport(0, 0, w, h);

    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

void renderFrame()
{
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) { grey = 0.0f; }

    glClearColor(grey, grey, grey, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gProgram);

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    glEnableVertexAttribArray(gvPositionHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

//endregion

//region rayfork
rf_context      global_rayfork_context;
rf_render_batch global_render_batch;
int             screen_width;
int             screen_height;
//endregion

volatile int err = 0;

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_init(JNIEnv * env, jobject obj,  jobject assetManager, jfloat density, jint width, jint height, jstring internal_storage_path)
{
    rf_init(&global_rayfork_context, width, height, RF_DEFAULT_OPENGL_PROCS);

    global_render_batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
    rf_set_active_render_batch(&global_render_batch);

    screen_width = width;
    screen_height = height;
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_onResize(JNIEnv * env, jobject obj,  jint width, jint height)
{
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_processPointerInput(JNIEnv * env, jobject obj,  jint event, jint pointer_index, jint x, jint y)
{
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_step(JNIEnv * env, jobject obj)
{
    rf_begin();
    {
        rf_clear(RF_WHITE);

        rf_draw_text("some basic shapes available on raylib", 20, 20, 20, RF_DARKGRAY);

        rf_draw_circle(screen_width / 4, 120, 35, RF_DARKBLUE);

        rf_draw_rectangle(screen_width / 4 * 2 - 60, 100, 120, 60, RF_RED);

        rf_draw_rectangle_outline((rf_rec) { screen_width / 4 * 2 - 40, 320, 80, 60 }, 0, RF_ORANGE);  // NOTE: Uses QUADS internally, not lines

        rf_draw_rectangle_gradient_h(screen_width / 4 * 2 - 90, 170, 180, 130, RF_MAROON, RF_GOLD);

        rf_draw_triangle((rf_vec2) { screen_width / 4 * 3, 80 },
                         (rf_vec2) { screen_width / 4 * 3 - 60, 150 },
                         (rf_vec2) { screen_width / 4 * 3 + 60, 150 }, RF_VIOLET);

        rf_draw_poly((rf_vec2) { screen_width / 4.0f * 3.0f, 320 }, 6, 80, 0, RF_BROWN);

        rf_draw_circle_gradient(screen_width / 4, 220, 60, RF_GREEN, RF_SKYBLUE);

        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        rf_draw_line(18, 42, screen_width - 18, 42, RF_BLACK);

        rf_draw_circle_lines(screen_width / 4, 340, 80, RF_DARKBLUE);

        rf_draw_triangle_lines(
                (rf_vec2) { screen_width / 4 * 3, 160 },
                (rf_vec2) { screen_width / 4 * 3 - 20, 230 },
                (rf_vec2) { screen_width / 4 * 3 + 20, 230 },
                RF_DARKBLUE);
    }
    rf_end();
}
