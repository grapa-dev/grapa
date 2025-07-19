# WIP Context Snapshot (as of last save)

## Top Priority
- Debug the new build.py system on Windows to ensure executables are properly created and included in packages. We just fixed a critical issue on Mac ARM64 where executables were being deleted during library builds, and now need to verify Windows builds work correctly.

## Recent Major Achievement
- **Fixed Mac ARM64 Executable Missing Issue** - Discovered and fixed a bug where executable removal logic was running for ALL build types, including library builds. Applied the same fix to Linux and AWS builds.

## Current Build System Status
- **New Automated Build System**: Implemented comprehensive `build.py` system that replaces manual copy-paste process from `BUILD.md`
- **Cross-Platform Support**: Windows, Mac (ARM64/AMD64), Linux (ARM64/AMD64), AWS (ARM64/AMD64)
- **Issue Fixed**: Executable removal logic now conditional on `is_library` parameter
- **Next Step**: Debug Windows builds to ensure same fix is applied and working

## Crash Recovery Protocol
- All in-progress work is now saved frequently in `maintainers/INTERNAL_NOTES/WIP/`.
- The persistent TODO is updated with WIP file references and current status.
- After any crash or restart (including switching machines), resume from the latest WIP file and TODO state.

## Current Focus
- Immediate next step: Debug Windows build system to verify executable preservation
- Check if Windows build has similar executable deletion issues as Unix platforms
- Verify `grapa.exe` is included in Windows packages
- Document any Windows-specific build process differences

## Windows Debugging Context
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

---

*This file ensures seamless resumption of work on any machine or after any crash. See BUILD_DEBUGGING.md for detailed debugging context.* 