---
tags:
  - maintainer
  - debugging
  - build
  - troubleshooting
---

# Build System Debugging Guide

This document captures key learnings and debugging techniques for the Grapa build system.

## Recent Debugging Session: Mac ARM64 Executable Missing

**Date**: July 19, 2024  
**Issue**: Executable not being included in Mac ARM64 package  
**Root Cause**: Executable deletion during library builds  
**Resolution**: Conditional executable removal logic

### Problem Description

The automated build system was successfully creating the `grapa` executable for Mac ARM64, but it was not being included in the final package. The package only contained:
- `source/grapa-lib/mac-arm64/libgrapa.a` (static library)
- `source/grapa-other/mac-arm64/libgrapa.so` (shared library)

**Missing**: `grapa` (executable)

### Investigation Process

1. **Initial Assumption**: Executable was being deleted by library builds
2. **Verification**: Added `ls -la` after executable build to confirm creation
3. **Debug Output**: Added executable existence checks after each build step
4. **Root Cause Discovery**: Executable removal logic was running for all build types

### Root Cause Analysis

The issue was in the `_run_mac_build_command` function:

```python
# PROBLEMATIC CODE (before fix)
def _run_mac_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
    # Remove existing executable
    if os.path.exists(config.output_name):
        os.remove(config.output_name)  # ❌ This ran for ALL builds
```

**Problem**: The executable removal logic ran at the beginning of **every** build call, including when building libraries. This meant:
1. Build executable → Creates `grapa` ✅
2. Build static library → Removes `grapa` ❌
3. Build shared library → Removes `grapa` ❌
4. Create package → No `grapa` to include ❌

### Solution Applied

**Fixed Code**:
```python
# FIXED CODE (after fix)
def _run_mac_build_command(self, config: BuildConfig, is_library: bool = False, is_static: bool = False):
    # Remove existing executable only when building executable
    if not is_library and os.path.exists(config.output_name):
        os.remove(config.output_name)  # ✅ Only runs for executable builds
```

### Additional Improvements

1. **Subprocess Performance**: Switched from `subprocess.run()` to `os.system()` for executable builds
2. **Debug Output**: Added comprehensive logging to track file existence
3. **Cleanup Logic**: Modified cleanup to preserve executable while removing object files

### Verification

After the fix, the build process now correctly:
1. **Build executable** → Creates `grapa` ✅
2. **Build static library** → Preserves `grapa`, creates `libgrapa.a` ✅
3. **Build shared library** → Preserves `grapa`, creates `libgrapa.so` ✅
4. **Create package** → Includes all three files ✅

Final package contents:
```
grapa                                    # 7.1MB executable
source/grapa-lib/mac-arm64/libgrapa.a   # Static library
source/grapa-other/mac-arm64/libgrapa.so # Shared library
```

## Cross-Platform Impact Analysis

### Platforms Affected

The same issue exists in other platform build functions:

1. **Linux Build** (`_run_linux_build_command`): ❌ Same issue
2. **AWS Build** (`_run_aws_build_command`): ❌ Same issue
3. **Windows Build**: ✅ Not affected (different build architecture)

### Required Fixes

**Linux and AWS builds need the same fix**:

```python
# Current problematic code in _run_linux_build_command and _run_aws_build_command
# Remove existing executable
if os.path.exists(config.output_name):
    os.remove(config.output_name)  # ❌ Runs for all builds

# Should be:
# Remove existing executable only when building executable
if not is_library and os.path.exists(config.output_name):
    os.remove(config.output_name)  # ✅ Only for executable builds
```

## Debugging Techniques Learned

### 1. File Existence Tracking

```python
# Add after each build step
print(f"After {step_name} - Executable exists: {os.path.exists(config.output_name)}")
```

### 2. Directory Listing

```python
# Show what files exist
print("Files after build step:")
subprocess.run(["ls", "-la"], check=True)
```

### 3. Package Content Verification

```python
# Verify package contents
subprocess.run(["tar", "-tzf", f"bin/grapa-{config.target}.tar.gz"], check=True)
```

### 4. Conditional Debug Output

```python
# Add debug output to package creation
print(f"Checking for executable: {config.output_name}")
print(f"Executable exists: {os.path.exists(config.output_name)}")
if os.path.exists(config.output_name):
    print(f"Added executable to package: {config.output_name}")
else:
    print(f"Executable not found: {config.output_name}")
```

## Build System Architecture Insights

### Build Flow Understanding

The build system follows this pattern for each platform:
1. **Executable Build**: Creates main executable
2. **Static Library Build**: Creates `.a` library
3. **Shared Library Build**: Creates `.so` library
4. **Package Creation**: Bundles all artifacts

**Key Insight**: Each build step should preserve artifacts from previous steps unless explicitly cleaning up.

### Subprocess vs os.system()

**Discovery**: `os.system()` performs better than `subprocess.run()` for complex shell commands with glob patterns.

```python
# Better performance for shell globs
result = os.system(" ".join(cmd))  # ✅ Faster, handles globs better

# vs
subprocess.run(cmd, shell=True)    # ❌ Slower, can hang
```

## Prevention Strategies

### 1. Code Review Checklist

When reviewing build system changes, check:
- [ ] Does executable removal logic respect `is_library` parameter?
- [ ] Are artifacts preserved between build steps?
- [ ] Does cleanup logic preserve required files?
- [ ] Are glob patterns handled correctly?

### 2. Testing Strategy

For each platform, verify:
- [ ] Executable is created and preserved
- [ ] Libraries are created correctly
- [ ] Package contains all expected files
- [ ] File sizes are reasonable

### 3. Automated Verification

Add automated checks:
```python
def verify_build_artifacts(self, config: BuildConfig):
    """Verify that all expected artifacts were created"""
    artifacts = [
        config.output_name,  # Executable
        f"source/grapa-lib/{config.target}/libgrapa.a",  # Static lib
        f"source/grapa-other/{config.target}/libgrapa.so"  # Shared lib
    ]
    
    for artifact in artifacts:
        if not os.path.exists(artifact):
            raise RuntimeError(f"Missing build artifact: {artifact}")
        print(f"✅ Verified: {artifact}")
```

## Lessons Learned

1. **Assumption Validation**: Always verify assumptions with actual file system checks
2. **Build Flow Understanding**: Understand the complete build flow before making changes
3. **Cross-Platform Consistency**: Issues in one platform often exist in others
4. **Debug Output**: Comprehensive logging is essential for build system debugging
5. **Artifact Preservation**: Build steps should preserve artifacts unless explicitly cleaning up

## Future Improvements

1. **Automated Testing**: Add automated verification of build artifacts
2. **Cross-Platform Fixes**: Apply the same fix to Linux and AWS builds
3. **Build Validation**: Add pre-commit hooks to verify build artifacts
4. **Documentation**: Keep this debugging guide updated with new learnings

---

**Note**: This debugging session revealed that the build system was fundamentally sound, but had a subtle logic error in executable removal. The fix was simple but required careful investigation to identify the root cause. 

## Windows Debugging Session - Next Steps

**Date**: July 19, 2024  
**Status**: Ready for Windows debugging  
**Context**: Mac ARM64 issue resolved, cross-platform fixes applied

### Current State

✅ **Completed**:
- Mac ARM64 executable missing issue resolved
- Linux and AWS build functions fixed with same logic
- Debugging guide created with comprehensive learnings
- Cross-platform impact analysis completed

### Windows Debugging Context

**Platform**: Windows  
**Architecture**: AMD64  
**Build Method**: Visual Studio 2022 (different from Unix platforms)

### What to Check on Windows

1. **Build System Architecture**: Windows uses Visual Studio projects, not direct compiler calls
2. **Executable Removal Logic**: Check if Windows build has similar executable deletion issues
3. **Package Creation**: Verify if `grapa.exe` is included in Windows packages
4. **Build Flow**: Understand Windows-specific build process

### Windows-Specific Investigation Points

#### 1. Build Method Differences
```python
# Windows uses Visual Studio projects, not direct compiler calls
def build_windows(self, config: BuildConfig) -> bool:
    # Uses msbuild with .vcxproj files
    # Different from Unix platforms that use direct compiler calls
```

#### 2. Executable Naming
- **Unix**: `grapa` (no extension)
- **Windows**: `grapa.exe` (with .exe extension)

#### 3. Package Format
- **Unix**: `.tar.gz` files
- **Windows**: `.zip` files using 7-Zip

#### 4. Library Formats
- **Unix**: `.a` (static), `.so` (shared)
- **Windows**: `.lib` (static), `.dll` (shared)

### Debugging Commands for Windows

```bash
# Test Windows build
python3 build.py windows amd64

# Check if executable exists after build
dir grapa.exe

# Check package contents
7z l bin/grapa-windows-amd64.zip

# Verify build artifacts
dir source\grapa-lib\win-amd64\
dir source\grapa-other\win-amd64\
```

### Expected Windows Build Flow

1. **Clean Windows Build**: Remove existing build artifacts
2. **Build Executable**: Create `grapa.exe` using Visual Studio
3. **Build Static Library**: Create `grapa.lib` 
4. **Build Shared Library**: Create `grapadll.dll` (if applicable)
5. **Create Package**: Zip `grapa.exe` and `grapa.lib`

### Potential Windows Issues

1. **Visual Studio Not Found**: Ensure VS2022 is installed and accessible
2. **Path Issues**: Windows path separators and environment variables
3. **Library Dependencies**: Windows-specific library linking
4. **Package Creation**: 7-Zip availability and usage

### Debugging Strategy for Windows

1. **Run Build**: `python3 build.py windows amd64`
2. **Check Output**: Look for any error messages or warnings
3. **Verify Artifacts**: Check if `grapa.exe` is created and preserved
4. **Check Package**: Verify package contents include executable
5. **Compare with Unix**: Note any differences in build behavior

### Key Questions for Windows Debugging

1. Does the Windows build create `grapa.exe` successfully?
2. Is `grapa.exe` preserved during library builds?
3. Is `grapa.exe` included in the final package?
4. Are there any Windows-specific build issues?
5. Does the Windows build follow the same pattern as Unix builds?

### Files to Examine on Windows

1. **build.py**: Windows build functions
2. **Visual Studio Projects**: `.vcxproj` files in `prj/` directory
3. **Package Creation**: `_create_windows_package` function
4. **Build Artifacts**: `bin/grapa-windows-amd64.zip`

### Success Criteria for Windows

✅ **Windows build should produce**:
- `grapa.exe` (executable)
- `source/grapa-lib/win-amd64/grapa.lib` (static library)
- `bin/grapa-windows-amd64.zip` (package containing both)

### Notes for Windows Session

- Windows build uses different architecture (Visual Studio projects)
- May not have the same executable deletion issue as Unix platforms
- Focus on understanding Windows-specific build process
- Compare behavior with Unix platforms for consistency
- Document any Windows-specific learnings or issues

---

**Next Session**: When you restart on Windows, ask "What are we working on?" and I will discover that the next step is to debug the build system on Windows. 