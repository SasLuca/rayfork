cmake_minimum_required(VERSION 3.15)
project(rayfork)

add_subdirectory(rayfork-examples)

include_directories(source)
include_directories(source/gfx)

add_library(rayfork source/gfx/rayfork.c)
target_compile_features(rayfork PUBLIC c_std_99)
target_include_directories(rayfork PUBLIC source/)
target_include_directories(rayfork PUBLIC source/gfx)
target_include_directories(rayfork PUBLIC source/libs)