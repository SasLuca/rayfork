add_executable(unit-test-suite)
target_sources(unit-test-suite PRIVATE tests.cpp)
target_link_libraries(unit-test-suite PUBLIC rayfork catch2)
target_compile_features(unit-test-suite PUBLIC cxx_std_17)
set_target_properties(unit-test-suite PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)