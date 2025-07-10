#!/usr/bin/env python3
"""
Python Test Runner for Grapa
Runs all Python integration and functionality tests
"""

import sys
import os
import subprocess
import time

def run_python_test(test_file, description):
    """Run a single Python test file and report results"""
    print(f"--- {description} ---")
    print(f"Running: {test_file}")
    
    try:
        # Run the test file
        result = subprocess.run([sys.executable, test_file], 
                              capture_output=True, text=True, timeout=30)
        
        if result.returncode == 0:
            print("✅ PASS - Test completed successfully")
            if result.stdout.strip():
                print("Output:")
                print(result.stdout.strip())
        else:
            print("❌ FAIL - Test failed")
            if result.stderr.strip():
                print("Error:")
                print(result.stderr.strip())
            if result.stdout.strip():
                print("Output:")
                print(result.stdout.strip())
        
        print("---")
        return result.returncode == 0
        
    except subprocess.TimeoutExpired:
        print("❌ FAIL - Test timed out after 30 seconds")
        print("---")
        return False
    except Exception as e:
        print(f"❌ FAIL - Test execution error: {e}")
        print("---")
        return False

def main():
    """Main test runner function"""
    print("=== GRAPA PYTHON TEST SUITE ===")
    print("Running Python integration and functionality tests")
    print(f"Timestamp: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    
    # Define test files and their descriptions
    tests = [
        ("test_python_examples.py", "Python Integration Tests"),
        ("test_grep_python_examples.py", "Python Grep Functionality Tests")
    ]
    
    # Get the directory where this script is located
    test_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Change to test directory
    os.chdir(test_dir)
    
    passed = 0
    total = len(tests)
    
    # Run each test
    for test_file, description in tests:
        if run_python_test(test_file, description):
            passed += 1
    
    # Summary
    print("=== TEST SUMMARY ===")
    print(f"Tests passed: {passed}/{total}")
    
    if passed == total:
        print("🎉 All Python tests passed!")
        print("Python integration and functionality verified successfully.")
        return 0
    else:
        print("⚠️  Some Python tests failed!")
        print("Please check the test output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 