cmake_minimum_required(VERSION 3.1)

include_directories(include)

add_library(rayfork rayfork.c)
target_compile_features(rayfork PUBLIC c_std_99)
target_include_directories(rayfork PRIVATE libs)

add_library(rayfork-audio-old rayfork_audio_old.c)
target_compile_features(rayfork-audio-old PUBLIC c_std_99)
target_include_directories(rayfork-audio-old PRIVATE libs)

add_library(rayfork-audio rayfork_audio.c rayfork_audio.h)
target_compile_features(rayfork-audio PUBLIC c_std_99)
target_include_directories(rayfork-audio PRIVATE libs)