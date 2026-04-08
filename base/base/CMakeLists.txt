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
)

target_compile_features(base
	PUBLIC cxx_std_26
)