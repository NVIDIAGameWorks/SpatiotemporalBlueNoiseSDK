set(project VectorApp)
set(folder "STBN SDK")

set(sources
	CMDOptionsParser.h
	CMDOptionsParser.cpp
	main.cpp
	VectorProgramOptions.h)

add_executable(VectorApp EXCLUDE_FROM_ALL ${sources})
# Workaround needed because Vector only has .h files but depends on other library
# When Vector gets de-templated, change Shared to Vector and remove the target_include_directories
target_link_libraries(${project} cxxopts Shared Vector)
target_include_directories(${project} PRIVATE "../Vector")
set_target_properties(VectorApp PROPERTIES FOLDER "STBN SDK")
