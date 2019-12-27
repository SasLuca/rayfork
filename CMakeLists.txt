cmake_minimum_required(VERSION 3.15)
project(rayfork_examples C)

set(CMAKE_C_STANDARD 99)

include_directories(dependencies)
include_directories(rayfork)
include_directories(examples/geometric_shapes)
include_directories(examples/geometric_shapes/dependencies)

add_executable(basic_shapes
        examples/basic_shapes/main.c
        examples/basic_shapes/game.c
        examples/basic_shapes/dependencies/glad/glad.c)

add_executable(collision_area
        examples/collision_area/main.c
        examples/collision_area/game.c
        examples/collision_area/dependencies/glad/glad.c)

add_executable(geometric_shapes
        examples/geometric_shapes/main.c
        examples/geometric_shapes/game.c
        examples/geometric_shapes/dependencies/glad/glad.c)

add_executable(raw_audio_stream
        examples/raw_audio_stream/main.c
        examples/raw_audio_stream/game.c
        examples/raw_audio_stream/dependencies/glad/glad.c)

add_executable(hot_code_reloading
        examples/hot_code_reloading/win32_main.c)

add_library(hot_code_reloading_dynamic SHARED
        examples/hot_code_reloading/game.c
        examples/hot_code_reloading/dependencies/glad/glad.c)