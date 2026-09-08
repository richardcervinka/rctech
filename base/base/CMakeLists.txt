add_library(base STATIC
    "vector.h"
    "matrix.h"
    "quaternion.h"
    "quaternion.cpp"
    "color.h"
    "color.cpp"
    "geometry.h"
    "math.h"
    "event.h"
    "error.h"
    "stopwatch.h"
    "utf.h"
    "utf.cpp"
    "cli.h"
    "cli.cpp"
    "str.h"
    "str.cpp"
    "jsonstream.h"
    "jsonstream.cpp"
    "image.h"
    "image.cpp"
    "file.h"
    "file.cpp"
)

target_compile_features(base
    PUBLIC cxx_std_26
)

target_compile_options(base
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>
)

target_link_libraries(base
    PRIVATE lodepng
)