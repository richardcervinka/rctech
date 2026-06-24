cmake_minimum_required(VERSION 4.0)

project(rctech)

add_definitions(-DNOMINMAX)

add_subdirectory("res")
add_subdirectory("ext")
add_subdirectory("tools")
add_subdirectory("shaders")
add_subdirectory("base")
add_subdirectory("core")
add_subdirectory("windows")
add_subdirectory("generic")
add_subdirectory("vulkan")
add_subdirectory("render")

add_executable(app WIN32
    "main.cpp"
 )

target_compile_features(app
    PRIVATE cxx_std_26
)

target_link_libraries(app
    PRIVATE base
    PRIVATE generic
    PRIVATE render
)