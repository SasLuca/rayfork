cmake_minimum_required(VERSION 3.1)

# c89atomic
add_library(c89atomic INTERFACE)
target_include_directories(c89atomic INTERFACE c89atomic/include)

# cgltf
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE cgltf/include)

# jar_mod
add_library(jar_mod INTERFACE)
target_include_directories(jar_mod INTERFACE jar_mod/include)

# jar_xm
add_library(jar_xm INTERFACE)
target_include_directories(jar_xm INTERFACE jar_xm/include)

# miniaudio
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE miniaudio/include)

# par_shapes
add_library(par_shapes INTERFACE)
target_include_directories(par_shapes INTERFACE par_shapes/include)

# sokol_gfx
add_library(sokol_gfx INTERFACE)
target_include_directories(sokol_gfx INTERFACE sokol_gfx/include)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE stb_image/include)

# stb_image_resize
add_library(stb_image_resize INTERFACE)
target_include_directories(stb_image_resize INTERFACE stb_image_resize/include)

# stb_perlin
add_library(stb_perlin INTERFACE)
target_include_directories(stb_perlin INTERFACE stb_perlin/include)

# stb_rect_pack
add_library(stb_rect_pack INTERFACE)
target_include_directories(stb_rect_pack INTERFACE stb_rect_pack/include)

# stb_truetype
add_library(stb_truetype INTERFACE)
target_include_directories(stb_truetype INTERFACE stb_truetype/include)

# stb_vorbis
add_library(stb_vorbis INTERFACE)
target_include_directories(stb_vorbis INTERFACE stb_vorbis/include)

# tinyobj_loader_c
add_library(tinyobj_loader_c INTERFACE)
target_include_directories(tinyobj_loader_c INTERFACE tinyobj_loader_c/include)

# catch2
add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE catch2/include)

# glad
add_subdirectory(glad)

# sokol_app
add_library(sokol_app INTERFACE)
target_include_directories(sokol_app INTERFACE sokol_app/include)

# sokol_time
add_library(sokol_time INTERFACE)
target_include_directories(sokol_time INTERFACE sokol_time/include)

# sdl2
set(LIBC ON CACHE BOOL "" FORCE)
add_subdirectory(sdl2)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)