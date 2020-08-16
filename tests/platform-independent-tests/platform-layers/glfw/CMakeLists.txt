cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

# Setup GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw EXCLUDE_FROM_ALL)

add_library(glfw-platform-layer)
target_sources(glfw-platform-layer PRIVATE glfw_main.c glad/glad.c)
target_include_directories(glfw-platform-layer PUBLIC "${rayfork_path}" "${platform_header_path}" ".")
target_link_libraries(glfw-platform-layer PRIVATE glfw)
target_link_libraries(glfw-platform-layer PUBLIC common-platform-interface)