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

1. **First time setup**: [Dependencies](DEPENDENCIES.md) - Install required libraries
2. **Build process**: [Build Instructions](BUILD.md) - Complete build guide
3. **Cloud deployment**: [AWS/Docker Setup](SETUPAWSDOCKER.md) - Cloud deployment
4. **Production**: [Deployment](DEPLOYMENT.md) - Production deployment (coming soon)
5. **Packaging**: [Packaging](PACKAGING.md) - Package manager support (coming soon)

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

bin/                 # Compiled executables
```

## Platform Support

| Platform | Architecture | Status | Notes |
|----------|-------------|--------|-------|
| Windows | AMD64 | ✅ | Visual Studio 2022 |
| Windows | ARM64 | ✅ | Visual Studio 2022 |
| macOS | AMD64 | ✅ | Xcode/Clang |
| macOS | ARM64 | ✅ | Xcode/Clang |
| Linux | AMD64 | ✅ | GCC/Clang |
| Linux | ARM64 | ✅ | GCC/Clang |
| AWS | AMD64 | ✅ | Docker support |
| AWS | ARM64 | ✅ | Docker support |

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

## Next Steps

- [ ] Package manager support (Homebrew, apt-get, Chocolatey)
- [ ] CI/CD pipeline setup
- [ ] Automated testing in build process
- [ ] Docker multi-stage builds
- [ ] Cloud deployment automation

---

**For users**: If you're looking for installation instructions, see the main [Getting Started](../GETTING_STARTED.md) guide. 