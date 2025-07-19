# Windows Debugging Context - Quick Reference

## What We're Working On

**Task**: Debug the Grapa build system on Windows to ensure executables are properly created and included in packages.

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

## Quick Commands for Windows

```bash
# Test Windows build
python3 build.py windows amd64

# Check executable
dir grapa.exe

# Check package contents
7z l bin/grapa-windows-amd64.zip

# Check build artifacts
dir source\grapa-lib\win-amd64\
```

## Expected Output

Windows build should produce:
- ✅ `grapa.exe` (executable)
- ✅ `source/grapa-lib/win-amd64/grapa.lib` (static library)
- ✅ `bin/grapa-windows-amd64.zip` (package containing both)

## Key Questions

1. Does Windows build create `grapa.exe` successfully?
2. Is `grapa.exe` preserved during library builds?
3. Is `grapa.exe` included in the final package?
4. Are there Windows-specific build issues?

## Files to Check

- `build.py` - Windows build functions
- `prj/` directory - Visual Studio project files
- `bin/grapa-windows-amd64.zip` - Final package

## Success Criteria

The Windows build should work as smoothly as the Mac ARM64 build now does, with all artifacts properly created and packaged. 