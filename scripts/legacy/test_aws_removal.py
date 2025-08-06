#!/usr/bin/env python3
"""
Test script to verify AWS detection has been removed from build.py and setup.py
"""

import sys
import os
import subprocess

def test_build_py_aws_removal():
    """Test that build.py no longer detects AWS"""
    print("Testing build.py AWS detection removal...")
    
    # Check if AWS detection code exists in build.py
    with open("build.py", "r") as f:
        content = f.read()
    
    # These strings should NOT be found
    aws_strings = [
        "Amazon Linux",
        "/etc/system-release",
        "aws-arm64",
        "aws-amd64",
        "_run_aws_build_command"
    ]
    
    found_aws = []
    for aws_string in aws_strings:
        if aws_string in content:
            found_aws.append(aws_string)
    
    if found_aws:
        print(f"❌ Found AWS-related code in build.py: {found_aws}")
        return False
    else:
        print("✅ No AWS detection code found in build.py")
        return True

def test_setup_py_aws_removal():
    """Test that setup.py no longer detects AWS"""
    print("Testing setup.py AWS detection removal...")
    
    # Check if AWS detection code exists in setup.py
    with open("setup.py", "r") as f:
        content = f.read()
    
    # These strings should NOT be found
    aws_strings = [
        "Amazon Linux",
        "is_aws",
        "aws-arm64",
        "aws-amd64"
    ]
    
    found_aws = []
    for aws_string in aws_strings:
        if aws_string in content:
            found_aws.append(aws_string)
    
    if found_aws:
        print(f"❌ Found AWS-related code in setup.py: {found_aws}")
        return False
    else:
        print("✅ No AWS detection code found in setup.py")
        return True

def test_platform_detection():
    """Test that platform detection works correctly"""
    print("Testing platform detection...")
    
    try:
        # Run build.py to see what platform it detects
        result = subprocess.run([sys.executable, "build.py", "--exe-only"], 
                              capture_output=True, text=True, timeout=30)
        
        if result.returncode == 0:
            print("✅ Platform detection works correctly")
            return True
        else:
            print(f"❌ Platform detection failed: {result.stderr}")
            return False
    except subprocess.TimeoutExpired:
        print("❌ Platform detection timed out")
        return False
    except Exception as e:
        print(f"❌ Platform detection error: {e}")
        return False

def main():
    """Run all tests"""
    print("🔍 Testing AWS removal from build system...")
    print("=" * 50)
    
    tests = [
        test_build_py_aws_removal,
        test_setup_py_aws_removal,
        test_platform_detection
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        if test():
            passed += 1
        print()
    
    print("=" * 50)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("✅ All tests passed! AWS removal successful.")
        return 0
    else:
        print("❌ Some tests failed. AWS removal incomplete.")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 