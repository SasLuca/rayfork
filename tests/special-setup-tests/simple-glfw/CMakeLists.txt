cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_compile_definitions(GLFW_INCLUDE_NONE)

add_executable(simple-glfw main.c ../../platform-independent-tests/platform-layers/sokol/glad/glad.c)
target_link_libraries(simple-glfw PRIVATE glfw rayfork)