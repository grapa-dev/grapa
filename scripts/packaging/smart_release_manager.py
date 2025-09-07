#!/usr/bin/env python3
"""
Grapa Smart Release Manager

This script creates platform-specific releases that are under GitHub's 100MB limit,
with a universal installer that automatically downloads the correct platform package.

Usage:
    python3 scripts/packaging/smart_release_manager.py --version <version> [--create] [--delete-current]
    python3 scripts/packaging/smart_release_manager.py --help

Examples:
    python3 scripts/packaging/smart_release_manager.py --version 0.1.53 --create
    python3 scripts/packaging/smart_release_manager.py --delete-current
"""

import os
import sys
import argparse
import subprocess
import shutil
import tempfile
import zipfile
import platform
from pathlib import Path
from typing import Optional, Dict, List

class SmartReleaseManager:
    """Manages Grapa platform-specific releases with universal installer"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent.parent
        self.bin_dir = self.project_root / "bin"
        self.releases_dir = self.project_root / "releases"
        
        # Platform mappings
        self.platforms = {
            "mac-arm64": "macOS ARM64",
            "mac-amd64": "macOS Intel", 
            "linux-amd64": "Linux AMD64",
            "linux-arm64": "Linux ARM64",
            "win-amd64": "Windows AMD64",
            "aws-amd64": "AWS AMD64",
            "aws-arm64": "AWS ARM64"
        }
        
        # Check prerequisites
        self._check_prerequisites()
    
    def _check_prerequisites(self):
        """Check if required tools are available"""
        print("🔍 Checking prerequisites...")
        
        # Check if bin directory exists
        if not self.bin_dir.exists():
            print("❌ Error: bin/ directory not found. Run build first.")
            sys.exit(1)
        
        # Check if GitHub CLI is available
        try:
            subprocess.run(["gh", "--version"], capture_output=True, check=True)
            print("✅ GitHub CLI found")
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("❌ Error: GitHub CLI (gh) not found. Install it first.")
            sys.exit(1)
        
        # Check if user is authenticated
        try:
            result = subprocess.run(["gh", "auth", "status"], capture_output=True, text=True)
            if "Logged in" not in result.stdout:
                print("❌ Error: Not authenticated with GitHub CLI. Run 'gh auth login' first.")
                sys.exit(1)
            print("✅ GitHub CLI authenticated")
        except subprocess.CalledProcessError:
            print("❌ Error: GitHub CLI authentication failed.")
            sys.exit(1)
        
        print("✅ All prerequisites met")
    
    def create_platform_packages(self, version: str) -> Dict[str, Path]:
        """Create platform-specific packages"""
        print(f"📦 Creating platform-specific packages for version {version}...")
        
        # Ensure releases directory exists
        self.releases_dir.mkdir(exist_ok=True)
        
        packages = {}
        
        for platform_key, platform_name in self.platforms.items():
            print(f"  📦 Creating {platform_name} package...")
            
            # Check if platform directory exists
            platform_dir = self.bin_dir / "platforms" / platform_key
            lib_dir = self.bin_dir / "lib" / platform_key
            
            if not platform_dir.exists() or not lib_dir.exists():
                print(f"    ⚠️  Skipping {platform_name} - platform files not found")
                continue
            
            # Create temporary directory for this platform
            with tempfile.TemporaryDirectory() as temp_dir:
                temp_path = Path(temp_dir)
                package_dir = temp_path / f"grapa-{version}"
                package_dir.mkdir()
                
                # Copy platform-specific files
                shutil.copytree(platform_dir, package_dir / "platforms" / platform_key)
                shutil.copytree(lib_dir, package_dir / "lib" / platform_key)
                
                # Copy universal files
                shutil.copy2(self.bin_dir / "install-grapa.py", package_dir)
                shutil.copy2(self.bin_dir / "CMakeLists.txt", package_dir)
                shutil.copy2(self.bin_dir / "main.cpp", package_dir)
                shutil.copy2(self.bin_dir / "README.md", package_dir)
                shutil.copytree(self.bin_dir / "include", package_dir / "include")
                shutil.copytree(self.bin_dir / "legal", package_dir / "legal")
                
                # Create platform-specific README
                self._create_platform_readme(package_dir, version, platform_name)
                
                # Create zip file
                zip_path = self.releases_dir / f"grapa-{version}-{platform_key}.zip"
                with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
                    for file_path in package_dir.rglob('*'):
                        if file_path.is_file():
                            arcname = file_path.relative_to(package_dir)
                            zipf.write(file_path, arcname)
                
                # Check file size
                size_mb = zip_path.stat().st_size / (1024 * 1024)
                print(f"    ✅ {platform_name}: {size_mb:.1f}MB")
                
                if size_mb > 100:
                    print(f"    ⚠️  Warning: {platform_name} package is {size_mb:.1f}MB (over 100MB limit)")
                
                packages[platform_key] = zip_path
        
        return packages
    
    def _create_platform_readme(self, package_dir: Path, version: str, platform_name: str):
        """Create platform-specific README"""
        readme_content = f"""# Grapa {version} - {platform_name}

This package contains Grapa for {platform_name}.

## Quick Start

1. **Install Grapa:**
   ```bash
   python3 install-grapa.py
   ```

2. **Build and run the example:**
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ./grapa_example
   ```

3. **Install Python extension:**
   ```bash
   pip install grapapy=={version}
   ```

## What's Included

- Grapa executable and libraries for {platform_name}
- Universal installer script
- Sample C++ application with CMake build system
- Headers and development files
- Legal documentation

## Universal Installer

The `install-grapa.py` script automatically:
- Detects your platform
- Installs Grapa to the correct location
- Sets up environment variables
- Creates symbolic links

## Support

For more information, visit: https://grapa.dev
"""
        
        readme_path = package_dir / "README.md"
        readme_path.write_text(readme_content)
    
    def create_universal_installer(self, version: str) -> Path:
        """Create a universal installer that downloads the correct platform package"""
        print(f"🔧 Creating universal installer for version {version}...")
        
        installer_content = f'''#!/usr/bin/env python3
"""
Grapa Universal Installer

This installer automatically detects your platform and downloads the appropriate
Grapa package from GitHub releases.

Usage:
    python3 install-grapa.py [--version <version>] [--install-dir <directory>]
"""

import os
import sys
import platform
import subprocess
import tempfile
import zipfile
import shutil
from pathlib import Path
from urllib.request import urlopen
import json

class GrapaInstaller:
    def __init__(self):
        self.version = "{version}"
        self.install_dir = Path.home() / ".local" / "grapa"
        self.repo = "grapa-dev/grapa"
        
    def detect_platform(self):
        """Detect the current platform"""
        system = platform.system().lower()
        machine = platform.machine().lower()
        
        if system == "darwin":  # macOS
            if machine in ["arm64", "aarch64"]:
                return "mac-arm64"
            else:
                return "mac-amd64"
        elif system == "linux":
            if machine in ["arm64", "aarch64"]:
                return "linux-arm64"
            else:
                return "linux-amd64"
        elif system == "windows":
            return "win-amd64"
        else:
            raise RuntimeError(f"Unsupported platform: {{system}} {{machine}}")
    
    def get_download_url(self, platform_key):
        """Get the download URL for the platform package"""
        # Try to get the latest release info
        try:
            url = f"https://api.github.com/repos/{{self.repo}}/releases/latest"
            with urlopen(url) as response:
                data = json.loads(response.read())
                
            # Find the asset for this platform
            for asset in data.get("assets", []):
                if f"grapa-{{self.version}}-{{platform_key}}.zip" in asset["name"]:
                    return asset["browser_download_url"]
        except Exception as e:
            print(f"Warning: Could not fetch release info: {{e}}")
        
        # Fallback URL
        return f"https://github.com/{{self.repo}}/releases/download/v{{self.version}}/grapa-{{self.version}}-{{platform_key}}.zip"
    
    def download_package(self, url, dest_path):
        """Download the platform package"""
        print(f"📥 Downloading from {{url}}...")
        
        with urlopen(url) as response:
            with open(dest_path, 'wb') as f:
                shutil.copyfileobj(response, f)
    
    def install_package(self, package_path):
        """Install the downloaded package"""
        print(f"📦 Installing to {{self.install_dir}}...")
        
        # Create install directory
        self.install_dir.mkdir(parents=True, exist_ok=True)
        
        # Extract package
        with zipfile.ZipFile(package_path, 'r') as zipf:
            zipf.extractall(self.install_dir)
        
        # Make executable
        grapa_exe = self.install_dir / "grapa-{version}" / "platforms" / self.detect_platform() / "grapa"
        if grapa_exe.exists():
            grapa_exe.chmod(0o755)
        
        print(f"✅ Grapa installed to {{self.install_dir}}")
        print(f"\\nTo use Grapa, add to your PATH:")
        print(f"export PATH=\\"{{self.install_dir}}/grapa-{version}/platforms/{{self.detect_platform()}}:$PATH\\"")
    
    def install(self):
        """Main installation process"""
        try:
            platform_key = self.detect_platform()
            print(f"🖥️  Detected platform: {{platform_key}}")
            
            # Download package
            url = self.get_download_url(platform_key)
            with tempfile.NamedTemporaryFile(suffix='.zip', delete=False) as temp_file:
                temp_path = Path(temp_file.name)
            
            self.download_package(url, temp_path)
            
            # Install package
            self.install_package(temp_path)
            
            # Cleanup
            temp_path.unlink()
            
        except Exception as e:
            print(f"❌ Installation failed: {{e}}")
            sys.exit(1)

if __name__ == "__main__":
    installer = GrapaInstaller()
    installer.install()
'''
        
        installer_path = self.releases_dir / f"install-grapa-{version}.py"
        installer_path.write_text(installer_content)
        installer_path.chmod(0o755)
        
        print(f"✅ Universal installer created: {installer_path}")
        return installer_path
    
    def create_release(self, version: str, packages: Dict[str, Path], installer: Path) -> bool:
        """Create a new GitHub release with platform packages and universal installer"""
        print(f"🚀 Creating GitHub release for version {version}...")
        
        try:
            # Create git tag with v-prefix
            tag_name = f"v{version}"
            print(f"🏷️  Creating tag {tag_name}...")
            
            subprocess.run(["git", "tag", "-a", tag_name, "-m", f"Release {version}"], check=True)
            subprocess.run(["git", "push", "origin", tag_name], check=True)
            
            # Create release notes
            release_notes = self._generate_release_notes(version, packages)
            
            # Create GitHub release
            print("📝 Creating GitHub release...")
            
            # Upload universal installer first
            subprocess.run([
                "gh", "release", "create", tag_name,
                "--title", f"Grapa {version}",
                "--notes", release_notes,
                str(installer)
            ], check=True)
            
            # Upload platform packages
            for platform_key, package_path in packages.items():
                print(f"📤 Uploading {platform_key} package...")
                subprocess.run([
                    "gh", "release", "upload", tag_name, str(package_path)
                ], check=True)
            
            print(f"✅ Release {version} created successfully!")
            print(f"🔗 View at: https://github.com/grapa-dev/grapa/releases/tag/{tag_name}")
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"❌ Error creating release: {e}")
            return False
    
    def _generate_release_notes(self, version: str, packages: Dict[str, Path]) -> str:
        """Generate release notes"""
        notes = f"""# Grapa {version} - Platform-Specific Release

This release provides platform-specific packages that are optimized for each platform and stay under GitHub's 100MB limit.

## 🚀 Quick Start

### Universal Installer (Recommended)
```bash
# Download and run the universal installer
curl -O https://github.com/grapa-dev/grapa/releases/download/v{version}/install-grapa-{version}.py
python3 install-grapa-{version}.py
```

### Manual Installation
Download the package for your platform and follow the included README.

## 📦 Available Packages

"""
        
        for platform_key, package_path in packages.items():
            platform_name = self.platforms[platform_key]
            size_mb = package_path.stat().st_size / (1024 * 1024)
            notes += f"- **{platform_name}** (`grapa-{version}-{platform_key}.zip`) - {size_mb:.1f}MB\\n"
        
        notes += f"""
## 🔧 What's New

- **Platform-Specific Packages**: Optimized packages for each platform
- **Universal Installer**: Automatically detects platform and downloads correct package
- **Under 100MB**: Each package stays well under GitHub's file size limit
- **Complete Development Kit**: Includes sample code, CMake build system, and universal installer

## 📋 Installation

1. **Run the universal installer** (recommended):
   ```bash
   python3 install-grapa-{version}.py
   ```

2. **Or download manually** for your platform and follow the included README.

## 🐍 Python Integration

```bash
pip install grapapy=={version}
```

## 📚 Documentation

- [Installation Guide](https://grapa.dev/installation/)
- [Getting Started](https://grapa.dev/getting_started/)
- [C++ Integration](https://grapa.dev/cpp_integration/)
- [Python Integration](https://grapa.dev/python_integration/)

## 🔗 Links

- **Website**: https://grapa.dev
- **Documentation**: https://grapa.dev/docs/
- **GitHub**: https://github.com/grapa-dev/grapa
"""
        
        return notes
    
    def delete_current_release(self) -> bool:
        """Delete the current release"""
        try:
            # Get current release
            result = subprocess.run(["gh", "release", "list", "--limit", "1"], 
                                  capture_output=True, text=True, check=True)
            
            if not result.stdout.strip():
                print("❌ No releases found")
                return False
            
            # Extract release ID
            release_line = result.stdout.split('\\n')[0]
            release_id = release_line.split('\\t')[0]
            
            print(f"🗑️  Deleting release {release_id}...")
            subprocess.run(["gh", "release", "delete", release_id, "--yes"], check=True)
            
            print("✅ Release deleted successfully")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"❌ Error deleting release: {e}")
            return False
    
    def list_releases(self):
        """List all releases"""
        try:
            subprocess.run(["gh", "release", "list"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"❌ Error listing releases: {e}")
    
    def cleanup(self):
        """Clean up local release files"""
        if self.releases_dir.exists():
            print(f"🧹 Cleaning up {self.releases_dir}...")
            shutil.rmtree(self.releases_dir)
            print("✅ Cleanup complete")
        else:
            print("ℹ️  No release files to clean up")

def main():
    parser = argparse.ArgumentParser(description="Grapa Smart Release Manager")
    parser.add_argument("--version", help="Version number (e.g., 0.1.53)")
    parser.add_argument("--create", action="store_true", help="Create a new release")
    parser.add_argument("--delete-current", action="store_true", help="Delete the current release")
    parser.add_argument("--list", action="store_true", help="List all releases")
    parser.add_argument("--cleanup", action="store_true", help="Clean up local release files")
    
    args = parser.parse_args()
    
    manager = SmartReleaseManager()
    
    if args.create:
        if not args.version:
            print("❌ Error: --version is required when creating a release")
            sys.exit(1)
        
        print(f"🚀 Creating smart release for version {args.version}...")
        
        # Create platform packages
        packages = manager.create_platform_packages(args.version)
        
        if not packages:
            print("❌ No platform packages created")
            sys.exit(1)
        
        # Create universal installer
        installer = manager.create_universal_installer(args.version)
        
        # Create release
        success = manager.create_release(args.version, packages, installer)
        
        if success:
            print(f"\\n🎉 Smart release {args.version} created successfully!")
            print("\\n📋 Next steps:")
            print("1. Test the universal installer on different platforms")
            print("2. Update documentation to reference the new installer")
            print("3. Announce the release")
        else:
            print("❌ Release creation failed")
            sys.exit(1)
    
    elif args.delete_current:
        manager.delete_current_release()
    
    elif args.list:
        manager.list_releases()
    
    elif args.cleanup:
        manager.cleanup()
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
