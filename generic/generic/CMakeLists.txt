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
