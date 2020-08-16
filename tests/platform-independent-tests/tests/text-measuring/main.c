#include "include/rayfork.h"
#include "glad.h"
#include "GLFW/glfw3.h"

#define SCREEN_WIDTH (800*2)
#define SCREEN_HEIGHT (450*2)
#define MAX_TEXT_LEN (1024)

rf_context rf_ctx;
rf_default_render_batch rf_mem;

char text[MAX_TEXT_LEN];
int  text_len;

static void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    if (text_len == MAX_TEXT_LEN) return;

    text[text_len] = (char) codepoint;
    text_len++;
}

int main()
{
    //GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rayfork example - text measuring", NULL, NULL);
    glfwSetCharCallback(window, character_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL();

    //Rayfork and game init
    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR);

    while (!glfwWindowShouldClose(window))
    {
        rf_begin();
        {
            rf_clear(RF_RAYWHITE);

            rf_rec rec = { 0, 0, SCREEN_WIDTH / 16, SCREEN_HEIGHT / 2 };
            rf_draw_rectangle_rec(rec, RF_RED);

            rf_sizef text_size = rf_measure_text_rec(rf_get_default_font(), text, text_len, rec, 20, 1, true);
            rf_draw_rectangle_rec((rf_rec) { 0, 0, text_size.width, text_size.height }, RF_BLUE);

            rf_draw_text_rec(rf_get_default_font(), text, text_len, (rf_rec) { 0, 0, rec.width, rec.height }, 20, 1, RF_WORD_WRAP, RF_WHITE);
        }
        rf_end();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}