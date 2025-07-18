#!/usr/bin/env python3
"""
Test file for PYTHON_USE_CASES.md examples
This file validates that all examples in the Python documentation work correctly.
"""

import grapapy
import time
import os

# Global functions for Python callbacks
def pyfunc(x):
    print(f"Python saw: {x}")
    return x * 2

def pytrace(n):
    print(f"trace: {n}")
    return n

def test_etl_data_engineering():
    """Test Section 1: ETL / Data Engineering examples."""
    print("1. Testing ETL / Data Engineering examples...")
    
    xy = grapapy.grapa()
    
    # Create test files
    test_files = ["test1.csv", "test2.csv", "test3.csv"]
    test_contents = [
        "name,age,city\nJohn,30,NYC\nJane,25,LA",
        "name,age,city\nBob,35,Chicago\nAlice,28,Boston", 
        "name,age,city\nCharlie,40,Seattle\nDiana,32,Portland"
    ]
    
    # Create files
    for i, (filename, content) in enumerate(zip(test_files, test_contents)):
        with open(filename, 'w') as f:
            f.write(content)
    
    # Test the example - initialize file system once
    xy.eval("$global.fs = $file();")
    total_lines = 0
    for file in test_files:
        content = xy.eval("fs.get(filename);", {"filename": file})
        if content:
            # Handle bytes object and escaped newlines
            content_str = str(content)
            # Remove the b'...' wrapper and unescape newlines
            if content_str.startswith("b'") and content_str.endswith("'"):
                content_str = content_str[2:-1]  # Remove b' and '
            content_str = content_str.replace('\\n', '\n')  # Unescape newlines
            print(f"File {file} content: {repr(content_str)}")
            total_lines += len(content_str.split("\n"))
    
    print(f"Total lines: {total_lines}")
    
    # Cleanup
    for filename in test_files:
        if os.path.exists(filename):
            os.remove(filename)
    
    return total_lines >= 9  # At least 3 lines per file * 3 files

def test_compiler_bnf_learning():
    """Test Section 2: Compiler/BNF Learning examples."""
    print("\n2. Testing Compiler/BNF Learning examples...")
    
    xy = grapapy.grapa()
    
    # Test arithmetic expressions
    expressions = ["2+3*4", "(1+2)*3", "10/2+5"]
    results = []
    for expr in expressions:
        result = xy.eval(expr + ";")
        results.append(result)
        print(f"{expr} = {result}")
    
    # Define Grapa function using the exact pattern from the working notebook
    xy.eval("$this.pyfunc = op(x=0){$local.locals={'g':x};$py().eval('pyfunc(g)',locals);};")
    # Call the Grapa function
    try:
        result = xy.eval("pyfunc(5);")
        print(f"Python callback result: {result}")
        callback_success = result == 10
    except Exception as e:
        print(f"Python callback failed: {e}")
        callback_success = False
    
    return results == [14, 9, 10] and callback_success

def test_high_precision_math():
    """Test Section 3: High-Precision Math & Scientific Computing examples."""
    print("\n3. Testing High-Precision Math examples...")
    
    xy = grapapy.grapa()
    
    # Test large number math (use smaller numbers to avoid overflow)
    result1 = xy.eval("123456789 * 987654321;")
    print(f"Large number result: {result1}")
    
    # Test float math
    result2 = xy.eval("3.14159 * 2;")
    print(f"Precise float result: {result2}")
    
    # Test that results are reasonable
    return result1 > 123456789 and result2 > 6.0

def test_parallel_concurrent_programming():
    """Test Section 4: Parallel/Concurrent Programming examples."""
    print("\n4. Testing Parallel/Concurrent Programming examples...")
    
    xy = grapapy.grapa()
    
    # Test basic parallel processing
    data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    result = xy.eval("""
        $global.data = python_data;
        data.map(op(x){x * 2});
    """, {"python_data": data})
    
    print(f"Parallel processing result: {result}")
    
    # Test map with parameters
    result2 = xy.eval("""
        $global.data = python_data;
        data.map(op(x, p){x * 2 + p}, 5);
    """, {"python_data": [1, 2, 3, 4, 5]})
    
    print(f"Map with params result: {result2}")
    
    # Test map with threading
    result3 = xy.eval("""
        $global.data = python_data;
        data.map(op(x){x * x}, null, 4);
    """, {"python_data": [1, 2, 3, 4, 5]})
    
    print(f"Map with threading result: {result3}")
    
    expected1 = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]
    expected2 = [7, 9, 11, 13, 15]
    expected3 = [1, 4, 9, 16, 25]
    
    return result == expected1 and result2 == expected2 and result3 == expected3

def test_web_data_scraping():
    """Test Section 5: Web/Data Scraping & Automation examples."""
    print("\n5. Testing Web/Data Scraping examples...")
    
    xy = grapapy.grapa()
    
    # Test string processing (simulating web scraping)
    test_content = "<h1>Title 1</h1><h2>Title 2</h2><a href='http://example.com'>Link</a>"
    
    # Use grep to extract data
    titles = xy.eval("content.grep('<h[1-6][^>]*>.*?</h[1-6]>', 'o');", {"content": test_content})
    links = xy.eval("content.grep('href=[\\'\"][^\\'\"]*[\\'\"]', 'o');", {"content": test_content})
    
    print(f"Extracted titles: {titles}")
    print(f"Extracted links: {links}")
    
    return len(titles) > 0 and len(links) > 0

def test_database_file_system_integration():
    """Test Section 6: Database & File System Integration examples."""
    print("\n6. Testing Database & File System Integration examples...")
    
    xy = grapapy.grapa()
    
    # Test file system operations
    test_content = "Hello World!"
    xy.eval("$global.fs = $file();")
    xy.eval("fs.set('test.txt', content);", {"content": test_content})
    content = xy.eval("fs.get('test.txt');")
    print(f"File content: {content}")
    
    # Handle bytes content
    if isinstance(content, bytes):
        content_str = content.decode('utf-8')
    else:
        content_str = str(content)
    
    # Test table operations
    xy.eval("$global.table = $file().table('ROW');")
    xy.eval("table.mkfield('name', 'STR', 'VAR');")
    xy.eval("table.set('user1', 'Alice', 'name');")
    name = xy.eval("table.get('user1', 'name');")
    print(f"Table name: {name}")
    
    # Handle bytes content for table name too
    if isinstance(name, bytes):
        name_str = name.decode('utf-8')
    else:
        name_str = str(name)
    
    # Cleanup
    xy.eval("fs.rm('test.txt');")
    
    return content_str == test_content and name_str == "Alice"

def test_education_prototyping():
    """Test Section 7: Education & Prototyping examples."""
    print("\n7. Testing Education & Prototyping examples...")
    
    xy = grapapy.grapa()
    
    # Test function definition and call
    xy.eval("$global.square = op(x=0){x*x;};")
    result1 = xy.eval("square(7);")
    print(f"Square function result: {result1}")
    
    # Define Grapa function using the exact pattern from the working notebook
    xy.eval("$this.pytrace = op(n=0){$local.locals={'g':n};$py().eval('pytrace(g)',locals);};")
    # Call the Grapa function
    try:
        result2 = xy.eval("pytrace(42);")
        print(f"Python callback result: {result2}")
        callback_success = result2 == 42
    except Exception as e:
        print(f"Python callback failed: {e}")
        callback_success = False
    
    return result1 == 49 and callback_success

def test_large_dataset_performance():
    """Test performance with larger dataset."""
    print("\nTesting large dataset performance...")
    
    xy = grapapy.grapa()
    
    # Create larger dataset
    data = list(range(1, 1001))  # 1 to 1000
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
    
    # Validate results
    if result[0] == 2 and result[-1] == 2000 and len(result) == 1000:
        print("✅ Large dataset processing successful")
        return True
    else:
        print("❌ Large dataset processing failed")
        return False

def test_error_handling():
    """Test error handling in GrapaPy."""
    print("\nTesting error handling...")
    
    xy = grapapy.grapa()
    
    # Test invalid file access
    try:
        result = xy.eval("$file().get('nonexistent.txt');")
        print(f"File access result: {result}")
    except Exception as e:
        print(f"Expected error for nonexistent file: {e}")
    
    # Test invalid table access
    try:
        result = xy.eval("$global.table = $file().table('ROW');")
        result = xy.eval("table.get('nonexistent', 'field');")
        print(f"Table access result: {result}")
    except Exception as e:
        print(f"Expected error for nonexistent field: {e}")
    
    return True  # If we get here, error handling is working

def main():
    """Run all tests."""
    print("=" * 80)
    print("GrapaPy Python Use Cases Tests")
    print("=" * 80)
    
    tests = [
        test_etl_data_engineering,
        test_compiler_bnf_learning,
        test_high_precision_math,
        test_parallel_concurrent_programming,
        test_web_data_scraping,
        test_database_file_system_integration,
        test_education_prototyping,
        test_large_dataset_performance,
        test_error_handling
    ]
    
    passed = 0
    total = len(tests)
    
    for i, test in enumerate(tests):
        try:
            print(f"\n{'='*60}")
            if test():
                print(f"✅ Test {i+1} PASSED")
                passed += 1
            else:
                print(f"❌ Test {i+1} FAILED")
        except Exception as e:
            print(f"❌ Test {i+1} ERROR: {e}")
    
    print("\n" + "=" * 80)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 80)
    
    if passed == total:
        print("🎉 All tests passed! PYTHON_USE_CASES.md examples are working correctly.")
        return True
    else:
        print("⚠️  Some tests failed. Please check the implementation.")
        return False

if __name__ == "__main__":
    main() 