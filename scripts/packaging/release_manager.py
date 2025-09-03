#!/usr/bin/env python3
"""
Grapa Release Manager

This script manages the creation and upload of Grapa releases to GitHub.
It compresses the bin directory contents and creates a new release.

Usage:
    python3 scripts/packaging/release_manager.py --version <version> [--create] [--delete-current]
    python3 scripts/packaging/release_manager.py --help

Examples:
    python3 scripts/packaging/release_manager.py --version 0.1.53 --create
    python3 scripts/packaging/release_manager.py --delete-current
"""

import os
import sys
import argparse
import subprocess
import shutil
import tempfile
from pathlib import Path
from typing import List, Optional

class ReleaseManager:
    """Manages Grapa releases"""
    
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
    
    def create_platform_archives(self, version: str) -> List[Path]:
        """Create compressed archives for each platform"""
        print(f"📦 Creating platform archives for version {version}...")
        
        archives = []
        
        # Ensure releases directory exists
        self.releases_dir.mkdir(exist_ok=True)
        
        for platform_dir in self.bin_dir.iterdir():
            if not platform_dir.is_dir():
                continue
                
            platform_name = platform_dir.name
            print(f"  📁 Processing platform: {platform_name}")
            
            # Create archive name - use .zip format to match create_github_release.sh
            archive_name = f"grapa-{version}-{platform_name}.zip"
            archive_path = self.releases_dir / archive_name
            
            # Create zip archive
            try:
                # Use Python's built-in compression for cross-platform compatibility
                import tempfile
                import shutil
                
                # Create a temporary directory for the archive
                with tempfile.TemporaryDirectory() as temp_dir:
                    # Copy platform directory to temp location
                    platform_temp_dir = Path(temp_dir) / platform_name
                    shutil.copytree(platform_dir, platform_temp_dir)
                    
                    # Create zip archive
                    shutil.make_archive(str(archive_path.with_suffix('')), 'zip', temp_dir, platform_name)
                    
                    # Get file size
                    size_mb = archive_path.stat().st_size / (1024 * 1024)
                    print(f"    ✅ Created: {archive_name} ({size_mb:.1f} MB)")
                    archives.append(archive_path)
                
            except Exception as e:
                print(f"    ❌ Failed to create archive for {platform_name}: {e}")
                continue
        
        return archives
    
    def create_release(self, version: str, archives: List[Path]) -> bool:
        """Create a new GitHub release with the archives"""
        print(f"🚀 Creating GitHub release for version {version}...")
        
        try:
            # Create git tag with v-prefix to match create_github_release.sh
            tag_name = f"v{version}"
            subprocess.run(["git", "tag", tag_name], check=True)
            print(f"✅ Created git tag: {tag_name}")
            
            # Push tag to remote
            subprocess.run(["git", "push", "origin", tag_name], check=True)
            print(f"✅ Pushed tag to remote: {tag_name}")
            
            # Create release notes matching create_github_release.sh format
            release_notes = self._generate_release_notes(version, archives)
            
            # Create release with all assets in single command (like create_github_release.sh)
            # This creates a published release, not a draft
            cmd = [
                "gh", "release", "create", tag_name,
                "--title", f"Grapa {version}",
                "--notes", release_notes
            ]
            
            # Add all archive files as assets
            for archive in archives:
                cmd.append(str(archive))
            
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
    
    def _generate_release_notes(self, version: str, archives: List[Path]) -> str:
        """Generate release notes matching create_github_release.sh format"""
        notes = f"""## Grapa {version} Release

### Features
- Modern, high-performance programming language
- Unlimited precision arithmetic
- Comprehensive cryptography with OpenSSL 3.5.2
- Advanced vector and matrix operations
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy
- Improved build system with static library support

### Supported Platforms
"""
        
        # Add platform information
        for archive in archives:
            platform_name = archive.stem.replace(f"grapa-{version}-", "")
            size_mb = archive.stat().st_size / (1024 * 1024)
            
            # Map platform names to display names
            display_names = {
                "mac-arm64": "**macOS ARM64** (Apple Silicon)",
                "win-amd64": "**Windows AMD64**",
                "linux-amd64": "**Linux AMD64**",
                "linux-arm64": "**Linux ARM64**",
                "aws-amd64": "**AWS AMD64**",
                "aws-arm64": "**AWS ARM64**"
            }
            
            display_name = display_names.get(platform_name, f"**{platform_name}**")
            notes += f"- {display_name}: `{archive.name}`\n"
        
        notes += f"""
## Installation
Each package contains:
- Grapa executable and libraries
- Automated install script for your platform
- Platform-specific documentation

**Quick Start:**
1. Download the appropriate `.zip` file for your platform
2. Extract the archive
3. Run the included install script
4. Start using Grapa!

**Python users**: Install GrapaPy with `pip install grapapy`

### Documentation
- [GitHub Repository](https://github.com/grapa-dev/grapa)
- [Documentation](https://grapa-dev.github.io/grapa/)
- [Installation Guide](https://grapa-dev.github.io/grapa/installation/)

### Changes in this Release
- Version bump to {version}
- Improved build system with static library support
- Enhanced Python integration
- AWS platform support (AMD64 and ARM64)
- Cross-platform compatibility improvements
- Better error handling and build reliability
- Automated install scripts for all platforms

### SHA256 Checksums
```
"""
        
        # Add SHA256 checksums
        for archive in archives:
            try:
                result = subprocess.run(
                    ["shasum", "-a", "256", str(archive)],
                    capture_output=True, text=True, check=True
                )
                checksum_line = result.stdout.strip()
                # Extract just the filename part
                filename = archive.name
                checksum = checksum_line.split()[0]
                notes += f"{checksum}  {filename}\n"
            except subprocess.CalledProcessError:
                # Fallback for Windows or if shasum not available
                notes += f"<checksum>  {archive.name}\n"
        
        notes += "```"
        
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
    parser = argparse.ArgumentParser(description="Grapa Release Manager")
    parser.add_argument("--version", help="Version number for new release (e.g., 0.1.53)")
    parser.add_argument("--create", action="store_true", help="Create a new release")
    parser.add_argument("--delete-current", action="store_true", help="Delete the current release")
    parser.add_argument("--list", action="store_true", help="List all releases")
    parser.add_argument("--cleanup", action="store_true", help="Clean up local releases directory")
    
    args = parser.parse_args()
    
    if not any([args.create, args.delete_current, args.list, args.cleanup]):
        parser.print_help()
        return
    
    manager = ReleaseManager()
    
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
            
            # Create platform archives
            archives = manager.create_platform_archives(args.version)
            if not archives:
                print("❌ No archives created")
                sys.exit(1)
            
            # Create release
            if not manager.create_release(args.version, archives):
                sys.exit(1)
        
        print("✅ Release management completed successfully")
        
    except KeyboardInterrupt:
        print("\n⚠️  Operation cancelled by user")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Release management failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
