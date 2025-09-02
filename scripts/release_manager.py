#!/usr/bin/env python3
"""
Grapa Release Manager

This script manages the creation and upload of Grapa releases to GitHub.
It compresses the bin directory contents and creates a new release.

Usage:
    python3 scripts/release_manager.py --version <version> [--create] [--delete-current]
    python3 scripts/release_manager.py --help

Examples:
    python3 scripts/release_manager.py --version 0.1.52 --create
    python3 scripts/release_manager.py --delete-current
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
        self.project_root = Path(__file__).parent.parent
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
            
            # Create archive name
            archive_name = f"grapa-{version}-{platform_name}.tar.gz"
            archive_path = self.releases_dir / archive_name
            
            # Create tar.gz archive
            try:
                subprocess.run([
                    "tar", "-czf", str(archive_path), "-C", str(self.bin_dir), platform_name
                ], check=True)
                
                # Get file size
                size_mb = archive_path.stat().st_size / (1024 * 1024)
                print(f"    ✅ Created: {archive_name} ({size_mb:.1f} MB)")
                archives.append(archive_path)
                
            except subprocess.CalledProcessError as e:
                print(f"    ❌ Failed to create archive for {platform_name}: {e}")
                continue
        
        return archives
    
    def create_release(self, version: str, archives: List[Path]) -> bool:
        """Create a new GitHub release with the archives"""
        print(f"🚀 Creating GitHub release for version {version}...")
        
        try:
            # Create git tag
            subprocess.run(["git", "tag", version], check=True)
            print(f"✅ Created git tag: {version}")
            
            # Push tag to remote
            subprocess.run(["git", "push", "origin", version], check=True)
            print(f"✅ Pushed tag to remote: {version}")
            
            # Create release notes
            release_notes = self._generate_release_notes(version, archives)
            
            # Create release
            subprocess.run([
                "gh", "release", "create", version,
                "--title", f"Grapa {version}",
                "--notes", release_notes,
                "--draft"
            ], check=True)
            
            print(f"✅ Created draft release: {version}")
            
            # Upload assets
            for archive in archives:
                print(f"📤 Uploading {archive.name}...")
                subprocess.run([
                    "gh", "release", "upload", version, str(archive)
                ], check=True)
                print(f"✅ Uploaded: {archive.name}")
            
            print(f"🎉 Release {version} created successfully!")
            print("📝 Review the draft release and publish when ready")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"❌ Failed to create release: {e}")
            return False
        except Exception as e:
            print(f"❌ Unexpected error: {e}")
            return False
    
    def _generate_release_notes(self, version: str, archives: List[Path]) -> str:
        """Generate release notes"""
        notes = f"""# Grapa {version}

## Downloads

"""
        
        for archive in archives:
            platform_name = archive.stem.replace(f"grapa-{version}-", "")
            size_mb = archive.stat().st_size / (1024 * 1024)
            notes += f"- **{platform_name}**: [{archive.name}]({archive.name}) ({size_mb:.1f} MB)\n"
        
        notes += f"""

## Installation

Each platform archive contains:
- Grapa executable
- Static library (with `_static` suffix)
- Shared library (where applicable)
- Platform-specific install script

### Windows
Run `install-grapa.ps1` as Administrator

### Unix/Linux/macOS
Run `sudo ./install-grapa.sh`

## What's New

- Updated library naming convention: static libraries now use `_static` suffix
- Improved cross-platform build system
- Enhanced install scripts for all platforms
- Better organization of binary distributions

## Build Information

This release was built using the updated build system with:
- Static libraries: `*_static.*` naming convention
- Platform-specific directories in `bin/`
- Automated install script generation
- Improved library management
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
    parser = argparse.ArgumentParser(description="Grapa Release Manager")
    parser.add_argument("--version", help="Version number for new release (e.g., 0.1.52)")
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
