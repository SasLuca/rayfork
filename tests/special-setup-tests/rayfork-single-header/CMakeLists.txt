cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

include_directories(../../../generated/single-header)
add_executable(rayfork-single-header main.c ../../libs/glad/glad.c)