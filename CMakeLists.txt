cmake_minimum_required(VERSION 3.4...3.18)
project(grapapy)
add_subdirectory(pybind11)

if (MSVC)
include_directories(
	/Users/matic/Documents/GitHub/Grapa/source
	)
link_directories(
	/Users/matic/Documents/GitHub/Grapa/source
	/Users/matic/Documents/GitHub/Grapa/source/grapa-lib/win
	)
pybind11_add_module(
	grapapy source/mainpy.cpp
	)
target_link_libraries(
	grapapy PRIVATE 
	/Users/matic/Documents/GitHub/Grapa/source/grapa-lib/win/grapa.lib
	)
endif (MSVC)

if (UNIX)
include_directories(
	/home/matichuk/Documents/GitHub/grapa/source
	)
link_directories(
	/home/matichuk/Documents/GitHub/grapa/source
	/home/matichuk/Documents/GitHub/grapa/source/grapa-lib/ubuntu64
	)
pybind11_add_module(
	grapapy source/mainpy.cpp
	)
target_link_libraries(
	grapapy PRIVATE
	/home/matichuk/Documents/GitHub/grapa/source/grapa-lib/ubuntu64/libgrapa.so
	)
endif (UNIX)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(grapapy PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

