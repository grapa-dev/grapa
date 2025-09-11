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
        platform_dir = self.script_dir / "platforms" / self.platform
        
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
        
        # Find static library (required)
        if platform.system().lower() == "windows":
            static_path = platform_dir / "grapa_static.lib"
        else:
            static_path = platform_dir / "libgrapa_static.a"
        
        if static_path.exists():
            files['static_lib'] = static_path
        else:
            raise FileNotFoundError(f"Static library not found: {static_path}")
        
        # Shared libraries are no longer supported
        files['shared_lib'] = None
        
        return files
    
    def _detect_ubuntu_version(self):
        """Detect Ubuntu version and return as tuple (major, minor) or None"""
        try:
            with open("/etc/os-release", "r") as f:
                content = f.read()
                for line in content.split('\n'):
                    if line.startswith("VERSION_ID="):
                        version_str = line.split('=')[1].strip('"')
                        # Parse version like "22.04" or "24.04"
                        parts = version_str.split('.')
                        if len(parts) >= 2:
                            return (int(parts[0]), int(parts[1]))
        except (FileNotFoundError, ValueError, IndexError):
            pass
        return None

    def _check_system_dependencies(self, auto_install=False):
        """Check if required system dependencies are available"""
        missing_deps = []
        
        if platform.system().lower() == "linux":
            # Check for required libraries
            try:
                import ctypes
                # Check for common system libraries that Grapa depends on
                libs_to_check = [
                    'libssl.so.1.1', 'libssl.so.3', 'libssl.so',  # OpenSSL
                    'libcrypto.so.1.1', 'libcrypto.so.3', 'libcrypto.so',  # OpenSSL crypto
                    'libX11.so.6', 'libX11.so',  # X11
                    'libXext.so.6', 'libXext.so',  # X11 extensions
                ]
                
                for lib in libs_to_check:
                    try:
                        ctypes.CDLL(lib)
                    except OSError:
                        # Try alternative names
                        if 'libssl' in lib:
                            try:
                                ctypes.CDLL('libssl.so.1.0.0')
                            except OSError:
                                missing_deps.append('OpenSSL (libssl)')
                        elif 'libcrypto' in lib:
                            try:
                                ctypes.CDLL('libcrypto.so.1.0.0')
                            except OSError:
                                missing_deps.append('OpenSSL (libcrypto)')
                        elif 'libX11' in lib:
                            missing_deps.append('X11 development libraries')
                        elif 'libXext' in lib:
                            missing_deps.append('X11 extensions')
                        
            except ImportError:
                # ctypes not available, skip library checking
                pass
                
        elif platform.system().lower() == "darwin":
            # macOS - check for Xcode command line tools
            try:
                result = subprocess.run(['xcode-select', '--print-path'], 
                                      capture_output=True, text=True, check=True)
                if not result.stdout.strip():
                    missing_deps.append('Xcode Command Line Tools')
            except (subprocess.CalledProcessError, FileNotFoundError):
                missing_deps.append('Xcode Command Line Tools')
                
        elif platform.system().lower() == "windows":
            # Windows - check for Visual Studio build tools and dependencies
            try:
                # Check for MSBuild (Visual Studio 2022)
                result = subprocess.run(['msbuild', '/version'], capture_output=True, text=True, check=True)
                if '17.' not in result.stdout:
                    missing_deps.append('Visual Studio 2022 or Build Tools for Visual Studio 2022 (version 17.x required)')
            except (subprocess.CalledProcessError, FileNotFoundError):
                missing_deps.append('Visual Studio 2022 or Build Tools for Visual Studio 2022')
            
            # Note: 7-Zip is not required for installing pre-built Grapa
            # It's only needed for building/developing Grapa from source
            
            # Check for Python 3
            if sys.version_info < (3, 6):
                missing_deps.append('Python 3.6 or higher')
            
            # Check for Windows SDK
            try:
                import winreg
                sdk_keys = [
                    r"SOFTWARE\Microsoft\Windows Kits\Installed Roots",
                    r"SOFTWARE\WOW6432Node\Microsoft\Windows Kits\Installed Roots"
                ]
                found_sdk = False
                for key_path in sdk_keys:
                    try:
                        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, key_path)
                        winreg.CloseKey(key)
                        found_sdk = True
                        break
                    except FileNotFoundError:
                        continue
                if not found_sdk:
                    missing_deps.append('Windows 10/11 SDK (usually included with Visual Studio 2022)')
            except ImportError:
                # winreg not available, skip SDK check
                pass
        
        if missing_deps:
            print("⚠️  Missing system dependencies detected:")
            for dep in set(missing_deps):  # Remove duplicates
                print(f"   - {dep}")
            
            if auto_install:
                print("\n🔧 Attempting to install dependencies automatically...")
                try:
                    self._install_system_dependencies()
                    print("✅ Dependencies installed successfully, continuing with installation...")
                    return  # Dependencies installed, continue
                except Exception as e:
                    print(f"❌ Automatic dependency installation failed: {e}")
                    print("Please install dependencies manually or use --force to continue anyway.")
                    raise RuntimeError("Dependency installation failed")
            else:
                print("\nPlease install the missing dependencies before continuing.")
                print("You can:")
                print("  1. Install manually (see instructions below)")
                print("  2. Use --install-dependencies to install automatically")
                print("  3. Use --force to continue anyway (not recommended)")
                print("\nManual installation instructions:")
                self._print_manual_install_instructions()
                
                response = input("\nContinue installation anyway? (y/N): ").strip().lower()
                if response not in ['y', 'yes']:
                    raise RuntimeError("Installation cancelled due to missing dependencies")
                else:
                    print("⚠️  Continuing installation - Grapa may not work properly without these dependencies")
    
    def _install_system_dependencies(self):
        """Install system dependencies automatically"""
        print("🔧 Installing system dependencies...")
        
        if platform.system().lower() == "linux":
            # Detect package manager and install dependencies
            if shutil.which("apt"):
                # Ubuntu/Debian - detect version for appropriate dependencies
                ubuntu_version = self._detect_ubuntu_version()
                print(f"Detected Ubuntu version: {ubuntu_version}")
                
                packages = ["build-essential", "python3-dev", "libssl-dev", "libx11-dev", "libxext-dev", "cmake", 
                           "libxfixes-dev", "libxft-dev", "libxrender-dev", "libxinerama-dev", 
                           "libxcursor-dev", "libpng-dev", "libfontconfig1-dev", "libfreetype6-dev",
                           "x11-apps", "gdebi-core"]
                
                # Ubuntu 24.04+ requires --break-system-packages for pip installs
                if ubuntu_version and ubuntu_version >= (24, 4):
                    print("⚠️  Ubuntu 24.04+ detected: pip installs may require --break-system-packages flag")
                # Check if we're running as root (e.g., in Docker)
                if os.geteuid() == 0:
                    cmd = ["apt-get", "update", "&&", "apt-get", "install", "-y"] + packages
                else:
                    cmd = ["sudo", "apt-get", "update", "&&", "sudo", "apt-get", "install", "-y"] + packages
                print(f"Installing packages: {' '.join(packages)}")
                result = subprocess.run(" ".join(cmd), shell=True, capture_output=True, text=True)
                
            elif shutil.which("yum"):
                # Amazon Linux/RHEL/CentOS
                if os.geteuid() == 0:
                    cmd1 = ["yum", "groupinstall", "-y", "'Development Tools'"]
                    cmd2 = ["yum", "install", "-y", "gcc-c++", "python3-devel", "openssl-devel", "libX11-devel", "libXext-devel", "cmake",
                           "libXfixes-devel", "libXft-devel", "libXrender-devel", "libXinerama-devel",
                           "libXcursor-devel", "libpng-devel", "fontconfig-devel", "freetype-devel"]
                else:
                    cmd1 = ["sudo", "yum", "groupinstall", "-y", "'Development Tools'"]
                    cmd2 = ["sudo", "yum", "install", "-y", "gcc-c++", "python3-devel", "openssl-devel", "libX11-devel", "libXext-devel", "cmake",
                           "libXfixes-devel", "libXft-devel", "libXrender-devel", "libXinerama-devel",
                           "libXcursor-devel", "libpng-devel", "fontconfig-devel", "freetype-devel"]
                print("Installing Development Tools group and development libraries...")
                result1 = subprocess.run(cmd1, capture_output=True, text=True)
                result2 = subprocess.run(cmd2, capture_output=True, text=True)
                result = result2  # Use the second result for error checking
                
            elif shutil.which("dnf"):
                # Fedora/newer RHEL
                if os.geteuid() == 0:
                    cmd1 = ["dnf", "groupinstall", "-y", "'Development Tools'"]
                    cmd2 = ["dnf", "install", "-y", "gcc-c++", "python3-devel", "openssl-devel", "libX11-devel", "libXext-devel", "cmake",
                           "libXfixes-devel", "libXft-devel", "libXrender-devel", "libXinerama-devel",
                           "libXcursor-devel", "libpng-devel", "fontconfig-devel", "freetype-devel"]
                else:
                    cmd1 = ["sudo", "dnf", "groupinstall", "-y", "'Development Tools'"]
                    cmd2 = ["sudo", "dnf", "install", "-y", "gcc-c++", "python3-devel", "openssl-devel", "libX11-devel", "libXext-devel", "cmake",
                           "libXfixes-devel", "libXft-devel", "libXrender-devel", "libXinerama-devel",
                           "libXcursor-devel", "libpng-devel", "fontconfig-devel", "freetype-devel"]
                print("Installing Development Tools group and development libraries...")
                result1 = subprocess.run(cmd1, capture_output=True, text=True)
                result2 = subprocess.run(cmd2, capture_output=True, text=True)
                result = result2  # Use the second result for error checking
                
            else:
                raise RuntimeError("No supported package manager found (apt, yum, dnf)")
            
            if result.returncode != 0:
                print(f"❌ Failed to install dependencies: {result.stderr}")
                raise RuntimeError("Dependency installation failed")
            else:
                print("✅ System dependencies installed successfully")
                
        elif platform.system().lower() == "darwin":
            # macOS - install Xcode Command Line Tools
            print("Installing Xcode Command Line Tools...")
            result = subprocess.run(["xcode-select", "--install"], capture_output=True, text=True)
            
            if result.returncode != 0:
                print(f"❌ Failed to install Xcode Command Line Tools: {result.stderr}")
                raise RuntimeError("Xcode Command Line Tools installation failed")
            else:
                print("✅ Xcode Command Line Tools installation initiated")
                print("Please complete the installation in the popup window, then run the installer again.")
                
        elif platform.system().lower() == "windows":
            # Windows - provide comprehensive instructions
            print("❌ Automatic dependency installation not supported on Windows")
            print("Please install the following dependencies manually:")
            print("1. Visual Studio 2022 or Build Tools for Visual Studio 2022")
            print("   Download from: https://visualstudio.microsoft.com/downloads/")
            print("2. Run from 'x64 Native Tools Command Prompt for VS 2022'")
            print("   (This sets up the proper build environment)")
            raise RuntimeError("Manual installation required on Windows")
    
    def _print_manual_install_instructions(self):
        """Print manual installation instructions for the current platform"""
        if platform.system().lower() == "linux":
            if shutil.which("apt"):
                print("  Ubuntu/Debian: sudo apt-get install build-essential libssl-dev libx11-dev libxext-dev")
            elif shutil.which("yum"):
                print("  Amazon Linux/RHEL/CentOS: sudo yum groupinstall 'Development Tools' && sudo yum install openssl-devel libX11-devel libXext-devel")
            elif shutil.which("dnf"):
                print("  Fedora: sudo dnf groupinstall 'Development Tools' && sudo dnf install openssl-devel libX11-devel libXext-devel")
            else:
                print("  Install build tools, OpenSSL development libraries, and X11 development libraries")
        elif platform.system().lower() == "darwin":
            print("  macOS: xcode-select --install")
        elif platform.system().lower() == "windows":
            print("  Windows: Install Visual Studio 2022 or Build Tools for Visual Studio 2022")
            print("  Download from: https://visualstudio.microsoft.com/downloads/")
            print("  Run from 'x64 Native Tools Command Prompt for VS 2022'")
    
    def _check_requirements(self, auto_install=False):
        """Check if all required files exist and system dependencies are available"""
        # Check system dependencies first
        self._check_system_dependencies(auto_install)
        
        # Check required files
        files = self._get_platform_files()
        missing = []
        
        if not files['executable']:
            missing.append("executable (grapa.exe/grapa)")
        if not files['static_lib']:
            missing.append("static library")
        
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
        
        # Shared libraries are no longer supported
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
        
        # Shared libraries are no longer supported
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
            # Try different PowerShell paths
            powershell_paths = [
                'powershell',
                'C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe',
                'C:\\Windows\\SysWOW64\\WindowsPowerShell\\v1.0\\powershell.exe'
            ]
            
            success = False
            for ps_path in powershell_paths:
                try:
                    # Use PowerShell to modify PATH
                    cmd = [
                        ps_path, '-Command',
                        f'$currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine"); '
                        f'if ($currentPath -notlike "*{bin_path}*") {{ '
                        f'[Environment]::SetEnvironmentVariable("PATH", "$currentPath;{bin_path}", "Machine") }}'
                    ]
                    result = subprocess.run(cmd, check=True, capture_output=True, text=True)
                    print(f"✅ Added Grapa to system PATH")
                    success = True
                    break
                except (subprocess.CalledProcessError, FileNotFoundError):
                    continue
            
            if not success:
                # Fallback: try using setx command
                try:
                    cmd = ['setx', 'PATH', f'%PATH%;{bin_path}', '/M']
                    subprocess.run(cmd, check=True, capture_output=True, text=True)
                    print(f"✅ Added Grapa to system PATH (using setx)")
                except (subprocess.CalledProcessError, FileNotFoundError):
                    print(f"⚠️  Could not automatically update PATH")
                    print(f"Please manually add {bin_path} to your system PATH")
                    
        except Exception as e:
            print(f"⚠️  Could not automatically update PATH: {e}")
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
    
    def install(self, force=False, auto_install=False):
        """Install Grapa"""
        print(f"🚀 Installing Grapa for {self.platform}...")
        
        try:
            files = self._check_requirements(auto_install)
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
                
                # Provide Ubuntu version-specific pip installation instructions
                if platform.system().lower() == "linux" and shutil.which("apt"):
                    ubuntu_version = self._detect_ubuntu_version()
                    if ubuntu_version and ubuntu_version >= (24, 4):
                        print(f"\n📦 For Python package installation (grapapy):")
                        print(f"  Ubuntu 24.04+ detected - use: pip3 install grapapy --break-system-packages")
                        print(f"  Or: python3 -m pip install grapapy --break-system-packages")
                    else:
                        print(f"\n📦 For Python package installation (grapapy):")
                        print(f"  Use: pip3 install grapapy")
                        print(f"  Or: python3 -m pip install grapapy")
            
            return True
            
        except Exception as e:
            print(f"❌ Installation failed: {e}")
            import traceback
            print(f"Error details: {traceback.format_exc()}")
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
    parser.add_argument('--install-dependencies', action='store_true', 
                       help='Automatically install system dependencies (requires sudo/admin privileges)')
    
    args = parser.parse_args()
    
    installer = GrapaInstaller()
    
    if args.install_path:
        installer.install_path = Path(args.install_path)
    
    if args.uninstall:
        installer.uninstall()
    else:
        installer.install(force=args.force, auto_install=args.install_dependencies)

if __name__ == "__main__":
    main()
