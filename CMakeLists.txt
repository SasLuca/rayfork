cmake_minimum_required(VERSION 3.13)
project(rayfork)

# Include the cmake file with all the options for configuring rayfork from cmake
include(options.cmake)

# Add all the thirdparty library target definitions
add_subdirectory(thirdparty)

# Add rayfork modules
add_subdirectory(rayfork-foundation)
add_subdirectory(rayfork)

# Add rayfork tests
if (RAYFORK_TESTS)
add_subdirectory(tests/platform-independent-tests)
add_subdirectory(tests/unit-tests)
endif()