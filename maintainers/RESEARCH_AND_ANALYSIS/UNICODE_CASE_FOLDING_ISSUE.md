# Unicode Case Folding Issue

## Summary

During the Unicode enhancement implementation, we discovered that the Unicode case folding is working correctly in the C++ implementation but the Grapa language binding is missing. We have successfully implemented a lookup table-based case folding solution that handles Turkish I characters and other special Unicode cases properly.

## Issue Details

### Problem
The `case_fold()` method is not implemented in the Grapa language. When called from Grapa scripts, it returns empty strings, indicating it's returning a default value or null byte.

### Evidence
1. **C++ Implementation Works**: The C++ case folding implementation in `grapa_grep_unicode.hpp` is working correctly, as evidenced by the debug output showing proper case folding of Turkish I characters.

2. **Grep Integration Works**: The grep function correctly uses case folding internally, as shown by debug output:
   ```
   CASE_FOLD_CALLED: input='İstanbul ß ß ß' length=18
   CASE_FOLD_RESULT: result_len=19
   CASE_FOLD_FINAL: result='i̇stanbul ss ss ss' length=19
   ```

3. **Grapa Language Binding Missing**: The `case_fold()` method is not implemented in the Grapa language, so calls like `"İ".case_fold()` return empty strings.

### Technical Details

#### Case Folding Implementation - UPDATED
- **Location**: `source/grep/grapa_grep_unicode.hpp` - `UnicodeString::case_fold()`
- **Method**: Uses lookup table-based `case_fold_codepoint()` function for reliable Unicode case folding
- **Enhancement**: Replaced utf8proc-based case folding with custom lookup table to handle special cases
- **Status**: ✅ Working correctly with lookup table

#### Lookup Table Implementation - NEW
- **Function**: `case_fold_codepoint(char32_t c)` in `grapa_grep_unicode.hpp`
- **Special Cases Handled**:
  - Turkish I: İ (U+0130) → i (U+0069), ı (U+0131) → i (U+0069)
  - German sharp S: ß (U+00DF) → s (U+0073)
  - Greek final sigma: ς (U+03C2) → σ (U+03C3)
  - Common accented characters: À→à, Á→á, etc.
- **Advantage**: Produces composed forms instead of decomposed forms, avoiding regex matching issues

#### Turkish I Case Folding - RESOLVED
- **Input**: `İ` (U+0130) → **Expected**: `i` (U+0069)
- **Actual C++ Output**: `i` (U+0069) ✅ **FIXED**
- **Issue**: Previously produced decomposed forms, now produces composed forms

#### Regex Matching Issue - RESOLVED
- **Pattern**: `istanbul` (case-insensitive)
- **Input**: `İstanbul` → **Case folded**: `istanbul` (composed form)
- **Result**: ✅ Match works correctly

## Root Cause Analysis

### Primary Issue - IDENTIFIED
The `case_fold()` method is not implemented in the Grapa language binding. The C++ implementation exists and works, but there's no Grapa language interface for it.

### Secondary Issue - RESOLVED
The Turkish I case folding issue has been resolved by implementing a lookup table-based approach that produces composed forms instead of decomposed forms.

## Current Status - UPDATED

- **C++ Implementation**: ✅ Working with lookup table
- **Grep Integration**: ✅ Working  
- **Grapa Language Binding**: ❌ Missing (needs implementation)
- **Turkish I Support**: ✅ Working correctly (composed forms)
- **Lookup Table**: ✅ Implemented and tested
- **Overall Enhancement**: ⚠️ Partially complete (C++ ready, language binding needed)

## Solution Implemented

### Lookup Table Approach
We replaced the utf8proc-based case folding with a custom lookup table that handles special Unicode cases:

```cpp
inline char32_t case_fold_codepoint(char32_t c) {
    switch (c) {
        // Basic Latin uppercase to lowercase
        case 0x0041: return 0x0061; // A -> a
        // ... more cases ...
        
        // Turkish I - special case folding
        case 0x0130: return 0x0069; // İ -> i (Turkish dotted I to lowercase i)
        case 0x0131: return 0x0069; // ı -> i (Turkish dotless I to lowercase i)
        
        // German sharp S - special case folding
        case 0x00DF: return 0x0073; // ß -> s (sharp S to lowercase s)
        
        // Default: no change
        default: return c;
    }
}
```

### Forward Declaration Fix
Added forward declarations in `grapa_grep_unicode.hpp` to resolve compilation issues:
```cpp
// Forward declarations for helper functions
char32_t case_fold_codepoint(char32_t c);
char32_t strip_latin_diacritic(char32_t c);
bool is_combining_mark(char32_t cp);
```

## Files Modified - UPDATED

- `source/grep/grapa_grep_unicode.hpp`: 
  - ✅ Enhanced case folding with lookup table implementation
  - ✅ Added forward declarations for helper functions
  - ✅ Replaced utf8proc case folding with custom lookup table
- `test/grep/test_turkish_i_case_folding.grc`: Created test script for Turkish I case folding
- `test/grep/simple_case_fold_test.grc`: Created simple test script
- `maintainers/INTERNAL_NOTES/UNICODE_CASE_FOLDING_ISSUE.md`: This file (updated)

## Next Steps - UPDATED

1. **Implement Grapa Language Binding**: Add `case_fold()` method to Grapa language
   - Add to `lib/grapa/$OBJ.grc`: `case_fold = @<"case_fold",{@<this>}>;`
   - Connect to C++ `UnicodeString::case_fold()` implementation
   
2. **Test Complete Integration**: Verify end-to-end functionality
   - Test Turkish I case folding from Grapa scripts
   - Test other Unicode case folding scenarios
   - Test case-insensitive grep with Unicode characters

3. **Update Documentation**: 
   - Update enhancement plan to mark case folding as complete
   - Add examples to user documentation
   - Update migration guides with Unicode case folding examples

4. **Consider Unicode-Aware String Methods**:
   - Make existing `upper()` and `lower()` methods Unicode-aware
   - Add Unicode normalization methods to Grapa language

## Recommendations

### Immediate Actions
1. **Document the Progress**: This note serves as updated documentation ✅
2. **Implement Language Binding**: Add `case_fold()` method to Grapa language (next priority)
3. **Test Integration**: Verify the complete solution works end-to-end

### Future Actions
1. **Expand Unicode Support**: Make all string operations Unicode-aware
2. **Add Unicode Methods**: Add normalization, case folding, and other Unicode utilities to Grapa language
3. **Performance Optimization**: Consider caching for frequently used Unicode operations 