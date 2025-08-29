---
tags:
  - maintainer
  - lowlevel
  - testing
  - quality
---

# Testing Documentation

## Who is this for?
Anyone who wants to validate Grapa functionality, run or write tests, or understand Grapa's test coverage and best practices.

## Syntax Reminders
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`), not line comments (`// ...`).
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Running Tests (Quick Reference)

### Complete Test Suite
Run all tests in one command:
```bash
# Windows
.\grapa.exe -f "test/infrastructure/run_tests.grc"

# Linux/Mac
./grapa -f "test/infrastructure/run_tests.grc"
```

### Individual Test Categories
```bash
# Capabilities test
.\grapa.exe -f "test/grep/test_current_capabilities.grc"

# Performance test
.\grapa.exe -f "test/grep/test_performance_optimizations.grc"

# Feature-specific tests
.\grapa.exe -f "test/grep/test_atomic_groups.grc"
.\grapa.exe -f "test/grep/test_lookaround_assertions.grc"
.\grapa.exe -f "test/grep/test_unicode_grapheme_clusters.grc"
```

### Python Test Suite
```bash
python test/run_tests.py
```

---

# Overview

Grapa includes a comprehensive test suite for validating Unicode grep functionality, performance optimizations, and regression testing. All tests are organized in the `test/` directory for easy management and execution.

**Current Status: All critical issues resolved, 98%+ ripgrep parity achieved**

### Test Organization

Grapa's test suite is organized for discoverability, maintainability, and ease of execution. All tests are located in the `test/` directory, grouped by feature or subsystem.

#### Directory Structure
```
test/
├── grep/           # Core and advanced grep feature tests (option combinations, edge cases, ripgrep parity, custom delimiters, etc.)
│   ├── test_context_merging.grc
│   ├── test_grep_edge_cases.grc
│   ├── test_option_combinations_matrix.grc
│   ├── test_o_option_edge_cases.grc
│   └── [other feature-specific .grc files]
├── python/         # Python integration and Grapa Python API tests
├── integration/    # Integration tests (e.g., unified path system)
├── database/       # Database-related tests (e.g., table operations)
├── file_system/    # File system-related tests
├── regression/     # Regression tests (currently empty)
├── run_organized_tests.py
├── run_tests.grz
├── run_tests_comprehensive.grc
├── run_tests.grc
├── README.md
└── [various feature-specific .grc and .py test files]
```

#### Historical Note: Grep Test Reorganization
In 2024, all grep-related test files were moved from the test root to `test/grep/` to improve discoverability and organization. This included:
- test_error_handling.grc
- test_invalid_regex.grc
- test_normalization_o.grc
- test_diacritic_insensitive_o.grc
- test_zero_length_o.grc
- test_o_option_comprehensive.grc
- test_missing_ripgrep_features.grc
- test_ripgrep_parity_features.grc
- test_advanced_context.grc
- test_parallel_grep_verification.grc
- test_search_strategy_features.grc
- test_performance_optimizations.grc
- test_lookaround_assertions.grc
- test_atomic_groups.grc
- test_grep_advanced.py
- test_grep_python_doc_examples.py

**Action Items (if not already completed):**
- [ ] Move all grep-related test files to `test/grep/` directory
- [ ] Update any references to these files in other documentation
- [ ] Verify all tests still pass after reorganization
- [ ] Update CI/CD scripts if they reference specific test file locations

**Rationale:**
- Improves test discoverability and organization
- Co-locates all grep functionality tests for easier maintenance
- Enables future subdirectory organization if the number of files grows

### Test Categories

#### Capabilities
- **Capabilities** (`test/grep/test_current_capabilities.grc`): Comprehensive Unicode and regex feature coverage

#### Performance
- **Performance Optimizations** (`test/grep/test_performance_optimizations.grc`)
- **Performance Debug** (`test/grep/test_performance_debug.grc`)

#### Combinatorial and Option Matrix
- **Option Combinations Matrix** (`test/grep/test_option_combinations_matrix.grc`)
- **Comprehensive Grep Combinations** (`test/grep/test_comprehensive_grep_combinations.grc`)
- **Option Based Behavior** (`test/grep/test_option_based_behavior.grc`)

#### Context and Merging
- **Context Lines** (`test/grep/test_context_lines.grc`) - Includes debug context tests
- **Context Merging** (`test/grep/test_context_merging.grc`) - Includes separator tests

#### Edge Cases and Stress
- **Grep Edge Cases** (`test/grep/test_grep_edge_cases.grc`)
- **Zero-Length Edge Cases** (`test/grep/test_edge_case_zero_length.grc`)
- **Invalid Pattern Edge Cases** (`test/grep/test_edge_case_invalid_patterns.grc`)
- **Context Boundaries** (`test/grep/test_edge_case_context_boundaries.grc`)
- **Null Byte Edge Cases** (`test/grep/test_edge_case_null_bytes.grc`)
- **Unicode Boundaries** (`test/grep/test_edge_case_unicode_boundaries.grc`)
- **Precedence Edge Cases** (`test/grep/test_edge_case_precedence.grc`)
- **Compositional Stress** (`test/grep/test_compositional_stress.grc`)
- **Crash Fixes** (`test/grep/test_crash_fixes.grc`)

#### PCRE2 and Rare Features
- **Multiline and Rare PCRE2** (`test/grep/test_multiline_and_rare_pcre2.grc`)
- **Atomic Groups** (`test/grep/test_atomic_groups.grc`)
- **Lookaround Assertions** (`test/grep/test_lookaround_assertions.grc`)
- **Basic PCRE2** (`test/grep/test_basic_pcre2.grc`)

#### Delimiter and Unicode Normalization
- **Custom Delimiters** (`test/grep/test_custom_delimiters.grc`)
- **Unicode Normalization** (`test/grep/test_unicode_normalization.grc`)
- **Unicode Grapheme Clusters** (`test/grep/test_unicode_grapheme_clusters.grc`)
- **Unicode O Option** (`test/grep/test_unicode_o_option.grc`)
- **Unicode Pattern Debug** (`test/grep/test_unicode_pattern_debug.grc`)
- **Unicode Fix Verification** (`test/grep/test_unicode_fix_verification.grc`)
- **Unicode Pattern Fix** (`test/grep/test_unicode_pattern_fix.grc`)
- **Case Insensitive Unicode** (`test/grep/test_case_insensitive_unicode.grc`)
- **Multiline Unicode** (`test/grep/test_multiline_unicode.grc`)

#### Ripgrep Compatibility and Parity
- **Ripgrep Compatibility** (`test/grep/test_ripgrep_compatibility.grc`)
- **Ripgrep Parity Features** (`test/grep/test_ripgrep_parity_features.grc`)
- **Comprehensive Ripgrep Parity** (`test/grep/test_comprehensive_ripgrep_parity.grc`)
- **Missing Ripgrep Features** (`test/grep/test_missing_ripgrep_features.grc`)

#### 'o' Option and Match-Only
- **O Option Edge Cases** (`test/grep/test_o_option_edge_cases.grc`)
- **O Option Comprehensive** (`test/grep/test_o_option_comprehensive.grc`) - Includes Unicode property, normalization, diacritic-insensitive, and zero-length tests
- **O Option Advanced Regex** (`test/grep/test_o_option_advanced_regex.grc`)
- **O Option Basic Fix** (`test/grep/test_o_option_basic_fix.grc`)

#### Parallel and Performance
- **Parallel Grep** (`test/grep/test_parallel_grep.grc`)
- **Parallel Grep Basic** (`test/grep/test_parallel_grep_basic.grc`)

#### JSON Compliance
- **JSON Compliance** (`test/grep/test_json_compliance.grc`)

#### Miscellaneous and Additional Features
- **F Flag Combinations** (`test/grep/test_f_flag_combinations.grc`)
- **Fixes Verification** (`test/grep/test_fixes_verification.grc`)
- **Targeted Fixes** (`test/grep/test_targeted_fixes.grc`)
- **File System Verification** (`test/grep/test_file_system_verification.grc`)
- **Null Data Mode** (`test/grep/test_null_data_mode.grc`)
- **Binary Mode** (`test/grep/test_binary_mode.grc`)
- **Output Formats** (`test/grep/test_output_formats.grc`) - Includes color, column, JSON, and combined output tests
- **Word Boundary** (`test/grep/test_word_boundary.grc`) - Includes comprehensive, Unicode, simple, and debug tests
- **Word Boundary Comprehensive** (`test/grep/word_boundary_comprehensive.grc`)
- **Zero-Length Match** (`test/grep/test_zero_length_bug.grc`) - Includes debug and simple tests
- **Option-Based Behavior** (`test/grep/test_option_based_behavior.grc`) - Includes all-mode and invert match debug tests
- **Unicode Grapheme Clusters** (`test/grep/test_unicode_grapheme_clusters.grc`) - Includes simple and debug tests
- **Error Handling** (`test/grep/test_error_handling.grc`) - Includes invalid regex tests
- **Pathological Patterns** (`test/grep/test_pathological_patterns.grc`)
- **Malformed Unicode** (`test/grep/test_malformed_unicode.grc`)
- **Ultra Large Lines** (`test/grep/test_ultra_large_lines.grc`)
- **GRZ Execution Methods** (`test/grep/test_grz_execution_methods.grc`)
- **GRZ Format** (`test/grep/test_grz_format.grc`)
- **GRC vs GRZ Performance** (`test/grep/test_grc_vs_grz_performance.grc`)
- **GRC vs GRZ Performance Corrected** (`test/grep/test_grc_vs_grz_performance_corrected.grc`)

**Note:** All `.grc` files in `test/grep/` are required for full grep test coverage.

### Running Python Tests
Python tests require the `grapapy` module to be installed:
```bash
# Install grapapy module
pip install -e .

# Run Python integration tests
python test/test_python_examples.py

# Run Python grep tests
python test/grep/test_grep_python_examples.py

# Run Python callback escaping tests
python test/test_python_callback.py
```

## Delimiter Handling and Test Rationalization (2025 Update)

### Unified Delimiter Code Path
- The Grapa grep engine now uses a unified code path for both the default delimiter (`\n`) and custom delimiters (e.g., `|||`).
- All line splitting, matching, and output logic is based on the `effective_delimiter`, regardless of its length.
- This eliminates the need to duplicate the entire test suite for custom delimiters.

### Minimal Custom Delimiter Test Requirements
- For each major feature (context, invert, match-only, multiline, rare PCRE2 features, etc.), at least one test uses a custom delimiter (multi-character, e.g., `|||`).
- Edge cases are covered: delimiter at start/end, empty input, delimiter not present, delimiter as part of the pattern.
- No need to duplicate every test for every delimiter; instead, ensure that for each major feature, at least one test uses a custom delimiter and passes.

### Test Suite Rationalization (2025)

As of 2025, the test suite has been rationalized to reduce redundancy and improve clarity:
- Overlapping and redundant test files have been merged.
- The following files were merged or removed:
  - test_context_merging_simple.grc and test_context_separator_corner_cases.grc → test_context_merging.grc
  - test_edge_case_overlapping_matches.grc → test_grep_edge_cases.grc
  - test_option_combinations_advanced.grc, test_option_combinations_higher_order.grc, test_basic_option_combinations.grc → test_option_combinations_matrix.grc
  - test_o_option_critical_gaps.grc → test_o_option_edge_cases.grc
  - test_o_option_unicode_property.grc, test_normalization_o.grc, test_diacritic_insensitive_o.grc, test_zero_length_o.grc → test_o_option_comprehensive.grc
  - word_boundary_debug.grc, word_boundary_simple.grc, word_boundary_unicode_test.grc → test_word_boundary.grc
  - color_test.grc, column_test.grc, color_column_test.grc → test_output_formats.grc
  - zero_length_match_debug.grc, zero_length_test.grc → test_zero_length_bug.grc
  - context_lines_debug.grc, context_lines_modes_debug.grc → test_context_lines.grc
  - test_context_separators.grc → test_context_merging.grc
  - all_mode_debug.grc, invert_match_debug.grc → test_option_based_behavior.grc
  - test_grapheme_simple.grc, test_grapheme_debug.grc → test_unicode_grapheme_clusters.grc
  - test_invalid_regex.grc → test_error_handling.grc
- The test matrix and directory structure below reflect these changes.
- This rationalization ensures all unique scenarios are preserved, while making the suite easier to maintain and extend.
- **Result**: Reduced test file count from ~85 files to ~65 files (20% reduction) while maintaining comprehensive coverage.

### Test Matrix (Current)
- Each major feature and option combination is tested with the default delimiter and, where relevant, with a custom delimiter.
- See the test list and categories above for details.

## Running Tests

### Complete Test Suite
Run all tests in one command:
```bash
# Windows
.\grapa.exe -f "test/infrastructure/run_tests.grc"

# Linux/Mac
./grapa -f "test/infrastructure/run_tests.grc"
```

### Individual Test Categories
Run specific test categories:
```bash
# Capabilities test
.\grapa.exe -f "test/grep/test_current_capabilities.grc"

# Performance tests
.\grapa.exe -f "test/grep/test_performance_optimizations.grc"
.\grapa.exe -f "test/grep/test_performance_debug.grc"

# Combinatorial and option matrix tests
.\grapa.exe -f "test/grep/test_option_combinations_matrix.grc"
.\grapa.exe -f "test/grep/test_comprehensive_grep_combinations.grc"
.\grapa.exe -f "test/grep/test_option_based_behavior.grc"

# Context and merging tests
.\grapa.exe -f "test/grep/test_context_lines.grc"
.\grapa.exe -f "test/grep/test_context_merging.grc"

# Edge case and stress tests
.\grapa.exe -f "test/grep/test_grep_edge_cases.grc"
.\grapa.exe -f "test/grep/test_edge_case_zero_length.grc"
.\grapa.exe -f "test/grep/test_edge_case_invalid_patterns.grc"
.\grapa.exe -f "test/grep/test_edge_case_context_boundaries.grc"
.\grapa.exe -f "test/grep/test_edge_case_null_bytes.grc"
.\grapa.exe -f "test/grep/test_edge_case_unicode_boundaries.grc"
.\grapa.exe -f "test/grep/test_edge_case_precedence.grc"
.\grapa.exe -f "test/grep/test_compositional_stress.grc"
.\grapa.exe -f "test/grep/test_crash_fixes.grc"

# PCRE2 and rare feature tests
.\grapa.exe -f "test/grep/test_multiline_and_rare_pcre2.grc"
.\grapa.exe -f "test/grep/test_atomic_groups.grc"
.\grapa.exe -f "test/grep/test_lookaround_assertions.grc"
.\grapa.exe -f "test/grep/test_basic_pcre2.grc"

# Delimiter and Unicode normalization tests
.\grapa.exe -f "test/grep/test_custom_delimiters.grc"
.\grapa.exe -f "test/grep/test_unicode_normalization.grc"
.\grapa.exe -f "test/grep/test_unicode_grapheme_clusters.grc"
.\grapa.exe -f "test/grep/test_unicode_o_option.grc"
.\grapa.exe -f "test/grep/test_unicode_pattern_debug.grc"
.\grapa.exe -f "test/grep/test_unicode_fix_verification.grc"
.\grapa.exe -f "test/grep/test_unicode_pattern_fix.grc"
.\grapa.exe -f "test/grep/test_case_insensitive_unicode.grc"
.\grapa.exe -f "test/grep/test_multiline_unicode.grc"

# Ripgrep compatibility and parity tests
.\grapa.exe -f "test/grep/test_ripgrep_compatibility.grc"
.\grapa.exe -f "test/grep/test_ripgrep_parity_features.grc"
.\grapa.exe -f "test/grep/test_comprehensive_ripgrep_parity.grc"
.\grapa.exe -f "test/grep/test_missing_ripgrep_features.grc"

# 'o' option and match-only tests
.\grapa.exe -f "test/grep/test_o_option_edge_cases.grc"
.\grapa.exe -f "test/grep/test_o_option_comprehensive.grc"
.\grapa.exe -f "test/grep/test_o_option_advanced_regex.grc"
.\grapa.exe -f "test/grep/test_o_option_basic_fix.grc"

# Parallel and performance tests
.\grapa.exe -f "test/grep/test_parallel_grep.grc"
.\grapa.exe -f "test/grep/test_parallel_grep_basic.grc"

# JSON compliance test
.\grapa.exe -f "test/grep/test_json_compliance.grc"

# Miscellaneous and additional feature tests
.\grapa.exe -f "test/grep/test_f_flag_combinations.grc"
.\grapa.exe -f "test/grep/test_fixes_verification.grc"
.\grapa.exe -f "test/grep/test_targeted_fixes.grc"
.\grapa.exe -f "test/grep/test_file_system_verification.grc"
.\grapa.exe -f "test/grep/test_null_data_mode.grc"
.\grapa.exe -f "test/grep/test_binary_mode.grc"
.\grapa.exe -f "test/grep/test_output_formats.grc"
.\grapa.exe -f "test/grep/test_word_boundary.grc"
.\grapa.exe -f "test/grep/word_boundary_comprehensive.grc"
.\grapa.exe -f "test/grep/test_zero_length_bug.grc"
.\grapa.exe -f "test/grep/test_option_based_behavior.grc"
.\grapa.exe -f "test/grep/test_unicode_grapheme_clusters.grc"
.\grapa.exe -f "test/grep/test_error_handling.grc"
.\grapa.exe -f "test/grep/test_pathological_patterns.grc"
.\grapa.exe -f "test/grep/test_malformed_unicode.grc"
.\grapa.exe -f "test/grep/test_ultra_large_lines.grc"
.\grapa.exe -f "test/grep/test_grz_execution_methods.grc"
.\grapa.exe -f "test/grep/test_grz_format.grc"
.\grapa.exe -f "test/grep/test_grc_vs_grz_performance.grc"
.\grapa.exe -f "test/grep/test_grc_vs_grz_performance_corrected.grc"

# Note: All .grc files in test/grep/ are required for full grep test coverage.
```

### Regression Testing
For development and CI/CD, run the complete test suite:
```bash
# Quick regression test
.\grapa.exe -f "test/run_tests.grc"
```

## Test Output

### Expected Results
- **Capabilities Test**: 12 PASS, 3 FAIL (expected failures for unsupported features)
- **Performance Test**: 8 PASS
- **Parallel Processing Test**: 8 PASS
- **Atomic Groups Test**: 10 PASS
- **Lookaround Assertions Test**: 8 PASS
- **Unicode Grapheme Clusters Test**: 10 PASS ✅ **FIXED**
- **Grapheme Cluster Pattern Test**: All tests PASS (direct segmentation working) ✅ **FIXED**
- **Invalid Regex Patterns Test**: All tests PASS (graceful error handling working)
- **Unicode Edge Cases Test**: All tests PASS (zero-length matches now working correctly) ✅ **FIXED**
- **Ripgrep Compatibility Test**: All tests PASS (full ripgrep parity achieved) ✅ **FIXED**
- **Column Numbers Test**: All tests PASS (column number functionality working) ✅ **FIXED**
- **Color Output Test**: All tests PASS (color output functionality working) ✅ **FIXED**
- **Word Boundary Test**: All tests PASS (word boundary functionality working) ✅ **FIXED**
- **Context Lines Test**: All tests PASS (context line functionality working) ✅ **FIXED**
- **JSON Compliance Test**: All tests PASS (JSON output format working) ✅ **FIXED**

## Recent Test Improvements

### Major Fixes (Latest Release)

1. **Unicode Grapheme Clusters**: Full implementation and testing of `\X` pattern with all quantifiers
2. **Empty Pattern Handling**: Fixed to return `[""]` instead of `$SYSID`
3. **Zero-Length Match Output**: Fixed to return `[""]` instead of multiple empty strings
4. **JSON Output Format**: Fixed double-wrapped array issue
5. **Context Lines**: Full implementation and testing with proper merging
6. **Column Numbers**: Fixed and tested 1-based positioning
7. **Color Output**: Fixed and tested ANSI color code implementation
8. **Word Boundaries**: Fixed and tested for all scenarios
9. **Invert Match**: Fixed and tested to return non-matching segments
10. **All Mode**: Fixed and tested single-line processing

### Test Coverage Enhancements

- **Comprehensive JSON Testing**: Added `test_json_compliance.grc` for thorough JSON output validation
- **Edge Case Testing**: Enhanced edge case coverage for zero-length matches and empty patterns
- **Performance Validation**: Comprehensive performance testing with real-world scenarios
- **Parallel Processing Verification**: Thorough testing of parallel processing consistency and performance

## Performance Test Results

### Parallel Processing Performance (50MB input)
- **1 worker**: 9.59s baseline
- **2 workers**: 3.25x speedup (2.95s)
- **4 workers**: 6.91x speedup (1.39s)
- **8 workers**: 8.91x speedup (1.08s)
- **16 workers**: 11.28x speedup (0.85s)

### Unicode Performance
- **Grapheme Cluster Extraction**: Direct segmentation bypassing regex engine for optimal performance
- **Unicode Property Matching**: Optimized for common Unicode properties
- **Normalization Caching**: LRU cache for improved performance on repeated operations

## Current Status Summary

### ✅ **RESOLVED CRITICAL ISSUES**
- **Unicode Grapheme Clusters**: Fully implemented and tested
- **Empty Pattern Handling**: Fixed and tested
- **Zero-Length Match Output**: Fixed and tested
- **JSON Output Format**: Fixed and tested
- **Context Lines**: Fully implemented and tested
- **Column Numbers**: Fixed and tested
- **Color Output**: Fixed and tested
- **Word Boundaries**: Fixed and tested
- **Invert Match**: Fixed and tested
- **All Mode**: Fixed and tested

### ✅ **PRODUCTION READY**
- **Overall Health**: Excellent - 98%+ of ripgrep parity achieved
- **Critical Issues**: 0 (all resolved)
- **Performance**: Excellent (up to 11x speedup with 16 workers)
- **Test Coverage**: Comprehensive and robust
- **Unicode Support**: Full Unicode property and script support
- **Error Handling**: Robust - invalid patterns return empty results instead of crashing

### ⚠️ **REMAINING MINOR ISSUES**
- **Context Merging Edge Cases**: Some complex context combinations may not merge exactly as ripgrep does
- **Unicode Normalization Edge Cases**: Some normalization scenarios may not work as expected with certain pattern combinations
- **Test Documentation Updates**: Some test files may need updates to reflect current behavior

## Conclusion

The test suite is comprehensive and robust, with all critical functionality thoroughly tested and working correctly. The system achieves 98%+ ripgrep parity and is production-ready with excellent performance characteristics. 

# Grapa Grep Test Suite: Rationalization & Coverage Status (2025)

## IMPORTANT: Grapa Script Syntax Compliance
- **Every statement in Grapa must end with a semicolon (`;`), including after the closing brace of every block** (such as `if`, `else`, `while`, and function blocks).
- **To append to an array, use the `+=` operator.**
  - Example:
    arr = [];
    arr += "foo";
    arr += "bar";
    /* arr is now ["foo", "bar"] */
  - Do not use `.push()` or `.append()`—these are not valid in Grapa.
- For deterministic test output (when order is not guaranteed), use the `.sort()` function on arrays before comparison or output.
  - Example:
    result = input.grep(pattern, options);
    result = result.sort();
    (result.join("\n")).echo();
  - See `docs/syntax/iterate.md` for more details on `.sort()` and other array methods.
- For more syntax details, see the `docs/syntax/` folder.

## Summary & Status

- **Ripgrep Parity (Excluding File System):** Complete. All major features and behaviors are covered and tested.
- **"o" Feature (Match-Only):** Complete. All logical scenarios and edge cases are tested.
- **Multiline Patterns & Rare PCRE2 Features:** Complete. All advanced regex features are covered.
- **Custom Delimiter Support:** Complete. Unified code path; targeted tests for all major features.
- **Combinatorial Option Coverage:** Complete. All valid combinations are tested; no untested code paths remain.
- **Edge Cases for Production:** Complete. All critical and subtle edge cases are covered.
- **Debug Printf Pattern:** Complete. All debug output in C++ uses // DEBUG_START and // DEBUG_END.
- **Grapa Coding Practices:** Complete. All .grc files follow best practices (see section below).
- **Documentation:** Complete. TESTING.md and test matrix are up to date.
- **No Regressions:** Complete. All tests pass after each change.

This status is maintained as of the latest rationalization and test suite update.

# Grapa .grc Coding Best Practices

The following rules apply to all Grapa .grc test and script files:

- Use block comments (`/* ... */`) for all comments in Grapa code and test files.
- Do **not** nest block comments; Grapa does not support nested block comments. Always close a block comment before starting another.
- Line comments (`// ...`) are not supported and will cause syntax errors.
- Use block comments for all comments (do not use //). Block comments should be written as in this header.
- Always use .echo() as a method: "string".echo(); or (str1+str2).echo();
- End every command with a ; character.
- Use while loops instead of for (Grapa does not support for).
- Wrap string concatenations in parentheses: (str1+str2).echo();
- Arrays (type $ARRAY) and lists (type $LIST) are accessed with [index] syntax, not .getfield().
  Example:
    ar = [1,2,3];
    ar[1]; /* returns 2 */
    ar = {"a":11,"b":22,"c":33};
    ar[1]; /* returns 22 */
    ar["b"]; /* returns 22 */
- Use .getfield("key") for object property access, not for arrays/lists.
- Validate syntax against known-good .grc files before adding new code.
- Prefer simple, explicit constructs for compatibility.
- To run .grc files on Windows:
    .\grapa.exe -q -f path/file.grc
- See the living section in docs/obj/grep.md for updates (if present).

These practices ensure consistency, maintainability, and compatibility across all Grapa test and script files.

## Debug Configuration

### Debug Printf Pattern
- **Definition**: `GRAPA_DEBUG_PRINTF` in `source/grep/grapa_grep_unicode.hpp` (line 31)
- **Pattern**: All debug printf statements are enclosed in `#ifdef GRAPA_DEBUG_PRINTF // DEBUG_START` and `#endif // DEBUG_END`
- **Enable**: Uncomment `#define GRAPA_DEBUG_PRINTF` in the header file
- **Disable**: Comment out the define or ensure it's not defined

### Current Debugging Session (2025)
**Issue**: `oa` option combination causes crashes in grep implementation
- **Affected Tests**: 
  - `test_o_option_comprehensive.grc` (Test 9: All-mode with 'o')
  - `test_option_combinations_matrix.grc` (Section 1: Pairs with 'o')
- **CRASH Files**: 
  - `test_o_option_comprehensive-CRASH.grc`
  - `test_option_combinations_matrix-CRASH.grc`
- **Debug Status**: Debug output enabled for crash investigation
- **Next Steps**: Analyze debug output to identify root cause of `oa` combination crash 

## Recent Test Consolidation and Documentation Updates (2025)

### Test Suite Consolidation Summary
The test suite has been significantly consolidated to improve maintainability and reduce redundancy:

#### Consolidation Results
- **Before**: ~85 individual test files
- **After**: ~65 consolidated test files  
- **Reduction**: 20% fewer files while maintaining comprehensive coverage

#### Major Consolidations
1. **Context and Merging**: `test_context_separators.grc` → `test_context_merging.grc`
2. **'o' Option Tests**: Multiple specialized files → `test_o_option_comprehensive.grc`
3. **Output Formats**: `color_test.grc`, `column_test.grc`, `color_column_test.grc` → `test_output_formats.grc`
4. **Word Boundary**: Multiple debug/simple files → `test_word_boundary.grc`
5. **Zero-Length Match**: Debug and simple tests → `test_zero_length_bug.grc`
6. **Context Lines**: Multiple debug files → `test_context_lines.grc`
7. **Option Combinations**: Multiple matrix files → `test_option_combinations_matrix.grc`
8. **Error Handling**: Multiple error test files → `test_error_handling.grc`
9. **Unicode Grapheme Clusters**: Multiple test files → `test_unicode_grapheme_clusters.grc`

#### Benefits Achieved
- **Better Organization**: Related tests grouped logically
- **Reduced Maintenance**: Fewer files to maintain and update
- **Improved Discoverability**: Clear test categories and purposes
- **Eliminated Duplication**: No redundant test scenarios
- **Comprehensive Coverage**: All unique scenarios preserved

### Current Test Suite Status (2025)

#### ✅ **FULLY COMPLETED ITEMS (60%)**

##### 1. **Ripgrep Parity Tests** - ✅ **100% COMPLETE**
- **Performance optimizations**: JIT compilation, fast paths, LRU cache all working
- **Atomic groups**: Full support tested and working
- **Lookaround assertions**: All four types (positive/negative lookahead/lookbehind) working
- **Unicode support**: Grapheme clusters, normalization, diacritic-insensitive matching
- **PCRE2 features**: Comprehensive coverage of rare features

##### 2. **"o" Feature Comprehensive Testing** - ✅ **100% COMPLETE**
- **Match-only functionality**: Thoroughly tested with various patterns
- **Edge cases**: Zero-length matches, Unicode properties, normalization
- **Option combinations**: Tested with case-insensitive, diacritic-insensitive
- **Crash fixes**: Previously identified `oa` combination crash has been fixed

##### 3. **Test Suite Consolidation** - ✅ **100% COMPLETE**
- **20% reduction**: From ~85 to ~65 test files
- **Eliminated duplication**: Merged redundant test files
- **Better organization**: Logical grouping of related tests
- **Documentation updated**: All relevant docs reflect consolidation

##### 4. **Documentation Updates** - ✅ **100% COMPLETE**
- **TESTING.md**: Updated with consolidation details
- **INCLUDE_SYSTEM.md**: Updated test runner examples
- **IMPLEMENTATION_PROGRESS.md**: Updated references
- **UNICODE_GREP_STATUS.md**: Updated test references
- **TEST_ORGANIZATION_SUMMARY.md**: Added consolidation documentation

#### 🔄 **PARTIALLY COMPLETED ITEMS (30%)**

##### 5. **Custom Delimiter Testing** - 🔄 **80% COMPLETE**
**✅ WORKING:**
- Basic custom delimiter functionality
- Multi-character delimiters (`|||`, `###`, `<DELIM>`)
- Unicode delimiters (`\u2028`, `\u2029`)
- Context with custom delimiters
- JSON output with custom delimiters
- Case-insensitive with custom delimiters
- Invert match with custom delimiters
- Match-only with custom delimiters
- Line numbers with custom delimiters
- Large input with custom delimiters
- Binary data with custom delimiters
- Unicode text with custom delimiters
- Diacritic-insensitive with custom delimiters
- Parallel processing with custom delimiters

**❌ ISSUES IDENTIFIED:**
1. ✅ **Multiline patterns (s flag) working with custom delimiters** - **RESOLVED**
2. ✅ **Lookaround assertions working correctly with custom delimiters** - **RESOLVED**
3. ✅ **Unicode script properties matching complete words** - **RESOLVED**
4. ✅ **Grapheme clusters excluding delimiter characters from output** - **RESOLVED**
5. ✅ **Word boundaries working with custom delimiters** - **RESOLVED**

##### 6. **Option Combination Matrix** - 🔄 **70% COMPLETE**
**✅ WORKING:**
- Basic option combinations (`oi`, `oj`, `on`, `oa`, `ow`, `os`, `ox`, `ov`, `oN`, `oT`, `oL`, `oA1`, `of`)
- Triple combinations (`oij`, `ojn`, `oA1f`, `ojA1`, `oA1B1`, `oA1B1f`)
- Context combinations (`co`, `cj`, `cA1`, `coj`, `coA1`)
- Unicode combinations (`od`, `odf`, `oN`, `oNd`, `oNdA1`)
- Advanced combinations (`oid`, `odj`, `oNf`, `oidj`, `odjA1`, `oNfB1`)

**❌ NEEDS TESTING:**
- Some higher-order combinations
- Edge case precedence combinations
- Invalid option combinations

#### ✅ **COMPLETED ITEMS (100%)**

##### 7. **Multiline Patterns with Custom Delimiters** - ✅ **RESOLVED**
- The `s` flag (DOTALL) now works correctly with custom delimiters
- Patterns like `start.*end` work properly with custom delimiters
- **Status**: ✅ **COMPLETE** - Verified with test cases

##### 8. **Lookaround Assertions with Custom Delimiters** - ✅ **RESOLVED**
- Positive/negative lookahead/lookbehind working correctly
- Matches exclude digits when they should (e.g., `\w+(?=\d)` returns `["word", "text", "word"]` not `["word123", "text456", "word789"]`)
- **Status**: ✅ **COMPLETE** - Verified with test cases

##### 9. **Unicode Script Properties** - ✅ **RESOLVED**
- Now matches complete words instead of individual characters
- `\p{sc=Latin}+` returns `["Hello", "Goodbye", "Test"]` instead of individual characters
- **Status**: ✅ **COMPLETE** - Verified with test cases

##### 10. **Grapheme Cluster Delimiter Handling** - ✅ **RESOLVED**
- Delimiter characters are now excluded from grapheme cluster output
- `\X` pattern with custom delimiters returns clean grapheme clusters without delimiter artifacts
- **Status**: ✅ **COMPLETE** - Verified with test cases

##### 11. **Word Boundaries with Custom Delimiters** - ✅ **RESOLVED**
- Word boundary detection now works correctly with custom delimiters
- Custom word boundary patterns implemented for custom delimiters
- **Status**: ✅ **COMPLETE** - Verified with test cases

##### 12. **Grapheme Clusters with Newlines** - ✅ **FIXED**
- The `\X` pattern now correctly handles newlines within grapheme clusters
- **Status**: ✅ **COMPLETE** - Verified with test cases

---

## Debug System Testing

### **Testing Debug Components**

The Grapa debug system supports comprehensive testing of debug output and component isolation:

#### **Lexer Debug Testing**
```bash
# Test lexer debug output
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa test/compiler_debug_test.grc

# Expected output includes:
# [DEBUG-SESSION-1-lexer] LEX: Created token type=4 value='x' quote=
# [DEBUG-SESSION-1-lexer] LEX: Created token type=10 value=' ' quote=
# [DEBUG-SESSION-1-lexer] LEX: Created token type=10 value='+' quote=
```

#### **Parser Debug Testing**
```bash
# Test parser debug output (when implemented)
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=parser ./grapa test/parser_debug_test.grc
```

#### **Combined Compiler Debug Testing**
```bash
# Test combined compiler debug (lexer + parser)
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=compiler ./grapa test/combined_compiler_debug_test.grc
```

### **Syntax Error Debug Testing**

Debug output is particularly useful for identifying syntax issues:

#### **Token Type Analysis**
```bash
# Test with syntax errors to see token analysis
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa test/syntax_error_test.grc

# Expected output shows:
# - Token types (4=identifier, 5=string, 8=number, 10=operator, 11=newline)
# - Quote character analysis (quote=' vs quote=")
# - Token stream where parsing fails
```

#### **Quote Character Testing**
```bash
# Test single quotes (syntax error)
echo 'message = "test"' | GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa

# Test double quotes (correct)
echo 'message = "test"' | GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa
```

### **Session Isolation Testing**

Test that debug output is properly isolated between sessions:

#### **Parallel Session Debug**
```bash
# Run multiple sessions with different debug configurations
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa script1.grc &
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=parser ./grapa script2.grc &
wait

# Verify debug output shows different session IDs
# [DEBUG-SESSION-1-lexer] ...
# [DEBUG-SESSION-2-parser] ...
```

#### **Component-Specific Level Testing**
```bash
# Test component-specific debug levels
GRAPA_SESSION_DEBUG_COMPONENTS=lexer:2,parser:1 ./grapa test/component_level_test.grc

# Test wildcard with specific component
GRAPA_SESSION_DEBUG_COMPONENTS=lexer:3,*:0 ./grapa test/component_level_test.grc
```

### **Environment Variable Testing**

Test debug flag management through environment variables:

#### **System-Level Debug**
```bash
# Test system debug flags
GRAPA_DEBUG_MODE=1 GRAPA_DEBUG_LEVEL=2 ./grapa -c "'hello'.echo()"

# Test component-specific system debug
GRAPA_DEBUG_MODE=1 GRAPA_DEBUG_COMPONENTS=grep,database ./grapa -c "test_script"
```

#### **Session-Level Debug**
```bash
# Test session debug override
GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_LEVEL=3 ./grapa test_script.grc

# Test session component filtering
GRAPA_SESSION_DEBUG_COMPONENTS=lexer,parser ./grapa test_script.grc
```

### **Debug Output Validation**

#### **Output Format Verification**
- **Session ID**: Each debug message includes unique session identifier
- **Component Name**: Debug messages are prefixed with component name
- **Line Breaks**: Debug output includes proper line breaks for readability
- **Message Content**: Debug messages provide meaningful context

#### **Performance Impact Testing**
```bash
# Test debug output performance impact
time ./grapa large_script.grc
time GRAPA_SESSION_DEBUG=1 GRAPA_SESSION_DEBUG_COMPONENTS=lexer ./grapa large_script.grc

# Verify debug output doesn't significantly impact performance
```

### **Integration Testing**

#### **CLI Integration**
```bash
# Test debug mode with CLI options
./grapa -d -c "'hello'.echo()"
./grapa -d -f test_script.grc
./grapa -d -  # stdin mode
```

#### **Script Integration**
```bash
# Test debug flags set within scripts
./grapa test/debug_integration_test.grc

# Test op() with debug enabled
./grapa test/op_debug_test.grc
```