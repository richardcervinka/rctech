# transformations

add_executable(test_transformations "test_transformations.cpp")

target_link_libraries(test_transformations
	PRIVATE core
	PRIVATE gtest_main
)