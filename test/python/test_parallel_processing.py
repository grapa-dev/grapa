#!/usr/bin/env python3
"""
Test file to validate the corrected parallel processing example for GrapaPy.
This tests the .map() functionality with proper Grapa syntax.
"""

import grapapy
import time

def test_parallel_processing():
    """Test the corrected parallel processing example."""
    print("Testing GrapaPy parallel processing with .map()...")
    
    xy = grapapy.grapa()
    
    # Create array in Python
    data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    print(f"Input data: {data}")
    
    # Pass array to Grapa and process in parallel with a single eval call
    result = xy.eval("""
        $global.data = python_data;
        data.map(op(x){x * 2});
    """, {"python_data": data})
    
    print(f"Result: {result}")
    
    # Validate the result
    expected = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]
    if result == expected:
        print("✅ Test PASSED: Result matches expected output")
        return True
    else:
        print(f"❌ Test FAILED: Expected {expected}, got {result}")
        return False

def test_map_with_parameters():
    """Test .map() with additional parameters."""
    print("\nTesting .map() with parameters...")
    
    xy = grapapy.grapa()
    
    data = [1, 2, 3, 4, 5]
    print(f"Input data: {data}")
    
    # Test with additional parameter
    result = xy.eval("""
        $global.data = python_data;
        data.map(op(x, p){x * 2 + p}, 5);
    """, {"python_data": data})
    
    print(f"Result (x * 2 + 5): {result}")
    
    # Validate the result
    expected = [7, 9, 11, 13, 15]  # (1*2+5), (2*2+5), etc.
    if result == expected:
        print("✅ Test PASSED: Result matches expected output")
        return True
    else:
        print(f"❌ Test FAILED: Expected {expected}, got {result}")
        return False

def test_map_with_threading():
    """Test .map() with threading specification."""
    print("\nTesting .map() with threading...")
    
    xy = grapapy.grapa()
    
    data = [1, 2, 3, 4, 5]
    print(f"Input data: {data}")
    
    # Test with threading specification (4 threads)
    result = xy.eval("""
        $global.data = python_data;
        data.map(op(x){x * x}, null, 4);
    """, {"python_data": data})
    
    print(f"Result (x * x with 4 threads): {result}")
    
    # Validate the result
    expected = [1, 4, 9, 16, 25]  # squares
    if result == expected:
        print("✅ Test PASSED: Result matches expected output")
        return True
    else:
        print(f"❌ Test FAILED: Expected {expected}, got {result}")
        return False

def test_large_dataset():
    """Test with a larger dataset to see parallel processing in action."""
    print("\nTesting with larger dataset...")
    
    xy = grapapy.grapa()
    
    # Create larger dataset
    data = list(range(1, 101))  # 1 to 100
    print(f"Processing {len(data)} items...")
    
    start_time = time.time()
    
    result = xy.eval("""
        $global.data = python_data;
        data.map(op(x){x * 2});
    """, {"python_data": data})
    
    end_time = time.time()
    processing_time = end_time - start_time
    
    print(f"Processed {len(result)} items in {processing_time:.4f} seconds")
    print(f"First 10 results: {result[:10]}")
    print(f"Last 10 results: {result[-10:]}")
    
    # Validate first and last few results
    if result[0] == 2 and result[-1] == 200 and len(result) == 100:
        print("✅ Test PASSED: Large dataset processing successful")
        return True
    else:
        print("❌ Test FAILED: Large dataset processing failed")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("GrapaPy Parallel Processing Tests")
    print("=" * 60)
    
    tests = [
        test_parallel_processing,
        test_map_with_parameters,
        test_map_with_threading,
        test_large_dataset
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"❌ Test FAILED with exception: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("🎉 All tests passed! The parallel processing example is working correctly.")
        return True
    else:
        print("⚠️  Some tests failed. Please check the implementation.")
        return False

if __name__ == "__main__":
    main() 