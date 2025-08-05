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

#### OpenSSL
```bash
cd dep/openssl-1.1.1w
# Use Visual Studio Developer Command Prompt
perl Configure VC-WIN64A -fPIC -std=c++17 no-shared
nmake
# Copy libcrypto_static.lib and libssl_static.lib to source/openssl-lib/win-amd64/
```

#### FLTK
```bash
cd dep/fltk-1.3.11-source
# Use Visual Studio Developer Command Prompt
# FLTK requires Visual Studio build system
# Build with static library configuration
# Copy *.lib files to source/fl-lib/win-amd64/
```

#### PCRE2
```bash
cd dep/pcre2-10.45
# Use Visual Studio Developer Command Prompt
# Build with CMake for Visual Studio
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
# Build with Visual Studio
# Copy pcre2-8-static.lib to source/pcre2-lib/win-amd64/
```

#### BLST
```bash
cd dep/blst-master
# Use Visual Studio Developer Command Prompt
# Build with Visual Studio
# Copy blst.lib to source/blst-lib/win-amd64/
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