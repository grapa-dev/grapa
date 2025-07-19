#!/usr/bin/env python3
"""
Python Integration Example
This file demonstrates how to use Grapa from Python
"""

import grapa

def basic_grapa_usage():
    """Demonstrate basic Grapa usage from Python"""
    print("Basic Grapa usage from Python:")
    
    # Create a Grapa instance
    g = grapa.Grapa()
    
    # Execute simple Grapa code
    result = g.execute('x = 42; x * 2')
    print(f"Result: {result}")
    
    # Execute more complex code
    code = """
    arr = [1, 2, 3, 4, 5];
    sum = arr.reduce((a, b) => a + b, 0);
    sum
    """
    result = g.execute(code)
    print(f"Array sum: {result}")

def file_operations():
    """Demonstrate file operations"""
    print("\nFile operations:")
    
    g = grapa.Grapa()
    
    # Create and write to a file
    code = """
    f = $file();
    f.create("python_test.txt");
    f.write("Hello from Python integration!\\n");
    f.write("Time: " + $sys().time() + "\\n");
    f.close();
    "File created successfully"
    """
    result = g.execute(code)
    print(f"File operation: {result}")

def grep_operations():
    """Demonstrate grep operations"""
    print("\nGrep operations:")
    
    g = grapa.Grapa()
    
    # Create a test file and search it
    code = """
    // Create test file
    f = $file();
    f.create("test_data.txt");
    f.write("apple\\nbanana\\ncherry\\napple pie\\n");
    f.close();
    
    // Search for lines containing 'apple'
    $grep("apple", "test_data.txt")
    """
    result = g.execute(code)
    print(f"Grep result: {result}")

if __name__ == "__main__":
    basic_grapa_usage()
    file_operations()
    grep_operations()
    print("\nPython integration example completed!") 