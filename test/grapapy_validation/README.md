# GrapaPy Validation Testing

This directory contains comprehensive validation tests for GrapaPy (Python extension) after build and installation.

## Purpose

Validate that GrapaPy works correctly on each platform after:
1. Building the Python extension
2. Installing via pip
3. Testing core functionality

## Test Structure

### Core Test Files
- `test_basic_operations.py` - Basic math, strings, arrays
- `test_functional_methods.py` - map, reduce, filter operations
- `test_file_operations.py` - File system operations
- `test_grep_operations.py` - String searching and processing
- `test_python_callbacks.py` - Python function integration
- `test_high_precision_math.py` - Large number calculations
- `test_error_handling.py` - Error conditions and edge cases

### Validation Scripts
- `run_validation.py` - Main validation runner
- `install_and_test.py` - Automated install and test
- `platform_specific_tests.py` - Platform-specific validations

## Usage

### For Windows/Linux/macOS Agents

1. **Install GrapaPy:**
   ```bash
   pip install grapapy==0.0.49
   ```

2. **Run Basic Validation:**
   ```bash
   python test_basic_operations.py
   ```

3. **Run Full Validation Suite:**
   ```bash
   python run_validation.py
   ```

4. **Run Platform-Specific Tests:**
   ```bash
   python platform_specific_tests.py
   ```

## Expected Results

### ✅ What Should Work
- Basic math operations (+, -, *, /)
- String operations (concatenation, .len())
- Array operations (.len(), indexing, .map(), .filter())
- File operations (create, read, write)
- Grep operations on strings
- High precision math
- Python callbacks

### ⚠️ Known Issues to Investigate
- Complex recursion (factorial) may hang
- File content returned as bytes (needs .decode())
- Some operations return `{"error":-1}`

### ✅ Issues Resolved
- `.reduce()` method works correctly with `+=` syntax (not `+`)

## Troubleshooting

### Common Issues

1. **Import Error:**
   ```bash
   pip install grapapy==0.0.49 --force-reinstall
   ```

2. **File Content as Bytes:**
   ```python
   content = xy.eval("fs.get('test.txt');")
   if isinstance(content, bytes):
       content = content.decode('utf-8')
   ```

3. **Reduce Operations:**
   - Use `+=` (compound assignment) not `+` (addition)
   - Example: `arr.reduce(op(acc, x) { acc += x; }, 0)`
   - Test with simple examples first

4. **Hanging Operations:**
   - Avoid complex recursion
   - Use simple iterative functions
   - Test with small datasets first

## Platform-Specific Notes

### Windows
- Uses `pip` for installation
- File paths use backslashes
- PowerShell commands available

### Linux
- Uses `pip3` for installation
- File paths use forward slashes
- May need X11 dependencies

### macOS
- Uses `pip3` for installation
- File paths use forward slashes
- May need additional permissions

## Reporting Issues

When reporting issues, include:
1. Platform (Windows/Linux/macOS)
2. Python version
3. GrapaPy version
4. Exact error message
5. Steps to reproduce
6. Expected vs actual behavior

## Validation Checklist

- [ ] Basic math operations work
- [ ] String operations work
- [ ] Array operations work
- [ ] File operations work
- [ ] Grep operations work
- [ ] Python callbacks work
- [ ] High precision math works
- [ ] Error handling works correctly
- [ ] No hanging operations
- [ ] File content properly decoded
- [x] Reduce operations work correctly 