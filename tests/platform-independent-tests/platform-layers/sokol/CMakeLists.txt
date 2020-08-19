cmake_minimum_required(VERSION 3.1)
set(CMAKE_C_STANDARD 99)

add_library(sokol-platform-layer)
target_sources(sokol-platform-layer PRIVATE sokol_main.c glad/glad.c)
target_include_directories(sokol-platform-layer PUBLIC "${rayfork_path}" "sokol" ".")

if (APPLE)
target_link_libraries(sokol-platform-layer PUBLIC common-platform-interface "-framework Cocoa" "-framework QuartzCore" "-framework OpenGL")
target_compile_options(sokol-platform-layer PUBLIC "-fobjc-arc" "-ObjC")
endif()