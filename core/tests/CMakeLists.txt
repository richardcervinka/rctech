# camera

add_executable(test_camera "test_camera.cpp")

target_link_libraries(test_camera
    PRIVATE core
    PRIVATE gtest_main
)

# transformations

add_executable(test_transformations "test_transformations.cpp")

target_link_libraries(test_transformations
    PRIVATE core
    PRIVATE gtest_main
)