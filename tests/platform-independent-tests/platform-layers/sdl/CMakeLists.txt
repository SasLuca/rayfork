cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)

# Setup SDL
add_subdirectory(sdl EXCLUDE_FROM_ALL)
add_compile_definitions(SDL_MAIN_HANDLED)

add_library(sdl-platform-layer)
target_sources(sdl-platform-layer PRIVATE sdl_main.c glad/glad.c)
target_include_directories(sdl-platform-layer PUBLIC "${rayfork_path}" "${platform_header_path}" "${SDL2_INCLUDE_DIR}" ".")
target_link_libraries(sdl-platform-layer PRIVATE SDL2-static)
target_link_libraries(sdl-platform-layer PUBLIC common-platform-interface)

if (APPLE)
    target_link_libraries(sdl-platform-layer PUBLIC common-platform-interface "-framework Cocoa" "-framework QuartzCore" "-framework OpenGL")
    target_compile_options(sdl-platform-layer PUBLIC "-fobjc-arc" "-ObjC")
endif()