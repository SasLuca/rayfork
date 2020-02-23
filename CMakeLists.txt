cmake_minimum_required(VERSION 3.15)
project(rayfork C)

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

set(RAYFORK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rayfork/include)

include_directories(rayfork/include)

add_library(rayfork
        rayfork/src/rayfork.c
        rayfork/src/rayfork_gfx_backend_gl.c)

target_compile_features(rayfork PUBLIC c_std_99)
target_include_directories(rayfork INTERFACE rayfork/include)
target_include_directories(rayfork PRIVATE rayfork/libs)