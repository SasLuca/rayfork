cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE RAYFORK_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.c") # Define RAYFORK_GDX_SOURCES as a list of all source files
set(RAYFORK_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/") # Define RAYFORK_GDX_INCLUDE to be the path to the include directory of rayfork

add_library(${PROJECT_NAME} ${RAYFORK_LIB_TYPE}) # Declare the rayfork library with the lib type (static/shared) as defined by the options
target_sources(${PROJECT_NAME} PRIVATE ${RAYFORK_SOURCES}) # Add the sources
target_include_directories(${PROJECT_NAME} PUBLIC ${RAYFORK_INCLUDE}) # Add the include directory
target_compile_definitions(${PROJECT_NAME} PUBLIC ${RAYFORK_GFX_BACKEND}=1) # Add proper defines to setup rayfork
target_link_libraries(${PROJECT_NAME} PUBLIC rayfork-foundation PRIVATE c89atomic cgltf jar_mod jar_xm miniaudio par_shapes sokol_gfx stb_image stb_image_resize stb_perlin stb_rect_pack stb_truetype stb_vorbis tinyobj_loader_c) # Link all the dependencies to rayfork