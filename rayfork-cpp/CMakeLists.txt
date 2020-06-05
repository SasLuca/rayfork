cmake_minimum_required(VERSION 3.1)

include_directories(include)

add_library(rayfork-cpp rayfork.cpp rayfork.hpp)
target_compile_features(rayfork-cpp PUBLIC cxx_std_17)
#set_target_properties(rayfork-cpp PROPERTIES
#    CXX_STANDARD 17
#    CXX_STANDARD_REQUIRED YES
##    CXX_EXTENSIONS NO
#)
target_link_libraries(rayfork-cpp PRIVATE rayfork)
target_include_directories(rayfork-cpp INTERFACE include)