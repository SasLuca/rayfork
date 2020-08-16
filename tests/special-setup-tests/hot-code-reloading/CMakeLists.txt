cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(hot-code-reloading win32_main.c)

add_library(hot-code-reloading-dynamic SHARED
        game.c
        ../../platform-independent-tests/platform-layers/sokol/glad/glad.c)

target_link_libraries(hot-code-reloading PRIVATE rayfork)
target_link_libraries(hot-code-reloading-dynamic PRIVATE rayfork)