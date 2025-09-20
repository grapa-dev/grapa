# Grapa Development Kit

This directory contains everything you need to build and run Grapa applications using the Grapa static library, including AI/ML model support via LLAMA.cpp integration.

## Directory Structure

```
bin/
├── include/grapa/          # Grapa public API headers
├── include/llama/          # LLAMA.cpp headers for AI/ML support
├── lib/                    # 3rd party static libraries
│   ├── mac-arm64/         # Mac ARM64 libraries (including LLAMA.cpp)
│   ├── linux-arm64/       # Linux ARM64 libraries (including LLAMA.cpp)
│   ├── linux-amd64/       # Linux AMD64 libraries (including LLAMA.cpp)
│   ├── aws-arm64/         # AWS ARM64 libraries (including LLAMA.cpp)
│   ├── aws-amd64/         # AWS AMD64 libraries (including LLAMA.cpp)
│   └── win-amd64/         # Windows AMD64 libraries (including LLAMA.cpp)
├── platforms/              # Platform-specific Grapa files
│   ├── mac-arm64/         # Mac ARM64 executable and static library
│   ├── linux-arm64/       # Linux ARM64 executable and static library
│   ├── linux-amd64/       # Linux AMD64 executable and static library
│   ├── aws-arm64/         # AWS ARM64 executable and static library
│   ├── aws-amd64/         # AWS AMD64 executable and static library
│   └── win-amd64/         # Windows AMD64 executable and static library
├── main.cpp                # Example C++ source code
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
└── install-grapa.py        # Universal installer script
```

## Building the Example

### Prerequisites

- **CMake** 3.16 or later
- **C++17 compatible compiler**:
  - **Windows**: Visual Studio 2019 or later
  - **macOS**: Xcode Command Line Tools (clang++)
  - **Linux**: GCC 7+ or Clang 5+
- **AI/ML Dependencies** (for $MODEL functionality):
  - **Linux**: OpenMP support (`libomp-dev` on Ubuntu/Debian, `libgomp` on CentOS/RHEL) - requires separate library linking
  - **macOS**: Metal framework (included with Xcode) - uses hardware acceleration
  - **Windows**: OpenMP support (included with Visual Studio 2019+ or LLVM/Clang) - built into MSVC runtime, no separate library needed
    - **Debug Support**: Debug LLAMA libraries are available in `source/llama-lib/win-amd64-debug/` for Visual Studio debugging sessions

### Build Steps

1. **Navigate to the bin directory:**
   ```bash
   cd bin
   ```

2. **Optional: Rebuild LLAMA libraries with OpenMP (Windows only):**
   If you need to rebuild LLAMA libraries with OpenMP support or for debugging:
   ```bash
   python ../scripts/build_llama_windows.py
   ```
   This creates both Release and Debug versions with full CPU optimizations.

3. **Create and enter build directory:**
   ```bash
   mkdir build
   cd build
   ```

4. **Configure with CMake:**
   ```bash
   cmake ..
   ```

5. **Build the example:**
   ```bash
   cmake --build .
   ```

6. **Run the example:**
   ```bash
   ./grapa_example -c "2+3"
   ```

7. **Test AI/ML functionality:**
   ```bash
   ./grapa_example -c "m = \$MODEL(); m.info();"
   ```

## AI/ML Model Support

Grapa includes comprehensive AI/ML model support via LLAMA.cpp integration:

### Available Libraries
- **libllama.a** - Main LLAMA.cpp library
- **libggml.a** - Core GGML tensor library  
- **libggml-base.a** - Base GGML operations
- **libggml-cpu.a** - CPU-specific GGML operations
- **libmtmd.a** - Multi-threaded matrix operations
- **libcommon.a** - Common utilities and GGUF support

### Platform-Specific Features
- **macOS**: Metal GPU acceleration support
- **Linux**: OpenMP multi-threading support
- **Windows**: CPU-only implementation

### Usage Example
```cpp
#include "grapa/GrapaLink.h"
#include "grapa/GrapaValue.h"

// Create a model instance
GrapaValue model = GrapaValue::CreateObject("MODEL");

// Load a GGUF model file
GrapaValue result = model.Call("load", "path/to/model.gguf");

// Generate text
GrapaValue response = model.Call("generate", "Hello, how are you?");
```

## Using Grapa in Your Project

### Linking Against Grapa

The example demonstrates how to link against the Grapa static library. Key points:

1. **Include the headers:**
   ```cpp
   #include "grapa/GrapaLink.h"
   #include "grapa/GrapaValue.h"
   #include "grapa/GrapaSystem.h"
   ```

2. **Link against the static library:**
   - **Windows**: `grapa_static.lib`
   - **Unix-like**: `libgrapa_static.a`

3. **Required global variable:**
   ```cpp
   GrapaSystem* gSystem = NULL;  // Required by Grapa libraries
   ```

### CMake Integration

To use Grapa in your CMake project:

```cmake
# Find Grapa
find_package(Grapa REQUIRED)

# Link against Grapa
target_link_libraries(your_target PRIVATE Grapa::grapa)
```

Or manually specify the library:

```cmake
# Set include directories
target_include_directories(your_target PRIVATE 
    ${GRAPA_ROOT}/include
)

# Link against static library
target_link_libraries(your_target PRIVATE 
    ${GRAPA_ROOT}/platforms/${YOUR_PLATFORM}/libgrapa_static.a
)
```

## Installation

### Universal Installer

Use the included Python installer for easy installation:

```bash
# Install to default location
python3 install-grapa.py

# Install to custom location
python3 install-grapa.py --install-path ~/my-grapa

# Uninstall
python3 install-grapa.py --uninstall
```

### Manual Installation

1. **Copy the executable** to a directory in your PATH:
   ```bash
   # Unix-like systems
   sudo cp platforms/linux-arm64/grapa /usr/local/bin/
   
   # Windows
   copy platforms\win-amd64\grapa.exe C:\Windows\
   ```

2. **Copy the static library** to your project's library directory

3. **Copy the headers** to your project's include directory

## Platform Support

| Platform | Architecture | Status | AI/ML Support | Notes |
|----------|--------------|---------|---------------|-------|
| macOS | ARM64 | ✅ Supported | ✅ Complete | Apple Silicon with Metal GPU |
| macOS | AMD64 | ❌ Not supported | ❌ N/A | Use Apple Silicon |
| Linux | ARM64 | ✅ Supported | ✅ Complete | Ubuntu, Debian, etc. |
| Linux | AMD64 | ✅ Supported | ⏳ Pending | Libraries need to be built |
| AWS Linux | ARM64 | ✅ Supported | ⏳ Pending | Libraries need to be built |
| AWS Linux | AMD64 | ✅ Supported | ⏳ Pending | Libraries need to be built |
| Windows | AMD64 | ✅ Supported | ✅ Complete | Visual Studio 2019+ |

## Troubleshooting

### Common Issues

1. **"Grammar not available" error:**
   - Ensure you're using the static library, not shared library
   - Check that all required 3rd party libraries are available

2. **Linker errors:**
   - Verify the static library exists for your platform
   - Ensure all 3rd party dependencies are linked

3. **Runtime errors:**
   - Check that `gSystem` is properly initialized
   - Verify platform compatibility

4. **AI/ML model errors:**
   - **Linux**: Ensure OpenMP is installed (`libomp-dev` on Ubuntu/Debian)
   - **Missing libraries**: Verify all LLAMA.cpp libraries are present in `lib/{platform}/`
   - **Model loading**: Check that GGUF model files are accessible and valid
   - **OpenMP errors**: Install `libomp-dev` (Ubuntu/Debian) or `libgomp` (CentOS/RHEL)

### Getting Help

- Check the [Grapa documentation](https://grapa.dev)
- Review the example code in `main.cpp`
- Ensure you're using the correct platform and architecture

## Development

### Building from Source

To build Grapa from source:

```bash
# Clone the repository
git clone https://github.com/your-org/grapa.git
cd grapa

# Build for current platform
python3 build.py

# Build with Python extension
python3 build.py --python
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on multiple platforms
5. Submit a pull request

## License

This development kit is provided under the same license as the main Grapa project.
