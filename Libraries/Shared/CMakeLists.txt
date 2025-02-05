set(project Shared)
set(folder "STBN")

set(sources
	BlueNoiseTexturesND.h
	BlueNoiseTexturesND.cpp
	ProgressContext.h
	ProgressContext.cpp
	STBNMath.h
	STBNRandom.h
	"Kernel/BlueNoiseGaussianKernel.h"
	"Kernel/ConstantKernel.h"
	"Kernel/GaussianKernel.h"
	"Kernel/SymmetricKernel.h"
	"Kernel/VectorBlueNoiseGaussianKernel.h"
	"Kernel/BlueNoiseGaussianKernel.cpp"
	"Kernel/ConstantKernel.cpp"
	"Kernel/GaussianKernel.cpp"
	"Kernel/SymmetricKernel.cpp"
	"Kernel/VectorBlueNoiseGaussianKernel.cpp")

add_library(Shared EXCLUDE_FROM_ALL ${sources})
set_target_properties(Shared PROPERTIES FOLDER "STBN SDK")
target_link_libraries(${project} pcg-c-basic stb)
target_include_directories(Shared PUBLIC ".")