#!/usr/bin/env python3
"""
Simple Docker-based ONNX Runtime Build Script
Uses a pre-built Linux container to build ONNX Runtime static libraries.
"""

import os
import sys
import platform
import subprocess
import shutil
from pathlib import Path

def check_docker():
    """Check if Docker is available and running."""
    print("🔍 Checking Docker availability...")
    
    try:
        result = subprocess.run(['docker', '--version'], capture_output=True, text=True)
        if result.returncode != 0:
            return False
        print(f"✅ Docker found: {result.stdout.strip()}")
        
        # Check if Docker daemon is running
        result = subprocess.run(['docker', 'info'], capture_output=True, text=True)
        if result.returncode != 0:
            print("❌ Docker daemon not running")
            return False
        print("✅ Docker daemon is running")
        return True
        
    except FileNotFoundError:
        print("❌ Docker not found")
        return False
    except Exception as e:
        print(f"❌ Docker error: {e}")
        return False

def build_with_docker():
    """Build ONNX Runtime using a simple Docker approach."""
    print("🐳 Building ONNX Runtime with Docker...")
    
    # Create output directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    output_dir = project_root / "temp_onnx_docker"
    output_dir.mkdir(exist_ok=True)
    
    try:
        # Use a simple one-liner Docker build
        print("🔨 Building ONNX Runtime in Docker container...")
        
        docker_cmd = [
            "docker", "run", "--rm",
            "-v", f"{output_dir}:/output",
            "ubuntu:22.04",
            "bash", "-c", """
                set -e
                apt-get update
                apt-get install -y build-essential cmake git python3 python3-pip libprotobuf-dev protobuf-compiler
                pip3 install numpy
                git clone --recursive https://github.com/microsoft/onnxruntime.git
                cd onnxruntime
                python3 tools/ci_build/build.py --config Release --build_dir build --parallel --cmake_extra_defines CMAKE_CXX_FLAGS=-fPIC --cmake_extra_defines CMAKE_C_FLAGS=-fPIC --cmake_extra_defines ONNX_BUILD_SHARED_LIBS=OFF
                mkdir -p /output/lib /output/include
                find build -name "*.a" -exec cp {} /output/lib/ \\;
                cp -r include/* /output/include/
                echo "Build completed successfully"
            """
        ]
        
        print("⏳ This may take 10-15 minutes...")
        result = subprocess.run(docker_cmd, check=True, capture_output=True, text=True)
        print("✅ Docker build completed successfully")
        
        # Check what was built
        lib_files = list(output_dir.glob("lib/*.a"))
        header_files = list(output_dir.glob("include/**/*.h"))
        
        print(f"📚 Found {len(lib_files)} static libraries")
        print(f"📄 Found {len(header_files)} header files")
        
        if lib_files:
            print("✅ Static libraries found!")
            for lib_file in lib_files[:5]:  # Show first 5
                print(f"  - {lib_file.name}")
            if len(lib_files) > 5:
                print(f"  ... and {len(lib_files) - 5} more")
        else:
            print("❌ No static libraries found")
            return False
        
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Docker build failed: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False
    except Exception as e:
        print(f"❌ Docker error: {e}")
        return False

def copy_to_grapa():
    """Copy the Docker-built libraries to Grapa directories."""
    print("📁 Copying libraries to Grapa directories...")
    
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    output_dir = project_root / "temp_onnx_docker"
    
    if not output_dir.exists():
        print("❌ Docker output directory not found")
        return False
    
    # Determine platform name
    system = platform.system().lower()
    machine = platform.machine().lower()
    
    if system == 'darwin':
        if machine == 'arm64':
            platform_name = 'mac-arm64'
        elif machine == 'x86_64':
            platform_name = 'mac-amd64'
        else:
            platform_name = 'mac-universal'
    else:
        print(f"❌ This script is designed for macOS, detected: {system}")
        return False
    
    # Create target directories
    lib_dir = project_root / "source" / "onnxruntime-lib" / platform_name
    include_dir = project_root / "source" / "onnxruntime"
    
    lib_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)
    
    # Copy libraries
    lib_files = list(output_dir.glob("lib/*.a"))
    for lib_file in lib_files:
        if lib_file.is_file():
            dest_path = lib_dir / lib_file.name
            shutil.copy2(lib_file, dest_path)
            print(f"  ✅ Copied library: {lib_file.name}")
    
    # Copy headers
    header_files = list(output_dir.glob("include/**/*.h"))
    for header_file in header_files:
        if header_file.is_file():
            rel_path = header_file.relative_to(output_dir / "include")
            dest_path = include_dir / rel_path
            dest_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(header_file, dest_path)
            print(f"  ✅ Copied header: {rel_path}")
    
    print(f"✅ Libraries copied to:")
    print(f"  📚 Libraries: {lib_dir}")
    print(f"  📄 Headers: {include_dir}")
    
    return True

def cleanup():
    """Clean up temporary files."""
    print("🧹 Cleaning up...")
    
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    output_dir = project_root / "temp_onnx_docker"
    
    if output_dir.exists():
        shutil.rmtree(output_dir)
        print("  🗑️  Removed temporary directory")

def main():
    """Main function for simple Docker-based ONNX Runtime build."""
    print("🐳 Simple ONNX Runtime Docker Build Script")
    print("=" * 50)
    
    # Check if we're on macOS
    if platform.system().lower() != 'darwin':
        print("❌ This script is designed for macOS")
        return 1
    
    print(f"🎯 Target platform: macOS {platform.machine()}")
    
    # Check Docker
    if not check_docker():
        print("\n📝 Docker Setup Instructions:")
        print("  1. Install Docker Desktop for Mac")
        print("  2. Start Docker Desktop")
        print("  3. Run this script again")
        return 1
    
    # Build ONNX Runtime with Docker
    if not build_with_docker():
        print("\n❌ Docker build failed")
        return 1
    
    # Copy the built libraries
    if not copy_to_grapa():
        print("\n❌ Failed to copy libraries")
        return 1
    
    # Clean up
    cleanup()
    
    print("\n✅ ONNX Runtime static libraries built successfully!")
    print("📝 Next steps:")
    print("  1. Run 'python3 build.py --exe-only' to build Grapa with ONNX Runtime")
    print("  2. Test ONNX functionality with your models")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
