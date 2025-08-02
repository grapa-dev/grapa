#!/usr/bin/env python3
"""
Simple GrapaPy Examples
Testing various operations based on the documentation
"""

import grapapy

def test_basic_operations():
    """Test basic mathematical and string operations"""
    print("=== Basic Operations ===")
    
    xy = grapapy.grapa()
    
    # Simple math
    result = xy.eval("2 + 2;")
    print(f"2 + 2 = {result}")
    
    # String operations
    result = xy.eval("'Hello ' + 'World';")
    print(f"String concatenation: {result}")
    
    # Array operations
    result = xy.eval("[1, 2, 3, 4, 5].len();")
    print(f"Array length: {result}")
    
    # High precision math
    result = xy.eval("12345678901234567890 * 98765432109876543210;")
    print(f"High precision math: {result}")

def test_file_operations():
    """Test file system operations using global namespace"""
    print("\n=== File Operations ===")
    
    xy = grapapy.grapa()
    
    # Initialize file system in global namespace
    xy.eval("$global.fs = $file();")
    
    # Create a test file
    xy.eval("fs.set('test.txt', 'Hello from GrapaPy!\\nThis is a test file.\\n');")
    print("Created test.txt")
    
    # Read the file back
    content = xy.eval("fs.get('test.txt');")
    print(f"File content: {content}")
    
    # Get file info
    info = xy.eval("fs.info('test.txt');")
    print(f"File info: {info}")

def test_grep_operations():
    """Test grep operations on strings"""
    print("\n=== Grep Operations ===")
    
    xy = grapapy.grapa()
    
    # Create test data and search it
    test_data = "apple\nbanana\ncherry\napple pie\norange\npear\napple cider\n"
    
    # Simple grep
    result = xy.eval("data.grep('apple');", {"data": test_data})
    print(f"Grep for 'apple': {result}")
    
    # Grep with context
    result = xy.eval("data.grep('apple', 'A1B1');", {"data": test_data})
    print(f"Grep with context: {result}")

def test_function_definition():
    """Test defining and calling Grapa functions"""
    print("\n=== Function Definition ===")
    
    xy = grapapy.grapa()
    
    # Define a function in global namespace
    xy.eval("$global.square = op(x=0){x*x;};")
    
    # Call the function
    result = xy.eval("square(7);")
    print(f"square(7) = {result}")
    
    # Define another function
    xy.eval("$global.factorial = op(n=0){if(n<=1,1,n*factorial(n-1));};")
    result = xy.eval("factorial(5);")
    print(f"factorial(5) = {result}")

def test_python_callback():
    """Test Python functions being called from Grapa"""
    print("\n=== Python Callback ===")
    
    def pyfunc(x):
        print(f"Python function called with: {x}")
        return x * 2
    
    xy = grapapy.grapa()
    
    # Register Python function
    xy.eval("$this.pyfunc = op(x=0){$py().eval('pyfunc', {'x':x});};")
    
    # Call it from Grapa
    result = xy.eval("pyfunc(10);")
    print(f"Result from Python callback: {result}")

def test_table_operations():
    """Test database table operations"""
    print("\n=== Table Operations ===")
    
    xy = grapapy.grapa()
    
    # Create a table in global namespace
    xy.eval("$global.table = $file().table('ROW');")
    
    # Create fields
    xy.eval("table.mkfield('name', 'STR', 'VAR');")
    xy.eval("table.mkfield('age', 'INT', 'FIX', 4);")
    xy.eval("table.mkfield('city', 'STR', 'VAR');")
    
    # Add some data
    xy.eval("table.set('user1', 'Alice', 'name');")
    xy.eval("table.set('user1', 25, 'age');")
    xy.eval("table.set('user1', 'New York', 'city');")
    
    xy.eval("table.set('user2', 'Bob', 'name');")
    xy.eval("table.set('user2', 30, 'age');")
    xy.eval("table.set('user2', 'Los Angeles', 'city');")
    
    # Retrieve data
    name = xy.eval("table.get('user1', 'name');")
    age = xy.eval("table.get('user1', 'age');")
    city = xy.eval("table.get('user1', 'city');")
    
    print(f"User1: {name}, Age: {age}, City: {city}")

def test_array_operations():
    """Test array and list operations"""
    print("\n=== Array Operations ===")
    
    xy = grapapy.grapa()
    
    # Create and manipulate arrays
    xy.eval("$global.numbers = [1, 2, 3, 4, 5];")
    
    # Array length
    length = xy.eval("numbers.len();")
    print(f"Array length: {length}")
    
    # Array access
    first = xy.eval("numbers[0];")
    last = xy.eval("numbers[numbers.len()-1];")
    print(f"First element: {first}, Last element: {last}")
    
    # Array operations
    doubled = xy.eval("numbers.map(op(x){x*2;});")
    print(f"Doubled array: {doubled}")
    
    # Filter
    evens = xy.eval("numbers.filter(op(x){x%2==0;});")
    print(f"Even numbers: {evens}")

def cleanup():
    """Clean up test files"""
    print("\n=== Cleanup ===")
    
    xy = grapapy.grapa()
    
    # Remove test file
    xy.eval("fs.remove('test.txt');")
    print("Removed test.txt")

if __name__ == "__main__":
    print("GrapaPy Simple Examples")
    print("=" * 50)
    
    try:
        test_basic_operations()
        test_file_operations()
        test_grep_operations()
        test_function_definition()
        test_python_callback()
        test_table_operations()
        test_array_operations()
        cleanup()
        
        print("\n🎉 All tests completed successfully!")
        print("GrapaPy is working perfectly!")
        
    except Exception as e:
        print(f"\n❌ Error during testing: {e}")
        import traceback
        traceback.print_exc() 