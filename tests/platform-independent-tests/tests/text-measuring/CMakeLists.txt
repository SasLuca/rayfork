cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_compile_definitions(GLFW_INCLUDE_NONE)

add_executable(text-measuring main.c ../../../libs/glad/glad.c)
target_link_libraries(text-measuring PRIVATE glfw rayfork)