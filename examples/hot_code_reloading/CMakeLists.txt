cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(hot_code_reloading win32_main.c)

add_library(hot_code_reloading_dynamic SHARED
        game.c
        ../dependencies/glad/glad.c)