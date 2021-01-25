cmake_minimum_required(VERSION 3.1)

add_library(platform-common INTERFACE)
target_include_directories(platform-common INTERFACE RAYFORK_INCLUDE include)