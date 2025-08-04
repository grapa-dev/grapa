# Grapa Build System

This directory now contains an automated build system that replaces the manual copy-paste process from `maintainers/BUILD_AND_DEPLOYMENT/BUILD.md`.

**IMPORTANT: This build system now supports cross-compilation and automated multi-platform builds via GitHub Actions. The local build system supports both current platform and cross-compilation.**

## Quick Start

### Build for Current Platform
```bash
# Auto-detects your platform and architecture
python3 build.py

# Or use the shell wrapper
./build.sh
```

### Cross-Compilation Build
```bash
# Build for specific target platform
python3 build.py --target-platform mac-amd64    # Cross-compile macOS AMD64 from ARM64
python3 build.py --target-platform linux-arm64  # Cross-compile Linux ARM64 from AMD64
python3 build.py --target-platform win-amd64    # Build Windows AMD64
```

### Build with Tests
```bash
python3 build.py --test
```

## Supported Platforms

| Platform | Architecture | Compiler | Cross-Compilation | Notes |
|----------|--------------|----------|-------------------|-------|
| Windows | AMD64 | Visual Studio (msbuild) | ✅ Yes | Requires VS 2022 |
| Mac | ARM64 | clang/clang++ | ✅ Yes | Apple Silicon (native) |
| Mac | AMD64 | clang/clang++ | ✅ Yes | Intel Mac (cross-compiled) |
| Linux | ARM64 | g++ | ✅ Yes | Ubuntu 24.04+ (native or QEMU emulation) |
| Linux | AMD64 | g++ | ✅ Yes | Ubuntu 24.04+ (native) |
| AWS | ARM64 | g++ | ✅ Yes | Amazon Linux (native) |
| AWS | AMD64 | g++ | ✅ Yes | Amazon Linux (native) |

## What It Does

The build system automatically:

1. **Detects Platform**: Auto-detects your current platform and architecture
2. **Builds Executable**: Creates the main `grapa` executable (or `grapa.exe` on Windows)
3. **Builds Libraries**: Creates both static (`.a`) and shared (`.so`) libraries
4. **Packages**: Creates distribution packages in the `bin/` directory
5. **Python Package**: Builds and installs the Python package
6. **Tests**: Optionally runs the test suite
7. **Cleanup**: Removes build artifacts

## Recent Improvements (v0.0.194+)

### Cross-Compilation Support
- ✅ **Platform Normalization**: Fixed `win-amd64` → `windows-amd64` mapping
- ✅ **Native ARM64 Compilation**: Uses QEMU emulation instead of cross-compilation for Linux ARM64
- ✅ **Explicit Target Platform**: `--target-platform` option for precise cross-compilation control

### Static Library Improvements
- ✅ **Fully Static Libraries**: Static libraries now include all dependencies (OpenSSL, FLTK, BLST, PCRE2)
- ✅ **Self-Contained**: No external dependencies required for static libraries
- ✅ **Python Extension Ready**: Perfect for `setup.py` which only links against `libgrapa.a`

### Linker Fixes
- ✅ **Proper Library Ordering**: All `-l*` flags moved after `.a` files for correct symbol resolution
- ✅ **Separated Static/Shared Builds**: Clean separation to avoid linking conflicts
- ✅ **Explicit libbsd Linking**: Added `-lbsd` for Linux ARM64 builds

### Build System Robustness
- ✅ **GitHub Actions Integration**: Automated multi-platform builds and PyPI deployment
- ✅ **Artifact Management**: Proper collection and distribution of all platform artifacts
- ✅ **Error Handling**: Comprehensive error handling and diagnostic logging

## Prerequisites

### Windows
- Visual Studio 2022
- 7-Zip (for packaging)
- Python 3

### Mac
- Xcode Command Line Tools
- Python 3
- Code signing certificate (optional)

### Linux (Ubuntu)
- g++ with C++17 support
- Python 3
- X11 development libraries
- OpenSSL development libraries

### AWS (Amazon Linux)
- g++ with C++17 support
- Python 3
- X11 development libraries
- OpenSSL development libraries

## Usage Examples

### Development Workflow
```bash
# Build for your current platform
python3 build.py --test

# This will:
# 1. Build the executable and libraries
# 2. Create distribution packages
# 3. Build Python package
# 4. Run all tests
```

### Release Build
```bash
# Build for current platform
python3 build.py

# This creates packages for your current platform:
# - grapa-windows-amd64.zip (on Windows)
# - grapa-mac-arm64.tar.gz (on Mac ARM64)
# - grapa-mac-amd64.tar.gz (on Mac AMD64)
# - grapa-linux-arm64.tar.gz (on Linux ARM64)
# - grapa-linux-amd64.tar.gz (on Linux AMD64)
# - grapa-aws-arm64.tar.gz (on AWS ARM64)
# - grapa-aws-amd64.tar.gz (on AWS AMD64)
```

## File Structure

```
grapa/
├── build.py              # Main build script
├── build.sh              # Shell wrapper
├── BUILD_README.md       # This file
├── bin/                  # Distribution packages
│   ├── grapa-windows-amd64.zip
│   ├── grapa-mac-arm64.tar.gz
│   ├── grapa-linux-amd64.tar.gz
│   └── ...
├── source/
│   ├── grapa-lib/        # Built libraries
│   │   ├── mac-arm64/
│   │   ├── linux-amd64/
│   │   └── ...
│   └── grapa-other/      # Shared libraries
│       ├── mac-arm64/
│       └── ...
└── dist/                 # Python packages
```

## Troubleshooting

### Common Issues

1. **Missing Dependencies**
   ```bash
   # Ubuntu/Debian
   sudo apt install build-essential python3-dev
   
   # Mac
   xcode-select --install
   
   # AWS
   sudo yum groupinstall "Development Tools"
   ```

2. **C++17 Not Supported**
   ```bash
   # Ubuntu 20.04
   sudo add-apt-repository ppa:ubuntu-toolchain-r/test
   sudo apt install g++-13
   sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
   ```

3. **Visual Studio Not Found**
   - Install Visual Studio 2022
   - Run from "x64 Native Tools Command Prompt for VS 2022"

4. **Code Signing Issues (Mac)**
   - Remove codesign commands from build script if you don't have certificates
   - Or obtain developer certificates from Apple

### Getting Help

```bash
# Show help
python3 build.py --help
```

## Migration from Manual Build

### Before (Manual)
```bash
# Copy-paste from BUILD.md
rm grapa
clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3 
clang++ -Isource source/main.cpp source/grapa/*.cpp utf8proc.o source/openssl-lib/mac-arm64/*.a source/fl-lib/mac-arm64/*.a source/blst-lib/mac-arm64/*.a source/pcre2-lib/mac-arm64/libpcre2-8.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -o grapa
# ... many more commands
```

### After (Automated)
```bash
# Single command
python3 build.py --test
```

## Contributing

To modify the build system:

1. Edit `build.py` for build logic changes
2. Update `BUILD_README.md` for documentation
3. Test on multiple platforms
4. Update `maintainers/BUILD_AND_DEPLOYMENT/BUILD.md` if needed

## Notes

- The build system respects your existing library structure in `source/*-lib/`
- It automatically handles platform-specific compiler flags and libraries
- AWS builds use the same commands as Linux but with different library paths
- All build artifacts are cleaned up automatically
- Python packages are built and installed automatically
- **This script builds only for the current platform. For multi-platform builds, run the script on each target platform.** 