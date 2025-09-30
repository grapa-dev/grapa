#!/usr/bin/env python3
"""
ONNX Runtime Shared Library Download Script for Grapa
Downloads official ONNX Runtime shared libraries from Microsoft releases.
"""

import os
import sys
import platform
import subprocess
import shutil
import urllib.request
import zipfile
import json
import ssl
from pathlib import Path

def get_latest_release_info():
    """Get the latest release information from the official ONNX Runtime repository."""
    print("🔍 Fetching latest ONNX Runtime release information...")
    
    try:
        # Create SSL context that doesn't verify certificates
        ssl_context = ssl.create_default_context()
        ssl_context.check_hostname = False
        ssl_context.verify_mode = ssl.CERT_NONE
        
        url = "https://api.github.com/repos/microsoft/onnxruntime/releases/latest"
        with urllib.request.urlopen(url, context=ssl_context) as response:
            data = json.loads(response.read().decode())
            return data
    except Exception as e:
        print(f"❌ Failed to fetch release information: {e}")
        return None

def detect_platform():
    """Detect the current platform and return the appropriate download filename and platform name."""
    system = platform.system().lower()
    machine = platform.machine().lower()
    
    print(f"🔍 Detected platform: {system} {machine}")
    
    # Map platform to download filename and platform name
    if system == 'darwin':
        if machine == 'arm64':
            return 'onnxruntime-osx-arm64-1.23.0.tgz', 'mac-arm64'
        elif machine == 'x86_64':
            # Use universal2 for x86_64 macOS to support both Intel and Apple Silicon
            return 'onnxruntime-osx-universal2-1.23.0.tgz', 'mac-amd64'
        else:
            return 'onnxruntime-osx-universal2-1.23.0.tgz', 'mac-universal'
    elif system == 'linux':
        if machine == 'aarch64' or machine == 'arm64':
            return 'onnxruntime-linux-aarch64-1.23.0.tgz', 'linux-arm64'
        elif machine == 'x86_64':
            return 'onnxruntime-linux-x64-1.23.0.tgz', 'linux-amd64'
        else:
            return 'onnxruntime-linux-x64-1.23.0.tgz', 'linux-amd64'
    elif system == 'windows':
        if machine == 'amd64':
            return 'onnxruntime-win-x64-1.23.0.zip', 'win-amd64'
        elif machine == 'arm64':
            return 'onnxruntime-win-arm64-1.23.0.zip', 'win-arm64'
        else:
            return 'onnxruntime-win-x64-1.23.0.zip', 'win-amd64'
    else:
        print(f"❌ Unsupported platform: {system} {machine}")
        return None, None

def download_onnxruntime_shared(platform_name, filename):
    """Download ONNX Runtime shared libraries."""
    print(f"📥 Downloading ONNX Runtime shared libraries for {platform_name}...")
    
    # Get release info
    release_info = get_latest_release_info()
    if not release_info:
        return False
    
    # Find the download URL for our platform
    download_url = None
    for asset in release_info.get('assets', []):
        if asset['name'] == filename:
            download_url = asset['browser_download_url']
            break
    
    if not download_url:
        print(f"❌ Could not find download URL for {filename}")
        print("Available assets:")
        for asset in release_info.get('assets', []):
            print(f"  - {asset['name']}")
        return False
    
    print(f"📥 Downloading from: {download_url}")
    
    # Create temp directory
    temp_dir = Path("temp_onnx_shared")
    temp_dir.mkdir(exist_ok=True)
    
    try:
        # Download the file
        zip_path = temp_dir / filename
        print(f"📥 Downloading to: {zip_path}")
        
        # Create SSL context for download
        ssl_context = ssl.create_default_context()
        ssl_context.check_hostname = False
        ssl_context.verify_mode = ssl.CERT_NONE
        
        # Use urlopen with SSL context instead of urlretrieve
        with urllib.request.urlopen(download_url, context=ssl_context) as response:
            with open(zip_path, 'wb') as f:
                f.write(response.read())
        print(f"✅ Downloaded {filename}")
        
        # Extract the archive
        print(f"📦 Extracting {filename}...")
        if filename.endswith('.tgz'):
            subprocess.run(['tar', '-xzf', str(zip_path), '-C', str(temp_dir)], check=True)
        else:
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(temp_dir)
        
        # Find the extracted directory
        extracted_dirs = [d for d in temp_dir.iterdir() if d.is_dir() and d.name != '__pycache__']
        if not extracted_dirs:
            print("❌ No extracted directory found")
            return False
        
        extracted_dir = extracted_dirs[0]
        print(f"📦 Extracted to: {extracted_dir}")
        
        # Copy libraries to target directory
        copy_onnxruntime_shared_files(extracted_dir, platform_name)
        
        # Clean up
        shutil.rmtree(temp_dir)
        print("🧹 Cleaned up temporary files")
        
        return True
        
    except Exception as e:
        print(f"❌ Error downloading/extracting ONNX Runtime: {e}")
        if temp_dir.exists():
            shutil.rmtree(temp_dir)
        return False

def copy_onnxruntime_shared_files(source_dir, platform_name):
    """Copy ONNX Runtime shared library files to the appropriate locations."""
    print(f"📁 Copying ONNX Runtime shared libraries for {platform_name}...")
    
    # Create target directories
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    lib_dir = project_root / "source" / "onnxruntime-lib" / platform_name
    include_dir = project_root / "source" / "onnxruntime"
    
    lib_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)
    
    # Copy shared libraries
    shared_libs = list(source_dir.glob("**/*.dylib")) + list(source_dir.glob("**/*.so")) + list(source_dir.glob("**/*.dll"))
    for lib_file in shared_libs:
        if lib_file.is_file():
            dest_path = lib_dir / lib_file.name
            shutil.copy2(lib_file, dest_path)
            print(f"  ✅ Copied shared library: {lib_file.name}")
    
    # Note: We keep the unversioned library for the executable build
    # The Python package build process will handle creating a copy for distribution
    # This ensures the executable can link against the unversioned library
    
    # Copy headers from the include directory directly to source/onnxruntime
    include_source_dir = source_dir / "include"
    if include_source_dir.exists():
        # Copy all contents of the include directory directly to source/onnxruntime
        for item in include_source_dir.rglob("*"):
            if item.is_file():
                # Get relative path from the include directory
                rel_path = item.relative_to(include_source_dir)
                dest_path = include_dir / rel_path
                dest_path.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(item, dest_path)
                print(f"  ✅ Copied header: {rel_path}")
    else:
        print("  ⚠️  Warning: No include directory found in the downloaded package")
    
    print(f"✅ ONNX Runtime shared libraries copied to:")
    print(f"  📚 Libraries: {lib_dir}")
    print(f"  📄 Headers: {include_dir}")
    
    # Create a README explaining the shared library approach
    readme_path = lib_dir / "README_SHARED_LIBS.md"
    with open(readme_path, 'w') as f:
        f.write(f"""# ONNX Runtime Shared Libraries for {platform_name}

This directory contains ONNX Runtime shared libraries downloaded from the official Microsoft releases.

## Files:
""")
        for lib_file in shared_libs:
            if lib_file.is_file():
                f.write(f"- {lib_file.name}\n")
        
        f.write(f"""
## Usage:
- The Grapa application will link against these shared libraries at runtime
- Users must have ONNX Runtime installed on their system
- If ONNX Runtime is not available, the .load() method for "onnx" will fail gracefully

## Installation Requirements:
- macOS: No additional installation required (libraries included)
- Linux: May require additional dependencies (libprotobuf, etc.)
- Windows: May require Visual C++ Redistributable

## Runtime Dependencies:
- The shared libraries must be available in the system library path
- For development, ensure the libraries are in the same directory as the executable
- For production, consider bundling the libraries with the application
""")
    
    print(f"  📝 Created README: {readme_path}")

def main():
    """Main function to download and setup ONNX Runtime shared libraries."""
    print("🚀 ONNX Runtime Shared Libraries Download Script")
    print("=" * 60)
    
    # Detect platform
    filename, platform_name = detect_platform()
    if not platform_name:
        print("❌ Unsupported platform")
        return 1
    
    print(f"🎯 Target platform: {platform_name}")
    print(f"📦 Download file: {filename}")
    
    # Download and setup ONNX Runtime
    success = download_onnxruntime_shared(platform_name, filename)
    
    if success:
        print("\n✅ ONNX Runtime shared libraries downloaded and installed successfully!")
        print("📝 Next steps:")
        print("  1. Run 'python3 build.py --exe-only' to build Grapa with ONNX Runtime")
        print("  2. Test ONNX functionality with your models")
        print("  3. Ensure ONNX Runtime shared libraries are available at runtime")
        return 0
    else:
        print("\n❌ Failed to download ONNX Runtime shared libraries")
        return 1

if __name__ == "__main__":
    sys.exit(main())
