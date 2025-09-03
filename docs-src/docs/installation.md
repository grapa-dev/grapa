# Installing Grapa

Grapa is available for download from GitHub releases for all supported platforms, making installation quick and easy.

**Note**: Documentation and downloads are available on [GitHub](https://github.com/grapa-dev/grapa).

## 🚀 **Installation from GitHub Releases**

The easiest way to install Grapa is to download the latest release from GitHub and use the automated install scripts included in each package.

### **Step 1: Download Latest Release**
1. Go to [GitHub Releases](https://github.com/grapa-dev/grapa/releases)
2. Download the appropriate package for your platform:
   - **Windows**: `grapa-0.1.52-win-amd64.zip`
   - **macOS**: `grapa-0.1.52-mac-arm64.zip`
   - **Linux**: `grapa-0.1.52-linux-amd64.zip` or `grapa-0.1.52-linux-arm64.zip`
   - **AWS**: `grapa-0.1.52-aws-amd64.zip` or `grapa-0.1.52-aws-arm64.zip`

### **Step 2: Extract and Run Install Script**
Each package contains an automated install script that handles the entire installation process.

## Platform-Specific Installation

### macOS (ARM64 - Apple Silicon)

**Download and install:**
```bash
# Download the macOS ARM64 package
curl -L -o grapa-0.1.52-mac-arm64.zip https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-mac-arm64.zip

# Extract the package
unzip grapa-0.1.52-mac-arm64.zip
cd grapa-0.1.52-mac-arm64

# Run the automated installer
./install-grapa.sh

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
# The installer provides uninstall instructions
# Or manually remove:
sudo rm -f /usr/local/bin/grapa
sudo rm -f /usr/local/lib/libgrapa_static.a
sudo rm -f /usr/local/lib/libgrapa.so
```

### Ubuntu/Debian

**Download and install for your architecture:**

#### ARM64 (ARM servers)
```bash
# Download the ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-linux-arm64.zip

# Extract and install
unzip grapa-0.1.52-linux-arm64.zip
cd grapa-0.1.52-linux-arm64
sudo ./install-grapa.sh

# Verify installation
grapa --version
```

#### AMD64 (Intel/AMD processors)
```bash
# Download the AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-linux-amd64.zip

# Extract and install
unzip grapa-0.1.52-linux-amd64.zip
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

### Amazon Linux

**Download and install for your architecture:**

#### ARM64 (Graviton processors)
```bash
# Download the ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-aws-arm64.zip

# Extract and install
unzip grapa-0.1.52-aws-arm64.zip
cd grapa-0.1.52-aws-arm64
sudo ./install-grapa.sh

# Verify installation
grapa --version
```

#### AMD64 (Intel/AMD processors)
```bash
# Download the AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.52/grapa-0.1.52-aws-amd64.zip

# Extract and install
unzip grapa-0.1.52-aws-amd64.zip
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

### Windows

**Download and install the Windows package:**

#### Automated Installation (Recommended)

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

**Test basic functionality:**
```powershell
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
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
# Upgrade pip first
pip install --upgrade pip

# Install GrapaPy
pip install grapapy==0.1.52
```

**Non-Windows:**
```bash
# Upgrade pip first
pip3 install --upgrade pip

# Install GrapaPy
pip3 install grapapy==0.1.52
```

### **Verify Python Integration**
```python
import grapapy
g = grapapy.grapa()
result = g.eval('2 + 2')
print(f'Grapa result: {result}')
```

## Available Packages

All packages include automated install scripts and both static and shared libraries for maximum compatibility:

- `grapa-0.1.52-linux-amd64.zip` - Linux AMD64 (includes install script)
- `grapa-0.1.52-linux-arm64.zip` - Linux ARM64 (includes install script)
- `grapa-0.1.52-mac-arm64.zip` - macOS ARM64 (includes install script)
- `grapa-0.1.52-win-amd64.zip` - Windows AMD64 (includes PowerShell install script)
- `grapa-0.1.52-aws-amd64.zip` - AWS AMD64 (includes install script)
- `grapa-0.1.52-aws-arm64.zip` - AWS ARM64 (includes install script)

## Troubleshooting

### "grapa: command not found"

1. **Verify installation completed successfully:**
   ```bash
   # Check if grapa is installed
   which grapa
   
   # Check if it's executable
   ls -la $(which grapa)
   ```

2. **Check if PATH is set correctly:**
   ```bash
   echo $PATH | grep "/usr/local/bin"
   ```

3. **Try restarting your terminal** after installation.

### Permission Denied

1. **Ensure you have appropriate permissions:**
   ```bash
   # Linux/macOS
   sudo chmod +x /usr/local/bin/grapa
   ```

2. **Check file permissions:**
   ```bash
   # Linux/macOS
   ls -la $(which grapa)
   ```

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

## Getting Help

If you encounter issues not covered here:

1. **Check the [Troubleshooting Guide](troubleshooting.md)**
2. **Search existing issues** on [GitHub](https://github.com/grapa-dev/grapa/issues)
3. **Create a new issue** with details about your platform and error messages

## Next Steps

After successful installation:

1. **Read the [Getting Started Guide](getting_started.md)** for your first Grapa program
2. **Explore the [CLI Quick Start](cli_quickstart.md)** for command-line usage
3. **Check out [Examples](examples.md)** for sample code and use cases
