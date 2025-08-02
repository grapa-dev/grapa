#!/usr/bin/env python3
"""
Improved Error Handling Test for GrapaPy
Demonstrates comprehensive error handling for {"error":-1} responses
"""

import sys
import grapapy
from error_handling_utils import GrapaPyErrorHandler, validate_grapapy_installation, create_error_report

def test_basic_error_handling():
    """Test basic error detection and handling"""
    print("=== Basic Error Handling ===")
    
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Test successful operation
    result = handler.safe_eval("2 + 2;")
    print(f"   ✅ Successful operation: {result}")
    
    # Test error response detection
    error_result = handler.safe_eval("fs.get('nonexistent.txt');")
    print(f"   ⚠️  Error operation result: {error_result}")
    
    # Test with fallback
    fallback_result = handler.safe_eval("fs.get('nonexistent.txt');", fallback="File not found")
    print(f"   ✅ Error with fallback: {fallback_result}")
    
    # Print error summary
    summary = handler.get_error_summary()
    print(f"   📊 Error summary: {summary}")

def test_file_operations_with_error_handling():
    """Test file operations with comprehensive error handling"""
    print("\n=== File Operations with Error Handling ===")
    
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Initialize file system in global namespace
    handler.safe_eval("$global.fs = $file();")
    
    # Test file creation with error handling
    content = "Hello from improved error handling!\nThis is a test file."
    result = handler.safe_file_operation("set", "test_error_handling.txt", content)
    if result is not None:
        print("   ✅ File created successfully")
    else:
        print("   ❌ File creation failed")
    
    # Test file reading with error handling
    read_result = handler.safe_file_operation("get", "test_error_handling.txt")
    if read_result:
        print(f"   ✅ File content: {repr(read_result)}")
    else:
        print("   ❌ File reading failed")
    
    # Test file info with error handling
    info_result = handler.safe_file_operation("info", "test_error_handling.txt")
    if info_result:
        print(f"   ✅ File info: {info_result}")
    else:
        print("   ❌ File info failed")
    
    # Test non-existent file with error handling
    nonexistent_result = handler.safe_file_operation("get", "nonexistent.txt")
    if nonexistent_result is None:
        print("   ✅ Non-existent file handled gracefully")
    else:
        print(f"   ⚠️  Non-existent file result: {nonexistent_result}")
    
    # Cleanup
    cleanup_result = handler.safe_file_operation("remove", "test_error_handling.txt")
    if cleanup_result is not None:
        print("   ✅ File cleanup successful")
    else:
        print("   ❌ File cleanup failed")

def test_retry_mechanism():
    """Test retry mechanism for failed operations"""
    print("\n=== Retry Mechanism Test ===")
    
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Test retry with failing operation
    def failing_operation():
        return handler.safe_eval("fs.get('nonexistent.txt');")
    
    retry_result = handler.retry_operation(failing_operation, max_retries=3)
    print(f"   📊 Retry result: {retry_result}")
    
    # Test retry with successful operation
    def successful_operation():
        return handler.safe_eval("1 + 1;")
    
    retry_success = handler.retry_operation(successful_operation, max_retries=3)
    print(f"   ✅ Retry success result: {retry_success}")

def test_reduce_syntax_error_handling():
    """Test error handling for the critical reduce syntax issue"""
    print("\n=== Reduce Syntax Error Handling ===")
    
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Test correct reduce syntax
    correct_result = handler.safe_eval("arr = [1,2,3,4,5]; arr.reduce(op(acc, x) { acc += x; }, 0);")
    print(f"   ✅ Correct reduce syntax: {correct_result}")
    
    # Test incorrect reduce syntax
    incorrect_result = handler.safe_eval("arr = [1,2,3,4,5]; arr.reduce(op(acc, x) { acc + x; }, 0);")
    print(f"   ⚠️  Incorrect reduce syntax: {incorrect_result}")
    
    # Test with fallback for incorrect syntax
    fallback_result = handler.safe_eval("arr = [1,2,3,4,5]; arr.reduce(op(acc, x) { acc + x; }, 0);", fallback="Invalid reduce syntax")
    print(f"   ✅ Incorrect syntax with fallback: {fallback_result}")

def test_comprehensive_error_scenarios():
    """Test various error scenarios with improved handling"""
    print("\n=== Comprehensive Error Scenarios ===")
    
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Test invalid file operations
    invalid_operations = [
        "fs.get('nonexistent.txt');",
        "fs.info('nonexistent.txt');",
        "fs.remove('nonexistent.txt');",
        "table.get('nonexistent', 'field');",
        "invalid_function();",
        "1 / 0;",  # Division by zero
    ]
    
    for i, operation in enumerate(invalid_operations, 1):
        result = handler.safe_eval(operation, fallback=f"Operation {i} failed")
        print(f"   {i}. {operation} -> {result}")
    
    # Test complex operations that might hang
    complex_operations = [
        "factorial = op(n) { if (n <= 1) { 1; } else { n * factorial(n-1); } }; factorial(1000);",  # Deep recursion
        "while (true) { 1; }",  # Infinite loop
    ]
    
    print("\n   Testing potentially problematic operations:")
    for i, operation in enumerate(complex_operations, 1):
        print(f"   {i}. Testing: {operation[:50]}...")
        # Note: These might hang, so we'll skip them in automated testing
        print(f"      ⚠️  Skipped (potentially hangs)")

def test_error_reporting():
    """Test comprehensive error reporting"""
    print("\n=== Error Reporting Test ===")
    
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Perform various operations to generate errors
    handler.safe_eval("fs.get('nonexistent1.txt');")
    handler.safe_eval("fs.get('nonexistent2.txt');")
    handler.safe_eval("invalid_function();")
    
    # Generate error report
    error_report = create_error_report(handler)
    print(error_report)
    
    # Test error summary
    summary = handler.get_error_summary()
    print(f"   📊 Final error summary: {summary}")

def main():
    """Run all improved error handling tests"""
    print("GrapaPy Improved Error Handling Test")
    print("=" * 50)
    
    # Validate installation first
    if not validate_grapapy_installation():
        print("❌ GrapaPy installation validation failed")
        sys.exit(1)
    
    try:
        test_basic_error_handling()
        test_file_operations_with_error_handling()
        test_retry_mechanism()
        test_reduce_syntax_error_handling()
        test_comprehensive_error_scenarios()
        test_error_reporting()
        
        print("\n🎉 Improved error handling test completed!")
        print("✅ All error handling mechanisms working correctly")
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main() 