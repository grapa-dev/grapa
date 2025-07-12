**See Also:** [Main Grep Documentation (grep.md)](grep.md)

> **Note:** All runnable code examples in this document are automatically tested in [`test/test_grep_python_doc_examples.py`](../../test/test_grep_python_doc_examples.py).

# Unicode Grep in Python

## Overview

The Unicode grep functionality in Grapa provides advanced text searching capabilities with full Unicode support, PCRE2-powered regular expressions, and comprehensive output options. This document focuses on using grep from Python via the GrapaPy extension.

## Key Features

### 🔍 **Unicode Support**
- Full Unicode character handling (Cyrillic, Chinese, Japanese, Korean, Arabic, Hebrew, Thai, etc.)
- Unicode normalization (NFC, NFD, NFKC, NFKD)
- **Advanced Unicode properties** (`\p{L}`, `\p{N}`, `\p{Emoji}`, `\p{So}`, etc.)
- **Unicode grapheme clusters** (`\X`)
- Case-insensitive matching with proper Unicode case folding

### 🎯 **Advanced Regex Features**
- PCRE2-powered regular expressions
- Named groups (`(?P<name>...)`)
- Unicode properties and script extensions
- Atomic groups (`(?>...)`)
- **Lookaround assertions** (`(?=...)`, `(?<=...)`, `(?!...)`, `(?<!...)`)
- **Possessive quantifiers** (`*+`, `++`, `?+`, `{n,m}+`)
- **Conditional patterns** (`?(condition)...`)
- Unicode categories (`\p{L}`, `\p{N}`, `\p{Z}`, `\p{P}`, `\p{S}`, `\p{C}`, `\p{M}`)
- Unicode scripts (`\p{sc=Latin}`, `\p{sc=Han}`, etc.)
- Unicode script extensions (`\p{scx:Han}`, etc.)
- Unicode general categories (`\p{Lu}`, `\p{Ll}`, etc.)

### 📊 **Output Formats**
- Standard text output
- JSON output with named groups, offsets, and line numbers
- **Context lines** (before/after matches)
- Line numbers and byte offsets
- Match-only or full-line output

## Installation

```bash
pip install grapapy
```

## Basic Usage

### Method 1: Embedded Parameters
Pass parameters directly in the Grapa string:

```python
import grapapy
xy = grapapy.grapa()

# Simple pattern matching
xy.eval('"Hello world".grep("world")')
# Result: ['Hello world']

# Case-insensitive matching
xy.eval('"Hello WORLD".grep("world", "i")')
# Result: ['Hello WORLD']

# Match-only output
xy.eval('"Hello world".grep(r"\\w+", "o")')
# Result: ['Hello', 'world']
```

### Method 2: Parameter Variables
Pass parameters as Python variables to be used in the Grapa string:

```python
import grapapy
xy = grapapy.grapa()

# For parameter variables, always provide the variables dictionary:
# Basic grep with parameter variables
xy.eval("input.grep(pattern,options);", {
    "input": "apple 123 pear 456\nbanana 789",
    "pattern": "\d+",
    "options": "o"
})
# Result: ['123', '456', '789']

# With all parameters as variables
xy.eval("input.grep(pattern,options,delimiter,normalization,mode);", {
    "input": "data1\\x00data2\\x00data3",
    "pattern": "data\\d+",
    "options": "o",
    "delimiter": "\\x00",
    "normalization": "NONE",
    "mode": "BINARY"
})
# Result: ['data1', 'data2', 'data3']
```

## Syntax

```python
# Method 1: Embedded parameters
xy.eval('"string".grep(pattern, options, delimiter, normalization, mode)')

# Method 2: Parameter variables
xy.eval("string.grep(pattern, options, delimiter, normalization, mode);", {
    "string": "input_text",
    "pattern": "regex_pattern",
    "options": "option_flags",
    "delimiter": "line_delimiter",
    "normalization": "normalization_form",
    "mode": "processing_mode"
})
```

### Parameters

**Required Parameters:**
- **string**: The input text to search
- **pattern**: PCRE2 regular expression pattern with Unicode support

**Optional Parameters (all have sensible defaults):**
- **options**: String containing option flags (default: `""` - no options)
- **delimiter**: Custom line delimiter (default: `"\n"`)
- **normalization**: Unicode normalization form: `"NONE"`, `"NFC"`, `"NFD"`, `"NFKC"`, `"NFKD"` (default: `"NONE"`)
- **mode**: Processing mode: `"UNICODE"` for full Unicode processing, `"BINARY"` for raw byte processing (default: `"UNICODE"`)
- **num_workers**: Number of worker threads for parallel processing: `0` for auto-detection, `1` for sequential, `2+` for parallel (default: `0`)

## Options Reference

### Basic Options

| Option | Description | Python Example |
|--------|-------------|----------------|
| `a` | All-mode (match across full input string, context options ignored) | `xy.eval('"text".grep("pattern", "a")')` |
| `i` | Case-insensitive matching | `xy.eval('"Text".grep("text", "i")')` |
| `v` | Invert match (return lines that do NOT match the pattern) | `xy.eval('"text".grep("pattern", "v")')` |
| `x` | Exact line match (whole line must match) | `xy.eval('"text".grep("^text$", "x")')` |
| `N` | Normalize input and pattern to NFC | `xy.eval('"caf\u00e9".grep("cafe", "N")')` |
| `d` | Diacritic-insensitive matching | `xy.eval('"caf\u00e9".grep("cafe", "d")')` |

> **Note:** Unicode normalization (N, or normalization parameter) does **not** remove diacritics or accents. It only canonicalizes Unicode forms. To match characters with and without accents (e.g., `cafe` vs `café`), you must use the `d` option for diacritic-insensitive matching.

### Diacritic-Insensitive Matching (`d` option)

The `d` option enables **diacritic-insensitive matching**. When enabled, both the input and the pattern are:
1. **Unicode normalized** (NFC by default, or as specified)
2. **Case folded** (Unicode-aware, not just ASCII)
3. **Diacritics/accents are stripped** (works for Latin, Greek, Cyrillic, Turkish, Vietnamese, and more)

> **Note:** If you want `"cafe"` to match `"café"`, you must use the `d` option. Normalization alone is not sufficient.

### Output Options

| Option | Description | Python Example |
|--------|-------------|----------------|
| `o` | Match-only (output only matched text) | `xy.eval('"Hello world".grep(r"\\w+", "o")')` |
| `n` | Prefix matches with line numbers | `xy.eval('"text".grep("pattern", "n")')` |
| `l` | Line number only output | `xy.eval('"text".grep("pattern", "l")')` |
| `b` | Output byte offset with matches | `xy.eval('"text".grep("pattern", "b")')` |
| `j` | JSON output format with named groups, offsets, and line numbers | `xy.eval('"text".grep("pattern", "oj")')` |

### Context Options

| Option | Description | Python Example |
|--------|-------------|----------------|
| `A<n>` | Show n lines after match | `xy.eval('"text".grep("pattern", "A2")')` |
| `B<n>` | Show n lines before match | `xy.eval('"text".grep("pattern", "B1")')` |
| `C<n>` | Show n lines before and after | `xy.eval('"text".grep("pattern", "C3")')` |

### Processing Options

| Option | Description | Python Example |
|--------|-------------|----------------|
| `c` | Count of matches | `xy.eval('"text".grep("pattern", "c")')` |
| `d` | Deduplicate results | `xy.eval('"text".grep("pattern", "d")')` |
| `g` | Group results per line | `xy.eval('"text".grep("pattern", "g")')` |

## Unicode Examples

### Basic Unicode Support

```python
import grapapy
xy = grapapy.grapa()

# Unicode characters
xy.eval('"Привет мир".grep("мир")')
# Result: ['Привет мир']

# Unicode properties
xy.eval('"Hello 世界 123 €".grep(r"\\p{L}+", "o")')
# Result: ['Hello', '世界']

# Emoji handling
xy.eval('"Hello 👋 world 🌍".grep(r"(?:\\p{So}(?:\\u200D\\p{So})*)+", "o")')
# Result: ['👋', '🌍']

# Unicode grapheme clusters
xy.eval('"café mañana".grep(r"\\X", "o")')
# Result: ['c', 'a', 'f', 'é', ' ', 'm', 'a', 'ñ', 'a', 'n', 'a']
```

### Advanced Unicode Features

```python
import grapapy
xy = grapapy.grapa()

# Named groups with JSON output
xy.eval('"John Doe (30)".grep(r"(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")')
# Result: [{"match":"John Doe (30)","first":"John","last":"Doe","age":"30","offset":0,"line":1}]

# Unicode scripts
xy.eval('"Hello 世界 こんにちは 안녕하세요".grep(r"\\p{sc=Han}+", "o")')
# Result: ['世界']

# Atomic groups
xy.eval('"aaa".grep(r"(?>a+)a", "o")')
# Result: [] (atomic group prevents backtracking)

# Lookaround assertions
xy.eval('"abc123def456".grep(r"(?<=\\d)(?=\\d)", "o")')
# Result: ['', '', '', '', '', '']
```

## Raw String Literals

For better readability of regex patterns, use raw string literals by prefixing the string with `r`:

```python
import grapapy
xy = grapapy.grapa()

# Without raw string (requires escaping)
xy.eval('"file.txt".grep("^[a-zA-Z0-9_]+\\.txt$", "x")')
# Result: ['file.txt']

# With raw string (no escaping needed)
xy.eval('"file.txt".grep(r"^[a-zA-Z0-9_]+\\.txt$", "x")')
# Result: ['file.txt'] - Much cleaner!

# Complex patterns benefit greatly
xy.eval('"user@domain.com".grep(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "x")')
# Result: ['user@domain.com']
```

## Performance Considerations

### Optimization Features

```python
import grapapy
xy = grapapy.grapa()

# JIT compilation for complex patterns
xy.eval('"large_text".grep(r"complex_pattern", "i")')
# Automatically uses PCRE2 JIT compilation

# Fast path for simple patterns
xy.eval('"text".grep("literal", "o")')
# Uses optimized literal matching

# LRU caching for repeated operations
xy.eval('"text".grep("pattern", "N")')
# Normalization results are cached
```

### Best Practices

1. **Use raw strings** for regex patterns to avoid escaping issues
2. **Combine options** when possible (e.g., `"oi"` for match-only case-insensitive)
3. **Use appropriate normalization** for your use case
4. **Consider binary mode** for non-text data
5. **Use JSON output** for structured data processing

## Error Handling

```python
import grapapy
xy = grapapy.grapa()

try:
    # Invalid regex pattern
    result = xy.eval('"text".grep("(", "o")')
except Exception as e:
    print(f"Regex error: {e}")

try:
    # Invalid normalization
    result = xy.eval('"text".grep("pattern", "", "\\n", "INVALID", "UNICODE")')
except Exception as e:
    print(f"Parameter error: {e}")
```

## Zero-Length Match and Empty String Output

> **Update (2024-06):** As of this release, zero-length matches and explicit empty strings in arrays are always output as `""` (empty string), never as `null`. This matches ripgrep’s behavior and ensures round-trip consistency and correct scripting semantics.

### Example: Zero-Length Match
```python
xy.eval('"a\nb\n".grep("^", "o")')
# Result: ["", "a", "", "b", ""]
```

### Example: Array Literal with Empty String
```python
xy.eval('[1, "", 2]')
# Result: [1, "", 2]
```

## Test Coverage and Regression Testing

> **Update (2024-06):** The test suite now includes explicit checks for empty string vs null output, zero-length matches, and all advanced edge cases to ensure full ripgrep parity (excluding file system features). See [Testing Documentation](../../docs/TESTING.md) for details.

## Comparison with Other Tools

| Feature | Grapa Grep | Python re | grep | ripgrep |
|---------|------------|-----------|------|---------|
| Unicode support | ✅ Full | ✅ Basic | ❌ Limited | ✅ Good |
| Diacritic-insensitive | ✅ Yes | ❌ No | ❌ No | ❌ No |
| Named groups | ✅ Yes | ✅ Yes | ❌ No | ✅ Yes |
| JSON output | ✅ Yes | ❌ No | ❌ No | ✅ Yes |
| Context lines | ✅ Yes | ❌ No | ✅ Yes | ✅ Yes |
| Performance | ✅ Fast | ✅ Fast | ✅ Fast | ✅ Very Fast |

## Integration Examples

### File Processing

```python
import grapapy
xy = grapapy.grapa()

# Read file and process with grep
with open('data.txt', 'r', encoding='utf-8') as f:
    content = f.read()

# Find all email addresses
emails = xy.eval("content.grep(r'[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}', 'o');", {
    "content": content
})

# Find lines with context
matches = xy.eval("content.grep(r'error|warning|critical', 'A2B1');", {
    "content": content
})
```

### Data Analysis

```python
import grapapy
xy = grapapy.grapa()

# Extract structured data
log_data = """
2024-01-15 10:30:15 INFO User login successful
2024-01-15 10:31:22 ERROR Database connection failed
2024-01-15 10:32:45 WARN High memory usage detected
"""

# Extract timestamps and log levels
entries = xy.eval("log_data.grep(r'(?P<timestamp>\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}) (?P<level>\\w+) (?P<message>.*)', 'oj');", {
    "log_data": log_data
})

# Result: [{"timestamp":"2024-01-15 10:30:15","level":"INFO","message":"User login successful",...}, ...]
```

For more detailed information about grep functionality, see the main [Grep Documentation](obj/grep_python.md). 

## Example Test File

See `test/test_grep_python_examples.py` for a runnable example of grep usage from Python. 

For detailed grep documentation, see [Grep Documentation](grep.md).

## Feature Comparison and Limitations

For a comprehensive comparison of Grapa grep features with other tools like ripgrep, GNU grep, and Python's `re` module, including detailed feature matrices and use case recommendations, see the [Main Grep Documentation](grep.md).

The Python interface provides access to all Grapa grep features, including:
- ✅ **Full Unicode support** with grapheme clusters and normalization
- ✅ **Advanced regex features** (atomic groups, lookarounds, possessive quantifiers)
- ✅ **Diacritic-insensitive matching** (unique to Grapa)
- ✅ **JSON output** with named groups and metadata
- ✅ **Context lines** and flexible output options

**Note:** All search strategy features have functional equivalents in Grapa grep. Smart-case matching is achieved by using "i" flag for lowercase patterns and no flag for uppercase patterns. Column numbers are available via byte offsets with the "b" option. File handling features are intentionally handled by the Grapa language environment rather than within the grep function itself, providing more flexibility and control. 

## Ripgrep Compatibility

**✅ FULL RIPGREP PARITY ACHIEVED** - Grapa grep has achieved complete parity with ripgrep for all in-memory/streaming features (excluding file system features).

### Supported Features

All core grep functionality is fully implemented and tested:
- ✅ **Context lines** with proper merging and separators
- ✅ **Match-only output** with comprehensive Unicode support
- ✅ **Case-insensitive and diacritic-insensitive matching**
- ✅ **Word boundaries, column numbers, and color output**
- ✅ **Custom delimiters and Unicode normalization**
- ✅ **Parallel processing and performance optimizations**
- ✅ **Graceful error handling and robust edge case coverage**

### Production Ready

Grapa grep is production-ready and provides:
- **Robust error handling** - Invalid patterns return empty results instead of crashing
- **High performance** - JIT compilation, parallel processing, and fast path optimizations
- **Complete Unicode support** - Full Unicode property and script support
- **Comprehensive testing** - All features thoroughly tested with edge cases
- **Ripgrep compatibility** - Matches ripgrep behavior for all supported features

### Known Limitations

**File System Features:**
- ❌ File searching (not implemented by design)
- ❌ Directory traversal (not implemented by design)

**Scripting Layer Issues:**
- ⚠️ Zero-length matches return `[null]` instead of `[""]`
- ⚠️ Unicode string functions (`len()`, `ord()`) count bytes not characters
- ⚠️ Null-data mode limited by string parser (`\x00` not converted) 