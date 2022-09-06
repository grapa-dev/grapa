cmake_minimum_required(VERSION 3.4...3.18)
project(grapapy)
add_subdirectory(pybind11)
include_directories(source)
link_directories(source)
pybind11_add_module(grapapy source/mainpy.cpp)

if (MSVC)
target_link_libraries(grapapy PRIVATE grapa-lib/win/grapa)
endif (MSVC)

if (UNIX)
target_link_libraries(grapapy PRIVATE grapa-lib/ubuntu64/libgrapa.so)
endif (UNIX)

target_compile_definitions(grapapy PRIVATE VERSION_INFO="0.0.1")
