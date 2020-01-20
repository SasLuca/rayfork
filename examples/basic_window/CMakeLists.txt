cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(basic_window
        main.c
        game.c
        ../dependencies/glad/glad.c)