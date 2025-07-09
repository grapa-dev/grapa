# Unicode Grep Documentation

## Overview

The Unicode grep functionality in Grapa provides advanced text searching capabilities with full Unicode support, PCRE2-powered regular expressions, and comprehensive output options. It's designed to handle international text, emoji, and complex Unicode properties while maintaining high performance.

## Key Features

### 🔍 **Unicode Support**
- Full Unicode character handling (Cyrillic, Chinese, Japanese, Korean, Arabic, Hebrew, Thai, etc.)
- Unicode normalization (NFC, NFD, NFKC, NFKD)
- Unicode properties (`\p{L}`, `\p{N}`, `\p{Emoji}`, etc.)
- Unicode grapheme clusters (`\X`)
- Case-insensitive matching with proper Unicode case folding

### 🎯 **Advanced Regex Features**
- PCRE2-powered regular expressions
- Named groups (`(?P<name>...)`)
- Lookaround assertions (`(?=...)`, `(?<=...)`, `(?!...)`, `(?<!...)`)
- Unicode properties and script extensions
- Atomic groups and possessive quantifiers
- Conditional patterns and recursive matching

### 📊 **Output Formats**
- Standard text output
- JSON output with named groups, offsets, and line numbers
- Context lines (before/after matches)
- Line numbers and byte offsets
- Match-only or full-line output

## Syntax

```grapa
string.grep(pattern, options, delimiter, normalization, mode)
```

### Parameters

- **string**: The input text to search
- **pattern**: PCRE2 regular expression pattern with Unicode support
- **options**: String containing option flags
- **delimiter**: Custom line delimiter (defaults to `\n`)
- **normalization**: Unicode normalization form: `"NONE"`, `"NFC"`, `"NFD"`, `"NFKC"`, `"NFKD"` (default: `"NONE"`)
- **mode**: Processing mode: `"UNICODE"` for full Unicode processing, `"BINARY"` for raw byte processing (default: `"UNICODE"`)

## Options Reference

### Basic Options

| Option | Description | Example |
|--------|-------------|---------|
| `a` | All-mode (match across full input string) | `"text".grep("pattern", "a")` |
| `i` | Case-insensitive matching | `"Text".grep("text", "i")` |
| `v` | Invert match (return non-matching lines) | `"text".grep("pattern", "v")` |
| `x` | Exact line match (whole line must match) | `"text".grep("^text$", "x")` |
| `N` | Normalize input and pattern to NFC | `"café".grep("cafe", "N")` |

### Output Options

| Option | Description | Example |
|--------|-------------|---------|
| `o` | Match-only (output only matched text) | `"Hello world".grep("\\w+", "o")` |
| `n` | Prefix matches with line numbers | `"text".grep("pattern", "n")` |
| `l` | Line number only output | `"text".grep("pattern", "l")` |
| `b` | Output byte offset with matches | `"text".grep("pattern", "b")` |
| `j` | JSON output format | `"text".grep("pattern", "oj")` |

### Context Options

| Option | Description | Example |
|--------|-------------|---------|
| `A<n>` | Show n lines after match | `"text".grep("pattern", "A2")` |
| `B<n>` | Show n lines before match | `"text".grep("pattern", "B1")` |
| `C<n>` | Show n lines before and after | `"text".grep("pattern", "C3")` |

### Processing Options

| Option | Description | Example |
|--------|-------------|---------|
| `c` | Count of matches | `"text".grep("pattern", "c")` |
| `d` | Deduplicate results | `"text".grep("pattern", "d")` |
| `g` | Group results per line | `"text".grep("pattern", "g")` |

## Additional Parameters

### Unicode Normalization

The `normalization` parameter controls Unicode normalization:

| Value | Description | Use Case |
|-------|-------------|----------|
| `"NONE"` | No normalization (default) | Standard text processing |
| `"NFC"` | Normalization Form Canonical Composition | Most common for text storage |
| `"NFD"` | Normalization Form Canonical Decomposition | Unicode analysis |
| `"NFKC"` | Normalization Form Compatibility Composition | Search and matching |
| `"NFKD"` | Normalization Form Compatibility Decomposition | Compatibility processing |

### Processing Mode

The `mode` parameter controls how the input is processed:

| Value | Description | Use Case |
|-------|-------------|----------|
| `"UNICODE"` | Full Unicode processing (default) | Text files, user input |
| `"BINARY"` | Raw byte processing | Binary files, network data |

## Examples

### Basic Usage

```grapa
// Simple pattern matching
"Hello world".grep("world")
// Result: ["Hello world"]

// Case-insensitive matching
"Hello WORLD".grep("world", "i")
// Result: ["Hello WORLD"]

// Match-only output
"Hello world".grep("\\w+", "o")
// Result: ["Hello", "world"]

// Raw string literals for better readability
"Hello world".grep(r"\w+", "o")
// Result: ["Hello", "world"] - No need to escape backslashes

// Complex patterns with raw strings
"file.txt".grep(r"^[a-zA-Z0-9_]+\.txt$", "x")
// Result: ["file.txt"] - Much more readable than "\\^[a-zA-Z0-9_]\\+\\.txt\\$"
```

### Unicode Examples

```grapa
// Unicode characters
"Привет мир".grep("мир")
// Result: ["Привет мир"]

// Unicode properties
"Hello 世界 123 €".grep("\\p{L}+", "o")
// Result: ["Hello", "世界"]

// Emoji handling
"Hello 👋 world 🌍".grep("\\p{Emoji}", "o")
// Result: ["👋", "🌍"]

// Unicode normalization
"café".grep("cafe", "N")
// Result: ["café"]
```

### Raw String Literals

For better readability of regex patterns, you can use raw string literals by prefixing the string with `r`. This prevents escape sequence processing, making patterns much more readable:

```grapa
// Without raw string (requires escaping)
"file.txt".grep("^[a-zA-Z0-9_]+\\.txt$", "x")
// Result: ["file.txt"]

// With raw string (no escaping needed)
"file.txt".grep(r"^[a-zA-Z0-9_]+\.txt$", "x")
// Result: ["file.txt"] - Much cleaner!

// Complex patterns benefit greatly
"user@domain.com".grep(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$", "x")
// Result: ["user@domain.com"]

// Named groups with raw strings
"John Doe (30)".grep(r"(?P<first>\w+) (?P<last>\w+) \((?P<age>\d+)\)", "oj")
// Result: [{"match":"John Doe (30)","first":"John","last":"Doe","age":"30","offset":0,"line":1}]
```

**Note**: While raw strings suppress most escape sequences, hex escapes (`\x`) and Unicode escapes (`\u`) are still processed. If you need literal backslashes before these sequences, you may need to use regular string literals with proper escaping.

### Named Groups

```grapa
// Basic named groups
"John Doe".grep("(?P<first>\\w+) (?P<last>\\w+)", "oj")
// Result: [{"match":"John Doe","first":"John","last":"Doe","offset":0,"line":1}]

// Email extraction
"Contact: john@example.com".grep("(?P<email>[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,})", "oj")
// Result: [{"match":"john@example.com","email":"john@example.com","offset":9,"line":1}]

// Phone number parsing
"Call +1-555-123-4567".grep("(?P<country>\\+\\d{1,3})-(?P<area>\\d{3})-(?P<prefix>\\d{3})-(?P<line>\\d{4})", "oj")
// Result: [{"match":"+1-555-123-4567","country":"+1","area":"555","prefix":"123","line":"4567","offset":5,"line":1}]

// Direct access to named groups
result = "John Doe".grep("(?P<first>\\w+) (?P<last>\\w+)", "oj")
first = result[0]["first"]  // "John"
last = result[0]["last"]    // "Doe"
```

### Context Lines

```grapa
// Show 2 lines after match
"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 2", "A2")
// Result: ["Line 2", "Line 3", "Line 4"]

// Show 1 line before and 1 after
"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 3", "C1")
// Result: ["Line 2", "Line 3", "Line 4"]
```

### Advanced Regex Features

```grapa
// Lookaround assertions
"password123".grep("(?<=\\w{8})\\d+", "o")
// Result: ["123"]

// Unicode grapheme clusters
"café".grep("\\X", "o")
// Result: ["c", "a", "f", "é"]

// Unicode script extensions
"Hello 世界".grep("\\p{scx:Han}", "o")
// Result: ["世界"]

// Atomic groups
"aaaa".grep("(?>a+)a", "o")
// Result: []

// Possessive quantifiers
"aaaa".grep("a++a", "o")
// Result: []
```

### JSON Output Examples

```grapa
// Basic JSON output
"Hello world".grep("\\w+", "oj")
// Result: [{"match":"Hello","offset":0,"line":1},{"match":"world","offset":6,"line":1}]

// JSON with named groups
"John Doe (30)".grep("(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")
// Result: [{"match":"John Doe (30)","first":"John","last":"Doe","age":"30","offset":0,"line":1}]

// Complex JSON example
"Email: user@domain.com, Phone: +1-555-1234".grep("(?P<email>[\\w.-]+@[\\w.-]+)|(?P<phone>\\+\\d{1,3}-\\d{3}-\\d{4})", "oj")
// Result: [
//   {"match":"user@domain.com","email":"user@domain.com","offset":7,"line":1},
//   {"match":"+1-555-1234","phone":"+1-555-1234","offset":31,"line":1}
// ]

// Accessing named groups directly
result = "John Doe (30)".grep("(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")
first_name = result[0]["first"]  // "John"
last_name = result[0]["last"]    // "Doe"
age = result[0]["age"]           // "30"
```

### Additional Parameters Examples

```grapa
// Unicode normalization examples
"café".grep("cafe", "o", "", "NFC")
// Result: ["café"] - NFC normalization matches decomposed form

"café".grep("cafe", "o", "", "NFD")
// Result: ["café"] - NFD normalization matches composed form

// Binary mode for raw byte processing
"\\x48\\x65\\x6c\\x6c\\x6f".grep("Hello", "o", "", "NONE", "BINARY")
// Result: ["Hello"] - Binary mode processes raw bytes

// Custom delimiter with normalization
"apple|||pear|||banana".grep("\\w+", "o", "|||", "NFC")
// Result: ["apple", "pear", "banana"] - Custom delimiter with NFC normalization

// More custom delimiter examples
"section1###section2###section3".grep("section\\d+", "o", "###")
// Result: ["section1", "section2", "section3"] - Using "###" as delimiter

"item1|item2|item3".grep("item\\d+", "o", "|")
// Result: ["item1", "item2", "item3"] - Using "|" as delimiter

"record1---record2---record3".grep("record\\d+", "o", "---")
// Result: ["record1", "record2", "record3"] - Using "---" as delimiter

// Binary mode with custom delimiter
"data1\\x00data2\\x00data3".grep("data\\d+", "o", "\\x00", "NONE", "BINARY")
// Result: ["data1", "data2", "data3"] - Binary mode with null delimiter
```

## Performance Features

### Caching
- Pattern compilation caching
- Text normalization caching
- Offset mapping caching
- Thread-safe cache management

### Optimization
- ASCII-only pattern detection
- Fast path for simple patterns
- Unicode property optimization
- Memory-efficient processing

## Error Handling

### Invalid Patterns
```grapa
// Invalid regex returns error
"text".grep("([", "j")
// Result: $ERR (Invalid regex pattern)
```

### Unicode Handling
- Graceful handling of malformed UTF-8
- Fallback to original text on normalization errors
- Robust Unicode property processing

## Best Practices

### Performance
1. Use specific Unicode properties instead of broad categories
2. Enable normalization only when needed (`N` option)
3. Use `a` option for single-string searches
4. Combine options efficiently (e.g., `"oj"` for JSON output)

### Unicode
1. Always use Unicode normalization for international text
2. Use Unicode properties for script-specific matching
3. Consider grapheme clusters for emoji and combining characters
4. Test with various Unicode scripts

### Regex Patterns
1. Use named groups for structured data extraction
2. Leverage lookaround assertions for complex matching
3. Use atomic groups for performance-critical patterns
4. Test patterns with diverse Unicode content

## Limitations

### Not Supported
- Unicode blocks (`\p{In_Basic_Latin}`)
- Unicode age properties (`\p{Age=6.0}`)
- Unicode bidirectional classes (`\p{Bidi_Class:Left_To_Right}`)

### Workarounds
- Unicode blocks can be implemented with character ranges
- Age properties can be handled with custom character classes
- Bidirectional classes can be approximated with script properties

## Integration

### Grapa Script Integration
```grapa
// File processing
file_content = file.read("data.txt")
matches = file_content.grep("pattern", "oj")

// Stream processing
stream.grep("pattern", "A1B1") | foreach(line => process(line))

// Data extraction
data = "Name: John, Age: 30".grep("(?P<key>\\w+): (?P<value>[^,]+)", "oj")
```

### Error Handling in Scripts
```grapa
result = text.grep("pattern", "j")
if (result.type() == $ERR) {
    echo("Regex compilation failed")
} else {
    // Process matches
    result | foreach(match => echo(match))
}
```

## Version History

- **v0.0.39**: Full Unicode support with PCRE2, named groups, JSON output
- Enhanced error handling for invalid patterns
- Comprehensive Unicode property support
- Performance optimizations and caching
- Production-ready with extensive testing

## Related Documentation

- [Unicode Properties](type/UNICODE.md)
- [Regular Expressions](syntax/regex.md)
- [String Operations](operators/string.md)
- [JSON Processing](type/JSON.md) 