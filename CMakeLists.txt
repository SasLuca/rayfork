cmake_minimum_required(VERSION 3.15)
project(rayfork)

add_subdirectory(rayfork-examples)

include_directories(source)
include_directories(source/include)
include_directories(generated)

add_library(rayfork-gen generated/rayfork_gen.h)
set_target_properties(rayfork-gen PROPERTIES LINKER_LANGUAGE C)
target_compile_definitions(rayfork-gen PUBLIC RAYFORK_IMPLEMENTATION)

add_library(rayfork source/rayfork.c source/rayfork_gfx_backend_gl.c)
target_compile_features(rayfork PUBLIC c_std_99)
target_include_directories(rayfork PRIVATE source/libs)

add_library(rayfork-audio source/rayfork_audio.c source/rayfork_audio.h)
target_compile_features(rayfork-audio PUBLIC c_std_99)
target_include_directories(rayfork-audio PRIVATE source/libs)