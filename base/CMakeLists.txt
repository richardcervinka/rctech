add_subdirectory(base)

target_include_directories(base
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

#TODO if (ENABLE_TESTING)
add_subdirectory(tests)
