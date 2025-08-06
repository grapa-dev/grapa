#!/usr/bin/env python3
"""
Simple build validation script for Grapa
Tests CLI and Python version output for each platform
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def test_cli_version(executable_path):
    """Test CLI version using -c option with $sys().getenv($VERSION)"""
    try:
        result = subprocess.run([executable_path, "-c", "$sys().getenv($VERSION)"], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            version = result.stdout.strip()
            print(f"✅ CLI Version: {version}")
            return True
        else:
            print(f"❌ CLI failed: {result.stderr}")
            return False
    except Exception as e:
        print(f"❌ CLI error: {e}")
        return False

def test_python_version():
    """Test Python version"""
    try:
        import grapapy
        version = grapapy.__version__
        print(f"✅ Python Version: {version}")
        return True
    except ImportError as e:
        print(f"❌ Python import failed: {e}")
        return False
    except Exception as e:
        print(f"❌ Python error: {e}")
        return False

def main():
    """Main validation function"""
    print("🔍 Grapa Build Validation")
    print("=" * 50)
    
    # Get current platform
    current_platform = platform.system().lower()
    current_arch = platform.machine().lower()
    
    print(f"Current platform: {current_platform} {current_arch}")
    print()
    
    # Test CLI version
    print("📋 Testing CLI Version:")
    if current_platform == "windows":
        cli_path = "./grapa.exe"
    else:
        cli_path = "./grapa"
    
    if os.path.exists(cli_path):
        cli_success = test_cli_version(cli_path)
    else:
        print(f"❌ CLI executable not found: {cli_path}")
        cli_success = False
    
    print()
    
    # Test Python version
    print("🐍 Testing Python Version:")
    python_success = test_python_version()
    
    print()
    print("=" * 50)
    
    if cli_success and python_success:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 