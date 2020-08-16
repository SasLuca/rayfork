cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

include_directories(../../../generated/single-header-and-source)
add_executable(rayfork-single-header-and-source main.c ../../platform-independent-tests/platform-layers/sokol/glad/glad.c ../../../generated/single-header-and-source/rayfork.c)