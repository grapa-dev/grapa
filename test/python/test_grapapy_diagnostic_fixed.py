#!/usr/bin/env python3
"""
Python Integration Diagnostic Test for GrapaPy (Fixed, Refactored)
Follows the structure of test_python_file_operations.py
"""

import grapapy
import os
import sys
import time

class GrapaFileHelper:
    """Helper class for $file operations"""
    def __init__(self, grapa_instance):
        self.xy = grapa_instance
        self.xy.eval(r'$global.f = $file();')
    def pwd(self):
        return self.xy.eval(r'f.pwd();')
    def phd(self):
        return self.xy.eval(r'f.phd();')
    def set(self, name, content):
        escaped_content = content.replace('"', '\\"').replace('\n', '\\n')
        return self.xy.eval(fr'f.set("{name}", "{escaped_content}");')
    def get(self, name):
        return self.xy.eval(fr'f.get("{name}");')
    def get_str(self, name):
        return self.xy.eval(fr'f.get("{name}").str();')
    def info(self, name):
        return self.xy.eval(fr'f.info("{name}");')
    def ls(self):
        return self.xy.eval(r'f.ls();')
    def mk(self, name):
        return self.xy.eval(fr'f.mk("{name}");')
    def rm(self, name):
        return self.xy.eval(fr'f.rm("{name}");')
    def cd(self, path):
        return self.xy.eval(fr'f.cd("{path}");')

def test_basic_operations_fixed():
    print("\n--- Test 1: Basic Operations ---")
    xy = grapapy.grapa()
    try:
        result = xy.eval("2 + 2;")
        print(f"Basic math: {result}")
        result = xy.eval("x = 10; x;")
        print(f"Variable assignment: {result}")
        return True
    except Exception as e:
        print(f"Basic operations error: {e}")
        return False

def test_file_object_methods_fixed():
    print("\n--- Test 2: File Object Methods ---")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        pwd = file_helper.pwd()
        print(f"pwd(): {pwd}")
        phd = file_helper.phd()
        print(f"phd(): {phd}")
        return True
    except Exception as e:
        print(f"File object methods error: {e}")
        return False

def test_file_operations_fixed():
    print("\n--- Test 3: File Operations ---")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        file_helper.set("test_diagnostic_fixed.txt", "test content")
        print("set() with relative path: success")
        abs_path = os.path.abspath("test_diagnostic_abs_fixed.txt")
        file_helper.set(abs_path, "test content")
        print("set() with absolute path: success")
        if os.path.exists("test_diagnostic_fixed.txt"):
            with open("test_diagnostic_fixed.txt", "r") as f:
                content = f.read()
            print(f"test_diagnostic_fixed.txt exists, Content: {content}")
        else:
            print("test_diagnostic_fixed.txt does not exist")
        files = file_helper.ls()
        print(f"ls(): {files}")
        info = file_helper.info("test_diagnostic_fixed.txt")
        print(f"info(): {info}")
        return True
    except Exception as e:
        print(f"File operations error: {e}")
        return False

def test_file_reading_fixed():
    print("\n--- Test 4: File Reading ---")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        result = file_helper.get_str("test_diagnostic_fixed.txt")
        print(f"get() and str(): {result}")
        return True
    except Exception as e:
        print(f"File reading error: {e}")
        return False

def test_directory_operations_fixed():
    print("\n--- Test 5: Directory Operations ---")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        file_helper.mk("test_diagnostic_dir")
        file_helper.cd("test_diagnostic_dir")
        pwd = file_helper.pwd()
        print(f"pwd() after cd: {pwd}")
        return True
    except Exception as e:
        print(f"Directory operations error: {e}")
        return False

def test_extension_info_fixed():
    print("\n--- Test 6: GrapaPy Extension Info ---")
    xy = grapapy.grapa()
    try:
        result = xy.eval(r'$global.f = $file(); f;')
        print(f"File object: {result}")
        sys_info = xy.eval(r'$sys();')
        print(f"$sys(): {sys_info}")
        return True
    except Exception as e:
        print(f"Extension info error: {e}")
        return False

def cleanup_fixed():
    print("\n--- Cleanup ---")
    try:
        xy = grapapy.grapa()
        file_helper = GrapaFileHelper(xy)
        file_helper.cd("..")
        file_helper.rm("test_diagnostic_dir")
        file_helper.rm("test_diagnostic_fixed.txt")
        if os.path.exists("test_diagnostic_abs_fixed.txt"):
            file_helper.rm("test_diagnostic_abs_fixed.txt")
        print("Cleanup completed")
        return True
    except Exception as e:
        print(f"Cleanup error: {e}")
        return False

def main():
    print("=== GRAPAPY DIAGNOSTIC TESTS (FIXED, REFACTORED) ===")
    print(f"Python version: {sys.version}")
    print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    passed = 0
    total = 7
    if test_basic_operations_fixed():
        passed += 1
    if test_file_object_methods_fixed():
        passed += 1
    if test_file_operations_fixed():
        passed += 1
    if test_file_reading_fixed():
        passed += 1
    if test_directory_operations_fixed():
        passed += 1
    if test_extension_info_fixed():
        passed += 1
    if cleanup_fixed():
        passed += 1
    print(f"\n=== TEST SUMMARY ===\nTests passed: {passed}/{total}")
    if passed == total:
        print("\n🎉 All fixed diagnostic tests passed!\n")
        return 0
    else:
        print("\n❌ Some fixed diagnostic tests failed!\n")
        return 1

if __name__ == "__main__":
    exit(main()) 