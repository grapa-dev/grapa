#!/usr/bin/env python3
"""
Organized Test Runner for Grapa
Runs .grc files using the Grapa executable and .py files using the Grapa extension
"""

import os
import sys
import subprocess
import time
from pathlib import Path

def run_grapa_grc_file(test_file):
    """Run a .grc file using the Grapa executable"""
    try:
        print(f"Running GRC test: {test_file}")
        
        # Run the .grc file directly with Grapa executable
        result = subprocess.run(
            ["./grapa", "-cfile", test_file],
            capture_output=True,
            text=True,
            timeout=300  # 5 minute timeout
        )
        
        if result.returncode == 0:
            print(f"✓ {test_file} - PASSED")
            return True
        else:
            print(f"✗ {test_file} - FAILED")
            print(f"Error: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print(f"✗ {test_file} - TIMEOUT")
        return False
    except Exception as e:
        print(f"✗ {test_file} - ERROR: {e}")
        return False

def run_python_file(test_file):
    """Run a .py file using Python directly"""
    try:
        print(f"Running Python test: {test_file}")
        
        # Run the Python file directly
        result = subprocess.run(
            [sys.executable, test_file],
            capture_output=True,
            text=True,
            timeout=300  # 5 minute timeout
        )
        
        if result.returncode == 0:
            print(f"✓ {test_file} - PASSED")
            return True
        else:
            print(f"✗ {test_file} - FAILED")
            print(f"Error: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print(f"✗ {test_file} - TIMEOUT")
        return False
    except Exception as e:
        print(f"✗ {test_file} - ERROR: {e}")
        return False

def run_file_system_tests():
    """Run file system tests (.grc files)"""
    print("\n=== RUNNING FILE SYSTEM TESTS ===")
    
    fs_tests = [
        "test/file_system/test_file_basic.grc",
        "test/file_system/test_file_advanced.grc"
    ]
    
    passed = 0
    total = len(fs_tests)
    
    for test_file in fs_tests:
        if os.path.exists(test_file):
            if run_grapa_grc_file(test_file):
                passed += 1
        else:
            print(f"⚠ {test_file} - NOT FOUND")
    
    return passed, total

def run_database_tests():
    """Run database tests (.grc files)"""
    print("\n=== RUNNING DATABASE TESTS ===")
    
    db_tests = [
        "test/database/test_table_basic.grc"
    ]
    
    passed = 0
    total = len(db_tests)
    
    for test_file in db_tests:
        if os.path.exists(test_file):
            if run_grapa_grc_file(test_file):
                passed += 1
        else:
            print(f"⚠ {test_file} - NOT FOUND")
    
    return passed, total

def run_integration_tests():
    """Run integration tests (.grc files)"""
    print("\n=== RUNNING INTEGRATION TESTS ===")
    
    integration_tests = [
        "test/integration/test_unified_path_system.grc"
    ]
    
    passed = 0
    total = len(integration_tests)
    
    for test_file in integration_tests:
        if os.path.exists(test_file):
            if run_grapa_grc_file(test_file):
                passed += 1
        else:
            print(f"⚠ {test_file} - NOT FOUND")
    
    return passed, total

def run_python_tests():
    """Run Python integration tests (.py files)"""
    print("\n=== RUNNING PYTHON INTEGRATION TESTS ===")
    
    python_tests = [
        "test/python/test_python_file_operations.py"
    ]
    
    passed = 0
    total = len(python_tests)
    
    for test_file in python_tests:
        if os.path.exists(test_file):
            if run_python_file(test_file):
                passed += 1
        else:
            print(f"⚠ {test_file} - NOT FOUND")
    
    return passed, total

def run_grep_tests():
    """Run grep tests (.grc files)"""
    print("\n=== RUNNING GREP TESTS ===")

    # Automatically discover all .grc files in test/grep/
    grep_dir = Path("test/grep")
    grep_tests = sorted(str(f) for f in grep_dir.glob("*.grc"))

    passed = 0
    total = len(grep_tests)

    for test_file in grep_tests:
        if os.path.exists(test_file):
            if run_grapa_grc_file(test_file):
                passed += 1
        else:
            print(f"⚠ {test_file} - NOT FOUND")

    return passed, total

def main():
    """Main test runner"""
    print("=== GRAPA ORGANIZED TEST SUITE ===")
    print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    
    # Check if grapa executable exists
    if not os.path.exists("./grapa"):
        print("Error: grapa executable not found in current directory")
        sys.exit(1)
    
    total_passed = 0
    total_tests = 0
    
    # Run different test categories
    test_categories = [
        ("File System (.grc)", run_file_system_tests),
        ("Database (.grc)", run_database_tests),
        ("Integration (.grc)", run_integration_tests),
        ("Python Integration (.py)", run_python_tests),
        ("Grep (.grc)", run_grep_tests)
    ]
    
    results = {}
    
    for category_name, test_runner in test_categories:
        print(f"\n{'='*50}")
        print(f"CATEGORY: {category_name}")
        print(f"{'='*50}")
        
        passed, total = test_runner()
        results[category_name] = (passed, total)
        total_passed += passed
        total_tests += total
        
        print(f"\n{category_name} Results: {passed}/{total} passed")
    
    # Print summary
    print(f"\n{'='*60}")
    print("FINAL TEST SUMMARY")
    print(f"{'='*60}")
    
    for category_name, (passed, total) in results.items():
        percentage = (passed / total * 100) if total > 0 else 0
        print(f"{category_name:25} {passed:3}/{total:3} ({percentage:5.1f}%)")
    
    print(f"{'='*60}")
    overall_percentage = (total_passed / total_tests * 100) if total_tests > 0 else 0
    print(f"OVERALL:                {total_passed:3}/{total_tests:3} ({overall_percentage:5.1f}%)")
    
    if total_passed == total_tests:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        return 0
    else:
        print(f"\n❌ {total_tests - total_passed} TESTS FAILED")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 