# Multi-Platform Build Guide

## Overview

This guide covers building Grapa for all 5 supported platforms:
- **Linux ARM64** - Docker container
- **Linux AMD64** - Docker container  
- **macOS ARM64** - Native build
- **macOS AMD64** - Cross-compilation from ARM64 Mac
- **Windows AMD64** - Windows machine or GitHub Actions

## Quick Start

### Check Current Status
```bash
./scripts/check_platform_status.sh
```

### Build All Available Platforms
```bash
./scripts/build_all_platforms.sh
```

### Build Individual Platforms
```bash
# Linux (Docker)
./scripts/build_grapa_linux_arm64.sh
./scripts/build_grapa_linux_amd64.sh

# macOS
python3 build.py --bin-only                    # ARM64 native
./scripts/build_grapa_macos_amd64.sh          # AMD64 cross-compilation

# Windows
./scripts/build_grapa_windows_amd64.sh        # Requires Windows machine
```

## Platform-Specific Details

### Linux (ARM64 & AMD64)

**Requirements:**
- Docker Desktop
- Ubuntu 22.04 base image

**Build Method:**
- Uses `Dockerfile.grapa-build`
- Runs `python3 build.py --bin-only` in container
- Creates compressed packages in `bin/`

**Output:**
- `grapa` (executable)
- `source/grapa-lib/linux-{arch}/libgrapa.a` (static library)
- `source/grapa-other/linux-{arch}/libgrapa.so` (shared library)
- `bin/grapa-linux-{arch}.tar.gz` (compressed package)

### macOS ARM64

**Requirements:**
- macOS machine
- Xcode Command Line Tools
- Python 3.x

**Build Method:**
- Native build using `python3 build.py --bin-only`
- Uses system clang compiler

**Output:**
- `grapa` (executable)
- `source/grapa-lib/mac-arm64/libgrapa.a` (static library)
- `source/grapa-other/mac-arm64/libgrapa.so` (shared library)
- `bin/grapa-mac-arm64.tar.gz` (compressed package)

### macOS AMD64

**Requirements:**
- ARM64 Mac (for cross-compilation)
- Xcode Command Line Tools
- Python 3.x

**Build Method:**
- Cross-compilation from ARM64 to AMD64
- Uses `--target-platform mac-amd64` flag

**Output:**
- `grapa` (executable)
- `source/grapa-lib/mac-amd64/libgrapa.a` (static library)
- `source/grapa-other/mac-amd64/libgrapa.so` (shared library)
- `bin/grapa-mac-amd64.tar.gz` (compressed package)

### Windows AMD64

**Requirements:**
- Windows machine
- Visual Studio 2019 or later
- Python 3.x
- Git for Windows

**Build Method:**
- Native build on Windows
- Uses Visual Studio compiler

**Alternative:**
- GitHub Actions workflow: `.github/workflows/build-windows.yml`

**Output:**
- `grapa.exe` (executable)
- `source/grapa-lib/win-amd64/grapa.lib` (static library)
- `bin/grapa-win-amd64.zip` (compressed package)

**Note:** Windows builds only create static libraries (`.lib`), no shared libraries (`.dll`) are built.

## Build Options

### `--bin-only` Option

The `--bin-only` option is the recommended way to build complete distribution packages:

```bash
python3 build.py --bin-only                    # Current platform
python3 build.py --bin-only --target-platform mac-amd64  # Cross-compilation
```

**What it does:**
1. Builds executable
2. Builds static library
3. Builds shared library
4. Copies libraries to correct locations
5. Creates compressed package in `bin/`
6. Cleans up temporary files

### Other Options

```bash
python3 build.py --exe-only    # Build executable only
python3 build.py --lib-only    # Build libraries only
python3 build.py --python-only # Build Python extension only
```

## Docker Setup

### Base Dockerfile

```dockerfile
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    python3 \
    python3-pip \
    libx11-dev \
    libxfixes-dev \
    libxft-dev \
    libxext-dev \
    libxrender-dev \
    libxinerama-dev \
    libfontconfig-dev \
    libxcursor-dev \
    libjpeg-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /data
CMD ["/bin/bash"]
```

### Building Docker Image

```bash
docker build -f Dockerfile.grapa-build -t grapa-build .
```

## GitHub Actions

### Windows Build Workflow

The `.github/workflows/build-windows.yml` workflow provides automated Windows builds:

- **Trigger:** Manual dispatch or push to main
- **Runner:** `windows-latest`
- **Setup:** Python 3.11 + Visual Studio Build Tools
- **Output:** Uploads artifacts for download

### Usage

1. **Manual trigger:** Go to Actions → Build Windows AMD64 → Run workflow
2. **Automatic trigger:** Push changes to main branch
3. **Download artifacts:** From the workflow run page

## File Structure

### Expected Output Structure

```
grapa/                          # Current platform executable
source/
├── grapa-lib/
│   ├── linux-arm64/libgrapa.a
│   ├── linux-amd64/libgrapa.a
│   ├── mac-arm64/libgrapa.a
│   ├── mac-amd64/libgrapa.a
│   └── win-amd64/grapa.lib
└── grapa-other/
    ├── linux-arm64/libgrapa.so
    ├── linux-amd64/libgrapa.so
    ├── mac-arm64/libgrapa.so
    ├── mac-amd64/libgrapa.so
    └── win-amd64/ (empty - no shared library built for Windows)
bin/
├── grapa-linux-arm64.tar.gz
├── grapa-linux-amd64.tar.gz
├── grapa-mac-arm64.tar.gz
├── grapa-mac-amd64.tar.gz
└── grapa-win-amd64.zip
```

## Package Contents

### Compressed Package Contents

Each platform creates a compressed package with the following contents:

#### Linux Packages (`grapa-linux-*.tar.gz`)
- **Executable**: `grapa` (platform-specific binary)
- **Static Library**: `libgrapa.a` (from `source/grapa-lib/linux-{arch}/`)
- **Shared Library**: `libgrapa.so` (from `source/grapa-other/linux-{arch}/`)

#### macOS Packages (`grapa-mac-*.tar.gz`)
- **Executable**: `grapa` (platform-specific binary)
- **Static Library**: `libgrapa.a` (from `source/grapa-lib/mac-{arch}/`)
- **Shared Library**: `libgrapa.so` (from `source/grapa-other/mac-{arch}/`)

#### Windows Package (`grapa-win-amd64.zip`)
- **Executable**: `grapa.exe` (Windows AMD64 binary)
- **Static Library**: `grapa.lib` (from `source/grapa-lib/win-amd64/`)

**Note:** Windows builds only create static libraries, no shared libraries are built.

### Validation

The `build_all_platforms.sh` script includes automatic validation that checks:

1. **Executables**: `grapa` (Linux/macOS) or `grapa.exe` (Windows)
2. **Static Libraries**: `libgrapa.a` (Linux/macOS) or `grapa.lib` (Windows)
3. **Shared Libraries**: `libgrapa.so` (Linux/macOS only)
4. **Compressed Packages**: All expected files are included

Validation fails if any required artifacts are missing.

## Troubleshooting

### Common Issues

1. **Docker build fails with linker errors**
   - Install missing X11 libraries: `libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig-dev libxcursor-dev libjpeg-dev`

2. **macOS cross-compilation warnings**
   - Warnings about newer macOS version are normal and don't affect functionality

3. **Windows build requires Visual Studio**
   - Install Visual Studio 2019 or later with C++ build tools
   - Or use GitHub Actions workflow

4. **Permission denied on scripts**
   - Run: `chmod +x scripts/*.sh`

### Validation

Use the status check script to verify builds:

```bash
./scripts/check_platform_status.sh
```

This will show:
- ✅ Available builds
- ❌ Missing builds
- File sizes and timestamps
- Build instructions for missing platforms

## Next Steps

1. **Complete Windows build** on Windows machine or via GitHub Actions
2. **Test all platforms** to ensure compatibility
3. **Deploy to PyPI** from Mac with all platform artifacts
4. **Set up automated builds** for releases

## Scripts Reference

| Script | Purpose | Platform |
|--------|---------|----------|
| `check_platform_status.sh` | Check build status | All |
| `build_all_platforms.sh` | Build all available | All |
| `build_grapa_linux_arm64.sh` | Linux ARM64 | Docker |
| `build_grapa_linux_amd64.sh` | Linux AMD64 | Docker |
| `build_grapa_macos_amd64.sh` | macOS AMD64 | Cross-compilation |
| `build_grapa_windows_amd64.sh` | Windows AMD64 | Native/Instructions | 