# Build Scripts

This directory contains build scripts for Grapa dependencies and utilities.

## FLTK 1.4.4 Build Script

The `build_fltk.py` script builds FLTK 1.4.4 static libraries for all supported platforms.

### Prerequisites

- FLTK 1.4.4 source code extracted to `dep/fltk-1.4.4/`
- Python 3.6+
- Platform-specific build tools:
  - **macOS**: Xcode Command Line Tools
  - **Linux**: GCC, Make, autotools
  - **Windows**: Visual Studio or MinGW
  - **AWS**: Same as Linux

### Usage

```bash
# Build for current platform (auto-detected)
python3 scripts/build/build_fltk.py

# Build for specific target
python3 scripts/build/build_fltk.py --target mac-arm64
python3 scripts/build/build_fltk.py --target linux-amd64
python3 scripts/build/build_fltk.py --target windows-amd64

# Clean build artifacts
python3 scripts/build/build_fltk.py --clean

# List all supported targets
python3 scripts/build/build_fltk.py --list-targets

# Build and verify
python3 scripts/build/build_fltk.py --verify
```

**Windows Users**: Use `python` instead of `python3`:
```cmd
python scripts\build\build_fltk.py --target windows-amd64
```

### Supported Targets

- `mac-arm64` - macOS on Apple Silicon
- `mac-amd64` - macOS on Intel
- `linux-arm64` - Linux on ARM64
- `linux-amd64` - Linux on x86_64
- `windows-arm64` - Windows on ARM64
- `windows-amd64` - Windows on x86_64
- `aws-arm64` - AWS Linux on ARM64
- `aws-amd64` - AWS Linux on x86_64

### What It Builds

The script builds the following static libraries:
- `libfltk.a` - Core FLTK library
- `libfltk_forms.a` - Forms library
- `libfltk_gl.a` - OpenGL library
- `libfltk_images.a` - Image loading library
- `libfltk_jpeg.a` - JPEG support
- `libfltk_png.a` - PNG support
- `libfltk_z.a` - Zlib compression

### Output Locations

- **Libraries**: `source/fl-lib/{platform}-{arch}/`
- **Headers**: `source/FL/`

### Platform-Specific Notes

#### macOS
- Automatically includes `ScreenCaptureKit` framework for macOS 15+
- Uses `clang++` compiler
- Builds with Quartz graphics driver

#### Linux
- Uses `g++` compiler
- May require X11 development libraries
- Builds with X11 graphics driver

#### Windows
- Supports both Visual Studio and MinGW
- Visual Studio build requires manual configuration
- MinGW build uses autotools

#### AWS Linux
- Same as regular Linux
- Detected automatically by checking `/etc/system-release`

### Troubleshooting

#### Common Issues

1. **FLTK source not found**
   ```
   ❌ FLTK source not found at dep/fltk-1.4.4
   ```
   - Ensure FLTK 1.4.4 is extracted to `dep/fltk-1.4.4/`

2. **Compiler not found (Windows)**
   ```
   ❌ No suitable compiler found. Install Visual Studio or MinGW.
   ```
   - Install Visual Studio or MinGW
   - Ensure compiler is in PATH

3. **Missing dependencies (Linux)**
   ```
   configure: error: X11 not found
   ```
   - Install X11 development packages: `sudo apt-get install libx11-dev libxext-dev libxrender-dev libxinerama-dev`

4. **Build fails with ScreenCaptureKit errors (macOS)**
   - Ensure you're on macOS 15+ or disable ScreenCaptureKit in the script

#### Verification

Use the `--verify` flag to check that all expected libraries were built:

```bash
python3 scripts/build/build_fltk.py --verify
```

This will list all libraries and their sizes, highlighting any missing ones.

### Integration with Grapa Build

After building FLTK libraries, you can build Grapa normally:

```bash
# Build Grapa executable
python3 build.py --exe-only

# Build full package
python3 build.py
```

The Grapa build system will automatically use the FLTK libraries from `source/fl-lib/{platform}-{arch}/`.
