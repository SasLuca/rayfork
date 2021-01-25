cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_compile_definitions(GLFW_INCLUDE_NONE)

add_executable(first-person-maze main.c ../../../libs/glad/glad.c)
target_link_libraries(first-person-maze PRIVATE glfw rayfork)