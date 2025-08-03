# GrapaPy Platform Validation Guide

This guide provides comprehensive instructions for validating GrapaPy on different platforms (Windows, Linux, macOS).

## Overview

GrapaPy is the Python extension for Grapa that provides:
- Mathematical operations and high-precision arithmetic
- String processing and grep operations
- Array operations with functional methods (map, filter, reduce)
- File system operations
- Python callback integration

## Pre-Validation Setup

### 1. Install GrapaPy
```bash
# Windows
pip install grapapy==0.0.49

# Linux/macOS
pip3 install grapapy==0.0.49
```

### 2. Verify Installation
```python
import grapapy
xy = grapapy.grapa()
result = xy.eval("2 + 2;")
print(f"Basic test: {result}")  # Should print: 4
```

## Validation Test Suite

### Running Individual Tests

1. **Basic Operations Test:**
   ```bash
   python test_basic_operations.py
   ```
   - Tests: Math, strings, arrays, high-precision math, simple functions

2. **Functional Methods Test:**
   ```bash
   python test_functional_methods.py
   ```
   - Tests: map, filter, reduce operations with correct syntax

3. **File Operations Test:**
   ```bash
   python test_file_operations.py
   ```
   - Tests: File creation, reading, grep integration, error handling

4. **Complete Validation Suite:**
   ```bash
   python run_validation.py
   ```
   - Runs all tests and provides summary

## Expected Results

### ✅ What Should Work Perfectly

1. **Basic Math Operations:**
   - Addition, subtraction, multiplication, division
   - Exponentiation (`**`), modulo (`%`)
   - High-precision arithmetic for large numbers

2. **String Operations:**
   - Concatenation (`+`)
   - Length (`.len()`)
   - Case conversion (`.upper()`, `.lower()`)

3. **Array Operations:**
   - Creation: `[1, 2, 3]`
   - Length: `.len()`
   - Indexing: `[0]`, `[1]`, etc.
   - Functional methods: `.map()`, `.filter()`, `.reduce()`

4. **Functional Methods:**
   - **Map:** `arr.map(op(x) { x * 2; })`
   - **Filter:** `arr.filter(op(x) { x % 2 == 0; })`
   - **Reduce:** `arr.reduce(op(acc, x) { acc += x; }, 0)` ⚠️ **IMPORTANT: Use `+=` not `+`**

5. **File Operations:**
   - File creation and writing
   - File reading (returns bytes, needs `.decode()`)
   - File info retrieval
   - File removal

6. **Grep Operations:**
   - String searching: `text.grep('pattern')`
   - Context options: `text.grep('pattern', 'A1B1')`

7. **High Precision Math:**
   - Large number calculations
   - Arbitrary precision arithmetic

### ⚠️ Known Issues and Workarounds

1. **Reduce Operations:**
   - **Issue:** Using `+` instead of `+=` returns 0
   - **Solution:** Always use `+=` (compound assignment)
   - **Correct:** `arr.reduce(op(acc, x) { acc += x; }, 0)`
   - **Wrong:** `arr.reduce(op(acc, x) { acc + x; }, 0)`

2. **File Content Handling:**
   - **Issue:** File content returned as bytes
   - **Solution:** Decode bytes to string
   ```python
   content = xy.eval("fs.get('file.txt');")
   if isinstance(content, bytes):
       content = content.decode('utf-8')
   ```

3. **Complex Recursion:**
   - **Issue:** Factorial functions may hang
   - **Solution:** Avoid complex recursive functions
   - **Workaround:** Use iterative approaches

4. **Error Handling:**
   - **Issue:** Some operations return `{"error":-1}`
   - **Solution:** Check for error objects and handle gracefully

## Platform-Specific Notes

### Windows
- Use `pip` for installation
- File paths use backslashes
- PowerShell commands available
- All tests should work as expected

### Linux
- Use `pip3` for installation
- File paths use forward slashes
- May need X11 dependencies for GUI components
- All tests should work as expected

### macOS
- Use `pip3` for installation
- File paths use forward slashes
- May need additional permissions for file operations
- All tests should work as expected

## Troubleshooting

### Common Issues

1. **Import Error:**
   ```bash
   pip install grapapy==0.0.49 --force-reinstall
   ```

2. **Reduce Returns 0:**
   - Check if using `+=` instead of `+`
   - Verify initial value is correct

3. **File Content Issues:**
   - Always decode bytes: `content.decode('utf-8')`
   - Check for `{"error":-1}` responses

4. **Hanging Operations:**
   - Avoid complex recursion
   - Use simple iterative functions
   - Test with small datasets first

### Debugging Commands

```python
# Test basic functionality
import grapapy
xy = grapapy.grapa()
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

## Validation Checklist

- [x] Basic math operations work
- [x] String operations work
- [x] Array operations work
- [x] File operations work (with byte handling)
- [x] Grep operations work
- [x] High precision math works
- [x] Functional methods work (map, filter, reduce)
- [x] Reduce operations work correctly (with `+=` syntax)
- [ ] Complex recursion tested (avoid if hangs)
- [x] File content properly decoded
- [x] Error handling works correctly

## Reporting Issues

When reporting issues, include:
1. **Platform:** Windows/Linux/macOS
2. **Python version:** `python --version`
3. **GrapaPy version:** `pip show grapapy`
4. **Exact error message**
5. **Steps to reproduce**
6. **Expected vs actual behavior**
7. **Test output:** Copy the full test output

## Success Criteria

A successful validation should show:
- All basic operations working
- Functional methods working with correct syntax
- File operations working (with proper byte handling)
- No hanging operations
- Proper error handling
- All tests in `run_validation.py` passing

## Next Steps

After successful validation:
1. Document any platform-specific issues
2. Test edge cases and error conditions
3. Verify performance with larger datasets
4. Test integration with other Python libraries
5. Report any new issues found 