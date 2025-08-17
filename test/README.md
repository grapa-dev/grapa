# Grapa Test Suite

This directory contains a comprehensive, organized test suite for Grapa functionality.

## Test Organization Principles

### File Type Separation

- **`.grc` files**: Contain pure Grapa code and are executed directly by the Grapa executable
- **`.py` files**: Contain Python code that uses the Grapa Python extension directly
- **No cross-calling**: `.grc` files never call Python, and `.py` files never execute `.grc` files

### Execution Methods

- **`.grc` files**: Run with `./grapa -f filename.grc`
- **`.py` files**: Run with `python filename.py` (uses Grapa extension)

## Test Organization

The test suite is organized into the following categories:

### Directory Structure

```
test/
├── core/                  # Core language feature tests (.grc files)
│   ├── test_extension_system.grc
│   ├── test_string_distance_functions.grc
│   ├── test_cosine_comprehensive.grc
│   └── ... (other core tests)
├── database/              # Database operation tests (.grc files)
│   ├── test_col.grc
│   ├── test_row.grc
│   ├── database_examples.grc
│   └── ... (other database tests)
├── file_system/           # File system operation tests (.grc files)
│   ├── test_file_basic.grc
│   └── test_file_advanced.grc
├── grep/                  # Unicode grep functionality tests (.grc files)
│   ├── test_current_capabilities.grc
│   ├── test_performance_optimizations.grc
│   └── ... (existing grep tests)
├── infrastructure/        # Debug and infrastructure tests (.grc files)
│   ├── compiler_debug_test.grc
│   ├── debug_flag_management_test.grc
│   └── ... (other debug tests)
├── integration/           # Integration and unified path tests (.grc files)
│   └── test_unified_path_system.grc
├── python/                # Python integration tests (.py files)
│   ├── test_string_distance_validation.py
│   └── ... (other Python tests)
├── cli/                   # CLI and command-line tests (.grc files)
│   └── test_cli.grc
├── use_cases/             # Use case and example tests (.grc files)
│   └── ... (use case tests)
└── README.md              # This file
```

## Running Tests

### Option 1: Comprehensive Test Runner (Recommended)

Run all `.grc` tests using the comprehensive test runner:

```bash
./grapa -f "test/run_tests_comprehensive.grc"
```

This will execute all `.grc` tests in the organized structure and provide detailed output.

**Note:** Python integration tests are run separately using Python.

### Option 2: Python Test Runner

Run both `.grc` and `.py` tests using the Python test runner:

```bash
python test/run_organized_tests.py
```

This runner:
- Executes `.grc` files using the Grapa executable
- Executes `.py` files using Python directly
- Provides comprehensive reporting

### Option 3: Individual Test Categories

Run specific test categories:

```bash
# Core language tests (.grc files)
./grapa -f "test/core/test_extension_system.grc"
./grapa -f "test/core/test_string_distance_functions.grc"

# Database tests (.grc files)
./grapa -f "test/database/test_col.grc"
./grapa -f "test/database/test_row.grc"

# File system tests (.grc files)
./grapa -f "test/file_system/test_file_basic.grc"
./grapa -f "test/file_system/test_file_advanced.grc"

# Integration tests (.grc files)
./grapa -f "test/integration/test_unified_path_system.grc"

# Python integration tests (.py files)
python test/python/test_string_distance_validation.py
```

## Test Categories

### Core Language Tests (`core/`)

**Extension System Tests**
- Object and class extension with `+=` and `++=` operators
- System class extension and inheritance
- Instance method calling across different types

**String Distance Tests**
- Levenshtein distance algorithm
- Jaro-Winkler similarity
- Cosine similarity with TF-IDF and word frequency methods

### Database Tests (`database/`)

**Table Type Tests**
- ROW store tests (`test_row.grc`)
- COL store tests (`test_col.grc`)
- Comprehensive database validation
- Field type testing and edge cases

### File System Tests (`file_system/`)

**Basic Tests (`test_file_basic.grc`)**
- Directory navigation (`cd`, `pwd`, `phd`)
- File operations (`set`, `get`, `rm`)
- Directory operations (`mk`, `ls`)
- File information (`info`)
- Error handling

**Advanced Tests (`test_file_advanced.grc`)**
- File splitting functionality (`split`)
- Large file operations
- CSV file splitting with header preservation
- Custom delimiter splitting
- Start option for smart boundary detection
- Content verification and integrity checks

### Database Tests (`database/`)

**Basic Tests (`test_table_basic.grc`)**
- Table creation (ROW, COL, GROUP stores)
- Field creation and types (STR, INT, FLOAT, BOOL, TIME)
- Data operations (`set`, `get`)
- Field information (`debug`)
- Error handling
- Performance testing

### Infrastructure Tests (`infrastructure/`)

**Debug and Compiler Tests**
- Compiler debug functionality (`compiler_debug_test.grc`)
- Parser debug testing (`parser_debug_test.grc`)
- Debug flag management (`debug_flag_management_test.grc`)
- Complete debug system validation (`complete_debug_test.grc`)

### Integration Tests (`integration/`)

**Unified Path System (`test_unified_path_system.grc`)**
- Seamless navigation between file system and database
- Parent directory navigation
- Mixed path operations
- Complex nested structures
- Path validation and error handling
- Data persistence across context transitions

### Python Integration Tests (`python/`)

**File Operations (`test_python_file_operations.py`)**
- Uses GrapaPy extension directly (`import grapapy`)
- Creates Grapa instance with `xy = grapapy.grapa()`
- Executes Grapa code with `xy.eval("grapa_code")`
- File system operations from Python (`$file()`)
- Database operations from Python (`$file().table()`)
- File split functionality from Python
- Data processing with CSV and JSON
- Error handling and exception management
- Performance testing and benchmarking
- Grep functionality testing

### Grep Tests (`grep/`)

Existing Unicode grep functionality tests:
- Current capabilities
- Performance optimizations
- Atomic groups
- Lookaround assertions
- Unicode grapheme clusters
- Parallel grep verification
- Basic option combinations
- Error handling
- Binary mode
- Custom delimiters
- Advanced context
- Unicode normalization
- Context lines

### Regression Tests (`regression/`)

**Known Bugs (`test_known_bugs.grc`)**
- Tests for previously identified issues
- Edge cases that caused problems
- Performance regression tests

**Edge Cases (`test_edge_cases.grc`)**
- Boundary conditions
- Error scenarios
- Resource limits
- Concurrent operations

## Test Features

### Comprehensive Coverage

The test suite covers:
- ✅ All file system operations
- ✅ All database operations (ROW, COL, GROUP stores)
- ✅ Unified path system
- ✅ File splitting functionality
- ✅ Python integration via extension
- ✅ Unicode grep functionality
- ✅ Error handling and edge cases
- ✅ Performance testing
- ✅ Regression testing

### Test Quality

Each test includes:
- Clear test descriptions
- Comprehensive error handling
- Proper cleanup procedures
- Performance measurements
- Detailed output and logging
- Verification of expected results

### Maintainability

- Organized directory structure
- Consistent naming conventions
- Modular test design
- Clear separation between file types
- Proper execution methods

## Adding New Tests

### For File System Operations (.grc files)

1. Create a new `.grc` file in `test/file_system/`
2. Write pure Grapa code (no Python calls)
3. Follow the existing test structure
4. Include proper cleanup
5. Add to the comprehensive test runner

### For Database Operations (.grc files)

1. Create a new `.grc` file in `test/database/`
2. Test all table types (ROW, COL, GROUP)
3. Include field type testing
4. Add performance comparisons

### For Python Integration (.py files)

1. Create a new `.py` file in `test/python/`
2. Use the GrapaPy extension directly (`import grapapy`)
3. Create a Grapa instance with `xy = grapapy.grapa()`
4. Execute Grapa code with `xy.eval("grapa_code")`
5. Include proper exception handling
6. Add performance benchmarks
7. **Never** build strings to execute `.grc` files

### For Integration Tests (.grc files)

1. Create a new `.grc` file in `test/integration/`
2. Test cross-context operations
3. Verify data persistence
4. Test complex navigation scenarios

## Test Output

Tests provide detailed output including:
- Test category and name
- Step-by-step progress
- Expected vs actual results
- Performance metrics
- Error details
- Cleanup confirmation

## Troubleshooting

### Common Issues

1. **Grapa executable not found**
   - Ensure `grapa` is in the current directory
   - Check file permissions

2. **Python extension not found**
   - Install the GrapaPy extension: `pip install grapapy`
   - Check Python path configuration

3. **Test timeouts**
   - Increase timeout values for large file operations
   - Check system resources

4. **Permission errors**
   - Ensure write permissions in test directories
   - Run with appropriate user privileges

### Debug Mode

For debugging test issues, run individual tests with verbose output:

```bash
./grapa -f "test/file_system/test_file_basic.grc" -v
```

## Test Results

The test suite provides:
- Pass/fail status for each test
- Detailed error messages
- Performance metrics
- Coverage statistics
- Summary reports

## Key Principles

1. **Separation of Concerns**: `.grc` files test Grapa directly, `.py` files test Python integration
2. **No String Building**: Never build strings to execute other file types
3. **Direct Execution**: Use appropriate execution methods for each file type
4. **Comprehensive Coverage**: Test all functionality thoroughly
5. **Maintainability**: Keep tests organized and well-documented

This organized test suite ensures comprehensive coverage of all Grapa functionality while maintaining clear separation between different execution methods. 