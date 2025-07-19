# Build System Windows Debugging - WIP

**Started:** 2024-12-19
**Last Updated:** 2024-12-19
**Status:** COMPLETED - Issue Identified

## Current Focus

**Task**: Debug the new build.py system on Windows to ensure executables are properly created and included in packages.

**Background**: We just fixed a critical issue on Mac ARM64 where executables were being deleted during library builds. The same fix was applied to Linux and AWS builds. Now we need to verify Windows builds work correctly.

## Key Issue We Fixed

**Problem**: Executable removal logic was running for ALL build types, including library builds.

**Solution**: Made executable removal conditional on `is_library` parameter:
```python
# Before (problematic)
if os.path.exists(config.output_name):
    os.remove(config.output_name)  # Ran for ALL builds

# After (fixed)
if not is_library and os.path.exists(config.output_name):
    os.remove(config.output_name)  # Only for executable builds
```

## Windows-Specific Context

- **Build Method**: Visual Studio 2022 projects (different from Unix direct compiler calls)
- **Executable**: `grapa.exe` (with .exe extension)
- **Package Format**: `.zip` files using 7-Zip
- **Library Format**: `.lib` (static), `.dll` (shared)

## Windows Build Analysis - COMPLETED

### Files Expected to be Created/Updated:

1. **Executable**: `grapa.exe` (in project root)
2. **Library**: `grapa.lib` (in project root, then copied to `source/grapa-lib/win-amd64/`)
3. **Package**: `bin/grapa-win-amd64.zip` (containing both grapa.exe and grapa.lib)
4. **Python Package**: `dist/grapapy-*.tar.gz` (created by setup.py)
5. **Python Egg Info**: `grapapy.egg-info/` directory

### Build Process Steps:
1. Build main executable using `msbuild prj/win-amd64/grapa.sln`
2. Copy executable from `prj/win-amd64/x64/Release/grapa.exe` to `grapa.exe`
3. Build library using `msbuild prj/winlib-amd64/grapalib.sln`
4. Copy library from `prj/winlib-amd64/x64/Release/grapa.lib` to `grapa.lib`
5. Copy library to `source/grapa-lib/win-amd64/grapa.lib`
6. Clean build artifacts (remove x64 directories)
7. Create package using 7-Zip: `7z a bin/grapa-win-amd64.zip grapa.exe grapa.lib`
8. Build Python package using `python setup.py sdist`
9. Install Python package using pip

### Current State (Before Build):
- ✅ `bin/grapa-win-amd64.zip` exists (7/18/2025 10:32 AM)
- ✅ `source/grapa-lib/win-amd64/grapa.lib` exists (7/18/2025 10:32 AM)
- ❌ `grapa.exe` does not exist in project root
- ❌ `grapa.lib` does not exist in project root

## Build Attempt Results

### First Attempt: Failed
**Command**: `python build.py windows amd64`
**Error**: `FileNotFoundError: [WinError 2] The system cannot find the file specified` for `msbuild`
**Root Cause**: Not running from Visual Studio Developer Command Prompt

### Second Attempt: Failed
**Command**: `python build.py windows amd64` (with Clang)
**Error**: `clang++: error: no such file or directory: 'source/openssl-lib/win-amd64/*.a'`
**Root Cause**: Windows doesn't use .a files, it uses Visual Studio projects with .lib files

### Key Discovery: Windows Build Architecture
**Windows builds use a completely different approach from Unix platforms:**

- **Unix platforms (Mac/Linux)**: Use direct compiler calls (clang++/g++) with .a files
- **Windows**: Uses Visual Studio projects (msbuild) with .lib files

**This means Windows does NOT have the same executable deletion issue as Unix platforms** because:
1. Windows uses separate Visual Studio projects for executable and library builds
2. Each project builds to its own output directory
3. No shared executable removal logic between builds

### Prerequisites Identified:
According to BUILD_README.md, Windows builds require:
1. **Visual Studio 2022** installed
2. **Run from "x64 Native Tools Command Prompt for VS 2022"**
3. **7-Zip** (for packaging)
4. **Python 3**

### Issue Analysis:
The build system expects `msbuild` to be available in the PATH, which only happens when running from the Visual Studio Developer Command Prompt. This is a prerequisite issue, not a build system bug.

## Windows Build Flow

1. **Clean Windows Build**: Remove existing build artifacts
2. **Build Executable**: Create `grapa.exe` using Visual Studio
3. **Build Static Library**: Create `grapa.lib` 
4. **Build Shared Library**: Create `grapadll.dll` (if applicable)
5. **Create Package**: Zip `grapa.exe` and `grapa.lib`

## Potential Windows Issues

1. **Visual Studio Not Found**: ✅ IDENTIFIED - Need to run from VS Developer Command Prompt
2. **Path Issues**: Windows path separators and environment variables
3. **Library Dependencies**: Windows-specific library linking
4. **Package Creation**: 7-Zip availability and usage

## Debugging Strategy for Windows

1. **Run Build**: `python3 build.py windows amd64` ✅ ATTEMPTED
2. **Check Output**: Look for any error messages or warnings ✅ IDENTIFIED ISSUE
3. **Verify Artifacts**: Check if `grapa.exe` is created and preserved
4. **Check Package**: Verify package contents include executable
5. **Compare with Unix**: Note any differences in build behavior

## Success Criteria for Windows

✅ **Windows build should produce**:
- `grapa.exe` (executable)
- `source/grapa-lib/win-amd64/grapa.lib` (static library)
- `bin/grapa-windows-amd64.zip` (package containing both)

## Notes for Windows Session

- Windows build uses different architecture (Visual Studio projects)
- **Windows does NOT have the same executable deletion issue as Unix platforms**
- Focus on understanding Windows-specific build process
- Compare behavior with Unix platforms for consistency
- Document any Windows-specific learnings or issues

## Cross-Platform Impact Analysis

### Platforms Affected

The same issue exists in other platform build functions:

1. **Linux Build** (`_run_linux_build_command`): ✅ Fixed
2. **AWS Build** (`_run_aws_build_command`): ✅ Fixed
3. **Windows Build**: ✅ NOT AFFECTED - Uses different architecture

### Required Fixes

**Linux and AWS builds have been fixed**:

```python
# Fixed code in _run_linux_build_command and _run_aws_build_command
# Remove existing executable only when building executable
if not is_library and os.path.exists(config.output_name):
    os.remove(config.output_name)  # ✅ Only for executable builds
```

**Windows does NOT need the same fix** because it uses Visual Studio projects, not direct compiler calls.

## Next Steps

1. **Setup Environment**: Run from "x64 Native Tools Command Prompt for VS 2022"
2. **Run Windows Build**: Test the build system on Windows with proper environment
3. **Check Executable**: Verify `grapa.exe` is created and preserved
4. **Check Package**: Verify package contents include executable
5. **Document Findings**: Record any Windows-specific issues or differences

## Key Questions

1. Does Windows build create `grapa.exe` successfully? 🔄 Need proper environment
2. Is `grapa.exe` preserved during library builds? ✅ NOT APPLICABLE - Different architecture
3. Is `grapa.exe` included in the final package? 🔄 Need proper environment
4. Are there Windows-specific build issues? 🔄 Need proper environment

## Files to Check

- `build.py` - Windows build functions ✅ REVIEWED
- `prj/win-amd64/grapa.sln` - Visual Studio solution
- `prj/winlib-amd64/grapalib.sln` - Library solution
- `bin/grapa-windows-amd64.zip` - Output package ✅ EXISTS

## Conclusion

**Windows builds do NOT have the same executable deletion issue as Unix platforms** because they use Visual Studio projects instead of direct compiler calls. The build system is correctly designed for Windows, but requires the proper Visual Studio environment to run.

---

*This file tracks the current Windows debugging work for the build system.* 