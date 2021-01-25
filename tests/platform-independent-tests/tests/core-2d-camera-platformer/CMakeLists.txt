cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(core-2d-camera-platformer
        main.c
        game.c
        ../../../libs/glad/glad.c)

target_link_libraries(core-2d-camera-platformer PRIVATE rayfork)