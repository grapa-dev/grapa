# Grapa Development Kit

This directory contains everything you need to embed the Grapa library into your own C++ projects and install Grapa on your system.

## Contents

- `include/` - Grapa public API headers
- `main.cpp` - Example C++ source code demonstrating Grapa embedding
- `CMakeLists.txt` - Cross-platform build configuration
- `install-grapa.py` - Universal installation script for all platforms
- Platform-specific folders containing shared libraries and executables

## Quick Installation

### Universal Installer (Recommended)

The `install-grapa.py` script automatically detects your platform and installs Grapa:

```bash
# Install Grapa
python3 install-grapa.py

# Force installation (no prompts)
python3 install-grapa.py --force

# Uninstall Grapa
python3 install-grapa.py --uninstall

# Custom installation path
python3 install-grapa.py --install-path /custom/path
```

### Platform-Specific Installation

**Windows:**
```powershell
# Run PowerShell as Administrator
.\install-grapa.ps1

# Or use the universal installer
python install-grapa.py
```

**macOS/Linux:**
```bash
# Run with sudo for system-wide installation
sudo ./install-grapa.sh

# Or use the universal installer
python3 install-grapa.py
```

## Installation Features

The universal installer provides:

- **Automatic platform detection** (Windows, macOS, Linux)
- **Smart installation** with version detection and confirmation prompts
- **Clean replacement** of existing installations
- **PATH management** for command-line access
- **Comprehensive error handling** and user feedback
- **File validation** before installation
- **Easy uninstallation** with cleanup

### Installation Locations

**Windows:**
- Executable: `C:\Program Files\Grapa\bin\grapa.exe`
- Static Library: `C:\Program Files\Grapa\lib\grapa_static.lib`
- Shared Library: `C:\Program Files\Grapa\lib\grapa.dll`

**macOS/Linux:**
- Executable: `/usr/local/bin/grapa`
- Static Library: `/usr/local/lib/libgrapa_static.a`
- Shared Library: `/usr/local/lib/libgrapa.dylib` (macOS) or `/usr/local/lib/libgrapa.so` (Linux)

## Building the Example

### Prerequisites

- **CMake 3.16 or later**
- **C++17 compatible compiler**
  - Windows: Visual Studio 2017 or later
  - macOS: Xcode Command Line Tools (clang++)
  - Linux: GCC 7+ or Clang 5+

### Build Commands

#### All Platforms (Recommended)
```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .
```

#### Platform-Specific Commands

**Windows:**
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

**macOS:**
```bash
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
```

**Linux:**
```bash
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
```

### Running the Example

After building, you can run the example:

**Windows:**
```cmd
.\grapa_example.exe "2+3"
```

**macOS/Linux:**
```bash
./grapa_example "2+3"
```

## Example Usage

The `main.cpp` file demonstrates how to:
- Initialize the Grapa engine
- Execute Grapa commands
- Handle output and errors
- Link against the Grapa shared library

## Linking in Your Own Projects

### CMake (Recommended)
```cmake
# Find the Grapa library
find_library(GRAPA_LIB 
    NAMES grapa libgrapa
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/path/to/grapa/bin/${CMAKE_SYSTEM_NAME}
    NO_DEFAULT_PATH
)

# Include Grapa headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/path/to/grapa/bin/include)

# Link against Grapa
target_link_libraries(your_target ${GRAPA_LIB})
```

### Manual Compilation
```bash
# Include Grapa headers
g++ -I./include your_file.cpp -L./path/to/lib -lgrapa -o your_program
```

## Platform Support

- **Windows AMD64**: `grapa.dll` + `grapa_example.exe`
- **macOS ARM64**: `libgrapa.dylib` + `grapa_example`
- **Linux ARM64**: `libgrapa.so` + `grapa_example`

## Troubleshooting

### Installation Issues

**Permission Errors:**
- Windows: Run PowerShell as Administrator
- macOS/Linux: Use `sudo` for system-wide installation

**PATH Issues:**
- Windows: Restart terminal after installation
- macOS/Linux: Run `source ~/.zshrc` or `source ~/.bashrc`

**Missing Files:**
- Ensure all required files are in the platform-specific directory
- Check that the universal installer can detect your platform

### Library Not Found
- Ensure the shared library is in the correct platform folder
- Check that RPATH settings are correct for your platform
- Verify the library path in CMakeLists.txt matches your setup

### Compilation Errors
- Ensure you have a C++17 compatible compiler
- Check that all required headers are in the `include/` directory
- Verify CMake version is 3.16 or later

### Runtime Errors
- Ensure the shared library is accessible at runtime
- Check that the executable has the correct permissions
- Verify platform compatibility

## Manual Installation

If you prefer to install manually instead of using the installer:

### Windows
```powershell
# Create directories
New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\bin"
New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\lib"

# Copy files
Copy-Item -Path "grapa.exe" -Destination "C:\Program Files\Grapa\bin\" -Force
Copy-Item -Path "grapa_static.lib" -Destination "C:\Program Files\Grapa\lib\" -Force
if (Test-Path "grapa.dll") {
    Copy-Item -Path "grapa.dll" -Destination "C:\Program Files\Grapa\lib\" -Force
}

# Add to system PATH
$currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
$grapaPath = "C:\Program Files\Grapa\bin"
if ($currentPath -notlike "*$grapaPath*") {
    [Environment]::SetEnvironmentVariable('PATH', $currentPath + ";" + $grapaPath, 'Machine')
}
```

### macOS/Linux
```bash
# Create directories
sudo mkdir -p /usr/local/bin /usr/local/lib

# Copy files
sudo cp grapa /usr/local/bin/
sudo cp libgrapa_static.a /usr/local/lib/
if [ -f "libgrapa.dylib" ] || [ -f "libgrapa.so" ]; then
    sudo cp libgrapa.* /usr/local/lib/
fi

# Set permissions
sudo chmod +x /usr/local/bin/grapa

# Add to PATH
echo 'export PATH="$PATH:/usr/local/bin"' >> ~/.zshrc
source ~/.zshrc
```

## Verification

After installation, verify that Grapa is working:

```bash
# Check if grapa is available
grapa --version

# Test basic functionality
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

## Support

For issues or questions about embedding Grapa, please refer to the main project documentation or create an issue in the project repository.
