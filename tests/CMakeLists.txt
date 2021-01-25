cmake_minimum_required(VERSION 3.1)
project(rayfork-tests LANGUAGES C CXX)

add_subdirectory(thirdparty/catch2)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/sdl)
add_subdirectory(thirdparty/sokol)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw EXCLUDE_FROM_ALL)

add_subdirectory(platform-independent-tests)

# Setup unit-tests
add_executable(unit-test-suite)
target_sources(unit-test-suite PRIVATE unit-tests/tests.cpp)
target_link_libraries(unit-test-suite PUBLIC ${rayfork})
target_compile_features(unit-test-suite PUBLIC cxx_std_17)
set_target_properties(unit-test-suite PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_include_directories(unit-test-suite PRIVATE "libs")