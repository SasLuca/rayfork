cmake_minimum_required(VERSION 3.15)
project(rayfork C)

if (RAYFORK_EXAMPLES)
    include_directories(rayfork)
    add_subdirectory(rayfork-examples)
endif()

add_subdirectory(rayfork)