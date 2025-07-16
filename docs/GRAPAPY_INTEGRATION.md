# GrapaPy Integration Guide

## Overview

GrapaPy provides Python integration for the Grapa language, allowing you to execute Grapa code from Python applications. This guide covers the essential concepts, best practices, and common patterns for effective GrapaPy integration.

## Basic Usage

> **Web Scraping Tip:** For web scraping, you can use Python's `requests` library to fetch web pages, then process the HTML or data using Grapa's parsing and extraction functions via GrapaPy. This is often more robust than relying on Grapa's built-in HTTP for complex sites.

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

> **Tip:** When using GrapaPy, always create persistent objects (like file handles or tables) in the `$global` namespace. After initialization, you can use the variable name directly in subsequent calls.

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

# Quickstart for Python Users

> **Essentials:**
> - Always use `$global` for objects that need to persist across eval calls (file handles, tables, etc.).
> - Use `.str()`, `.int()`, `.float()` for type conversion of results.
> - Check for `$ERR` return values—no try/catch, handle errors explicitly.
> - Use `[]` for list/array access, not `.get()`.
> - See [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users) and the upcoming [Python-to-Grapa Migration Guide](PYTHON_TO_GRAPA_MIGRATION.md) for more.
> **Parallel ETL Advantage for Python Users:**
> GrapaPy enables true, production-grade parallel ETL/data processing from Python. Unlike standard Python threading (limited by the GIL), GrapaPy leverages Grapa’s C++ backend for real parallelism, making high-throughput data workflows simple and robust. Use GrapaPy for large file processing, data transformation, analytics, and more.

> **See Also:**
> - [Getting Started](GETTING_STARTED.md)
> - [Python Use Cases](PYTHON_USE_CASES.md)
> - [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users)
> - [Python-to-Grapa Migration Guide](PYTHON_TO_GRAPA_MIGRATION.md)
> - [Examples](EXAMPLES.md)