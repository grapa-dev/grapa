# Installing Grapa

Grapa is available through native package managers on all supported platforms, making installation quick and easy.

**Note**: Documentation and downloads are available on [GitHub](https://github.com/grapa-dev/grapa).

## 🚀 **New in Version 0.1.52: Enhanced Installation System**

Version 0.1.52 introduces a comprehensive installation system with:
- **Automated install scripts** for all platforms
- **Both static and shared libraries** for maximum compatibility
- **Python extension support** with `--python` build option
- **Cross-platform release management** via GitHub releases

## Installation Status

| Platform | Package Manager | Status | Manual Installation |
|----------|----------------|--------|-------------------|
| **macOS ARM64** | Homebrew | ✅ **Available** | ✅ Available |
| **Ubuntu/Debian ARM64** | apt | 🔄 **In Progress** | ✅ Available |
| **Ubuntu/Debian AMD64** | apt | 🔄 **In Progress** | ✅ Available |
| **Amazon Linux ARM64** | yum | 🔄 **In Progress** | ✅ Available |
| **Amazon Linux AMD64** | yum | 🔄 **In Progress** | ✅ Available |
| **Windows AMD64** | Manual Installer | ✅ **Available** | ✅ Available |

**Legend:**
- ✅ **Available**: Ready for installation via package manager
- 🔄 **In Progress**: Package manager integration in development
- ✅ **Available**: Manual installation available

## 🎯 **Recommended Installation Method: GitHub Releases**

The easiest way to install Grapa is to download the latest release from GitHub and use the automated install scripts:

### **Step 1: Download Latest Release**
1. Go to [GitHub Releases](https://github.com/grapa-dev/grapa/releases)
2. Download the appropriate package for your platform:
   - **Windows**: `grapa-0.1.52-win-amd64.zip`
   - **macOS**: `grapa-0.1.52-mac-arm64.tar.gz`
   - **Linux**: `grapa-0.1.52-linux-amd64.tar.gz` or `grapa-0.1.52-linux-arm64.tar.gz`
   - **AWS**: `grapa-0.1.52-aws-amd64.tar.gz` or `grapa-0.1.52-aws-arm64.tar.gz`

### **Step 2: Extract and Run Install Script**
Each package contains an automated install script that handles the entire installation process.

### **Alternative: Package Managers**
If you prefer package managers, some platforms offer them (see status table below).

---

## Quick Start

Choose your platform and run the appropriate command:

### macOS
```bash
brew install grapa
```

**Note**: Homebrew installation is in review. For immediate installation, see [Manual Installation](#manual-installation) below.

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install grapa
```

**Note**: Package manager installation is in progress. For immediate installation, see [Manual Installation](#manual-installation) below.

### Amazon Linux/Red Hat
```bash
sudo yum install grapa
```

**Note**: Package manager installation is in progress. For immediate installation, see [Manual Installation](#manual-installation) below.

### Windows
```powershell
# Manual installation required - see Windows section below
```

**Note**: Windows installation uses an automated installer script. See [Windows Installation](#windows-manual-installation) below.

## Verify Installation

After installation, verify that Grapa is working correctly:

```bash
grapa --version
```

You should see output similar to:
```
Grapa version 0.1.52
```

## Test Basic Functionality

Try running a simple Grapa command:

```bash
grapa -c "print('Hello, Grapa!')"
```

You should see:
```
Hello, Grapa!
```

## Platform-Specific Installation

### macOS (Homebrew)

**Prerequisites:**
- macOS 12.0 or later
- Apple Silicon Mac (arm64)
- Homebrew installed

**Installation:**
```bash
brew install grapa
```

**Update:**
```bash
brew upgrade grapa
```

### Ubuntu/Debian (apt)

**Prerequisites:**
- Ubuntu 20.04+ or Debian 11+
- amd64 or arm64 architecture

**Installation:**
```bash
sudo apt update
sudo apt install grapa
```

**Update:**
```bash
sudo apt update
sudo apt upgrade grapa
```

### Amazon Linux/Red Hat (yum)

**Prerequisites:**
- Amazon Linux 2+ or Red Hat Enterprise Linux 8+
- amd64 or arm64 architecture

**Installation:**
```bash
sudo yum install grapa
```

**Update:**
```bash
sudo yum update grapa
```

### Windows (Manual Installer)

**Prerequisites:**
- Windows 10 or later
- amd64 architecture
- PowerShell (Administrator privileges required)

**Installation:**
Download the latest Windows release and run the automated installer:

1. **Download the Windows package:**
   ```powershell
   # Download the latest Windows release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-win-amd64.zip" -OutFile "grapa-0.1.52-win-amd64.zip"
   ```

2. **Extract and run the installer:**
   ```powershell
   Expand-Archive -Path "grapa-win-amd64.zip" -DestinationPath "grapa-win-amd64" -Force
   cd grapa-win-amd64
   .\install-grapa.ps1
   ```

**Update:**
```powershell
# Download and install the latest version
Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-win-amd64.zip" -OutFile "grapa-0.1.52-win-amd64.zip"
Expand-Archive -Path "grapa-0.1.52-win-amd64.zip" -DestinationPath "grapa-0.1.52-win-amd64" -Force
cd grapa-0.1.52-win-amd64
.\install-grapa.ps1 -Force
```

**Uninstall:**
```powershell
# Run the installer with uninstall flag
.\install-grapa.ps1 -Uninstall
```

**Installation Location:**
After installation, Grapa will be available at:
- **Executable**: `C:\Program Files\Grapa\bin\grapa.exe`
- **Static Library**: `C:\Program Files\Grapa\lib\grapa_static.lib`
- **Shared Library**: `C:\Program Files\Grapa\lib\grapa.dll` (if available)
- **PATH**: Automatically added to system PATH for command-line access

**Note**: The installer automatically adds `C:\Program Files\Grapa\bin` to your system PATH during installation.

#### Manual Installation (Alternative)

If you prefer to install manually without the automated installer:

1. **Download the Windows package:**
   ```powershell
   # Download the latest Windows release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-win-amd64.zip" -OutFile "grapa-0.1.52-win-amd64.zip"
   ```

2. **Extract the package:**
   ```powershell
   Expand-Archive -Path "grapa-0.1.52-win-amd64.zip" -DestinationPath "grapa-0.1.52-win-amd64" -Force
   ```

3. **Install manually:**
   ```powershell
   # Create installation directory
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\bin" | Out-Null
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\lib" | Out-Null
   
   # Copy files
   Copy-Item -Path "grapa-0.1.52-win-amd64\grapa.exe" -Destination "C:\Program Files\Grapa\bin\" -Force
   Copy-Item -Path "grapa-0.1.52-win-amd64\grapa_static.lib" -Destination "C:\Program Files\Grapa\lib\" -Force
   if (Test-Path "grapa-0.1.52-win-amd64\grapa.dll") {
       Copy-Item -Path "grapa-0.1.52-win-amd64\grapa.dll" -Destination "C:\Program Files\Grapa\lib\" -Force
   }
   
   # Add to PATH (requires Administrator privileges)
   $currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
   $grapaPath = "C:\Program Files\Grapa\bin"
   if ($currentPath -notlike "*$grapaPath*") {
       [Environment]::SetEnvironmentVariable('PATH', $currentPath + ";" + $grapaPath, 'Machine')
   }
   ```

4. **Verify installation:**
   ```powershell
   grapa --version
   ```

## 🐍 **Python Extension Build**

Grapa 0.1.52 includes support for building Python extensions. If you need to build from source with Python support:

### **Build with Python Extension**
```bash
# Build Grapa with Python extension support
python3 build.py --python

# On Windows, use:
python build.py --python
```

### **Install Python Extension**
After building, install the Python extension:

**Windows:**
```powershell
pip install dist/grapapy-0.1.52.tar.gz
```

**Non-Windows:**
```bash
pip3 install dist/grapapy-0.1.52.tar.gz
```

### **Verify Python Integration**
```python
import grapapy
g = grapapy.grapa()
result = g.eval('2 + 2')
print(f'Grapa result: {result}')
```

---

## Manual Installation

If you prefer manual installation or your platform isn't supported by package managers, you can download and install Grapa directly.

### macOS (Manual Installation)

**Download and install the appropriate binary for your architecture:**

#### ARM64 (Apple Silicon Macs)
```bash
# Download the ARM64 binary
curl -L -o grapa-0.1.52-mac-arm64.tar.gz https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-mac-arm64.tar.gz

# Extract the binary
tar -xzf grapa-0.1.52-mac-arm64.tar.gz

# Make it executable
chmod +x grapa

# Move to a directory in your PATH
sudo mv grapa /usr/local/bin/

# Verify installation
grapa --version
```

**Test basic functionality:**
```bash
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

**Uninstall:**
```bash
sudo rm /usr/local/bin/grapa
```

### Ubuntu/Debian (Manual Installation)

**Download and install the appropriate package for your architecture:**

#### ARM64 (Apple Silicon, ARM servers)
```bash
# Download the ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-linux-arm64.tar.gz

# Extract and install
tar -xzf grapa-0.1.52-linux-arm64.tar.gz
cd grapa-0.1.52-linux-arm64
sudo ./install-grapa.sh

# Verify installation
grapa --version
```

#### AMD64 (Intel/AMD processors)
```bash
# Download the AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-linux-amd64.tar.gz

# Extract and install
tar -xzf grapa-0.1.52-linux-amd64.tar.gz
cd grapa-0.1.52-linux-amd64
sudo ./install-grapa.sh

# Verify installation
grapa --version
```

**Test basic functionality:**
```bash
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

**Uninstall:**
```bash
# Remove from system directories
sudo rm -f /usr/local/bin/grapa
sudo rm -f /usr/local/lib/libgrapa_static.a
sudo rm -f /usr/local/lib/libgrapa.so
```

### Amazon Linux (Manual Installation)

**Download and install the appropriate package for your architecture:**

#### ARM64 (Graviton processors)
```bash
# Download the ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-aws-arm64.tar.gz

# Extract and install
tar -xzf grapa-0.1.52-aws-arm64.tar.gz
cd grapa-0.1.52-aws-arm64
sudo ./install-grapa.sh

# Verify installation
grapa --version
```

#### AMD64 (Intel/AMD processors)
```bash
# Download the AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-aws-amd64.tar.gz

# Extract and install
tar -xzf grapa-0.1.52-aws-amd64.tar.gz
cd grapa-0.1.52-aws-amd64
sudo ./install-grapa.sh

# Verify installation
grapa --version
```

**Test basic functionality:**
```bash
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

**Uninstall:**
```bash
# Remove from system directories
sudo rm -f /usr/local/bin/grapa
sudo rm -f /usr/local/lib/libgrapa_static.a
sudo rm -f /usr/local/lib/libgrapa.so
```

### Windows (Manual Installation)

**Download and install the Windows package:**

#### Option 1: Automated Installation (Recommended)

1. **Download the Windows package:**
   ```powershell
   # Download the latest Windows release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-win-amd64.zip" -OutFile "grapa-0.1.52-win-amd64.zip"
   ```

2. **Extract the package:**
   ```powershell
   Expand-Archive -Path "grapa-0.1.52-win-amd64.zip" -DestinationPath "grapa-0.1.52-win-amd64" -Force
   ```

3. **Run the automated installer** (run PowerShell as Administrator):
   ```powershell
   cd grapa-0.1.52-win-amd64
   .\install-grapa.ps1
   ```

4. **Verify installation:**
   ```powershell
   grapa --version
   ```

#### Option 2: Manual Installation

1. **Download the Windows package:**
   ```powershell
   # Download the latest Windows release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-win-amd64.zip" -OutFile "grapa-0.1.52-win-amd64.zip"
   ```

2. **Extract the package:**
   ```powershell
   Expand-Archive -Path "grapa-0.1.52-win-amd64.zip" -DestinationPath "grapa-0.1.52-win-amd64" -Force
   ```

3. **Install manually** (run PowerShell as Administrator):
   ```powershell
   # Create installation directory
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\bin" | Out-Null
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\lib" | Out-Null
   
   # Copy files
   Copy-Item -Path "grapa-0.1.52-win-amd64\grapa.exe" -Destination "C:\Program Files\Grapa\bin\" -Force
   Copy-Item -Path "grapa-0.1.52-win-amd64\grapa_static.lib" -Destination "C:\Program Files\Grapa\lib\" -Force
   if (Test-Path "grapa-0.1.52-win-amd64\grapa.dll") {
       Copy-Item -Path "grapa-0.1.52-win-amd64\grapa.dll" -Destination "C:\Program Files\Grapa\bin\" -Force
   }
   
   # Add to PATH (requires Administrator privileges)
   $currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
   $grapaPath = "C:\Program Files\Grapa\bin"
   if ($currentPath -notlike "*$grapaPath*") {
       [Environment]::SetEnvironmentVariable('PATH', $currentPath + ";" + $grapaPath, 'Machine')
   }
   ```

4. **Verify installation:**
   ```powershell
   grapa --version
   ```

**Installation Location:**
After installation, Grapa will be available at:
- **Executable**: `C:\Program Files\Grapa\bin\grapa.exe`
- **Static Library**: `C:\Program Files\Grapa\lib\grapa_static.lib`
- **Shared Library**: `C:\Program Files\Grapa\lib\grapa.dll` (if available)
- **PATH**: Automatically added to system PATH for command-line access

**Uninstall:**
```powershell
# Run PowerShell as Administrator
.\install-grapa.ps1 -Uninstall
```

**Test basic functionality:**
```powershell
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

**Verify PATH and installation:**
```powershell
# Check if grapa is in PATH
Get-Command grapa

# Check grapa version
grapa --version

# Check where grapa is installed
(Get-Command grapa).Source

# Test basic functionality
grapa -c "2+2"
```

### Other Platforms

For other platforms, download the appropriate binary from the [GitHub releases page](https://github.com/grapa-dev/grapa/releases):

1. **Download** the binary for your platform and architecture
2. **Make it executable**: `chmod +x grapa`
3. **Move to PATH**: `sudo mv grapa /usr/local/bin/`
4. **Verify**: `grapa --version`

**Available packages:**
- `grapa-0.1.52-linux-amd64.tar.gz` - Linux AMD64 (includes install script)
- `grapa-0.1.52-linux-arm64.tar.gz` - Linux ARM64 (includes install script)
- `grapa-0.1.52-mac-arm64.tar.gz` - macOS ARM64 (includes install script)
- `grapa-0.1.52-win-amd64.zip` - Windows AMD64 (includes PowerShell install script)
- `grapa-0.1.52-aws-amd64.tar.gz` - AWS AMD64 (includes install script)
- `grapa-0.1.52-aws-arm64.tar.gz` - AWS ARM64 (includes install script)

**Note**: All packages now include automated install scripts and both static and shared libraries for maximum compatibility.

## Troubleshooting

### "grapa: command not found"

1. **Verify installation completed successfully:**
   ```bash
   # macOS
   brew list grapa
   
   # Ubuntu/Debian
   dpkg -l | grep grapa
   
   # Amazon Linux/Red Hat
   rpm -qa | grep grapa
   
   # Windows
   Test-Path "C:\Program Files\Grapa\bin\grapa.exe"
   ```

2. **Check if PATH is set correctly:**
   ```bash
   # macOS
   echo $PATH | grep -E "(homebrew|opt/homebrew)"
   
   # Linux
   echo $PATH | grep "/usr/bin"
   
   # Windows
   echo $env:PATH | Select-String "Grapa"
   ```

3. **Try restarting your terminal** after installation.

### Permission Denied

1. **Ensure you have appropriate permissions:**
   ```bash
   # Linux/macOS
   sudo chmod +x /usr/bin/grapa
   
   # Windows
   # Run PowerShell as Administrator
   ```

2. **Check file permissions:**
   ```bash
   # Linux/macOS
   ls -la $(which grapa)
   
   # Windows
   Get-ChildItem (Get-Command grapa).Source
   ```

### Package Manager Issues

1. **Update your package manager first:**
   ```bash
   # macOS
   brew update
   
   # Ubuntu/Debian
   sudo apt update
   
   # Amazon Linux/Red Hat
   sudo yum update
   ```

2. **Clear package manager cache:**
   ```bash
   # macOS
   brew cleanup
   
   # Ubuntu/Debian
   sudo apt clean
   
   # Amazon Linux/Red Hat
   sudo yum clean all
   ```

3. **Check network connectivity** and try again.

### Windows-Specific Issues

#### Installation Script Issues
If the `install-grapa.ps1` script fails:

1. **Check execution policy** (run PowerShell as Administrator):
   ```powershell
   Get-ExecutionPolicy
   Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
   ```

2. **Verify the script exists:**
   ```powershell
   Test-Path "install-grapa.ps1"
   ```

3. **Run the script with verbose output:**
   ```powershell
   .\install-grapa.ps1 -Verbose
   ```

#### Execution Policy Issues
If you encounter execution policy errors:

1. **Check current execution policy:**
   ```powershell
   Get-ExecutionPolicy
   ```

2. **Set appropriate execution policy** (run PowerShell as Administrator):
   ```powershell
   Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
   ```

#### PATH Issues
If Grapa is installed but not found in PATH:

1. **Check if Grapa is in the installation directory:**
   ```powershell
   Test-Path "C:\Program Files\Grapa\bin\grapa.exe"
   ```

2. **Add Grapa to PATH manually:**
   ```powershell
   $env:PATH += ";C:\Program Files\Grapa\bin"
   ```

3. **Restart PowerShell** to ensure PATH changes take effect.

#### PowerShell Profile Management
For persistent PATH changes across PowerShell sessions:

1. **Check if you have a PowerShell profile:**
   ```powershell
   Test-Path $PROFILE
   ```

2. **Create a PowerShell profile if it doesn't exist:**
   ```powershell
   if (!(Test-Path $PROFILE)) {
       New-Item -Type File -Path $PROFILE -Force
   }
   ```

3. **Add Grapa to your PowerShell profile:**
   ```powershell
   Add-Content $PROFILE "`$env:PATH += `";C:\Program Files\Grapa\bin`""
   ```

4. **Reload your profile:**
   ```powershell
   . $PROFILE
   ```

**Note**: This adds the standard Grapa installation path to your PowerShell profile. If you installed Grapa to a different location, replace `C:\Program Files\Grapa\bin` with your actual installation path.

## Getting Help

If you encounter issues not covered here:

1. **Check the [Troubleshooting Guide](troubleshooting.md)**
2. **Search existing issues** on [GitHub](https://github.com/grapa/grapa/issues)
3. **Create a new issue** with details about your platform and error messages

## Next Steps

After successful installation:

1. **Read the [Getting Started Guide](getting_started.md)** for your first Grapa program
2. **Explore the [CLI Quick Start](cli_quickstart.md)** for command-line usage
3. **Check out [Examples](examples.md)** for sample code and use cases
