# Scripts Directory

This directory contains organized scripts for building, testing, and deploying Grapa.

## Directory Structure

### 📦 `build/` - Build and Deployment Scripts
Core build scripts for all platforms and version management.
- **Main script**: `build_all_platforms.sh` - Master build orchestrator
- **Platform scripts**: Individual platform build scripts
- **Version management**: `bump_version_and_deploy.py`

### 🔄 `ci-cd/` - CI/CD and Artifact Management
Scripts for Continuous Integration/Continuous Deployment operations.
- **Windows artifacts**: Monitor and download Windows build artifacts
- **Workflow management**: Trigger GitHub Actions workflows

### ✅ `validation/` - Testing and Validation Scripts
Scripts for testing and validating builds, syntax, and functionality.
- **Platform validation**: Check status of all platform builds
- **Syntax validation**: Comprehensive Grapa syntax validation

### 📚 `documentation/` - Documentation Deployment
Scripts for building and deploying documentation.
- **Deployment scripts**: Deploy documentation to GitHub Pages
- **Cross-platform**: Both Bash and PowerShell versions

### 🗂️ `legacy/` - Legacy Scripts
Scripts that are no longer actively used but kept for reference.
- **Historical reference**: AWS-related scripts and older validation tools

## Quick Start

### Build All Platforms
```bash
./scripts/build/build_all_platforms.sh
```

### Check Build Status
```bash
./scripts/validation/check_platform_status.sh
```

### Deploy Documentation
```bash
./scripts/documentation/deploy_docs.sh
```

## Migration Notes

This directory was reorganized from a flat structure to improve organization and maintainability. All script references have been updated throughout the codebase.

### Old vs New Paths
- `./scripts/build_all_platforms.sh` → `./scripts/build/build_all_platforms.sh`
- `./scripts/check_platform_status.sh` → `./scripts/validation/check_platform_status.sh`
- `./scripts/monitor_and_download_windows.sh` → `./scripts/ci-cd/monitor_and_download_windows.sh`
- `./scripts/deploy_docs.sh` → `./scripts/documentation/deploy_docs.sh`

## Dependencies

- **Docker**: Required for Linux builds
- **Python 3**: Required for most scripts
- **GitHub CLI**: Required for Windows artifact management
- **Platform-specific tools**: Xcode (macOS), Visual Studio (Windows) 