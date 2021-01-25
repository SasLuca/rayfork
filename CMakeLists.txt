cmake_minimum_required(VERSION 3.13)

project(rayfork)

# options
include(options.cmake)

# rayfork
add_subdirectory(thirdparty)
file(GLOB_RECURSE RAYFORK_SOURCES CONFIGURE_DEPENDS "sources/*.c")
set(RAYFORK_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_library(rayfork ${RAYFORK_LIB_TYPE})
target_include_directories(rayfork PUBLIC include)
target_compile_definitions(rayfork PUBLIC ${RAYFORK_GFX_BACKEND}=1)
target_link_libraries(rayfork PUBLIC c89atomic cgltf jar_mod jar_xm miniaudio par_shapes sokoL_gfx stb_image stb_image_resize stb_perlin stb_rect_pack stb_truetype stb_vorbis tinyobj_loader_c)
target_sources(rayfork PRIVATE ${RAYFORK_SOURCES})

# tests
add_subdirectory(tests)