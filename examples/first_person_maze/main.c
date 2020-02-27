//In this file we only initialise the window using sokol_app

#include "rayfork.h"
#include "glad.h"
#include "GLFW/glfw3.h"

#define SCREEN_WIDTH (800*2)
#define SCREEN_HEIGHT (450*2)

rf_input_state_for_update_camera input;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        switch (key)
        {
            case GLFW_KEY_LEFT_ALT:     input.is_camera_alt_control_key_down    = action == GLFW_PRESS; break;
            case GLFW_KEY_LEFT_CONTROL: input.is_camera_smooth_zoom_control_key = action == GLFW_PRESS; break;
            case GLFW_KEY_W: input.direction_keys[0] = action == GLFW_PRESS; break;
            case GLFW_KEY_S: input.direction_keys[1] = action == GLFW_PRESS; break;
            case GLFW_KEY_D: input.direction_keys[2] = action == GLFW_PRESS; break;
            case GLFW_KEY_A: input.direction_keys[3] = action == GLFW_PRESS; break;
            case GLFW_KEY_E: input.direction_keys[4] = action == GLFW_PRESS; break;
            case GLFW_KEY_Q: input.direction_keys[5] = action == GLFW_PRESS; break;
            default: break;
        }
    }
}

int main()
{
    //GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rayfork [core] example - first person maze", NULL, NULL);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    //Rayfork and game init
    //Initialise rayfork and load the default font
    rf_context rf_ctx;
    rf_renderer_memory_buffers rf_mem;
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR);
    rf_set_target_fps(60);

    //Load stuff
    rf_image imMap        = rf_load_image_from_file("../../../examples/assets/cubicmap.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO); // Load cubicmap image (RAM)
    rf_texture2d cubicmap = rf_load_texture_from_image(imMap); // Convert image to texture to display (VRAM)
    rf_mesh mesh          = rf_gen_mesh_cubicmap(imMap, (rf_vec3){1.0f, 1.0f, 1.0f });
    rf_model model        = rf_load_model_from_mesh(mesh);

    rf_vec2 ball_position = {(float) SCREEN_WIDTH / 2, (float) SCREEN_HEIGHT / 2 };
    rf_camera3d camera = { { 0.2f, 0.4f, 0.2f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };

    // NOTE: By default each cube is mapped to one part of texture atlas
    rf_texture2d texture = rf_load_texture("../../../examples/assets/cubicmap_atlas.png"); // Load map texture
    model.materials[0].maps[RF_MAP_DIFFUSE].texture = texture; // Set map diffuse texture

    rf_color* mapPixels = rf_get_image_pixel_data(imMap);
    rf_unload_image(imMap); // Unload image from RAM

    rf_vec3 mapPosition = {-16.0f, 0.0f, -8.0f };  // Set model position
    rf_vec3 playerPosition = camera.position;

    rf_set_camera_mode(camera, RF_CAMERA_FIRST_PERSON);

    while (!glfwWindowShouldClose(window))
    {
        //Update
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            input.mouse_position.x = (int) xpos;
            input.mouse_position.y = (int) ypos;
        }

        rf_vec3 oldCamPos = camera.position; // Store old camera position

        rf_update_camera3d(&camera, input); // Update camera

        // Check player collision (we simplify to 2D collision detection)
        rf_vec2 playerPos = {camera.position.x, camera.position.z };
        float playerRadius = 0.1f;  // Collision radius (player is modelled as a cilinder for collision)

        int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
        int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

        // Out-of-limits security check
        if (playerCellX < 0) playerCellX = 0;
        else if (playerCellX >= cubicmap.width) playerCellX = cubicmap.width - 1;

        if (playerCellY < 0) playerCellY = 0;
        else if (playerCellY >= cubicmap.height) playerCellY = cubicmap.height - 1;

        // Check map collisions using image data and player position
        // TODO: Improvement: Just check player surrounding cells for collision
        for (int y = 0; y < cubicmap.height; y++)
        {
            for (int x = 0; x < cubicmap.width; x++)
            {
                if ((mapPixels[y*cubicmap.width + x].r == 255) && // Collision: white pixel, only check R channel
                    (rf_check_collision_circle_rec(playerPos, playerRadius,(rf_rec){mapPosition.x - 0.5f + x * 1.0f, mapPosition.z - 0.5f + y * 1.0f, 1.0f, 1.0f })))
                {
                    // Collision detected, reset camera position
                    camera.position = oldCamPos;
                }
            }
        }

        //Render
        rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_begin_3d(camera);

        rf_draw_model(model, mapPosition, 1.0f, RF_WHITE); // Draw maze map
        //DrawCubeV(playerPosition, (Vector3){ 0.2f, 0.4f, 0.2f }, RED);  // Draw player

        rf_end_3d();

        rf_draw_texture(cubicmap, (rf_vec2) { SCREEN_WIDTH - cubicmap.width * 4 - 20, 20 }, 0.0f, 4.0f, RF_WHITE);
        rf_draw_rectangle_outline((rf_rec ) { SCREEN_WIDTH - cubicmap.width * 4 - 20, 20, cubicmap.width * 4 }, cubicmap.height * 4, RF_GREEN);

        // Draw player position radar
        rf_draw_rectangle(SCREEN_WIDTH - cubicmap.width * 4 - 20 + playerCellX * 4, 20 + playerCellY * 4, 4, 4, RF_RED);

        rf_draw_fps(10, 10);

        rf_end();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}