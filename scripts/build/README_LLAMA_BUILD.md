# LLAMA.cpp Static Library Builder

This document explains how to build LLAMA.cpp static libraries for all Linux platforms to support the `$MODEL` functionality in Grapa.

## Overview

The `scripts/build_llama_libs.py` script builds LLAMA.cpp static libraries for the current platform and copies them to the appropriate location in `source/llama-lib/`. This enables Grapa to use AI/ML models through the `$MODEL` functionality.

## Supported Platforms

- **linux-amd64**: Standard Linux x86_64
- **linux-arm64**: Standard Linux ARM64 (aarch64)
- **aws-amd64**: Amazon Linux x86_64
- **aws-arm64**: Amazon Linux ARM64 (aarch64)

## Prerequisites

### System Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
```

**CentOS/RHEL/Amazon Linux:**
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake git
```

**Amazon Linux 2/3:**
```bash
sudo yum update
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 git
# Create symlink if needed
sudo ln -sf /usr/bin/cmake3 /usr/bin/cmake
```

### LLAMA.cpp Source

The script expects the LLAMA.cpp source to be located at:
```
dep/llama.cpp-master/
```

If this directory doesn't exist, you need to clone it:
```bash
cd dep
git clone https://github.com/ggerganov/llama.cpp.git llama.cpp-master
cd llama.cpp-master
git checkout master  # or specific version tag
```

## Usage

### Basic Usage

Run the script from the Grapa project root:

```bash
python3 scripts/build_llama_libs.py
```

The script will:
1. Detect the current platform
2. Clean any previous LLAMA.cpp build
3. Configure and build LLAMA.cpp static libraries
4. Copy the libraries to `source/llama-lib/{platform}/`

### Platform Detection

The script automatically detects the platform using:
- System information (`platform.system()`, `platform.machine()`)
- AWS environment variables (`AWS_EXECUTION_ENV`)
- OS release files (`/etc/os-release`, `/etc/system-release`)

### Expected Output

```
🚀 LLAMA.cpp Static Library Builder
==================================================
🖥️  Detected platform: linux-amd64
🧹 Cleaning previous LLAMA.cpp build...
✅ Cleaned LLAMA.cpp build directory
🔨 Building LLAMA.cpp static libraries for linux-amd64...
📋 Configuring CMake with args: -DCMAKE_BUILD_TYPE=Release -DLLAMA_STATIC=ON ...
✅ CMake configuration successful
🔨 Building LLAMA.cpp libraries...
✅ LLAMA.cpp build successful
✅ All expected static libraries found
📦 Copying libraries to source/llama-lib/linux-amd64/...
  ✅ Copied libllama.a
  ✅ Copied libggml.a
  ✅ Copied libggml-base.a
  ✅ Copied libggml-cpu.a
  ✅ Copied libmtmd.a
📦 Copied 5 libraries to source/llama-lib/linux-amd64
==================================================
🎉 LLAMA.cpp static library build completed successfully!
📁 Libraries are now available in: source/llama-lib/linux-amd64/
```

## Built Libraries

The script builds and copies the following static libraries:

- **libllama.a**: Main LLAMA.cpp library
- **libggml.a**: Core GGML tensor library
- **libggml-base.a**: Base GGML operations
- **libggml-cpu.a**: CPU-specific GGML operations
- **libmtmd.a**: Multi-threaded matrix operations

## Build Configuration

The script uses the following CMake configuration:

```cmake
-DCMAKE_BUILD_TYPE=Release
-DLLAMA_STATIC=ON
-DLLAMA_NATIVE=OFF          # Disable native optimizations for portability
-DLLAMA_ACCELERATE=OFF      # Disable Accelerate framework
-DLLAMA_METAL=OFF           # Disable Metal
-DLLAMA_CUBLAS=OFF          # Disable CUDA
-DLLAMA_CLBLAST=OFF         # Disable OpenCL
-DLLAMA_HIPBLAS=OFF         # Disable HIP
-DLLAMA_KOMPUTE=OFF         # Disable Kompute
-DLLAMA_SYCL=OFF            # Disable SYCL
-DLLAMA_GGML_BACKEND=CPU    # Use CPU backend only
-DLLAMA_BUILD_TESTS=OFF
-DLLAMA_BUILD_EXAMPLES=OFF
-DLLAMA_BUILD_SERVER=OFF
-DLLAMA_BUILD_TOOLS=ON      # Keep tools for debugging
```

## Multi-Platform Build Process

To build for all platforms, run the script on each target platform:

### 1. Linux AMD64
```bash
# On a Linux x86_64 system
python3 scripts/build_llama_libs.py
```

### 2. Linux ARM64
```bash
# On a Linux ARM64 system
python3 scripts/build_llama_libs.py
```

### 3. AWS AMD64
```bash
# On an Amazon Linux x86_64 EC2 instance
python3 scripts/build_llama_libs.py
```

### 4. AWS ARM64
```bash
# On an Amazon Linux ARM64 EC2 instance
python3 scripts/build_llama_libs.py
```

## Verification

After building, verify the libraries exist:

```bash
ls -la source/llama-lib/*/
```

You should see directories for each platform with the static libraries:
```
source/llama-lib/
├── linux-amd64/
│   ├── libggml.a
│   ├── libggml-base.a
│   ├── libggml-cpu.a
│   ├── libllama.a
│   └── libmtmd.a
├── linux-arm64/
│   ├── libggml.a
│   ├── libggml-base.a
│   ├── libggml-cpu.a
│   ├── libllama.a
│   └── libmtmd.a
├── aws-amd64/
│   └── ...
└── aws-arm64/
    └── ...
```

## Troubleshooting

### Common Issues

1. **CMake not found:**
   ```bash
   sudo apt-get install cmake  # Ubuntu/Debian
   sudo yum install cmake3     # CentOS/RHEL
   ```

2. **Build tools not found:**
   ```bash
   sudo apt-get install build-essential  # Ubuntu/Debian
   sudo yum groupinstall "Development Tools"  # CentOS/RHEL
   ```

3. **Git not found:**
   ```bash
   sudo apt-get install git  # Ubuntu/Debian
   sudo yum install git      # CentOS/RHEL
   ```

4. **LLAMA.cpp source not found:**
   ```bash
   cd dep
   git clone https://github.com/ggerganov/llama.cpp.git llama.cpp-master
   ```

5. **Build fails with memory errors:**
   - Reduce parallel jobs: Edit the script and change `-j {os.cpu_count()}` to `-j 2`
   - Increase swap space on the system

6. **Platform detection fails:**
   - Check `/etc/os-release` or `/etc/system-release` files
   - Verify AWS environment variables if on AWS

### Debug Mode

To see more detailed output, you can modify the script to add verbose flags:

```python
# In the build_llama_libs.py script, add:
cmake_args += ["-DCMAKE_VERBOSE_MAKEFILE=ON"]
```

## Integration with Grapa Build

After building the LLAMA.cpp libraries, the Grapa build system will automatically:

1. **Include LLAMA.cpp headers** in the build process
2. **Link LLAMA.cpp static libraries** with the Grapa executable
3. **Enable $MODEL functionality** in Grapa scripts
4. **Support AI/ML model operations** through the Grapa language

## Next Steps

After building LLAMA.cpp libraries on all platforms:

1. **Test the build** on each platform:
   ```bash
   python3 build.py --exe-only
   ```

2. **Test $MODEL functionality**:
   ```bash
   ./grapa -c "m = \$MODEL(); m.info();"
   ```

3. **Test Python extension** (if building with Python support):
   ```bash
   python3 setup.py build_ext --inplace
   python3 -c "import grapapy; g = grapapy.grapa(); print(g.eval('m = \$MODEL(); m.info();'))"
   ```

## Notes

- **No Hardware Acceleration**: The current build configuration disables all hardware acceleration (CUDA, Metal, etc.) for maximum compatibility
- **CPU Only**: All operations run on CPU, which is sufficient for basic AI/ML model operations
- **Portable**: The static libraries are built for maximum portability across different Linux distributions
- **Future Enhancement**: Hardware acceleration can be added later by modifying the CMake configuration
