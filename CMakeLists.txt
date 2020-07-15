cmake_minimum_required(VERSION 3.1)
project(rayfork LANGUAGES C)

# Define library
add_library(rayfork)

# Set C99 for the library
target_compile_features(rayfork PUBLIC c_std_99)

# Add the source files to the executable
target_sources(rayfork PRIVATE "rayfork.c")

# Include all the files from the source directory
target_include_directories(rayfork PUBLIC "source")

# Setup assertions
if (RAYFORK_ENABLE_ASSERTIONS)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_ASSERTIONS)
endif()

# Include test optional suite
if(RAYFORK_TESTS)
    add_subdirectory(tests)
endif()