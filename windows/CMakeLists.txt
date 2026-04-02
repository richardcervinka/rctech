add_subdirectory(platform)

target_include_directories(platform
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
