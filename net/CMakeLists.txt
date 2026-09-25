add_subdirectory(net)

target_include_directories(net
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
