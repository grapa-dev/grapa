# GrapaPy Namespace Solution

## Problem Solved

The original issue was that GrapaPy file and table operations were returning `{"error":-1}` when using the `$file()` object. This was caused by a fundamental misunderstanding of how GrapaPy handles namespaces.

## Root Cause

GrapaPy has two execution modes that affect variable persistence:

### Direct Execution (No Parameters)
- **Execution level**: Global namespace
- **Variable persistence**: Variables persist automatically between calls
- **`$global` usage**: Not needed for variable persistence

### Parameterized Execution (With Parameters)
- **Execution level**: Local scope on top of global
- **Variable persistence**: Variables are local and don't persist between calls
- **`$global` usage**: Required for variable persistence

When we created a `$file()` object without understanding the execution model, it was being created in the wrong scope and deleted before we could use it in subsequent calls.

## Solution

The solution depends on your execution mode:

### Direct Execution (No Parameters) - Simple Approach

```python
import grapapy

xy = grapapy.grapa()

# Variables persist automatically - no $global needed
xy.eval("fs = $file();")
xy.eval("fs.set('test.txt', 'Hello World');")
content = xy.eval("fs.get('test.txt');")
print(content)  # Hello World
```

### Parameterized Execution (With Parameters) - Use $global

```python
import grapapy

xy = grapapy.grapa()

# When using parameters, use $global for persistence
xy.eval("$global.fs = $file();", {'x': 5})
xy.eval("fs.set('test.txt', 'Hello World');")
content = xy.eval("fs.get('test.txt');")
print(content)  # Hello World
```

### Mixed Approach (Recommended)

```python
import grapapy

xy = grapapy.grapa()

# Initialize persistent objects with $global (works in both modes)
xy.eval("$global.fs = $file();")

# Use objects directly (Grapa finds them in global namespace)
xy.eval("fs.set('test.txt', 'Hello World');")
content = xy.eval("fs.get('test.txt');")
print(content)  # Hello World
```

## Variable Resolution Order

Grapa searches for variables in this order:
1. Current context (within `{ }` brackets)
2. Function scope
3. Local namespace
4. Call stack namespaces
5. Global namespace

## Key Insights

1. **Execution mode determines persistence** - Direct execution persists variables automatically, parameterized execution requires `$global`
2. **The `$` character is NOT the problem** - It works correctly in GrapaPy
3. **Namespace management is the key** - Use global namespace for persistent objects when using parameters
4. **Mixed approach is recommended** - Use `$global` for initialization to work in both modes
5. **All Grapa operations work** - File system, database, grep, etc.

## Test Results

Our comprehensive test shows that with the global namespace approach:

- ✅ File operations work perfectly
- ✅ Table operations work perfectly  
- ✅ Directory operations work perfectly
- ✅ File splitting works perfectly
- ✅ Performance operations work perfectly
- ✅ Error handling works correctly

## Best Practices

1. **Initialize objects in global namespace**: `$global.fs = $file();`
2. **Use simple variable names**: `fs.set()` instead of `$global.fs.set()`
3. **Handle binary data properly**: Check for bytes and decode if needed
4. **Use batch operations**: Leverage Grapa's built-in loops for performance

## Conclusion

The GrapaPy extension works perfectly for all Grapa operations when using the correct namespace management approach. The original `{"error":-1}` issues were entirely due to namespace scope, not fundamental problems with the extension.

This solution enables full Python integration with Grapa's powerful file system, database, and grep capabilities. 