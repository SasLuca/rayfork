cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(test_compilation
        main.c
        ../dependencies/glad/glad.c)