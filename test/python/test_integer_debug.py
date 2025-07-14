#!/usr/bin/env python3
"""
Debug test for integer field issues in GrapaPy table operations
"""

import grapapy
import time

def test_integer_debug():
    print("=== INTEGER FIELD DEBUG TEST ===")
    xy = grapapy.grapa()
    
    try:
        # Create table with $global
        xy.eval('$global.table = $file().table("ROW");')
        
        print("--- Testing different field creation methods ---")
        
        # Method 1: Try without explicit mode
        xy.eval('table.mkfield("age1", "INT");')
        xy.eval('table.set("user1", 30, "age1");')
        result1 = xy.eval('table.get("user1", "age1");')
        type1 = xy.eval('table.get("user1", "age1").type();')
        print(f"Method 1 - Value: {result1}, Type: {type1}")
        
        # Method 2: Try with VAR mode
        xy.eval('table.mkfield("age2", "INT", "VAR");')
        xy.eval('table.set("user1", 30, "age2");')
        result2 = xy.eval('table.get("user1", "age2");')
        type2 = xy.eval('table.get("user1", "age2").type();')
        print(f"Method 2 - Value: {result2}, Type: {type2}")
        
        # Method 3: Try with FIX mode and size
        xy.eval('table.mkfield("age3", "INT", "FIX", 4);')
        xy.eval('table.set("user1", 30, "age3");')
        result3 = xy.eval('table.get("user1", "age3");')
        type3 = xy.eval('table.get("user1", "age3").type();')
        print(f"Method 3 - Value: {result3}, Type: {type3}")
        
        # Method 4: Try setting as string first
        xy.eval('table.mkfield("age4", "INT", "FIX", 4);')
        xy.eval('table.set("user1", "30", "age4");')
        result4 = xy.eval('table.get("user1", "age4");')
        type4 = xy.eval('table.get("user1", "age4").type();')
        print(f"Method 4 - Value: {result4}, Type: {type4}")
        
        # Method 5: Try with explicit integer conversion in Grapa
        xy.eval('table.mkfield("age5", "INT", "FIX", 4);')
        xy.eval('table.set("user1", 30.int(), "age5");')
        result5 = xy.eval('table.get("user1", "age5");')
        type5 = xy.eval('table.get("user1", "age5").type();')
        print(f"Method 5 - Value: {result5}, Type: {type5}")
        
        return True
        
    except Exception as e:
        print(f"Error: {e}")
        return False

if __name__ == "__main__":
    test_integer_debug() 