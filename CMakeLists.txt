cmake_minimum_required(VERSION 3.15)
project(rayfork)

if (RAYFORK_EXAMPLES)
    include_directories(rayfork)
    include_directories(rayfork-cpp)
    add_subdirectory(rayfork-examples)
endif()

add_subdirectory(rayfork)
add_subdirectory(rayfork-cpp)