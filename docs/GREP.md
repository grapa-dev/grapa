# Grapa Grep Documentation

## Overview

Grapa grep provides comprehensive pattern matching functionality with full Unicode support, ripgrep compatibility, and advanced features like context lines, custom delimiters, and parallel processing.

## Basic Usage

```grapa
// Basic pattern matching
"Hello world".grep("world")
["Hello world"]

// Match-only output
"Hello world".grep("world", "o")
["world"]

// Case-insensitive matching
"Hello WORLD".grep("world", "i")
["Hello WORLD"]
```

## Function Signature

```grapa
input.grep(pattern, options, delimiter, normalization, mode, num_workers)
```

### Parameters

- **input**: String to search in
- **pattern**: Regex pattern to match
- **options**: String of option flags (see Options section)
- **delimiter**: Custom line delimiter (default: newline)
- **normalization**: Unicode normalization form ("NONE", "NFC", "NFD", "NFKC", "NFKD")
- **mode**: Processing mode ("UNICODE" or "BINARY")
- **num_workers**: Number of parallel workers (0 = auto-detect)

## Options

### Output Options

| Option | Description | Example |
|--------|-------------|---------|
| `o` | Match-only output (extract matches only) | `"Hello world".grep("\\w+", "o")` → `["Hello", "world"]` |
| `j` | JSON output format | `"Hello world".grep("world", "j")` → JSON object |
| `n` | Include line numbers | `"Line 1\nLine 2".grep("Line", "n")` → `["1:Line 1", "2:Line 2"]` |
| `l` | Files with matches only | Returns array of matching lines |
| `c` | Count only | Returns count of matches |

### Matching Options

| Option | Description | Example |
|--------|-------------|---------|
| `i` | Case-insensitive matching | `"Hello WORLD".grep("world", "i")` |
| `d` | Diacritic-insensitive matching | `"café".grep("cafe", "d")` |
| `v` | Invert match (non-matching lines) | `"Line 1\nLine 2".grep("Line 1", "v")` → `["Line 2"]` |
| `x` | Exact match (whole line) | `"Hello".grep("^Hello$", "x")` |
| `w` | Word boundaries | `"foo bar".grep("foo", "w")` |
| `a` | All-mode (treat input as single line) | `"Line 1\nLine 2".grep("Line.*Line", "a")` |

### Context Options

| Option | Description | Example |
|--------|-------------|---------|
| `A<n>` | After context (n lines after match) | `"Line 1\nLine 2\nLine 3".grep("Line 2", "A1")` |
| `B<n>` | Before context (n lines before match) | `"Line 1\nLine 2\nLine 3".grep("Line 2", "B1")` |
| `C<n>` | Context (n lines before and after) | `"Line 1\nLine 2\nLine 3".grep("Line 2", "C1")` |

### Special Options

| Option | Description | Example |
|--------|-------------|---------|
| `T` | Column output | `"foo bar".grep("foo", "T")` |
| `z` | Null-data mode (split on null bytes) | `"data1\x00data2".grep("data", "z")` |
| `L` | Color output | `"Hello world".grep("world", "L")` |
| `N` | Unicode normalization | `"café".grep("cafe", "N")` |

## Unicode Support

### Normalization Forms

```grapa
// NFC normalization (default)
"café".grep("cafe", "NFC")

// NFD normalization
"café".grep("cafe", "NFD")

// NFKC normalization
"café".grep("cafe", "NFKC")

// NFKD normalization
"café".grep("cafe", "NFKD")
```

### Unicode Properties

```grapa
// Match letters
"Hello 世界 123".grep("\\p{L}+", "o")
["Hello", "世界"]

// Match numbers
"Hello 世界 123".grep("\\p{N}+", "o")
["123"]

// Match word characters
"Hello 世界 123".grep("\\w+", "o")
["Hello", "123"]
```

### Diacritic-Insensitive Matching

```grapa
// Match café, cafe, café, etc.
"café résumé naïve".grep("cafe", "d")
["café résumé naïve"]
```

## Context Lines

### After Context

```grapa
input = "Header\nLine 1\nLine 2\nLine 3\nFooter";
input.grep("Line 2", "A1")
["Line 2\n", "Line 3\n"]
```

### Before Context

```grapa
input.grep("Line 2", "B1")
["Line 1\n", "Line 2\n"]
```

### Combined Context

```grapa
input.grep("Line 2", "A1B1")
["Line 1\n", "Line 2\n", "Line 3\n"]
```

### Context Precedence

- `C<n>` takes precedence over `A<n>` and `B<n>`
- Only the last occurrence of each context option is used
- Example: `A1B2C3` uses `C3` (3 lines before and after)

## Custom Delimiters

```grapa
// Pipe-delimited input
"Line 1|Line 2|Line 3".grep("Line 2", "", "|")
["Line 2"]

// Tab-delimited input
"Line 1\tLine 2\tLine 3".grep("Line 2", "", "\t")
["Line 2"]

// Multi-character delimiter
"Line 1\r\nLine 2\r\nLine 3".grep("Line 2", "", "\r\n")
["Line 2"]
```

## Binary Mode

```grapa
// Process as binary data (no Unicode processing)
binary_data.grep("pattern", "", "", "", "BINARY")
```

## Parallel Processing

```grapa
// Auto-detect number of workers
large_input.grep("pattern", "o", "", "", "", "", 0)

// Use 4 workers
large_input.grep("pattern", "o", "", "", "", "", 4)

// Sequential processing
large_input.grep("pattern", "o", "", "", "", "", 1)
```

## Error Handling

### Exception-Based Error Handling

All errors are thrown as exceptions and caught by the Grapa scripting layer:

```grapa
// Invalid regex pattern
result = input.grep("invalid[", "o");
if (result.type() == $ERR) {
    "Error: " + result.str() + "\n".echo();
}

// Empty pattern
result = input.grep("", "o");
if (result.type() == $ERR) {
    "Error: " + result.str() + "\n".echo();
}
```

### Common Error Messages

- `"Invalid regex pattern"` - Malformed regex pattern
- `"Empty pattern is not allowed"` - Empty pattern provided

## Ripgrep Compatibility

Grapa grep achieves full parity with ripgrep (excluding file system features):

### ✅ Supported Features

- **Context lines** (A<n>, B<n>, C<n>) with proper precedence
- **Match-only output** ("o" option) for all scenarios
- **Case-insensitive matching** ("i" option)
- **Diacritic-insensitive matching** ("d" option)
- **Invert match** ("v" option)
- **All-mode** ("a" option)
- **JSON output** ("j" option)
- **Line numbers** ("n" option)
- **Word boundaries** ("w" option)
- **Custom delimiters**
- **Unicode normalization**
- **Parallel processing**
- **Error handling**
- **Option precedence** (ripgrep-style precedence rules)

### ⚠️ Known Differences

- **Zero-length match output**: Returns `[null, null]` instead of `["", ""]` due to Grapa scripting layer behavior
- **File system features**: Not implemented (file searching, directory traversal, etc.)

## Performance Features

### JIT Compilation

PCRE2 JIT compilation is automatically enabled for better performance.

### Fast Path Optimizations

- **Literal patterns**: Optimized for simple string matching
- **Word patterns**: Optimized for word boundary matching
- **Digit patterns**: Optimized for numeric matching

### LRU Cache

Text normalization results are cached for improved performance.

### Parallel Processing

Large inputs are automatically processed in parallel for better performance.

## Examples

### Basic Examples

```grapa
// Find lines containing "error"
log_content.grep("error")

// Find lines containing "error" (case-insensitive)
log_content.grep("error", "i")

// Extract only the "error" matches
log_content.grep("error", "o")

// Find lines NOT containing "error"
log_content.grep("error", "v")
```

### Advanced Examples

```grapa
// Find "error" with 2 lines of context
log_content.grep("error", "A2B2")

// Find word "error" (word boundaries)
log_content.grep("error", "w")

// Find "error" in JSON format
log_content.grep("error", "j")

// Find "error" with line numbers
log_content.grep("error", "n")

// Count "error" occurrences
log_content.grep("error", "c")
```

### Unicode Examples

```grapa
// Match Unicode letters
text.grep("\\p{L}+", "o")

// Case-insensitive Unicode matching
"Café RÉSUMÉ".grep("café", "i")

// Diacritic-insensitive matching
"café résumé naïve".grep("cafe", "d")

// Unicode normalization
"café".grep("cafe", "NFC")
```

### Context Examples

```grapa
// Show 3 lines before and after matches
log_content.grep("error", "C3")

// Show 2 lines after matches only
log_content.grep("error", "A2")

// Show 1 line before matches only
log_content.grep("error", "B1")

// Complex context with other options
log_content.grep("error", "A2B1io")
```

### Custom Delimiter Examples

```grapa
// Pipe-delimited data
"field1|field2|field3".grep("field2", "", "|")

// Tab-delimited data
"field1\tfield2\tfield3".grep("field2", "", "\t")

// Custom multi-character delimiter
"record1<DELIM>record2<DELIM>record3".grep("record2", "", "<DELIM>")
```

## Best Practices

1. **Use appropriate options**: Choose the right options for your use case
2. **Handle errors**: Always check for `$ERR` results
3. **Use Unicode normalization**: For international text, consider using normalization
4. **Optimize performance**: Use parallel processing for large inputs
5. **Test edge cases**: Verify behavior with empty inputs, special characters, etc.

## Performance Tips

1. **Use literal patterns** when possible for better performance
2. **Enable parallel processing** for large inputs
3. **Use appropriate normalization** (NFC is usually sufficient)
4. **Cache results** when processing the same input multiple times
5. **Use binary mode** for non-text data 