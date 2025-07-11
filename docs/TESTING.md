# Testing Documentation

## Overview

Grapa includes a comprehensive test suite for validating Unicode grep functionality, performance optimizations, and regression testing. All tests are organized in the `test/` directory for easy management and execution.

## Test Organization

### Test Directory Structure
```
test/
├── test_python_examples.py        # Python integration and callback tests
├── test_grep_python_examples.py   # Python grep functionality tests
├── test_python_callback.py        # Python callback escaping/troubleshooting
```

### Test Categories

#### 1. **Capabilities Test** (`test_current_capabilities.grc`)
- **Purpose**: Comprehensive testing of all Unicode and regex features
- **Coverage**: 15 feature categories (12 supported, 3 unsupported)
- **Features Tested**:
  - Basic Unicode properties (`\p{L}`, `\p{N}`, etc.)
  - Named groups and JSON output
  - Lookaround assertions
  - Unicode grapheme clusters
  - Advanced Unicode properties
  - Context lines
  - Atomic groups
  - Possessive quantifiers
  - Conditional patterns
  - Unicode scripts and script extensions
  - Unicode general categories
  - Unicode blocks (not supported)
  - Unicode age properties (not supported)
  - Unicode bidirectional classes (not supported)

#### 2. **Performance Test** (`test_performance_optimizations.grc`)
- **Purpose**: Validate performance optimizations are working
- **Coverage**: 8 performance optimization tests
- **Features Tested**:
  - JIT compilation detection and functionality
  - Fast path optimizations for literal patterns
  - Fast path optimizations for word patterns
  - Fast path optimizations for digit patterns
  - LRU cache functionality for text normalization
  - Complex Unicode pattern performance
  - Mixed pattern performance
  - Edge case performance

#### 3. **Parallel Processing Test** (`test_parallel_grep_verification.grc`)
- **Purpose**: Validate parallel processing functionality and performance
- **Coverage**: 8 parallel processing tests
- **Features Tested**:
  - Auto-detection with num_workers = 0
  - Sequential processing with num_workers = 1
  - 2-thread processing with num_workers = 2
  - 4-thread processing with num_workers = 4
  - Small input handling (should use sequential)
  - Result consistency verification
  - Basic performance comparison
  - Parallel function verification

#### 4. **Feature-Specific Tests**
- **Atomic Groups** (`test_atomic_groups.grc`): 10 tests for atomic group functionality
- **Lookaround Assertions** (`test_lookaround_assertions.grc`): 8 tests for lookaround functionality
- **Unicode Grapheme Clusters** (`test_unicode_grapheme_clusters.grc`): 10 tests for grapheme cluster functionality

#### 5. **Python Integration Tests**
- **Python Examples** (`test_python_examples.py`): Comprehensive Python-Grapa integration and callback tests, including argument passing and return values.
- **Python Grep Examples** (`test_grep_python_examples.py`): Grep functionality via Python interface.
- **Python Callback Escaping** (`test_python_callback.py`): Troubleshooting and demonstration of correct callback escaping patterns.

### Running Python Tests
Python tests require the `grapapy` module to be installed:
```bash
# Install grapapy module
pip install -e .

# Run Python integration tests
python test/test_python_examples.py

# Run Python grep tests
python test/test_grep_python_examples.py

# Run Python callback escaping tests
python test/test_python_callback.py
```

## Running Tests

### Complete Test Suite
Run all tests in one command:
```bash
# Windows
.\grapa.exe -cfile "test/run_tests.grc"

# Linux/Mac
./grapa -cfile "test/run_tests.grc"
```

### Individual Test Categories
Run specific test categories:
```bash
# Capabilities test
.\grapa.exe -cfile "test/test_current_capabilities.grc"

# Performance test
.\grapa.exe -cfile "test/test_performance_optimizations.grc"

# Feature-specific tests
.\grapa.exe -cfile "test/test_atomic_groups.grc"
.\grapa.exe -cfile "test/test_lookaround_assertions.grc"
.\grapa.exe -cfile "test/test_unicode_grapheme_clusters.grc"

# Parallel processing test
.\grapa.exe -cfile "test/test_parallel_grep_verification.grc"

# Python tests (requires grapapy module)
python test/test_python_md_examples.py
python test/test_grep_python_examples.py
```

### Regression Testing
For development and CI/CD, run the complete test suite:
```bash
# Quick regression test
.\grapa.exe -cfile "test/run_tests.grc"
```

## Test Output

### Expected Results
- **Capabilities Test**: 12 PASS, 3 FAIL (expected failures for unsupported features)
- **Performance Test**: 8 PASS
- **Parallel Processing Test**: 8 PASS
- **Atomic Groups Test**: 10 PASS
- **Lookaround Assertions Test**: 8 PASS
- **Unicode Grapheme Clusters Test**: 10 PASS

### Sample Output
```
=== GRAPA UNICODE GREP TEST SUITE ===
Running comprehensive test suite for Unicode grep functionality
Timestamp: 2024-01-15 14:30:00

--- TEST 1: CAPABILITIES TEST ---
Running comprehensive capability testing...
=== CURRENT CAPABILITIES TEST ===
Test 1: Basic Unicode Properties
PASS - Basic Unicode properties work
...
--- CAPABILITIES TEST COMPLETED ---

--- TEST 2: PERFORMANCE TEST ---
Running performance optimization testing...
=== PERFORMANCE OPTIMIZATIONS TEST ===
Test 1: JIT Compilation Detection
PASS - JIT compilation enabled and working
...
--- PERFORMANCE TEST COMPLETED ---

=== ALL TESTS COMPLETED ===
Test suite execution finished.
All essential functionality has been validated.
```

## Test Development

### Adding New Tests
1. Create new test file in `test/` directory
2. Follow naming convention: `test_<feature_name>.grc` for Grapa tests or `test_<feature_name>.py` for Python tests
3. Add include statement to `test/run_tests.grc` for Grapa tests
4. Test with both individual and complete test suite
5. For Python tests, ensure grapapy module is installed and test independently

### Test File Structure
```grapa
/* Test Feature Name */
/* Brief description of what this test validates */

"=== FEATURE TEST ===\n".echo();
"Testing specific feature functionality\n".echo();

/* Test 1: Basic functionality */
"Test 1: Basic functionality\n".echo();
input = "test input";
result = input.grep("pattern", "oj");
if (result.type() != $ERR && result.len() > 0) {
    "PASS - Basic functionality works\n".echo();
} else {
    "FAIL - Basic functionality not working\n".echo();
}
"---\n".echo();

/* Additional tests... */

"=== FEATURE TEST COMPLETED ===\n".echo();
"Feature test completed successfully!\n".echo();
```

### Test Best Practices
1. **Clear test names**: Use descriptive test names
2. **Expected results**: Document expected PASS/FAIL outcomes
3. **Error handling**: Test both success and failure cases
4. **Performance**: Keep tests fast for quick feedback
5. **Isolation**: Each test should be independent

## Continuous Integration

### Automated Testing
The test suite is designed for automated CI/CD pipelines:
```bash
# CI/CD script example
grapa -cfile "test/run_tests.grc" > test_results.log
if grep -q "FAIL" test_results.log; then
    echo "Tests failed!"
    exit 1
else
    echo "All tests passed!"
    exit 0
fi
```

### Pre-commit Testing
Run tests before committing changes:
```bash
# Quick regression test
.\grapa.exe -cfile "test/run_tests.grc"
```

## Troubleshooting

### Common Issues
1. **Test file not found**: Ensure you're running from the project root directory
2. **Include path errors**: Check that test files are in the `test/` directory
3. **Performance issues**: Tests should complete quickly (< 30 seconds)
4. **Memory issues**: Tests should not consume excessive memory

### Debugging Tests
1. Run individual test files to isolate issues
2. Check test output for specific error messages
3. Verify test input data is correct
4. Ensure test expectations match current implementation

## Test Coverage

### Current Coverage
- ✅ **Unicode Support**: Full Unicode property and script support
- ✅ **Regex Features**: Advanced regex features (atomic groups, lookarounds, etc.)
- ✅ **Performance**: JIT compilation, fast paths, caching
- ✅ **Output Formats**: JSON output, context lines, named groups
- ✅ **Edge Cases**: Error handling, malformed input, boundary conditions

### Coverage Gaps
- ❌ **Unicode blocks**: Not supported (by design)
- ❌ **Unicode age properties**: Not supported (by design)  
- ❌ **Unicode bidirectional classes**: Not supported (by design)

**Note:** For a comprehensive analysis of Grapa grep features compared to other tools like ripgrep, including detailed feature matrices and use case recommendations, see the [Main Grep Documentation](obj/grep.md).

## Related Documentation

- [Unicode Grep Documentation](obj/grep.md) - Detailed grep functionality
- [Build Documentation](BUILD.md) - How to build Grapa
- [Run Documentation](RUN.md) - How to run Grapa
- [Examples Documentation](EXAMPLES.md) - Usage examples 