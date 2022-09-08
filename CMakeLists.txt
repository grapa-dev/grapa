cmake_minimum_required(VERSION 3.4...3.18)
project(grapapy)
add_subdirectory(source/pybind11)
add_subdirectory(source/grapa-lib)

if (MSVC)
link_directories(source ${CMAKE_SOURCE_DIR}/source/grapa-lib/win)
endif (MSVC)

if (UNIX)
link_directories(source ${CMAKE_SOURCE_DIR}/source/grapa-lib/ubuntu64)
endif (UNIX)

pybind11_add_module(grapapy source/mainpy.cpp)
target_link_libraries(grapapy PRIVATE grapa)

target_compile_definitions(grapapy PRIVATE VERSION_INFO="0.0.2")