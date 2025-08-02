#!/usr/bin/env python3
"""
Functional Methods Test for GrapaPy
Tests map, reduce, and filter operations with correct syntax
"""

import grapapy
import sys

def test_map_operations():
    """Test map operations with correct syntax"""
    print("=== Map Operations ===")
    
    xy = grapapy.grapa()
    
    # Test basic map
    try:
        result = xy.eval("[1, 2, 3, 4, 5].map(op(x) { x * 2; });")
        print(f"   ✅ Basic map: {result}")
    except Exception as e:
        print(f"   ❌ Basic map failed: {e}")
    
    # Test map with strings
    try:
        result = xy.eval("['john', 'jane', 'bob'].map(op(name) { name.upper(); });")
        print(f"   ✅ String map: {result}")
    except Exception as e:
        print(f"   ❌ String map failed: {e}")

def test_filter_operations():
    """Test filter operations with correct syntax"""
    print("\n=== Filter Operations ===")
    
    xy = grapapy.grapa()
    
    # Test basic filter
    try:
        result = xy.eval("[1, 2, 3, 4, 5, 6].filter(op(x) { x % 2 == 0; });")
        print(f"   ✅ Even filter: {result}")
    except Exception as e:
        print(f"   ❌ Even filter failed: {e}")
    
    # Test filter with strings
    try:
        result = xy.eval("['hello', '', 'world', '', 'test'].filter(op(line) { line.len() > 0; });")
        print(f"   ✅ Non-empty filter: {result}")
    except Exception as e:
        print(f"   ❌ Non-empty filter failed: {e}")

def test_reduce_operations():
    """Test reduce operations with correct syntax"""
    print("\n=== Reduce Operations ===")
    
    xy = grapapy.grapa()
    
    # Test basic reduce (sum)
    try:
        result = xy.eval("[1, 2, 3, 4, 5].reduce(op(acc, x) { acc += x; }, 0);")
        print(f"   ✅ Sum reduce: {result}")
    except Exception as e:
        print(f"   ❌ Sum reduce failed: {e}")
    
    # Test string reduce
    try:
        result = xy.eval("['hello', 'world', 'test'].reduce(op(acc, word) { acc += ' ' + word; }, '');")
        print(f"   ✅ String reduce: {result}")
    except Exception as e:
        print(f"   ❌ String reduce failed: {e}")

def test_method_chaining():
    """Test chaining map, filter, and reduce"""
    print("\n=== Method Chaining ===")
    
    xy = grapapy.grapa()
    
    # Test filter -> map -> reduce chain
    try:
        result = xy.eval("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10].filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(acc, x) { acc += x; }, 0);")
        print(f"   ✅ Filter->Map->Reduce chain: {result}")
    except Exception as e:
        print(f"   ❌ Method chaining failed: {e}")

def test_range_with_functional_methods():
    """Test range with functional methods"""
    print("\n=== Range with Functional Methods ===")
    
    xy = grapapy.grapa()
    
    # Test range with map
    try:
        result = xy.eval("(10).range(0,1).map(op(x) { x * x; });")
        print(f"   ✅ Range with map: {result}")
    except Exception as e:
        print(f"   ❌ Range with map failed: {e}")
    
    # Test range with filter
    try:
        result = xy.eval("(10).range(0,1).filter(op(x) { x % 2 == 0; });")
        print(f"   ✅ Range with filter: {result}")
    except Exception as e:
        print(f"   ❌ Range with filter failed: {e}")
    
    # Test range with reduce
    try:
        result = xy.eval("(10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);")
        print(f"   ✅ Range with reduce: {result}")
    except Exception as e:
        print(f"   ❌ Range with reduce failed: {e}")

def test_parallel_processing():
    """Test parallel processing with thread counts"""
    print("\n=== Parallel Processing ===")
    
    xy = grapapy.grapa()
    
    # Test parallel map with thread count
    try:
        result = xy.eval("(100).range(0,1).map(op(x) { x * x; }, 4);")
        print(f"   ✅ Parallel map (4 threads): {len(result)} items")
    except Exception as e:
        print(f"   ❌ Parallel map failed: {e}")
    
    # Test parallel filter with thread count
    try:
        result = xy.eval("(100).range(0,1).filter(op(x) { x % 2 == 0; }, 4);")
        print(f"   ✅ Parallel filter (4 threads): {len(result)} items")
    except Exception as e:
        print(f"   ❌ Parallel filter failed: {e}")

def test_edge_cases():
    """Test edge cases for functional methods"""
    print("\n=== Edge Cases ===")
    
    xy = grapapy.grapa()
    
    # Test empty array
    try:
        result = xy.eval("[].map(op(x) { x * 2; });")
        print(f"   ✅ Empty array map: {result}")
    except Exception as e:
        print(f"   ❌ Empty array map failed: {e}")
    
    # Test single element array
    try:
        result = xy.eval("[5].map(op(x) { x * 2; });")
        print(f"   ✅ Single element map: {result}")
    except Exception as e:
        print(f"   ❌ Single element map failed: {e}")

def main():
    """Run all functional method tests"""
    print("GrapaPy Functional Methods Test")
    print("=" * 50)
    
    try:
        test_map_operations()
        test_filter_operations()
        test_reduce_operations()
        test_method_chaining()
        test_range_with_functional_methods()
        test_parallel_processing()
        test_edge_cases()
        
        print("\n🎉 Functional methods test completed!")
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main() 