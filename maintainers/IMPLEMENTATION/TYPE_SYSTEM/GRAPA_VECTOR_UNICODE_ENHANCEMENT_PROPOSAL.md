# GrapaVector Unicode Enhancement Proposal

## Overview

This document proposes specific updates to `GrapaVector.cpp` to bring header row operations in line with Grapa's modern Unicode support, as demonstrated in string operations like `.len()`, `.rpad()`, and `.lpad()`.

## Current State Analysis

### Unicode Support in String Operations (GrapaLibRule.cpp)

Modern Grapa string functions use comprehensive Unicode support:

```cpp
// Pattern from GrapaLibraryRuleLenEvent::Run (lines 15915-15925)
// For strings, count Unicode characters (grapheme clusters) instead of bytes
std::string input_str(reinterpret_cast<const char*>(r1.vVal->mValue.mBytes), r1.vVal->mValue.mLength);

// Count grapheme clusters (Unicode characters)
size_t char_count = 0;
size_t offset = 0;
while (offset < input_str.size()) {
    std::string cluster = extract_grapheme_cluster(input_str, offset);
    if (cluster.empty()) break;
    char_count++;
    offset += cluster.size();
}
```

### Current GrapaVector.cpp Limitations

```cpp
// Current header processing (lines 630-635) - byte-based only
if (cols == 1 && !isQuoted && vL >= 3 && memcmp(vS, "ï»¿", 3) == 0) {
    vS += 3;  // Only handles UTF-8 BOM
    vL -= 3;
}
mLabels.PushTail(new GrapaRuleEvent(0, GrapaBYTE(vS, vL), GrapaCHAR("")));
```

**Problems:**
1. **Byte-based length calculations** (`vL` represents bytes, not characters)
2. **No Unicode normalization** for header names
3. **No case folding** for header comparisons
4. **Limited BOM handling** (only UTF-8 BOM)

## Proposed Enhancements

### 1. Unicode-Aware Header Storage

**Current Code (lines 630-635):**
```cpp
if (cols == 1 && !isQuoted && vL >= 3 && memcmp(vS, "ï»¿", 3) == 0) {
    vS += 3;
    vL -= 3;
}
mLabels.PushTail(new GrapaRuleEvent(0, GrapaBYTE(vS, vL), GrapaCHAR("")));
```

**Proposed Enhancement:**
```cpp
// Enhanced header processing with Unicode support
char* processed_header = vS;
u64 processed_len = vL;

// Handle UTF-8 BOM (existing functionality)
if (cols == 1 && !isQuoted && vL >= 3 && memcmp(vS, "ï»¿", 3) == 0) {
    processed_header += 3;
    processed_len -= 3;
}

// Optional: Normalize header name for consistent comparisons
std::string header_str(processed_header, processed_len);
// Could add normalization here if needed for header comparisons

mLabels.PushTail(new GrapaRuleEvent(0, GrapaBYTE(processed_header, processed_len), GrapaCHAR("")));
```

### 2. Unicode-Aware Header Search

**Current Code (line 2959):**
```cpp
GrapaRuleEvent* b = mLabels.Search(a->mValue, index);
```

**Enhancement Needed:**
The `mLabels.Search()` function should support Unicode-aware string comparison. This would require updating the search mechanism to use Unicode normalization and case folding.

### 3. Unicode-Aware Header Length Calculations

**Problem:** Header operations that depend on string length use byte counts instead of character counts.

**Proposed Helper Function:**
```cpp
// Add to GrapaVector.cpp - Unicode-aware string length
size_t GetUnicodeLength(const char* str, size_t byte_len) {
    std::string input_str(str, byte_len);
    size_t char_count = 0;
    size_t offset = 0;
    
    while (offset < input_str.size()) {
        std::string cluster = extract_grapheme_cluster(input_str, offset);
        if (cluster.empty()) break;
        char_count++;
        offset += cluster.size();
    }
    
    return char_count;
}
```

### 4. Enhanced BOM Detection

**Current Code:** Only handles UTF-8 BOM
**Proposed Enhancement:** Support additional Unicode BOMs

```cpp
// Enhanced BOM detection
bool ProcessUnicodeBOM(char*& vS, u64& vL) {
    // UTF-8 BOM: EF BB BF
    if (vL >= 3 && memcmp(vS, "\xEF\xBB\xBF", 3) == 0) {
        vS += 3;
        vL -= 3;
        return true;
    }
    
    // UTF-16 BE BOM: FE FF  
    if (vL >= 2 && memcmp(vS, "\xFE\xFF", 2) == 0) {
        // Note: Would need UTF-16 to UTF-8 conversion
        // For now, just detect and skip
        vS += 2;
        vL -= 2;
        return true;
    }
    
    // UTF-16 LE BOM: FF FE
    if (vL >= 2 && memcmp(vS, "\xFF\xFE", 2) == 0) {
        // Note: Would need UTF-16 to UTF-8 conversion
        vS += 2;
        vL -= 2;
        return true;
    }
    
    return false;
}
```

## Implementation Priority

### High Priority (Recommended)
1. **Include Unicode headers** - Add `#include "../grep/grapa_grep_unicode.hpp"`
2. **Unicode-aware header length** - Use grapheme cluster counting for header operations
3. **Enhanced BOM detection** - Support UTF-16 BOMs in addition to UTF-8

### Medium Priority
1. **Unicode normalization** - Normalize header names for consistent comparison
2. **Case-insensitive header search** - Support case-folding in header lookups

### Low Priority (Future Enhancement)
1. **UTF-16 conversion** - Full UTF-16 to UTF-8 conversion for non-UTF-8 CSV files
2. **Locale-aware sorting** - Locale-specific header sorting

## Backward Compatibility

### ✅ **Safe Changes**
- Enhanced BOM detection (additive)
- Unicode-aware length calculations (improves accuracy)
- Including Unicode headers (no runtime impact if not used)

### ⚠️ **Potential Breaking Changes**
- **Header comparison behavior** - Unicode normalization might change matching behavior
- **Length calculations** - Character count vs byte count could affect existing code
- **Performance impact** - Unicode processing is slower than byte processing

### 🛡️ **Compatibility Strategy**
1. **Feature flags** - Make Unicode processing optional via environment variables
2. **Gradual rollout** - Implement basic features first, advanced features later
3. **Documentation** - Clearly document behavior changes

## Required Dependencies

### Already Available
- `extract_grapheme_cluster()` function
- `utf8proc` library
- Unicode normalization functions

### New Includes Needed
```cpp
#include "../grep/grapa_grep_unicode.hpp"
```

## Testing Requirements

### Unit Tests Needed
1. **Unicode header names** - CSV with non-ASCII headers
2. **Mixed encodings** - UTF-8, UTF-16 BOMs
3. **Grapheme clusters** - Headers with combining characters
4. **Case sensitivity** - Header lookup with different cases
5. **Performance** - Large CSV files with Unicode headers

### Test Cases
```grapa
// Unicode header test
unicode_csv = "Naïve,Café,Résumé\nValue1,Value2,Value3";
vec = $vector(unicode_csv);
assert(vec["Naïve"] != null);
assert(vec["Café"] != null);

// BOM test  
bom_csv = "ï»¿Name,Value\nTest,123";
vec2 = $vector(bom_csv);
assert(vec2["Name"] != null);  // Should work after BOM removal

// Case insensitive test (if implemented)
vec3 = $vector("Name,Value\nTest,123");
assert(vec3["name"] == vec3["Name"]);  // Case folding
```

## Implementation Phases

### Phase 1: Foundation
- Add Unicode header includes
- Implement `GetUnicodeLength()` helper
- Enhanced BOM detection

### Phase 2: Core Features  
- Unicode-aware header storage
- Grapheme cluster counting in header operations

### Phase 3: Advanced Features
- Unicode normalization
- Case-insensitive header search
- Performance optimization

## Conclusion

These enhancements would bring GrapaVector's header support in line with modern Grapa string operations, providing:

1. **Consistent behavior** with other Grapa string functions
2. **International CSV support** with proper Unicode handling
3. **Improved user experience** for non-ASCII data
4. **Future-proof architecture** for additional Unicode features

The changes can be implemented incrementally with careful attention to backward compatibility.


