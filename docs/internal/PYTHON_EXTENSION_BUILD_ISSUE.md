# Python Extension Build Issue: Windows SDK Compatibility

## Problem Description

The Python extension (`grapapy`) fails to build on Windows due to missing Windows SDK headers (`io.h`, `basetsd.h`, etc.). This occurs because:

1. **Python 3.12.2** was compiled with **MSVC v.1937**
2. **Visual Studio 2022** uses **MSVC v.14.44.35207**
3. **Windows SDK version mismatch** - newer SDK versions don't include older headers
4. **Python's `pyconfig.h`** expects these headers to be available

## Why Executable Build Works But Python Extension Doesn't

### Executable Build (Visual Studio)
- Uses **Visual Studio project files** with pre-configured settings
- Uses **MSBuild** with specific Windows SDK paths
- Has **proper include paths** configured in `.vcxproj` files
- Uses **Visual Studio's own Windows SDK detection**

### Python Extension Build (pip/setuptools)
- Uses **pip/setuptools** to build the extension
- Uses **Python's own build system** (setuptools)
- **Python's `pyconfig.h`** tries to include missing Windows SDK headers
- **Different compiler environment** - pip uses different compiler setup than Visual Studio
- **Different include paths** - pip doesn't have the same Windows SDK configuration

## Root Cause

The issue is that we're trying to **compile the extension on the user's machine**, which requires specific Windows SDK components that may not be available or compatible.

## Proper Solution: Pre-compiled Wheels

### Long-term Solution
1. **Build pre-compiled wheels** on CI/CD systems with correct Windows SDK
2. **Distribute these wheels** instead of requiring compilation
3. **Use GitHub Actions** to build wheels for all platforms and Python versions

### Implementation
- Created `.github/workflows/build-wheels.yml` for automated wheel building
- Created `build_wheels.py` for local wheel building
- Wheels will be built on CI/CD with proper Windows SDK versions

### Benefits
- **No compilation required** on user machines
- **Works on all Windows versions** regardless of SDK
- **Faster installation** - just download and install
- **Better user experience** - no Visual Studio required

## Temporary Workarounds (Not Recommended)

### Option 1: Downgrade Python
- Install Python 3.11 or earlier
- May have compatibility issues with newer features

### Option 2: Install Older Windows SDK
- Install Windows SDK 10.0.22621.0 or earlier
- Requires significant system changes
- May break other development tools

### Option 3: Use Different Compiler
- Install Visual Studio 2019 or earlier
- May not be compatible with current development environment

## Recommended Action

1. **Use the GitHub Actions workflow** to build wheels automatically
2. **Publish wheels to PyPI** for easy installation
3. **Update documentation** to recommend `pip install grapapy` instead of building from source
4. **Remove source build instructions** from user-facing documentation

## Testing the Solution

To test the wheel building locally:

```bash
# Build wheel for current platform
python build_wheels.py

# Install from wheel
pip install dist/grapapy-*.whl
```

## Future Improvements

1. **Automated wheel publishing** on PyPI
2. **Multi-platform wheel support** (Windows, macOS, Linux)
3. **Multiple Python version support** (3.8-3.12)
4. **Development wheels** for testing 