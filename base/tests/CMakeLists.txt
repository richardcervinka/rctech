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

# event

add_executable(test_event "test_event.cpp")

target_link_libraries(test_event
	PRIVATE base
	PRIVATE gtest_main
	PRIVATE gmock_main
	PRIVATE gmock
)

# utf

add_executable(test_utf "test_utf.cpp")

target_link_libraries(test_utf
	PRIVATE base
	PRIVATE gtest_main
)

# cli

add_executable(test_cli "test_cli.cpp")

target_link_libraries(test_cli
	PRIVATE base
	PRIVATE gtest_main
)

# chars

add_executable(test_chars "test_chars.cpp")

target_link_libraries(test_chars
	PRIVATE base
	PRIVATE gtest_main
)