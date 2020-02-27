cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(bunnymark
        main.c
        game.c
        ../dependencies/glad/glad.c)

target_link_libraries(bunnymark PRIVATE rayfork)