#!/usr/bin/env python3
"""
ONNX Runtime Pre-built Libraries Download Script for Grapa
Downloads pre-built ONNX Runtime static libraries from community repository.

This script downloads pre-built ONNX Runtime static libraries instead of building from source,
avoiding protobuf version compatibility issues.
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
    """Get the latest release information from the community repository."""
    print("🔍 Fetching latest ONNX Runtime release information...")
    
    try:
        # Create SSL context that doesn't verify certificates
        ssl_context = ssl.create_default_context()
        ssl_context.check_hostname = False
        ssl_context.verify_mode = ssl.CERT_NONE
        
        url = "https://api.github.com/repos/csukuangfj/onnxruntime-libs/releases/latest"
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
            return 'onnxruntime-osx-arm64-1.22.2.zip', 'mac-arm64'
        elif machine == 'x86_64':
            return 'onnxruntime-osx-x86_64-1.22.2.zip', 'mac-amd64'
        else:
            return 'onnxruntime-osx-universal2-1.22.2.zip', 'mac-universal'
    elif system == 'linux':
        if machine == 'aarch64' or machine == 'arm64':
            return 'onnxruntime-linux-aarch64-glibc2_28-Release-1.22.2.zip', 'linux-arm64'
        elif machine == 'x86_64':
            return 'onnxruntime-linux-x64-glibc2_17-Release-1.22.2.zip', 'linux-amd64'
        else:
            return 'onnxruntime-linux-x64-glibc2_17-Release-1.22.2.zip', 'linux-amd64'
    else:
        print(f"❌ Unsupported platform: {system} {machine}")
        return None, None

def download_onnxruntime_libs(platform_name, filename):
    """Download ONNX Runtime pre-built libraries."""
    print(f"📥 Downloading ONNX Runtime libraries for {platform_name}...")
    
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
        return False
    
    print(f"📥 Downloading from: {download_url}")
    
    # Create temp directory
    temp_dir = Path("temp_onnx_download")
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
        has_static_libs = copy_onnxruntime_files(extracted_dir, platform_name)
        
        # Clean up
        shutil.rmtree(temp_dir)
        print("🧹 Cleaned up temporary files")
        
        return has_static_libs
        
    except Exception as e:
        print(f"❌ Error downloading/extracting ONNX Runtime: {e}")
        if temp_dir.exists():
            shutil.rmtree(temp_dir)
        return False

def copy_onnxruntime_files(source_dir, platform_name):
    """Copy ONNX Runtime files to the appropriate locations."""
    print(f"📁 Copying ONNX Runtime files for {platform_name}...")
    
    # Create target directories
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    lib_dir = project_root / "source" / "onnxruntime-lib" / platform_name
    include_dir = project_root / "source" / "onnxruntime"
    
    lib_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)
    
    # Check for static libraries first
    static_libs = list(source_dir.glob("**/*.a")) + list(source_dir.glob("**/*.lib"))
    shared_libs = list(source_dir.glob("**/*.dylib")) + list(source_dir.glob("**/*.so")) + list(source_dir.glob("**/*.dll"))
    
    if static_libs:
        print("  📚 Found static libraries (.a/.lib files)")
        for lib_file in static_libs:
            if lib_file.is_file():
                dest_path = lib_dir / lib_file.name
                shutil.copy2(lib_file, dest_path)
                print(f"    ✅ Copied library: {lib_file.name}")
    elif shared_libs:
        print("  ⚠️  Only shared libraries found (.dylib/.so/.dll files)")
        print("  📝 Note: Grapa requires static libraries for linking")
        print("  🔧 Consider building ONNX Runtime from source for static libraries")
        
        # Copy shared libraries anyway for reference
        for lib_file in shared_libs:
            if lib_file.is_file():
                dest_path = lib_dir / lib_file.name
                shutil.copy2(lib_file, dest_path)
                print(f"    📦 Copied shared library: {lib_file.name}")
    else:
        print("  ❌ No libraries found in the downloaded package")
        return False
    
    # Copy headers
    header_files = list(source_dir.glob("**/*.h"))
    for header_file in header_files:
        if header_file.is_file():
            # Preserve directory structure for headers
            rel_path = header_file.relative_to(source_dir)
            dest_path = include_dir / rel_path
            dest_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(header_file, dest_path)
            print(f"  ✅ Copied header: {rel_path}")
    
    print(f"✅ ONNX Runtime files copied to:")
    print(f"  📚 Libraries: {lib_dir}")
    print(f"  📄 Headers: {include_dir}")
    
    return len(static_libs) > 0

def main():
    """Main function to download and setup ONNX Runtime libraries."""
    print("🚀 ONNX Runtime Pre-built Libraries Download Script")
    print("=" * 60)
    
    # Detect platform
    filename, platform_name = detect_platform()
    if not platform_name:
        print("❌ Unsupported platform")
        return 1
    
    print(f"🎯 Target platform: {platform_name}")
    print(f"📦 Download file: {filename}")
    
    # Download and setup ONNX Runtime
    success = download_onnxruntime_libs(platform_name, filename)
    
    if success:
        print("\n✅ ONNX Runtime libraries downloaded and installed successfully!")
        print("📝 Next steps:")
        print("  1. Run 'python3 build.py --exe-only' to build Grapa with ONNX Runtime")
        print("  2. Test ONNX functionality with your models")
        return 0
    else:
        print("\n⚠️  ONNX Runtime downloaded but only shared libraries found")
        print("📝 This means:")
        print("  - Headers are available for development")
        print("  - Static libraries are needed for Grapa linking")
        print("  - Consider building ONNX Runtime from source for static libraries")
        print("  - Or use the placeholder ONNX implementation for now")
        return 0

if __name__ == "__main__":
    sys.exit(main())