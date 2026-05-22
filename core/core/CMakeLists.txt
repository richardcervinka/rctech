add_library(core STATIC
	"vertex.h"
    "vertex.cpp"
	"transformations.h"
    "transformations.cpp"
)

target_compile_features(core
	PUBLIC cxx_std_26
)

target_link_libraries(core
	PUBLIC base
)