# Grapa CLI Analysis and Improvement Plan

This document provides a comprehensive analysis of the current Grapa command-line interface functionality, identifying current features, testing requirements, areas for improvement, and a roadmap for enhancement.

---

## 📊 Current CLI State Analysis

### ✅ **Implemented & Working Well**

#### Standard Information Options
```bash
-h, --help          # Show help message ✅
-v, --version       # Show version ✅  
-q, --quiet         # Suppress header output ✅
```

#### Execution Modes
```bash
grapa               # Interactive mode (default) ✅
grapa -i            # Force interactive mode ✅
grapa script.grc    # Direct file execution ✅
grapa "'hello'"     # Direct quoted command ✅
grapa 'hello'.echo() # Direct method call ✅
```

#### Smart Input Detection
```bash
echo "data" | grapa  # Auto-detect pipe input ✅
cat file.txt | grapa # Automatic stdin reading ✅
```

### 🔄 **Needs Standardization**

#### Non-Standard Options (To Be Replaced)
```bash
-c script           # Execute command (standard)
-f file             # Execute file (standard)
-s                  # Read from stdin (standard)
-S                  # Store stdin in $ARGCIN (standard)
```

#### Conflicting Options (Removed)
```bash
# These options have been removed from the implementation
# -c, --console       # Conflicts with standard -c for command
# -e, --env           # Conflicts with standard -e for eval
```

### ❌ **GUI Options (Moved)**
```bash
# These options have been removed from the implementation
# -w                  # Editor mode → moved to separate GUI tool
# -wfile file         # Editor with file → moved to separate GUI tool
```

### 🔧 **Internal Options (Removed)**
```bash
# These options have been removed from the implementation
# -argv               # Reset argument list (internal use) - REMOVED
# -                   # Force non-exit (internal use) - REPURPOSED as standard stdin option
```

---

## 🐍 Python CLI Standards (Reference Model)

### Core Python CLI Patterns
```bash
# Command execution
python -c "print('hello')"           # Execute command
python -e "print('hello')"           # Execute expression (some versions)

# File execution  
python script.py                     # Direct execution
python -f script.py                  # File execution (some versions)

# Interactive mode
python -i                            # Interactive mode
python                               # Interactive mode (default)

# Input/Output
python -s                            # Read from stdin
python -o output.txt                 # Output to file (not standard)
```

### Python CLI Philosophy
- **Simplicity**: Direct execution without unnecessary flags
- **Intuitive**: Auto-detection of input types
- **Standard**: Follow Unix conventions
- **Consistent**: Predictable behavior across platforms

---

## 🔧 Debugging and Development Options

### Current Debug Options
**Priority:** MEDIUM - Development support

#### Implemented Features
- `grapa -d` - Debug mode
- `grapa --debug` - Long debug option
- `grapa -t` - Trace mode
- `grapa --trace` - Long trace option

#### Proposed Enhancements
- `grapa --dump-ast` - Show parsed AST
- `grapa --dump-bytecode` - Show compiled bytecode
- `grapa --profile` - Performance profiling
- `grapa --strict` - Strict error mode

### Performance Options
**Priority:** MEDIUM - Performance optimization

#### Proposed Features
- `grapa --optimize` - Enable optimizations
- `grapa --no-cache` - Disable caching
- `grapa --memory-limit N` - Set memory limit
- `grapa --timeout N` - Set execution timeout

---

## 🧪 Testing Requirements

### Information Options Testing
**Priority:** HIGH - Core functionality

#### Success Criteria
- Help text displays correctly
- Version information is accurate
- Quiet mode suppresses appropriate output
- No crashes or errors

#### Edge Cases to Test
- Multiple options combined (`grapa -h -v -q`)
- Invalid option combinations
- Help text formatting on different terminals

### Execution Modes Testing
**Priority:** HIGH - Core functionality

#### Success Criteria
- Interactive mode starts correctly
- File execution works with valid files
- Quoted commands execute properly
- Error handling for invalid files

#### Edge Cases to Test
- Non-existent files
- Files with syntax errors
- Files with runtime errors
- Very large files
- Files with special characters in names

### Smart Input Detection Testing
**Priority:** HIGH - Core functionality

#### Success Criteria
- Pipe input is read correctly
- Method calls are detected and executed
- Output is displayed properly
- Error handling for invalid methods

#### Edge Cases to Test
- Empty pipe input
- Very large pipe input
- Binary pipe input
- Method calls with complex parameters
- Invalid method names

---

## 🎯 Improvement Roadmap

### Phase 1: Standardization (HIGH PRIORITY)
1. **Replace Non-Standard Options**:
   - `-c` (command execution) ✅ **COMPLETED**
- `-f` (file execution) ✅ **COMPLETED**
- `-s` (stdin reading) ✅ **COMPLETED**
- `-S` (stdin with $ARGCIN) ✅ **COMPLETED**

2. **Remove Conflicting Options**: ✅ **COMPLETED**
   - Remove `-c, --console` (conflicts with standard -c) ✅ **REMOVED**
   - Remove `-e, --env` (conflicts with standard -e) ✅ **REMOVED**

3. **Move GUI Options**: ✅ **COMPLETED**
   - Move `-w` and `-wfile` to separate GUI tool ✅ **COMPLETED**
   - Create `grapa-edit` or similar for editor functionality ⏳ **FUTURE ENHANCEMENT**

### Phase 2: Enhancement (MEDIUM PRIORITY)

#### **Enhanced Debug Mode**
- [x] **Improved Debug Output**: Enhanced `-d` option with verbose output capabilities ✅ **COMPLETED**
  - [x] **Execution Tracing**: Step-by-step execution tracking with variable state snapshots ✅ **COMPLETED**
  - [x] **Error Context Display**: Better error context information in debug mode ✅ **COMPLETED**
  - [x] **Performance Timing**: Built-in performance timing and memory usage tracking ✅ **COMPLETED**
  - [x] **AST/Bytecode Dumping**: Execution tree visualization and bytecode inspection ✅ **COMPLETED**

**Implementation Details:**
- **System-Level Debug** (`GrapaDebug`): Uses direct output for pre-session and session-agnostic operations
- **Session-Level Debug** (`GrapaScriptExecStateDebug`): Uses response system for session-specific operations
- **Component-Specific Debugging**: Support for filtering debug output by specific components (e.g., "lexer", "parser", "compiler")
- **Component-Specific Levels**: Different verbosity levels per component (e.g., "lexer:2,parser:1")
- **Session Isolation**: Proper separation prevents debug output interference between parallel sessions
- **Environment Variable Integration**: Debug flags can be set via `$sys().putenv()` and `$sys().getenv()`

**Usage Examples:**
```bash
# Enable system-level debug
grapa -d -c "'hello'.echo()"

# Enable session-specific debug with components
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa script.grc

# Enable component-specific levels
GRAPA_SESSION_DEBUG_COMPONENTS=lexer:2,parser:1 ./grapa script.grc
```

#### **Better Error Messages**
- [ ] **Enhanced Syntax Error Reporting**: More descriptive and helpful error messages
  - [ ] **Error Context Information**: Detailed error context with file, line, and function information
  - [ ] **Stack Trace Display**: Execution stack trace for debugging
  - [ ] **Variable State Snapshots**: Variable state at error points
  - [ ] **Error Type Classification**: Specific error types and hierarchies
  - [ ] **Error Recovery Suggestions**: Helpful suggestions for common errors
  - [ ] **Error History Tracking**: Comprehensive error history for debugging

**Enhanced Error Message Examples:**
```bash
# Current error message
grapa -c "x = 1 + ;"
# Error: Syntax error at line 1

# Enhanced error message
grapa -c "x = 1 + ;"
# Error: Syntax error at line 1, column 7
#   x = 1 + ;
#         ^
# Expected: expression, identifier, or literal
# Suggestion: Add an expression after the '+' operator
# Context: Assignment statement

# Error with context information
grapa -c "
function process_data() {
    data = $file().get('missing.txt');
    return data.len();
}
result = process_data();
"
# Error: FileNotFoundError at line 3, column 8 in function 'process_data'
#   data = $file().get('missing.txt');
#          ^
# File 'missing.txt' not found in current directory
# Stack trace:
#   - process_data() at line 3
#   - <main> at line 6
# Suggestion: Check if file exists or provide a default value using .iferr()

# Error with variable state
grapa -c "
x = 10;
y = 0;
result = x / y;
"
# Error: DivisionByZeroError at line 4, column 12
#   result = x / y;
#            ^
# Division by zero attempted
# Variables at error:
#   x = 10
#   y = 0
# Stack trace:
#   - <main> at line 4
# Suggestion: Check if y is zero before division or use .iferr() for fallback
```

**Implementation Strategy:**
1. **Error context capture** - Automatic error context capture during execution
2. **Syntax error enhancement** - Enhanced syntax error reporting with suggestions
3. **Error type classification** - Specific error types and error hierarchies
4. **Stack trace generation** - Execution stack trace generation and display
5. **Variable state snapshots** - Variable state capture at error points
6. **Error recovery suggestions** - Helpful suggestions for common error patterns
7. **Error history tracking** - Comprehensive error history for debugging
8. **Integration with debug mode** - Enhanced error reporting in debug mode

### Phase 3: Documentation and Integration (LOW PRIORITY)
6. **Documentation Updates**:
   - Update CLI documentation to reference existing language capabilities
   - Add examples showing how to use built-in performance, environment, and timing features
   - Cross-reference language features in CLI documentation

7. **Language Integration**:
   - Document how to use `$TIME().utc()` for performance profiling from CLI
   - Document how to use `$sys().getenv()` and `$sys().putenv()` for environment management
   - Document how to use parallel processing with `.map()` and thread counts

#### Language Capabilities Integration Examples

**Performance Profiling:**
```bash
# Built-in timing capabilities
grapa -c "
start_time = \$TIME().utc();
/* ... operations ... */
end_time = \$TIME().utc();
elapsed_ms = ((end_time - start_time) / 1000000).int();
('Performance: ' + elapsed_ms + ' ms').echo();
"
```

**Environment Management:**
```bash
# Environment variable access
grapa -c "\$sys().getenv('USERNAME').echo();"
grapa -c "\$sys().putenv('DEBUG_MODE', 'true');"

# Environment-based configuration
grapa -c "
env = \$sys().getenv('NODE_ENV');
if (env == 'production') {
    'Running in production mode'.echo();
} else {
    'Running in development mode'.echo();
}
"
```

**Parallel Processing:**
```bash
# Parallel data processing
grapa -c "
data = (10000).range(0,1);
processed = data.map(op(x) { x * x + 1; }, 4);  /* 4 workers */
('Processed ' + processed.len() + ' items').echo();
"
```

**Output Control:**
```bash
# Shell piping (already works)
grapa -c "'Hello World'.echo()" > output.txt
grapa -c "'Hello World'.echo()" | grep "Hello"

# Output formatting
grapa -c "
data = ['apple', 'banana', 'cherry'];
for (item in data) {
    ('Item: ' + item.upper()).echo();
}
"
```

**Dynamic Code Execution:**
```bash
# String templates and dynamic construction
grapa -c "
greeting = op('name'=0, 'time'=0){
    'Good ' + time + ', ' + name + '!'
};
greeting('Alice', 'morning').echo();
"

# Dynamic code execution
grapa -c "
template = 'name + \"! You are \" + age.str() + \" years old.\"';
name = 'Alice';
age = 25;
result = op()(template)();
result.echo();
"
```

**Error Handling:**
```bash
# Error handling with iferr
grapa -c "
iferr {
    result = 10 / 0;
    'This won\'t print'.echo();
} {
    ('Error occurred: ' + \$sys.error).echo();
}
"
```

**Database Operations:**
```bash
# Database operations
grapa -c "
db = \$file().table('ROW');
db.mkfield('name', 'STR');
db.mkfield('age', 'INT');
db.set('user1', 'Alice', 'name');
db.set('user1', 25, 'age');
result = db.get('user1', 'name');
('User name: ' + result).echo();
"
```

---

## 🚀 Implementation Strategy

### Design Principles
- **Python-inspired**: Follow Python CLI patterns where appropriate
- **Unix conventions**: Use standard option naming and behavior
- **Backward compatibility**: Maintain existing functionality during transition
- **User experience**: Prioritize intuitive and predictable behavior

### Migration Plan
1. **Deprecation Phase**: Add new options alongside old ones with deprecation warnings
2. **Transition Phase**: Update documentation and examples to use new options
3. **Removal Phase**: Remove deprecated options after sufficient notice

### Testing Strategy
- **Unit Tests**: Test each option individually
- **Integration Tests**: Test option combinations and edge cases
- **Cross-Platform Tests**: Ensure consistent behavior on Windows, Mac, Linux
- **User Acceptance Tests**: Validate with real-world usage scenarios

---

## 📋 References

- **Implementation Details**: `maintainers/IMPLEMENTATION/GRAPA_CLI_IMPLEMENTATION.md`
- **Bug Reports**: `maintainers/DEVELOPMENT/CLI_BUG_REPORTS.md`
- **Project Status**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`

---

*This comprehensive analysis provides the foundation for improving Grapa's CLI through standardization, enhancement, and better user experience.* 