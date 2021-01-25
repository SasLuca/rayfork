cmake_minimum_required(VERSION 3.1)
project(catch2)

add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE include)