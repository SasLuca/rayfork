cmake_minimum_required(VERSION 3.1)
project(rayfork LANGUAGES C)

# Define library ---------------------------------------------------------------------

add_library(rayfork)

# Set C99 for the library
target_compile_features(rayfork PUBLIC c_std_99)

# Add the source files to the executable
target_sources(rayfork PRIVATE "rayfork.c")

# Add rayfork.h to the include dirs
target_include_directories(rayfork PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# rayfork options --------------------------------------------------------------------

# Setup assertions
if (RAYFORK_ENABLE_ASSERTIONS)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_ASSERTIONS)
endif()

# Enable audio
if (RAYFORK_ENABLE_AUDIO)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_AUDIO)
endif()

# Misc -------------------------------------------------------------------------------

# Include test optional suite
if(RAYFORK_TESTS)
    set(rayfork_path "${CMAKE_CURRENT_SOURCE_DIR}/")
    add_subdirectory(tests)
endif()