#!/usr/bin/env python3
"""
Test script for Windows dependency checking
Run this on a clean Windows system to verify dependency detection
"""

import sys
import subprocess
import platform

def test_msbuild():
    """Test MSBuild detection"""
    try:
        result = subprocess.run(['msbuild', '/version'], capture_output=True, text=True, check=True)
        print(f"✅ MSBuild found: {result.stdout.strip()}")
        if '17.' in result.stdout:
            print("✅ Visual Studio 2022 detected")
            return True
        else:
            print("❌ Visual Studio 2022 not detected (version 17.x required)")
            return False
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("❌ MSBuild not found")
        return False

def test_7zip():
    """Test 7-Zip detection"""
    try:
        subprocess.run(['7z'], capture_output=True, check=True)
        print("✅ 7-Zip found")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("❌ 7-Zip not found")
        return False

def test_python():
    """Test Python version"""
    version = sys.version_info
    print(f"Python version: {version.major}.{version.minor}.{version.micro}")
    if version >= (3, 6):
        print("✅ Python 3.6+ detected")
        return True
    else:
        print("❌ Python 3.6+ required")
        return False

def test_windows_sdk():
    """Test Windows SDK detection"""
    try:
        import winreg
        sdk_keys = [
            r"SOFTWARE\Microsoft\Windows Kits\Installed Roots",
            r"SOFTWARE\WOW6432Node\Microsoft\Windows Kits\Installed Roots"
        ]
        for key_path in sdk_keys:
            try:
                key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, key_path)
                winreg.CloseKey(key)
                print("✅ Windows SDK found")
                return True
            except FileNotFoundError:
                continue
        print("❌ Windows SDK not found")
        return False
    except ImportError:
        print("⚠️  winreg not available, skipping SDK check")
        return False

def main():
    print("=" * 60)
    print("Windows Dependency Check Test")
    print("=" * 60)
    print(f"Platform: {platform.system()} {platform.machine()}")
    print()
    
    results = []
    results.append(("MSBuild (Visual Studio 2022)", test_msbuild()))
    results.append(("7-Zip", test_7zip()))
    results.append(("Python 3.6+", test_python()))
    results.append(("Windows SDK", test_windows_sdk()))
    
    print()
    print("=" * 60)
    print("Summary:")
    print("=" * 60)
    
    all_passed = True
    for name, passed in results:
        status = "✅ PASS" if passed else "❌ FAIL"
        print(f"{name:25} {status}")
        if not passed:
            all_passed = False
    
    print()
    if all_passed:
        print("🎉 All dependencies found! Ready for Grapa development.")
    else:
        print("⚠️  Some dependencies missing. Install missing components:")
        print("   • Visual Studio 2022: https://visualstudio.microsoft.com/downloads/")
        print("   • 7-Zip: https://www.7-zip.org/")
        print("   • Run from 'x64 Native Tools Command Prompt for VS 2022'")

if __name__ == "__main__":
    main()
