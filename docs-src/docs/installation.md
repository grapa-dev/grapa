# Installing Grapa

Grapa is available as platform-specific packages with a universal installer that automatically detects your platform and downloads the appropriate package.

**Note**: Documentation and downloads are available on [GitHub](https://github.com/grapa-dev/grapa).

## 🚀 **Universal Installer (Recommended)**

Grapa 0.1.54+ provides a **universal installer** that automatically detects your platform and downloads the correct package:

- **Smart platform detection** - automatically identifies your system
- **Automatic dependency installation** - installs system dependencies on Linux/macOS
- **Ubuntu version detection** - provides version-specific installation instructions
- **Optimized packages** - each platform gets only what it needs
- **Under 100MB** - all packages stay well under GitHub's file size limit
- **Complete development kit** - includes sample code, CMake build system, and all libraries
- **Self-contained** - no external dependencies required

### **Quick Installation**
```bash
# Download and run the universal installer
curl -O https://github.com/grapa-dev/grapa/releases/download/v0.1.54/install-grapa-0.1.54.py
python3 install-grapa-0.1.54.py
```

### **Installation with Automatic Dependencies**
```bash
# Install Grapa and automatically install system dependencies (Linux/macOS)
python3 install-grapa-0.1.54.py --install-dependencies
```

The installer will:
1. **Detect your platform** (macOS, Linux, Windows, AWS)
2. **Install system dependencies** (Linux/macOS only, with `--install-dependencies`)
3. **Download the appropriate package** for your system
4. **Install Grapa** to `/usr/local/` (or `~/.local/grapa/` on some systems)
5. **Set up the environment** for immediate use
6. **Provide platform-specific instructions** for Python extension installation

## Manual Installation (Alternative)

If you prefer to download and install manually, you can download the specific package for your platform:

### macOS (ARM64 - Apple Silicon)

**Download and install:**
```bash
# Download the macOS ARM64 package
curl -L -o grapa-0.1.53-mac-arm64.zip https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53-mac-arm64.zip

# Extract the package
unzip grapa-0.1.53-mac-arm64.zip
cd grapa-0.1.53

# Run the installer
python3 install-grapa.py

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

### Linux (AMD64)

**Download and install:**
```bash
# Download the Linux AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53-linux-amd64.zip

# Extract and install
unzip grapa-0.1.53-linux-amd64.zip
cd grapa-0.1.53
sudo python3 install-grapa.py

# Verify installation
grapa --version
```

### Linux (ARM64)

**Download and install:**
```bash
# Download the Linux ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53-linux-arm64.zip

# Extract and install
unzip grapa-0.1.53-linux-arm64.zip
cd grapa-0.1.53
sudo python3 install-grapa.py

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

### AWS (AMD64)

**Download and install:**
```bash
# Download the AWS AMD64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53-aws-amd64.zip

# Extract and install
unzip grapa-0.1.53-aws-amd64.zip
cd grapa-0.1.53
sudo python3 install-grapa.py

# Verify installation
grapa --version
```

### AWS (ARM64)

**Download and install:**
```bash
# Download the AWS ARM64 package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53-aws-arm64.zip

# Extract and install
unzip grapa-0.1.53-aws-arm64.zip
cd grapa-0.1.53
sudo python3 install-grapa.py

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

### Windows (AMD64)

**Download and install:**

#### Automated Installation (Recommended)

1. **Download the Windows package:**
   ```powershell
   # Download the Windows AMD64 release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53-win-amd64.zip" -OutFile "grapa-0.1.53-win-amd64.zip"
   ```

2. **Extract the package:**
   ```powershell
   Expand-Archive -Path "grapa-0.1.53-win-amd64.zip" -DestinationPath "grapa-0.1.53" -Force
   ```

3. **Run the installer** (run PowerShell as Administrator):
   ```powershell
   cd grapa-0.1.53
   python install-grapa.py
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
python install-grapa.py --uninstall
```

## 🛠️ **Building and Running the Example**

The universal development kit includes a complete C++ example demonstrating how to embed Grapa in your applications.

### **Build the Example Application**

After extracting the universal package, you can build and run the included example:

```bash
# Navigate to the extracted directory
cd grapa-0.1.53

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the example
./grapa_example  # On Unix/macOS
grapa_example.exe  # On Windows
```

### **What the Example Demonstrates**

The included `main.cpp` shows how to:
- Initialize the Grapa runtime
- Execute Grapa code from C++
- Handle errors and return values
- Clean up resources properly

### **Development Workflow**

The universal development kit supports the standard CMake workflow:

```bash
# Standard CMake build process
mkdir build && cd build
cmake ..
cmake --build .
```

This works consistently across all platforms (Windows, macOS, Linux) with the same commands.

## Verify Installation

After installation, verify that Grapa is working correctly:

```bash
grapa --version
```

You should see output similar to:
```
Grapa version 0.1.53
```

## Test Basic Functionality

Try running a simple Grapa command:

```bash
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

You should see:
```
4
Hello World
```

## 🐍 **Python Extension Installation**

GrapaPy provides Python integration with Grapa's powerful data processing capabilities. Installation requirements vary by platform:

### **Platform Requirements**

#### **Linux/macOS**
- **System dependencies**: Automatically installed with `--install-dependencies` flag
- **Python packages**: Standard `pip` installation

#### **Windows**
- **Visual Studio Build Tools**: Required for compilation during installation
- **Python packages**: Standard `pip` installation

### **Installation Methods**

#### **Method 1: From PyPI (Recommended)**
```bash
# Linux/macOS
pip3 install grapapy

# Windows
pip install grapapy
```

#### **Method 2: From Distribution Package**
```bash
# After installing Grapa CLI
pip3 install dist/grapapy-0.1.54.tar.gz

# Ubuntu 24.04+ may require:
pip3 install dist/grapapy-0.1.54.tar.gz --break-system-packages
```

### **Dependency Installation**

#### **Automatic (Linux/macOS)**
```bash
# Install Grapa with all dependencies
python3 install-grapa-0.1.54.py --install-dependencies
```

#### **Manual (All Platforms)**
**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install build-essential python3-dev libssl-dev libx11-dev libxext-dev cmake
```

**Linux (CentOS/RHEL/Amazon Linux):**
```bash
sudo yum groupinstall 'Development Tools'
sudo yum install gcc-c++ python3-devel openssl-devel libX11-devel libXext-devel cmake
```

**macOS:**
```bash
xcode-select --install
```

**Windows:**
- Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) or Build Tools for Visual Studio 2022
- Ensure "Desktop development with C++" workload is selected

## 🛠️ **Building from Source**

Grapa 0.1.54 includes support for building Python extensions from source:

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
pip install grapapy==0.1.53
```

**Non-Windows:**
```bash
# Upgrade pip first
pip3 install --upgrade pip

# Install GrapaPy
pip3 install grapapy==0.1.53
```

### **Verify Python Integration**
```python
import grapapy
g = grapapy.grapa()
result = g.eval('2 + 2')
print(f'Grapa result: {result}')
```

## Available Packages

The platform-specific release provides optimized packages for each platform:

- `install-grapa-0.1.53.py` - **Universal Installer** (4 KB)
  - Automatically detects your platform
  - Downloads the appropriate package
  - Installs Grapa with proper setup

**Platform-Specific Packages:**
- `grapa-0.1.53-mac-arm64.zip` - **macOS ARM64** (10.1 MB)
- `grapa-0.1.53-linux-amd64.zip` - **Linux AMD64** (13.0 MB)
- `grapa-0.1.53-linux-arm64.zip` - **Linux ARM64** (13.8 MB)
- `grapa-0.1.53-win-amd64.zip` - **Windows AMD64** (41.3 MB)
- `grapa-0.1.53-aws-amd64.zip` - **AWS AMD64** (12.4 MB)
- `grapa-0.1.53-aws-arm64.zip` - **AWS ARM64** (12.8 MB)

**Each package includes:**
- Platform-specific Grapa executable
- Universal installer (`install-grapa.py`)
- Sample C++ application (`main.cpp`)
- Complete CMake build system
- Platform libraries and headers
- Self-contained with no external dependencies

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
If the `install-grapa.py` script fails:

1. **Check Python installation:**
   ```powershell
   python --version
   # Should show Python 3.6 or higher
   ```

2. **Verify the script exists:**
   ```powershell
   Test-Path "install-grapa.py"
   ```

3. **Run the script with verbose output:**
   ```powershell
   python install-grapa.py --verbose
   ```

#### Python Installation Issues
If you encounter Python-related errors:

1. **Check Python installation:**
   ```powershell
   python --version
   ```

2. **Install Python if needed:**
   - Download from [python.org](https://www.python.org/downloads/)
   - Make sure to check "Add Python to PATH" during installation

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
