#include "platform.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

input_t input_state;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (action == GLFW_PRESS && input_state.keys[key] == KEY_DEFAULT_STATE)
    {
        input_state.keys[key] = KEY_PRESSED_DOWN;
        input_state.any_key_pressed = true;
    }

    if (action == GLFW_RELEASE)
    {
        input_state.keys[key] = KEY_RELEASE;
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    input_state.mouse_scroll_y = yoffset;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT  && input_state.left_mouse_btn  == BTN_DEFAULT_STATE) input_state.left_mouse_btn  = BTN_PRESSED_DOWN;
        if (button == GLFW_MOUSE_BUTTON_RIGHT && input_state.right_mouse_btn == BTN_DEFAULT_STATE) input_state.right_mouse_btn = BTN_PRESSED_DOWN;
    }

    if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT ) input_state.left_mouse_btn  = BTN_RELEASE;
        if (button == GLFW_MOUSE_BUTTON_RIGHT) input_state.right_mouse_btn = BTN_RELEASE;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    input_state.mouse_x = (int) xpos;
    input_state.mouse_y = (int) ypos;
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    game_window_resize(width, height);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, window_title, NULL, NULL);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // We only need this for the fps test

    gladLoadGL();
    glfwSwapInterval(1);

    game_init(RF_DEFAULT_GFX_BACKEND_INIT_DATA);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        game_update(&input_state);

        for (int i = 0; i < sizeof(input_state.keys) / sizeof(btn_t); i++)
        {
            if (input_state.keys[i] == KEY_RELEASE)
            {
                input_state.keys[i] = KEY_DEFAULT_STATE;
            }

            if (input_state.keys[i] == KEY_PRESSED_DOWN)
            {
                input_state.keys[i] = KEY_HOLD_DOWN;
            }
        }

        input_state.any_key_pressed = false;

        if (input_state.mouse_scroll_y != 0) input_state.mouse_scroll_y = 0;
        if (input_state.left_mouse_btn  == BTN_RELEASE)      input_state.left_mouse_btn  = BTN_DEFAULT_STATE;
        if (input_state.left_mouse_btn  == BTN_PRESSED_DOWN) input_state.left_mouse_btn  = BTN_HOLD_DOWN;
        if (input_state.right_mouse_btn == BTN_RELEASE)      input_state.right_mouse_btn = BTN_DEFAULT_STATE;
        if (input_state.right_mouse_btn == BTN_PRESSED_DOWN) input_state.right_mouse_btn = BTN_HOLD_DOWN;
    }
}