# Unicode Grep Documentation

**See Also:** [Grapa Grep from Python (grep_python.md)](obj/grep_python.md)

## Overview

The Unicode grep functionality in Grapa provides advanced text searching capabilities with full Unicode support, PCRE2-powered regular expressions, and comprehensive output options. It's designed to handle international text, emoji, and complex Unicode properties while maintaining high performance.

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

## Syntax

```grapa
string.grep(pattern, options, delimiter, normalization, mode, num_workers)
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
- **num_workers**: Number of worker threads for parallel processing: `0` for auto-detection, `1` for sequential, `2+` for parallel (default: `0` - auto-detection)

### Simple Usage Examples

```grapa
// Minimal usage - only required parameters
"Hello world".grep("world")
// Result: ["Hello world"]

// With options
"Hello world".grep("world", "i")
// Result: ["Hello world"]

// With parallel processing (auto-detection)
"Hello world".grep("world", "i", "", "", "", 0)
// Result: ["Hello world"] - Uses optimal number of threads

// Manual parallel processing
"Hello world".grep("world", "i", "", "", "", 4)
// Result: ["Hello world"] - Uses 4 worker threads

// All parameters (rarely needed)
"Hello world".grep("world", "i", "\n", "NONE", "UNICODE", 2)
// Result: ["Hello world"]
```

## Options Reference

### Basic Options

| Option | Description | Example |
|--------|-------------|---------|
| `a` | All-mode (match across full input string, context options ignored) | `"text".grep("pattern", "a")` |
| `i` | Case-insensitive matching | `"Text".grep("text", "i")` |
| `v` | Invert match (return lines that do NOT match the pattern) | `"text".grep("pattern", "v")` |
| `x` | Exact line match (whole line must match) | `"text".grep("^text$", "x")` |
| `N` | Normalize input and pattern to NFC | `"café".grep("cafe", "N")` |
| `d` | Diacritic-insensitive matching (strip accents/diacritics from both input and pattern, robust Unicode-aware) | `"café".grep("cafe", "d")` |

---

### Diacritic-Insensitive Matching (`d` option)

The `d` option enables **diacritic-insensitive matching**. When enabled, both the input and the pattern are:
1. **Unicode normalized** (NFC by default, or as specified)
2. **Case folded** (Unicode-aware, not just ASCII)
3. **Diacritics/accents are stripped** (works for Latin, Greek, Cyrillic, Turkish, Vietnamese, and more)

This allows matches like:
- `"café".grep("cafe", "d")` → `["café"]`
- `"CAFÉ".grep("cafe", "di")` → `["CAFÉ"]`
- `"mañana".grep("manana", "d")` → `["mañana"]`
- `"İstanbul".grep("istanbul", "di")` → `["İstanbul"]`
- `"καφές".grep("καφες", "d")` → `["καφές"]`
- `"кофе".grep("кофе", "di")` → `["кофе"]`

#### Special Capabilities
- Handles both precomposed (NFC) and decomposed (NFD) Unicode forms
- Supports diacritic-insensitive matching for Latin, Greek, Cyrillic, Turkish, Vietnamese, and more
- Works with case-insensitive (`i`) and normalization (`N`, or normalization parameter) options
- Robust for international text, including combining marks

#### Limitations
- Only covers scripts and diacritics explicitly mapped (Latin, Greek, Cyrillic, Turkish, Vietnamese, etc.)
- Does **not** transliterate between scripts (e.g., Greek to Latin)
- Does **not** remove all possible Unicode marks outside supported ranges (e.g., rare/archaic scripts)
- For full Unicode normalization, use with the normalization parameter (e.g., `"NFC"`, `"NFD"`)
- Does **not** perform locale-specific collation (e.g., German ß vs ss)

#### Example
```grapa
input = "café\nCAFÉ\ncafe\u0301\nCafe\nCAFÉ\nmañana\nmañana\nİstanbul\nistanbul\nISTANBUL\nstraße\nSTRASSE\nStraße\nкофе\nКофе\nκαφές\nΚαφές\n";
result = input.grep(r"cafe", "di");
// Result: ["café", "CAFÉ", "café", "Cafe", "CAFÉ"]
```

### Output Options

| Option | Description | Example |
|--------|-------------|---------|
| `o` | Match-only (output only matched text) | `"Hello world".grep("\\w+", "o")` |
| `n` | Prefix matches with line numbers | `"text".grep("pattern", "n")` |
| `l` | Line number only output | `"text".grep("pattern", "l")` |
| `b` | Output byte offset with matches | `"text".grep("pattern", "b")` |
| `j` | JSON output format with named groups, offsets, and line numbers | `"text".grep("pattern", "oj")` |

### Context Options

| Option | Description | Example |
|--------|-------------|---------|
| `A<n>` | Show n lines after match | `"text".grep("pattern", "A2")` |
| `B<n>` | Show n lines before match | `"text".grep("pattern", "B1")` |
| `C<n>` | Show n lines before and after | `"text".grep("pattern", "C3")` |
| `A<n>B<m>` | Show n lines after and m lines before | `"text".grep("pattern", "A2B1")` |
| `B<m>C<n>` | Show m lines before and n lines before/after | `"text".grep("pattern", "B1C2")` |

**Note**: Context options can be combined flexibly. For example, `"A2B1C3"` would show 2 lines after, 1 line before, and 3 lines before/after the match. Overlapping context lines are allowed (like ripgrep behavior) to ensure all relevant context is shown.

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

// Raw strings preserve literal escape sequences
"\\x45".grep(r"\x45", "o")
// Result: ["\\x45"] - Literal string, not character "E"
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
"Hello 👋 world 🌍".grep("(?:\\p{So}(?:\\u200D\\p{So})*)+", "o")
// Result: ["👋", "🌍"]

// Emoji sequence (family)
"Family: 👨‍👩‍👧‍👦".grep("(?:\\p{So}(?:\\u200D\\p{So})*)+", "o")
// Result: ["👨‍👩‍👧‍👦"]

// Unicode grapheme clusters
"Hello 👋 world 🌍".grep("\\X", "o")
// Result: ["H", "e", "l", "l", "o", " ", "👋", " ", "w", "o", "r", "l", "d", " ", "🌍"]

// Emoji sequences as grapheme clusters
"👨‍👩‍👧‍👦".grep("\\X", "o")
// Result: ["👨‍👩‍👧‍👦"] (entire family emoji as one grapheme cluster)

// Combining characters as grapheme clusters
"café mañana".grep("\\X", "o")
// Result: ["c", "a", "f", "é", " ", "m", "a", "ñ", "a", "n", "a"] (é and ñ as single grapheme clusters)

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

**Note**: Raw strings suppress all escape sequences except for escaping the quote character used to enclose the string. This means `\x45` becomes the literal string `"\x45"` rather than the character `"E"`. If you need hex or Unicode escapes to be processed, use regular string literals.

### JSON Output Format

The `j` option produces JSON output with detailed match information. Each match is returned as a JSON object containing:

- **`match`**: The full matched substring
- **Named groups**: Each named group from the regex pattern (e.g., `year`, `month`, `day`)
- **`offset`**: Byte offset of the match in the input string
- **`line`**: Line number where the match was found

#### JSON Object Structure

```json
{
  "match": "matched text",
  "group1": "captured value",
  "group2": "captured value",
  "offset": 0,
  "line": 1
}
```

#### Examples

```grapa
// Basic JSON output
"Hello world".grep("\\w+", "oj")
// Result: [{"match":"Hello","offset":0,"line":1},{"match":"world","offset":6,"line":1}]

// JSON with named groups
"John Doe (30)".grep("(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")
// Result: [{"match":"John Doe (30)","first":"John","last":"Doe","age":"30","offset":0,"line":1}]

// Date parsing with named groups
"2023-04-27\n2022-12-31".grep("(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})", "oj")
// Result: [
//   {"match":"2023-04-27","year":"2023","month":"04","day":"27","offset":0,"line":1},
//   {"match":"2022-12-31","year":"2022","month":"12","day":"31","offset":11,"line":2}
// ]

// Complex JSON example with multiple patterns
"Email: user@domain.com, Phone: +1-555-1234".grep("(?P<email>[\\w.-]+@[\\w.-]+)|(?P<phone>\\+\\d{1,3}-\\d{3}-\\d{4})", "oj")
// Result: [
//   {"match":"user@domain.com","email":"user@domain.com","phone":null,"offset":7,"line":1},
//   {"match":"+1-555-1234","email":null,"phone":"+1-555-1234","offset":31,"line":1}
// ]
```

#### Accessing Named Groups

```grapa
// Extract specific groups from JSON output
result = "John Doe (30)".grep("(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")
first_name = result[0]["first"]  // "John"
last_name = result[0]["last"]    // "Doe"
age = result[0]["age"]           // "30"
```

#### Notes

- **Named groups**: All named groups from the regex pattern are included in the JSON output
- **Unmatched groups**: Groups that don't match are set to `null`
- **Line numbers**: Correctly calculated based on newline characters in the input
- **Offsets**: Byte offsets from the start of the input string
- **Order**: JSON object key order may vary but all named groups are always present

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
// Unicode categories
"Hello 世界 123 €".grep("\\p{L}+", "o")
// Result: ["Hello", "世界"]

// Unicode scripts
"Hello 世界".grep("\\p{sc=Latin}", "o")
// Result: ["Hello"]

// Unicode script extensions
"Hello 世界".grep("\\p{scx:Han}", "o")
// Result: ["世界"]

// Unicode general categories
"Hello World".grep("\\p{Lu}", "o")
// Result: ["H", "W"]

// Atomic groups
"aaaa".grep("(?>a+)a", "o")
// Result: [] (atomic group prevents backtracking)

// Lookaround assertions
// Positive lookahead - word followed by number
"word123 text456".grep("\\w+(?=\\d)", "o")
// Result: ["word", "text"]

// Negative lookahead - word not followed by number
"word123 text456".grep("\\w+(?!\\d)", "o")
// Result: ["word123", "text456"]

// Positive lookbehind - number preceded by word
"word123 text456".grep("(?<=\\w)\\d+", "o")
// Result: ["123", "456"]

// Negative lookbehind - number not preceded by word
"123 word456".grep("(?<!\\w)\\d+", "o")
// Result: ["123"]

// Complex password validation
"password123".grep("(?=.*[A-Z])(?=.*[a-z])(?=.*\\d).{8,}", "o")
// Result: [] (no uppercase letter)

"Password123".grep("(?=.*[A-Z])(?=.*[a-z])(?=.*\\d).{8,}", "o")
// Result: ["Password123"] (valid password)

// Advanced Unicode properties
"Hello 😀 World 🌍".grep("\\p{Emoji}", "o")
// Result: ["😀", "🌍"]

"Hello 👨‍👩‍👧‍👦 World".grep("\\p{So}", "o")
// Result: ["👨‍👩‍👧‍👦"]

// Advanced Unicode properties with mixed content
"Hello 世界 😀 🌍".grep("\\p{So}", "o")
// Result: ["😀", "🌍"] (symbols only, not Han characters)

// Emoji sequences as symbols
"Family: 👨‍👩‍👧‍👦".grep("\\p{So}", "o")
// Result: ["👨‍👩‍👧‍👦"] (entire family emoji as one symbol)

// Possessive quantifiers
"aaaa".grep("a++a", "o")
// Result: [] (possessive quantifier prevents backtracking)

"aaa".grep("a++", "o")
// Result: ["aaa"] (matches all a's greedily without backtracking)

// Edge cases for possessive quantifiers
"a".grep("a?+", "o")
// Result: ["a"] (possessive optional quantifier)

"abc".grep("a*+b", "o")
// Result: ["ab"] (possessive star with following character)

// Conditional patterns
"abc123".grep("(a)?(?(1)b|c)", "o")
// Result: ["ab"] (conditional pattern works)

"c123".grep("(a)?(?(1)b|c)", "o")
// Result: ["c"] (alternative branch when 'a' is not present)

// More complex conditional patterns
"xyz".grep("(x)?(?(1)y|z)", "o")
// Result: ["xy"] (first branch when 'x' is present)

"yz".grep("(x)?(?(1)y|z)", "o")
// Result: ["z"] (second branch when 'x' is not present)

// Context lines
"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 3", "A1")
// Result: ["Line 3", "Line 4"] (shows 1 line after)

"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 3", "B1")
// Result: ["Line 2", "Line 3"] (shows 1 line before)

"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 3", "C1")
// Result: ["Line 2", "Line 3", "Line 4"] (shows 1 line before and after)

// Named groups with JSON output
"John Doe (30)".grep("(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")
// Result: [{"match":"John Doe (30)","first":"John","last":"Doe","age":"30","offset":0,"line":1}]
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

### Performance Optimization Details

Grapa grep includes several performance optimizations:

1. **Pattern Compilation Caching** - Compiled patterns are cached for reuse
2. **PCRE2 JIT Compilation** - Just-In-Time compilation for fast pattern matching
3. **Fast Path Expansions** - Optimized paths for simple literal, word, and digit patterns
4. **LRU Cache Management** - Thread-safe LRU cache for text normalization
5. **Parallel Processing** - Multi-threaded processing for large inputs

### Parallel Processing

Grapa grep now supports parallel processing for large inputs:

- **Automatic worker detection** - Determines optimal number of threads based on input size
- **Smart chunking** - Splits input at line boundaries to avoid breaking matches
- **Thread-safe processing** - Uses std::async for cross-platform compatibility
- **Fallback to sequential** - Automatically uses single-threaded processing for small inputs

**Usage:**
```grapa
// Automatic parallel processing (recommended)
"large_input".grep("pattern", "o")

// Manual parallel processing with specific worker count
"large_input".grep("pattern", "o", "", "", "", "", 4)  // 4 worker threads

// Sequential processing (force single-threaded)
"large_input".grep("pattern", "o", "", "", "", "", 1)  // 1 worker thread

// Auto-detection (same as default)
"large_input".grep("pattern", "o", "", "", "", "", 0)  // Auto-detect optimal threads
```

**num_workers Parameter Values:**
- **`0`** (default): Auto-detection - determines optimal number of threads based on input size
- **`1`**: Sequential processing - forces single-threaded execution
- **`2+`**: Parallel processing - uses specified number of worker threads

**Performance characteristics:**
- **Small inputs** (< 1MB): Single-threaded processing (auto-detected)
- **Medium inputs** (1-10MB): 2-4 worker threads (auto-detected)
- **Large inputs** (> 10MB): Up to 16 worker threads (auto-detected, configurable)

**Note**: All grep features (context lines, invert match, all-mode) work correctly in parallel mode.

**Performance Examples:**
```grapa
// Large file processing with parallel workers
large_content.grep("pattern", "oj", "", "", "", "", 4)
// Result: Faster processing with 4 worker threads

// Sequential processing for small inputs
small_content.grep("pattern", "oj", "", "", "", "", 1)
// Result: Sequential processing, no threading overhead

// Auto-detection for optimal performance
any_size_content.grep("pattern", "oj", "", "", "", "", 0)
// Result: Automatically chooses best approach
```

### Binary Mode Processing

#### When to Use Binary Mode
Binary mode is useful for:
- **Binary files**: Executables, images, compressed files
- **Network data**: Raw packet analysis
- **Memory dumps**: Forensic analysis
- **Data that should not be Unicode-processed**

#### Binary vs Unicode Mode

| Aspect | Unicode Mode | Binary Mode |
|--------|-------------|-------------|
| **Processing** | Full Unicode normalization and case folding | Raw byte processing |
| **Performance** | Slower due to Unicode overhead | Faster for binary data |
| **Memory** | Higher due to normalization | Lower memory usage |
| **Use case** | Text files, user input | Binary files, network data |

```grapa
// Unicode mode (default) - for text files
"café".grep("cafe", "i")               // Case-insensitive with Unicode folding

// Binary mode - for binary data
"\\x48\\x65\\x6c\\x6c\\x6f".grep("Hello", "o", "", "NONE", "BINARY")
// Result: ["Hello"] - Raw byte processing

// Binary data with null delimiters
"data1\\x00data2\\x00data3".grep("data\\d+", "o", "\\x00", "NONE", "BINARY")
// Result: ["data1", "data2", "data3"] - Binary mode with null delimiter
```

### Advanced Usage Patterns

#### Complex Context Line Combinations
Context options can be combined flexibly for sophisticated output:

```grapa
// Show 2 lines after, 1 line before, and 3 lines before/after
"Line 1\nLine 2\nLine 3\nLine 4\nLine 5".grep("Line 3", "A2B1C3")
// Result: ["Line 2", "Line 3", "Line 4", "Line 5"] 
// (B1: Line 2, A2: Line 4-5, C3: additional context)
// Note: Overlapping context lines are allowed for complete coverage

// Show 1 line before and 2 lines after
"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 3", "B1A2")
// Result: ["Line 2", "Line 3", "Line 4"]

// Show 3 lines before and 1 line after
"Line 1\nLine 2\nLine 3\nLine 4\nLine 5".grep("Line 4", "B3A1")
// Result: ["Line 1", "Line 2", "Line 3", "Line 4", "Line 5"]
```

#### Performance Tuning for Large Datasets

**For very large files (>100MB)**:
```grapa
// Use 'a' option for single-string processing
large_content.grep("pattern", "a")     // Process as single string

// Use specific Unicode properties instead of broad categories
large_content.grep("\\p{Lu}", "o")     // Better than \\p{L} for uppercase only

// Disable normalization if not needed
large_content.grep("pattern", "o")     // No 'N' option unless required

// Use fast path patterns when possible
large_content.grep("\\w+", "o")        // Fast path for word matching
```

**Memory usage considerations**:
- **Cache size**: LRU cache limits memory usage automatically
- **Pattern compilation**: Compiled patterns are cached but use memory
- **Large files**: Consider processing in chunks for very large files

#### Thread Safety
All grep operations are thread-safe:
- **Concurrent access**: Multiple threads can call grep simultaneously
- **Cache safety**: All caches are protected with mutexes
- **No shared state**: Each grep call is independent

```grapa
// Thread-safe concurrent usage
// Thread 1
result1 = text.grep("pattern1", "oj")

// Thread 2 (simultaneous)
result2 = text.grep("pattern2", "oj")

// Both operations are safe and independent
```

### Troubleshooting

#### Common Regex Compilation Errors

**Invalid pattern syntax**:
```grapa
// Unmatched parentheses
"text".grep("(", "j")                  // Error: Unmatched '('

// Invalid quantifier
"text".grep("a{", "j")                 // Error: Invalid quantifier

// Invalid Unicode property
"text".grep("\\p{Invalid}", "j")       // Error: Unknown property
```

**Solutions**:
```grapa
// Fix unmatched parentheses
"text".grep("(group)", "j")            // Valid: matched parentheses

// Fix invalid quantifier
"text".grep("a{1,3}", "j")             // Valid: proper quantifier

// Use valid Unicode properties
"text".grep("\\p{L}", "j")             // Valid: letter property
```

#### Performance Issues

**Slow pattern matching**:
```grapa
// Problem: Catastrophic backtracking
"a".repeat(10000).grep("(a+)+", "o")   // Very slow

// Solution: Use atomic groups
"a".repeat(10000).grep("(?>a+)+", "o") // Much faster

// Problem: Broad Unicode categories
"text".grep("\\p{L}+", "o")            // Slower for large text

// Solution: Use specific properties
"text".grep("\\p{Lu}+", "o")           // Faster for uppercase only
```

**Memory usage issues**:
```grapa
// Problem: Large cache accumulation
// Solution: Process in smaller chunks or restart application

// Problem: Large compiled patterns
// Solution: Use simpler patterns or break into multiple searches
```

#### Unicode Normalization Issues

**Unexpected matches**:
```grapa
// Problem: Different normalization forms
"café".grep("cafe", "o")               // No match without normalization

// Solution: Use normalization
"café".grep("cafe", "N")               // Matches with NFC normalization

// Problem: Case sensitivity with Unicode
"İstanbul".grep("istanbul", "i")       // May not match due to Turkish 'İ'

// Solution: Use diacritic-insensitive matching
"İstanbul".grep("istanbul", "di")      // Matches with diacritic stripping
```

#### Debugging Tips

**Check pattern validity**:
```grapa
// Test pattern compilation
result = text.grep("pattern", "j")
if (result.type() == $ERR) {
    echo("Pattern compilation failed")
    // Check pattern syntax
}
```

**Verify Unicode handling**:
```grapa
// Test Unicode normalization
"café".grep("cafe", "N")               // Should match with normalization

// Test case folding
"CAFÉ".grep("cafe", "i")               // Should match case-insensitive

// Test diacritic stripping
"café".grep("cafe", "d")               // Should match diacritic-insensitive
```

**Performance profiling**:
```grapa
// Test with small sample first
sample = large_text.substring(0, 1000)
result = sample.grep("pattern", "oj")   // Test pattern on small sample

// If successful, test on full text
if (result.type() != $ERR) {
    full_result = large_text.grep("pattern", "oj")
}
```

## Testing and Verification

### Performance Testing
A comprehensive performance test file is available to verify optimizations:

```grapa
// Run performance tests
grapa -cfile "test_performance_optimizations.grc"
```

**Test Coverage**:
- JIT compilation detection and functionality
- Fast path optimizations for literal, word, and digit patterns
- LRU cache functionality for text normalization
- Complex Unicode pattern performance
- Mixed pattern performance
- Edge case handling

### Capability Testing
Verify current Unicode and regex capabilities:

```grapa
// Run comprehensive capability tests
grapa -cfile "test_current_capabilities.grc"
```

**Test Coverage**:
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

### Feature-Specific Tests
Individual test files for specific features:

```grapa
// Test Unicode normalization and diacritic handling
grapa -cfile "test_grapheme_unicode_normalization.grc"

// Test advanced Unicode features
grapa -cfile "test_unicode_advanced_features.grc"

// Test lookaround assertions
grapa -cfile "test_lookaround_assertions.grc"

// Test atomic groups
grapa -cfile "test_atomic_groups.grc"

// Test Unicode grapheme clusters
grapa -cfile "test_unicode_grapheme_clusters.grc"
```

### Regression Testing
To ensure no regressions after changes:

```grapa
// Run core functionality tests
grapa -cfile "test_current_capabilities.grc"
grapa -cfile "test_performance_optimizations.grc"

// Verify basic functionality
"Hello world".grep("world", "oj")       // Should return matches
"café".grep("cafe", "N")               // Should match with normalization
"Hello 世界".grep("\\p{L}+", "oj")      // Should match Unicode letters
```

## Comprehensive Features Summary

### ✅ **Fully Supported Features**

**Unicode Support:**
- ✅ **Basic Unicode properties** (`\p{L}`, `\p{N}`, `\p{Z}`, `\p{P}`, `\p{S}`, `\p{C}`, `\p{M}`)
- ✅ **Advanced Unicode properties** (`\p{Emoji}`, `\p{So}`, `\p{Sc}`, etc.)
- ✅ **Unicode scripts** (`\p{sc=Latin}`, `\p{sc=Han}`, `\p{sc=Cyrillic}`, etc.)
- ✅ **Unicode script extensions** (`\p{scx:Han}`, etc.)
- ✅ **Unicode general categories** (`\p{Lu}`, `\p{Ll}`, `\p{Lt}`, etc.)
- ✅ **Unicode grapheme clusters** (`\X`) - handles emoji sequences, combining characters
- ✅ **Unicode normalization** (NFC, NFD, NFKC, NFKD)
- ✅ **Case-insensitive matching** with proper Unicode case folding

**Advanced Regex Features:**
- ✅ **Named groups** (`(?P<name>...)`)
- ✅ **Atomic groups** (`(?>...)`)
- ✅ **Lookaround assertions** (`(?=...)`, `(?<=...)`, `(?!...)`, `(?<!...)`)
- ✅ **Possessive quantifiers** (`*+`, `++`, `?+`, `{n,m}+`)
- ✅ **Conditional patterns** (`?(condition)...`)

**Output and Context Features:**
- ✅ **JSON output** with named groups, offsets, and line numbers
- ✅ **Context lines** (`A<n>`, `B<n>`, `C<n>`) with flexible combinations
- ✅ **All basic grep options** (`o`, `i`, `v`, `x`, `n`, `l`, `b`, `c`, `d`, `g`)

**Performance Features:**
- ✅ **Pattern compilation caching**
- ✅ **Text normalization caching**
- ✅ **Offset mapping caching**
- ✅ **Thread-safe cache management**

### ❌ **Not Supported (3 specialized features):**
- ❌ **Unicode blocks** (`\p{In_Basic_Latin}`) - use Unicode scripts instead
- ❌ **Unicode age properties** (`\p{Age=1.1}`) - very specialized
- ❌ **Unicode bidirectional classes** (`\p{Bidi_Class:Left_To_Right}`) - very specialized

**Coverage:** Grapa supports **95%+ of practical Unicode and regex use cases** with production-ready reliability.

## Features Not Currently Supported

### **Search Strategy Features**
- ✅ **Smart-case matching** - Use "i" flag for lowercase patterns, no flag for uppercase patterns
- ✅ **Word boundary mode** - Use `\b` pattern anchors: `r"\bword\b"`
- ✅ **Column numbers** - Use "b" option for byte offsets, can calculate character position

### **File Handling Features** (handled by Grapa language or Python integration)
- ❌ **Automatic .gitignore support** - Grapa handles file filtering separately via `file().ls()` with filters
- ❌ **File type detection** - Use Grapa's file operations (`file().extension()`, `file().type()`) instead
- ❌ **File size limits** - Use Grapa's file size checking (`file().size()`) before grep operations
- ❌ **Hidden file filtering** - Use Grapa's file listing with filters (`file().ls(".*", "h")`)

**Note:** Many of these features are handled differently in Grapa's integrated environment, where file operations and filtering are managed by the Grapa language or Python integration rather than within the grep function itself. This design provides more flexibility and control over file operations.

## Summary: Actual Missing Features (Excluding File Handling)

When you exclude file handling (since that's handled by the Grapa language), Grapa grep is missing just **1 feature** that ripgrep has:

### **Performance Features (1 missing)**
- ❌ **SIMD optimizations** - Standard optimizations (ripgrep uses CPU vector instructions)

**Bottom Line:** Grapa grep has about **95%+ of ripgrep's core text processing features**, plus several unique advanced Unicode capabilities that ripgrep doesn't have. The main gaps are in **performance optimizations**.

## **Achieving "Missing" Features in Grapa**

### **Smart-case Matching**
```grapa
// ripgrep: rg "hello" (case-insensitive for lowercase)
"Hello WORLD".grep("hello", "i")

// ripgrep: rg "HELLO" (case-sensitive for uppercase)  
"Hello WORLD".grep("HELLO", "")
```

### **Word Boundary Mode**
```grapa
// ripgrep: rg --word-regexp "hello"
"hello world".grep(r"\bhello\b", "o")
```

### **Column Numbers**
```grapa
// ripgrep: rg --column "hello"
"hello world".grep("hello", "b")  // Shows byte offset
// Can calculate character position from byte offset if needed
```

## Grapa vs. ripgrep: Feature Comparison Summary

### **Grapa's Strengths (Where Grapa excels)**
- ✅ **Advanced Unicode** - Grapheme clusters, normalization, diacritic-insensitive matching
- ✅ **Language Integration** - Native part of Grapa language, not standalone
- ✅ **Advanced Regex** - Named groups, atomic groups, lookaround assertions
- ✅ **JSON Output** - Structured output with metadata
- ✅ **JIT Compilation** - Fast pattern matching
- ✅ **Unicode Properties** - Full Unicode categories, scripts, and properties

### **ripgrep's Strengths (Where ripgrep excels)**
- ✅ **Performance** - SIMD optimizations
- ✅ **File Handling** - Automatic .gitignore, file type detection, size limits, memory-mapped I/O (standalone tool)

### **Shared Strengths (Both tools excel)**
- ✅ **Regex Engine** - Full PCRE2 support with Unicode
- ✅ **Case Handling** - Case-sensitive and case-insensitive modes
- ✅ **Context Lines** - Before/after context with `-A`, `-B`, `-C`
- ✅ **Binary Mode** - Skip binary files or search within them
- ✅ **Line Numbers** - Show line numbers with `-n`
- ✅ **Invert Match** - Show non-matching lines with `-v`
- ✅ **Smart-case matching** - Use "i" flag for lowercase patterns, no flag for uppercase
- ✅ **Word boundary mode** - Use `\b` pattern anchors or ripgrep's `--word-regexp`
- ✅ **Column numbers** - Byte offsets in Grapa, character positions in ripgrep
- ✅ **Parallel processing** - Multi-threaded processing for large inputs

### **Feature Coverage Comparison**

**Grapa grep covers ~95% of ripgrep's non-file-system features:**
- ✅ All core text processing, regex, Unicode, and search strategy features
- ❌ Only missing: SIMD (vectorized) search optimizations

**ripgrep covers ~80-85% of Grapa grep's features:**
- ✅ Core regex, case handling, context lines, binary mode, line numbers, invert match
- ❌ Missing: Unicode normalization, diacritic-insensitive matching, grapheme clusters, advanced Unicode properties, script extensions, flexible JSON output, integrated language features, Python integration

### **When to Use Each Tool**

| Use Case | Recommended Tool | Reason |
|----------|-----------------|--------|
| **International Text Processing** | Grapa | Best Unicode support, normalization, diacritic-insensitive |
| **High-Performance File Search** | ripgrep | Fastest for large file systems, multi-threaded |
| **Integrated Development** | Grapa | Part of programming environment, Python integration |
| **Command-line Search** | ripgrep | Optimized for CLI usage, smart defaults |
| **Unicode Analysis** | Grapa | Grapheme clusters, normalization, advanced Unicode features |
| **Large-scale File Operations** | Grapa | Parallel processing, integrated language |
| **Cross-platform Scripts** | Grapa | Consistent behavior, integrated language |
| **File Processing Workflows** | Grapa | File operations handled by language, grep focuses on text processing |

**Bottom Line:** Grapa grep has about **95%** of ripgrep's core text processing features, plus unique advanced Unicode capabilities. ripgrep covers about **80-85%** of Grapa grep's features. For most text processing tasks, especially Unicode-heavy work, Grapa is quite capable. ripgrep remains the gold standard for high-performance file system searches. 

## Grapa's Integrated Approach vs. ripgrep's Standalone Approach

### **File Handling Philosophy**

**ripgrep (Standalone Tool):**
- File handling is built into the grep function
- Automatic .gitignore support
- File type detection and filtering
- File size limits and hidden file handling
- Optimized for command-line file system searches

**Grapa (Integrated Language):**
- File handling is separated from text processing
- File operations use Grapa language functions: `file().ls()`, `file().size()`, `file().type()`
- More flexible and programmable file filtering
- grep function focuses purely on text pattern matching
- Better for complex workflows and integrated development

### **Example: File Processing Workflow**

**ripgrep approach:**
```bash
rg "pattern" --type python --max-filesize 1M --hidden
```

**Grapa approach:**
```grapa
// File operations handled by language
files = file().ls("*.py", "h");  // Get Python files, including hidden
filtered = files.filter(f => file().size(f) < 1024*1024);  // Size filter
content = filtered.map(f => file().read(f));  // Read files
matches = content.grep("pattern", "oj");  // Pure text processing
```

This separation allows Grapa grep to focus on what it does best: advanced Unicode text processing with sophisticated regex features, while file operations are handled by the appropriate language constructs.
