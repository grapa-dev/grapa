# Python Integration

Grapa provides seamless Python integration through the GrapaPy extension, allowing you to execute Grapa code from Python applications and call Python functions from Grapa.

## Installation

Install GrapaPy from PyPI:

### Platform Support

GrapaPy supports the following platforms:
- **Windows**: AMD64 (x86_64) - Requires Visual Studio Build Tools or Visual Studio
- **macOS**: AMD64 (x86_64) and ARM64 (Apple Silicon) - Requires Xcode Command Line Tools  
- **Linux**: AMD64 (x86_64) and ARM64 - Requires GCC and development libraries

### Installation Commands

```bash
# Windows
pip install grapapy

# macOS/Linux
pip3 install grapapy
```

**Note**: The installation process compiles a C++ extension, so you'll need the appropriate development tools for your platform. On Windows, this typically means having Visual Studio Build Tools installed.

## Basic Usage

### Import and Initialize

```python
import grapapy

# Create a Grapa instance
xy = grapapy.grapa()

# Execute Grapa code
result = xy.eval("2 + 2;")
print(result)  # Output: 4
```

### Basic Operations

```python
# Mathematical operations
result = xy.eval("10 * 5;")
print(result)  # Output: 50

# String operations
result = xy.eval("'Hello ' + 'World';")
print(result)  # Output: Hello World

# Array operations
result = xy.eval("[1, 2, 3, 4, 5].len();")
print(result)  # Output: 5
```

## Functional Programming Methods

GrapaPy supports functional programming methods with specific syntax requirements:

### Map Operations

```python
# Transform array elements
result = xy.eval("[1, 2, 3, 4, 5].map(op(x) { x * 2; });")
print(result)  # Output: [2, 4, 6, 8, 10]

# Transform strings
result = xy.eval("['john', 'jane', 'bob'].map(op(name) { name.upper(); });")
print(result)  # Output: ['JOHN', 'JANE', 'BOB']
```

### Filter Operations

```python
# Filter even numbers
result = xy.eval("[1, 2, 3, 4, 5, 6].filter(op(x) { x % 2 == 0; });")
print(result)  # Output: [2, 4, 6]

# Filter non-empty strings
result = xy.eval("['hello', '', 'world', '', 'test'].filter(op(line) { line.len() > 0; });")
print(result)  # Output: ['hello', 'world', 'test']
```

### Reduce Operations

**⚠️ CRITICAL:** For reduce operations, you must use `+=` (compound assignment), not `+` (addition):

> **Design Philosophy:** Unlike most languages that expect return-based accumulation (`return acc + x`), Grapa's `reduce` requires explicit mutation (`acc += x`). This design doesn't assume the nature of the data being reduced and allows for complex operations like network calls, file I/O, or database queries within the reduce callback. The imperative approach enables sophisticated stateful operations.

```python
# ✅ CORRECT - Use += for compound assignment
result = xy.eval("[1, 2, 3, 4, 5].reduce(op(acc, x) { acc += x; }, 0);")
print(result)  # Output: 15

# ❌ WRONG - Using + returns 0
result = xy.eval("[1, 2, 3, 4, 5].reduce(op(acc, x) { acc + x; }, 0);")
print(result)  # Output: 0 (WRONG!)

# String concatenation with reduce
result = xy.eval("['hello', 'world', 'test'].reduce(op(acc, word) { acc += ' ' + word; }, '');")
print(result)  # Output: " hello world test"
```

### Method Chaining

```python
# Filter -> Map -> Reduce chain
result = xy.eval("""
[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    .filter(op(x) { x % 2 == 0; })
    .map(op(x) { x * x; })
    .reduce(op(acc, x) { acc += x; }, 0);
""")
print(result)  # Output: 220
```

## File Operations

GrapaPy provides file system operations with proper byte handling:

```python
# Initialize file system
xy.eval("$global.fs = $file();")

# Create and write files
xy.eval("fs.set('test.txt', 'Hello from GrapaPy!\\nThis is a test file.\\n');")

# Read files (returns bytes, needs decoding)
content = xy.eval("fs.get('test.txt');")
if isinstance(content, bytes):
    content = content.decode('utf-8')
print(content)

# Get file info
info = xy.eval("fs.info('test.txt');")
print(info)  # Output: {'$TYPE': 'FILE', '$BYTES': 41}

# Remove files
xy.eval("fs.remove('test.txt');")
```

## High Precision Math

GrapaPy supports high-precision arithmetic:

```python
# Large number calculations
result = xy.eval("12345678901234567890 * 98765432109876543210;")
print(result)  # Output: Large number result

# High precision division
result = xy.eval("1000000000000000000000 / 3;")
print(result)  # Precise decimal result
```

## Python Callbacks

Call Python functions from Grapa:

```python
def python_function(name):
    return f"Hello from Python, {name}!"

# Register Python function in Grapa
xy.eval("$global.python_greeting = python_function;", {"python_function": python_function})

# Call Python function from Grapa
result = xy.eval("python_greeting('Alice');")
print(result)  # Output: Hello from Python, Alice!
```

## Error Handling

Handle errors and edge cases:

```python
# Check for error responses
result = xy.eval("fs.get('nonexistent.txt');")
if isinstance(result, dict) and result.get("error") == -1:
    print("File not found")

# Handle bytes vs strings
content = xy.eval("fs.get('test.txt');")
if isinstance(content, bytes):
    content = content.decode('utf-8')
```

## Validation and Testing

### Running Validation Tests

GrapaPy includes comprehensive validation tests in the `test/grapapy_validation/` directory:

```bash
# Run basic operations test
python test_basic_operations.py

# Run functional methods test
python test_functional_methods.py

# Run file operations test
python test_file_operations.py

# Run complete validation suite
python run_validation.py
```

### Known Issues and Workarounds

1. **Reduce Operations**: Always use `+=` (compound assignment), not `+` (addition)
2. **File Content**: File content is returned as bytes and needs `.decode('utf-8')`
3. **Complex Recursion**: Avoid complex recursive functions (may hang)
4. **Error Responses**: Some operations return `{"error":-1}` - check for this pattern

### Platform Support

- **Windows**: Use `pip install grapapy==0.0.49`
- **Linux**: Use `pip3 install grapapy==0.0.49`
- **macOS**: Use `pip3 install grapapy==0.0.49`

All platforms support the same functionality with the same syntax requirements.

## Best Practices

1. **Always use `+=` for reduce operations**
2. **Decode file content**: `content.decode('utf-8')`
3. **Check for error responses**: `{"error":-1}`
4. **Test with small datasets first**
5. **Use proper error handling for file operations**
6. **Avoid complex recursion in Grapa code**

## Troubleshooting

### Common Issues

1. **Reduce returns 0**: Check if using `+=` instead of `+`
2. **File content issues**: Always decode bytes to strings
3. **Import errors**: Try `pip install grapapy==0.0.49 --force-reinstall`
4. **Hanging operations**: Avoid complex recursion, use iterative approaches

### Debugging Commands

```python
import grapapy
xy = grapapy.grapa()

# Test basic functionality
print(xy.eval("2 + 2;"))  # Should be 4

# Test reduce syntax
print(xy.eval("[1,2,3].reduce(op(acc, x) { acc += x; }, 0);"))  # Should be 6

# Test file operations
xy.eval("$global.fs = $file();")
xy.eval("fs.set('test.txt', 'Hello');")
content = xy.eval("fs.get('test.txt');")
if isinstance(content, bytes):
    content = content.decode('utf-8')
print(content)  # Should be 'Hello'
```

## See Also

- [GrapaPy Validation Guide](../../test/grapapy_validation/PLATFORM_VALIDATION_GUIDE.md)
- [Basic Syntax](../syntax/basic_syntax.md)
- [Functional Methods](../operators/loop.md)
- [API Reference](../api_reference.md) 