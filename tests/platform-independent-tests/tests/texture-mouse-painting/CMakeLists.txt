cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(texture-mouse-painting
        main.c
        game.c
        ../../../libs/glad/glad.c)

target_link_libraries(texture-mouse-painting PRIVATE rayfork)