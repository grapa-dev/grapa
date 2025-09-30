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

include_directories(source source/llama source/onnxruntime)

pybind11_add_module(grapapy 
    source/mainpy.cpp
    )

# Use target_link_directories instead of link_directories to avoid adding to RPATH
target_link_directories(grapapy PRIVATE
    source 
    source/grapa-lib/${GRAPAPY_OS}
    source/fl-lib/${GRAPAPY_OS} 
    source/openssl-lib/${GRAPAPY_OS} 
    source/blst-lib/${GRAPAPY_OS}
    source/pcre2-lib/${GRAPAPY_OS}
    source/llama-lib/${GRAPAPY_OS}
    )

if (MSVC)
# Enable OpenMP for Windows
find_package(OpenMP REQUIRED)
target_link_libraries(grapapy PRIVATE 
    grapa_static
    llama
    ggml
    ggml-base
    ggml-cpu
    mtmd
    onnx
    onnx_proto
    # ONNX Runtime shared libraries - link them directly into the extension
    ${CMAKE_CURRENT_SOURCE_DIR}/source/onnxruntime-lib/${GRAPAPY_OS}/onnxruntime.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/source/onnxruntime-lib/${GRAPAPY_OS}/onnxruntime_providers_shared.dll
    Gdi32
    Advapi32
    User32
    Ole32
    Shell32
    Comdlg32
    winspool
    crypt32
    OpenMP::OpenMP_CXX
    )
endif (MSVC)

if (${GRAPAPY_OS} MATCHES "linux")
target_link_libraries(grapapy PRIVATE
    libgrapa_static.a
    libblst.a
    libfltk_forms.a libfltk_gl.a libfltk_images.a libfltk_jpeg.a libfltk.a
    libcrypto.a libssl.a
    libpcre2-8.a
    libllama.a
    libggml.a
    libggml-base.a
    libggml-cpu.a
    libmtmd.a
    libcommon.a
    # ONNX Runtime shared libraries - link them directly into the extension
    libonnxruntime.so
    libonnxruntime_providers_shared.so
    # The actual shared libraries (.dylib, .so, .dll) must be available in the system path
    X11 Xfixes Xft Xext Xrender Xinerama Xcursor xcb Xau png16 fontconfig freetype
    gomp
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
    libllama.a
    libggml.a
    libggml-base.a
    libggml-cpu.a
    libmtmd.a
    libcommon.a
    # ONNX Runtime shared libraries - link them directly into the extension
    libonnxruntime.so
    libonnxruntime_providers_shared.so
    # The actual shared libraries (.dylib, .so, .dll) must be available in the system path
    X11 Xfixes Xft Xext Xrender Xinerama Xcursor xcb Xau png16 fontconfig freetype
    gomp
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
    libllama.a
    libggml.a
    libggml-base.a
    libggml-cpu.a
    libggml-metal.a
    libggml-blas.a
    libmtmd.a
    # ONNX Runtime shared libraries - use full path to avoid absolute RPATH
    ${CMAKE_CURRENT_SOURCE_DIR}/source/onnxruntime-lib/${GRAPAPY_OS}/libonnxruntime.dylib
    # The actual shared libraries (.dylib, .so, .dll) must be available in the system path
    "-framework CoreFoundation"
    "-framework AppKit"
    "-framework IOKit"
    "-framework ScreenCaptureKit"
    "-framework Metal"
    "-framework MetalKit"
    "-framework Accelerate"
    )
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "@loader_path")
endif (APPLE)
