add_library(platform-common INTERFACE)
target_include_directories(platform-common INTERFACE platform-layers/platform-common/include)

# Declare all of the platform layers
# sokol-app
add_library(sokol-platform-layer)
target_sources(sokol-platform-layer PRIVATE platform-layers/sokol/sokol-main.c)
target_link_libraries(sokol-platform-layer PRIVATE sokol_app glad sokol_time PUBLIC platform-common rayfork)

# glfw
add_library(glfw-platform-layer)
target_sources(glfw-platform-layer PRIVATE platform-layers/glfw/glfw-main.c)
target_link_libraries(glfw-platform-layer PRIVATE glfw glad sokol_time PUBLIC platform-common rayfork)

# sdl
add_library(sdl-platform-layer)
target_sources(sdl-platform-layer PRIVATE platform-layers/sdl/sdl-main.c)
target_compile_definitions(sdl-platform-layer PRIVATE SDL_MAIN_HANDLED)
target_link_libraries(sdl-platform-layer PRIVATE SDL2-static glad sokol_time PUBLIC platform-common rayfork)

# If not platform layer is selected we select sokol by default
if (NOT DEFINED RAYFORK_TESTS_PLATFORM_LAYER)
	message("-- No platform layer selected for the testing suite. Picking SOKOL as the default.")
	set(RAYFORK_TESTS_PLATFORM_LAYER "SOKOL")
endif()

# Select the platform layer we want to use
if ("${RAYFORK_TESTS_PLATFORM_LAYER}" STREQUAL "SOKOL")
	set(platform-layer sokol-platform-layer)
elseif ("${RAYFORK_TESTS_PLATFORM_LAYER}" STREQUAL "GLFW")
	set(platform-layer glfw-platform-layer)
elseif ("${RAYFORK_TESTS_PLATFORM_LAYER}" STREQUAL "SDL")
	set(platform-layer sdl-platform-layer)
endif()

# Special macos stuff
if (APPLE)
    target_link_libraries(${platform-layer} PUBLIC "-framework Cocoa" "-framework QuartzCore" "-framework OpenGL")
    target_compile_options(${platform-layer} PUBLIC "-fobjc-arc" "-ObjC")
endif()

# Load examples
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")    							 							# Create an iterable with all the filenames in examples/
foreach(file ${files})                                      							 							# Iterate over all files
	get_filename_component(name ${file} NAME_WE)            							 							# Create a variable `name` that stores the filename without the extension. NAME_WE means filename without extension. (https://cmake.org/cmake/help/v3.0/command/get_filename_component.html).
	add_executable(${name})                                 							 							# Create the target for the specific example
	target_sources(${name} PRIVATE ${file})				    							 							# Add sources to target
	target_link_libraries(${name} PUBLIC ${platform-layer} platform-common glad rayfork) # Link with the platform layer and rayfork
	target_compile_definitions(${name} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../assets/")				    # Add the ASSETS_PATH macro
endforeach()