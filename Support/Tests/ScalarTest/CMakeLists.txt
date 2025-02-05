set(project ScalarTest)
set(folder "STBN SDK")

set(sources 
	ScalarTest.cpp
	STBNDataTest.cpp
	Kernel/ConstantKernelTest.cpp
	Kernel/GaussianKernelTest.cpp
	Kernel/SymmetricKernelTest.cpp
	VoidAndCluster/ReferenceImplTest.cpp
	VoidAndCluster/SliceCacheController2Dx1Dx1DTest.cpp
	VoidAndCluster/SliceCacheController2Dx2DTest.cpp
	VoidAndCluster/SliceCacheImpl2Dx1Dx1DTest.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${sources})

add_executable(${project} EXCLUDE_FROM_ALL ${sources})
target_link_libraries(${project} cxxopts gtest Scalar)