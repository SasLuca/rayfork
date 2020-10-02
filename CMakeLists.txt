cmake_minimum_required(VERSION 3.1)
project(rayfork LANGUAGES C CXX)

add_library(rayfork-dev)
target_sources(rayfork-dev PRIVATE "source/rayfork.c")
target_compile_features(rayfork-dev PUBLIC c_std_99)
target_include_directories(rayfork-dev PUBLIC "source" "source/core" "source/gfx" "source/audio" "source/internal" "source/math" "source/unicode" "source/libs" "source/audio")
# target_compile_definitions(rayfork-dev PUBLIC RAYFORK_GRAPHICS_BACKEND_DIRECTX)

if (RAYFORK_TEST_AMALGAMATED)
    add_library(amalgamated)
    target_compile_features(amalgamated PUBLIC c_std_99)
    target_sources(amalgamated PRIVATE "amalgamated/rayfork.c")
    target_include_directories(amalgamated PUBLIC "amalgamated")

    set(rayfork_path "${CMAKE_CURRENT_SOURCE_DIR}/amalgamated")
    set(rayfork amalgamated)
else()
    set(rayfork_path "${CMAKE_CURRENT_SOURCE_DIR}/rayfork-dev")
    set(rayfork rayfork-dev)
endif()

add_subdirectory(tests)