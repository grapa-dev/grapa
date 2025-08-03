# GrapaPy Build Process

## Overview

This document describes the original, proven approach for building and distributing GrapaPy wheels. This process ensures that `pip install grapapy` works on all supported platforms without requiring users to have development tools or build from source.

## Architecture

The build process uses a **pre-built library approach**:

1. **Build libraries on each target platform** using `build.py --lib-only`
2. **Commit pre-built libraries** to the repository in `source/*-lib/` directories
3. **Build wheels on any single platform** using the pre-built libraries
4. **Upload wheels to PyPI** using `twine`

This approach eliminates cross-compilation complexity and ensures all platforms get proper wheels.

## Supported Platforms

- **Windows**: AMD64, ARM64
- **macOS**: AMD64, ARM64  
- **Linux**: AMD64, ARM64
- **AWS Lambda**: AMD64, ARM64 (treated as standard Linux)

## Step-by-Step Build Process

### Phase 1: Build Libraries on Each Platform

For each supported platform, run:

```bash
# Clone the repository
git clone https://github.com/grapa-dev/grapa.git
cd grapa

# Build libraries only
python build.py --lib-only

# Verify libraries were created
ls -la source/grapa-lib/
ls -la source/openssl-lib/
ls -la source/fl-lib/
ls -la source/blst-lib/
ls -la source/pcre2-lib/
```

**Expected Output:**
- Libraries copied to appropriate `source/*-lib/*/` directories
- Platform-specific library files (`.a`, `.so`, `.lib`) created

### Phase 2: Commit Pre-built Libraries

```bash
# Add all library directories
git add source/*-lib/

# Commit with descriptive message
git commit -m "Update pre-built libraries for all platforms"

# Push to GitHub
git push origin main
```

### Phase 3: Build Wheels (Single Platform)

On any platform (preferably Linux for simplicity):

```bash
# Pull latest with pre-built libraries
git pull origin main

# Clean previous builds
rm -rf dist/ build/ *.egg-info/

# Build wheels using pre-built libraries
python setup.py bdist_wheel

# Verify wheels created
ls -la dist/
```

**Expected Output:**
- Wheels for all platforms created in `dist/`
- Wheel names like: `grapapy-0.0.51-cp313-cp313-linux_aarch64.whl`

### Phase 4: Upload to PyPI

```bash
# Install twine if not already installed
pip install twine

# Upload to PyPI
twine upload dist/*.whl

# Verify upload
pip install grapapy --upgrade
python -c "import grapapy; print(grapapy.__version__)"
```

## Build Script Options

### `build.py` Options

```bash
python build.py --help
```

**Key Options:**
- `--lib-only`: Build only libraries (skip executable, Python package)
- `--python-only`: Build only Python extension (assumes executable exists)
- `--exe-only`: Build only executable (skip libraries, Python package)
- `--preserve-dist`: Keep dist/ directory after build
- `--clean`: Clean build artifacts before building

### `setup.py` Configuration

The `setup.py` file automatically:
- Detects the current platform
- Uses appropriate pre-built libraries
- Sets correct compiler flags and link options
- Handles platform-specific library paths

## Platform-Specific Details

### Windows
- **Libraries**: `source/grapa-lib/win-amd64/`, `source/grapa-lib/win-arm64/`
- **Build**: Uses Visual Studio project files
- **Wheels**: `grapapy-*-win_amd64.whl`, `grapapy-*-win_arm64.whl`

### macOS
- **Libraries**: `source/grapa-lib/mac-amd64/`, `source/grapa-lib/mac-arm64/`
- **Build**: Uses Xcode/clang
- **Wheels**: `grapapy-*-macosx_*.whl` (universal binaries)

### Linux
- **Libraries**: `source/grapa-lib/linux-amd64/`, `source/grapa-lib/linux-arm64/`
- **Build**: Uses gcc/clang
- **Wheels**: `grapapy-*-manylinux2014_x86_64.whl`, `grapapy-*-linux_aarch64.whl`

## Version Management

### Updating Version

1. **Update version in `setup.py`:**
   ```python
   grapapy_version = "0.0.52"
   ```

2. **Update version in `source/mainpy.cpp`:**
   ```cpp
   m.attr("__version__") = "0.0.52";
   ```

3. **Clean and rebuild:**
   ```bash
   rm -rf dist/ build/ *.egg-info/
   python setup.py bdist_wheel
   ```

### Version Naming Convention

- **Format**: `0.0.XX` where XX is incremented for each release
- **Examples**: `0.0.51`, `0.0.52`, `0.0.53`
- **Tagging**: Create git tag `v0.0.XX` for each release

## Automated CI/CD Workflow

### Current Status (January 2025)

The original manual process has been automated using GitHub Actions in `.github/workflows/build-libraries.yml`. The workflow implements a two-stage process:

1. **Stage 1 - Build Libraries**: Runs on multiple platforms (Windows AMD64, macOS AMD64/ARM64, Linux AMD64/ARM64) to build and commit pre-built libraries
2. **Stage 2 - Build and Deploy**: Builds universal wheels containing all platform libraries and deploys to PyPI

### Current Issues

- **Version Mismatch**: PyPI shows v0.0.73 with 3 wheels, but `pip install grapapy` still installs v0.0.42
- **Caching Issue**: Local pip cache may be preventing installation of latest version
- **Workflow Status**: Latest tag is v0.0.87, but deployment may not have completed successfully

### Debugging Steps

1. **Clear pip cache**: `pip cache purge`
2. **Force reinstall**: `pip install --force-reinstall grapapy`
3. **Check PyPI directly**: `pip index versions grapapy`
4. **Verify workflow runs**: Check GitHub Actions for v0.0.87 deployment status

### Manual Override (if needed)

If the automated workflow fails, the original manual process can still be used:

```bash
# On each platform, build libraries
python build.py --lib-only

# Commit and push libraries
git add source/*-lib/
git commit -m "Update libraries for v0.0.88"
git push origin main

# Tag and trigger deployment
git tag v0.0.88
git push origin v0.0.88
```

## Troubleshooting

### Common Issues

1. **X11 Library Errors**: Ensure X11 development libraries are installed on Linux
2. **Platform Tag Issues**: Use `--plat-name any` for universal wheels
3. **Cached Installations**: Use `pip cache purge` and `--force-reinstall`
4. **Workflow Failures**: Check GitHub Actions logs for specific error messages

### Platform-Specific Notes

- **Windows**: Requires Visual Studio build tools
- **macOS**: Requires Xcode command line tools
- **Linux**: Requires X11 development libraries (`libx11-dev`, etc.)
- **ARM64**: May require specific compiler flags for compatibility

## Best Practices

1. **Always test on target platforms** before releasing
2. **Use semantic versioning** for tags (v0.0.XX)
3. **Verify PyPI uploads** immediately after deployment
4. **Keep local pip cache clean** during testing
5. **Monitor GitHub Actions** for workflow success/failure 