cmake_minimum_required(VERSION 3.10)

project(STBN)

# C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Set compiler flags
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /wd5045 /MP")

    macro(replace_msvcrt var value)
        # Remove the existing /MD-type flags, if any
        string(REGEX REPLACE "/M[TD]d?\\s*" "" ${var} ${${var}})
        # Append the new flag
        set(${var} "${${var}} ${value}")
    endmacro(replace_msvcrt)
    
    replace_msvcrt(CMAKE_C_FLAGS_DEBUG "/MTd")
    replace_msvcrt(CMAKE_C_FLAGS_MINSIZEREL "/MT")
    replace_msvcrt(CMAKE_C_FLAGS_RELEASE "/MT")
    replace_msvcrt(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT")

    replace_msvcrt(CMAKE_CXX_FLAGS_DEBUG "/MTd")
    replace_msvcrt(CMAKE_CXX_FLAGS_MINSIZEREL "/MT")
    replace_msvcrt(CMAKE_CXX_FLAGS_RELEASE "/MT")
    replace_msvcrt(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	# JsonCpp uses its own deprecated methods, which produces annoying warnings.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

endif()

# CMake directory locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Add cxxopts dependency
if (NOT TARGET cxxopts)
	option(CXXOPTS_BUILD_EXAMPLES OFF)
	option(CXXOPTS_BUILD_TESTS OFF)
	option(CXXOPTS_ENABLE_INSTALL OFF)
	add_subdirectory(External/cxxopts)
endif()

# Add pcg dependency
set(pcg-sources
    External/pcg-c-basic/pcg_basic.c
    External/pcg-c-basic/pcg_basic.h
)
add_library(pcg-c-basic STATIC EXCLUDE_FROM_ALL ${pcg-sources})
set_target_properties(pcg-c-basic PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(pcg-c-basic PUBLIC "External/pcg-c-basic")

# Add stb dependency
set(stb-sources
    External/stb/stb_image.h
    External/stb/stb_image_write.h
)
add_library(stb INTERFACE EXCLUDE_FROM_ALL ${stb-sources})
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stb INTERFACE "External/stb")

add_subdirectory(External/gtest)

add_subdirectory(Libraries/Scalar)
add_subdirectory(Libraries/Shared)
add_subdirectory(Libraries/Vector)
add_subdirectory(Support/Tests/ScalarTest)
add_subdirectory(Support/Tests/VectorTest)
add_subdirectory(Tools/ScalarApp)
add_subdirectory(Tools/VectorApp)

