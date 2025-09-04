#!/usr/bin/env python3
"""
Universal Grapa Installation Script
Supports Windows, macOS, and Linux with automatic platform detection
"""

import os
import sys
import shutil
import platform
import subprocess
import argparse
from pathlib import Path

class GrapaInstaller:
    def __init__(self):
        self.script_dir = Path(__file__).parent.absolute()
        self.platform = self._detect_platform()
        self.install_path = self._get_default_install_path()
        
    def _detect_platform(self):
        """Detect the current platform and architecture - using same logic as build.py"""
        system = platform.system().lower()
        machine = platform.machine().lower()
        
        if system == "windows":
            return "win-amd64"
        elif system == "darwin":
            # On Mac, only support Apple Silicon (arm64)
            if machine == "arm64":
                return "mac-arm64"
            else:
                raise RuntimeError(f"mac-amd64 is no longer supported. Please use an Apple Silicon Mac (arm64). Current architecture: {machine}")
        elif system == "linux":
            # Check if this is AWS Linux by looking for Amazon Linux specific files
            if (os.path.exists("/etc/system-release") and 
                ("Amazon Linux" in open("/etc/system-release").read())):
                return "aws-arm64" if machine == "aarch64" else "aws-amd64"
            else:
                return "linux-arm64" if machine == "aarch64" else "linux-amd64"
        else:
            raise RuntimeError(f"Unsupported platform: {system}")
    
    def _get_default_install_path(self):
        """Get the default installation path for the current platform"""
        if platform.system().lower() == "windows":
            return Path("C:\\Program Files\\Grapa")
        else:
            return Path("/usr/local")
    
    def _get_platform_files(self):
        """Get the files needed for the current platform"""
        platform_dir = self.script_dir / self.platform
        
        if not platform_dir.exists():
            raise FileNotFoundError(f"Platform directory {self.platform} not found")
        
        files = {
            'executable': None,
            'static_lib': None,
            'shared_lib': None
        }
        
        # Find executable
        if platform.system().lower() == "windows":
            exe_path = platform_dir / "grapa.exe"
            if exe_path.exists():
                files['executable'] = exe_path
        else:
            exe_path = platform_dir / "grapa"
            if exe_path.exists():
                files['executable'] = exe_path
        
        # Find static library (optional)
        if platform.system().lower() == "windows":
            static_path = platform_dir / "grapa_static.lib"
        else:
            static_path = platform_dir / "libgrapa_static.a"
        
        if static_path.exists():
            files['static_lib'] = static_path
        
        # Find shared library
        if platform.system().lower() == "windows":
            shared_path = platform_dir / "grapa.dll"
        elif platform.system().lower() == "darwin":
            shared_path = platform_dir / "libgrapa.dylib"
        else:
            shared_path = platform_dir / "libgrapa.so"
        
        if shared_path.exists():
            files['shared_lib'] = shared_path
        
        return files
    
    def _check_requirements(self):
        """Check if all required files exist"""
        files = self._get_platform_files()
        missing = []
        
        if not files['executable']:
            missing.append("executable (grapa.exe/grapa)")
        if not files['shared_lib']:
            missing.append("shared library")
        
        if missing:
            raise FileNotFoundError(f"Missing required files: {', '.join(missing)}")
        
        return files
    
    def _install_windows(self, files, force=False):
        """Install on Windows"""
        bin_dir = self.install_path / "bin"
        lib_dir = self.install_path / "lib"
        
        # Create directories
        bin_dir.mkdir(parents=True, exist_ok=True)
        lib_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy files
        shutil.copy2(files['executable'], bin_dir / "grapa.exe")
        
        if files['static_lib']:
            shutil.copy2(files['static_lib'], lib_dir / "grapa_static.lib")
        
        if files['shared_lib']:
            shutil.copy2(files['shared_lib'], lib_dir / "grapa.dll")
        
        # Add to PATH
        self._add_to_windows_path(str(bin_dir))
        
        return {
            'executable': str(bin_dir / "grapa.exe"),
            'static_lib': str(lib_dir / "grapa_static.lib") if files['static_lib'] else None,
            'shared_lib': str(lib_dir / "grapa.dll") if files['shared_lib'] else None
        }
    
    def _install_unix(self, files, force=False):
        """Install on Unix-like systems (macOS/Linux)"""
        bin_dir = self.install_path / "bin"
        lib_dir = self.install_path / "lib"
        
        # Create directories
        bin_dir.mkdir(parents=True, exist_ok=True)
        lib_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy files
        shutil.copy2(files['executable'], bin_dir / "grapa")
        
        if files['static_lib']:
            shutil.copy2(files['static_lib'], lib_dir / "libgrapa_static.a")
        
        if files['shared_lib']:
            shutil.copy2(files['shared_lib'], lib_dir / files['shared_lib'].name)
        
        # Set permissions
        (bin_dir / "grapa").chmod(0o755)
        
        # Add to PATH
        self._add_to_unix_path(str(bin_dir))
        
        return {
            'executable': str(bin_dir / "grapa"),
            'static_lib': str(lib_dir / "libgrapa_static.a") if files['static_lib'] else None,
            'shared_lib': str(lib_dir / files['shared_lib'].name) if files['shared_lib'] else None
        }
    
    def _add_to_windows_path(self, bin_path):
        """Add Grapa to Windows PATH"""
        try:
            # Use PowerShell to modify PATH
            cmd = [
                'powershell', '-Command',
                f'$currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine"); '
                f'if ($currentPath -notlike "*{bin_path}*") {{ '
                f'[Environment]::SetEnvironmentVariable("PATH", "$currentPath;{bin_path}", "Machine") }}'
            ]
            subprocess.run(cmd, check=True, capture_output=True)
            print(f"✅ Added Grapa to system PATH")
        except subprocess.CalledProcessError:
            print("⚠️  Could not automatically update PATH")
            print(f"Please manually add {bin_path} to your system PATH")
    
    def _add_to_unix_path(self, bin_path):
        """Add Grapa to Unix PATH"""
        # Try to add to shell profile files
        profile_files = [
            Path.home() / ".zshrc",
            Path.home() / ".bash_profile", 
            Path.home() / ".bashrc"
        ]
        
        for profile in profile_files:
            if profile.exists():
                # Check if already added
                content = profile.read_text()
                if bin_path not in content:
                    with profile.open('a') as f:
                        f.write(f"\n# Grapa installation\nexport PATH=\"$PATH:{bin_path}\"\n")
                    print(f"✅ Added Grapa to PATH in {profile}")
                    return
        
        print("⚠️  Could not automatically add to shell profile")
        print(f"Please manually add the following line to your shell profile:")
        print(f"export PATH=\"$PATH:{bin_path}\"")
    
    def install(self, force=False):
        """Install Grapa"""
        print(f"🚀 Installing Grapa for {self.platform}...")
        
        try:
            files = self._check_requirements()
            print("✅ All required files found")
            
            if platform.system().lower() == "windows":
                result = self._install_windows(files, force)
            else:
                result = self._install_unix(files, force)
            
            print("\n🎉 Installation complete!")
            print(f"\nInstallation details:")
            print(f"  Executable: {result['executable']}")
            if result['static_lib']:
                print(f"  Static Library: {result['static_lib']}")
            else:
                print(f"  Static Library: Not available (shared library only)")
            if result['shared_lib']:
                print(f"  Shared Library: {result['shared_lib']}")
            
            print(f"\nNext steps:")
            if platform.system().lower() == "windows":
                print("  1. Restart your terminal for PATH changes to take effect")
                print("  2. Verify installation: grapa --version")
                print("  3. Test functionality: grapa -c '2+2'")
            else:
                print("  1. Restart your terminal or run: source ~/.zshrc (or ~/.bashrc)")
                print("  2. Verify installation: grapa --version")
                print("  3. Test functionality: grapa -c '2+2'")
            
            return True
            
        except Exception as e:
            print(f"❌ Installation failed: {e}")
            return False
    
    def uninstall(self):
        """Uninstall Grapa"""
        print(f"🗑️  Uninstalling Grapa...")
        
        try:
            if platform.system().lower() == "windows":
                bin_dir = self.install_path / "bin"
                lib_dir = self.install_path / "lib"
                
                # Remove from PATH
                try:
                    cmd = [
                        'powershell', '-Command',
                        f'$currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine"); '
                        f'$newPath = ($currentPath -split \';\' | Where-Object {{ $_ -ne \'{bin_dir}\' }}) -join \';\'; '
                        f'[Environment]::SetEnvironmentVariable("PATH", $newPath, "Machine")'
                    ]
                    subprocess.run(cmd, check=True, capture_output=True)
                    print("✅ Removed Grapa from system PATH")
                except subprocess.CalledProcessError:
                    print("⚠️  Could not automatically remove from PATH")
                
            else:
                bin_dir = self.install_path / "bin"
                lib_dir = self.install_path / "lib"
                
                # Remove from shell profiles
                profile_files = [
                    Path.home() / ".zshrc",
                    Path.home() / ".bash_profile",
                    Path.home() / ".bashrc"
                ]
                
                for profile in profile_files:
                    if profile.exists():
                        content = profile.read_text()
                        lines = content.split('\n')
                        new_lines = [line for line in lines if 'export PATH' not in line or 'grapa' not in line]
                        if len(new_lines) != len(lines):
                            profile.write_text('\n'.join(new_lines))
                            print(f"✅ Removed Grapa from {profile}")
            
            # Remove installation directory
            if self.install_path.exists():
                shutil.rmtree(self.install_path)
                print(f"✅ Removed installation directory: {self.install_path}")
            
            print("✅ Uninstallation complete!")
            return True
            
        except Exception as e:
            print(f"❌ Uninstallation failed: {e}")
            return False

def main():
    parser = argparse.ArgumentParser(description="Universal Grapa Installer")
    parser.add_argument('--force', action='store_true', help='Force installation without prompts')
    parser.add_argument('--uninstall', action='store_true', help='Uninstall Grapa')
    parser.add_argument('--install-path', help='Custom installation path')
    
    args = parser.parse_args()
    
    installer = GrapaInstaller()
    
    if args.install_path:
        installer.install_path = Path(args.install_path)
    
    if args.uninstall:
        installer.uninstall()
    else:
        installer.install(force=args.force)

if __name__ == "__main__":
    main()
