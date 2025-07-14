#!/usr/bin/env python3
"""
Python Integration Test for Grapa Global Namespace (Refactored)
Follows the structure of test_python_file_operations.py
"""

import grapapy
import os
import time

class GrapaFileHelper:
    def __init__(self, grapa_instance):
        self.xy = grapa_instance
        self.xy.eval('$global.fs = $file();')
    def pwd(self):
        return self.xy.eval('fs.pwd();')
    def phd(self):
        return self.xy.eval('fs.phd();')
    def set(self, name, content):
        escaped_content = content.replace('"', '\\"').replace('\n', '\\n')
        return self.xy.eval(f'fs.set("{name}", "{escaped_content}");')
    def get(self, name):
        return self.xy.eval(f'fs.get("{name}");')
    def get_str(self, name):
        return self.xy.eval(f'fs.get("{name}").str();')
    def info(self, name):
        return self.xy.eval(f'fs.info("{name}");')
    def ls(self):
        return self.xy.eval('fs.ls();')
    def mk(self, name):
        return self.xy.eval(f'fs.mk("{name}");')
    def rm(self, name):
        return self.xy.eval(f'fs.rm("{name}");')
    def cd(self, path):
        return self.xy.eval(f'fs.cd("{path}");')

class GrapaTableHelper:
    def __init__(self, grapa_instance, store_type="ROW"):
        self.xy = grapa_instance
        self.xy.eval(f'$global.table = $file().table("{store_type}");')
    def mkfield(self, name, ftype, fmode=None, flen=None):
        if fmode and flen:
            return self.xy.eval(f'table.mkfield("{name}", "{ftype}", "{fmode}", {flen});')
        elif fmode:
            return self.xy.eval(f'table.mkfield("{name}", "{ftype}", "{fmode}");')
        else:
            return self.xy.eval(f'table.mkfield("{name}", "{ftype}");')
    def set(self, key, value, field):
        return self.xy.eval(f'table.set("{key}", {repr(value)}, "{field}");')
    def get(self, key, field, conv=None):
        if conv:
            return self.xy.eval(f'table.get("{key}", "{field}").{conv}();')
        return self.xy.eval(f'table.get("{key}", "{field}");')
    def ls(self):
        return self.xy.eval('table.ls();')
    def rmfield(self, name):
        return self.xy.eval(f'table.rmfield("{name}");')
    def debug(self):
        return self.xy.eval('table.debug();')

def test_global_variable():
    print("\n=== Test Global Variable ===")
    xy = grapapy.grapa()
    try:
        xy.eval("$global.test_var = 'Hello from global';")
        result = xy.eval("$global.test_var;")
        print(f"Global variable test: {result}")
        xy.eval("local_var = 'Hello from local';")
        try:
            result = xy.eval("local_var;")
            print(f"Local variable (should fail): {result}")
        except Exception as e:
            print(f"Local variable correctly not found: {e}")
        return True
    except Exception as e:
        print(f"Global variable test error: {e}")
        return False

def test_file_operations_global():
    print("\n=== Test File Operations with Global Namespace ===")
    xy = grapapy.grapa()
    try:
        file_helper = GrapaFileHelper(xy)
        pwd = file_helper.pwd()
        print(f"Current directory: {pwd}")
        home = file_helper.phd()
        print(f"Home directory: {home}")
        file_helper.set('test_global.txt', 'Hello from global namespace!')
        content = file_helper.get_str('test_global.txt')
        print(f"File content: {content}")
        info = file_helper.info('test_global.txt')
        print(f"File info: {info}")
        files = file_helper.ls()
        print(f"Directory files: {files}")
        file_helper.mk('test_global_dir')
        file_helper.cd('test_global_dir')
        current_dir = file_helper.pwd()
        print(f"Changed to directory: {current_dir}")
        file_helper.set('subdir_file.txt', 'File in subdirectory')
        subdir_content = file_helper.get_str('subdir_file.txt')
        print(f"Subdirectory file content: {subdir_content}")
        file_helper.cd('..')
        file_helper.rm('test_global_dir')
        file_helper.rm('test_global.txt')
        print("File operations with global namespace completed successfully!")
        return True
    except Exception as e:
        print(f"File operations global error: {e}")
        return False

def test_table_operations_global():
    print("\n=== Test Table Operations with Global Namespace ===")
    xy = grapapy.grapa()
    try:
        table = GrapaTableHelper(xy, "ROW")
        table.mkfield('name', 'STR', 'VAR')
        table.mkfield('age', 'INT')
        table.mkfield('salary', 'FLOAT', 'FIX', 8)
        table.set('user1', 'John Doe', 'name')
        table.set('user1', 30, 'age')
        table.set('user1', 75000.50, 'salary')
        name = table.get('user1', 'name', 'str')
        age = table.get('user1', 'age', 'int')
        salary = table.get('user1', 'salary', 'float')
        print(f"User data - Name: {name}, Age: {age}, Salary: {salary}")
        table.set('user2', 'Jane Smith', 'name')
        table.set('user2', 25, 'age')
        table.set('user2', 65000.75, 'salary')
        records = table.ls()
        print(f"All records: {records}")
        print("Table operations with global namespace completed successfully!")
        return True
    except Exception as e:
        print(f"Table operations global error: {e}")
        return False

def main():
    print("=== GRAPA GLOBAL NAMESPACE TEST (REFACTORED) ===")
    print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    passed = 0
    total = 3
    if test_global_variable():
        passed += 1
    if test_file_operations_global():
        passed += 1
    if test_table_operations_global():
        passed += 1
    print(f"\n=== TEST SUMMARY ===\nTests passed: {passed}/{total}")
    if passed == total:
        print("\n🎉 All global namespace tests passed!\n")
        return 0
    else:
        print("\n❌ Some global namespace tests failed!\n")
        return 1

if __name__ == "__main__":
    exit(main()) 