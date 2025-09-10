# Bad Syntax Examples

This directory contains Grapa script files with intentional syntax errors to demonstrate the capabilities of the Grapa Syntax Checker.

## Purpose

These files serve as test cases for the syntax checker, showing various types of syntax errors that can occur in Grapa code. They help validate that the syntax checker can properly detect and report different categories of syntax issues.

## Files

### `unmatched_brackets.grc`
Demonstrates bracket matching errors:
- Unclosed opening brackets
- Unexpected closing brackets
- Mismatched bracket types (using `]` instead of `}`)
- Multiple nested bracket issues

### `missing_semicolons.grc`
Demonstrates missing semicolon errors:
- Missing semicolons after variable assignments
- Missing semicolons after control flow statements (`return`, `break`, `continue`, `throw`)
- Mixed semicolon usage in the same file

### `unterminated_strings.grc`
Demonstrates string literal errors:
- Missing closing quotes (both single and double)
- Mismatched quote types
- Strings spanning multiple lines without proper escaping
- Strings in various contexts (assignments, function calls, arrays)

### `invalid_operators.grc`
Demonstrates operator usage errors:
- Consecutive operators (`+ +`, `- -`, `* *`)
- Misplaced operators (at start/end of lines)
- Invalid assignment operators (`=+` instead of `+=`)
- Spaces in operators (`= =` instead of `==`)

### `invalid_statements.grc`
Demonstrates control structure errors:
- Missing parentheses in `if`, `while`, `for` statements
- Invalid function call syntax
- Malformed control flow statements
- Invalid use of `break`/`continue` outside loop context

### `mixed_errors.grc`
Demonstrates multiple error types in realistic scenarios:
- Combination of bracket, semicolon, string, and operator errors
- Nested structures with multiple issues
- Real-world scenarios where multiple syntax problems exist

## Usage

These files are designed to be used with the Grapa Syntax Checker:

```grapa
checker = $SyntaxChecker();

// Check a specific bad example
result = checker.check("bad_syntax/unmatched_brackets.grc");
result.echo();

// Check all bad examples
badFiles = ["unmatched_brackets.grc", "missing_semicolons.grc", "unterminated_strings.grc", "invalid_operators.grc", "invalid_statements.grc", "mixed_errors.grc"];
results = checker.checkFiles(badFiles);
results.summary.echo();
```

## Expected Results

Each file should produce multiple syntax errors when checked, demonstrating the syntax checker's ability to:
- Detect different types of syntax issues
- Provide accurate line numbers and positions
- Suggest fixes for common problems
- Handle multiple errors in a single file
- Categorize errors by type

## Learning Value

These examples help developers understand:
- Common syntax mistakes in Grapa
- How the syntax checker identifies problems
- Best practices for writing valid Grapa code
- Error reporting and debugging techniques
