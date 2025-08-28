# Installing Grapa

Grapa is available through native package managers on all supported platforms, making installation quick and easy.

**Note**: Documentation and downloads are available on [GitHub](https://github.com/grapa-dev/grapa).

## Installation Status

| Platform | Package Manager | Status | Manual Installation |
|----------|----------------|--------|-------------------|
| **macOS ARM64** | Homebrew | ✅ **Available** | ✅ Available |
| **Ubuntu/Debian ARM64** | apt | 🔄 **In Progress** | ✅ Available |
| **Ubuntu/Debian AMD64** | apt | 🔄 **In Progress** | ✅ Available |
| **Amazon Linux ARM64** | yum | 🔄 **In Progress** | ✅ Available |
| **Amazon Linux AMD64** | yum | 🔄 **In Progress** | ✅ Available |
| **Windows AMD64** | Chocolatey | ✅ **Available** | ✅ Available |

**Legend:**
- ✅ **Available**: Ready for installation via package manager
- 🔄 **In Progress**: Package manager integration in development
- ✅ **Available**: Manual installation available

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
choco install grapa
```

**Note**: Chocolatey installation is available. For manual installation, see [Manual Installation](#manual-installation) below.

## Verify Installation

After installation, verify that Grapa is working correctly:

```bash
grapa --version
```

You should see output similar to:
```
Grapa version 0.1.51
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

### Windows (Chocolatey)

**Prerequisites:**
- Windows 10 or later
- Chocolatey package manager installed
- amd64 architecture

**Installation:**
```powershell
choco install grapa
```

**Update:**
```powershell
choco upgrade grapa
```

**Uninstall:**
```powershell
choco uninstall grapa
```

**Installation Location:**
After installation, Grapa will be available at:
- **Executable**: `C:\Program Files\Grapa\bin\grapa.exe`
- **Library**: `C:\Program Files\Grapa\lib\grapa.lib`
- **PATH**: Automatically added to system PATH for command-line access

**Note**: The Chocolatey package automatically adds `C:\Program Files\Grapa\bin` to your system PATH during installation.

#### Installing from GitHub Release (If Package Not Published)

If the Chocolatey package hasn't been published yet, you can install directly from a GitHub release:

1. **Download the Windows package:**
   ```powershell
   # Download the latest Windows release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa-win-amd64.zip" -OutFile "grapa-win-amd64.zip"
   ```

2. **Extract the package:**
   ```powershell
   Expand-Archive -Path "grapa-win-amd64.zip" -DestinationPath "grapa-win-amd64" -Force
   ```

3. **Install manually:**
   ```powershell
   # Create installation directory
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\bin" | Out-Null
   New-Item -ItemType Directory -Force -Path "C:\Program Files\Grapa\lib" | Out-Null
   
   # Copy files
   Copy-Item -Path "grapa-win-amd64\grapa.exe" -Destination "C:\Program Files\Grapa\bin\" -Force
   Copy-Item -Path "grapa-win-amd64\grapa.lib" -Destination "C:\Program Files\Grapa\lib\" -Force
   
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

## Manual Installation

If you prefer manual installation or your platform isn't supported by package managers, you can download and install Grapa directly.

### macOS (Manual Installation)

**Download and install the appropriate binary for your architecture:**

#### ARM64 (Apple Silicon Macs)
```bash
# Download the ARM64 binary
curl -L -o grapa-mac-arm64.tar.gz https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa-mac-arm64.tar.gz

# Extract the binary
tar -xzf grapa-mac-arm64.tar.gz

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
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-debian/grapa_0.1.51_arm64.deb

# Install the package
sudo dpkg -i grapa_0.1.51_arm64.deb

# Verify installation
grapa --version
```

#### AMD64 (Intel/AMD processors)
```bash
# Download the AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-debian/grapa_0.1.51_amd64.deb

# Install the package
sudo dpkg -i grapa_0.1.51_amd64.deb

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
sudo dpkg -r grapa
```

### Amazon Linux (Manual Installation)

**Download and install the appropriate package for your architecture:**

#### ARM64 (Graviton processors)
```bash
# Download the ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-rpm/grapa_0.1.51_arm64.rpm

# Install the package
sudo rpm -i grapa_0.1.51_arm64.rpm

# Verify installation
grapa --version
```

#### AMD64 (Intel/AMD processors)
```bash
# Download the AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-rpm/grapa_0.1.51_amd64.rpm

# Install the package
sudo rpm -i grapa_0.1.51_amd64.rpm

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
sudo rpm -e grapa
```

### Windows (Manual Installation)

**Download and install the Windows executable:**

```powershell
# Download the Windows binary
Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa-win-amd64.exe" -OutFile "grapa.exe"

# Move to a directory in your PATH (run PowerShell as Administrator)
Move-Item grapa.exe "C:\Windows\System32\grapa.exe"

# Verify installation
grapa --version
```

**Alternative installation to user directory:**
```powershell
# Create a directory for Grapa
New-Item -ItemType Directory -Path "$env:USERPROFILE\grapa" -Force

# Download the Windows binary
Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa-win-amd64.exe" -OutFile "$env:USERPROFILE\grapa\grapa.exe"

# Add to PATH for current session
$env:PATH += ";$env:USERPROFILE\grapa"

# Add to PATH permanently (recommended)
[Environment]::SetEnvironmentVariable("PATH", $env:PATH + ";$env:USERPROFILE\grapa", "User")

# Verify installation
grapa --version
```

**Alternative installation to Program Files:**
```powershell
# Create a directory for Grapa (run PowerShell as Administrator)
New-Item -ItemType Directory -Path "C:\Program Files\grapa" -Force

# Download the Windows binary
Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.51/grapa-win-amd64.exe" -OutFile "C:\Program Files\grapa\grapa.exe"

# Add to system PATH (run PowerShell as Administrator)
[Environment]::SetEnvironmentVariable("PATH", $env:PATH + ";C:\Program Files\grapa", "Machine")

# Verify installation
grapa --version
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

**Uninstall:**
```powershell
# Remove from System32 (if installed there)
Remove-Item "C:\Windows\System32\grapa.exe" -Force

# Or remove from user directory
Remove-Item "$env:USERPROFILE\grapa\grapa.exe" -Force
Remove-Item "$env:USERPROFILE\grapa" -Force
```

### Other Platforms

For other platforms, download the appropriate binary from the [GitHub releases page](https://github.com/grapa-dev/grapa/releases):

1. **Download** the binary for your platform and architecture
2. **Make it executable**: `chmod +x grapa`
3. **Move to PATH**: `sudo mv grapa /usr/local/bin/`
4. **Verify**: `grapa --version`

**Available binaries:**
- `grapa-linux-amd64` - Linux AMD64
- `grapa-linux-arm64` - Linux ARM64  
- `grapa-mac-arm64` - macOS ARM64 (Apple Silicon)
- `grapa.exe` - Windows AMD64

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
   choco list grapa
   ```

2. **Check if PATH is set correctly:**
   ```bash
   # macOS
   echo $PATH | grep -E "(homebrew|opt/homebrew)"
   
   # Linux
   echo $PATH | grep "/usr/bin"
   
   # Windows
   echo $env:PATH | Select-String "chocolatey"
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
   
   # Windows
   choco upgrade all
   ```

2. **Clear package manager cache:**
   ```bash
   # macOS
   brew cleanup
   
   # Ubuntu/Debian
   sudo apt clean
   
   # Amazon Linux/Red Hat
   sudo yum clean all
   
   # Windows
   choco cache remove
   ```

3. **Check network connectivity** and try again.

### Windows-Specific Issues

#### Chocolatey Not Found
If `choco` command is not recognized:

1. **Install Chocolatey** (run PowerShell as Administrator):
   ```powershell
   Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
   ```

2. **Refresh environment variables:**
   ```powershell
   refreshenv
   ```

3. **Verify Chocolatey installation:**
   ```powershell
   choco --version
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
