#!/usr/bin/env python3
"""
Python Integration Test for Grapa File Operations (Simple, Refactored)
Follows the structure of test_python_file_operations.py
"""

import grapapy
import os
import time

class GrapaFileHelper:
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

def test_basic_integration():
    print("\n=== Test Basic Python Integration ===")
    xy = grapapy.grapa()
    try:
        result = xy.eval("x = 3.45; y = 4.32; x**y;")
        print(f"Basic calculation result: {result}")
        result = xy.eval(r'f = $file(); f.pwd();')
        print(f"Simple file operation result: {result}")
        return True
    except Exception as e:
        print(f"Error with basic integration: {e}")
        return False

def test_file_system_operations():
    print("\n=== Test File System Operations ===")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        file_helper.mk("python_test_dir")
        file_helper.cd("python_test_dir")
        current_dir = file_helper.pwd()
        print(f"Current directory: {current_dir}")
        file_helper.set("test_file.txt", "Python test content")
        content = file_helper.get_str("test_file.txt")
        print(f"File content: {content}")
        info = file_helper.info("test_file.txt")
        print(f"File info: {info}")
        files = file_helper.ls()
        print(f"Directory files: {files}")
        file_helper.rm("test_file.txt")
        file_helper.cd("..")
        file_helper.rm("python_test_dir")
        print("File system operations completed")
        return True
    except Exception as e:
        print(f"Error with file system operations: {e}")
        return False

def test_file_split():
    print("\n=== Test File Split Functionality ===")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        xy.eval(r'''
            content = "";
            i = 1;
            while (i <= 20) {
                content = content + "Line " + i + ": Test data for splitting.\n";
                i = i + 1;
            };
            f.set("test_file.txt", content);
        ''')
        file_info = file_helper.info("test_file.txt")
        print(f"File info: {file_info}")
        file_helper.mk("split_output")
        result = file_helper.xy.eval('f.split(4, "test_file.txt", "split_output", "\\n", "");')
        print(f"Split result: {result}")
        file_helper.cd("split_output")
        split_files = file_helper.ls()
        print(f"Split files created: {split_files}")
        file_helper.cd("..")
        file_helper.rm("split_output")
        file_helper.rm("test_file.txt")
        print("File split test completed")
        return True
    except Exception as e:
        print(f"Error with file split: {e}")
        return False

def test_error_handling():
    print("\n=== Test Error Handling ===")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        try:
            result = file_helper.get("nonexistent.txt")
            print(f"Non-existent file result: {result}")
        except Exception as e:
            print(f"Error getting non-existent file: {e}")
        try:
            result = file_helper.cd("nonexistent_dir")
            print(f"Non-existent directory result: {result}")
        except Exception as e:
            print(f"Error changing to non-existent directory: {e}")
        print("Error handling test completed")
        return True
    except Exception as e:
        print(f"Error with error handling test: {e}")
        return False

def main():
    print("=== GRAPA PYTHON INTEGRATION TESTS (SIMPLE, REFACTORED) ===")
    print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    passed = 0
    total = 4
    if test_basic_integration():
        passed += 1
    if test_file_system_operations():
        passed += 1
    if test_file_split():
        passed += 1
    if test_error_handling():
        passed += 1
    print(f"\n=== TEST SUMMARY ===\nTests passed: {passed}/{total}")
    if passed == total:
        print("\n🎉 All simple integration tests passed!\n")
        return 0
    else:
        print("\n❌ Some simple integration tests failed!\n")
        return 1

if __name__ == "__main__":
    exit(main()) 