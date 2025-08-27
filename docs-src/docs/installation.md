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
| **Windows AMD64** | Chocolatey | 🔄 **In Progress** | ✅ Available |

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

**Note**: Package manager installation is in progress. For immediate installation, see [Manual Installation](#manual-installation) below.

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
   ls -la $(which grapa)
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
   ```

3. **Check network connectivity** and try again.

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
