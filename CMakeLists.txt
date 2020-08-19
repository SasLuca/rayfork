cmake_minimum_required(VERSION 3.1)
project(rayfork LANGUAGES C)

add_library(rayfork)
target_compile_features(rayfork PUBLIC c_std_99)
target_sources(rayfork PRIVATE "src/rayfork.c")
target_include_directories(rayfork PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(rayfork PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/libs")

# rayfork options --------------------------------------------------------------------

# Setup assertions
if (RAYFORK_ENABLE_ASSERTIONS)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_ASSERTIONS)
endif()

# Enable audio
if (RAYFORK_ENABLE_AUDIO)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_AUDIO)
endif()

# rayfork test -------------------------------------------------------------------------------
if (EXISTS tests)
    set(rayfork_path "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_subdirectory(tests)
endif()