# Binary Grep Implementation

## Overview

The binary grep implementation provides functionality to search through raw binary data without Unicode processing. This is essential for:

- Searching through binary files (executables, images, etc.)
- Network packet analysis
- Memory dumps
- Any data that should be treated as raw bytes rather than text

## Implementation Approaches

There are two ways to achieve binary grep functionality:

### 1. Dedicated Binary Grep Functions (Recommended for Binary-Specific Use Cases)

The dedicated binary grep functions provide specialized functionality for binary data:

```cpp
// Basic binary grep - treats input as raw bytes
std::vector<MatchPosition> grep_binary(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = ""
);

// Binary grep with hex pattern support
std::vector<MatchPosition> grep_binary_hex(
    const std::string& input,
    const std::string& hex_pattern,
    const std::string& options = "",
    const std::string& line_delim = ""
);
```

### 2. Unicode Grep with Binary Mode (Recommended for Unified API)

The Unicode grep functions now support a `ProcessingMode` parameter that allows you to choose between Unicode and binary processing:

```cpp
// Unicode grep with binary mode option
std::vector<MatchPosition> grep_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NFC,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
);

// Extract matches with binary mode option
std::vector<std::string> grep_extract_matches_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NFC,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE
);
```

## Processing Modes

```cpp
enum class ProcessingMode {
    UNICODE_MODE,    // Full Unicode processing (normalization, case folding)
    BINARY_MODE      // Raw byte processing (no Unicode processing)
};
```

## Key Differences from Unicode Grep

### Unicode Mode (`ProcessingMode::UNICODE_MODE`)
- **Purpose**: Text processing with Unicode support
- **Processing**: Normalizes Unicode text, performs case folding
- **Patterns**: Unicode-aware regex patterns
- **Use cases**: Text files, user input, international content
- **Performance**: Slower due to Unicode processing overhead

### Binary Mode (`ProcessingMode::BINARY_MODE`)
- **Purpose**: Raw byte processing
- **Processing**: No Unicode normalization or case folding
- **Patterns**: Standard regex patterns
- **Use cases**: Binary files, network data, memory analysis
- **Performance**: Faster for binary data, no Unicode overhead

## Functions

### Core Binary Grep Functions

```cpp
// Basic binary grep - treats input as raw bytes
std::vector<MatchPosition> grep_binary(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = ""
);

// Binary grep with hex pattern support
std::vector<MatchPosition> grep_binary_hex(
    const std::string& input,
    const std::string& hex_pattern,
    const std::string& options = "",
    const std::string& line_delim = ""
);

// Extract matched content
std::vector<std::string> grep_extract_matches_binary(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = ""
);

// Extract matched content from hex patterns
std::vector<std::string> grep_extract_matches_binary_hex(
    const std::string& input,
    const std::string& hex_pattern,
    const std::string& options = "",
    const std::string& line_delim = ""
);
```

### Unicode Grep with Binary Mode

```cpp
// Unicode grep with binary mode option
std::vector<MatchPosition> grep_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NFC,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE
);

// Extract matches with binary mode option
std::vector<std::string> grep_extract_matches_unicode(
    const std::string& input,
    const std::string& pattern,
    const std::string& options = "",
    const std::string& line_delim = "",
    GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NFC,
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE
);
```

### Utility Functions

```cpp
namespace BinaryUtils {
    // Convert hex string to binary data
    std::string hex_to_binary(const std::string& hex_string);
    
    // Convert binary data to hex string
    std::string binary_to_hex(const std::string& binary_data);
    
    // Check if string contains only ASCII printable characters
    bool is_printable_ascii(const std::string& data);
    
    // Escape non-printable characters for display
    std::string escape_binary(const std::string& data);
}
```

## Pattern Types

### 1. Standard Regex Patterns
```cpp
// Find null bytes
auto matches = grep_binary(data, "\\x00", "", "");

// Find specific byte sequences
auto matches = grep_binary(data, "\\xFF\\xFE", "", "");

// Find text within binary data
auto matches = grep_binary(data, "Hello", "", "");

// Using Unicode grep with binary mode
auto matches = grep_unicode(data, "Hello", "", "", NormalizationForm::NFC, ProcessingMode::BINARY_MODE);
```

### 2. Hex Patterns
```cpp
// Find "Hello" using hex representation
auto matches = grep_binary_hex(data, "48 65 6C 6C 6F", "", "");

// Find PNG header
auto matches = grep_binary_hex(data, "89 50 4E 47", "", "");

// Find null bytes
auto matches = grep_binary_hex(data, "00", "", "");
```

## Options

The same options as regular grep are supported:

- `i`: Case insensitive (for ASCII text)
- `v`: Invert match
- `a`: All mode (treat entire input as single line)
- `o`: Match only (return only matched portions)
- `g`: Global match (find all occurrences)
- `b`: Byte mode (same as `o`)
- `c`: Count only
- `x`: Exact match

## Examples

### Example 1: Using Unicode Grep with Binary Mode
```cpp
std::string binary_data = "Hello\x00World\xFF\xFE\xFD";

// Binary mode - no Unicode processing
auto binary_matches = grep_unicode(binary_data, "Hello", "", "", 
                                  NormalizationForm::NFC, ProcessingMode::BINARY_MODE);

// Unicode mode - with Unicode processing
auto unicode_matches = grep_unicode(binary_data, "Hello", "", "", 
                                   NormalizationForm::NFC, ProcessingMode::UNICODE_MODE);
```

### Example 2: File Header Detection
```cpp
std::string file_data = read_binary_file("image.png");

// Using dedicated binary grep
auto png_matches = grep_binary_hex(file_data, "89 50 4E 47", "", "");

// Using Unicode grep with binary mode
auto png_matches2 = grep_unicode(file_data, "PNG", "", "", 
                                NormalizationForm::NFC, ProcessingMode::BINARY_MODE);
```

### Example 3: Finding Null-Terminated Strings
```cpp
std::string binary_data = "Hello\x00World\x00Test";

// Using dedicated binary grep
auto null_matches = grep_binary(binary_data, "\\x00", "", "");

// Using Unicode grep with binary mode
auto null_matches2 = grep_unicode(binary_data, "\\x00", "", "", 
                                 NormalizationForm::NFC, ProcessingMode::BINARY_MODE);
```

### Example 4: Performance Comparison
```cpp
std::string large_data = generate_large_binary_data();

// Unicode mode (slower for binary data)
auto start = std::chrono::high_resolution_clock::now();
auto unicode_matches = grep_unicode(large_data, "pattern", "", "", 
                                   NormalizationForm::NFC, ProcessingMode::UNICODE_MODE);
auto unicode_time = std::chrono::high_resolution_clock::now() - start;

// Binary mode (faster for binary data)
start = std::chrono::high_resolution_clock::now();
auto binary_matches = grep_unicode(large_data, "pattern", "", "", 
                                  NormalizationForm::NFC, ProcessingMode::BINARY_MODE);
auto binary_time = std::chrono::high_resolution_clock::now() - start;
```

## Performance Characteristics

### Binary Mode Advantages
- **No Unicode processing overhead**
- **Direct byte comparison**
- **Faster for binary data**
- **Lower memory usage**

### Unicode Mode Advantages
- **Proper Unicode normalization**
- **Case folding for international text**
- **Unicode-aware word boundaries**
- **Better for text processing**

## When to Use Each Approach

### Use Dedicated Binary Grep Functions When:
- You need hex pattern support
- Working with specialized binary analysis tools
- You want explicit binary-only APIs
- Performance is critical for binary data

### Use Unicode Grep with Binary Mode When:
- You want a unified API for both text and binary processing
- You're building applications that handle both data types
- You want to minimize API surface area
- You need to switch between modes dynamically

### Use Unicode Mode When:
- Processing text files
- Working with international content
- Need Unicode normalization
- Searching for Unicode-aware patterns
- Working with user input
- Need proper case folding

## Integration

### Unified API Approach
```cpp
// For text data
auto text_matches = grep_unicode(text_data, pattern, options, line_delim, 
                                NormalizationForm::NFC, ProcessingMode::UNICODE_MODE);

// For binary data
auto binary_matches = grep_unicode(binary_data, pattern, options, line_delim, 
                                  NormalizationForm::NFC, ProcessingMode::BINARY_MODE);

// For hex patterns (use dedicated function)
auto hex_matches = grep_binary_hex(binary_data, hex_pattern, options, line_delim);
```

### Dedicated API Approach
```cpp
// For text data
auto text_matches = grep_unicode(text_data, pattern, options, line_delim);

// For binary data
auto binary_matches = grep_binary(binary_data, pattern, options, line_delim);

// For hex patterns
auto hex_matches = grep_binary_hex(binary_data, hex_pattern, options, line_delim);
```

## Building and Testing

### Build the Binary Grep Test
```bash
# Using Visual Studio
build_binary_grep_test.bat

# Or manually
cl /EHsc /std:c++17 test_binary_grep.cpp source\grep\grapa_grep_binary.cpp
```

### Build the Unicode Binary Mode Test
```bash
# Using Visual Studio
build_unicode_binary_mode.bat

# Or manually
cl /EHsc /std:c++17 test_unicode_binary_mode.cpp source\grep\grapa_grep_unicode.cpp source\utf8proc\utf8proc.c
```

### Run Comparison Tests
```bash
build_grep_comparison.bat
```

This will demonstrate the differences between Unicode and binary grep for various types of data. 