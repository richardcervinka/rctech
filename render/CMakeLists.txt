add_subdirectory(render)

target_include_directories(render
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
