add_library(platform STATIC
	"application.h"
	"application.cpp"
	"window.h"
	"window.cpp"
	"input.h"
	"input.cpp"
	"log.h"
	"log.cpp"
	"utility.h"
	"utility.cpp"
	"system.h"
	"system.cpp"
	"library.h"
	"library.cpp"
	"error.h"
 	"error.cpp"
)

target_compile_features(platform
	PUBLIC cxx_std_26
)

target_link_libraries(platform
	PUBLIC base
	PUBLIC generic
	#PRIVATE renderer
)

target_compile_options(platform
	PRIVATE $<$<CXX_COMPILER_ID:Clang>:-finput-charset=UTF-8>
	PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fexec-charset=UTF-8>
	PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>
)