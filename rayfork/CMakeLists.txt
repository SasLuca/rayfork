cmake_minimum_required(VERSION 3.1)

option(RAYFORK_GRAPHICS_BACKEND_GL_33 "Use OpenGL 33 Backend" ON)
option(RAYFORK_GRAPHICS_BACKEND_GL_ES3 "Use OpenGL ES3 Backend" OFF)
option(RAYFORK_GRAPHICS_BACKEND_SOKOL "Use Sokol Gfx Backend" OFF)

include_directories(include)

add_library(rayfork rayfork.c)
target_compile_features(rayfork PUBLIC c_std_99)
target_include_directories(rayfork PRIVATE libs)
if (RAYFORK_GRAPHICS_BACKEND_GL_33)
    target_compile_definitions(rayfork PUBLIC RAYFORK_GRAPHICS_BACKEND_GL_33)
endif()
if (RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    target_compile_definitions(rayfork PUBLIC RAYFORK_GRAPHICS_BACKEND_GL_ES3)
endif()
if (RAYFORK_GRAPHICS_BACKEND_SOKOL)
    target_compile_definitions(rayfork PUBLIC RAYFORK_GRAPHICS_BACKEND_SOKOL)
endif()

add_library(rayfork-audio-old rayfork_audio_old.c)
target_compile_features(rayfork-audio-old PUBLIC c_std_99)
target_include_directories(rayfork-audio-old PRIVATE libs)

add_library(rayfork-audio rayfork_audio.c rayfork_audio.h)
target_compile_features(rayfork-audio PUBLIC c_std_99)
target_include_directories(rayfork-audio PRIVATE libs)