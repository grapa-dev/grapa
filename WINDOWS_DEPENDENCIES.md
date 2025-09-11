# Windows Dependencies for Grapa

This document outlines the complete dependency requirements for building and running Grapa on Windows.

## Required Dependencies

### 1. Visual Studio 2022 (or Build Tools for Visual Studio 2022)
- **Purpose**: Provides MSBuild, C++ compiler, and Windows SDK
- **Download**: https://visualstudio.microsoft.com/downloads/
- **Required Components**:
  - MSVC v143 - VS 2022 C++ x64/x86 build tools
  - Windows 10/11 SDK (latest version)
  - CMake tools for Visual Studio (optional but recommended)

### 2. 7-Zip (Development Only)
- **Purpose**: Required for building/developing Grapa from source (packaging and creating distribution archives)
- **Download**: https://www.7-zip.org/
- **Installation**: Add to PATH: `C:\Program Files\7-Zip`
- **Note**: Not required for installing the Python package (`pip install grapapy`)

### 3. Python 3.6+
- **Purpose**: Required for build system and Python integration
- **Download**: https://www.python.org/downloads/
- **Note**: Python 3.6+ is required for the build system

### 4. Developer Command Prompt
- **Purpose**: Sets up proper build environment with correct PATH and environment variables
- **Usage**: Always run from "x64 Native Tools Command Prompt for VS 2022"

## Verification

### Automatic Dependency Checking
Both `setup.py` and `bin/install-grapa.py` now include comprehensive Windows dependency checking:

```powershell
# Test dependency checking
python setup.py --help
python bin/install-grapa.py --help
```

### Manual Verification
Use the provided test script:

```powershell
python test_windows_deps.py
```

This will check for:
- ✅ MSBuild (Visual Studio 2022)
- ✅ 7-Zip
- ✅ Python 3.6+
- ✅ Windows SDK

## Installation Instructions

### Step 1: Install Visual Studio 2022
1. Download from: https://visualstudio.microsoft.com/downloads/
2. Install with these workloads:
   - Desktop development with C++
   - Python development (optional)
3. Ensure Windows 10/11 SDK is included

### Step 2: Install 7-Zip
1. Download from: https://www.7-zip.org/
2. Install to default location: `C:\Program Files\7-Zip`
3. Add to system PATH:
   - Open System Properties → Environment Variables
   - Add `C:\Program Files\7-Zip` to PATH

### Step 3: Install Python 3.6+
1. Download from: https://www.python.org/downloads/
2. Install with "Add Python to PATH" checked
3. Verify installation: `python --version`

### Step 4: Set Up Build Environment
1. Open "x64 Native Tools Command Prompt for VS 2022"
2. Navigate to Grapa project directory
3. Run dependency check: `python test_windows_deps.py`

## Build Process

### Using the Automated Build System
```powershell
# From x64 Native Tools Command Prompt for VS 2022
python build.py
```

### Using CMake (Development Kit)
```powershell
# From x64 Native Tools Command Prompt for VS 2022
cd bin
cmake ..
cmake --build .
```

### Using setup.py (Python Package)
```powershell
# From x64 Native Tools Command Prompt for VS 2022
python setup.py build_ext --inplace
```

## Troubleshooting

### Common Issues

#### MSBuild Not Found
- **Cause**: Not running from Developer Command Prompt
- **Solution**: Use "x64 Native Tools Command Prompt for VS 2022"

#### 7-Zip Not Found
- **Cause**: 7-Zip not in PATH
- **Solution**: Add `C:\Program Files\7-Zip` to system PATH

#### Python Not Found
- **Cause**: Python not in PATH or wrong version
- **Solution**: Reinstall Python with "Add to PATH" option

#### Windows SDK Not Found
- **Cause**: Visual Studio installation incomplete
- **Solution**: Reinstall Visual Studio with Windows SDK component

### Verification Commands

```powershell
# Check MSBuild version
msbuild /version

# Check 7-Zip
7z

# Check Python version
python --version

# Check Windows SDK (registry)
reg query "HKLM\SOFTWARE\Microsoft\Windows Kits\Installed Roots"
```

## Clean Environment Testing

To test on a clean Windows environment:

1. **Create Windows VM** (Windows 10/11)
2. **Test scenarios**:
   - Clean system (no dependencies)
   - Partial installation (missing some dependencies)
   - Complete installation (all dependencies)
3. **Run verification**: `python test_windows_deps.py`

## Integration with CI/CD

For automated testing, ensure CI/CD systems have:
- Visual Studio 2022 Build Tools
- 7-Zip in PATH
- Python 3.6+
- Proper environment setup

## Notes

- **Architecture**: Only AMD64 (x64) is supported
- **Windows ARM64**: Not supported in current build system
- **Cross-compilation**: Not supported - must build on Windows
- **Dependencies**: All dependencies must be installed on build machine
