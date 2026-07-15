add_library(std26 INTERFACE
    "inplace_vector.h"
)

target_compile_features(std26
    INTERFACE cxx_std_26
)