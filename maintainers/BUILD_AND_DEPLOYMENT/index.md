---
tags:
  - maintainer
  - lowlevel
  - build
---

# Build & Deployment

This section contains all documentation related to building, packaging, and deploying Grapa.

## Overview

The build system supports multiple platforms and deployment scenarios:
- **Platforms**: Windows, macOS, Linux (AMD64/ARM64)
- **Cloud**: AWS, Docker containers
- **Packaging**: Package manager support (planned)
- **Deployment**: Production deployment guides

## Quick Start

### 🚀 **Automated Build System** ✅ **COMPLETED**
The automated build system is now fully functional across all platforms:
```bash
# Build for current platform (auto-detected)
python3 build.py

# Build with tests
python3 build.py --test
```

**Status**: All 7 platforms working (Windows, Mac ARM64/AMD64, Linux ARM64/AMD64, AWS ARM64/AMD64)

See [BUILD_README.md](../../BUILD_README.md) for complete documentation.

### 📋 **Manual Build Process** (Legacy)
For reference or troubleshooting:
1. **First time setup**: [Dependencies](DEPENDENCIES.md) - Install required libraries
2. **Build process**: [Build Instructions](BUILD.md) - Complete manual build guide
3. **Cloud deployment**: [AWS/Docker Setup](SETUPAWSDOCKER.md) - Cloud deployment
4. **Production**: [Deployment](DEPLOYMENT.md) - Production deployment (coming soon)
5. **Packaging**: [Packaging](PACKAGING.md) - Package manager support (coming soon)
6. **Debugging**: [Build Debugging Guide](BUILD_DEBUGGING.md) - Troubleshooting and learnings

## Build System Architecture

```
source/
├── grapa/           # Core Grapa C++ implementation
├── FL/              # FLTK GUI library headers
├── openssl/         # OpenSSL cryptography headers
├── blst/            # BLS signature library headers
├── pcre2/           # PCRE2 regex library headers
└── utf8proc/        # Unicode processing library

lib/
├── grapa/           # Pre-compiled Grapa libraries (.grz files)
└── grapa/           # Source files (.grc files)

bin/                 # Compiled executables and packages
```

## Platform Support

| Platform | Architecture | Status | Build Method | Notes |
|----------|-------------|--------|--------------|-------|
| Windows | AMD64 | ✅ | `python3 build.py windows amd64` | Visual Studio 2022 |
| macOS | AMD64 | ✅ | `python3 build.py mac amd64` | Xcode/Clang |
| macOS | ARM64 | ✅ | `python3 build.py mac arm64` | Xcode/Clang |
| Linux | AMD64 | ✅ | `python3 build.py linux amd64` | GCC/Clang |
| Linux | ARM64 | ✅ | `python3 build.py linux arm64` | GCC/Clang |
| AWS | AMD64 | ✅ | `python3 build.py aws amd64` | Docker support |
| AWS | ARM64 | ✅ | `python3 build.py aws arm64` | Docker support |

## Dependencies

- **FLTK 1.3.11**: GUI framework
- **OpenSSL 1.1.1w**: Cryptography
- **BLST**: BLS signature library
- **PCRE2**: Regular expressions
- **utf8proc**: Unicode processing

See [Dependencies](DEPENDENCIES.md) for detailed installation instructions.

## Common Build Issues

### Windows
- **Visual Studio**: Ensure VS2022 with C++17 support
- **Paths**: Use forward slashes or escaped backslashes
- **Libraries**: Copy pre-built libraries to correct locations

### macOS
- **Xcode**: Install Xcode Command Line Tools
- **Architecture**: Use `-m64` flag for AMD64 builds
- **Frameworks**: Link CoreFoundation, AppKit, IOKit

### Linux
- **GCC/Clang**: Version 7+ with C++17 support
- **Libraries**: Install development packages
- **Threading**: Use `-pthread` flag

## Build System Comparison

| Feature | Automated Build | Manual Build |
|---------|----------------|--------------|
| **Ease of Use** | Single command | Copy-paste 50+ lines |
| **Error Handling** | Automatic fallbacks | Manual troubleshooting |
| **Cross-Platform** | Auto-detection | Platform-specific commands |
| **Python Package** | Automatic build/install | Manual steps |
| **Testing** | `--test` flag | Manual test execution |
| **Packaging** | Automatic | Manual tar/zip creation |
| **Cleanup** | Automatic | Manual cleanup |

## Next Steps

- [x] ✅ Automated build system - **COMPLETED**
- [x] ✅ All platform support (7 platforms)
- [x] ✅ Python package integration
- [x] ✅ AWS platform detection
- [x] ✅ Cross-platform consistency
- [ ] Package manager support (Homebrew, apt-get, Chocolatey)
- [ ] CI/CD pipeline setup
- [ ] Automated testing in build process
- [ ] Docker multi-stage builds
- [ ] Cloud deployment automation

---

**For users**: If you're looking for installation instructions, see the main [Getting Started](../GETTING_STARTED.md) guide. 