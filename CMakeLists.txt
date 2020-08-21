cmake_minimum_required(VERSION 3.1)
project(rayfork LANGUAGES C)

add_library(rayfork-dev)
target_compile_features(rayfork-dev PUBLIC c_std_99)
target_sources(rayfork-dev PRIVATE "src/rayfork.c")
target_include_directories(rayfork-dev PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(rayfork-dev PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/libs")

if (RAYFORK_TEST_AMALGAMATED)
    add_library(rayfork-amalgamated)
    target_compile_features(rayfork-amalgamated PUBLIC c_std_99)
    target_sources(rayfork-amalgamated PRIVATE "rayfork-amalgamated/rayfork.c")
    target_include_directories(rayfork-amalgamated PUBLIC "rayfork-amalgamated")
    set(rayfork rayfork-amalgamated)
else()
    set(rayfork rayfork-dev)
endif()

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