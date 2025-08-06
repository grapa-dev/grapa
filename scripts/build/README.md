# Build Scripts

This directory contains scripts for building Grapa across different platforms and architectures.

## Scripts

### Core Build Scripts
- `build_all_platforms.sh` - Master build orchestrator for all platforms
- `bump_version_and_deploy.py` - Version management and deployment

### Platform-Specific Build Scripts
- `build_grapa_linux_amd64.sh` - Linux AMD64 build
- `build_grapa_linux_arm64.sh` - Linux ARM64 build
- `build_grapa_macos_amd64.sh` - macOS AMD64 build (cross-compilation)
- `build_grapa_windows_amd64.sh` - Windows AMD64 build

### Specialized Build Scripts
- `build_linux_amd64_native.sh` - Native Linux AMD64 build
- `rebuild_linux_amd64_libraries.sh` - Rebuild Linux AMD64 libraries

## Usage

For most users, the main script to use is:
```bash
./scripts/build/build_all_platforms.sh
```

For version bumps and releases:
```bash
./scripts/build/build_all_platforms.sh --bump-version
```

## Dependencies

These scripts depend on:
- Docker (for Linux builds)
- Python 3
- GitHub CLI (for Windows builds)
- Platform-specific build tools 