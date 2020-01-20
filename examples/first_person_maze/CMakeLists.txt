cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(first_person_maze main.c ../dependencies/glad/glad.c)

target_link_libraries(first_person_maze PRIVATE glfw)