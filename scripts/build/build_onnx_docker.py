#!/usr/bin/env python3
"""
Docker-based ONNX Runtime Build Script for macOS
Uses Docker to build ONNX Runtime static libraries in a Linux environment.
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
            print("❌ Docker not found. Please install Docker Desktop for Mac")
            return False
        else:
            print(f"✅ Docker found: {result.stdout.strip()}")
    except FileNotFoundError:
        print("❌ Docker not found. Please install Docker Desktop for Mac")
        return False
    
    # Check if Docker daemon is running
    try:
        result = subprocess.run(['docker', 'info'], capture_output=True, text=True)
        if result.returncode != 0:
            print("❌ Docker daemon not running. Please start Docker Desktop")
            return False
        else:
            print("✅ Docker daemon is running")
    except Exception as e:
        print(f"❌ Docker daemon error: {e}")
        return False
    
    return True

def build_onnx_docker():
    """Build ONNX Runtime using Docker."""
    print("🐳 Building ONNX Runtime with Docker...")
    
    script_dir = Path(__file__).parent
    dockerfile_path = script_dir / "Dockerfile.onnx-mac"
    
    if not dockerfile_path.exists():
        print(f"❌ Dockerfile not found: {dockerfile_path}")
        return False
    
    # Create output directory
    output_dir = script_dir.parent.parent / "temp_onnx_docker_output"
    output_dir.mkdir(exist_ok=True)
    
    try:
        # Build Docker image
        print("🔨 Building Docker image...")
        build_cmd = [
            "docker", "build",
            "-f", str(dockerfile_path),
            "-t", "onnx-builder",
            str(script_dir)
        ]
        
        result = subprocess.run(build_cmd, check=True, capture_output=True, text=True)
        print("✅ Docker image built successfully")
        
        # Run Docker container to extract libraries
        print("🚀 Running Docker container to extract libraries...")
        run_cmd = [
            "docker", "run",
            "--rm",
            "-v", f"{output_dir}:/host_output",
            "onnx-builder"
        ]
        
        result = subprocess.run(run_cmd, check=True, capture_output=True, text=True)
        print("✅ Libraries extracted from Docker container")
        
        # Check what was extracted
        lib_files = list(output_dir.glob("lib/*.a"))
        header_files = list(output_dir.glob("include/**/*.h"))
        
        print(f"📚 Found {len(lib_files)} static libraries")
        print(f"📄 Found {len(header_files)} header files")
        
        if lib_files:
            print("✅ Static libraries found!")
            for lib_file in lib_files:
                print(f"  - {lib_file.name}")
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

def copy_docker_output():
    """Copy the Docker-built libraries to the appropriate locations."""
    print("📁 Copying Docker-built libraries...")
    
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    output_dir = project_root / "temp_onnx_docker_output"
    
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
    """Clean up temporary files and Docker image."""
    print("🧹 Cleaning up...")
    
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    output_dir = project_root / "temp_onnx_docker_output"
    
    # Remove temporary output directory
    if output_dir.exists():
        shutil.rmtree(output_dir)
        print("  🗑️  Removed temporary output directory")
    
    # Remove Docker image
    try:
        subprocess.run(['docker', 'rmi', 'onnx-builder'], capture_output=True)
        print("  🗑️  Removed Docker image")
    except:
        pass  # Image might not exist

def main():
    """Main function for Docker-based ONNX Runtime build."""
    print("🐳 ONNX Runtime Docker Build Script for macOS")
    print("=" * 60)
    
    # Check if we're on macOS
    if platform.system().lower() != 'darwin':
        print("❌ This script is designed for macOS")
        return 1
    
    print(f"🎯 Target platform: macOS {platform.machine()}")
    
    # Check Docker
    if not check_docker():
        print("\n📝 Docker Setup Instructions:")
        print("  1. Install Docker Desktop for Mac from: https://www.docker.com/products/docker-desktop/")
        print("  2. Start Docker Desktop")
        print("  3. Run this script again")
        return 1
    
    # Build ONNX Runtime with Docker
    if not build_onnx_docker():
        print("\n❌ Docker build failed")
        return 1
    
    # Copy the built libraries
    if not copy_docker_output():
        print("\n❌ Failed to copy libraries")
        return 1
    
    # Clean up
    cleanup()
    
    print("\n✅ ONNX Runtime static libraries built successfully with Docker!")
    print("📝 Next steps:")
    print("  1. Run 'python3 build.py --exe-only' to build Grapa with ONNX Runtime")
    print("  2. Test ONNX functionality with your models")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
