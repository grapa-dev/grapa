# Troubleshooting

## Critical Issues

This guide covers common issues encountered when working with Grapa and GrapaPy, along with their solutions and debugging techniques.

## Installation and Dependency Issues

### Python Extension Installation Failures

#### **"Missing system dependencies" Error**
**Problem**: `pip install grapapy` fails with dependency errors.

**Solutions**:

**Linux/macOS - Automatic:**
```bash
# Install Grapa with automatic dependency installation
python3 install-grapa-0.1.54.py --install-dependencies
```

**Linux/macOS - Manual:**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential python3-dev libssl-dev libx11-dev libxext-dev cmake

# CentOS/RHEL/Amazon Linux
sudo yum groupinstall 'Development Tools'
sudo yum install gcc-c++ python3-devel openssl-devel libX11-devel libXext-devel cmake

# macOS
xcode-select --install
```

**Windows:**
- Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) or Build Tools for Visual Studio 2022
- Ensure "Desktop development with C++" workload is selected

#### **Ubuntu 24.04+ "externally-managed-environment" Error**
**Problem**: `pip install grapapy` fails with externally-managed-environment error.

**Solution**:
```bash
# Use --break-system-packages flag
pip3 install grapapy --break-system-packages
```

#### **"undefined symbol" Error on Linux**
**Problem**: `ImportError: undefined symbol: _ZN17GrapaLibraryEvent17HasActiveTryBlockEP10GrapaNames`

**Solution**: This indicates an outdated distribution package. The issue is resolved in Grapa 0.1.54+ with updated static libraries.

### CMake Build Failures

#### **Windows: "Visual Studio Compiler Required"**
**Problem**: CMake fails with compiler detection errors.

**Solution**:
1. Install Visual Studio 2022 or Build Tools for Visual Studio 2022
2. Ensure MSVC compiler is selected in CMake
3. Verify Windows SDK is installed

#### **Linux: "OpenSSL Library Not Found"**
**Problem**: CMake fails to find OpenSSL libraries.

**Solution**:
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# CentOS/RHEL/Amazon Linux
sudo yum install openssl-devel
```

#### **macOS: "Xcode Command Line Tools Required"**
**Problem**: CMake fails on macOS.

**Solution**:
```bash
xcode-select --install
```

> **See Also:**
> - [Getting Started](getting_started.md)
> - [Debugging](debugging.md) - Comprehensive debugging system for troubleshooting
> - [Migration Tips for Python Users](python_use_cases.md#migration-tips-for-python-users)
> - [Python Integration Guide](python_integration.md)

## Common Error Patterns

### `{"error":-1}` Responses

**Problem**: Operations return `{"error":-1}` instead of expected results.

**Common Causes**:
1. **Namespace Issues**: Objects created in local namespace are lost between calls
2. **Missing Global Variables**: Objects not properly stored in global namespace
3. **Type Conversion Issues**: Binary data not converted to appropriate types

**Solutions**:

#### Namespace Issues
```python
# ❌ Problem: Object lost between calls (when using parameters)
xy.eval('table = {}.table("ROW")', {'x': 5})
xy.eval('table.mkfield("name", "STR")')  # Error: table not found

# ✅ Solution: Use global namespace for parameterized execution
xy.eval('$global.table = {}.table("ROW")', {'x': 5})
xy.eval('table.mkfield("name", "STR")')  # Success

# ✅ Alternative: Direct execution (no parameters) - variables persist automatically
xy.eval('table = {}.table("ROW")')
xy.eval('table.mkfield("name", "STR")')  # Success
```

#### Type Conversion Issues
```python
# ❌ Problem: Binary data not converted
result = xy.eval('table.get("user1", "name")')  # Binary data

# ✅ Solution: Convert to appropriate type
result = xy.eval('table.get("user1", "name").str()')  # String
```

### Object Not Found Errors

**Problem**: Variables or objects are not found when expected.

**Common Causes**:
1. **Local Namespace Clearing**: Variables created in one call are lost in the next
2. **Missing Global Declaration**: Objects not stored in global namespace
3. **Incorrect Variable Names**: Typos or incorrect variable references

**Solutions**:

```python
# ✅ Always use $global for persistent objects
xy.eval('$global.fs = $file()')
xy.eval('$global.table = {}.table("ROW")')

# ✅ Use variable names directly after global declaration
xy.eval('fs.set("test.txt", "content")')
xy.eval('table.mkfield("name", "STR")')
```

### Field Type Issues

**Problem**: Integer fields are created as string types instead of integer types.

**Common Cause**: Incorrect field creation syntax for integer fields.

**Solutions**:

```python
# ❌ Problem: May create string fields
xy.eval('table.mkfield("age", "INT", "FIX", 4)')

# ✅ Solution: Use without explicit mode for integers
xy.eval('table.mkfield("age", "INT")')
```

### Syntax Errors

**Problem**: Grapa code fails to parse or execute.

**Common Causes**:
1. **Incorrect Echo Syntax**: Using function calls instead of method calls
2. **Wrong Loop Syntax**: Using `for` loops instead of `while` loops
3. **Missing Parentheses**: String concatenation not properly wrapped

**Solutions**:

#### Echo Syntax
```grapa
/* ❌ Incorrect */
echo("Hello World");

/* ✅ Correct */
"Hello World".echo();
```

#### Loop Syntax
```grapa
/* ❌ Incorrect */
for i in 1..10 {
    echo("Item " + i);
}

/* ✅ Correct */
i = 1;
while (i <= 10) {
    ("Item " + i.str()).echo();
    i = i + 1;
}
```

#### String Concatenation
```grapa
/* ❌ Incorrect */
"Name: " + name.str() + ", Age: " + age.str().echo();

/* ✅ Correct */
("Name: " + name.str() + ", Age: " + age.str()).echo();
```

### Array and Object Access Issues

**Problem**: Confusion about when to use bracket notation vs `.get()` method.

**Solution**: Understand the distinction between different object types:

#### Arrays and Lists
```grapa
/* ✅ Arrays support bracket notation */
element = ["a", "b", "c"];
value = element[1];        /* Returns "b" */
/* Note: .get() method not supported for arrays - use bracket notation */

/* ✅ Lists support bracket notation and .getname() */
obj = {"a": 11, "b": 22, "c": 33};
value = obj["b"];          /* Returns 22 */
name = obj.getname(1);     /* Returns "b" */
/* Note: .get() method not supported for lists - use bracket notation */
```

#### $file and $TABLE Objects
```grapa
/* ✅ ALWAYS use .getfield() and .setfield() for $file and $TABLE objects */
files = fs.ls();
file_info = files.getfield(0);

table_data = table.getfield("user1", "name");

/* ❌ Avoid bracket notation for $file and $TABLE objects */
file_info = files[0];  /* May not work reliably */
```

#### Property Names with Hyphens
```grapa
/* ⚠️ IMPORTANT: Hyphens in property names require quotes */
/* The hyphen (-) is interpreted as a subtraction operator */

/* ❌ Problem: Hyphen interpreted as subtraction */
config = {ab:3, "a-b":4, a-b:5};  /* a-b becomes 5 (math operation) */
config.a-b;  /* This is interpreted as config.a - b (subtraction) */

/* ✅ Solution: Use quotes for hyphenated names */
config = {ab:3, "a-b":4};
config["a-b"];  /* 4 (correct) */
config."a-b";   /* 4 (also correct) */

/* ✅ Also works for function calls */
config = {ab:3, "a-b":op(){44;}};
config."a-b"();  /* 44 */
```

## Debugging Techniques

### Check Object Types

```python
# Check what type an object is
xy.eval('$global.debug_obj = some_operation()')
obj_type = xy.eval('debug_obj.type()')
print(f"Object type: {obj_type}")
```

### Verify Global Variables

```python
# Test if global variable exists
xy.eval('$global.test_var = "test"')
result = xy.eval('test_var')
print(f"Global variable: {result}")
```

### Step-by-Step Debugging

```python
# Test each step individually
xy.eval('$global.fs = $file()')
print("File object created")

xy.eval('fs.set("test.txt", "Hello")')
print("File written")

content = xy.eval('fs.get("test.txt").str()')
print(f"File content: {content}")
```

### Error Pattern Analysis

```python
# Check for specific error patterns
result = xy.eval('some_operation()')
if result == {"error":-1}:
    print("Operation failed with error -1")
elif result.type() == "$ERR":
    print("Operation returned error type")
else:
    print(f"Operation succeeded: {result}")
```

## GrapaPy Integration Issues

### Namespace Persistence

**Problem**: Objects don't persist between Python calls.

**Solution**: Always use `$global` for objects that need to persist:

```python
# ✅ Correct pattern
xy.eval('$global.table = {}.table("ROW")')
xy.eval('table.mkfield("name", "STR", "VAR")')
xy.eval('table.set("user1", "John", "name")')
```

### Type Conversion

**Problem**: Binary data returned instead of Python types.

**Solution**: Use Grapa's conversion methods:

```python
# ✅ Convert binary data to Python types
name = xy.eval('table.get("user1", "name").str()')
age = xy.eval('table.get("user1", "age").int()')
salary = xy.eval('table.get("user1", "salary").float()')
```

### Helper Classes

**Problem**: Repetitive code for common operations.

**Solution**: Create helper classes:

```python
class GrapaTableHelper:
    def __init__(self, grapa_instance, table_name):
        self.xy = grapa_instance
        self.table_name = table_name
        self.xy.eval(f'$global.{table_name} = {}.table("ROW")')
    
    def mkfield(self, name, field_type, mode="VAR", size=None):
        if size:
            self.xy.eval(f'{self.table_name}.mkfield("{name}", "{field_type}", "{mode}", {size})')
        else:
            self.xy.eval(f'{self.table_name}.mkfield("{name}", "{field_type}")')
    
    def set(self, key, value, field):
        self.xy.eval(f'{self.table_name}.set("{key}", "{value}", "{field}")')
    
    def get(self, key, field, convert_type="str"):
        return self.xy.eval(f'{self.table_name}.get("{key}", "{field}").{convert_type}()')
```

## Command Line Issues

### File Execution

**Problem**: `.grc` files don't execute properly.

**Solution**: Use correct command line options:

```powershell
# ✅ Correct: Use -f for .grc files
.\grapa.exe -f "examples/basic_example.grc"

# ❌ Incorrect: -c may cause parsing issues
.\grapa.exe -c "examples/basic_example.grc"
```

### Quiet Mode

**Problem**: Too much output from version headers.

**Solution**: Use `-q` flag to suppress version output:

```powershell
.\grapa.exe -f "examples/basic_example.grc" -q
```

### Debug Output Stream Separation

**Problem**: Debug output mixed with normal program output.

**Solution**: Use stream redirection to separate outputs:

```powershell
# Separate normal and debug output
.\grapa.exe -d -f "script.grc" > normal_output.txt 2> debug_output.txt

# Suppress debug output entirely
.\grapa.exe -d -f "script.grc" 2> $null

# Capture only debug output
.\grapa.exe -d -f "script.grc" > $null 2> debug_output.txt
```

**Note**: Grapa properly separates normal program output (stdout) from debug output (stderr), allowing for flexible stream redirection and logging.

## Best Practices for Debugging

### 1. Test Incrementally

```python
# Test each operation step by step
xy.eval('$global.test = "Hello"')
print("Step 1: Variable created")

result = xy.eval('test')
print(f"Step 2: Variable retrieved: {result}")

xy.eval('$global.fs = $file()')
print("Step 3: File object created")
```

### 2. Use Descriptive Variable Names

```python
# ✅ Good: Clear variable names
xy.eval('$global.user_table = {}.table("ROW")')
xy.eval('$global.file_system = $file()')

# ❌ Bad: Unclear variable names
xy.eval('$global.t = {}.table("ROW")')
xy.eval('$global.f = $file()')
```

### 3. Parameter Side Effects

**Problem**: Function parameters are modified unexpectedly

```python
# ✅ Fixed: Default values are now immutable
f = op(x=0) {
    x += 1;
    x;
};
f();  // Returns 1
f();  // Returns 1 (expected - default value is not modified)

# ❌ Problem: Original variables are modified
data = [1, 2, 3];
process = op(arr) {
    arr[0] = 999;
    arr;
};
process(data);  // Returns [999, 2, 3]
data;          // Now [999, 2, 3] (unexpected!)
```

**Solution**: Use the `.copy()` method or copy functions when you need to avoid side effects

```grapa
// ✅ Solution 1: Use .copy() method (recommended)
data = [1, 2, 3];
process(data.copy());  // Returns [999, 2, 3]
data;                  // Still [1, 2, 3] (expected!)

// ✅ Solution 2: Copy function to avoid side effects
copy = op(x) { x; };

data = [1, 2, 3];
process(copy(data));  // Returns [999, 2, 3]
data;                // Still [1, 2, 3] (expected!)
```

**Why This Happens**: Grapa uses pass-by-reference for performance reasons, avoiding the need for a full garbage collection system.

### 4. Check Return Values

```python
# Always check what operations return
result = xy.eval('some_operation()')
print(f"Operation returned: {result}")
print(f"Type: {type(result)}")
```

### 4. Use Error Handling

```python
try:
    result = xy.eval('some_operation()')
    if result == {"error":-1}:
        print("Operation failed")
    else:
        print("Operation succeeded")
except Exception as e:
    print(f"Exception occurred: {e}")
```

## Common Patterns and Solutions

### Table Operations Pattern

```python
# ✅ Complete table operation pattern
xy.eval('$global.table = {}.table("ROW")')
xy.eval('table.mkfield("name", "STR", "VAR")')
xy.eval('table.mkfield("age", "INT")')
xy.eval('table.mkfield("salary", "FLOAT", "FIX", 8)')

xy.eval('table.set("user1", "John Doe", "name")')
xy.eval('table.set("user1", 30, "age")')
xy.eval('table.set("user1", 75000.50, "salary")')

name = xy.eval('table.get("user1", "name").str()')
age = xy.eval('table.get("user1", "age").int()')
salary = xy.eval('table.get("user1", "salary").float()')
```

### File Operations Pattern

```python
# ✅ Complete file operation pattern
xy.eval('$global.fs = $file()')
xy.eval('fs.set("test.txt", "Hello World")')
content = xy.eval('fs.get("test.txt").str()')
xy.eval('fs.rm("test.txt")')
```

This troubleshooting guide covers the most common issues and their solutions. When encountering problems, start with the namespace and type conversion issues, as these are the most frequent causes of `{"error":-1}` responses. 

> **For Python Users:**
> If you encounter issues with GrapaPy integration, see the [Migration Tips for Python Users](python_use_cases.md#migration-tips-for-python-users) and [GrapaPy Namespace Solution](grapapy_namespace_solution.md) for solutions to common problems and best practices. 