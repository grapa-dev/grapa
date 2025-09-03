# Grapa Scripts Directory

This directory contains organized scripts for building, packaging, deploying, and maintaining the Grapa project.

## Directory Structure

### 📦 `packaging/` - Package Management & Deployment
- **Release Management**: `release_manager.py` - Python-based GitHub release manager
- **Package Deployment**: Scripts for Homebrew, Debian, RPM, Chocolatey, and VSCode extension
- **GitHub Releases**: Both Python and legacy shell script approaches

### 🔨 `build/` - Build System & Dependencies
- **Dependency Builds**: FLTK, OpenSSL, and other third-party libraries
- **Build Utilities**: Version bumping and build verification tools
- **Cross-platform**: Support for all target platforms (Mac, Linux, Windows, AWS)

### 📚 `documentation/` - Documentation Build & Deployment
- **User Docs**: Build and deploy user documentation to GitHub Pages
- **Validation**: Link checking, case sensitivity validation
- **Automation**: Streamlined documentation deployment workflow

### ✅ `validation/` - Testing & Quality Assurance
- **Test Scripts**: Automated testing and validation
- **Quality Checks**: Code quality and consistency verification

## Quick Start

### Create a New Release
```bash
# Using the new Python release manager (recommended)
python3 scripts/packaging/release_manager.py --version 0.1.52 --create

# Using legacy shell script
./scripts/packaging/create_github_release.sh
```

### Build Dependencies
```bash
# Build FLTK libraries for current platform
python3 scripts/build/build_fltk.py

# Build OpenSSL libraries
python3 scripts/build/build_openssl.py
```

### Deploy Documentation
```bash
# Build and validate user documentation
./scripts/documentation/deploy_docs.sh
```

## Script Categories

### Release Management
The **Python Release Manager** (`packaging/release_manager.py`) is the recommended tool for creating GitHub releases:
- **Cross-platform**: Works on Windows, macOS, and Linux
- **Automated**: Creates archives, tags, and releases automatically
- **Professional**: Generates comprehensive release notes
- **Quality Control**: Creates draft releases for review

### Package Deployment
Scripts for deploying to various package managers:
- **Homebrew**: macOS package manager
- **Debian/Ubuntu**: Linux package repositories
- **RPM**: Red Hat/Amazon Linux packages
- **Chocolatey**: Windows package manager
- **VSCode Extension**: Extension marketplace deployment

### Build System
Comprehensive build scripts for all dependencies:
- **FLTK**: GUI framework libraries
- **OpenSSL**: Cryptographic libraries
- **Cross-compilation**: Support for all target platforms
- **Verification**: Build validation and testing

## Prerequisites

### Common Requirements
- **Python 3.7+**: For Python-based scripts
- **Git**: For version control operations
- **GitHub CLI**: For release management (`gh`)

### Platform-Specific
- **macOS**: Xcode Command Line Tools
- **Linux**: GCC, Make, autotools
- **Windows**: Visual Studio or MinGW

## Best Practices

1. **Use Python scripts** when available (more robust, cross-platform)
2. **Test locally** before deploying to production
3. **Review releases** before publishing (draft mode enabled by default)
4. **Clean up** temporary files after operations
5. **Check prerequisites** before running scripts

## Troubleshooting

- **Missing dependencies**: Check platform-specific prerequisites
- **Build failures**: Verify source code availability and permissions
- **Release issues**: Ensure GitHub CLI is authenticated
- **Permission errors**: Use `sudo` for system-wide installations

For detailed information about specific script categories, see the README files in each subdirectory. 