#!/usr/bin/env python3
"""
Script to rebuild LLAMA.cpp static libraries for Windows with OpenMP support.
This script rebuilds both Release and Debug LLAMA libraries with OpenMP and CPU optimizations.

Output directories:
- Release: source/llama-lib/win-amd64/
- Debug: source/llama-lib/win-amd64-debug/

The Debug libraries are required for Visual Studio debugging sessions.

Note: Windows OpenMP is built into the MSVC runtime, so no separate library linking
is required (unlike Linux which needs -lgomp). Only the /openmp compiler flag is needed.
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path

def rebuild_llama_windows():
    """Rebuild LLAMA.cpp libraries for Windows with OpenMP support (Release and Debug)"""
    print("🔧 Rebuilding LLAMA.cpp libraries for Windows with OpenMP support...")
    
    # Check if we're on Windows
    if os.name != 'nt':
        print("❌ This script is for Windows only")
        return False
    
    # Check if LLAMA source exists
    llama_source = Path("dep/llama.cpp-master")
    if not llama_source.exists():
        print(f"❌ LLAMA source not found at {llama_source}")
        return False
    
    # Build both Release and Debug versions
    success = True
    
    # Build Release version
    print("\n🚀 Building RELEASE version...")
    if not build_llama_configuration(llama_source, "Release", "source/llama-lib/win-amd64"):
        success = False
    
    # Build Debug version
    print("\n🐛 Building DEBUG version...")
    if not build_llama_configuration(llama_source, "Debug", "source/llama-lib/win-amd64-debug"):
        success = False
    
    if success:
        print("\n🎉 LLAMA.cpp libraries rebuilt successfully with OpenMP support!")
        print("📁 Release libraries: source/llama-lib/win-amd64/")
        print("📁 Debug libraries: source/llama-lib/win-amd64-debug/")
    
    return success

def build_llama_configuration(llama_source, config_name, target_dir):
    """Build LLAMA.cpp for a specific configuration (Release/Debug)"""
    print(f"🔨 Building {config_name} configuration...")
    
    # Create build directory for this configuration
    build_dir = llama_source / f"build-windows-openmp-{config_name.lower()}"
    if build_dir.exists():
        print(f"🧹 Cleaning existing build directory: {build_dir}")
        shutil.rmtree(build_dir)
    
    build_dir.mkdir(exist_ok=True)
    print(f"📁 Created build directory: {build_dir}")
    
    # Configure with CMake
    print(f"🔧 Configuring LLAMA.cpp with CMake ({config_name})...")
    cmake_cmd = [
        "cmake",
        "-G", "Visual Studio 17 2022",  # Use Visual Studio 2022
        "-A", "x64",  # 64-bit architecture
        "-D", f"CMAKE_BUILD_TYPE={config_name}",
        "-D", "BUILD_SHARED_LIBS=OFF",  # Build static libraries
        "-D", "GGML_OPENMP=ON",  # Enable OpenMP
        "-D", "GGML_NATIVE=ON",  # Enable native optimizations
        # Enable CPU instruction set optimizations for maximum performance
        "-D", "GGML_SSE42=ON",  # Enable SSE4.2
        "-D", "GGML_AVX=ON",  # Enable AVX
        "-D", "GGML_AVX2=ON",  # Enable AVX2
        "-D", "GGML_BMI2=ON",  # Enable BMI2
        "-D", "GGML_FMA=ON",  # Enable FMA (fused multiply-add)
        "-D", "GGML_F16C=ON",  # Enable F16C (half-precision conversion)
    ]
    
    # Add LTO only for Release builds (Debug builds are slower with LTO)
    if config_name == "Release":
        cmake_cmd.extend([
            "-D", "GGML_LTO=ON",  # Enable Link Time Optimization
        ])
    
    # Disable unnecessary components
    cmake_cmd.extend([
        "-D", "LLAMA_BUILD_TESTS=OFF",  # Disable tests
        "-D", "LLAMA_BUILD_EXAMPLES=OFF",  # Disable examples
        "-D", "LLAMA_CURL=OFF",  # Disable CURL
        "-D", "LLAMA_HTTP=OFF",  # Disable HTTP support
        "-D", "LLAMA_SERVER=OFF",  # Disable server
        ".."
    ])
    
    print(f"🔧 Running: {' '.join(cmake_cmd)}")
    try:
        subprocess.run(cmake_cmd, cwd=build_dir, check=True)
        print(f"✅ CMake configuration successful ({config_name})")
    except subprocess.CalledProcessError as e:
        print(f"❌ CMake configuration failed ({config_name}): {e}")
        return False
    
    # Build the libraries
    print(f"🔨 Building LLAMA.cpp libraries ({config_name})...")
    build_cmd = [
        "cmake", "--build", ".", "--config", config_name
    ]
    
    try:
        subprocess.run(build_cmd, cwd=build_dir, check=True)
        print(f"✅ Build successful ({config_name})")
    except subprocess.CalledProcessError as e:
        print(f"❌ Build failed ({config_name}): {e}")
        return False
    
    # Copy libraries to the correct location
    print(f"📦 Copying libraries ({config_name})...")
    source_lib_dir = Path(target_dir)
    source_lib_dir.mkdir(parents=True, exist_ok=True)
    
    # Find the built libraries - they're in different subdirectories
    lib_dirs = [
        build_dir / "ggml" / "src" / config_name,
        build_dir / "src" / config_name, 
        build_dir / "tools" / "mtmd" / config_name,
        build_dir / "common" / config_name
    ]
    
    lib_files = []
    for lib_dir in lib_dirs:
        if lib_dir.exists():
            lib_files.extend(list(lib_dir.glob("*.lib")))
    
    if not lib_files:
        print(f"❌ No .lib files found in any library directories ({config_name})")
        return False
    
    print(f"📦 Found {len(lib_files)} library files ({config_name}):")
    for lib_file in lib_files:
        print(f"  - {lib_file.name}")
    
    for lib_file in lib_files:
        dest_file = source_lib_dir / lib_file.name
        shutil.copy2(lib_file, dest_file)
        print(f"✅ Copied {lib_file.name} to {dest_file}")
    
    print(f"🎉 {config_name} libraries built and copied successfully!")
    return True

if __name__ == "__main__":
    success = rebuild_llama_windows()
    if success:
        print("\n✅ LLAMA.cpp rebuild complete!")
        print("📁 Release libraries: source/llama-lib/win-amd64/")
        print("📁 Debug libraries: source/llama-lib/win-amd64-debug/")
        print("You can now rebuild Grapa with the updated libraries.")
    else:
        print("\n❌ LLAMA.cpp rebuild failed!")
        sys.exit(1)