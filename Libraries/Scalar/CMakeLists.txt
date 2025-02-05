set(project Scalar)
set(folder "STBN SDK")

set(headers
	"STBNData.h"
	"STBNMaker.h"
	"Reporting/ProgressReporter.h"
	"Utils/Dimensions.h"
	"Utils/PixelCoords.h"
	"VoidAndCluster/VCController.h"
	"VoidAndCluster/VCImpl.h"
	"VoidAndCluster/VoidAndCluster.h"
	"VoidAndCluster/Reference/ReferenceFuncs.h"
	"VoidAndCluster/Reference/ReferenceImpl.h"
	"VoidAndCluster/Reference/ReferenceController2Dx1Dx1D.h"
	"VoidAndCluster/Reference/ReferenceController2Dx2D.h"
	"VoidAndCluster/SliceCache/SliceCacheController2Dx1Dx1D.h"
	"VoidAndCluster/SliceCache/SliceCacheController2Dx2D.h"
	"VoidAndCluster/SliceCache/SliceCacheImpl.h"
	)

set(sources 
	"STBNData.cpp"
	"STBNMaker.cpp"
	"Reporting/ProgressReporter.cpp"
	"Utils/Dimensions.cpp"
	"Utils/PixelCoords.cpp"
	"VoidAndCluster/VCController.cpp"
	"VoidAndCluster/VCImpl.cpp"
	"VoidAndCluster/VoidAndCluster.cpp"
	"VoidAndCluster/Reference/ReferenceFuncs.cpp"
	"VoidAndCluster/Reference/ReferenceImpl.cpp"
	"VoidAndCluster/Reference/ReferenceController2Dx1Dx1D.cpp"
	"VoidAndCluster/Reference/ReferenceController2Dx2D.cpp"
	"VoidAndCluster/SliceCache/SliceCacheController2Dx1Dx1D.cpp"
	"VoidAndCluster/SliceCache/SliceCacheController2Dx2D.cpp"
	"VoidAndCluster/SliceCache/SliceCacheImpl.cpp"
	)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${headers})

add_library(${project} ${sources} ${headers})
set_target_properties(${project} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${project} Shared)
target_include_directories(${project} PUBLIC ".")