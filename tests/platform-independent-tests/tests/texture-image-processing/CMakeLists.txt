cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(texture-image-processing
        main.c
        game.c
        ../../platform-layers/sokol/glad/glad.c)

target_link_libraries(texture-image-processing PRIVATE rayfork)