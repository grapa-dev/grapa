---
tags:
  - maintainer
  - lowlevel
  - internal
  - binary
  - grep
  - implementation
---

# Binary Grep Implementation

## Overview

The binary grep implementation provides functionality to search through raw binary data without Unicode processing. This is essential for:

- Searching through binary files (executables, images, etc.)
- Network packet analysis
- Memory dumps
- Any data that should be treated as raw bytes rather than text

## Current Implementation Status (Updated December 2024)

### ✅ Working Features
- **Basic binary grep functionality**: Raw byte pattern matching
- **Hex pattern support**: Direct hex string matching
- **Unicode grep with binary mode**: Unified API with `ProcessingMode::BINARY_MODE`
- **Error handling**: Exception-based error handling with proper error propagation
- **Custom delimiter support**: Multi-character delimiters (e.g., "|||", "###")
- **Match-only mode**: Returns matched portions for binary data
- **All standard options**: `i`, `v`, `a`, `o`, `g`, `b`, `c`, `x`

### 🔧 Recent Improvements
- **Unified API**: Binary mode integrated into Unicode grep functions
- **Processing mode selection**: `UNICODE_MODE` vs `BINARY_MODE` for flexible processing
- **Enhanced error handling**: Proper exception handling instead of `$ERR` strings
- **Performance optimization**: Binary mode avoids Unicode processing overhead
- **Comprehensive testing**: Extensive test suite covering edge cases and combinations

### 📊 Test Results Summary
Based on comprehensive testing, the following features are working correctly:

1. **Output Formatting**: Delimiters properly removed from array results
2. **Custom Delimiters**: Multi-character delimiters work across all code paths
3. **Match-Only Mode**: Returns appropriate results based on pattern type
4. **Error Handling**: Proper handling of empty inputs and invalid patterns
5. **Edge Cases**: Empty input, delimiter-only input, trailing delimiters

### 🚧 Areas for Future Enhancement
- **Advanced binary patterns**: More sophisticated binary pattern matching
- **Performance optimization**: Parallel processing for large binary files
- **Memory efficiency**: Streaming processing for very large files
- **Pattern validation**: Enhanced validation for binary-specific patterns

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
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE,
    int num_workers = 0
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
    GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE,
    int num_workers = 0
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

## Error Handling

### Exception-Based Error Handling
All grep functions now use exception-based error handling instead of returning `"$ERR"` strings:

- **Invalid regex patterns**: Throws `std::runtime_error("Invalid regex pattern")`
- **Empty patterns**: Throws `std::runtime_error("Empty pattern is not allowed")`
- **PCRE2 compilation failures**: Throws `std::runtime_error("Invalid regex pattern")`

### Error Handling in Grapa Scripting Layer
Errors are caught by `GrapaLibraryRuleGrepEvent::Run` and returned as `$ERR` objects with descriptive messages:

```grapa
/* Example error handling */
result = input.grep("invalid[", "o");
if (result.type() == $ERR) {
    ("Error occurred: " + result.str() + "\n").echo();
}
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

// Detect PNG header using hex pattern
auto png_matches = grep_binary_hex(file_data, "89 50 4E 47", "", "");

// Detect PNG header using byte pattern
auto png_matches2 = grep_binary(file_data, "\\x89PNG", "", "");

// Using Unicode grep with binary mode
auto png_matches3 = grep_unicode(file_data, "\\x89PNG", "", "", 
                                NormalizationForm::NFC, ProcessingMode::BINARY_MODE);
```

### Example 3: Network Packet Analysis
```cpp
std::string packet_data = capture_network_packet();

// Find HTTP headers
auto http_matches = grep_binary(packet_data, "HTTP/[0-9]\\.[0-9]", "", "");

// Find specific byte sequences
auto null_sequences = grep_binary(packet_data, "\\x00{4,}", "", "");

// Using Unicode grep with binary mode
auto http_matches2 = grep_unicode(packet_data, "HTTP/[0-9]\\.[0-9]", "", "", 
                                 NormalizationForm::NFC, ProcessingMode::BINARY_MODE);
```

## Performance Considerations

### Binary Mode Performance
- **Faster processing**: No Unicode normalization or case folding overhead
- **Memory efficient**: Direct byte processing without Unicode conversion
- **Suitable for large files**: Streaming processing capabilities

### Unicode Mode Performance
- **Slower processing**: Unicode normalization and case folding overhead
- **Memory overhead**: Unicode conversion and normalization buffers
- **Suitable for text files**: Full Unicode support for international content

## Best Practices

### When to Use Binary Mode
- Searching through binary files (executables, images, archives)
- Network packet analysis and protocol parsing
- Memory dump analysis and forensics
- Any data that should be treated as raw bytes

### When to Use Unicode Mode
- Text file processing with international content
- User input validation and processing
- Web content and document processing
- Any data that requires Unicode normalization or case folding

### Pattern Design
- Use hex patterns for specific byte sequences
- Use standard regex for text patterns within binary data
- Consider performance implications for large files
- Test patterns thoroughly with representative data

## Testing and Validation

### Comprehensive Test Suite
The implementation includes a comprehensive test suite covering:

- **Basic functionality**: Pattern matching and result extraction
- **Edge cases**: Empty input, delimiter-only input, trailing delimiters
- **Error handling**: Invalid patterns, compilation failures
- **Performance**: Large file processing, memory usage
- **Integration**: Grapa scripting layer integration

### Test Results
Based on the comprehensive test suite, the following features are working correctly:

- ✅ Output formatting and delimiter removal
- ✅ Custom delimiter support (multi-character delimiters)
- ✅ Match-only mode for various pattern types
- ✅ Error handling for edge cases
- ✅ Binary mode vs Unicode mode differentiation
- ✅ Hex pattern support
- ✅ All standard grep options

## Future Enhancements

### Planned Improvements
- **Advanced binary patterns**: More sophisticated binary pattern matching
- **Performance optimization**: Parallel processing for large files
- **Memory efficiency**: Streaming processing for very large files
- **Pattern validation**: Enhanced validation for binary-specific patterns
- **Integration**: Better integration with other Grapa tools

### Research Areas
- **Pattern optimization**: Efficient pattern matching algorithms
- **Memory management**: Optimal memory usage for large files
- **Parallel processing**: Multi-threaded pattern matching
- **Streaming**: Real-time processing of streaming data 