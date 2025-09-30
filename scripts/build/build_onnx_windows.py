#!/usr/bin/env python3
"""
ONNX Runtime Pre-built Libraries Download Script for Windows
Downloads pre-built ONNX Runtime static libraries from community repository.

Note: The community repository (csukuangfj/onnxruntime-libs) does not currently
provide Windows builds. This script provides a fallback approach.
"""

import os
import sys
import platform
import subprocess
import shutil
import urllib.request
import zipfile
import json
from pathlib import Path

def get_latest_release_info():
    """Get the latest release information from the community repository."""
    print("🔍 Fetching latest ONNX Runtime release information...")
    
    try:
        url = "https://api.github.com/repos/csukuangfj/onnxruntime-libs/releases/latest"
        with urllib.request.urlopen(url) as response:
            data = json.loads(response.read().decode())
            return data
    except Exception as e:
        print(f"❌ Failed to fetch release information: {e}")
        return None

def check_windows_builds():
    """Check if Windows builds are available in the community repository."""
    print("🔍 Checking for Windows builds in community repository...")
    
    release_info = get_latest_release_info()
    if not release_info:
        return False
    
    # Look for Windows builds
    windows_builds = []
    for asset in release_info.get('assets', []):
        if 'win' in asset['name'].lower() or 'windows' in asset['name'].lower():
            windows_builds.append(asset['name'])
    
    if windows_builds:
        print(f"✅ Found Windows builds: {windows_builds}")
        return True
    else:
        print("❌ No Windows builds found in community repository")
        return False

def download_alternative_windows_build():
    """Download alternative Windows build or provide instructions."""
    print("🔍 Checking for alternative Windows ONNX Runtime builds...")
    
    # Check if we can find Windows builds in the official repository
    try:
        url = "https://api.github.com/repos/microsoft/onnxruntime/releases/latest"
        with urllib.request.urlopen(url) as response:
            data = json.loads(response.read().decode())
            
            # Look for Windows static libraries
            windows_assets = []
            for asset in data.get('assets', []):
                if 'win' in asset['name'].lower() and ('static' in asset['name'].lower() or 'lib' in asset['name'].lower()):
                    windows_assets.append(asset)
            
            if windows_assets:
                print(f"✅ Found Windows assets in official repository:")
                for asset in windows_assets:
                    print(f"  - {asset['name']}")
                return True
            else:
                print("❌ No Windows static libraries found in official repository")
                return False
                
    except Exception as e:
        print(f"❌ Error checking official repository: {e}")
        return False

def provide_windows_instructions():
    """Provide instructions for Windows users."""
    print("\n📋 Windows ONNX Runtime Setup Instructions:")
    print("=" * 50)
    print("The community repository does not provide Windows builds.")
    print("For Windows, you have the following options:")
    print()
    print("1. 🏗️  Build from source (recommended):")
    print("   - Install Visual Studio 2019 or later")
    print("   - Install CMake")
    print("   - Clone ONNX Runtime repository")
    print("   - Build with: .\\build.bat --config Release --build_shared_lib OFF")
    print()
    print("2. 📦 Use official shared libraries:")
    print("   - Download from: https://github.com/microsoft/onnxruntime/releases")
    print("   - Extract and use the shared libraries")
    print("   - Note: This requires runtime dependencies")
    print()
    print("3. 🔧 Use vcpkg (if available):")
    print("   - Install vcpkg")
    print("   - Run: vcpkg install onnxruntime:x64-windows-static")
    print()
    print("For now, this script will create placeholder directories.")
    print("You'll need to manually build or obtain Windows ONNX Runtime libraries.")

def create_placeholder_directories():
    """Create placeholder directories for Windows ONNX Runtime."""
    print("📁 Creating placeholder directories for Windows ONNX Runtime...")
    
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    lib_dir = project_root / "source" / "onnxruntime-lib" / "win-amd64"
    include_dir = project_root / "source" / "onnxruntime"
    
    lib_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)
    
    # Create a placeholder README
    readme_path = lib_dir / "README_WINDOWS.md"
    with open(readme_path, 'w') as f:
        f.write("""# Windows ONNX Runtime Libraries

This directory should contain Windows ONNX Runtime static libraries.

## Required Files:
- onnxruntime.lib (or onnxruntime.dll)
- onnxruntime_common.lib
- onnxruntime_framework.lib
- onnxruntime_graph.lib
- onnxruntime_mlas.lib
- onnxruntime_optimizer.lib
- onnxruntime_providers.lib
- onnxruntime_session.lib
- onnxruntime_util.lib
- onnx.lib
- onnx_proto.lib
- libprotobuf-lite.lib
- re2.lib
- absl_*.lib (various Abseil libraries)

## How to obtain:
1. Build ONNX Runtime from source
2. Use vcpkg: vcpkg install onnxruntime:x64-windows-static
3. Download from official releases (shared libraries only)

## Build Instructions:
```bash
git clone --recursive https://github.com/microsoft/onnxruntime
cd onnxruntime
.\build.bat --config Release --build_shared_lib OFF
```

The static libraries will be in: build/Windows/Release/
""")
    
    print(f"✅ Created placeholder directories:")
    print(f"  📚 Libraries: {lib_dir}")
    print(f"  📄 Headers: {include_dir}")
    print(f"  📝 Instructions: {readme_path}")

def main():
    """Main function for Windows ONNX Runtime setup."""
    print("🚀 ONNX Runtime Windows Setup Script")
    print("=" * 50)
    
    # Check if we're on Windows
    if platform.system().lower() != 'windows':
        print("❌ This script is designed for Windows systems")
        print("   Use build_onnx_libs.py for Linux/macOS")
        return 1
    
    print("🎯 Target platform: Windows")
    
    # Check for available Windows builds
    if check_windows_builds():
        print("✅ Windows builds found in community repository")
        # TODO: Implement download logic if Windows builds become available
    else:
        print("❌ No Windows builds found in community repository")
    
    # Check alternative sources
    if not download_alternative_windows_build():
        print("❌ No alternative Windows builds found")
    
    # Provide instructions
    provide_windows_instructions()
    
    # Create placeholder directories
    create_placeholder_directories()
    
    print("\n📝 Next steps:")
    print("  1. Follow the instructions above to obtain Windows ONNX Runtime libraries")
    print("  2. Place the libraries in: source/onnxruntime-lib/win-amd64/")
    print("  3. Place the headers in: source/onnxruntime/")
    print("  4. Run 'python3 build.py --exe-only' to build Grapa")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())