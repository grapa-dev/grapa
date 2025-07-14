#!/usr/bin/env python3
"""
Debug test for performance issues with multiple records in GrapaPy table operations
"""

import grapapy
import time

def test_performance_debug():
    print("=== PERFORMANCE DEBUG TEST ===")
    xy = grapapy.grapa()
    
    try:
        # Test with COLUMN store instead of ROW
        print("--- Testing with COLUMN store ---")
        xy.eval('$global.col_table = $file().table("COL");')
        
        # Create fields with working method
        xy.eval('col_table.mkfield("id", "INT");')
        xy.eval('col_table.mkfield("name", "STR", "VAR");')
        xy.eval('col_table.mkfield("value", "FLOAT", "FIX", 8);')
        
        print("COLUMN fields created")
        
        # Add 20 records to column table
        for i in range(1, 21):
            xy.eval(f'col_table.set("record{i}", {i}, "id");')
            xy.eval(f'col_table.set("record{i}", "User {i}", "name");')
            xy.eval(f'col_table.set("record{i}", {i * 100.5}, "value");')
        
        # Test reading record 15 from column table
        print("Testing COLUMN record 15:")
        id15 = xy.eval('col_table.get("record15", "id").int();')
        name15 = xy.eval('col_table.get("record15", "name").str();')
        value15 = xy.eval('col_table.get("record15", "value").float();')
        print(f"COLUMN Record 15 - ID: {id15}, Name: {name15}, Value: {value15}")
        
        # Test with ROW table but smaller dataset
        print("\n--- Testing with ROW store (10 records) ---")
        xy.eval('$global.row_table = $file().table("ROW");')
        
        xy.eval('row_table.mkfield("id", "INT");')
        xy.eval('row_table.mkfield("name", "STR", "VAR");')
        xy.eval('row_table.mkfield("value", "FLOAT", "FIX", 8);')
        
        print("ROW fields created")
        
        # Add only 10 records to row table
        for i in range(1, 11):
            xy.eval(f'row_table.set("record{i}", {i}, "id");')
            xy.eval(f'row_table.set("record{i}", "User {i}", "name");')
            xy.eval(f'row_table.set("record{i}", {i * 100.5}, "value");')
        
        # Test reading record 8 from row table
        print("Testing ROW record 8:")
        id8 = xy.eval('row_table.get("record8", "id").int();')
        name8 = xy.eval('row_table.get("record8", "name").str();')
        value8 = xy.eval('row_table.get("record8", "value").float();')
        print(f"ROW Record 8 - ID: {id8}, Name: {name8}, Value: {value8}")
        
        return True
        
    except Exception as e:
        print(f"Error: {e}")
        return False

if __name__ == "__main__":
    test_performance_debug() 