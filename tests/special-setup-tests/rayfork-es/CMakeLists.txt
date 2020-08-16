cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

include_directories(../../../generated/rayfork-es)
add_executable(rayfork-es main.c)