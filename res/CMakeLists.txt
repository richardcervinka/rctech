set(SPIRV_PATH ${CMAKE_CURRENT_BINARY_DIR})

# clang does not support correct #embed include paths
configure_file("resources.cpp" "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp")

add_library(resources STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/resources.h"
    "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp"
)

add_dependencies(resources spirv)

target_compile_features(resources
    PUBLIC cxx_std_26
)

target_include_directories(resources
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_options(resources
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>
)