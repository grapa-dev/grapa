# Build and Deployment - System Setup and Deployment

## Overview

This directory contains all build system and deployment documentation for the Grapa project, including build instructions, dependency management, packaging, and production deployment guides.

## Purpose

**BUILD_AND_DEPLOYMENT** provides comprehensive guidance for building, packaging, and deploying Grapa across all supported platforms.

## Quick Navigation

### Build System
- **[BUILD_SYSTEM.md](BUILD_SYSTEM.md)** - **COMPREHENSIVE**: Complete build system reference with all platform instructions, dependencies, and testing commands

### Deployment
- **[DOCUMENTATION_DEPLOYMENT.md](DOCUMENTATION_DEPLOYMENT.md)** - Documentation deployment process and procedures
- **[SETUPAWSDOCKER.md](SETUPAWSDOCKER.md)** - AWS and Docker setup instructions
- **[BUILD_DOCKER_MANUAL.md](BUILD_DOCKER_MANUAL.md)** - Docker build manual and containerization guide

## Supported Platforms

### ✅ **Working Platforms**
- **Mac**: ARM64 and AMD64 builds working
- **Linux**: ARM64 and AMD64 builds working  
- **AWS**: ARM64 and AMD64 builds working
- **Windows**: AMD64 only (ARM64 explicitly excluded)

### 🚫 **Explicitly Excluded**
- **Windows ARM64**: Not implementing due to complexity and limited demand

## Build System Features

### Automated Build System
The primary build system is `build.py` which provides:

#### Core Commands
```bash
# Build for current platform
python3 build.py

# Build for specific platform
python3 build.py --platform mac-arm64
python3 build.py --platform linux-amd64
python3 build.py --platform windows-amd64

# Clean build artifacts
python3 build.py --clean

# Build Python package only
python3 build.py --python-only

# Build all platforms
python3 build.py --all-platforms
```

#### Platform Detection
- Operating system (macOS, Linux, Windows)
- Architecture (ARM64, AMD64)
- Available compilers (gcc, clang, msvc)
- Required dependencies

#### Build Artifacts
- **Static libraries**: `libgrapa.a` (Unix), `grapalib.lib` (Windows)
- **Shared libraries**: `libgrapa.so` (Linux), `libgrapa.dylib` (macOS)
- **Python package**: `grapapy-0.0.25.tar.gz`
- **Executable**: `grapa` binary

## Dependencies

### Core Dependencies
- **FLTK (GUI framework)**: Version 1.3.11
- **OpenSSL (cryptography)**: Version 1.1.1w
- **BLST (BLS signatures)**: Latest master
- **PCRE2 (regex)**: Latest version
- **utf8proc (Unicode processing)**: Latest version

### Platform-Specific Dependencies

#### macOS
- **Xcode Command Line Tools**: Required for compilation
- **Homebrew**: Optional for dependency management
- **Python 3.7+**: Required for Python integration

#### Linux
- **GCC 7+ or Clang 6+**: Required for compilation
- **Make**: Required for build process
- **Python 3.7+**: Required for Python integration
- **Development headers**: `build-essential` package

#### Windows
- **Visual Studio 2019+**: Required for compilation
- **Windows SDK**: Required for Windows API
- **Python 3.7+**: Required for Python integration
- **CMake**: Required for build configuration

## Deployment

### Documentation Deployment
- **GitHub Pages**: Automatic deployment from `docs/` directory
- **Build Scripts**: `scripts/deploy_docs.sh` (Linux/Mac) and `scripts/deploy_docs.ps1` (Windows)
- **Live Site**: https://grapa-dev.github.io/grapa/

### Package Deployment
- **Python Package**: Available via `pip install grapapy`
- **Binary Distributions**: Platform-specific executables
- **Docker Images**: Containerized deployments

## Troubleshooting

### Common Build Issues
1. **Compiler Not Found**: Install appropriate development tools
2. **Missing Dependencies**: Clean and rebuild with `python3 build.py --clean`
3. **Platform Detection Issues**: Force platform with `--platform` flag
4. **Python Package Issues**: Rebuild with `python3 build.py --python-only`

### Debug Output
Enable debug output for troubleshooting:
```bash
# Enable debug mode
export GRAPA_DEBUG=1
python3 build.py

# Verbose output
python3 build.py --verbose
```

## Maintenance

### Regular Tasks
- **Dependency updates**: Monthly security updates
- **Platform testing**: Quarterly platform verification
- **Performance monitoring**: Continuous performance tracking

### Update Requirements
This documentation should be updated whenever:
- Build system changes are made
- New platforms are added/removed
- Dependencies are updated
- Build process improvements are implemented

## Related Documentation

- **Current Status**: `../PROJECT_MANAGEMENT/CURRENT_STATUS.md` - Project priorities
- **Implementation Details**: `../IMPLEMENTATION/` - Technical implementation
- **Testing**: `../TESTING_AND_QUALITY/` - Quality assurance
- **Development**: `../DEVELOPMENT/` - Development workflow

---

*This directory provides comprehensive build and deployment guidance. For current implementation details, see the IMPLEMENTATION directory.* 