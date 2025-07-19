# Documentation Examples

This directory contains test examples and sample files that are referenced by the Grapa documentation.

## Dependency System

**⚠️ IMPORTANT: Files in this directory are referenced by documentation. Do not delete or move files without updating the corresponding documentation links.**

## Testing Requirements

**🔬 CRITICAL: All .grc and .py files referenced in documentation MUST be tested before deployment.**

### Pre-Testing Review

**📋 IMPORTANT: Before testing any .grc or .py files, review them against the full basic syntax documentation.**

#### Syntax Review Checklist
1. **Comments**: Use block comments `/* ... */`, not line comments `// ...`
2. **Loops**: Use `while` loops, not `for` loops (Grapa doesn't support for loops)
3. **String concatenation**: Wrap in parentheses `("Hello " + name)`
4. **Array access**: Use bracket notation `array[index]` and `array.len()`
5. **List access**: Use bracket or dot notation `list[key]` or `list.key`
6. **Object access**: Use dot notation `object.property`
7. **Echo statements**: Use `.echo()` method, not function calls
8. **Control structures**: Use proper `if`, `while`, `switch` syntax
9. **Function definitions**: Use `op()` syntax for functions
10. **Type conversions**: Use `.str()`, `.int()`, `.float()` methods

#### Reference Documentation
- **Primary Reference**: `docs/docs/syntax/basic_syntax.md` - Canonical syntax guide
- **Operator Reference**: `docs/docs/syntax/operator.md` - Operator usage
- **Precedence Reference**: `docs/docs/syntax/precedence.md` - Operator precedence

### Testing Process

#### 1. Test .grc Files
```bash
# Test basic example
./grapa -cfile "docs/docs/examples/basic_example.grc" -q

# Test advanced example  
./grapa -cfile "docs/docs/examples/advanced_example.grc" -q

# Test performance example
./grapa -cfile "docs/docs/examples/performance_example.grc" -q
```

#### 2. Test .py Files
```bash
# Test Python integration example
python3 docs/docs/examples/python_integration_example.py
```

#### 3. Expected Results
- **Basic Example**: Should return `20` (result of 10 + 5 * 2)
- **Advanced Example**: Should return `[["Hello","World","Grapa","Programming"],10]` (words array and array length)
- **Performance Example**: Should return `[4950,10]` (sum of 0-99, array length)
- **Python Example**: Should run without errors (may show "module not installed" message)

### Testing Standards

#### Before Adding New Examples
1. **Write the example** with basic, reliable syntax
2. **Test locally** to ensure it works
3. **Document expected output** in comments
4. **Add to testing checklist** below

#### Before Deployment
1. **Run all example tests** listed above
2. **Verify expected outputs** match documentation
3. **Check for syntax errors** or runtime issues
4. **Update this README** if new examples are added

### Current Test Checklist

- [x] `basic_example.grc` - Basic syntax and operations
- [x] `advanced_example.grc` - Array and string operations  
- [x] `performance_example.grc` - Loop and array performance
- [x] `python_integration_example.py` - Python integration structure

### File Organization

- **`.grc` files**: Grapa source code examples
- **`.grz` files**: Compiled Grapa examples  
- **`.py` files**: Python integration examples
- **`.md` files**: Example documentation

### Adding New Examples

1. Place your example file in the appropriate subdirectory
2. **Test the example** to ensure it works correctly
3. Update the documentation to reference the new file
4. Add a comment in the documentation indicating the dependency
5. **Add to testing checklist** above

### Example Documentation Reference

```markdown
<!-- DEPENDENCY: examples/basic_example.grc -->
```grapa
include "examples/basic_example.grc";
```

### Moving/Deleting Examples

Before moving or deleting any file in this directory:

1. Search the documentation for references to the file
2. Update all documentation links
3. Consider if the example should be preserved elsewhere
4. **Remove from testing checklist** if deleting

### Search Commands

To find documentation references to a specific example:

```bash
# Search for references to a specific file
grep -r "examples/filename.grc" docs/docs/

# Search for all example references
grep -r "examples/" docs/docs/
```

## Current Examples

### Basic Example (`basic_example.grc`)
- **Purpose**: Demonstrates basic Grapa syntax and operations
- **Test Command**: `./grapa -cfile "docs/docs/examples/basic_example.grc" -q`
- **Expected Output**: `20` (result of 10 + 5 * 2)
- **Status**: ✅ Tested and working

### Advanced Example (`advanced_example.grc`)
- **Purpose**: Demonstrates complex array and string operations
- **Test Command**: `./grapa -cfile "docs/docs/examples/advanced_example.grc" -q`
- **Expected Output**: Array filtering and string splitting results
- **Status**: ✅ Tested and working

### Performance Example (`performance_example.grc`)
- **Purpose**: Demonstrates loop and array performance operations
- **Test Command**: `./grapa -cfile "docs/docs/examples/performance_example.grc" -q`
- **Expected Output**: `[4950, 100]` (sum of 0-99, array length)
- **Status**: ✅ Tested and working

### Python Integration Example (`python_integration_example.py`)
- **Purpose**: Demonstrates Python integration structure and usage
- **Test Command**: `python3 docs/docs/examples/python_integration_example.py`
- **Expected Output**: Demonstration structure (may show "module not installed")
- **Status**: ✅ Tested and working 