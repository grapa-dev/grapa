# ONNX Runtime Build Documentation

This document describes how to build ONNX Runtime static libraries for Grapa on all supported platforms.

## Overview

ONNX Runtime provides a high-performance inference engine for ONNX models, enabling support for embedding models like "all-MiniLM-L6-v2" in Grapa. The build process creates static libraries that can be linked into both the main Grapa application and the grpapy Python extension.

## Build Scripts

### `build_onnx_libs.py`
Main build script for Linux and macOS platforms. This script:
- Detects the current platform (linux-amd64, linux-arm64, mac-arm64, mac-amd64, aws-amd64, aws-arm64)
- Checks for required dependencies
- Builds ONNX Runtime static libraries
- Copies libraries to `source/onnxruntime-lib/{platform}/`
- Copies headers to `source/onnxruntime/`

### `build_onnx_windows.py`
Windows-specific build script that:
- Builds both Release and Debug configurations
- Uses Visual Studio 2022 generator
- Creates libraries in `source/onnxruntime-lib/win-amd64/` and `source/onnxruntime-lib/win-amd64-debug/`
- Copies headers to `source/onnxruntime/`

## Dependencies

### Linux/Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y cmake build-essential git python3 libprotobuf-dev libomp-dev
```

### CentOS/RHEL/Amazon Linux
```bash
sudo yum install -y cmake gcc gcc-c++ make git python3 protobuf-devel libgomp
```

### macOS
```bash
brew install cmake protobuf libomp
```

### Windows
- Visual Studio 2022 with C++ development tools
- CMake
- Git
- Python 3

## Build Process

### 1. Source Preparation
The ONNX Runtime source should be located at `dep/onnx-main/`. This directory contains the complete ONNX Runtime source code.

### 2. Platform Detection
The build scripts automatically detect the platform:
- **Linux**: `linux-amd64` or `linux-arm64`
- **AWS Linux**: `aws-amd64` or `aws-arm64`
- **macOS**: `mac-amd64` or `mac-arm64`
- **Windows**: `win-amd64`

### 3. CMake Configuration
The build uses CMake with the following key configurations:
- `ONNX_BUILD_SHARED_LIBS=OFF`: Build static libraries
- `ONNX_USE_PROTOBUF_SHARED_LIBS=OFF`: Use static protobuf
- `ONNX_USE_LITE_PROTO=ON`: Use lite protobuf for smaller size
- `ONNX_USE_MSVC_STATIC_RUNTIME=ON`: Static runtime on Windows
- `-fPIC`: Position-independent code for shared library compatibility

### 4. Library Output
The build process creates the following static libraries:
- `libonnx.a`: Core ONNX Runtime library
- `libonnx_proto.a`: Protobuf definitions
- `libonnxifi.a`: ONNX Interface library
- `libonnxifi_loader.a`: ONNX Interface loader

### 5. Header Files
All necessary header files are copied to `source/onnxruntime/` for inclusion in the Grapa build.

## Usage

### Linux/macOS
```bash
cd scripts/build
python3 build_onnx_libs.py
```

### Windows
```bash
cd scripts/build
python3 build_onnx_windows.py
```

## Integration with Grapa

### 1. CMake Configuration
Add ONNX Runtime to your CMake configuration:
```cmake
# Find ONNX Runtime
find_path(ONNX_INCLUDE_DIR
    NAMES onnx/onnx.pb.h
    PATHS source/onnx/${PLATFORM}
)

find_library(ONNX_LIBRARY
    NAMES onnx
    PATHS source/onnx-lib/${PLATFORM}
)

# Link ONNX Runtime
target_link_libraries(grapa ${ONNX_LIBRARY})
target_include_directories(grapa PRIVATE ${ONNX_INCLUDE_DIR})
```

### 2. C++ Integration
Include ONNX Runtime headers in your C++ code:
```cpp
#include "onnxruntime_cxx_api.h"
```

### 3. Model Loading
Use ONNX Runtime to load and run embedding models:
```cpp
Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "GrapaModel");
Ort::Session session(env, "model.onnx", Ort::SessionOptions{nullptr});
```

## Platform-Specific Notes

### Linux
- Requires OpenMP support for parallel processing
- Uses GCC with -fPIC for shared library compatibility
- Supports both x86_64 and ARM64 architectures

### macOS
- Uses Clang compiler
- Supports both Intel and Apple Silicon
- Requires Xcode command line tools

### Windows
- Uses Visual Studio 2022
- Builds both Release and Debug configurations
- Uses MSVC static runtime for proper static linking

## Troubleshooting

### Common Issues

1. **Missing Dependencies**
   - Ensure all required packages are installed
   - Check that CMake version is 3.15 or higher
   - Verify that Python 3 is available

2. **Build Failures**
   - Check that the source directory `dep/onnx-main/` exists
   - Ensure sufficient disk space (build requires ~2GB)
   - Verify that all dependencies are properly installed

3. **Linking Issues**
   - Ensure static libraries are built with -fPIC
   - Check that all required libraries are present
   - Verify that header files are correctly copied

### Debug Information

The build scripts provide detailed output including:
- Dependency checking
- CMake configuration
- Build progress
- Library verification
- File copying status

## Next Steps

After building ONNX Runtime libraries:

1. **Update GrapaModel.cpp**: Implement ONNX embedding support
2. **Add Model Support**: Integrate "all-MiniLM-L6-v2" model loading
3. **Test Integration**: Verify embedding generation works correctly
4. **Update Documentation**: Document the new embedding capabilities

## Support

For issues with the ONNX Runtime build process:
1. Check the build output for specific error messages
2. Verify all dependencies are installed
3. Ensure the source directory structure is correct
4. Check platform-specific requirements
