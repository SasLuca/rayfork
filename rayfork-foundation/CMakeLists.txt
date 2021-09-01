cmake_minimum_required(VERSION 3.13)
project(rayfork-foundation)

file(GLOB_RECURSE RAYFORK_FOUNDATION_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.c") # Define RAYFORK_FOUNDATION_SOURCES as a list of all source files
set(RAYFORK_FOUNDATION_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/") # Define RAYFORK_FOUNDATION_INCLUDE to be the path to the include directory of rayfork

add_library(${PROJECT_NAME} ${RAYFORK_LIB_TYPE}) # Declare the rayfork library with the lib type (static/shared) as defined by the options
target_sources(${PROJECT_NAME} PRIVATE ${RAYFORK_FOUNDATION_SOURCES}) # Add the sources
target_include_directories(${PROJECT_NAME} PUBLIC ${RAYFORK_FOUNDATION_INCLUDE}) # Add the include directory
