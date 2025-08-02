#!/usr/bin/env python3
"""
Basic GrapaPy Operations Test
Tests fundamental operations that should work reliably
"""

import grapapy
import sys

def test_basic_math():
    """Test basic mathematical operations"""
    print("=== Basic Math Operations ===")
    
    xy = grapapy.grapa()
    
    tests = [
        ("2 + 2", 4),
        ("10 - 5", 5),
        ("6 * 7", 42),
        ("100 / 4", 25),
        ("2 ** 8", 256),
        ("15 % 4", 3)
    ]
    
    for expr, expected in tests:
        try:
            result = xy.eval(f"{expr};")
            status = "✅" if result == expected else "❌"
            print(f"   {status} {expr} = {result} (expected {expected})")
        except Exception as e:
            print(f"   ❌ {expr} failed: {e}")

def test_string_operations():
    """Test string operations"""
    print("\n=== String Operations ===")
    
    xy = grapapy.grapa()
    
    tests = [
        ("'Hello' + ' World'", "Hello World"),
        ("'Test'.len()", 4),
        ("'Hello World'.upper()", "HELLO WORLD"),
        ("'HELLO WORLD'.lower()", "hello world")
    ]
    
    for expr, expected in tests:
        try:
            result = xy.eval(f"{expr};")
            status = "✅" if result == expected else "❌"
            print(f"   {status} {expr} = {result} (expected {expected})")
        except Exception as e:
            print(f"   ❌ {expr} failed: {e}")

def test_array_operations():
    """Test array operations"""
    print("\n=== Array Operations ===")
    
    xy = grapapy.grapa()
    
    # Test array creation and basic operations
    try:
        result = xy.eval("[1, 2, 3, 4, 5].len();")
        print(f"   ✅ Array length: {result}")
    except Exception as e:
        print(f"   ❌ Array length failed: {e}")
    
    try:
        result = xy.eval("[1, 2, 3, 4, 5][2];")
        print(f"   ✅ Array indexing: {result}")
    except Exception as e:
        print(f"   ❌ Array indexing failed: {e}")

def test_high_precision_math():
    """Test high precision mathematical operations"""
    print("\n=== High Precision Math ===")
    
    xy = grapapy.grapa()
    
    try:
        # Test large number multiplication
        result = xy.eval("12345678901234567890 * 98765432109876543210;")
        print(f"   ✅ Large multiplication: {result}")
    except Exception as e:
        print(f"   ❌ Large multiplication failed: {e}")

def test_simple_functions():
    """Test simple function definition and calling"""
    print("\n=== Simple Functions ===")
    
    xy = grapapy.grapa()
    
    try:
        # Define a simple function
        xy.eval("$global.square = op(x){x*x;};")
        result = xy.eval("square(5);")
        print(f"   ✅ square(5) = {result}")
    except Exception as e:
        print(f"   ❌ Simple function failed: {e}")

def main():
    """Run all basic operation tests"""
    print("GrapaPy Basic Operations Test")
    print("=" * 50)
    
    try:
        test_basic_math()
        test_string_operations()
        test_array_operations()
        test_high_precision_math()
        test_simple_functions()
        
        print("\n🎉 Basic operations test completed!")
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main() 