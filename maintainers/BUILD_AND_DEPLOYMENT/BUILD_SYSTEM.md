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

# Cross-compilation for specific platform
python3 build.py --target-platform mac-amd64    # Cross-compile macOS AMD64 from ARM64
python3 build.py --target-platform linux-arm64  # Cross-compile Linux ARM64 from AMD64
python3 build.py --target-platform win-amd64    # Build Windows AMD64

# Clean build artifacts
python3 build.py --clean

# Build Python package only
python3 build.py --python-only

# Build executable only (skip libraries, Python package)
python3 build.py --exe-only

# Build libraries only (skip executable, Python package)
python3 build.py --lib-only

# Preserve dist/ directory after build
python3 build.py --preserve-dist
```

#### **Platform Detection**
The build system automatically detects:
- Operating system (macOS, Linux, Windows)
- Architecture (ARM64, AMD64)
- Available compilers (gcc, clang, msvc)
- Required dependencies

#### **Build Artifacts**
- **Static libraries**: `libgrapa.a` (Unix), `grapalib.lib` (Windows) - **Fully static with all dependencies embedded**
- **Shared libraries**: `libgrapa.so` (Linux), `libgrapa.dylib` (macOS)
- **Python package**: `grapapy-0.0.25.tar.gz`
- **Executable**: `grapa` binary

#### **Recent Technical Improvements (v0.0.194+)**

##### **Cross-Compilation Enhancements**
- **Platform Normalization**: Fixed `win-amd64` → `windows-amd64` mapping for proper platform detection
- **Native ARM64 Compilation**: Linux ARM64 uses QEMU emulation instead of cross-compilation to avoid linker issues
- **Explicit Target Platform**: `--target-platform` option provides precise control over cross-compilation targets

##### **Static Library Architecture**
- **Fully Self-Contained**: Static libraries include all dependencies (OpenSSL, FLTK, BLST, PCRE2, utf8proc)
- **No Runtime Dependencies**: Perfect for Python extension which only links against `libgrapa.a`
- **Cross-Platform Compatibility**: No dynamic linking issues or version conflicts

##### **Linker Optimizations**
- **Proper Library Ordering**: All `-l*` flags moved after `.a` files for correct symbol resolution
- **Clean Build Separation**: Static and shared library builds are completely separate to avoid conflicts
- **Explicit Dependencies**: Added `-lbsd` for Linux ARM64 builds to resolve missing symbols

##### **Build System Robustness**
- **GitHub Actions Integration**: Automated multi-platform builds with proper artifact collection
- **Comprehensive Error Handling**: Diagnostic logging and graceful failure recovery
- **Artifact Management**: Proper distribution of platform-specific compressed files and libraries

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

---

## 🐛 **Troubleshooting: Git/GitHub Case-Sensitivity Issue (Uppercase/Lowercase File Names)**

### **Scenario: Duplicate Uppercase/Lowercase Files in Documentation**

**Problem:**
- On case-insensitive filesystems (macOS, Windows), renaming a file (e.g., `README.md` → `readme.md`) may not be detected by Git as a change.
- When pushed to GitHub (which is case-sensitive), both the uppercase and lowercase versions can exist in the same directory.
- This can result in both `README.md` and `readme.md` (or similar pairs) appearing in the remote repository, even if only one is visible locally.

**Symptoms:**
- GitHub web UI shows both uppercase and lowercase versions of a file in the same directory ([example](https://github.com/grapa-dev/grapa/tree/main/docs/docs)).
- Local filesystem only shows one version (usually the most recently checked out or updated).
- Cloning on a case-insensitive system may result in only one file, or unpredictable behavior.

### **How to Detect**
- Use `git ls-tree -r HEAD --name-only | grep '/[A-Z]'` to list all files with uppercase letters in their names as tracked by Git.
- Compare with local directory listing to see if both versions exist.
- Check the GitHub web UI for duplicate files.

### **How to Fix**
1. **For each uppercase file:**
   - Check if a lowercase version exists and is up to date.
   - If so, you can safely delete the uppercase file from the repository.
2. **Delete the uppercase file using Git:**
   ```sh
   git rm path/to/UPPERCASE_FILE.md
   git commit -m "Remove obsolete uppercase file to enforce lowercase standard"
   git push
   ```
3. **If the lowercase file is missing locally:**
   - Restore it from a previous commit or from the remote repository before deleting the uppercase version.
4. **If both files are needed:**
   - Manually merge content as needed before deleting.

### **Prevention**
- Always use lowercase file names for documentation and source files.
- When renaming files for case changes, use `git mv` to ensure Git tracks the change:
  ```sh
  git mv README.md readme.md
  git commit -m "Rename README.md to readme.md for case consistency"
  git push
  ```
- Regularly scan the repository for case issues, especially after bulk renames or migrations.

### **References**
- [GitHub repo directory view](https://github.com/grapa-dev/grapa/tree/main/docs/docs)
- [Stack Overflow: Git and case-insensitive filesystems](https://stackoverflow.com/questions/6839694/how-to-delete-a-git-file-case-insensitively)

--- 