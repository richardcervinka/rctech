# vector

add_executable(test_vector "test_vector.cpp")

target_link_libraries(test_vector
	PRIVATE base
	PRIVATE gtest_main
)

# matrix

add_executable(test_matrix "test_matrix.cpp")

target_link_libraries(test_matrix
	PRIVATE base
	PRIVATE gtest_main
)

target_compile_features(test_matrix
	PRIVATE cxx_std_26
)

# color

add_executable(test_color "test_color.cpp")

target_link_libraries(test_color
	PRIVATE base
	PRIVATE gtest_main
)

# quaternion

add_executable(test_quaternion "test_quaternion.cpp")

target_link_libraries(test_quaternion
	PRIVATE base
	PRIVATE gtest_main
)

# stopwatch

add_executable(test_stopwatch "test_stopwatch.cpp")

target_link_libraries(test_stopwatch
	PRIVATE base
	PRIVATE gtest_main
)

target_compile_features(test_stopwatch
	PRIVATE cxx_std_26
)

# event

add_executable(test_event "test_event.cpp")

target_link_libraries(test_event
	PRIVATE base
	PRIVATE gtest_main
	PRIVATE gmock_main
	PRIVATE gmock
)

target_compile_features(test_event
	PRIVATE cxx_std_26
)