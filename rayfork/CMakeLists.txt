cmake_minimum_required(VERSION 3.1)

set(RAYFORK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rayfork/include)

option(RAYFORK_GRAPHICS_BACKEND_GL_33 "Use OpenGL 33 Backend" ON)
option(RAYFORK_GRAPHICS_BACKEND_GL_ES3 "Use OpenGL ES3 Backend" OFF)

include_directories(include)

add_library(rayfork src/rayfork.c)

target_compile_features(rayfork PUBLIC c_std_99)
target_include_directories(rayfork INTERFACE include)
target_include_directories(rayfork PRIVATE libs)

if (RAYFORK_GRAPHICS_BACKEND_GL_33)
    target_compile_definitions(rayfork PRIVATE RAYFORK_GRAPHICS_BACKEND_GL_33)
endif()

if (RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    target_compile_definitions(rayfork PRIVATE RAYFORK_GRAPHICS_BACKEND_GL_ES3)
endif()