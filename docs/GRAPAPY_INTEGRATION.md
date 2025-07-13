# GrapaPy Integration Guide

## Overview

GrapaPy provides Python integration for the Grapa language, allowing you to execute Grapa code from Python applications. This guide covers the essential concepts, best practices, and common patterns for effective GrapaPy integration.

## Basic Usage

```python
import grapapy

# Create a Grapa instance
xy = grapapy.grapa()

# Execute simple Grapa code
result = xy.eval('2 + 3')
print(result)  # 5

# Execute complex operations
result = xy.eval('$file().pwd()')
print(result)  # Current working directory
```

## Namespace Behavior

### Local vs Global Namespaces

GrapaPy maintains two separate namespaces:

1. **Local Namespace**: Cleared between each `eval()` call
2. **Global Namespace**: Persists across multiple `eval()` calls

### The `$global` Pattern

For objects that need to persist across multiple calls, use the `$global` namespace:

```python
# First assignment - use $global
xy.eval('$global.my_table = $file().table("ROW")')
xy.eval('$global.my_file = $file()')

# Subsequent calls - use variable name directly
xy.eval('my_table.mkfield("name", "STR", "VAR")')
xy.eval('my_file.set("test.txt", "Hello World")')
```

### Why `$global` is Needed

Without `$global`, objects created in one call are lost in the next:

```python
# ❌ This will fail - object lost between calls
xy.eval('table = $file().table("ROW")')
xy.eval('table.mkfield("name", "STR")')  # Error: table not found

# ✅ This works - object persists in global namespace
xy.eval('$global.table = $file().table("ROW")')
xy.eval('table.mkfield("name", "STR")')  # Success
```

## Table Operations

### Creating Tables

```python
# Create table in global namespace
xy.eval('$global.table = $file().table("ROW")')

# Create fields with correct syntax
xy.eval('table.mkfield("name", "STR", "VAR")')
xy.eval('table.mkfield("age", "INT")')  # Note: no explicit mode for INT
xy.eval('table.mkfield("salary", "FLOAT", "FIX", 8)')
```

### Working with Data

```python
# Insert data
xy.eval('table.set("user1", "John Doe", "name")')
xy.eval('table.set("user1", 30, "age")')
xy.eval('table.set("user1", 75000.50, "salary")')

# Retrieve data with type conversion
name = xy.eval('table.get("user1", "name").str()')
age = xy.eval('table.get("user1", "age").int()')
salary = xy.eval('table.get("user1", "salary").float()')
```

## File Operations

### File System Access

```python
# Create file object in global namespace
xy.eval('$global.fs = $file()')

# Basic operations
xy.eval('fs.set("test.txt", "Hello World")')
content = xy.eval('fs.get("test.txt").str()')
xy.eval('fs.rm("test.txt")')
```

### Directory Operations

```python
# Create and navigate directories
xy.eval('fs.mk("test_dir")')
xy.eval('fs.cd("test_dir")')
files = xy.eval('fs.ls()')
xy.eval('fs.cd("..")')
xy.eval('fs.rm("test_dir")')
```

## Type Conversion

### Binary Data Handling

GrapaPy returns binary data for many operations. Use Grapa's conversion methods:

```python
# Convert to appropriate Python types
string_result = xy.eval('some_operation().str()')
int_result = xy.eval('some_operation().int()')
float_result = xy.eval('some_operation().float()')
```

### Common Patterns

```python
# File content
content = xy.eval('fs.get("file.txt").str()')

# Table data
name = xy.eval('table.get("key", "field").str()')
age = xy.eval('table.get("key", "field").int()')
value = xy.eval('table.get("key", "field").float()')

# Directory listing
files = xy.eval('fs.ls()')  # Returns array
```

## Best Practices

### 1. Use Global Namespace for Persistent Objects

```python
# ✅ Good: Objects persist across calls
xy.eval('$global.table = $file().table("ROW")')
xy.eval('table.mkfield("name", "STR")')

# ❌ Bad: Objects lost between calls
xy.eval('table = $file().table("ROW")')
xy.eval('table.mkfield("name", "STR")')  # Will fail
```

### 2. Convert Binary Data Appropriately

```python
# ✅ Good: Convert to Python types
name = xy.eval('table.get("user1", "name").str()')
age = xy.eval('table.get("user1", "age").int()')

# ❌ Bad: Use raw binary data
name = xy.eval('table.get("user1", "name")')  # Binary data
```

### 3. Use Correct Field Creation Syntax

```python
# ✅ Good: Correct integer field creation
xy.eval('table.mkfield("age", "INT")')

# ❌ Bad: May create string fields instead
xy.eval('table.mkfield("age", "INT", "FIX", 4)')
```

### 4. Error Handling

```python
try:
    result = xy.eval('some_operation()')
    if result == {"error":-1}:
        print("Operation failed")
    else:
        print("Success:", result)
except Exception as e:
    print("Exception:", e)
```

## Common Patterns

### Helper Classes

```python
class GrapaTableHelper:
    def __init__(self, grapa_instance, table_name):
        self.xy = grapa_instance
        self.table_name = table_name
        self.xy.eval(f'$global.{table_name} = $file().table("ROW")')
    
    def mkfield(self, name, field_type, mode="VAR", size=None):
        if size:
            self.xy.eval(f'{self.table_name}.mkfield("{name}", "{field_type}", "{mode}", {size})')
        else:
            self.xy.eval(f'{self.table_name}.mkfield("{name}", "{field_type}")')
    
    def set(self, key, value, field):
        self.xy.eval(f'{self.table_name}.set("{key}", "{value}", "{field}")')
    
    def get(self, key, field, convert_type="str"):
        return self.xy.eval(f'{self.table_name}.get("{key}", "{field}").{convert_type}()')

# Usage
table = GrapaTableHelper(xy, "users")
table.mkfield("name", "STR", "VAR")
table.set("user1", "John Doe", "name")
name = table.get("user1", "name")
```

## Troubleshooting

### Common Issues

1. **`{"error":-1}` responses**: Usually indicate namespace issues - use `$global`
2. **Object not found errors**: Ensure objects are created in global namespace
3. **Type conversion errors**: Use `.str()`, `.int()`, `.float()` methods
4. **Field type issues**: Use correct field creation syntax for integers

### Debugging Tips

```python
# Check object types
xy.eval('$global.debug_obj = some_operation()')
obj_type = xy.eval('debug_obj.type()')
print(f"Object type: {obj_type}")

# Check namespace contents
xy.eval('$global.test_var = "test"')
result = xy.eval('test_var')
print(f"Global variable: {result}")
```

## Examples

### Complete Table Example

```python
import grapapy

xy = grapapy.grapa()

# Create table
xy.eval('$global.users = $file().table("ROW")')
xy.eval('users.mkfield("name", "STR", "VAR")')
xy.eval('users.mkfield("age", "INT")')
xy.eval('users.mkfield("salary", "FLOAT", "FIX", 8)')

# Add data
xy.eval('users.set("user1", "John Doe", "name")')
xy.eval('users.set("user1", 30, "age")')
xy.eval('users.set("user1", 75000.50, "salary")')

# Retrieve data
name = xy.eval('users.get("user1", "name").str()')
age = xy.eval('users.get("user1", "age").int()')
salary = xy.eval('users.get("user1", "salary").float()')

print(f"User: {name}, Age: {age}, Salary: {salary}")
```

This guide covers the essential patterns and best practices for effective GrapaPy integration. Follow these guidelines to avoid common pitfalls and build robust Grapa-Python applications. 