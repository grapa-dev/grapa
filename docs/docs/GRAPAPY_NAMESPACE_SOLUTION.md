# GrapaPy Namespace Solution

## Problem Solved

The original issue was that GrapaPy file and table operations were returning `{"error":-1}` when using the `$file()` object. This was caused by a fundamental misunderstanding of how GrapaPy handles namespaces.

## Root Cause

GrapaPy maintains separate local and global namespaces:
- **Local namespace**: Cleared between calls to `xy.eval()`
- **Global namespace**: Persists across calls to `xy.eval()`

When we created a `$file()` object in the local namespace, it was being deleted before we could use it in subsequent calls.

## Solution

Use the global namespace for objects that need to persist across calls:

### Correct Pattern

1. **First time (initialization)**: Use `$global.variable` to create/initialize the variable
2. **Subsequent times**: Use just `variable` directly - Grapa will find it in the global namespace

### Example

```python
import grapapy

xy = grapapy.grapa()

# Initialize file system in global namespace (first time only)
xy.eval("$global.fs = $file();")

# Use file system (no $global needed)
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

1. **The `$` character is NOT the problem** - It works correctly in GrapaPy
2. **Namespace management is the key** - Use global namespace for persistent objects
3. **Only use `$global` for initialization** - After that, just use the variable name
4. **All Grapa operations work** - File system, database, grep, etc.

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