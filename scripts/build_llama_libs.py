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
