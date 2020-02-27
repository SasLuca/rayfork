cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(first_person_maze main.c ../dependencies/glad/glad.c)

add_compile_definitions(GLFW_INCLUDE_NONE)

target_link_libraries(first_person_maze PRIVATE glfw)
target_link_libraries(first_person_maze PRIVATE rayfork)