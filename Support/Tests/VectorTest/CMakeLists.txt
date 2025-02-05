set(project VectorTest)
set(folder "STBN SDK")

set(sources 
	main.cpp
	ValueDistanceFunctions/FloatValueDistanceFunctionsTest.cpp
	ValueDistanceFunctions/Float2ValueDistanceFunctionsTest.cpp
	ValueDistanceFunctions/Float3ValueDistanceFunctionsTest.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${sources})

add_executable(${project} EXCLUDE_FROM_ALL ${sources})
target_link_libraries(${project} gtest Vector)
