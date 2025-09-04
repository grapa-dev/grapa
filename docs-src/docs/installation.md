# Installing Grapa

Grapa is available as a universal development kit that provides everything you need to get started with Grapa development across all platforms.

**Note**: Documentation and downloads are available on [GitHub](https://github.com/grapa-dev/grapa).

## 🚀 **Universal Development Kit Installation**

Grapa 0.1.53+ introduces a **universal development kit** that contains everything you need in a single package:

- **Universal installer** that works on all platforms
- **Sample C++ application** demonstrating Grapa embedding
- **CMake build system** with cross-platform support
- **Complete API headers** and libraries for all platforms
- **Self-contained** - no external dependencies required

### **Step 1: Download Universal Package**
1. Go to [GitHub Releases](https://github.com/grapa-dev/grapa/releases)
2. Download the single universal package: `grapa-0.1.53.zip`

### **Step 2: Extract and Install**
The universal package contains everything for all platforms in one convenient download.

## Platform-Specific Installation

### macOS (ARM64 - Apple Silicon)

**Download and install:**
```bash
# Download the universal package
curl -L -o grapa-0.1.53.zip https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53.zip

# Extract the package
unzip grapa-0.1.53.zip
cd grapa-0.1.53

# Run the universal installer
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

### Ubuntu/Debian

**Download and install (works for both ARM64 and AMD64):**
```bash
# Download the universal package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53.zip

# Extract and install
unzip grapa-0.1.53.zip
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

### Amazon Linux

**Download and install (works for both ARM64 and AMD64):**
```bash
# Download the universal package
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53.zip

# Extract and install
unzip grapa-0.1.53.zip
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

### Windows

**Download and install using the universal package:**

#### Automated Installation (Recommended)

1. **Download the universal package:**
   ```powershell
   # Download the latest universal release
   Invoke-WebRequest -Uri "https://github.com/grapa-dev/grapa/releases/download/v0.1.53/grapa-0.1.53.zip" -OutFile "grapa-0.1.53.zip"
   ```

2. **Extract the package:**
   ```powershell
   Expand-Archive -Path "grapa-0.1.53.zip" -DestinationPath "grapa-0.1.53" -Force
   ```

3. **Run the universal installer** (run PowerShell as Administrator):
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

## 🐍 **Python Extension Build**

Grapa 0.1.53 includes support for building Python extensions. If you need to build from source with Python support:

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

The universal development kit provides everything in a single package:

- `grapa-0.1.53.zip` - **Universal Development Kit** (107 MB)
  - Works on all supported platforms
  - Includes universal installer (`install-grapa.py`)
  - Contains sample C++ application (`main.cpp`)
  - Complete CMake build system
  - All platform libraries and headers
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
