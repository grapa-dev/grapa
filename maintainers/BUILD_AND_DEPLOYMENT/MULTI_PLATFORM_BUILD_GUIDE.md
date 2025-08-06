# Multi-Platform Build Guide

## Overview

This guide covers building Grapa for all 5 supported platforms:
- **Linux ARM64** - Docker container
- **Linux AMD64** - Docker container  
- **macOS ARM64** - Native build
- **macOS AMD64** - Cross-compilation from ARM64 Mac
- **Windows AMD64** - Windows machine or GitHub Actions

## Prerequisites

### Required Software
- **Docker Desktop** - For Linux builds
- **Python 3.x** - For build scripts and Python package
- **Git** - For version control and workflow triggers
- **GitHub CLI** - For workflow monitoring (optional)

### Platform-Specific Requirements
- **macOS**: Xcode Command Line Tools
- **Windows**: Visual Studio 2019+ with C++ build tools
- **Linux**: Docker with Ubuntu 24.04 base image

### Environment Setup
```bash
# Clone repository
git clone https://github.com/grapa-dev/grapa.git
cd grapa

# Make scripts executable
chmod +x scripts/*.sh

# Verify setup
./scripts/check_platform_status.sh
```

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
python3 build.py --bin-only --clean                    # ARM64 native
./scripts/build_grapa_macos_amd64.sh                  # AMD64 cross-compilation

# Windows
./scripts/build_grapa_windows_amd64.sh                # Requires Windows machine
```

### Build Options

The `build.py` script supports several important options:

| Option | Purpose | Example |
|--------|---------|---------|
| `--clean` | Clean build artifacts after build | `python3 build.py --bin-only --clean` |
| `--preserve-exe` | Keep executable after cleaning | `python3 build.py --bin-only --clean --preserve-exe` |
| `--preserve-dist` | Keep Python distribution after cleaning | `python3 build.py --python-only --preserve-dist` |
| `--test` | Run validation tests after build | `python3 build.py --bin-only --test` |
| `--test-only` | Run tests only (assumes build completed) | `python3 build.py --test-only` |
| `--target-platform` | Cross-compilation target (macOS only) | `python3 build.py --bin-only --target-platform mac-amd64` |
| `--exe-only` | Build executable only | `python3 build.py --exe-only --test` |
| `--python-only` | Build Python package only | `python3 build.py --python-only --preserve-dist` |

**Note:** The `--target-platform` option only supports macOS platforms (`mac-arm64`, `mac-amd64`). Linux and Windows builds do not use this option.

## Platform-Specific Details

### Linux (ARM64 & AMD64)

**Requirements:**
- Docker Desktop
- Ubuntu 24.04 base image (updated for C++23 support)

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
- Native build using `python3 build.py --bin-only --clean`
- Uses system clang compiler
- Automatically cleans build artifacts after build

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
- Automatically cleans build artifacts after build

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
FROM ubuntu:24.04
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

### Python Distribution and Validation

After all platform builds complete, the script automatically:

1. **Builds Python package**: `python3 build.py --python-only --preserve-dist`
2. **Installs package**: `pip3 install dist/*.tar.gz`
3. **Validates version**: Compares `grapapy.__version__` with `$VERSION` environment variable
4. **Runs version script**: Uses `grapapy -c "\$sys().getenv(\$VERSION);"` to verify consistency

**Version Validation Process:**
- Extracts version from `grapapy.__version__`
- Runs grapapy with version script: `$sys().getenv($VERSION);`
- Compares both values for exact match
- Fails if versions don't match

### Testing and Validation

#### Manual Testing
```bash
# Test CLI executable after build
python3 build.py --exe-only --test

# Test Python package after build
python3 build.py --python-only --test

# Test both CLI and Python after build
python3 build.py --test

# Test only (assumes build already completed)
python3 build.py --test-only
```

#### Automatic Validation

The `build_all_platforms.sh` script includes automatic validation that checks:

1. **Executables**: `grapa` (Linux/macOS) or `grapa.exe` (Windows)
2. **Static Libraries**: `libgrapa.a` (Linux/macOS) or `grapa.lib` (Windows)
3. **Shared Libraries**: `libgrapa.so` (Linux/macOS only)
4. **Compressed Packages**: All expected files are included
5. **Build Timestamps**: Files were created during current build session
6. **Python Package**: Builds and validates Python distribution
7. **Version Validation**: Confirms `grapapy.__version__` matches `$VERSION` environment variable

Validation fails if any required artifacts are missing or version mismatch is detected.

## Troubleshooting

### Common Issues

1. **Docker build fails with linker errors**
   - Install missing X11 libraries: `libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig-dev libxcursor-dev libjpeg-dev`
   - Check Docker platform: Ensure `--platform=linux/amd64` is used for AMD64 builds

2. **macOS cross-compilation warnings**
   - Warnings about newer macOS version are normal and don't affect functionality
   - Ensure you're on ARM64 Mac for cross-compilation to AMD64

3. **Windows build requires Visual Studio**
   - Install Visual Studio 2019 or later with C++ build tools
   - Or use GitHub Actions workflow

4. **Permission denied on scripts**
   - Run: `chmod +x scripts/*.sh`

5. **Version bump fails**
   - Check git status: `git status`
   - Ensure no uncommitted changes in version files
   - Verify version format: Must be X.Y.Z (e.g., 0.0.248)

6. **GitHub Actions workflow doesn't trigger**
   - Check if version files were committed and pushed
   - Verify git tag was created: `git tag -l`
   - Check workflow status at: https://github.com/grapa-dev/grapa/actions

7. **Build artifacts not updated**
   - Check build timestamps: Files should be from current build session
   - Verify `--clean` option is working properly
   - Check for build errors in output

8. **Python package validation fails**
   - Ensure `pip3 install dist/*.tar.gz` completed successfully
   - Check `grapapy.__version__` matches environment `$VERSION`
   - Verify Python package was built with correct version

### Debugging Commands

```bash
# Check current version
grep 'grapapy_version = "' setup.py

# Check git status
git status

# Check recent commits
git log --oneline -5

# Check git tags
git tag -l

# Check build artifacts
ls -la bin/
ls -la source/grapa-lib/
ls -la source/grapa-other/

# Check Python package
python3 -c "import grapapy; print(grapapy.__version__)"

# Check CLI version
./grapa -c "\$sys().getenv(\$VERSION);"
```

### Workflow Monitoring and Artifact Management

#### Windows GitHub Actions Workflow

**Monitoring:**
```bash
# Monitor workflow and download artifacts
./scripts/monitor_and_download_windows.sh

# Force download artifacts (overwrites existing)
./scripts/download_windows_artifacts_force.sh
```

**Manual Monitoring:**
- Check workflow status: https://github.com/grapa-dev/grapa/actions
- Look for "Build Windows AMD64" workflow
- Download artifacts manually if needed

#### Artifact Management

**Expected Artifacts:**
- `grapa.exe` - Windows executable
- `grapa.lib` - Windows static library
- `grapa-win-amd64.zip` - Compressed package

**Artifact Locations:**
- **Linux**: `bin/grapa-linux-*.tar.gz`
- **macOS**: `bin/grapa-mac-*.tar.gz`
- **Windows**: `bin/grapa-win-amd64.zip` (after download)

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

## Version Bumping and Deployment

### Overview

Grapa supports multiple methods for version bumping and deployment, each suited for different use cases:

### Method 1: Full Multi-Platform Release (Recommended)

**Command:**
```bash
./scripts/build_all_platforms.sh --bump-version
```

**What it does:**
- Auto-increments current version by 1
- Builds all 5 platforms (Linux ARM64/AMD64, macOS ARM64/AMD64, Windows AMD64)
- Triggers Windows GitHub Actions workflow
- Validates all builds and Python package

**When to use:**
- Full releases
- When you want all platforms built
- Standard release process

**Version Logic:**
- Reads current version from `setup.py`
- Increments last number (e.g., `0.0.247` → `0.0.248`)
- Updates all version files automatically

### Method 2: Quick Version Bump Only

**Commands:**
```bash
# Auto-increment version
python3 scripts/bump_version_and_deploy.py --bump-version --commit-and-push

# Specify exact version
python3 scripts/bump_version_and_deploy.py 0.0.248 --commit-and-push
```

**What it does:**
- Bumps version and triggers Windows GitHub Actions workflow only
- Does NOT build other platforms
- Faster than full multi-platform build

**When to use:**
- Quick hotfixes
- When you only need Windows build
- Specific version numbers

### Method 3: Version Bump Without Commit

**Commands:**
```bash
# Auto-increment version (no commit)
python3 scripts/bump_version_and_deploy.py --bump-version

# Specify exact version (no commit)
python3 scripts/bump_version_and_deploy.py 0.0.248
```

**What it does:**
- Updates version files locally
- Does NOT commit or push
- Does NOT trigger workflows

**When to use:**
- Testing version changes
- Local development
- When you want to review changes before committing

### Version Files Updated

All version bumping methods update these 3 files:
1. **`setup.py`** - `grapapy_version = "0.0.248"`
2. **`source/grapa/GrapaLink.h`** - `#define grapa_version "0.0.248"`
3. **`source/mainpy.cpp`** - `m.attr("__version__") = "0.0.248"`

### Version Bumping Workflow

#### For Testing (No Version Bump)
```bash
./scripts/build_all_platforms.sh
```
- Builds all platforms with current version
- No version increment
- Safe for debugging and testing

#### For Full Releases
```bash
./scripts/build_all_platforms.sh --bump-version
```
- Auto-increments version
- Builds all platforms
- Triggers Windows workflow
- Validates everything

#### For Quick Hotfixes
```bash
python3 scripts/bump_version_and_deploy.py --bump-version --commit-and-push
```
- Auto-increments version
- Triggers Windows workflow only
- Faster than full build

### Version File Commit/Push Triggers

**Scenarios that commit and push version files:**

1. **Full Release**: `./scripts/build_all_platforms.sh --bump-version`
   - Auto-increments version
   - Updates 3 version files
   - Commits with message "Bump version to X.X.X"
   - Creates git tag `vX.X.X`
   - Pushes commit and tag
   - Triggers Windows GitHub Actions workflow

2. **Quick Release**: `python3 scripts/bump_version_and_deploy.py --bump-version --commit-and-push`
   - Updates 3 version files
   - Commits with message "Bump version to X.X.X"
   - Creates git tag `vX.X.X`
   - Pushes commit and tag
   - Triggers Windows GitHub Actions workflow

3. **Manual Version**: `python3 scripts/bump_version_and_deploy.py 0.0.248 --commit-and-push`
   - Updates 3 version files with specified version
   - Commits and pushes as above

**Scenarios that do NOT commit/push:**
- `./scripts/build_all_platforms.sh` (testing builds)
- `python3 scripts/bump_version_and_deploy.py --bump-version` (no `--commit-and-push` flag)
- `python3 scripts/bump_version_and_deploy.py 0.0.248` (no `--commit-and-push` flag)

**After commit/push:**
- Windows GitHub Actions workflow triggers automatically
- PyPI deployment begins after successful build
- Version files are permanently committed to git history

### Comprehensive Testing and Validation

The `build_all_platforms.sh` script performs comprehensive testing:

#### **Platform Build Validation**
- ✅ **Linux ARM64**: Builds and validates artifacts
- ✅ **Linux AMD64**: Builds and validates artifacts  
- ✅ **macOS ARM64**: Builds and validates artifacts
- ✅ **macOS AMD64**: Builds and validates artifacts
- ✅ **Windows AMD64**: Builds and validates artifacts

#### **CLI Testing (All 5 Platforms)**
For each platform, the script:
1. **Extracts** executable from compressed package
2. **Tests CLI help**: `./grapa -h` (5-second timeout)
3. **Tests CLI version**: `./grapa -c "\$sys().getenv(\$VERSION);"`
4. **Validates version** matches expected version from `setup.py`
5. **Reports** executable size and functionality

#### **Python Package Testing**
1. **Builds Python package**: `python3 build.py --python-only --preserve-dist`
2. **Installs package**: `pip3 install dist/*.tar.gz`
3. **Tests version**: `grapapy.__version__` matches expected version
4. **Tests functionality**: `grapapy.eval('\$sys().getenv(\$VERSION);')`
5. **Validates** Python package works with all platform artifacts

#### **Artifact Validation**
- **Version Consistency**: All 3 version files match
- **Build Timestamps**: All artifacts are from current build session
- **Package Contents**: Compressed packages contain expected files
- **File Sizes**: Reasonable file sizes for each platform

## Complete Release Process

### Step-by-Step Release Workflow

#### 1. Pre-Release Testing
```bash
# Test all platforms without version bump
./scripts/build_all_platforms.sh

# Verify all builds work correctly
./scripts/check_platform_status.sh
```

#### 2. Full Release
```bash
# Build all platforms with version bump
./scripts/build_all_platforms.sh --bump-version

# This automatically:
# - Increments version (e.g., 0.0.247 → 0.0.248)
# - Builds all 5 platforms
# - Triggers Windows GitHub Actions workflow
# - Validates all builds and Python package
```

#### 3. Monitor and Verify
```bash
# Check build status
./scripts/check_platform_status.sh

# Monitor Windows workflow
./scripts/monitor_and_download_windows.sh

# Verify Python package
python3 -c "import grapapy; print(grapapy.__version__)"
```

#### 4. Deploy to PyPI
```bash
# Build Python distribution
python3 build.py --python-only --preserve-dist

# Install and test
pip3 install dist/*.tar.gz

# Deploy (if automated deployment is set up)
# PyPI deployment happens automatically after successful builds
```

### Release Checklist

- [ ] **Pre-release testing** completed successfully
- [ ] **All platforms** built and validated
- [ ] **Windows artifacts** downloaded from GitHub Actions
- [ ] **Python package** built and tested
- [ ] **Version consistency** verified across all artifacts
- [ ] **GitHub Actions workflow** completed successfully
- [ ] **PyPI deployment** completed (if applicable)

### Quick Hotfix Process

For urgent fixes that only need Windows:

```bash
# Quick version bump and Windows build
python3 scripts/bump_version_and_deploy.py --bump-version --commit-and-push

# Monitor workflow
./scripts/monitor_and_download_windows.sh
```

## Next Steps

1. **Complete Windows build** on Windows machine or via GitHub Actions
2. **Test all platforms** to ensure compatibility
3. **Build Python distribution** using `python3 build.py --python-only --preserve-dist`
4. **Validate Python package** version matches environment `$VERSION`
5. **Deploy to PyPI** from Mac with all platform artifacts
6. **Set up automated builds** for releases

## Scripts Reference

| Script | Purpose | Platform | Testing |
|--------|---------|----------|---------|
| `check_platform_status.sh` | Check build status | All | None |
| `build_all_platforms.sh` | Build all available | All | ✅ CLI + Python |
| `build_all_platforms.sh --bump-version` | Build all + version bump | All | ✅ CLI + Python |
| `build_grapa_linux_arm64.sh` | Linux ARM64 | Docker | None |
| `build_grapa_linux_amd64.sh` | Linux AMD64 | Docker | None |
| `build_grapa_macos_amd64.sh` | macOS AMD64 | Cross-compilation | None |
| `build_grapa_windows_amd64.sh` | Windows AMD64 | Native/Instructions | None |
| `bump_version_and_deploy.py --bump-version` | Auto-increment version | Version only | None |
| `bump_version_and_deploy.py <version>` | Manual version bump | Version only | None | 