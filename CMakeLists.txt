cmake_minimum_required(VERSION 3.15)
project(rayfork C)

set(CMAKE_C_STANDARD 99)

if (EXAMPLES)
    include_directories(examples/dependencies/)
    include_directories(examples/dependencies/sokol)
    include_directories(examples/dependencies/glad)

    include_directories(rayfork)

    add_subdirectory(examples/basic_shapes)
    add_subdirectory(examples/collision_area)
    add_subdirectory(examples/geometric_shapes)
    add_subdirectory(examples/raw_audio_stream)
    add_subdirectory(examples/hot_code_reloading)
endif()

add_library(rayfork INTERFACE)

target_include_directories(rayfork INTERFACE rayfork/)

target_include_directories(rayfork INTERFACE dependencies)
target_include_directories(rayfork INTERFACE dependencies/cgltf)
target_include_directories(rayfork INTERFACE dependencies/dr_libs)
target_include_directories(rayfork INTERFACE dependencies/jar)
target_include_directories(rayfork INTERFACE dependencies/miniaudio)
target_include_directories(rayfork INTERFACE dependencies/par)
target_include_directories(rayfork INTERFACE dependencies/stb)
target_include_directories(rayfork INTERFACE dependencies/tinyobjloader-c)