cmake_minimum_required(VERSION 3.1)

# c89atomic
add_library(c89atomic INTERFACE)
target_include_directories(c89atomic INTERFACE c89atomic)

# cgltf
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE cgltf)

# jar_mod
add_library(jar_mod INTERFACE)
target_include_directories(jar_mod INTERFACE jar_mod)

# jar_xm
add_library(jar_xm INTERFACE)
target_include_directories(jar_xm INTERFACE jar_xm)

# miniaudio
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE miniaudio)

# par_shapes
add_library(par_shapes INTERFACE)
target_include_directories(par_shapes INTERFACE par_shapes)

# sokol_gfx
add_library(sokol_gfx INTERFACE)
target_include_directories(sokol_gfx INTERFACE sokol_gfx)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE stb_image)

# stb_image_resize
add_library(stb_image_resize INTERFACE)
target_include_directories(stb_image_resize INTERFACE stb_image_resize)

# stb_perlin
add_library(stb_perlin INTERFACE)
target_include_directories(stb_perlin INTERFACE stb_perlin)

# stb_rect_pack
add_library(stb_rect_pack INTERFACE)
target_include_directories(stb_rect_pack INTERFACE stb_rect_pack)

# stb_truetype
add_library(stb_truetype INTERFACE)
target_include_directories(stb_truetype INTERFACE stb_truetype)

# stb_vorbis
add_library(stb_vorbis INTERFACE)
target_include_directories(stb_vorbis INTERFACE stb_vorbis)

# tinyobj_loader_c
add_library(tinyobj_loader_c INTERFACE)
target_include_directories(tinyobj_loader_c INTERFACE tinyobj_loader_c)