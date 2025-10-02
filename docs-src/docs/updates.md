# Updating Grapa

Grapa is distributed through GitHub releases, making updates straightforward. You can check for updates and download the latest version directly from the releases page.

## Quick Update Commands

### Universal Installer (Recommended)
```bash
# Check current version
grapa --version

# Download and run the universal installer for the latest version
# Example for version 0.1.56:
curl -O https://github.com/grapa-dev/grapa/releases/download/v0.1.56/install-grapa-0.1.56.py
python3 install-grapa-0.1.56.py
```

### Manual Platform-Specific Update
```bash
# Check current version
grapa --version

# Download the appropriate package for your platform
# Example for version 0.1.56:

# macOS ARM64
curl -L -o grapa-0.1.56-mac-arm64.zip https://github.com/grapa-dev/grapa/releases/download/v0.1.56/grapa-0.1.56-mac-arm64.zip

# Linux AMD64
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.56/grapa-0.1.56-linux-amd64.zip

# Linux ARM64
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.56/grapa-0.1.56-linux-arm64.zip

# Windows AMD64 (PowerShell)
# Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.56/grapa-0.1.56-win-amd64.zip" -OutFile "grapa-0.1.56-win-amd64.zip"

# Extract and install
unzip grapa-0.1.56-<platform>.zip
cd grapa-0.1.56
python3 install-grapa.py
```

## Automatic Update Notifications

### GitHub Release Notifications

You can stay informed about new releases by:

1. **Watching the repository** on GitHub to receive notifications
2. **Checking the releases page** regularly: [GitHub Releases](https://github.com/grapa-dev/grapa/releases)
3. **Using the release manager script** to check for updates:
   ```bash
   python3 scripts/packaging/smart_release_manager.py --list
   ```

### Release Announcements

New releases are announced on:
- **GitHub Releases page** with detailed release notes
- **Repository homepage** showing the latest version
- **Release tags** in the git repository

## Checking Your Current Version

To see what version of Grapa you currently have installed:

```bash
grapa --version
```

Example output:
```
Grapa version 0.1.56
```

## Update Frequency

Grapa releases follow semantic versioning:

- **Patch releases** (0.1.55 → 0.1.56): Bug fixes and minor improvements
- **Minor releases** (0.1.55 → 0.2.0): New features, backward compatible
- **Major releases** (0.1.55 → 1.0.0): Breaking changes

Updates are typically available within a few days of a new release.

## Rollback (If Needed)

If you need to downgrade to a previous version:

### All Platforms (Universal Development Kit)
```bash
# Download and install specific version from GitHub releases
# Example for version 0.1.56:

# All platforms - download universal package
curl -L -o grapa-0.1.56.zip https://github.com/grapa-dev/grapa/releases/download/v0.1.56/grapa-0.1.56.zip

# Extract and install
unzip grapa-0.1.56.zip
cd grapa-0.1.56
python3 install-grapa.py

# On Windows, use:
# Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.56/grapa-0.1.56.zip" -OutFile "grapa-0.1.56.zip"
# Expand-Archive -Path "grapa-0.1.56.zip" -DestinationPath "grapa-0.1.56" -Force
# cd grapa-0.1.56
# python install-grapa.py
```

## Troubleshooting Updates

### Update Fails

If an update fails:

1. **Check your internet connection**
2. **Verify the release exists** on [GitHub Releases](https://github.com/grapa-dev/grapa/releases)
3. **Check file integrity** - ensure the downloaded file is complete
4. **Try downloading again** from the GitHub releases page

### Version Conflicts

If you encounter version conflicts:

1. **Remove the old version completely:**
   ```bash
   # macOS/Linux
   sudo rm -f /usr/local/bin/grapa
   sudo rm -f /usr/local/lib/libgrapa_static.a
   sudo rm -f /usr/local/lib/libgrapa.so
   
   # Windows (run PowerShell as Administrator)
   python install-grapa.py --uninstall
   ```

2. **Install the new version** from GitHub releases (see installation instructions above)

## Release Notes

For detailed information about what's new in each release:

- **GitHub Releases**: [https://github.com/grapa-dev/grapa/releases](https://github.com/grapa-dev/grapa/releases)
- **Changelog**: [https://github.com/grapa-dev/grapa/blob/main/CHANGELOG.md](https://github.com/grapa-dev/grapa/blob/main/CHANGELOG.md)

## Getting Help

If you encounter issues with updates:

1. **Check the [Troubleshooting Guide](troubleshooting.md)**
2. **Search existing issues** on [GitHub](https://github.com/grapa-dev/grapa/issues)
3. **Create a new issue** with details about your platform and error messages
