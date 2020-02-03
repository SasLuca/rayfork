cmake_minimum_required(VERSION 3.15)
project(rayfork C)

set(CMAKE_C_STANDARD 99)

if (RAYFORK_EXAMPLES)
    include_directories(dependencies)
    include_directories(dependencies/cgltf)
    include_directories(dependencies/dr_libs)
    include_directories(dependencies/jar)
    include_directories(dependencies/miniaudio)
    include_directories(dependencies/par)
    include_directories(dependencies/stb)
    include_directories(dependencies/tinyobjloader-c)

    include_directories(examples/dependencies/)
    include_directories(examples/dependencies/sokol)
    include_directories(examples/dependencies/glad)

    include_directories(rayfork)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(examples/dependencies/glfw EXCLUDE_FROM_ALL)

    add_subdirectory(examples/basic_window)
    add_subdirectory(examples/basic_shapes)
    add_subdirectory(examples/input_keys)
    add_subdirectory(examples/collision_area)
    add_subdirectory(examples/geometric_shapes)
    add_subdirectory(examples/raw_audio_stream)
    add_subdirectory(examples/hot_code_reloading)
    add_subdirectory(examples/bunnymark)
    add_subdirectory(examples/first_person_maze)
    add_subdirectory(examples/compile)
endif()

add_library(rayfork INTERFACE)

target_include_directories(rayfork INTERFACE dependencies)
target_include_directories(rayfork INTERFACE dependencies/cgltf)
target_include_directories(rayfork INTERFACE dependencies/dr_libs)
target_include_directories(rayfork INTERFACE dependencies/jar)
target_include_directories(rayfork INTERFACE dependencies/miniaudio)
target_include_directories(rayfork INTERFACE dependencies/par)
target_include_directories(rayfork INTERFACE dependencies/stb)
target_include_directories(rayfork INTERFACE dependencies/tinyobjloader-c)

target_include_directories(rayfork INTERFACE rayfork/)