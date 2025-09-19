# Grapa Development Kit

This directory contains everything you need to build and run Grapa applications using the Grapa static library.

## Directory Structure

```
bin/
├── include/grapa/          # Grapa public API headers
├── lib/                    # 3rd party static libraries
│   ├── mac-arm64/         # Mac ARM64 libraries
│   ├── linux-arm64/       # Linux ARM64 libraries
│   ├── linux-amd64/       # Linux AMD64 libraries
│   ├── aws-arm64/         # AWS ARM64 libraries
│   ├── aws-amd64/         # AWS AMD64 libraries
│   └── win-amd64/         # Windows AMD64 libraries
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

### Build Steps

1. **Navigate to the bin directory:**
   ```bash
   cd bin
   ```

2. **Create and enter build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the example:**
   ```bash
   cmake --build .
   ```

5. **Run the example:**
   ```bash
   ./grapa_example -c "2+3"
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

| Platform | Architecture | Status | Notes |
|----------|--------------|---------|-------|
| macOS | ARM64 | ✅ Supported | Apple Silicon only |
| macOS | AMD64 | ❌ Not supported | Use Apple Silicon |
| Linux | ARM64 | ✅ Supported | Ubuntu, Debian, etc. |
| Linux | AMD64 | ✅ Supported | Ubuntu, Debian, etc. |
| AWS Linux | ARM64 | ✅ Supported | Amazon Linux 2/3 |
| AWS Linux | AMD64 | ✅ Supported | Amazon Linux 2/3 |
| Windows | AMD64 | ✅ Supported | Visual Studio 2019+ |

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
