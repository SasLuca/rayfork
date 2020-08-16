cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(texture-to-image
        main.c
        game.c
        ../../platform-layers/sokol/glad/glad.c)

target_link_libraries(texture-to-image PRIVATE rayfork)