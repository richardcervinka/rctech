add_library(lodepng
    "lodepng.h"
    "lodepng.cpp"
)

target_compile_features(lodepng
    PUBLIC cxx_std_26
)

target_include_directories(lodepng
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(lodepng
    PUBLIC LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS=1
)