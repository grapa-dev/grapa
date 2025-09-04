# Packaging Deployment Scripts

This directory contains scripts for deploying Grapa packages to various package managers.

## Scripts

### GitHub Release
- `universal_release_manager.py` - **CURRENT**: Universal development kit release manager
- `create_github_release.sh` - Legacy shell script (deprecated)
- `release_manager.py` - Legacy platform-specific release manager (deprecated)

## Usage

### Create GitHub Release

**Option 1: Universal Release Manager (Current)**
```bash
# Create a new universal development kit release
python3 scripts/packaging/universal_release_manager.py --version 0.1.53 --create

# Delete current release
python3 scripts/packaging/universal_release_manager.py --delete-current

# List all releases
python3 scripts/packaging/universal_release_manager.py --list

# Clean up local files
python3 scripts/packaging/universal_release_manager.py --cleanup
```

**Option 2: Legacy Scripts (Deprecated)**
**Prerequisites:**
- GitHub CLI (gh) installed and authenticated
- Git installed
- All platform binaries built and available in `bin/`

**Preview Release Creation:**
```bash
./scripts/packaging/create_github_release.sh --dry-run
```

**Create GitHub Release:**
```bash
./scripts/packaging/create_github_release.sh
```

**Update SHA256 and Test:**
```bash
./scripts/packaging/deploy_homebrew.sh --update-sha256
```

## Process Overview

### GitHub Release Process

**Universal Release Manager (Current)**
1. **Validation**: Checks prerequisites and required binaries
2. **Package Creation**: Creates single universal archive containing all platforms
3. **Git Integration**: Creates and pushes version tags automatically
4. **Release Creation**: Creates GitHub release with comprehensive notes
5. **Asset Upload**: Uploads universal development kit automatically
6. **Quality Control**: Creates published release with universal package

**Legacy Shell Script**
1. **Validation**: Checks prerequisites and required binaries
2. **Asset Preparation**: Copies binaries with correct names
3. **Tag Creation**: Creates and pushes Git tag
4. **Release Creation**: Creates GitHub release with assets
5. **Cleanup**: Removes temporary files

### What the Script Does

**Universal Release Manager Advantages:**
- **Universal package**: Single archive containing all platforms and development tools
- **Cross-platform**: Works on Windows, macOS, and Linux
- **Better error handling**: Comprehensive error checking and user feedback
- **Automated notes**: Generates professional release notes automatically
- **Development kit**: Includes sample code, CMake build system, and universal installer
- **Self-contained**: All dependencies included, no external requirements
- **Cleanup options**: Built-in cleanup and management functions

**Legacy Shell Script:**
- **Unix-focused**: Designed for Unix-like systems
- **Manual process**: Requires more manual intervention
- **Basic functionality**: Core release creation without advanced features

## Monitoring

After submission, monitor the pull request:
- Automated tests will run
- PR will be merged if approved

## Troubleshooting

### Common Issues

**GitHub CLI Not Authenticated:**
```bash
gh auth login
```

**Formula Validation Fails:**
- Check for placeholder values
- Ensure binary exists
- Verify formula syntax

**Installation Test Fails:**
- Ensure binary is executable
- Check platform compatibility
- Verify SHA256 matches
