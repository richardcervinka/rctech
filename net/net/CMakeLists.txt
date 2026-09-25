add_library(net STATIC
    "ipv4_address.h"
    "ipv4_address.cpp"
)

target_compile_features(net
    PUBLIC cxx_std_26
)

target_compile_options(net
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>
)