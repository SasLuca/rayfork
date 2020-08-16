cmake_minimum_required(VERSION 3.1)
set(CMAKE_C_STANDARD 99)

add_library(common-platform-interface INTERFACE)
target_include_directories(common-platform-interface INTERFACE ".")