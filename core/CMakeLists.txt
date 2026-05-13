add_subdirectory(core)

target_include_directories(core
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

#TODO if (ENABLE_TESTING)
add_subdirectory(tests)
