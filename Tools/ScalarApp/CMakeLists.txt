set(project ScalarApp)
set(folder "STBN SDK")

set(sources 
	main.cpp)

add_executable(ScalarApp EXCLUDE_FROM_ALL ${sources})
target_link_libraries(${project} cxxopts Scalar)
set_target_properties(ScalarApp PROPERTIES FOLDER "STBN SDK")