# Troubleshooting

## Critical Issues

### 🚨 ROW Table Index Corruption Bug

**Issue**: ROW tables have a critical bug where the first record's index becomes corrupted when the third record is added.

**Symptoms**:
- First record returns `{"error":-1}` when retrieved
- Subsequent records work correctly
- Debug output shows empty RPTR entries for first record

**Workaround**: Use COL tables instead of ROW tables
```grapa
// Instead of:
tbl = $file().table("ROW");

// Use:
tbl = $file().table("COL");
```

**Status**: Under investigation. COL tables provide the same functionality and work correctly.

## Common Issues

This guide covers common issues encountered when working with Grapa and GrapaPy, along with their solutions and debugging techniques.

> **See Also:**
> - [Getting Started](GETTING_STARTED.md)
> - [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users)
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
# ❌ Problem: Object lost between calls
xy.eval('table = $file().table("ROW")')
xy.eval('table.mkfield("name", "STR")')  # Error: table not found

# ✅ Solution: Use global namespace
xy.eval('$global.table = $file().table("ROW")')
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
xy.eval('$global.table = $file().table("ROW")')

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
/* ✅ Arrays support both bracket notation and .get() */
element = ["a", "b", "c"];
value = element[1];        /* Returns "b" */
value = element.get(1);    /* Returns "b" */

/* ✅ Lists support bracket notation, .get(), and .getname() */
obj = {"a": 11, "b": 22, "c": 33};
value = obj["b"];          /* Returns 22 */
value = obj.get("b");      /* Returns 22 */
name = obj.getname(1);     /* Returns "b" */
```

#### $file and $TABLE Objects
```grapa
/* ✅ ALWAYS use .get() for $file and $TABLE objects */
files = fs.ls();
file_info = files.get(0);

table_data = table.get("user1", "name");

/* ❌ Avoid bracket notation for $file and $TABLE objects */
file_info = files[0];  /* May not work reliably */
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
xy.eval('$global.table = $file().table("ROW")')
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
```

## Command Line Issues

### File Execution

**Problem**: `.grc` files don't execute properly.

**Solution**: Use correct command line options:

```powershell
# ✅ Correct: Use -cfile for .grc files
.\grapa.exe -cfile "test/core/test_file.grc"

# ❌ Incorrect: -ccmd may cause parsing issues
.\grapa.exe -ccmd "test/core/test_file.grc"
```

### Quiet Mode

**Problem**: Too much output from version headers.

**Solution**: Use `-q` flag to suppress version output:

```powershell
.\grapa.exe -cfile "test/core/test_file.grc" -q
```

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
xy.eval('$global.user_table = $file().table("ROW")')
xy.eval('$global.file_system = $file()')

# ❌ Bad: Unclear variable names
xy.eval('$global.t = $file().table("ROW")')
xy.eval('$global.f = $file()')
```

### 3. Check Return Values

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
xy.eval('$global.table = $file().table("ROW")')
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
> If you encounter issues with GrapaPy integration, see the [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users) for solutions to common problems and best practices. 