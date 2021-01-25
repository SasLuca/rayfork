cmake_minimum_required(VERSION 3.1)

# Set ASSETS_PATH define
if (NOT DEFINED ASSETS_PATH)
	add_compile_definitions(ASSETS_PATH="${CMAKE_SOURCE_DIR}/tests/assets/")
endif()

add_subdirectory(platform-layers/platform-common)

# sokol-app
add_library(sokol-platform-layer)
target_sources(sokol-platform-layer PRIVATE platform-layers/sokol/sokol-main.c)
target_link_libraries(sokol-platform-layer PUBLIC sokol-app glad rayfork platform-common)

if (APPLE)
	target_link_libraries(sokol-platform-layer PUBLIC "-framework Cocoa" "-framework QuartzCore" "-framework OpenGL")
	target_compile_options(sokol-platform-layer PUBLIC "-fobjc-arc" "-ObjC")
endif()

# glfw
add_library(glfw-platform-layer)
target_sources(glfw-platform-layer PRIVATE platform-layers/glfw/glfw-main.c)
target_link_libraries(glfw-platform-layer PUBLIC glfw glad rayfork platform-common)

if (APPLE)
    target_link_libraries(glfw-platform-layer PUBLIC "-framework Cocoa" "-framework QuartzCore" "-framework OpenGL")
    target_compile_options(glfw-platform-layer PUBLIC "-fobjc-arc" "-ObjC")
endif()

# If not platform layer is selected we select sokol by default
if (NOT DEFINED RAYFORK_TESTS_PLATFORM_LAYER)
	message("-- No platform layer selected for the testing suite. Picking SOKOL as the default.")
	set(RAYFORK_TESTS_PLATFORM_LAYER "SOKOL")
endif()

# Setup platform layers
if (${RAYFORK_TESTS_PLATFORM_LAYER} STREQUAL "SOKOL")
	set(platform-layer sokol-platform-layer)
elseif(${RAYFORK_TESTS_PLATFORM_LAYER} STREQUAL "GLFW")
	set(platform-layer glfw-platform-layer)
elseif(${RAYFORK_TESTS_PLATFORM_LAYER} STREQUAL "SLD")
	set(platform-layer sdl-platform-layer)
endif()

# Load examples
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")     # Create an iterable with all the filenames in examples/
foreach(file ${files})                                       # Iterate over all files
	get_filename_component(name ${file} NAME_WE)             # Create a variable `name` that stores the filename without the extension. NAME_WE means filename with extension. (https://cmake.org/cmake/help/v3.0/command/get_filename_component.html).
	add_executable(${name})                                  # Create the target for the specific example
	target_sources(${name} PRIVATE ${file})				     # Add sources to target
	target_link_libraries(${name} PRIVATE ${platform-layer}) # Link with the platform layer and rayfork
endforeach()