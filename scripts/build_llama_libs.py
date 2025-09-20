#!/usr/bin/env python3
"""
Script to build LLAMA.cpp static libraries for all Linux platforms.
This script detects the platform and builds the appropriate static libraries.
"""

import os
import sys
import platform
import subprocess
import shutil
from pathlib import Path

def detect_platform():
    """Detect the current platform and return the appropriate platform identifier."""
    system = platform.system()
    machine = platform.machine().lower()
    
    if system == 'Linux':
        # Check for AWS/Amazon Linux
        is_aws = False
        try:
            # Check for AWS-specific environment variables
            if 'AWS' in os.environ.get('AWS_EXECUTION_ENV', ''):
                is_aws = True
            # Check for Amazon Linux in os-release
            elif os.path.exists('/etc/os-release'):
                with open('/etc/os-release', 'r') as f:
                    content = f.read().lower()
                    if any(identifier in content for identifier in ['amazon linux', 'amazon-linux', 'aws', 'amazon']):
                        is_aws = True
            # Check for Amazon Linux in system-release
            elif os.path.exists('/etc/system-release'):
                with open('/etc/system-release', 'r') as f:
                    content = f.read().lower()
                    if any(identifier in content for identifier in ['amazon linux', 'amazon-linux', 'aws', 'amazon']):
                        is_aws = True
        except:
            pass
        
        if is_aws:
            if machine in ['aarch64', 'arm64']:
                return 'aws-arm64'
            else:
                return 'aws-amd64'
        else:
            if machine in ['aarch64', 'arm64']:
                return 'linux-arm64'
            else:
                return 'linux-amd64'
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
        ]
        
        print(f"📋 Configuring CMake with args: {' '.join(cmake_args[2:])}")
        result = subprocess.run(cmake_args, check=True, capture_output=True, text=True)
        print("✅ CMake configuration successful")
        
        # Build the libraries
        print("🔨 Building LLAMA.cpp libraries...")
        build_args = ["cmake", "--build", ".", "--config", "Release", "-j", str(os.cpu_count())]
        result = subprocess.run(build_args, check=True, capture_output=True, text=True)
        print("✅ LLAMA.cpp build successful")
        
        # Verify the built libraries
        expected_libs = [
            "libllama.a",
            "libggml.a",
            "libggml-base.a",
            "libggml-cpu.a",
            "libmtmd.a"
        ]
        
        missing_libs = []
        for lib in expected_libs:
            lib_path = Path(lib)
            if not lib_path.exists():
                missing_libs.append(lib)
        
        if missing_libs:
            print(f"⚠️  Warning: Some expected libraries are missing: {missing_libs}")
            print("Available files in build directory:")
            for file in Path(".").glob("*.a"):
                print(f"  - {file.name}")
        else:
            print("✅ All expected static libraries found")
        
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Build failed with return code {e.returncode}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False
    finally:
        # Return to original directory
        os.chdir("../..")

def copy_libraries(platform_name):
    """Copy the built libraries to the appropriate platform directory."""
    print(f"📦 Copying libraries to source/llama-lib/{platform_name}/...")
    
    # Create target directory
    target_dir = Path(f"source/llama-lib/{platform_name}")
    target_dir.mkdir(parents=True, exist_ok=True)
    
    # Source directory
    source_dir = Path("dep/llama.cpp-master/build")
    
    # Libraries to copy
    libraries = [
        "libllama.a",
        "libggml.a", 
        "libggml-base.a",
        "libggml-cpu.a",
        "libmtmd.a"
    ]
    
    copied_count = 0
    for lib in libraries:
        source_path = source_dir / lib
        target_path = target_dir / lib
        
        if source_path.exists():
            shutil.copy2(source_path, target_path)
            print(f"  ✅ Copied {lib}")
            copied_count += 1
        else:
            print(f"  ⚠️  Library not found: {lib}")
    
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
    if not build_llama_libs(platform_name):
        print("❌ Failed to build LLAMA.cpp libraries")
        return 1
    
    # Copy libraries
    if not copy_libraries(platform_name):
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
