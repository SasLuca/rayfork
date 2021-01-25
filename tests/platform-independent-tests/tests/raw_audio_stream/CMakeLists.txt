cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

add_executable(raw_audio_stream
        main.c
        game.c
        ../../../libs/glad/glad.c)