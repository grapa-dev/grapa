# Multi-Platform Build Guide

This guide provides comprehensive instructions for building Grapa across all supported platforms using the hybrid build system.

## Overview

The Grapa build system uses a **hybrid approach** combining:
- **Docker** for Linux builds (consistent environments)
- **GitHub Actions** for Windows builds (automated artifact collection)
- **Native builds** for macOS (direct compilation)
- **PyPI deployment** for Python package distribution

## Supported Platforms

- **Windows**: AMD64 (x86_64)
- **macOS**: AMD64 (x86_64) and ARM64 (Apple Silicon)
- **Linux**: AMD64 (x86_64) and ARM64
- **Python**: PyPI distribution for all platforms

## Prerequisites

### System Requirements

**All Platforms:**
- **Git** for version control
- **Python 3.6+** for build scripts
- **Docker** (for Linux builds)

**Platform-Specific:**
- **Windows**: Visual Studio Build Tools 2019+ or Visual Studio 2019+
- **macOS**: Xcode Command Line Tools (`xcode-select --install`)
- **Linux**: GCC 7.0+, build essentials (`sudo apt-get install build-essential`)

### Environment Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/grapa-dev/grapa.git
   cd grapa
   ```

2. **Install Python dependencies:**
   ```bash
   pip install setuptools wheel build twine
   ```

3. **Set up Docker (for Linux builds):**
   ```bash
   # Build Docker image
   docker build -t grapa-build -f Dockerfile.grapa-build .
   ```

## Build Options

### `build.py` Flags

The main build script supports various options:

| Flag | Description | Use Case |
|------|-------------|----------|
| `--clean` | Clean build artifacts before/after build | Fresh builds, debugging |
| `--preserve-exe` | Keep executable after build | Development, testing |
| `--preserve-dist` | Keep distribution files after build | Package inspection |
| `--test` | Run tests after build | Validation |
| `--test-only` | Run tests without building | Quick validation |
| `--target-platform` | Specify target platform (Mac only) | Cross-compilation |
| `--exe-only` | Build executable only | CLI development |
| `--python-only` | Build Python package only | Python integration |

### Build Modes

**Full Build (Default):**
```bash
python3 build.py
```

**Executable Only:**
```bash
python3 build.py --exe-only
```

**Python Package Only:**
```bash
python3 build.py --python-only
```

**Clean Build:**
```bash
python3 build.py --clean
```

## Build Individual Platforms

### Local Builds (Linux/macOS)

**Linux AMD64:**
```bash
python3 build.py --clean --exe-only
```

**Linux ARM64:**
```bash
python3 build.py --clean --exe-only
```

**macOS AMD64:**
```bash
python3 build.py --clean --exe-only --target-platform mac-amd64
```

**macOS ARM64:**
```bash
python3 build.py --clean --exe-only
```

### Windows Build (GitHub Actions)

Windows builds are automated via GitHub Actions:

1. **Trigger Windows build:**
   ```bash
   python3 scripts/build/bump_version_and_deploy.py --bump-version --commit-and-push
   ```

2. **Monitor and download artifacts:**
   ```bash
   ./scripts/ci-cd/monitor_and_download_windows.sh
   ```

## Version Bumping and Deployment

Grapa supports multiple methods for version bumping and deployment, each suited for different use cases:

### Method 1: Full Release (Recommended)

**Command:**
```bash
./scripts/build/build_all_platforms.sh --bump-version
```

**What it does:**
- Bumps version automatically (patch increment)
- Builds all 5 platforms (Linux AMD64/ARM64, macOS AMD64/ARM64, Windows AMD64)
- Commits and pushes version files
- Triggers Windows GitHub Actions workflow
- Downloads and commits Windows artifacts
- Validates all builds and tests

**When to use:** Production releases, major updates

**Version logic:** Automatically increments patch number (0.0.251 → 0.0.252)

### Method 2: Quick Bump

**Command:**
```bash
python3 scripts/build/bump_version_and_deploy.py --bump-version --commit-and-push
```

**What it does:**
- Bumps version automatically (patch increment)
- Commits and pushes version files
- Triggers Windows GitHub Actions workflow
- Downloads and commits Windows artifacts

**When to use:** Quick hotfixes, minor updates

**Version logic:** Automatically increments patch number (0.0.251 → 0.0.252)

### Method 3: Manual Version

**Command:**
```bash
python3 scripts/build/bump_version_and_deploy.py 0.0.252 --commit-and-push
```

**What it does:**
- Sets version to specified value
- Commits and pushes version files
- Triggers Windows GitHub Actions workflow
- Downloads and commits Windows artifacts

**When to use:** Specific version requirements, major/minor version bumps

**Version logic:** Uses specified version exactly

### Method 4: No Commit (Development)

**Command:**
```bash
python3 scripts/build/bump_version_and_deploy.py --bump-version
```

**What it does:**
- Bumps version automatically (patch increment)
- Updates version files locally only
- No Git operations

**When to use:** Development, testing, debugging

**Version logic:** Automatically increments patch number (0.0.251 → 0.0.252)

## PyPI Deployment

### Automated PyPI Deployment

GrapaPy is automatically deployed to PyPI when a new version tag is pushed:

1. **Create and push a version tag:**
   ```bash
   git tag v0.0.252
   git push origin v0.0.252
   ```

2. **GitHub Actions automatically:**
   - Builds Python package for all platforms
   - Uploads to PyPI
   - Verifies deployment

### Manual PyPI Deployment

**Primary Method - GitHub Actions (Recommended):**
```bash
./scripts/build/deploy_to_pypi.sh
```

**Backup Method - Local Build and Deploy (Emergency):**
```bash
./scripts/build/build_and_deploy_pypi.sh -v 0.0.252
```

**Monitor PyPI only:**
```bash
./scripts/build/deploy_to_pypi.sh --monitor-only
```

**Test PyPI installation only:**
```bash
./scripts/build/deploy_to_pypi.sh --test-only
```

### PyPI Deployment Methods

#### **Method 1: GitHub Actions Deployment (Recommended)**
- **Script**: `deploy_to_pypi.sh`
- **Process**: Creates version tag → Triggers GitHub Actions → Builds in cloud → Deploys to PyPI
- **Prerequisites**: GitHub CLI, authenticated GitHub account, committed artifacts
- **Security**: PyPI secrets stored in GitHub
- **Use case**: Standard deployment process

#### **Method 2: Local Build and Deploy (Emergency)**
- **Script**: `build_and_deploy_pypi.sh`
- **Process**: Rebuilds Python package locally → Deploys to PyPI
- **Prerequisites**: Local build environment, local PyPI secrets
- **Security**: Requires local PyPI API token
- **Use case**: Emergency deployment when GitHub Actions is unavailable

### PyPI Deployment Process

The PyPI deployment process is **separate** from the build process:

1. **Build Phase** (completed by `build_all_platforms.sh`):
   - Builds all 5 platforms
   - Commits platform artifacts to GitHub
   - Bumps version and commits version files

2. **Deploy Phase** (completed by `deploy_to_pypi.sh`):
   - Creates and pushes version tag
   - Triggers GitHub Actions PyPI deployment
   - Monitors deployment progress
   - Tests PyPI installation

### Prerequisites for PyPI Deployment

- ✅ **build_all_platforms.sh completed successfully**
- ✅ **All platform artifacts committed to GitHub**
- ✅ **GitHub CLI (gh) installed and authenticated**
- ✅ **PyPI API token configured in GitHub secrets**

### Deployment Verification

After deployment, verify the package works correctly:

```bash
# Clear pip cache
pip cache purge

# Install the new version
pip install --no-cache-dir grapapy --force-reinstall

# Test basic functionality
python3 -c "
import grapapy
xy = grapapy.grapa()
result = xy.eval('2 + 2;')
print(f'GrapaPy test: 2 + 2 = {result}')
assert result == 4, 'Basic functionality test failed'
print('✅ PyPI deployment verified successfully!')
"
```

## Version File Commit/Push Triggers

### Scenarios that DO commit/push version files:

1. **Full Release:** `./scripts/build/build_all_platforms.sh --bump-version`
2. **Quick Bump:** `python3 scripts/build/bump_version_and_deploy.py --bump-version --commit-and-push`
3. **Manual Version:** `python3 scripts/build/bump_version_and_deploy.py 0.0.252 --commit-and-push`

### Scenarios that do NOT commit/push version files:

1. **Development:** `python3 scripts/build/bump_version_and_deploy.py --bump-version` (no `--commit-and-push`)
2. **Manual Version (no commit):** `python3 scripts/build/bump_version_and_deploy.py 0.0.252` (no `--commit-and-push`)
3. **Build only:** `python3 build.py --exe-only`
4. **Test only:** `python3 build.py --test-only`

## Comprehensive Testing and Validation

### CLI Testing

All platforms are tested for CLI functionality:

```bash
# Test CLI extraction and basic functionality
./scripts/validation/check_platform_status.sh
```

**Expected output:**
```
✅ Linux AMD64: CLI extraction works
✅ Linux ARM64: CLI extraction works  
✅ macOS AMD64: CLI extraction works
✅ macOS ARM64: CLI extraction works
⚠️  Windows AMD64: CLI extraction works (requires Windows environment for testing)
```

### Python Testing

**Note:** Python package testing is handled separately from platform builds. The `build_all_platforms.sh` script focuses on platform binary builds and CLI testing only.

Python package testing occurs during the PyPI deployment phase:

```bash
# Test Python package installation and basic functionality
python3 build.py --python-only --test
```

**Expected output:**
```
✅ Linux AMD64: Python package works
✅ Linux ARM64: Python package works
✅ macOS AMD64: Python package works  
✅ macOS ARM64: Python package works
✅ Windows AMD64: Python package works
```

### PyPI Testing

After deployment, test PyPI installation:

```bash
# Monitor PyPI and test deployment
./scripts/validation/monitor_and_test_pypi.sh -v 0.0.252 --wait
```

**Expected output:**
```
✅ Version 0.0.252 is now available on PyPI!
✅ Basic functionality test passed!
✅ File operations test passed!
✅ Functional methods test passed!
✅ All tests passed!
```

## Complete Release Process

### Step-by-Step Workflow

1. **Prepare for release:**
   ```bash
   git pull origin main
   git status  # Ensure clean working directory
   ```

2. **Run full build and release:**
   ```bash
   ./scripts/build/build_all_platforms.sh --bump-version
   ```

3. **Verify all builds:**
   ```bash
   ./scripts/validation/check_platform_status.sh
   ```

4. **Deploy to PyPI:**
   ```bash
   git tag v0.0.252  # Use the version that was bumped
   git push origin v0.0.252
   ```

5. **Monitor PyPI deployment:**
   ```bash
   ./scripts/validation/monitor_and_test_pypi.sh -v 0.0.252 --wait
   ```

6. **Verify PyPI installation:**
   ```bash
   pip install --no-cache-dir grapapy --force-reinstall
   python3 -c "import grapapy; print(grapapy.grapa().eval('2 + 2;'))"
   ```

### Release Checklist

- [ ] **Pre-release:**
  - [ ] All tests pass locally
  - [ ] Documentation is up to date
  - [ ] Version files are consistent
  - [ ] Git repository is clean

- [ ] **Build and release:**
  - [ ] Run `./scripts/build/build_all_platforms.sh --bump-version`
  - [ ] Verify all 5 platforms build successfully
  - [ ] Verify CLI tests pass for all platforms
  - [ ] Verify CLI tests pass for all platforms
  - [ ] Verify Windows artifacts are downloaded and committed

- [ ] **PyPI deployment:**
  - [ ] Create and push version tag
  - [ ] Monitor GitHub Actions workflow
  - [ ] Verify PyPI upload completes
  - [ ] Test PyPI installation on all platforms

- [ ] **Post-release:**
  - [ ] Update release notes
  - [ ] Announce release
  - [ ] Monitor for issues

## Scripts Reference

| Script | Purpose | Location | Testing |
|--------|---------|----------|---------|
| `build_all_platforms.sh` | Master build script | `scripts/build/` | ✅ CLI only |
| `bump_version_and_deploy.py` | Version management | `scripts/build/` | ✅ Version files |
| `build_and_deploy_pypi.sh` | PyPI deployment | `scripts/build/` | ✅ PyPI upload |
| `monitor_and_download_windows.sh` | Windows artifact collection | `scripts/ci-cd/` | ✅ Artifact download |
| `monitor_and_test_pypi.sh` | PyPI monitoring and testing | `scripts/validation/` | ✅ PyPI deployment |
| `check_platform_status.sh` | Build validation | `scripts/validation/` | ✅ All platforms |

## Troubleshooting

### Common Issues

1. **Windows build fails:**
   - Check GitHub Actions workflow status
   - Verify `PYPI_API_TOKEN` secret is set
   - Check Windows artifacts download

2. **PyPI deployment fails:**
   - Verify version tag format (`v0.0.252`)
   - Check GitHub Actions workflow logs
   - Verify PyPI API token permissions

3. **Version mismatch:**
   - Clear pip cache: `pip cache purge`
   - Force reinstall: `pip install --force-reinstall grapapy`
   - Check version in `setup.py`

4. **Build failures:**
   - Check platform-specific requirements
   - Verify Docker setup (Linux builds)
   - Check development tools installation

### Debugging Commands

```bash
# Check current version
grep 'grapapy_version = "' setup.py

# Check PyPI versions
pip index versions grapapy

# Test local build
python3 build.py --exe-only --test

# Test PyPI installation
pip install --no-cache-dir grapapy --force-reinstall
python3 -c "import grapapy; print(grapapy.grapa().eval('2 + 2;'))"
```

## Environment Variables

| Variable | Purpose | Required |
|----------|---------|----------|
| `PYPI_API_TOKEN` | PyPI upload authentication | Yes (for deployment) |
| `GITHUB_TOKEN` | GitHub API access | Yes (for workflows) |
| `DOCKER_BUILDKIT` | Docker build optimization | No (optional) |

## Best Practices

1. **Always use `--bump-version` for releases**
2. **Test on all platforms before releasing**
3. **Monitor PyPI deployment after upload**
4. **Keep pip cache clean during testing**
5. **Use version tags for PyPI deployment**
6. **Verify PyPI installation on all platforms** 