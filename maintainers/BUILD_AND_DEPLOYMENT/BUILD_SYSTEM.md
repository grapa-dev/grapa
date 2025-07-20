# Grapa Build System Reference

**Last Updated**: July 19, 2024  
**Status**: Complete build system documentation  
**Scope**: All build processes, debugging, and dependencies

> **NOTE**: This is the single source of truth for build system information. All other build files are deprecated.

---

## 🎯 **Build System Overview**

### **Supported Platforms**
- **Mac**: ARM64 and AMD64 builds working ✅
- **Linux**: ARM64 and AMD64 builds working ✅
- **AWS**: ARM64 and AMD64 builds working ✅
- **Windows**: AMD64 only (ARM64 explicitly excluded) ✅

### **Build System Features**
- Multi-platform detection and build
- Static and shared library creation
- Python package integration
- Comprehensive cleanup
- Platform-specific optimizations

---

## 🚀 **Build Process**

### **Automated Build System**
The primary build system is `build.py` (698 lines) which provides:

#### **Core Commands**
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

#### **Platform Detection**
The build system automatically detects:
- Operating system (macOS, Linux, Windows)
- Architecture (ARM64, AMD64)
- Available compilers (gcc, clang, msvc)
- Required dependencies

#### **Build Artifacts**
- **Static libraries**: `libgrapa.a` (Unix), `grapalib.lib` (Windows)
- **Shared libraries**: `libgrapa.so` (Linux), `libgrapa.dylib` (macOS)
- **Python package**: `grapapy-0.0.25.tar.gz`
- **Executable**: `grapa` binary

---

## 🔧 **Dependencies**

### **Core Dependencies**
- **FLTK (GUI framework)**: Version 1.3.11
- **OpenSSL (cryptography)**: Version 1.1.1w
- **BLST (BLS signatures)**: Latest master
- **PCRE2 (regex)**: Latest version
- **utf8proc (Unicode processing)**: Latest version

### **Platform-Specific Dependencies**

#### **macOS**
- **Xcode Command Line Tools**: Required for compilation
- **Homebrew**: Optional for dependency management
- **Python 3.7+**: Required for Python integration

#### **Linux**
- **GCC 7+ or Clang 6+**: Required for compilation
- **Make**: Required for build process
- **Python 3.7+**: Required for Python integration
- **Development headers**: `build-essential` package

#### **Windows**
- **Visual Studio 2019+**: Required for compilation
- **Windows SDK**: Required for Windows API
- **Python 3.7+**: Required for Python integration
- **CMake**: Required for build configuration

#### **AWS**
- **Amazon Linux 2**: Tested platform
- **GCC 7+**: Required for compilation
- **Python 3.7+**: Required for Python integration

### **Dependency Management**
Dependencies are managed through:
- **Pre-built libraries**: Stored in `source/*-lib/` directories
- **Source downloads**: Automatic download and compilation
- **Version pinning**: Specific versions for stability

---

## 🐛 **Debugging Guide**

### **Common Build Issues**

#### **1. Compiler Not Found**
**Symptoms**: `error: command not found: gcc`
**Solutions**:
```bash
# macOS
xcode-select --install

# Linux
sudo apt-get install build-essential

# Windows
# Install Visual Studio with C++ workload
```

#### **2. Missing Dependencies**
**Symptoms**: `fatal error: 'fltk/Fl.H' file not found`
**Solutions**:
```bash
# Clean and rebuild
python3 build.py --clean
python3 build.py

# Manual dependency installation
# See dependency-specific instructions below
```

#### **3. Platform Detection Issues**
**Symptoms**: Wrong platform detected or build fails
**Solutions**:
```bash
# Force platform
python3 build.py --platform mac-arm64

# Check platform detection
python3 build.py --debug
```

#### **4. Python Package Issues**
**Symptoms**: `pip install` fails or package not found
**Solutions**:
```bash
# Rebuild Python package
python3 build.py --python-only

# Install locally
pip install -e .

# Check package contents
tar -tzf grapapy-0.0.25.tar.gz
```

### **Debug Output**
Enable debug output for troubleshooting:
```bash
# Enable debug mode
export GRAPA_DEBUG=1
python3 build.py

# Verbose output
python3 build.py --verbose
```

### **Log Files**
Build logs are stored in:
- **Build logs**: `build.log`
- **Error logs**: `error.log`
- **Platform logs**: `platform.log`

---

## 🏗️ **Build Configuration**

### **CMake Configuration**
The build system uses CMake for configuration:

#### **Key CMake Variables**
```cmake
# Platform detection
CMAKE_SYSTEM_NAME
CMAKE_SYSTEM_PROCESSOR

# Compiler settings
CMAKE_C_COMPILER
CMAKE_CXX_COMPILER

# Library paths
FLTK_DIR
OPENSSL_ROOT_DIR
BLST_DIR
PCRE2_DIR
UTF8PROC_DIR
```

#### **Platform-Specific Settings**

##### **macOS**
```cmake
# ARM64 settings
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch arm64")

# AMD64 settings
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64")
```

##### **Linux**
```cmake
# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Library linking
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ORIGIN")
```

##### **Windows**
```cmake
# Visual Studio settings
set(CMAKE_GENERATOR "Visual Studio 16 2019")
set(CMAKE_GENERATOR_PLATFORM "x64")

# Library linking
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
```

---

## 📦 **Python Package Build**

### **Package Configuration**
The Python package is configured in `setup.py`:

#### **Key Settings**
```python
# Package metadata
name="grapapy"
version="0.0.25"
description="Grapa language Python integration"
long_description=open("README.md").read()

# Dependencies
install_requires=[
    "numpy>=1.19.0",
    "pandas>=1.3.0"
]

# Build requirements
setup_requires=[
    "pybind11>=2.6.0",
    "setuptools>=45.0.0"
]
```

#### **Build Process**
```bash
# Build package
python3 build.py --python-only

# Install package
pip install grapapy-0.0.25.tar.gz

# Development install
pip install -e .
```

### **Package Contents**
The Python package includes:
- **GrapaPy module**: Python bindings for Grapa
- **Documentation**: API reference and examples
- **Test suite**: Python integration tests
- **Examples**: Sample scripts and notebooks

---

## 🔄 **Continuous Integration**

### **GitHub Actions**
The build system includes GitHub Actions for:
- **Multi-platform builds**: macOS, Linux, Windows
- **Python package testing**: Installation and basic functionality
- **Documentation deployment**: Automatic GitHub Pages updates

### **Build Matrix**
```yaml
# Supported platforms
platforms:
  - macos-latest
  - ubuntu-latest
  - windows-latest

# Python versions
python-versions:
  - "3.8"
  - "3.9"
  - "3.10"
  - "3.11"
```

---

## 🚫 **Explicitly Excluded**

### **Windows ARM64 Support**
- **Decision**: Not implementing Windows ARM64 support
- **Reason**: Complexity and limited demand
- **Build System**: No Windows ARM64 build paths
- **Documentation**: No Windows ARM64 instructions
- **Project Files**: No Windows ARM64 Visual Studio projects

### **Legacy Platform Support**
- **Windows 32-bit**: Not supported
- **macOS 10.14 and earlier**: Not tested
- **Linux distributions older than 2018**: Not tested

---

## 📊 **Performance Optimization**

### **Build Performance**
- **Parallel builds**: Uses `make -j$(nproc)` for parallel compilation
- **Incremental builds**: Only rebuilds changed components
- **Caching**: Caches compiled dependencies

### **Runtime Performance**
- **Static linking**: Reduces runtime dependencies
- **Optimization flags**: Uses `-O2` for release builds
- **Profile-guided optimization**: Available for performance-critical builds

---

## 🔧 **Maintenance Tasks**

### **Regular Maintenance**
- **Dependency updates**: Monthly security updates
- **Platform testing**: Quarterly platform verification
- **Performance monitoring**: Continuous performance tracking

### **Troubleshooting**
- **Build failures**: Check logs and platform compatibility
- **Performance issues**: Profile and optimize critical paths
- **Dependency conflicts**: Resolve version conflicts

---

## 📞 **Support and Coordination**

This document should be updated whenever:
- Build system changes are made
- New platforms are added/removed
- Dependencies are updated
- Build process improvements are implemented

**Maintainers**: Update this file to reflect current build system status and procedures.

---

**Last Updated**: July 19, 2024  
**Build System Version**: 0.0.39c  
**Status**: All platforms working correctly 