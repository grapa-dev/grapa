# Updating Grapa

Grapa integrates with your system's package manager for easy updates. You can check for updates and upgrade Grapa using the same commands you use for other software on your system.

## Quick Update Commands

### macOS (Homebrew)
```bash
# Check for updates
brew outdated

# Update Grapa
brew upgrade grapa

# Update all packages (including Grapa)
brew upgrade
```

### Ubuntu/Debian (apt)
```bash
# Check for updates
sudo apt update

# See what's outdated
apt list --upgradable | grep grapa

# Update Grapa
sudo apt upgrade grapa

# Update all packages (including Grapa)
sudo apt upgrade
```

### Amazon Linux/Red Hat (yum)
```bash
# Check for updates
sudo yum check-update grapa

# Update Grapa
sudo yum update grapa

# Update all packages (including Grapa)
sudo yum update
```

### Windows (Manual Installer)
```powershell
# Download and install the latest version
Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa-win-amd64.zip" -OutFile "grapa-win-amd64.zip"
Expand-Archive -Path "grapa-win-amd64.zip" -DestinationPath "grapa-win-amd64" -Force
cd grapa-win-amd64
.\install-grapa.ps1 -Force
```

## Automatic Update Notifications

### System-Level Notifications

Most package managers will notify you when updates are available:

- **macOS**: System Preferences → Software Update (for system packages)
- **Linux**: Package manager update notifications in terminal
- **Windows**: Manual download and installation from GitHub releases

### Package Manager Notifications

When you run package manager commands, you'll see notifications about available updates:

```bash
# Homebrew shows when you run any command
Warning: grapa 0.1.51 is outdated and will be updated to 0.1.52

# apt shows when you run apt update
The following packages will be upgraded: grapa

# yum shows when you run yum check-update
grapa.x86_64    0.1.52-1    grapa-repo
```

## Checking Your Current Version

To see what version of Grapa you currently have installed:

```bash
grapa --version
```

Example output:
```
Grapa version 0.1.51
```

## Update Frequency

Grapa releases follow semantic versioning:

- **Patch releases** (0.1.51 → 0.1.52): Bug fixes and minor improvements
- **Minor releases** (0.1.51 → 0.2.0): New features, backward compatible
- **Major releases** (0.1.51 → 1.0.0): Breaking changes

Updates are typically available within a few days of a new release.

## Rollback (If Needed)

If you need to downgrade to a previous version:

### macOS (Homebrew)
```bash
# List available versions
brew info grapa

# Install specific version
brew install grapa@0.1.50

# Or uninstall and reinstall specific version
brew uninstall grapa
brew install grapa@0.1.50
```

### Ubuntu/Debian (apt)
```bash
# List available versions
apt-cache policy grapa

# Install specific version
sudo apt install grapa=0.1.50-1
```

### Amazon Linux/Red Hat (yum)
```bash
# List available versions
yum list available grapa

# Install specific version
sudo yum install grapa-0.1.50-1
```

### Windows (Manual Installer)
```powershell
# Download and install specific version
Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.50/grapa-win-amd64.zip" -OutFile "grapa-win-amd64.zip"
Expand-Archive -Path "grapa-win-amd64.zip" -DestinationPath "grapa-win-amd64" -Force
cd grapa-win-amd64
.\install-grapa.ps1 -Force
```

## Troubleshooting Updates

### Update Fails

If an update fails:

1. **Check your internet connection**
2. **Update your package manager first:**
   ```bash
   # macOS
   brew update
   
   # Ubuntu/Debian
   sudo apt update
   
   # Amazon Linux/Red Hat
   sudo yum update
   
   # Windows
   choco upgrade all
   ```

3. **Clear package manager cache:**
   ```bash
   # macOS
   brew cleanup
   
   # Ubuntu/Debian
   sudo apt clean
   
   # Amazon Linux/Red Hat
   sudo yum clean all
   ```

4. **Try the update again**

### Version Conflicts

If you encounter version conflicts:

1. **Remove the old version completely:**
   ```bash
   # macOS
   brew uninstall grapa
   
   # Ubuntu/Debian
   sudo apt remove grapa
   
   # Amazon Linux/Red Hat
   sudo yum remove grapa
   
   # Windows
   choco uninstall grapa
   ```

2. **Install the new version:**
   ```bash
   # macOS
   brew install grapa
   
   # Ubuntu/Debian
   sudo apt install grapa
   
   # Amazon Linux/Red Hat
   sudo yum install grapa
   
   # Windows
   choco install grapa
   ```

## Release Notes

For detailed information about what's new in each release:

- **GitHub Releases**: [https://github.com/grapa/grapa/releases](https://github.com/grapa/grapa/releases)
- **Changelog**: [https://github.com/grapa/grapa/blob/main/CHANGELOG.md](https://github.com/grapa/grapa/blob/main/CHANGELOG.md)

## Getting Help

If you encounter issues with updates:

1. **Check the [Troubleshooting Guide](troubleshooting.md)**
2. **Search existing issues** on [GitHub](https://github.com/grapa/grapa/issues)
3. **Create a new issue** with details about your platform and error messages
