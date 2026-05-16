add_subdirectory(core)

target_include_directories(core
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if (BUILD_TESTING)
	add_subdirectory(tests)
endif()
