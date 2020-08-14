cmake_minimum_required(VERSION 3.1)
project(rayfork LANGUAGES C)

add_library(rayfork)
target_compile_features(rayfork PUBLIC c_std_99)
target_sources(rayfork PRIVATE "source/rayfork.c")
target_include_directories(rayfork PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(rayfork PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source" "${CMAKE_CURRENT_SOURCE_DIR}/libs")

# rayfork options --------------------------------------------------------------------

# Setup assertions
if (RAYFORK_ENABLE_ASSERTIONS)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_ASSERTIONS)
endif()

# Enable audio
if (RAYFORK_ENABLE_AUDIO)
    target_compile_definitions(rayfork PUBLIC RAYFORK_ENABLE_AUDIO)
endif()

# For rayfork devs -------------------------------------------------------------------------------

# Include test optional suite
if(RAYFORK_TESTS)
    set(rayfork_path "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_subdirectory(tests)
endif()