# Grapa Windows Installation

This directory contains the files needed to install Grapa on Windows.

## Files Included

- `grapa.exe` - The Grapa executable
- `grapa_static.lib` - The Grapa static library for C++ development
- `grapa.dll` - The Grapa shared library (DLL) for runtime linking
- `install-grapa.ps1` - PowerShell installation script
- `README-Windows-Installation.md` - This file

## Quick Installation

1. **Extract the zip file** to a temporary directory
2. **Open PowerShell as Administrator**
3. **Navigate to the extracted directory**
4. **Run the installation script:** `.\install-grapa.ps1`

## Installation Script Features

The `install-grapa.ps1` script provides the following features:

### Installation Options
- **Standard installation:** `.\install-grapa.ps1`
- **Force installation (no prompts):** `.\install-grapa.ps1 -Force`
- **Uninstall:** `.\install-grapa.ps1 -Uninstall`
- **Show help:** `.\install-grapa.ps1 -Help`

### Smart Installation
- **Version detection:** Automatically detects existing installations and shows version information
- **Confirmation prompts:** Asks for confirmation before replacing existing installations
- **Clean replacement:** Completely removes old installation before installing new version
- **PATH management:** Automatically adds/removes Grapa from system PATH
- **Error handling:** Comprehensive error checking and user feedback
- **File validation:** Checks that all required files are present before installation

### Installation Location
Grapa will be installed to:
- **Executable:** `C:\Program Files\Grapa\bin\grapa.exe`
- **Static Library:** `C:\Program Files\Grapa\lib\grapa_static.lib`
- **Shared Library:** `C:\Program Files\Grapa\lib\grapa.dll` (if available)
- **System PATH:** Automatically configured for command-line access

## Manual Installation

If you prefer to install manually instead of using the script:

1. **Create installation directories:**
   ```powershell
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\bin"
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\lib"
   ```

2. **Copy files:**
   ```powershell
   Copy-Item -Path "grapa.exe" -Destination "C:\Program Files\Grapa\bin\" -Force
   Copy-Item -Path "grapa_static.lib" -Destination "C:\Program Files\Grapa\lib\" -Force
   if (Test-Path "grapa.dll") {
       Copy-Item -Path "grapa.dll" -Destination "C:\Program Files\Grapa\lib\" -Force
   }
   ```

3. **Add to system PATH:**
   ```powershell
   $currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
   $grapaPath = "C:\Program Files\Grapa\bin"
   if ($currentPath -notlike "*$grapaPath*") {
       [Environment]::SetEnvironmentVariable('PATH', $currentPath + ";" + $grapaPath, 'Machine')
   }
   ```

## Verification

After installation, verify that Grapa is working:

```powershell
# Check if grapa is available
grapa --version

# Test basic functionality
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

## Uninstallation

To uninstall Grapa:

```powershell
# Run PowerShell as Administrator
.\install-grapa.ps1 -Uninstall
```

Or manually:
```powershell
# Remove from PATH
$currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
$grapaPath = "C:\Program Files\Grapa\bin"
$newPath = ($currentPath -split ';' | Where-Object { $_ -ne $grapaPath }) -join ';'
[Environment]::SetEnvironmentVariable('PATH', $newPath, 'Machine')

# Remove installation directory
Remove-Item -Path "C:\Program Files\Grapa" -Recurse -Force
```

## Troubleshooting

### Execution Policy Issues
If you get execution policy errors:
```powershell
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Permission Issues
- Ensure PowerShell is running as Administrator
- Check that you have write access to `C:\Program Files\`

### PATH Issues
- Restart your terminal after installation
- Verify PATH was updated: `echo $env:PATH | Select-String "Grapa"`

### Version Conflicts
- The script will detect existing installations and prompt for confirmation
- When replacing, the old installation is completely removed for a clean install
- Use `-Force` flag to skip confirmation prompts

### Missing Files
- Ensure all required files (`grapa.exe`, `grapa_static.lib`) are in the same directory as the script
- The `grapa.dll` file is optional but recommended for shared library support

## Requirements

- **Windows 10 or later**
- **PowerShell 5.0 or later**
- **Administrator privileges** (for system-wide installation)
- **grapa.exe and grapa_static.lib** in the same directory as the script
- **grapa.dll** (optional, for shared library support)

## Support

For issues with installation:
1. Check the troubleshooting section above
2. Run the script with verbose output: `.\install-grapa.ps1 -Help`
3. Check the [main installation documentation](https://grapa-dev.github.io/grapa/installation/)
4. Report issues on [GitHub](https://github.com/grapa-dev/grapa/issues)
