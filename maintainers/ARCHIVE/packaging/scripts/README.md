# Packaging Deployment Scripts

This directory contains scripts for deploying Grapa packages to various package managers.

## Scripts

### GitHub Release
- `create_github_release.sh` - Create GitHub release with all platform binaries (legacy shell script)
- `release_manager.py` - **NEW**: Python-based release manager with enhanced functionality

### Homebrew Deployment
- `deploy_homebrew.sh` - Deploy Homebrew formula to Homebrew core

## Usage

### Create GitHub Release

**Option 1: Python Release Manager (Recommended)**
```bash
# Create a new release
python3 scripts/packaging/release_manager.py --version 0.1.52 --create

# Delete current release
python3 scripts/packaging/release_manager.py --delete-current

# List all releases
python3 scripts/packaging/release_manager.py --list

# Clean up local files
python3 scripts/packaging/release_manager.py --cleanup
```

**Option 2: Legacy Shell Script**
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

### Deploy Homebrew Formula

**Prerequisites:**
- Homebrew installed
- GitHub CLI (gh) installed and authenticated
- Git installed
- Binary available at `bin/grapa`

**Test Formula Locally:**
```bash
./scripts/packaging/deploy_homebrew.sh --test-only
```

**Update SHA256 and Test:**
```bash
./scripts/packaging/deploy_homebrew.sh --update-sha256
```

**Full Deployment to Homebrew Core:**
```bash
./scripts/packaging/deploy_homebrew.sh --submit
```

## Process Overview

### GitHub Release Process

**Python Release Manager (Recommended)**
1. **Validation**: Checks prerequisites and required binaries
2. **Package Creation**: Creates compressed archives for each platform
3. **Git Integration**: Creates and pushes version tags automatically
4. **Release Creation**: Creates GitHub release with comprehensive notes
5. **Asset Upload**: Uploads all platform archives automatically
6. **Quality Control**: Creates draft release for review before publishing

**Legacy Shell Script**
1. **Validation**: Checks prerequisites and required binaries
2. **Asset Preparation**: Copies binaries with correct names
3. **Tag Creation**: Creates and pushes Git tag
4. **Release Creation**: Creates GitHub release with assets
5. **Cleanup**: Removes temporary files

### Homebrew Deployment Process

1. **Validates Prerequisites**:
   - Homebrew, GitHub CLI, Git installed
   - GitHub CLI authenticated
   - Formula file exists

2. **Updates Formula**:
   - Calculates SHA256 of binary
   - Updates placeholder in formula file

3. **Tests Locally**:
   - Creates test tap
   - Installs formula
   - Tests binary execution
   - Runs formula tests
   - Cleans up test environment

4. **Submits to Homebrew Core**:
   - Clones homebrew-core repository
   - Creates feature branch
   - Copies formula
   - Creates pull request with detailed description

### What the Script Does

**Python Release Manager Advantages:**
- **Cross-platform**: Works on Windows, macOS, and Linux
- **Better error handling**: Comprehensive error checking and user feedback
- **Automated notes**: Generates professional release notes automatically
- **Platform archives**: Creates `.zip` files for Windows compatibility
- **Draft releases**: Creates draft releases for review before publishing
- **Cleanup options**: Built-in cleanup and management functions

**Legacy Shell Script:**
- **Unix-focused**: Designed for Unix-like systems
- **Manual process**: Requires more manual intervention
- **Basic functionality**: Core release creation without advanced features

## Monitoring

After submission, monitor the pull request:
- Homebrew maintainers will review
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

## Future Scripts

Additional deployment scripts will be added for:
- Debian/Ubuntu (apt)
- Red Hat/Amazon Linux (yum)
- Windows (Chocolatey)
- Cross-platform package building
