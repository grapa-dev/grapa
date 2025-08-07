#!/usr/bin/env python3
"""
Post-install script to check for X11 dependencies on Linux systems.
This script is run after grapapy installation to verify X11 libraries are available.
"""

import os
import sys
import subprocess
import platform

def check_x11_libraries():
    """Check if X11 libraries are available on the system"""
    if platform.system() != 'Linux':
        return []  # Only check on Linux, return empty list (no missing libs)
    
    # List of X11 libraries that GrapaPy requires
    x11_libs = [
        'libX11.so',
        'libXfixes.so', 
        'libXft.so',
        'libXext.so',
        'libXrender.so',
        'libXinerama.so',
        'libfontconfig.so',
        'libXcursor.so'
    ]
    
    missing_libs = []
    
    for lib in x11_libs:
        try:
            # Try to find the library using ldconfig
            result = subprocess.run(['ldconfig', '-p'], capture_output=True, text=True)
            if lib not in result.stdout:
                missing_libs.append(lib)
        except FileNotFoundError:
            # ldconfig not available, try alternative method
            try:
                result = subprocess.run(['find', '/usr/lib*', '/lib*', '-name', lib], 
                                      capture_output=True, text=True)
                if not result.stdout.strip():
                    missing_libs.append(lib)
            except FileNotFoundError:
                missing_libs.append(lib)
    
    return missing_libs

def get_install_instructions():
    """Get installation instructions for the detected Linux distribution"""
    try:
        # Try to detect the Linux distribution
        if os.path.exists('/etc/os-release'):
            with open('/etc/os-release', 'r') as f:
                content = f.read().lower()
                if 'ubuntu' in content or 'debian' in content:
                    return """Ubuntu/Debian:
sudo apt-get update
sudo apt-get install libx11-dev libxfixes-dev libxft-dev libxext-dev libxrender-dev libxinerama-dev libfontconfig1-dev libxcursor-dev"""
                elif 'centos' in content or 'rhel' in content or 'redhat' in content:
                    return """CentOS/RHEL:
sudo yum install libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel"""
                elif 'fedora' in content:
                    return """Fedora:
sudo dnf install libX11-devel libXfixes-devel libXft-devel libXext-devel libXrender-devel libXinerama-devel fontconfig-devel libXcursor-devel"""
                elif 'arch' in content:
                    return """Arch Linux:
sudo pacman -S libx11 libxfixes libxft libxext libxrender libxinerama fontconfig libxcursor"""
                else:
                    return """Generic Linux:
Install X11 development libraries for your distribution.
Common package names: libX11-devel, libx11-dev, libXfixes-devel, etc."""
    except:
        return """Generic Linux:
Install X11 development libraries for your distribution.
Common package names: libX11-devel, libx11-dev, libXfixes-devel, etc."""

def main():
    """Main function to check X11 dependencies"""
    print("🔍 Checking X11 dependencies for GrapaPy...")
    
    missing_libs = check_x11_libraries()
    
    if not missing_libs:
        print("✅ All X11 dependencies are available!")
        print("🎉 GrapaPy should work correctly on this system.")
        return 0
    else:
        print("❌ Missing X11 dependencies detected:")
        for lib in missing_libs:
            print(f"   - {lib}")
        
        print("\n📋 To install X11 dependencies, run:")
        print(get_install_instructions())
        
        print("\n⚠️  Note: GrapaPy requires X11 libraries for GUI functionality.")
        print("   If you're on a headless server without GUI, you may encounter")
        print("   'undefined symbol: XDrawArc' errors when importing grapapy.")
        print("   Consider installing X11 libraries or using a system with GUI support.")
        
        return 1

if __name__ == "__main__":
    sys.exit(main()) 