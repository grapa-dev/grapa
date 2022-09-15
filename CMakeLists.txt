cmake_minimum_required(VERSION 3.4...3.18)
project(grapapy)
add_subdirectory(source/pybind11)
include_directories(source)

if (MSVC)
    link_directories(
        source 
        source/grapa-lib/win
        )
    pybind11_add_module(grapapy s
        ource/mainpy.cpp
        )
    target_link_libraries(grapapy PRIVATE 
        grapa.lib
        )
endif (MSVC)
if (LINUX)
    link_directories(
        source 
        source/grapa-lib/linux 
        source/fl-lib/linux 
        source/openssl-lib/linux 
        source/blst-lib/linux
        )
    pybind11_add_module(grapapy 
        source/mainpy.cpp
        )
    target_link_libraries(grapapy PRIVATE
        libgrapa.a
        libblst.a
        libfltk_forms.a libfltk_gl.a libfltk_images.a libfltk_jpeg.a libfltk.a
        libcrypto.a libssl.a
        X11 Xfixes Xft Xext Xrender Xinerama Xcursor xcb Xau png16 fontconfig freetype
        )
endif (LINUX)
if (APPLE)
    link_directories(
        grapapy
        #source 
        source/grapa-lib/mac-intel 
        source/fl-lib/mac-intel 
        source/openssl-lib/mac-intel 
        source/blst-lib/mac-intel
        )
    pybind11_add_module(grapapy 
        source/mainpy.cpp
        )
    target_link_libraries(grapapy PRIVATE
        libgrapa.a
        libblst.a
        libfltk_forms.a libfltk_gl.a libfltk_images.a libfltk_jpeg.a libfltk_png.a libfltk.a
        libcrypto.a libssl.a
        "-framework CoreFoundation"
        "-framework AppKit"
        "-framework IOKit"
        )
endif (APPLE)

target_compile_definitions(grapapy PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
