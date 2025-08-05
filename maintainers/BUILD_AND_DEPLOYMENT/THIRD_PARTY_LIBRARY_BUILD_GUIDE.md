# Third-Party Library Build Guide

This guide provides comprehensive instructions for building all third-party libraries used by Grapa across different platforms. These libraries are essential for the Grapa build system and must be compiled correctly for each target platform.

## Overview

Grapa depends on the following third-party libraries:
- **OpenSSL**: Cryptographic functions
- **FLTK**: GUI framework
- **PCRE2**: Regular expression support
- **BLST**: BLS signature support

## Platform-Specific Build Instructions

### Linux AMD64 (Native)

#### OpenSSL
```bash
cd dep/openssl-1.1.1w
./config -fPIC -std=c++17 no-shared
make
# Copy libcrypto.a and libssl.a to source/openssl-lib/linux-amd64/
```

#### FLTK
```bash
cd dep/fltk-1.3.11-source
NOCONFIGURE=1 ./autogen.sh
./configure --with-optim="-fPIC -std=c++17" --disable-shared
make
# Copy *.a files to source/fl-lib/linux-amd64/
```

#### PCRE2
```bash
cd dep/pcre2-10.45
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
make
# Copy libpcre2-8.a to source/pcre2-lib/linux-amd64/
```

#### BLST
```bash
cd dep/blst-master
make
# Copy libblst.a to source/blst-lib/linux-amd64/
```

### Linux ARM64 (Native)

#### OpenSSL
```bash
cd dep/openssl-1.1.1w
./config -fPIC -std=c++17 no-shared
make
# Copy libcrypto.a and libssl.a to source/openssl-lib/linux-arm64/
```

#### FLTK
```bash
cd dep/fltk-1.3.11-source
NOCONFIGURE=1 ./autogen.sh
./configure --with-optim="-fPIC -std=c++17" --disable-shared
make
# Copy *.a files to source/fl-lib/linux-arm64/
```

#### PCRE2
```bash
cd dep/pcre2-10.45
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
make
# Copy libpcre2-8.a to source/pcre2-lib/linux-arm64/
```

#### BLST
```bash
cd dep/blst-master
make
# Copy libblst.a to source/blst-lib/linux-arm64/
```

### macOS AMD64 (Native)

**Prerequisites:**
```bash
brew install llvm
```

**For Mac Intel (AMD64) Cross-Compilation:**
```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build && cd build
cmake -DLLVM_ENABLE_PROJECTS="libcxx;libcxxabi" -DCMAKE_BUILD_TYPE=Release ../llvm
make cxx
# Use the resulting *.a files in the build
```

#### OpenSSL
```bash
cd dep/openssl-1.1.1w
./config -fPIC -std=c++17 no-shared
make
# Copy libcrypto.a and libssl.a to source/openssl-lib/mac-amd64/
```

#### FLTK
```bash
cd dep/fltk-1.3.11-source
NOCONFIGURE=1 ./autogen.sh
./configure --with-optim="-fPIC -std=c++17" --disable-shared
make
# Copy *.a files to source/fl-lib/mac-amd64/
```

#### PCRE2
```bash
cd dep/pcre2-10.45
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
make
# Copy libpcre2-8.a to source/pcre2-lib/mac-amd64/
```

#### BLST
```bash
cd dep/blst-master
make
# Copy libblst.a to source/blst-lib/mac-amd64/
```

### macOS ARM64 (Native)

#### OpenSSL
```bash
cd dep/openssl-1.1.1w
./config -fPIC -std=c++17 no-shared
make
# Copy libcrypto.a and libssl.a to source/openssl-lib/mac-arm64/
```

#### FLTK
```bash
cd dep/fltk-1.3.11-source
NOCONFIGURE=1 ./autogen.sh
./configure --with-optim="-fPIC -std=c++17" --disable-shared
make
# Copy *.a files to source/fl-lib/mac-arm64/
```

#### PCRE2
```bash
cd dep/pcre2-10.45
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
make
# Copy libpcre2-8.a to source/pcre2-lib/mac-arm64/
```

#### BLST
```bash
cd dep/blst-master
make
# Copy libblst.a to source/blst-lib/mac-arm64/
```

### Windows AMD64 (Native)

**Prerequisites:**
- Install [Strawberry Perl](https://strawberryperl.com/)
- Install [NASM](https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/)
- Run "x64 Native Tools Command Prompt for VS 2022" in administrator mode

#### OpenSSL
```bash
cd dep/openssl-1.1.1w
# Use Visual Studio Developer Command Prompt
perl Configure VC-WIN64A
nmake
# Note: If compile freezes on building test app, this is OK - abort at that point
# Copy libcrypto_static.lib and libssl_static.lib to source/openssl-lib/win-amd64/
# Copy include/openssl to source/
```

#### FLTK
```bash
cd dep/fltk-1.3.11-source
# Use Visual Studio Developer Command Prompt
cmake -S . -B build_vs2022 -G "Visual Studio 17 2022" -A x64 -D BUILD_SHARED_LIBS=OFF -D OPTION_USE_SYSTEM_LIBJPEG=OFF -D OPTION_USE_SYSTEM_LIBPNG=OFF -D OPTION_USE_SYSTEM_ZLIB=OFF -D FLTK_BUILD_FLUID=OFF -D FLTK_BUILD_TEST=OFF -D FLTK_BUILD_EXAMPLES=OFF -D FLTK_MSVC_RUNTIME_DLL=OFF
cmake --build build_vs2022 --config Release
# Copy FL/* to source/FL
# Copy build_vs2022/FL/* to source/FL
# Copy build_vs2022/lib/Release/*.lib to source/fl-lib/win-amd64/
```

#### PCRE2
```bash
cd dep/pcre2-10.45
rmdir /s /q build_static
mkdir build_static
cd build_static
cmake -G "Unix Makefiles" -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DPCRE2_STATIC_RUNTIME=ON -DPCRE2_SUPPORT_UNICODE=ON -DCMAKE_BUILD_TYPE=Release ..
# Copy pcre2-8-static.lib to source/pcre2-lib/win-amd64/
```

#### BLST
```bash
cd prj/blst
build
# Copy bindings/*.h* and bindings/*.swg to source/blst
# Copy *.lib and *.pdb to source/blst-lib/win-amd64/
```

## Cross-Compilation Considerations

### Linux ARM64 Cross-Compilation (from AMD64)
For cross-compiling Linux ARM64 libraries from an AMD64 host:

```bash
# Install cross-compilation tools
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Set up cross-compilation environment
export CC=aarch64-linux-gnu-gcc
export CXX=aarch64-linux-gnu-g++
export AR=aarch64-linux-gnu-ar
export RANLIB=aarch64-linux-gnu-ranlib

# Then follow the Linux ARM64 build instructions above
```

### macOS AMD64 Cross-Compilation (from ARM64)
For cross-compiling macOS AMD64 libraries from an ARM64 Mac:

```bash
# Set up cross-compilation environment
export CC="clang -target x86_64-apple-macos10.9"
export CXX="clang++ -target x86_64-apple-macos10.9"
export AR="ar"
export RANLIB="ranlib"

# Then follow the macOS AMD64 build instructions above
```

## Critical Build Parameters

### C++17 Compatibility
All libraries must be built with C++17 compatibility to avoid `__isoc23_` symbol errors:
- **OpenSSL**: `-std=c++17`
- **FLTK**: `--with-optim="-fPIC -std=c++17"`
- **PCRE2**: Built with C++17 standard
- **BLST**: Built with C++17 standard

### Position Independent Code (PIC)
All libraries must be built with `-fPIC` for shared library compatibility:
- **OpenSSL**: `-fPIC`
- **FLTK**: `-fPIC`
- **PCRE2**: Built with PIC support
- **BLST**: Built with PIC support

### Static Library Configuration
All libraries must be built as static libraries:
- **OpenSSL**: `no-shared`
- **FLTK**: `--disable-shared`
- **PCRE2**: `-DBUILD_SHARED_LIBS=OFF`
- **BLST**: Static library build

## Library File Locations

After building, libraries should be placed in the following directory structure:

```
source/
├── openssl-lib/
│   ├── linux-amd64/
│   │   ├── libcrypto.a
│   │   └── libssl.a
│   ├── linux-arm64/
│   ├── mac-amd64/
│   ├── mac-arm64/
│   └── win-amd64/
│       ├── libcrypto_static.lib
│       └── libssl_static.lib
├── fl-lib/
│   ├── linux-amd64/
│   ├── linux-arm64/
│   ├── mac-amd64/
│   ├── mac-arm64/
│   └── win-amd64/
├── pcre2-lib/
│   ├── linux-amd64/
│   ├── linux-arm64/
│   ├── mac-amd64/
│   ├── mac-arm64/
│   └── win-amd64/
└── blst-lib/
    ├── linux-amd64/
    ├── linux-arm64/
    ├── mac-amd64/
    ├── mac-arm64/
    └── win-amd64/
```

## Verification

After building libraries, verify they are compatible:

```bash
# Check library architecture
file source/openssl-lib/linux-amd64/libcrypto.a
file source/fl-lib/mac-arm64/libfltk.a

# Check for C++17 compatibility (no __isoc23_ symbols)
nm source/openssl-lib/linux-arm64/libcrypto.a | grep __isoc23
```

## Troubleshooting

### Common Issues

1. **C++23 Symbol Errors**: Ensure all libraries are built with C++17 compatibility
2. **Architecture Mismatch**: Verify libraries match target platform architecture
3. **Missing Dependencies**: Install required build tools for each platform
4. **PIC Issues**: Ensure all libraries are built with `-fPIC`

### Build System Debugging

#### Executable Preservation Issue
**Problem**: Executable being deleted during library builds  
**Root Cause**: Executable removal logic running for all build types  
**Solution**: Only remove executable when building executable, not libraries

```python
# PROBLEMATIC CODE (before fix)
def _run_mac_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
    # Remove existing executable
    if os.path.exists(config.output_name):
        os.remove(config.output_name)  # ❌ This ran for ALL builds

# FIXED CODE (after fix)
def _run_mac_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
    # Remove existing executable only when building executable
    if not is_library and os.path.exists(config.output_name):
        os.remove(config.output_name)  # ✅ Only runs for executable builds
```

#### Windows Build Freezing
**Problem**: OpenSSL build freezes on test app compilation  
**Solution**: This is normal - abort the build at that point as libraries are already built

#### Cross-Platform Impact
The same executable preservation issue exists in:
- **Linux Build** (`_run_linux_build_command`): ❌ Same issue
- **AWS Build** (`_run_aws_build_command`): ❌ Same issue  
- **Windows Build**: ✅ Not affected (different build architecture)

### Platform-Specific Issues

#### Linux ARM64
- **Missing X11 libraries**: Install `libx11-dev`, `libxft-dev`, etc.
- **Missing build tools**: Install `build-essential`, `gcc`, `g++`
- **Chroot environment**: Use QEMU emulation for native ARM64 builds

#### Windows
- **Visual Studio**: Ensure Visual Studio is properly installed
- **Windows SDK**: Install Windows SDK for development
- **Build tools**: Use Visual Studio Developer Command Prompt

#### macOS
- **Xcode**: Install Xcode Command Line Tools
- **Cross-compilation**: Use appropriate target flags for cross-compilation

## Automation

Consider creating automated build scripts for each platform:

```bash
# Example: build-linux-amd64.sh
#!/bin/bash
set -e
cd dep/openssl-1.1.1w
./config -fPIC -std=c++17 no-shared
make
cp libcrypto.a libssl.a ../source/openssl-lib/linux-amd64/
# ... continue for other libraries
```

This ensures consistent builds across all platforms and makes library updates reproducible. 