cmake_minimum_required(VERSION 3.5...3.18)

project(grapapy)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Handle static compilation flags
if(UTF8PROC_STATIC)
    add_definitions(-DUtf8proc_STATIC)
endif()

if(PCRE2_STATIC)
    add_definitions(-DPCRE2_STATIC)
endif()

add_subdirectory(source/pybind11)

include_directories(source)

link_directories(
    source 
    source/grapa-lib/${GRAPAPY_OS}
    source/fl-lib/${GRAPAPY_OS} 
    source/openssl-lib/${GRAPAPY_OS} 
    source/blst-lib/${GRAPAPY_OS}
    source/pcre2-lib/${GRAPAPY_OS}
    )

pybind11_add_module(grapapy 
    source/mainpy.cpp
    )

if (MSVC)
target_link_libraries(grapapy PRIVATE 
    grapa_static
    Gdi32
    Advapi32
    User32
    Ole32
    Shell32
    Comdlg32
    winspool
    crypt32
    )
endif (MSVC)

if (${GRAPAPY_OS} MATCHES "linux")
target_link_libraries(grapapy PRIVATE
    libgrapa_static.a
    libblst.a
    libfltk_forms.a libfltk_gl.a libfltk_images.a libfltk_jpeg.a libfltk.a
    libcrypto.a libssl.a
    libpcre2-8.a
    X11 Xfixes Xft Xext Xrender Xinerama Xcursor xcb Xau png16 fontconfig freetype
    )
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")
endif (${GRAPAPY_OS} MATCHES "linux")

if (${GRAPAPY_OS} MATCHES "aws")
target_link_libraries(grapapy PRIVATE
    libgrapa_static.a
    libblst.a
    libfltk_forms.a libfltk_gl.a libfltk_images.a libfltk_jpeg.a libfltk.a
    libcrypto.a libssl.a
    libpcre2-8.a
    X11 Xfixes Xft Xext Xrender Xinerama Xcursor xcb Xau png16 fontconfig freetype
    )
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")
endif (${GRAPAPY_OS} MATCHES "aws")

if (APPLE)
target_link_libraries(grapapy PRIVATE
    libgrapa_static.a
    libblst.a
    libfltk.a libfltk_forms.a libfltk_gl.a libfltk_images.a libfltk_jpeg.a libfltk_png.a
    libcrypto.a libssl.a
    libpcre2-8.a
    "-framework CoreFoundation"
    "-framework AppKit"
    "-framework IOKit"
    "-framework ScreenCaptureKit"
    )
endif (APPLE)
