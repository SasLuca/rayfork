cmake_minimum_required(VERSION 3.15)
project(rayfork_examples C)

set(CMAKE_C_STANDARD 99)

include_directories(dependencies)
include_directories(rayfork)
include_directories(examples/geometric_shapes)
include_directories(examples/geometric_shapes/dependencies)

add_executable(geometric_shapes
        examples/geometric_shapes/main.c
        examples/geometric_shapes/game.c
        examples/geometric_shapes/rayfork_renderer.c
        examples/geometric_shapes/dependencies/glad/glad.c)