#!/usr/bin/env python3
"""
File Operations Test for GrapaPy
Tests file system operations with proper byte handling
"""

import grapapy
import sys

def test_basic_file_operations():
    """Test basic file operations"""
    print("=== Basic File Operations ===")
    
    xy = grapapy.grapa()
    
    # Initialize file system in global namespace
    xy.eval("$global.fs = $file();")
    
    # Test file creation
    try:
        xy.eval("fs.set('test.txt', 'Hello from GrapaPy!\\nThis is a test file.\\n');")
        print("   ✅ File created successfully")
    except Exception as e:
        print(f"   ❌ File creation failed: {e}")
        return
    
    # Test file reading (handle bytes)
    try:
        content = xy.eval("fs.get('test.txt');")
        if isinstance(content, bytes):
            content = content.decode('utf-8')
        print(f"   ✅ File content: {repr(content)}")
    except Exception as e:
        print(f"   ❌ File reading failed: {e}")
    
    # Test file info
    try:
        info = xy.eval("fs.info('test.txt');")
        print(f"   ✅ File info: {info}")
    except Exception as e:
        print(f"   ❌ File info failed: {e}")

def test_file_with_data():
    """Test file operations with structured data"""
    print("\n=== File with Data ===")
    
    xy = grapapy.grapa()
    
    # Create a CSV-like file
    csv_data = "name,age,city\nAlice,25,New York\nBob,30,Los Angeles\nCharlie,35,Chicago\n"
    
    try:
        xy.eval("fs.set('people.csv', csv_data);", {"csv_data": csv_data})
        print("   ✅ CSV file created")
    except Exception as e:
        print(f"   ❌ CSV file creation failed: {e}")
        return
    
    # Read and verify content
    try:
        content = xy.eval("fs.get('people.csv');")
        if isinstance(content, bytes):
            content = content.decode('utf-8')
        print(f"   ✅ CSV content: {repr(content)}")
    except Exception as e:
        print(f"   ❌ CSV file reading failed: {e}")

def test_file_cleanup():
    """Test file removal"""
    print("\n=== File Cleanup ===")
    
    xy = grapapy.grapa()
    
    try:
        xy.eval("fs.remove('test.txt');")
        print("   ✅ test.txt removed")
    except Exception as e:
        print(f"   ❌ test.txt removal failed: {e}")
    
    try:
        xy.eval("fs.remove('people.csv');")
        print("   ✅ people.csv removed")
    except Exception as e:
        print(f"   ❌ people.csv removal failed: {e}")

def test_file_operations_with_grep():
    """Test file operations combined with grep"""
    print("\n=== File Operations with Grep ===")
    
    xy = grapapy.grapa()
    
    # Create a multi-line file
    multi_line_data = "apple\nbanana\ncherry\napple pie\norange\npear\napple cider\n"
    
    try:
        xy.eval("fs.set('fruits.txt', multi_line_data);", {"multi_line_data": multi_line_data})
        print("   ✅ Multi-line file created")
    except Exception as e:
        print(f"   ❌ Multi-line file creation failed: {e}")
        return
    
    # Read file and process with grep
    try:
        content = xy.eval("fs.get('fruits.txt');")
        if isinstance(content, bytes):
            content = content.decode('utf-8')
        
        # Use grep on the content
        apple_lines = xy.eval("content.grep('apple');", {"content": content})
        print(f"   ✅ Lines containing 'apple': {apple_lines}")
    except Exception as e:
        print(f"   ❌ File grep processing failed: {e}")
    
    # Cleanup
    try:
        xy.eval("fs.remove('fruits.txt');")
        print("   ✅ fruits.txt removed")
    except Exception as e:
        print(f"   ❌ fruits.txt removal failed: {e}")

def test_error_handling():
    """Test error handling for file operations"""
    print("\n=== Error Handling ===")
    
    xy = grapapy.grapa()
    
    # Test reading non-existent file
    try:
        result = xy.eval("fs.get('nonexistent.txt');")
        print(f"   ⚠️  Non-existent file result: {result}")
    except Exception as e:
        print(f"   ✅ Non-existent file handled: {e}")
    
    # Test file info on non-existent file
    try:
        result = xy.eval("fs.info('nonexistent.txt');")
        print(f"   ⚠️  Non-existent file info: {result}")
    except Exception as e:
        print(f"   ✅ Non-existent file info handled: {e}")

def main():
    """Run all file operation tests"""
    print("GrapaPy File Operations Test")
    print("=" * 50)
    
    try:
        test_basic_file_operations()
        test_file_with_data()
        test_file_operations_with_grep()
        test_error_handling()
        test_file_cleanup()
        
        print("\n🎉 File operations test completed!")
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main() 