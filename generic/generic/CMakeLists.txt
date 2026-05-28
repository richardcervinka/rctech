add_library(generic STATIC
    "application.h"
    "application.cpp"
    "input.h"
    "char_input.h"
    "char_input.cpp"
)

target_compile_features(generic
    PUBLIC cxx_std_26
)

target_include_directories(generic
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(generic
    PUBLIC platform
    PRIVATE renderer
)

target_compile_options(base
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>
)