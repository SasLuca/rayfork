cmake_minimum_required(VERSION 3.1)
project(sokol-app)

add_library(sokol-app INTERFACE)
target_include_directories(sokol-app INTERFACE include)