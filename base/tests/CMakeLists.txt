add_library(test_base INTERFACE)

target_link_libraries(test_base
    INTERFACE base
    INTERFACE gtest_main
)

target_compile_features(test_base
    INTERFACE cxx_std_26
)

# vector

add_executable(test_vector "test_vector.cpp")

target_link_libraries(test_vector
    PRIVATE test_base
)

# matrix

add_executable(test_matrix "test_matrix.cpp")

target_link_libraries(test_matrix
    PRIVATE test_base
)

# color

add_executable(test_color "test_color.cpp")

target_link_libraries(test_color
    PRIVATE test_base
)

# quaternion

add_executable(test_quaternion "test_quaternion.cpp")

target_link_libraries(test_quaternion
    PRIVATE test_base
)

# stopwatch

add_executable(test_stopwatch "test_stopwatch.cpp")

target_link_libraries(test_stopwatch
    PRIVATE test_base
)

# event

add_executable(test_event "test_event.cpp")

target_link_libraries(test_event
    PRIVATE test_base
    PRIVATE gmock_main
    PRIVATE gmock
)

# utf

add_executable(test_utf "test_utf.cpp")

target_link_libraries(test_utf
    PRIVATE test_base
)

# cli

add_executable(test_cli "test_cli.cpp")

target_link_libraries(test_cli
    PRIVATE test_base
)

# str

add_executable(test_str "test_str.cpp")

target_link_libraries(test_str
    PRIVATE test_base
)

# jsonstream

add_executable(test_jsonstream "test_jsonstream.cpp")

target_link_libraries(test_jsonstream
    PRIVATE test_base
)
