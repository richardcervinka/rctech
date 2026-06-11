# projection

add_executable(test_projection "test_projection.cpp")

target_link_libraries(test_projection
    PRIVATE render
    PRIVATE gtest_main
)

target_compile_features(test_projection
    PRIVATE cxx_std_26
)