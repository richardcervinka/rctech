cmake_minimum_required(VERSION 4.0)

project(rctech)

find_program(
    TOOL_DXC dxc
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/bin/dxc"
    NO_DEFAULT_PATH
    REQUIRED
)

add_subdirectory("res")
add_subdirectory("ext")
add_subdirectory("tools")
add_subdirectory("hlsl")
add_subdirectory("base")
add_subdirectory("core")
add_subdirectory("windows")
add_subdirectory("generic")
add_subdirectory("vulkan")
add_subdirectory("renderer")

add_executable(app WIN32
    "main.cpp"
 )

target_compile_features(app
    PRIVATE cxx_std_26
)

target_link_libraries(app
    PRIVATE base
    PRIVATE generic
    PRIVATE renderer
)