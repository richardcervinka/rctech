add_library(platform STATIC
	"application.h"
	"application.cpp"
	"window.h"
	"window.cpp"
	"input.h"
	"input.cpp"
	"log.h"
	"log.cpp"
	"windows/utility.h"
	"windows/utility.cpp"
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
