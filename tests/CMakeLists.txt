cmake_minimum_required(VERSION 3.1)
project(rayfork-tests LANGUAGES C CXX)

# Set ASSETS_PATH define
if (NOT DEFINED ASSETS_PATH)
	add_compile_definitions(ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
endif()

# Add platform layers
add_subdirectory(platform-independent-tests/platform-layers/common-platform-interface)

if (RAYFORK_TESTS_PLATFORM_LAYER_SOKOL)
	add_subdirectory(platform-independent-tests/platform-layers/sokol)
	set(platform-layer sokol-platform-layer)
elseif(RAYFORK_TESTS_PLATFORM_LAYER_GLFW)
	add_subdirectory(platform-independent-tests/platform-layers/glfw)
	set(platform-layer glfw-platform-layer)
elseif(RAYFORK_TESTS_PLATFORM_LAYER_SDL)
	add_subdirectory(platform-independent-tests/platform-layers/sdl)
	set(platform-layer sdl-platform-layer)
else() # Choose sokol as the default
	message("-- No platform layer selected for the testing suite. Picking SOKOL as the default.")
	add_subdirectory(platform-independent-tests/platform-layers/sokol)
	set(platform-layer sokol-platform-layer)
endif()

# Load examples
file(GLOB files "platform-independent-tests/tests/*.c")      # Create an iterable with all the filenames in examples/
foreach(file ${files})                                       # Iterate over all files
	get_filename_component(name ${file} NAME_WE)             # Create a variable `name` that stores the filename without the extension. NAME_WE means filename with extension. (https://cmake.org/cmake/help/v3.0/command/get_filename_component.html).
	add_executable(${name})                                  # Create the target for the specific example
	target_sources(${name} PRIVATE ${file})				     # Add sources to target
	target_link_libraries(${name} PRIVATE ${platform-layer}) # Link with the platform layer and rayfork
	target_include_directories(${name} PUBLIC platform-independent-tests/platform-layers/) # Add platform.h to the include path
endforeach()

# Setup unit-tests
add_executable(unit-test-suite)
target_sources(unit-test-suite PRIVATE unit-tests/tests.cpp)
target_link_libraries(unit-test-suite PRIVATE ${rayfork})
target_compile_features(unit-test-suite PUBLIC cxx_std_17)
set_target_properties(unit-test-suite PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_include_directories(unit-test-suite PRIVATE "libs")