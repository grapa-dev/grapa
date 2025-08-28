---
tags:
  - maintainer
  - deployment
  - packaging
  - distribution
---

# Packaging Deployment

This document covers the deployment of Grapa packages to various package managers and distribution channels.

## 🎯 **Overview**

The packaging deployment system handles distribution of Grapa executables to:
- **GitHub Releases** - Direct binary downloads
- **Homebrew** - macOS package manager
- **Chocolatey** - Windows package manager
- **Debian/Ubuntu** - Linux package managers
- **RPM** - Red Hat/Fedora package managers
- **VSCode Extension** - Visual Studio Marketplace

## 📁 **Scripts Structure**

```
scripts/packaging/
├── create_github_release.sh      # GitHub release with binaries
├── deploy_homebrew.sh           # Homebrew formula deployment
├── deploy_chocolatey.ps1        # Chocolatey package deployment
├── deploy_debian.sh             # Debian/Ubuntu package deployment
├── deploy_rpm.sh                # RPM package deployment
├── submit_ubuntu_official.sh    # Ubuntu official repository submission
├── deploy_vscode_extension.sh   # VSCode extension deployment
└── README.md                    # Script overview and usage
```

## 🚀 **Deployment Scripts**

### **GitHub Release** (`create_github_release.sh`)
Creates GitHub releases with platform-specific binaries.

**Usage:**
```bash
# Preview release
./scripts/packaging/create_github_release.sh --dry-run

# Create release
./scripts/packaging/create_github_release.sh
```

**Process:**
1. Validates prerequisites and binaries
2. Prepares assets with correct names
3. Creates and pushes Git tag
4. Creates GitHub release with assets
5. Cleans up temporary files

### **Homebrew Deployment** (`deploy_homebrew.sh`)
Deploys Homebrew formula to Homebrew core repository.

**Usage:**
```bash
# Test formula locally
./scripts/packaging/deploy_homebrew.sh --test-only

# Update SHA256 and test
./scripts/packaging/deploy_homebrew.sh --update-sha256

# Submit to Homebrew core
./scripts/packaging/deploy_homebrew.sh --submit
```

**Process:**
1. Validates prerequisites and formula syntax
2. Calculates and updates SHA256 of binary
3. Creates test tap and validates installation
4. Creates pull request to Homebrew core
5. Monitors PR for approval

### **Chocolatey Deployment** (`deploy_chocolatey.ps1`)
Deploys Chocolatey package for Windows.

**Usage:**
```powershell
# Test package locally
.\scripts\packaging\deploy_chocolatey.ps1 -TestOnly

# Submit to Chocolatey
.\scripts\packaging\deploy_chocolatey.ps1 -Submit
```

**Process:**
1. Validates package structure and metadata
2. Tests package installation locally
3. Submits to Chocolatey repository
4. Monitors for approval

### **Debian/Ubuntu Deployment** (`deploy_debian.sh`)
Deploys Debian packages for Ubuntu/Debian systems.

**Usage:**
```bash
# Build package
./scripts/packaging/deploy_debian.sh --build

# Submit to PPA
./scripts/packaging/deploy_debian.sh --submit
```

**Process:**
1. Creates Debian package structure
2. Builds .deb package
3. Signs package with GPG key
4. Uploads to PPA or official repository

### **RPM Deployment** (`deploy_rpm.sh`)
Deploys RPM packages for Red Hat/Fedora systems.

**Usage:**
```bash
# Build package
./scripts/packaging/deploy_rpm.sh --build

# Submit to Copr
./scripts/packaging/deploy_rpm.sh --submit
```

**Process:**
1. Creates RPM spec file
2. Builds .rpm package
3. Signs package with GPG key
4. Uploads to Copr or official repository

### **Ubuntu Official Repository** (`submit_ubuntu_official.sh`)
Submits package to Ubuntu official repositories.

**Usage:**
```bash
# Submit to Ubuntu
./scripts/packaging/submit_ubuntu_official.sh
```

**Process:**
1. Prepares package for Ubuntu standards
2. Submits to Ubuntu package review process
3. Monitors for inclusion in official repositories

### **VSCode Extension** (`deploy_vscode_extension.sh`)
Deploys VSCode extension to Visual Studio Marketplace.

**Usage:**
```bash
# Build only
./scripts/packaging/deploy_vscode_extension.sh --build-only

# Build and publish
./scripts/packaging/deploy_vscode_extension.sh --publish
```

**Process:**
1. Builds and packages extension
2. Triggers GitHub Actions workflow
3. Publishes to Visual Studio Marketplace
4. Monitors verification process

## 📋 **Prerequisites**

### **Common Requirements**
- **Git** - Version control
- **GitHub CLI** - GitHub API access
- **Binary files** - Platform-specific executables in `bin/`

### **Platform-Specific Requirements**

#### **Homebrew**
- Homebrew installed
- GitHub CLI authenticated
- Formula file in repository

#### **Chocolatey**
- Chocolatey installed
- PowerShell execution policy
- Package metadata files

#### **Debian/Ubuntu**
- Debian packaging tools
- GPG key for signing
- Launchpad PPA access

#### **RPM**
- RPM build tools
- GPG key for signing
- Copr or Fedora access

#### **VSCode Extension**
- Node.js and npm
- VSCE tool
- Visual Studio Marketplace publisher account

## 🔄 **Deployment Workflow**

### **Release Process**
1. **Build binaries** for all platforms
2. **Create GitHub release** with binaries
3. **Deploy to package managers** in parallel
4. **Monitor submissions** for approval
5. **Update documentation** with new version

### **Version Management**
- **Semantic versioning** (MAJOR.MINOR.PATCH)
- **Consistent versioning** across all packages
- **Release notes** for each version
- **Changelog** maintenance

## 🐛 **Troubleshooting**

### **Common Issues**

#### **GitHub CLI Authentication**
```bash
gh auth login
gh auth status
```

#### **Binary Not Found**
- Verify binaries exist in `bin/` directory
- Check file permissions and executability
- Ensure correct platform-specific names

#### **Package Validation Fails**
- Check package metadata and structure
- Verify dependencies and requirements
- Test package installation locally

#### **Submission Rejected**
- Review package manager guidelines
- Check for naming conflicts
- Verify package quality standards

### **Debug Mode**
Most scripts support debug output:
```bash
# Enable verbose output
./scripts/packaging/deploy_homebrew.sh --verbose

# Check prerequisites
./scripts/packaging/deploy_homebrew.sh --check-prerequisites
```

## 📊 **Monitoring**

### **GitHub Release**
- Monitor download statistics
- Check for installation issues
- Review user feedback

### **Package Managers**
- Monitor pull request status
- Check automated test results
- Review maintainer feedback

### **VSCode Extension**
- Monitor marketplace statistics
- Check user ratings and reviews
- Track installation metrics

## 🔄 **Maintenance**

### **Regular Tasks**
- **Update package metadata** for new versions
- **Monitor package health** across platforms
- **Respond to user issues** and feedback
- **Maintain package quality** standards

### **Version Updates**
1. **Update version** in all package files
2. **Rebuild binaries** for all platforms
3. **Update package metadata** and dependencies
4. **Deploy to all channels** using scripts
5. **Monitor deployment** and user feedback

### **Quality Assurance**
- **Test packages** before deployment
- **Validate installation** on target platforms
- **Check for conflicts** with existing packages
- **Verify security** and compliance requirements

## 📞 **Support**

### **For Users**
- **GitHub Issues** - Report installation problems
- **Package manager support** - Platform-specific issues
- **Documentation** - Installation and usage guides

### **For Maintainers**
- **Script documentation** - Detailed usage instructions
- **Package manager guidelines** - Platform-specific requirements
- **Deployment logs** - Troubleshooting and debugging

---

**Last Updated**: August 28, 2024  
**Status**: All packaging scripts operational and documented
