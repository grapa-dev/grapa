#!/usr/bin/env python3
"""
Script to build LLAMA.cpp static libraries for all Linux platforms.
This script detects the platform and builds the appropriate static libraries.

Key improvements for successful builds:
- Uses 'make all' to ensure all components are built, including common library
- Includes libcommon.a which contains critical GGML functions
- Builds with proper static library configuration
- Handles circular dependencies in library linking order
"""

import os
import sys
import platform
import subprocess
import shutil
from pathlib import Path

def check_dependencies():
    """Check if all required dependencies are installed for the build."""
    print("🔍 Checking build dependencies...")
    
    missing_deps = []
    
    # Check CMake
    try:
        result = subprocess.run(['cmake', '--version'], capture_output=True, text=True)
        if result.returncode != 0:
            missing_deps.append('cmake')
        else:
            version = result.stdout.split('\n')[0]
            print(f"  ✅ CMake: {version}")
    except FileNotFoundError:
        missing_deps.append('cmake')
    
    # Check build tools (gcc/g++)
    try:
        result = subprocess.run(['gcc', '--version'], capture_output=True, text=True)
        if result.returncode != 0:
            missing_deps.append('build-essential')
        else:
            version = result.stdout.split('\n')[0]
            print(f"  ✅ GCC: {version}")
    except FileNotFoundError:
        missing_deps.append('build-essential')
    
    try:
        result = subprocess.run(['g++', '--version'], capture_output=True, text=True)
        if result.returncode != 0:
            missing_deps.append('build-essential')
        else:
            version = result.stdout.split('\n')[0]
            print(f"  ✅ G++: {version}")
    except FileNotFoundError:
        missing_deps.append('build-essential')
    
    # Check Git
    try:
        result = subprocess.run(['git', '--version'], capture_output=True, text=True)
        if result.returncode != 0:
            missing_deps.append('git')
        else:
            version = result.stdout.strip()
            print(f"  ✅ Git: {version}")
    except FileNotFoundError:
        missing_deps.append('git')
    
    # Check CURL development headers
    curl_headers_found = False
    curl_paths = [
        '/usr/include/curl/curl.h',
        '/usr/local/include/curl/curl.h',
        f'/usr/include/{platform.machine()}-linux-gnu/curl/curl.h',
        '/usr/include/x86_64-linux-gnu/curl/curl.h',
        '/usr/include/aarch64-linux-gnu/curl/curl.h'
    ]
    for path in curl_paths:
        if os.path.exists(path):
            curl_headers_found = True
            break
    
    if not curl_headers_found:
        missing_deps.append('libcurl4-openssl-dev')
    else:
        print("  ✅ CURL headers: Found")
    
    # Check OpenMP support
    omp_available = False
    try:
        result = subprocess.run(['gcc', '-fopenmp', '--version'], capture_output=True, text=True)
        if result.returncode == 0:
            omp_available = True
            print("  ✅ OpenMP: Available")
    except FileNotFoundError:
        pass
    
    # Check for OpenMP library
    omp_lib_found = False
    omp_paths = [
        f'/usr/lib/{platform.machine()}-linux-gnu/libgomp.so',
        f'/usr/lib/{platform.machine()}-linux-gnu/libgomp.so.1',
        '/usr/lib/x86_64-linux-gnu/libgomp.so',
        '/usr/lib/aarch64-linux-gnu/libgomp.so',
        '/usr/lib/libgomp.so',
        '/usr/lib64/libgomp.so',
        # Amazon Linux specific paths
        '/usr/lib64/libgomp.so.1',
        '/usr/lib/libgomp.so.1',
        # Check if libgomp is available via pkg-config or ldconfig
    ]
    for path in omp_paths:
        if os.path.exists(path):
            omp_lib_found = True
            break
    
    # If not found in standard paths, try ldconfig
    if not omp_lib_found:
        try:
            result = subprocess.run(['ldconfig', '-p'], capture_output=True, text=True)
            if result.returncode == 0 and 'libgomp' in result.stdout:
                omp_lib_found = True
                print("  ✅ OpenMP library: Found via ldconfig")
        except FileNotFoundError:
            pass
    
    if omp_lib_found and omp_available:
        print("  ✅ OpenMP: Fully available (compiler + library)")
    
    # Only add OpenMP dependency if both compiler support and library are missing
    if not omp_available and not omp_lib_found:
        missing_deps.append('libomp-dev')
    
    if missing_deps:
        print("\n❌ Missing dependencies:")
        for dep in missing_deps:
            print(f"  - {dep}")
        
        print("\n📦 Install missing dependencies:")
        print("Ubuntu/Debian:")
        print(f"  sudo apt-get update")
        print(f"  sudo apt-get install -y {' '.join(missing_deps)}")
        print("\nCentOS/RHEL/Amazon Linux:")
        centos_deps = []
        for dep in missing_deps:
            if dep == 'build-essential':
                centos_deps.append('gcc gcc-c++ make')
            elif dep == 'libcurl4-openssl-dev':
                centos_deps.append('libcurl-devel')
            elif dep == 'libomp-dev':
                centos_deps.append('libgomp')
            else:
                centos_deps.append(dep)
        print(f"  sudo yum install -y {' '.join(centos_deps)}")
        
        return False
    
    print("✅ All dependencies satisfied!")
    return True

def detect_platform():
    """Detect the current platform and return the appropriate platform identifier.
    Uses the same logic as build.py for consistency."""
    system = platform.system().lower()
    machine = platform.machine().lower()
    
    if system == 'linux':
        # Check if this is AWS Linux by looking for Amazon Linux specific files
        # Use exact same logic as build.py
        if (os.path.exists('/etc/system-release') and 
            ('Amazon Linux' in open('/etc/system-release').read())):
            return 'aws-arm64' if machine == 'aarch64' else 'aws-amd64'
        else:
            return 'linux-arm64' if machine == 'aarch64' else 'linux-amd64'
    elif system == 'Darwin':
        if machine in ['aarch64', 'arm64']:
            return 'mac-arm64'
        else:
            return 'mac-amd64'
    elif system == 'Windows':
        return 'win-amd64'
    else:
        raise RuntimeError(f"Unsupported platform: {system}")

def clean_llama_build():
    """Clean the LLAMA.cpp build directory."""
    llama_build_dir = Path("dep/llama.cpp-master/build")
    if llama_build_dir.exists():
        print("🧹 Cleaning previous LLAMA.cpp build...")
        shutil.rmtree(llama_build_dir)
        print("✅ Cleaned LLAMA.cpp build directory")

def build_llama_libs(platform_name):
    """Build LLAMA.cpp static libraries for the specified platform."""
    print(f"🔨 Building LLAMA.cpp static libraries for {platform_name}...")
    
    # Store original directory
    original_dir = os.getcwd()
    
    # Change to LLAMA.cpp directory
    llama_dir = Path("dep/llama.cpp-master")
    if not llama_dir.exists():
        raise RuntimeError("LLAMA.cpp directory not found at dep/llama.cpp-master")
    
    os.chdir(llama_dir)
    
    try:
        # Create build directory
        build_dir = Path("build")
        build_dir.mkdir(exist_ok=True)
        os.chdir(build_dir)
        
        # Configure CMake
        cmake_args = [
            "cmake",
            "..",
            "-DCMAKE_BUILD_TYPE=Release",
            "-DLLAMA_STATIC=ON",
            "-DBUILD_SHARED_LIBS=OFF",  # Force static libraries
            "-DLLAMA_NATIVE=OFF",  # Disable native optimizations for portability
            "-DLLAMA_ACCELERATE=OFF",  # Disable Accelerate framework
            "-DLLAMA_METAL=OFF",  # Disable Metal
            "-DLLAMA_CUBLAS=OFF",  # Disable CUDA
            "-DLLAMA_CLBLAST=OFF",  # Disable OpenCL
            "-DLLAMA_HIPBLAS=OFF",  # Disable HIP
            "-DLLAMA_KOMPUTE=OFF",  # Disable Kompute
            "-DLLAMA_SYCL=OFF",  # Disable SYCL
            "-DLLAMA_GGML_BACKEND=CPU",  # Use CPU backend only
            "-DLLAMA_BUILD_TESTS=OFF",
            "-DLLAMA_BUILD_EXAMPLES=OFF",
            "-DLLAMA_BUILD_SERVER=OFF",
            "-DLLAMA_BUILD_TOOLS=ON",  # Keep tools for debugging
            # Add -fPIC flag for Python extension compatibility
            "-DCMAKE_CXX_FLAGS=-fPIC",
            "-DCMAKE_C_FLAGS=-fPIC",
        ]
        
        print(f"📋 Configuring CMake with args: {' '.join(cmake_args[2:])}")
        result = subprocess.run(cmake_args, check=True, capture_output=True, text=True)
        print("✅ CMake configuration successful")
        
        # Build the libraries
        print("🔨 Building LLAMA.cpp libraries...")
        # Use 'make all' to ensure all components are built, including common library
        # This is critical for getting all required GGML functions and dependencies
        build_args = ["make", "all", "-j", str(os.cpu_count())]
        result = subprocess.run(build_args, check=True, capture_output=True, text=True)
        print("✅ LLAMA.cpp build successful")
        
        # Verify the built libraries
        expected_libs = [
            ("src/libllama.a", "libllama.a"),
            ("ggml/src/libggml.a", "libggml.a"),
            ("ggml/src/libggml-base.a", "libggml-base.a"),
            ("ggml/src/libggml-cpu.a", "libggml-cpu.a"),
            ("tools/mtmd/libmtmd.a", "libmtmd.a"),
            ("common/libcommon.a", "libcommon.a")  # Add common library
        ]
        
        missing_libs = []
        found_libs = []
        for source_path, target_name in expected_libs:
            lib_path = Path(source_path)
            if lib_path.exists():
                found_libs.append((source_path, target_name))
            else:
                missing_libs.append(target_name)
        
        if missing_libs:
            print(f"⚠️  Warning: Some expected libraries are missing: {missing_libs}")
            print("Available files in build directory:")
            for file in Path(".").rglob("*.a"):
                print(f"  - {file}")
        else:
            print("✅ All expected static libraries found")
        
        return found_libs
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Build failed with return code {e.returncode}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return []
    finally:
        # Return to original directory
        os.chdir(original_dir)

def copy_libraries(platform_name, found_libraries):
    """Copy the built libraries to the appropriate platform directory."""
    print(f"📦 Copying libraries to source/llama-lib/{platform_name}/...")
    
    # Create target directory
    target_dir = Path(f"source/llama-lib/{platform_name}")
    target_dir.mkdir(parents=True, exist_ok=True)
    
    # Source directory - libraries are in the build directory
    source_dir = Path("dep/llama.cpp-master/build")
    
    copied_count = 0
    for source_path, target_name in found_libraries:
        # source_path is relative to the build directory, so we need to construct the full path
        full_source_path = source_dir / source_path
        target_path = target_dir / target_name
        
        if full_source_path.exists():
            shutil.copy2(full_source_path, target_path)
            print(f"  ✅ Copied {target_name}")
            copied_count += 1
        else:
            print(f"  ⚠️  Library not found: {full_source_path}")
    
    print(f"📦 Copied {copied_count} libraries to {target_dir}")
    return copied_count > 0

def main():
    """Main function to build LLAMA.cpp libraries for the current platform."""
    print("🚀 LLAMA.cpp Static Library Builder")
    print("=" * 50)
    
    # Check dependencies first
    if not check_dependencies():
        print("\n❌ Build aborted due to missing dependencies.")
        print("Please install the missing dependencies and try again.")
        return 1
    
    print()  # Add spacing
    
    # Detect platform
    platform_name = detect_platform()
    print(f"🖥️  Detected platform: {platform_name}")
    
    # Check if this is a supported platform
    if not platform_name.startswith(('linux-', 'aws-')):
        print(f"❌ This script is designed for Linux platforms only.")
        print(f"   Current platform: {platform_name}")
        print(f"   For Mac, use the existing build system.")
        print(f"   For Windows, use the existing build system.")
        return 1
    
    # Clean previous build
    clean_llama_build()
    
    # Build libraries
    found_libraries = build_llama_libs(platform_name)
    if not found_libraries:
        print("❌ Failed to build LLAMA.cpp libraries")
        return 1
    
    # Copy libraries
    if not copy_libraries(platform_name, found_libraries):
        print("❌ Failed to copy libraries")
        return 1
    
    print("=" * 50)
    print("🎉 LLAMA.cpp static library build completed successfully!")
    print(f"📁 Libraries are now available in: source/llama-lib/{platform_name}/")
    print()
    print("Next steps:")
    print("1. Run this script on each target platform")
    print("2. Update build configurations to include LLAMA.cpp support")
    print("3. Test the build on each platform")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
