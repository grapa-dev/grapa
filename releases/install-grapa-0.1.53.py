#!/usr/bin/env python3
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
        self.version = "0.1.53"
        self.install_dir = Path.home() / ".local" / "grapa"
        self.repo = "matichuk/grapa"
        
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
            raise RuntimeError(f"Unsupported platform: {system} {machine}")
    
    def get_download_url(self, platform_key):
        """Get the download URL for the platform package"""
        # Try to get the latest release info
        try:
            url = f"https://api.github.com/repos/{self.repo}/releases/latest"
            with urlopen(url) as response:
                data = json.loads(response.read())
                
            # Find the asset for this platform
            for asset in data.get("assets", []):
                if f"grapa-{self.version}-{platform_key}.zip" in asset["name"]:
                    return asset["browser_download_url"]
        except Exception as e:
            print(f"Warning: Could not fetch release info: {e}")
        
        # Fallback URL
        return f"https://github.com/{self.repo}/releases/download/v{self.version}/grapa-{self.version}-{platform_key}.zip"
    
    def download_package(self, url, dest_path):
        """Download the platform package"""
        print(f"📥 Downloading from {url}...")
        
        with urlopen(url) as response:
            with open(dest_path, 'wb') as f:
                shutil.copyfileobj(response, f)
    
    def install_package(self, package_path):
        """Install the downloaded package"""
        print(f"📦 Installing to {self.install_dir}...")
        
        # Create install directory
        self.install_dir.mkdir(parents=True, exist_ok=True)
        
        # Extract package
        with zipfile.ZipFile(package_path, 'r') as zipf:
            zipf.extractall(self.install_dir)
        
        # Make executable
        grapa_exe = self.install_dir / "grapa-0.1.53" / "platforms" / self.detect_platform() / "grapa"
        if grapa_exe.exists():
            grapa_exe.chmod(0o755)
        
        print(f"✅ Grapa installed to {self.install_dir}")
        print(f"\nTo use Grapa, add to your PATH:")
        print(f"export PATH="{self.install_dir}/grapa-0.1.53/platforms/{self.detect_platform()}:$PATH"")
    
    def install(self):
        """Main installation process"""
        try:
            platform_key = self.detect_platform()
            print(f"🖥️  Detected platform: {platform_key}")
            
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
            print(f"❌ Installation failed: {e}")
            sys.exit(1)

if __name__ == "__main__":
    installer = GrapaInstaller()
    installer.install()
