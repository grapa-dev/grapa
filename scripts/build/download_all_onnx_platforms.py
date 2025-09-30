#!/usr/bin/env python3
"""
Download ONNX Runtime shared libraries for all supported Grapa platforms.
This script downloads libraries for all platforms that Grapa supports.
"""

import os
import sys
import platform
import urllib.request
import ssl
import tarfile
import zipfile
import shutil
from pathlib import Path

# Add the project root to the path so we can import from scripts
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))

def download_file(url, filepath):
    """Download a file from URL to filepath."""
    print(f"📥 Downloading from: {url}")
    print(f"📥 Downloading to: {filepath}")
    
    # Create SSL context that doesn't verify certificates
    ssl_context = ssl.create_default_context()
    ssl_context.check_hostname = False
    ssl_context.verify_mode = ssl.CERT_NONE
    
    try:
        with urllib.request.urlopen(url, context=ssl_context) as response:
            with open(filepath, 'wb') as f:
                f.write(response.read())
        print(f"✅ Downloaded {filepath.name}")
        return True
    except Exception as e:
        print(f"❌ Failed to download {url}: {e}")
        return False

def extract_archive(archive_path, extract_to):
    """Extract archive to directory."""
    print(f"📦 Extracting {archive_path.name}...")
    
    try:
        if archive_path.suffix == '.tgz' or archive_path.suffix == '.tar.gz':
            with tarfile.open(archive_path, 'r:gz') as tar:
                tar.extractall(extract_to)
        elif archive_path.suffix == '.zip':
            with zipfile.ZipFile(archive_path, 'r') as zip_ref:
                zip_ref.extractall(extract_to)
        else:
            print(f"❌ Unsupported archive format: {archive_path.suffix}")
            return False
        
        print(f"📦 Extracted to: {extract_to}")
        return True
    except Exception as e:
        print(f"❌ Failed to extract {archive_path}: {e}")
        return False

def copy_onnx_libraries(source_dir, platform_name):
    """Copy ONNX Runtime libraries and headers for the platform."""
    print(f"📁 Copying ONNX Runtime shared libraries for {platform_name}...")
    
    # Create target directories
    lib_dir = Path(f"source/onnxruntime-lib/{platform_name}")
    include_dir = Path("source/onnxruntime")
    
    lib_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)
    
    # Copy shared libraries
    lib_files_copied = 0
    for lib_file in source_dir.rglob("*"):
        if lib_file.is_file() and lib_file.suffix in ['.dylib', '.so', '.dll']:
            dest_path = lib_dir / lib_file.name
            shutil.copy2(lib_file, dest_path)
            print(f"  ✅ Copied shared library: {lib_file.name}")
            lib_files_copied += 1
    
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
    
    # Create README
    readme_path = lib_dir / "README_SHARED_LIBS.md"
    with open(readme_path, 'w') as f:
        f.write(f"""# ONNX Runtime Shared Libraries - {platform_name}

This directory contains ONNX Runtime shared libraries for {platform_name}.

## Files
- `libonnxruntime.1.23.0.dylib` - Versioned shared library
- `libonnxruntime.dylib` - Unversioned symlink/shim

## Usage
These libraries are automatically linked by the Grapa build system.
They must be available at runtime for ONNX functionality to work.

## Installation
The libraries are automatically copied to the installation directory
when using the Grapa installer.
""")
    
    print(f"✅ ONNX Runtime shared libraries copied to:")
    print(f"  📚 Libraries: {lib_dir}")
    print(f"  📄 Headers: {include_dir}")
    print(f"  📝 Created README: {readme_path}")
    
    return lib_files_copied > 0

def download_platform_libraries(platform_name, download_filename):
    """Download ONNX Runtime libraries for a specific platform."""
    print(f"\n🎯 Downloading ONNX Runtime libraries for {platform_name}...")
    
    # Create temporary directory
    temp_dir = Path("temp_onnx_shared")
    temp_dir.mkdir(exist_ok=True)
    
    # Download URL
    base_url = "https://github.com/microsoft/onnxruntime/releases/download/v1.23.0"
    url = f"{base_url}/{download_filename}"
    
    # Download file
    archive_path = temp_dir / download_filename
    if not download_file(url, archive_path):
        return False
    
    # Extract archive
    extract_dir = temp_dir / download_filename.replace('.tgz', '').replace('.zip', '')
    if not extract_archive(archive_path, temp_dir):
        return False
    
    # Copy libraries and headers
    if not copy_onnx_libraries(extract_dir, platform_name):
        return False
    
    # Clean up
    shutil.rmtree(temp_dir)
    print(f"🧹 Cleaned up temporary files for {platform_name}")
    
    return True

def main():
    """Download ONNX Runtime libraries for all supported platforms."""
    print("🚀 ONNX Runtime Shared Libraries Download Script - All Platforms")
    print("=" * 70)
    
    # Define all supported platforms and their download files
    platforms = {
        'mac-arm64': 'onnxruntime-osx-arm64-1.23.0.tgz',
        'mac-amd64': 'onnxruntime-osx-universal2-1.23.0.tgz',  # Universal binary
        'linux-amd64': 'onnxruntime-linux-x64-1.23.0.tgz',
        'linux-arm64': 'onnxruntime-linux-aarch64-1.23.0.tgz',
        'aws-amd64': 'onnxruntime-linux-x64-1.23.0.tgz',  # Same as linux-amd64
        'aws-arm64': 'onnxruntime-linux-aarch64-1.23.0.tgz',  # Same as linux-arm64
        'win-amd64': 'onnxruntime-win-x64-1.23.0.zip',
    }
    
    success_count = 0
    total_count = len(platforms)
    
    for platform_name, download_filename in platforms.items():
        try:
            if download_platform_libraries(platform_name, download_filename):
                success_count += 1
                print(f"✅ Successfully downloaded libraries for {platform_name}")
            else:
                print(f"❌ Failed to download libraries for {platform_name}")
        except Exception as e:
            print(f"❌ Error downloading libraries for {platform_name}: {e}")
    
    print(f"\n📊 Summary: {success_count}/{total_count} platforms completed successfully")
    
    if success_count == total_count:
        print("✅ All ONNX Runtime shared libraries downloaded and installed successfully!")
        print("📝 Next steps:")
        print("  1. Run 'python3 build.py --exe-only' to build Grapa with ONNX Runtime")
        print("  2. Test ONNX functionality with your models")
        print("  3. Ensure ONNX Runtime shared libraries are available at runtime")
    else:
        print("⚠️  Some platforms failed. Check the output above for details.")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
