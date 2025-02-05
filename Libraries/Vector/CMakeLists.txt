set(project Vector)
set(folder "STBN SDK")

set(headers
	Debug/DebugFileExport.h
	Exporters/RGBAExporters.h
	ImportanceSampling/ImportanceSamplingData.h
	ImportanceSampling/ImportanceSamplingDataLoaders.h
	RandomTypeGenerators/FloatRandomGenerators.h
	RandomTypeGenerators/Float2RandomGenerators.h
	RandomTypeGenerators/Float3RandomGenerators.h
	RandomTextureGenerators/CosineWeightedHemisphereFloat3RandomTextureGenerator.h
	RandomTextureGenerators/ImportanceSampledUnitFloat3RandomTextureGenerator.h
	RandomTextureGenerators/ImportanceSampledRandomTextureGenerator.h
	RandomTextureGenerators/DefaultRandomTextureGenerator.h
	RandomTextureGenerators/PDFStats.h
	Reporting/ProgressReporter.h
	SimulatedAnnealing/SAData.h
	SimulatedAnnealing/SADataController.h
	SimulatedAnnealing/SAProgressData.h
	SimulatedAnnealing/SimulatedAnnealing.h
	Utils/Dimensions3D.h
	Utils/PixelCoords3D.h
	Utils/SphericalCoordinateMath.h
	ValueDistanceFunctions/FloatValueDistanceFunctions.h
	ValueDistanceFunctions/Float2ValueDistanceFunctions.h
	ValueDistanceFunctions/Float3ValueDistanceFunctions.h
	Types/Float.h
	Types/Float2.h
	Types/Float3.h
	VectorSTBNArgs.h
	VectorSTBNImplSelector.h
	VectorSTBNMaker.h
	VectorSTBNProgressData.h)

set(sources 
	Debug/DebugFileExport.cpp
	Exporters/RGBAExporters.cpp
	ImportanceSampling/ImportanceSamplingDataLoaders.cpp
	RandomTypeGenerators/FloatRandomGenerators.cpp
	RandomTypeGenerators/Float2RandomGenerators.cpp
	RandomTypeGenerators/Float3RandomGenerators.cpp
	RandomTextureGenerators/CosineWeightedHemisphereFloat3RandomTextureGenerator.cpp
	RandomTextureGenerators/ImportanceSampledUnitFloat3RandomTextureGenerator.cpp
	Reporting/ProgressReporter.cpp
	Utils/Dimensions3D.cpp
	Utils/PixelCoords3D.cpp
	Utils/SphericalCoordinateMath.cpp
	ValueDistanceFunctions/FloatValueDistanceFunctions.cpp
	ValueDistanceFunctions/Float2ValueDistanceFunctions.cpp
	ValueDistanceFunctions/Float3ValueDistanceFunctions.cpp
	VectorSTBNImplSelector.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${headers})

add_library(Vector STATIC EXCLUDE_FROM_ALL ${sources} ${headers})
set_target_properties(Vector PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${project} Shared)
target_include_directories(Vector PUBLIC ".")
