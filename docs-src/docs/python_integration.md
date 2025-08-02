# Python Integration

GrapaPy provides a Python interface to the Grapa language, allowing you to execute Grapa code from Python applications.

## Installation

```bash
pip install grapapy
```

## Basic Usage

```python
import grapapy

# Create a Grapa instance
xy = grapapy.grapa()

# Execute Grapa code
result = xy.eval("2 + 2;")
print(result)  # 4

# Pass variables to Grapa
variables = {"name": "World", "numbers": [1, 2, 3, 4, 5]}
result = xy.eval("'Hello ' + name + '!';", variables)
print(result)  # Hello World!
```

## Error Handling

### Basic Error Detection

GrapaPy operations may return `{"error":-1}` for failed operations. Use proper error handling:

```python
import grapapy

xy = grapapy.grapa()

# Basic error handling
result = xy.eval("fs.get('nonexistent.txt');")
if isinstance(result, dict) and result.get("error") == -1:
    print("Operation failed with error -1")
else:
    print(f"Operation successful: {result}")
```

### Advanced Error Handling with Utilities

For comprehensive error handling, use the provided utilities:

```python
import grapapy
from error_handling_utils import GrapaPyErrorHandler

xy = grapapy.grapa()
handler = GrapaPyErrorHandler(xy)

# Safe evaluation with fallback
result = handler.safe_eval("fs.get('nonexistent.txt');", fallback="File not found")
print(result)  # File not found

# Safe file operations
content = handler.safe_file_operation("get", "test.txt")
if content is None:
    print("File operation failed")
else:
    print(f"File content: {content}")

# Retry mechanism
def operation():
    return xy.eval("fs.get('test.txt');")

result = handler.retry_operation(operation, max_retries=3)
```

### Error Reporting

Generate comprehensive error reports:

```python
from error_handling_utils import create_error_report

# After performing operations
error_report = create_error_report(handler)
print(error_report)
```

## Critical Syntax Issues

### Reduce Syntax

**IMPORTANT**: Grapa's reduce function requires compound assignment (`+=`), not simple addition (`+`):

```python
# ✅ CORRECT - Use compound assignment
result = xy.eval("""
arr = [1, 2, 3, 4, 5];
arr.reduce(op(acc, x) { acc += x; }, 0);
""")
print(result)  # 15

# ❌ WRONG - Simple addition returns 0
result = xy.eval("""
arr = [1, 2, 3, 4, 5];
arr.reduce(op(acc, x) { acc + x; }, 0);
""")
print(result)  # 0
```

### File Content Handling

File content is returned as bytes and needs explicit decoding:

```python
# Get file content
content = xy.eval("fs.get('test.txt');")
if isinstance(content, bytes):
    content = content.decode('utf-8')
print(content)
```

## File Operations

### Basic File Operations

```python
import grapapy

xy = grapapy.grapa()

# Initialize file system in global namespace
xy.eval("$global.fs = $file();")

# Create a file
xy.eval("fs.set('test.txt', 'Hello World');")

# Read file content
content = xy.eval("fs.get('test.txt');")
if isinstance(content, bytes):
    content = content.decode('utf-8')
print(content)  # Hello World

# Get file information
info = xy.eval("fs.info('test.txt');")
print(info)  # {'$TYPE': 'FILE', '$BYTES': 11}

# Remove file
xy.eval("fs.remove('test.txt');")
```

### File Operations with Error Handling

```python
from error_handling_utils import GrapaPyErrorHandler

xy = grapapy.grapa()
handler = GrapaPyErrorHandler(xy)

# Initialize file system
handler.safe_eval("$global.fs = $file();")

# Safe file operations
content = handler.safe_file_operation("set", "test.txt", "Hello World")
if content is not None:
    print("File created successfully")

read_content = handler.safe_file_operation("get", "test.txt")
if read_content:
    print(f"File content: {read_content}")

# Handle non-existent files gracefully
nonexistent = handler.safe_file_operation("get", "nonexistent.txt")
if nonexistent is None:
    print("File not found - handled gracefully")
```

## Functional Programming

### Map, Filter, and Reduce

```python
import grapapy

xy = grapapy.grapa()

# Map operation
result = xy.eval("""
numbers = [1, 2, 3, 4, 5];
numbers.map(op(x) { x * 2; });
""")
print(result)  # [2, 4, 6, 8, 10]

# Filter operation
result = xy.eval("""
numbers = [1, 2, 3, 4, 5, 6];
numbers.filter(op(x) { x % 2 == 0; });
""")
print(result)  # [2, 4, 6]

# Reduce operation (use += syntax)
result = xy.eval("""
numbers = [1, 2, 3, 4, 5];
numbers.reduce(op(acc, x) { acc += x; }, 0);
""")
print(result)  # 15
```

## Database Operations

### Table Operations

```python
import grapapy

xy = grapapy.grapa()

# Initialize database
xy.eval("$global.db = $file().table('ROW');")

# Create table schema
xy.eval("""
db.mkfield('name', 'STR', 'VAR');
db.mkfield('age', 'INT');
db.mkfield('salary', 'FLOAT', 'FIX', 8);
""")

# Insert data
xy.eval("db.set('user1', 'John Doe', 'name');")
xy.eval("db.set('user1', 30, 'age');")
xy.eval("db.set('user1', 75000.50, 'salary');")

# Retrieve data
name = xy.eval("db.get('user1', 'name').str();")
age = xy.eval("db.get('user1', 'age').int();")
salary = xy.eval("db.get('user1', 'salary').float();")

print(f"Name: {name}, Age: {age}, Salary: {salary}")
```

## Performance Considerations

### Large Dataset Processing

```python
import grapapy
from error_handling_utils import GrapaPyErrorHandler

xy = grapapy.grapa()
handler = GrapaPyErrorHandler(xy)

# Process large datasets with error handling
def process_large_dataset():
    try:
        # Create large array
        result = xy.eval("""
        large_array = [];
        i = 1;
        while (i <= 10000) {
            large_array.push(i);
            i = i + 1;
        }
        large_array.reduce(op(acc, x) { acc += x; }, 0);
        """)
        return result
    except Exception as e:
        print(f"Processing failed: {e}")
        return None

result = handler.retry_operation(process_large_dataset, max_retries=3)
if result:
    print(f"Sum of 1 to 10000: {result}")
```

## Troubleshooting

### Common Issues

1. **Reduce Syntax**: Always use `acc += x;` not `acc + x;`
2. **File Content**: Decode bytes explicitly: `content.decode('utf-8')`
3. **Error Responses**: Check for `{"error":-1}` pattern
4. **Namespace Management**: Use `$global` for persistent objects

### Error Handling Best Practices

1. **Use Error Handler**: Always use `GrapaPyErrorHandler` for production code
2. **Provide Fallbacks**: Use fallback values for failed operations
3. **Retry Logic**: Implement retry mechanisms for transient failures
4. **Logging**: Enable logging to track error patterns
5. **Graceful Degradation**: Handle errors without crashing the application

### Debugging Tips

```python
import grapapy
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)

xy = grapapy.grapa()

# Test basic functionality
result = xy.eval("2 + 2;")
print(f"Basic test: {result}")

# Test file operations
xy.eval("$global.fs = $file();")
file_result = xy.eval("fs.set('debug.txt', 'test');")
print(f"File operation: {file_result}")

# Check for errors
if isinstance(file_result, dict) and file_result.get("error") == -1:
    print("File operation failed")
```

## Integration Examples

### Web Application Integration

```python
from flask import Flask, request, jsonify
import grapapy
from error_handling_utils import GrapaPyErrorHandler

app = Flask(__name__)
xy = grapapy.grapa()
handler = GrapaPyErrorHandler(xy)

@app.route('/execute', methods=['POST'])
def execute_grapa():
    try:
        code = request.json.get('code')
        variables = request.json.get('variables', {})
        
        result = handler.safe_eval(code, variables, fallback="Execution failed")
        
        return jsonify({
            'success': True,
            'result': result,
            'errors': handler.get_error_summary()
        })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

if __name__ == '__main__':
    app.run(debug=True)
```

### Data Processing Pipeline

```python
import grapapy
from error_handling_utils import GrapaPyErrorHandler

def process_data_with_grapa(data_list):
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Initialize processing
    handler.safe_eval("$global.processor = $file();")
    
    results = []
    for i, data in enumerate(data_list):
        try:
            # Process each item
            result = handler.safe_eval(f"""
            input_data = {data};
            processed = input_data.map(op(x) {{ x * 2; }});
            processed.reduce(op(acc, x) {{ acc += x; }}, 0);
            """, fallback=f"Processing failed for item {i}")
            
            results.append(result)
            
        except Exception as e:
            print(f"Error processing item {i}: {e}")
            results.append(None)
    
    return results

# Usage
data = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
results = process_data_with_grapa(data)
print(f"Processing results: {results}")
```

This comprehensive guide covers all aspects of GrapaPy integration with proper error handling for production use. 