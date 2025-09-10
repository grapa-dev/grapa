# Grapa Syntax Checker

A comprehensive syntax validation tool written entirely in Grapa that demonstrates advanced language capabilities including string manipulation, pattern matching, file I/O, and error detection.

## Overview

The Grapa Syntax Checker is a powerful example that showcases Grapa's ability to handle complex text processing tasks. It can analyze Grapa source code files and detect various types of syntax errors, providing detailed reports with suggestions for fixes.

## Features

### 🔍 **Comprehensive Error Detection**
- **Bracket Matching**: Detects unmatched, unclosed, and mismatched brackets
- **Missing Semicolons**: Identifies missing statement terminators
- **String Literals**: Validates proper string quote matching
- **Operator Usage**: Detects consecutive and misplaced operators
- **Statement Structure**: Validates control flow statement syntax
- **Function Calls**: Checks proper function call syntax

### 📊 **Detailed Reporting**
- **Error Categorization**: Groups errors by type for easy analysis
- **Line Numbers**: Provides exact line and position information
- **Suggestions**: Offers specific fix recommendations
- **Summary Statistics**: Shows token counts and error summaries

### ⚡ **Performance Features**
- **Parallel Processing**: Uses `.map()` for concurrent file checking
- **Batch Operations**: Check multiple files simultaneously
- **Directory Scanning**: Automatically find and check all `.grc` files

## Usage

### Basic Usage

```grapa
// Create syntax checker instance
checker = $SyntaxChecker();

// Check a single file
result = checker.check("my_script.grc");
result.echo();
```

### CLI Usage

The syntax checker can be run directly from the command line:

```bash
# Check a single file
grapa working_demo.grc check my_script.grc

# Show help
grapa working_demo.grc help
```

### Batch File Checking

```grapa
// Check multiple files
files = ["script1.grc", "script2.grc", "script3.grc"];
results = checker.checkFiles(files);
results.summary.echo();
```

### Directory Checking

```grapa
// Check all .grc files in a directory
results = checker.checkDirectory("docs-src/docs/examples");
results.summary.echo();
```

## Error Types Detected

### 1. **Bracket Errors**
```grapa
// Unclosed bracket
if (condition) {
    "code";
// Missing closing brace

// Mismatched brackets
array = [1, 2, 3];  // Should use { } for objects
```

### 2. **Missing Semicolons**
```grapa
// Missing semicolons
x = 10
return 42
break
```

### 3. **String Literal Errors**
```grapa
// Unterminated strings
message = "Hello World
name = 'John Doe
```

### 4. **Operator Issues**
```grapa
// Consecutive operators
result = a + + b;

// Misplaced operators
+ 5;  // Operator at start of line
```

### 5. **Statement Structure**
```grapa
// Missing parentheses
if condition {  // Should be if (condition)
    "code";
}
```

## Example Output

### Successful Check
```json
{
  "status": "success",
  "message": "✅ No syntax errors found in script.grc",
  "file": "script.grc",
  "tokens": 45,
  "errors": [],
  "summary": "All syntax checks passed!"
}
```

### Error Report
```json
{
  "status": "error",
  "message": "❌ Found 3 syntax error(s) in script.grc",
  "file": "script.grc",
  "tokens": 42,
  "errors": [
    {
      "type": "unclosed_bracket",
      "message": "Unclosed bracket '{'",
      "line": 5,
      "position": 12,
      "suggestion": "Add closing bracket '}'"
    },
    {
      "type": "missing_semicolon",
      "message": "Missing semicolon after 'return' statement",
      "line": 8,
      "position": 15,
      "suggestion": "Add semicolon: return 42;"
    }
  ],
  "summary": "Found 3 syntax error(s):\n  - unclosed_bracket: 1 error(s)\n  - missing_semicolon: 2 error(s)"
}
```

## Technical Implementation

### Tokenization Process
The syntax checker uses a sophisticated tokenization system that:

1. **Removes Comments**: Handles both line (`//`) and block (`/* */`) comments
2. **Extracts Tokens**: Identifies keywords, operators, strings, numbers, and identifiers
3. **Preserves Context**: Maintains line numbers and positions for error reporting

### Validation Engine
The validation system performs multiple checks:

1. **Bracket Balancing**: Uses a stack-based algorithm to verify bracket matching
2. **Statement Analysis**: Validates control flow statement structure
3. **Operator Validation**: Detects consecutive and misplaced operators
4. **String Validation**: Ensures proper string literal formatting

### Error Reporting
The reporting system provides:

1. **Categorization**: Groups errors by type for better analysis
2. **Context**: Includes line numbers and character positions
3. **Suggestions**: Offers specific fix recommendations
4. **Statistics**: Provides summary information

## Advanced Features

### Custom Rule Support
The syntax checker can be extended with custom validation rules:

```grapa
// Add custom keyword
checker.rules.keywords.push("custom_keyword");

// Add custom operator
checker.rules.operators.push("custom_op");
```

### Parallel Processing
Leverages Grapa's threading capabilities for performance:

```grapa
// Check multiple files in parallel
results = files.map(op(file) { checker.check(file); });
```

### Integration Examples
Can be integrated into build systems or development workflows:

```grapa
// Pre-commit syntax check
function preCommitCheck() {
    results = checker.checkDirectory("src");
    if (results.errorCount > 0) {
        "❌ Syntax errors found. Commit aborted.";
        results.summary.echo();
        return false;
    }
    "✅ All syntax checks passed.";
    return true;
}
```

## Testing

The syntax checker includes comprehensive test cases in the `bad_syntax/` directory:

- **`unmatched_brackets.grc`**: Various bracket matching errors
- **`missing_semicolons.grc`**: Missing semicolon scenarios
- **`unterminated_strings.grc`**: String literal issues
- **`invalid_operators.grc`**: Operator usage problems
- **`invalid_statements.grc`**: Control structure errors
- **`mixed_errors.grc`**: Multiple error types in realistic scenarios

## Educational Value

This example demonstrates:

1. **Advanced String Processing**: Complex pattern matching and text analysis
2. **Data Structure Usage**: Lists, arrays, and objects for organizing data
3. **Error Handling**: Comprehensive error detection and reporting
4. **Algorithm Implementation**: Stack-based bracket matching, tokenization
5. **Real-World Application**: Practical tool for development workflows

## Performance Considerations

- **Memory Efficient**: Processes files line by line to minimize memory usage
- **Fast Tokenization**: Optimized string operations for quick parsing
- **Parallel Processing**: Leverages Grapa's threading for batch operations
- **Scalable**: Can handle large codebases efficiently

## Future Enhancements

Potential improvements could include:

1. **Semantic Analysis**: Variable usage validation, type checking
2. **Style Checking**: Code formatting and style guideline enforcement
3. **Auto-fixing**: Automatic correction of common syntax errors
4. **IDE Integration**: Real-time syntax checking in development environments
5. **Custom Rules**: User-defined syntax validation rules

## Conclusion

The Grapa Syntax Checker is an excellent example of Grapa's capabilities for complex text processing and analysis. It demonstrates how Grapa can be used to build sophisticated tools that provide real value to developers while showcasing the language's advanced features.

This implementation serves as both a practical development tool and a comprehensive learning resource for understanding advanced Grapa programming techniques.
