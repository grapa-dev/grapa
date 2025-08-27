# Grapa Packaging

This directory contains all packaging configurations for distributing Grapa through native package managers.

## Overview

Grapa is distributed as a single executable for each supported platform, packaged for native package managers:

- **macOS**: Homebrew formula
- **Ubuntu/Debian**: .deb packages
- **Amazon Linux/Red Hat**: .rpm packages  
- **Windows**: Chocolatey package

## Directory Structure

```
packaging/
├── homebrew/          # macOS Homebrew formula
│   └── grapa.rb
├── debian/            # Ubuntu/Debian packages
│   ├── grapa/
│   │   ├── DEBIAN/
│   │   │   └── control
│   │   └── usr/bin/
│   └── build-deb.sh
├── rpm/               # Red Hat/Amazon Linux packages
│   ├── grapa.spec
│   ├── grapa/
│   │   └── usr/bin/
│   └── build-rpm.sh
├── chocolatey/        # Windows Chocolatey package
│   ├── grapa.nuspec
│   └── tools/
│       ├── chocolateyinstall.ps1
│       └── chocolateyuninstall.ps1
├── scripts/           # Build and release automation
│   └── build-all-packages.sh
├── templates/         # Package generation templates
└── docs/              # Packaging documentation
```

## Building Packages

### Build All Packages

To build packages for all platforms:

```bash
./packaging/scripts/build-all-packages.sh
```

This script will:
1. Calculate SHA256 checksums for all binaries
2. Update package files with correct checksums
3. Build .deb and .rpm packages
4. Prepare Homebrew and Chocolatey packages

### Prerequisites

- **Linux**: `dpkg-deb`, `rpmbuild` (optional)
- **macOS**: Homebrew (for testing)
- **Windows**: Chocolatey (for testing)

## Package Details

### Homebrew (macOS)

**Formula**: `packaging/homebrew/grapa.rb`

**Features**:
- Auto-detects architecture (arm64/amd64)
- Downloads appropriate binary
- Installs to `/usr/local/bin/` or `/opt/homebrew/bin/`
- Automatic PATH integration

**Testing**:
```bash
brew install --build-from-source ./packaging/homebrew/grapa.rb
```

### Debian Package

**Control File**: `packaging/debian/grapa/DEBIAN/control`

**Features**:
- Installs to `/usr/bin/grapa`
- No dependencies (self-contained)
- Standard Debian package structure

**Building**:
```bash
cd packaging/debian
dpkg-deb --build grapa
```

### RPM Package

**Spec File**: `packaging/rpm/grapa.spec`

**Features**:
- Installs to `/usr/bin/grapa`
- No dependencies (self-contained)
- Standard RPM package structure

**Building**:
```bash
cd packaging/rpm
rpmbuild -bb grapa.spec
```

### Chocolatey (Windows)

**Package**: `packaging/chocolatey/`

**Features**:
- Downloads Windows executable
- Installs to Chocolatey bin directory
- Automatic PATH integration via shim

**Testing**:
```powershell
choco pack packaging/chocolatey/grapa.nuspec
choco install grapa -s .
```

## Version Management

### Updating Version

1. **Update version in all package files**:
   - `packaging/homebrew/grapa.rb`
   - `packaging/debian/grapa/DEBIAN/control`
   - `packaging/rpm/grapa.spec`
   - `packaging/chocolatey/grapa.nuspec`

2. **Update build script version**:
   - `packaging/scripts/build-all-packages.sh`

3. **Rebuild packages**:
   ```bash
   ./packaging/scripts/build-all-packages.sh
   ```

### SHA256 Checksums

The build script automatically calculates and updates SHA256 checksums for:
- Homebrew formula (mac-arm64, mac-amd64)
- Chocolatey install script (win-amd64)

## Distribution

### Package Repositories

**Homebrew**: Submit formula to Homebrew core or maintain a custom tap
**Debian**: Upload .deb to Ubuntu/Debian repositories
**RPM**: Upload .rpm to Red Hat/Amazon Linux repositories  
**Chocolatey**: Upload package to Chocolatey community repository

### Release Process

1. **Build new version** of Grapa executables
2. **Run build script** to create packages
3. **Test packages** on target platforms
4. **Upload to repositories** or GitHub Releases
5. **Update documentation** with new version

## Testing

### Local Testing

**Homebrew**:
```bash
brew install --build-from-source ./packaging/homebrew/grapa.rb
```

**Debian**:
```bash
sudo dpkg -i packaging/debian/grapa_0.1.51_amd64.deb
```

**RPM**:
```bash
sudo rpm -i packaging/rpm/RPMS/x86_64/grapa-0.1.51-1.x86_64.rpm
```

**Chocolatey**:
```powershell
choco pack packaging/chocolatey/grapa.nuspec
choco install grapa -s .
```

### Verification

After installation, verify:
```bash
grapa --version
grapa -c "print('Hello, Grapa!')"
```

## Troubleshooting

### Common Issues

1. **SHA256 mismatch**: Recalculate checksums and update package files
2. **Permission denied**: Ensure executables have correct permissions
3. **PATH issues**: Verify package installs to correct location
4. **Architecture mismatch**: Ensure correct binary for target platform

### Getting Help

- Check package manager documentation
- Review build logs for specific errors
- Test on clean virtual machines
- Verify all dependencies are met

## Contributing

When adding new platforms or updating packages:

1. **Follow existing patterns** in package files
2. **Test thoroughly** on target platforms
3. **Update documentation** with new instructions
4. **Verify installation** and uninstallation work correctly
