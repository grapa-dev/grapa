#!/usr/bin/env python3
"""
Grapa Universal Release Manager

This script creates a single universal release package containing the entire bin directory
with the new universal installer and sample code build system.

Usage:
    python3 scripts/packaging/universal_release_manager.py --version <version> [--create] [--delete-current]
    python3 scripts/packaging/universal_release_manager.py --help

Examples:
    python3 scripts/packaging/universal_release_manager.py --version 0.1.53 --create
    python3 scripts/packaging/universal_release_manager.py --delete-current
"""

import os
import sys
import argparse
import subprocess
import shutil
import tempfile
import zipfile
from pathlib import Path
from typing import Optional

class UniversalReleaseManager:
    """Manages Grapa universal releases"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent.parent
        self.bin_dir = self.project_root / "bin"
        self.releases_dir = self.project_root / "releases"
        
    def delete_current_release(self) -> bool:
        """Delete the current release from GitHub"""
        print("🗑️  Deleting current release from GitHub...")
        
        try:
            # Get current release info
            result = subprocess.run(
                ["gh", "release", "list", "--limit", "1"],
                capture_output=True, text=True, check=True
            )
            
            if not result.stdout.strip():
                print("ℹ️  No releases found to delete")
                return True
            
            # Parse the first line to get tag name
            first_line = result.stdout.strip().split('\n')[0]
            tag_name = first_line.split('\t')[2]  # Tag is the 3rd column
            
            print(f"🗑️  Deleting release with tag: {tag_name}")
            
            # Delete the release
            subprocess.run(
                ["gh", "release", "delete", tag_name, "--yes"],
                check=True
            )
            
            # Delete the tag
            subprocess.run(
                ["git", "tag", "-d", tag_name],
                check=True
            )
            
            # Push tag deletion to remote
            subprocess.run(
                ["git", "push", "origin", ":refs/tags/" + tag_name],
                check=True
            )
            
            print(f"✅ Successfully deleted release: {tag_name}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"❌ Failed to delete release: {e}")
            return False
        except Exception as e:
            print(f"❌ Unexpected error: {e}")
            return False
    
    def create_universal_archive(self, version: str) -> Optional[Path]:
        """Create a single universal archive containing the entire bin directory"""
        print(f"📦 Creating universal archive for version {version}...")
        
        # Ensure releases directory exists
        self.releases_dir.mkdir(exist_ok=True)
        
        # Create archive name
        archive_name = f"grapa-{version}.zip"
        archive_path = self.releases_dir / archive_name
        
        try:
            # Create zip archive with proper folder structure
            with zipfile.ZipFile(archive_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
                # Add all files from bin directory to grapa-{version}/ folder
                for root, dirs, files in os.walk(self.bin_dir):
                    for file in files:
                        file_path = Path(root) / file
                        # Create relative path from bin directory
                        rel_path = file_path.relative_to(self.bin_dir)
                        # Add to archive with grapa-{version}/ prefix
                        archive_path_in_zip = f"grapa-{version}/{rel_path}"
                        zipf.write(file_path, archive_path_in_zip)
                        print(f"  📄 Added: {archive_path_in_zip}")
            
            # Get file size
            size_mb = archive_path.stat().st_size / (1024 * 1024)
            print(f"✅ Created universal archive: {archive_name} ({size_mb:.1f} MB)")
            return archive_path
            
        except Exception as e:
            print(f"❌ Failed to create universal archive: {e}")
            return None
    
    def create_release(self, version: str, archive: Path) -> bool:
        """Create a new GitHub release with the universal archive"""
        print(f"🚀 Creating GitHub release for version {version}...")
        
        try:
            # Create git tag with v-prefix
            tag_name = f"v{version}"
            subprocess.run(["git", "tag", tag_name], check=True)
            print(f"✅ Created git tag: {tag_name}")
            
            # Push tag to remote
            subprocess.run(["git", "push", "origin", tag_name], check=True)
            print(f"✅ Pushed tag to remote: {tag_name}")
            
            # Create release notes
            release_notes = self._generate_release_notes(version, archive)
            
            # Create release with universal archive
            cmd = [
                "gh", "release", "create", tag_name,
                "--title", f"Grapa {version}",
                "--notes", release_notes,
                str(archive)
            ]
            
            subprocess.run(cmd, check=True)
            
            print(f"🎉 Release {tag_name} created successfully!")
            print(f"📝 Release URL: https://github.com/grapa-dev/grapa/releases/tag/{tag_name}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"❌ Failed to create release: {e}")
            return False
        except Exception as e:
            print(f"❌ Unexpected error: {e}")
            return False
    
    def _generate_release_notes(self, version: str, archive: Path) -> str:
        """Generate release notes for the universal release"""
        size_mb = archive.stat().st_size / (1024 * 1024)
        
        notes = f"""## Grapa {version} Release

### 🎉 Universal Development Kit

This release introduces a **universal development kit** that provides everything you need to get started with Grapa development across all platforms.

### ✨ What's New

- **Universal Installer**: Single `install-grapa.py` script works on all platforms
- **Sample Code Build System**: Complete CMake-based build system with example code
- **Cross-Platform Support**: Windows, macOS, and Linux support in one package
- **Self-Contained**: All dependencies included - no external requirements
- **Modern Build System**: Standard `cmake ..` and `cmake --build .` workflow

### 📦 Package Contents

The `grapa-{version}.zip` file contains:
- **Universal installer** (`install-grapa.py`) for all platforms
- **Sample C++ application** (`main.cpp`) demonstrating Grapa embedding
- **CMake build system** (`CMakeLists.txt`) with cross-platform support
- **Complete API headers** in `include/grapa/`
- **Platform-specific libraries** for all supported platforms:
  - macOS ARM64 (Apple Silicon)
  - Windows AMD64
  - Linux AMD64 and ARM64
  - AWS AMD64 and ARM64
- **Third-party libraries** (FLTK, OpenSSL, BLST, PCRE2)
- **Legal documentation** for all included components

### 🚀 Quick Start

1. **Download**: Get `grapa-{version}.zip` from this release
2. **Extract**: Unzip to get `grapa-{version}/` folder
3. **Install**: Run `python3 install-grapa.py` (or `python install-grapa.py` on Windows)
4. **Build Example**: 
   ```bash
   cd grapa-{version}
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
5. **Run**: Execute the built example application

### 🛠️ Development Workflow

The universal development kit supports the standard CMake workflow:

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the example
./grapa_example  # On Unix/macOS
grapa_example.exe  # On Windows
```

### 📋 Supported Platforms

- **macOS ARM64** (Apple Silicon) - Native support
- **Windows AMD64** - Visual Studio and MinGW support
- **Linux AMD64** - GCC and Clang support
- **Linux ARM64** - Cross-compilation support
- **AWS AMD64/ARM64** - Cloud deployment ready

### 🔧 Technical Details

- **Archive Size**: {size_mb:.1f} MB
- **Build System**: CMake 3.10+ required
- **C++ Standard**: C++17
- **Dependencies**: All included (static linking)
- **Installation**: Python 3.6+ required for installer

### 📚 Documentation

- [Installation Guide](https://grapa-dev.github.io/grapa/installation/)
- [Getting Started](https://grapa-dev.github.io/grapa/getting_started/)
- [API Reference](https://grapa-dev.github.io/grapa/api_reference/)
- [GitHub Repository](https://github.com/grapa-dev/grapa)

### 🐛 Bug Reports & Support

- [GitHub Issues](https://github.com/grapa-dev/grapa/issues)
- [Documentation](https://grapa-dev.github.io/grapa/)

### 📄 License

This release includes components under various licenses. See the `legal/` directory for details.

---

**Download**: `grapa-{version}.zip` ({size_mb:.1f} MB)
"""
        
        return notes
    
    def list_releases(self):
        """List all releases"""
        print("📋 Current releases:")
        try:
            subprocess.run(["gh", "release", "list"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"❌ Failed to list releases: {e}")
    
    def cleanup_releases_dir(self):
        """Clean up the local releases directory"""
        if self.releases_dir.exists():
            print(f"🧹 Cleaning up releases directory: {self.releases_dir}")
            shutil.rmtree(self.releases_dir)
            print("✅ Releases directory cleaned up")

def main():
    parser = argparse.ArgumentParser(description="Grapa Universal Release Manager")
    parser.add_argument("--version", help="Version number for new release (e.g., 0.1.53)")
    parser.add_argument("--create", action="store_true", help="Create a new release")
    parser.add_argument("--delete-current", action="store_true", help="Delete the current release")
    parser.add_argument("--list", action="store_true", help="List all releases")
    parser.add_argument("--cleanup", action="store_true", help="Clean up local releases directory")
    
    args = parser.parse_args()
    
    if not any([args.create, args.delete_current, args.list, args.cleanup]):
        parser.print_help()
        return
    
    manager = UniversalReleaseManager()
    
    try:
        if args.delete_current:
            if not manager.delete_current_release():
                sys.exit(1)
        
        if args.list:
            manager.list_releases()
        
        if args.cleanup:
            manager.cleanup_releases_dir()
        
        if args.create:
            if not args.version:
                print("❌ --version is required when creating a release")
                sys.exit(1)
            
            # Create universal archive
            archive = manager.create_universal_archive(args.version)
            if not archive:
                print("❌ Failed to create universal archive")
                sys.exit(1)
            
            # Create release
            if not manager.create_release(args.version, archive):
                sys.exit(1)
        
        print("✅ Universal release management completed successfully")
        
    except KeyboardInterrupt:
        print("\n⚠️  Operation cancelled by user")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Release management failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
