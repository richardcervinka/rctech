add_library(test_net INTERFACE)

target_link_libraries(test_net
    INTERFACE net
    INTERFACE gtest_main
)

target_compile_features(test_net
    INTERFACE cxx_std_26
)

# IPv4 address

add_executable(test_ipv4_address "test_ipv4_address.cpp")

target_link_libraries(test_ipv4_address
    PRIVATE test_net
)