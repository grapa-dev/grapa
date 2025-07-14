#!/usr/bin/env python3
"""
Python Integration Test for Grapa Table Operations (Refactored)
Follows the structure of test_python_file_operations.py
"""

import grapapy
import os
import time

class GrapaTableHelper:
    """Helper class for $file().table() operations"""
    def __init__(self, grapa_instance, store_type="ROW"):
        self.xy = grapa_instance
        self.store_type = store_type
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


def test_table_operations():
    print("\n=== Testing Table Operations ===")
    xy = grapapy.grapa()
    try:
        table = GrapaTableHelper(xy, "ROW")
        print("Created ROW table")
        table.mkfield("name", "STR", "VAR")
        table.mkfield("age", "INT")
        table.mkfield("salary", "FLOAT", "FIX", 8)
        print("Fields created")
        table.set("user1", "John Doe", "name")
        table.set("user1", 30, "age")
        table.set("user1", 75000.5, "salary")
        print("Data inserted")
        name = table.get("user1", "name", "str")
        age = table.get("user1", "age", "int")
        salary = table.get("user1", "salary", "float")
        print(f"User1: Name={name}, Age={age}, Salary={salary}")
        records = table.ls()
        print(f"Records: {records}")
        debug = table.debug()
        print(f"Debug info: {debug}")
        # Test field removal
        table.rmfield("salary")
        try:
            salary = table.get("user1", "salary", "float")
            print(f"Salary after removal: {salary}")
        except Exception as e:
            print(f"Salary retrieval after removal (expected error): {e}")
        print("Table operations test completed")
        return True
    except Exception as e:
        print(f"Error in table operations: {e}")
        return False

def test_table_performance():
    print("\n=== Testing Table Performance ===")
    xy = grapapy.grapa()
    try:
        table = GrapaTableHelper(xy, "ROW")
        table.mkfield("id", "INT")
        table.mkfield("name", "STR", "VAR")
        table.mkfield("value", "FLOAT", "FIX", 8)
        print("Fields created")
        print("Adding 100 records...")
        start_time = time.time()
        for i in range(1, 101):
            table.set(f"record{i}", i, "id")
            table.set(f"record{i}", f"User {i}", "name")
            table.set(f"record{i}", i * 100.5, "value")
        insert_time = time.time() - start_time
        print(f"Inserted 100 records in {insert_time:.4f} seconds")
        print("Reading all records...")
        start_time = time.time()
        records = table.ls()
        read_time = time.time() - start_time
        print(f"Read {len(records) if isinstance(records, list) else 'unknown'} records in {read_time:.4f} seconds")
        start_time = time.time()
        record50_id = table.get("record50", "id", "int")
        record50_name = table.get("record50", "name", "str")
        record50_value = table.get("record50", "value", "float")
        read_specific_time = time.time() - start_time
        print(f"Record 50 - ID: {record50_id}, Name: {record50_name}, Value: {record50_value}")
        print(f"Read specific record in {read_specific_time:.4f} seconds")
        print("Table performance test completed")
        return True
    except Exception as e:
        print(f"Error in table performance test: {e}")
        return False

def main():
    print("=== GRAPA TABLE OPERATIONS TEST (REFACTORED) ===")
    print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    passed = 0
    total = 2
    if test_table_operations():
        passed += 1
    if test_table_performance():
        passed += 1
    print(f"\n=== TEST SUMMARY ===\nTests passed: {passed}/{total}")
    if passed == total:
        print("\n🎉 All table tests passed!\n")
        return 0
    else:
        print("\n❌ Some table tests failed!\n")
        return 1

if __name__ == "__main__":
    exit(main()) 