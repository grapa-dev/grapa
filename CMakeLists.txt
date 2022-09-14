cmake_minimum_required(VERSION 3.4...3.18)
project(grapapy)
add_subdirectory(source/pybind11)
include_directories(source)

if (MSVC)
link_directories(source source/grapa-lib/win)
pybind11_add_module(grapapy source/mainpy.cpp)
target_link_libraries(grapapy PRIVATE grapa.lib)
endif (MSVC)

if (UNIX)
link_directories(source ${CMAKE_SOURCE_DIR}/source/grapa-lib/ubuntu64)
pybind11_add_module(grapapy source/mainpy.cpp)
target_link_libraries(grapapy PRIVATE libgrapa.a)
endif (UNIX)

if(APPLE)
link_directories(source ${INSTALL_PREFIX}/source/grapa-lib/mac-intel)
pybind11_add_module(grapapy source/mainpy.cpp)
target_link_libraries(grapapy PRIVATE libgrapa.a)
endif (APPLE)

target_compile_definitions(grapapy PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
