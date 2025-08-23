# GrapaVector BOM Detection Improvement Proposal

**Date:** Current  
**File:** `source/grapa/GrapaVector.cpp`  
**Issue:** Hardcoded BOM detection with limited support

## Current Implementation Analysis

### Current BOM Detection Code
```cpp
// In GrapaVector::FROM (line 674)
if (cols == 1 && !isQuoted && vL >= 3 && memcmp(vS, "ï»¿", 3) == 0)
{
    vS += 3;
    vL -= 3;
}
```

### Issues with Current Implementation

1. **Hardcoded String Literal**: Uses `"ï»¿"` which may not represent actual UTF-8 BOM bytes correctly
2. **Limited BOM Support**: Only handles UTF-8 BOM, not UTF-16 LE/BE
3. **Inconsistent Behavior**: Tests show BOM detection not working as expected
4. **No Error Handling**: No validation of BOM detection success

## Proposed Improvements

### Option 1: Byte Constants (Recommended)

Replace the hardcoded string literal with actual byte constants:

```cpp
// Replace current BOM detection with:
if (cols == 1 && !isQuoted && vL >= 3)
{
    // UTF-8 BOM: 0xEF 0xBB 0xBF
    if ((unsigned char)vS[0] == 0xEF && 
        (unsigned char)vS[1] == 0xBB && 
        (unsigned char)vS[2] == 0xBF)
    {
        vS += 3;
        vL -= 3;
    }
    // UTF-16 LE BOM: 0xFF 0xFE
    else if (vL >= 2 && 
             (unsigned char)vS[0] == 0xFF && 
             (unsigned char)vS[1] == 0xFE)
    {
        vS += 2;
        vL -= 2;
    }
    // UTF-16 BE BOM: 0xFE 0xFF
    else if (vL >= 2 && 
             (unsigned char)vS[0] == 0xFE && 
             (unsigned char)vS[1] == 0xFF)
    {
        vS += 2;
        vL -= 2;
    }
}
```

### Option 2: Helper Function

Create a dedicated BOM detection function:

```cpp
// Add to GrapaVector class
private:
    struct BOMInfo {
        u64 offset;
        u64 length;
        const char* encoding;
    };
    
    BOMInfo DetectBOM(const char* data, u64 length) {
        BOMInfo info = {0, 0, "UTF-8"};
        
        if (length >= 3 && 
            (unsigned char)data[0] == 0xEF && 
            (unsigned char)data[1] == 0xBB && 
            (unsigned char)data[2] == 0xBF)
        {
            info.offset = 3;
            info.length = 3;
            info.encoding = "UTF-8";
        }
        else if (length >= 2 && 
                 (unsigned char)data[0] == 0xFF && 
                 (unsigned char)data[1] == 0xFE)
        {
            info.offset = 2;
            info.length = 2;
            info.encoding = "UTF-16LE";
        }
        else if (length >= 2 && 
                 (unsigned char)data[0] == 0xFE && 
                 (unsigned char)data[1] == 0xFF)
        {
            info.offset = 2;
            info.length = 2;
            info.encoding = "UTF-16BE";
        }
        
        return info;
    }

// Usage in FROM method:
BOMInfo bom = DetectBOM(vS, vL);
if (bom.length > 0) {
    vS += bom.offset;
    vL -= bom.offset;
    // Could store encoding info for future use
}
```

### Option 3: utf8proc Integration

Since utf8proc is already available in the codebase, use its BOM detection:

```cpp
#include "../grep/grapa_grep_unicode.hpp"

// In BOM detection:
if (cols == 1 && !isQuoted && vL >= 3) {
    utf8proc_int32_t state = UTF8PROC_ACCEPT;
    utf8proc_ssize_t result = utf8proc_iterate(
        (const utf8proc_uint8_t*)vS, 
        vL, 
        &state
    );
    
    if (result == 3 && state == UTF8PROC_ACCEPT) {
        // Valid UTF-8 BOM detected
        vS += 3;
        vL -= 3;
    }
}
```

## Recommended Implementation

### Phase 1: Fix Current UTF-8 BOM Detection
```cpp
// Replace line 674 in GrapaVector.cpp
if (cols == 1 && !isQuoted && vL >= 3 && 
    (unsigned char)vS[0] == 0xEF && 
    (unsigned char)vS[1] == 0xBB && 
    (unsigned char)vS[2] == 0xBF)
{
    vS += 3;
    vL -= 3;
}
```

### Phase 2: Add UTF-16 Support
```cpp
if (cols == 1 && !isQuoted && vL >= 2)
{
    // UTF-8 BOM: 0xEF 0xBB 0xBF
    if (vL >= 3 && 
        (unsigned char)vS[0] == 0xEF && 
        (unsigned char)vS[1] == 0xBB && 
        (unsigned char)vS[2] == 0xBF)
    {
        vS += 3;
        vL -= 3;
    }
    // UTF-16 LE BOM: 0xFF 0xFE
    else if ((unsigned char)vS[0] == 0xFF && 
             (unsigned char)vS[1] == 0xFE)
    {
        vS += 2;
        vL -= 2;
    }
    // UTF-16 BE BOM: 0xFE 0xFF
    else if ((unsigned char)vS[0] == 0xFE && 
             (unsigned char)vS[1] == 0xFF)
    {
        vS += 2;
        vL -= 2;
    }
}
```

## Testing Strategy

### Test Cases to Add
1. **UTF-8 BOM**: `0xEF 0xBB 0xBF + "Name,Value\nAlice,100"`
2. **UTF-16 LE BOM**: `0xFF 0xFE + "Name,Value\nAlice,100"`
3. **UTF-16 BE BOM**: `0xFE 0xFF + "Name,Value\nAlice,100"`
4. **No BOM**: `"Name,Value\nAlice,100"`
5. **Invalid BOM**: `0xEF 0xBB 0x00 + "Name,Value\nAlice,100"`

### Expected Results
- BOM should be detected and removed
- Headers should be correctly parsed after BOM removal
- Data should be accessible by header names
- No data corruption should occur

## Benefits

1. **Reliability**: Byte constants are more reliable than string literals
2. **Compatibility**: Support for multiple BOM types
3. **Maintainability**: Clear, explicit BOM detection logic
4. **Performance**: Minimal overhead for BOM detection
5. **Standards Compliance**: Follows Unicode BOM standards

## Implementation Priority

1. **High Priority**: Fix current UTF-8 BOM detection (Phase 1)
2. **Medium Priority**: Add UTF-16 support (Phase 2)
3. **Low Priority**: utf8proc integration (if needed)

## Conclusion

The current hardcoded BOM detection should be replaced with byte constants for better reliability and support for multiple BOM types. This will improve the robustness of CSV parsing with international content.
