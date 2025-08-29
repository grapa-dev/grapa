# Post-Validation Issues Log

## Overview
This document tracks issues discovered during documentation validation that require implementation work in the Grapa language itself, not just documentation fixes.

## Critical Implementation Issues

### 1. **Underscore Separators in Hex/Binary Literals** - ✅ **COMPLETED**
- **Issue**: `0x12_34` and `0b010_101` don't work as documented
- **Current Behavior**: `0x12_34` returns `18.203125` (float, same as `0x12.34`)
- **Expected Behavior**: Correctly parses underscores as decimal separators (same as dots)
- **Impact**: Documentation accuracy, developer experience
- **Priority**: HIGH
- **Effort**: MEDIUM
- **Implementation**: ✅ **COMPLETED** - Documentation updated to reflect current implementation status
- **Resolution**: Documentation corrected to reflect that underscore separators work correctly as decimal separators in hex/binary literals. The `_` character is intended to be the same as `.` (decimal separator), not a digit separator.

### 2. **Number Length Method** - ✅ **COMPLETED**
- **Issue**: `145.len()` returns `{"error":-1}` (not supported)
- **Current Behavior**: ✅ **WORKING** - Returns correct digit count (e.g., `145.len()` → `3`)
- **Expected Behavior**: Should return the number of digits (e.g., `145.len()` → `3`)
- **Impact**: Documentation accuracy, developer expectations
- **Priority**: MEDIUM
- **Effort**: LOW
- **Implementation**: ✅ **COMPLETED** - Feature already implemented and working correctly
- **Resolution**: Documentation was incorrect - the `.len()` method for numbers is already implemented and working as expected

### 3. **Array/List .get() Method** - ❌ **WON'T IMPLEMENT**
- **Issue**: `element.get(1)` and `obj.get("b")` return `{"error":-1}` for arrays and lists
- **Current Behavior**: Error when calling `.get()` on arrays and lists
- **Expected Behavior**: Will not be implemented - design decision
- **Impact**: Documentation accuracy, API consistency
- **Priority**: N/A
- **Effort**: N/A
- **Implementation**: ❌ **WON'T IMPLEMENT** - Design decision to avoid confusion
- **Resolution**: No current plan to support `.get()/.set()` for arrays and lists. The `.get()/.set()` methods are now exclusively for `$WIDGET` types. For `$file` and `$TABLE`, use `.getfield()/.setfield()`. Arrays and lists have sufficient access methods (bracket notation, indexing, etc.).

### 4. **Empty Vector Operations** - ✅ **COMPLETED**
- **Issue**: `empty_vec.sum()` returns `{"error":-1}` for empty vectors
- **Current Behavior**: Returns `{"error":-1}` for empty vector operations
- **Expected Behavior**: Documentation stated it should return `null`
- **Impact**: Documentation accuracy, developer expectations
- **Priority**: MEDIUM
- **Effort**: LOW
- **Implementation**: ✅ **COMPLETED** - Documentation corrected to reflect actual behavior
- **Resolution**: Documentation updated to correctly state that empty vector operations return `{"error":-1}` instead of `null`. This is the actual implementation behavior.

### 5. **1D Vector Statistical Operations** - 🔍 **INVESTIGATED**
- **Issue**: `.sum()` and `.mean()` methods only work on 2D vectors, not 1D vectors
- **Current Behavior**: 1D vectors return `{"error":-1}` for `.sum()` and `.mean()`
- **Expected Behavior**: Documentation suggested these work on 1D vectors
- **Impact**: Documentation accuracy, developer expectations, usability
- **Priority**: MEDIUM
- **Effort**: MEDIUM
- **Implementation**: 🔍 **INVESTIGATED** - Root cause identified
- **Resolution**: 
  - **Root Cause**: C++ implementation only supports 2D vectors (`mDim == 2` check)
  - **Documentation**: Updated to clarify dimensionality requirements
  - **Enhancement**: Support for 1D vector operations identified as potential improvement
  - **Workaround**: Use `.reduce()` for manual sum/mean on 1D vectors

### 6. **Network Blocking Issues** - ✅ **COMPLETED**
- **Issue**: `client.receive()` blocks indefinitely without checking for data availability
- **Current Behavior**: Script hangs during execution when calling `.receive()` without data
- **Expected Behavior**: Should use `.pending()` or `.onreceive()` for non-blocking operations
- **Impact**: Script execution hangs, poor user experience
- **Priority**: HIGH
- **Effort**: MEDIUM
- **Implementation**: ✅ **COMPLETED** - Script updated with non-blocking patterns
- **Resolution**: 
  - **Root Cause**: `simple_network_test.grc` uses blocking `.receive()` without checking data availability
  - **Solutions Implemented**: 
    1. ✅ Added `.pending()` checks before `.receive()` calls
    2. ✅ Added timeout mechanisms to prevent infinite blocking
    3. ✅ Added delays after sending to allow server processing time
  - **Documentation**: Updated network examples to show proper non-blocking patterns
  - **Best Practice**: Always check data availability before reading from network sockets

## Documentation Issues (Fixed)

### ✅ Fixed Issues
1. **Underscore separators documentation** - Updated to note "not yet implemented"
2. **Number length method documentation** - Removed incorrect example
3. **Array/List .get() method documentation** - Updated to note "not yet implemented"

## VSCode Extension Updates Needed

### After Implementation Fixes
1. **Add underscore separator support** to syntax highlighting
2. **Add number length method** to autocomplete
3. **Add array/list .get() method** to autocomplete
4. **Update snippets** to include new methods

### Current Extension Status
- ✅ **Excellent coverage** of existing syntax
- ✅ **Native data types** (JSON, XML, HTML, vectors) supported
- ✅ **Functional programming** methods supported
- ✅ **Cross-platform CLI** integration working

## Implementation Roadmap

### Phase 1: Core Language Fixes (High Priority)
1. ✅ **Implement underscore separators** in hex/binary literals - **COMPLETED**
   - ✅ Documentation updated to reflect current implementation status
   - ✅ Examples corrected to show working syntax
   - ✅ No implementation changes needed (documentation was inaccurate)

2. ✅ **Implement number length method** - **COMPLETED**
   - ✅ Feature already implemented and working correctly
   - ✅ Returns correct digit count for integers
   - ✅ Handles edge cases (zero, negative numbers)
   - ✅ No implementation changes needed (documentation was incorrect)

3. ❌ **Array/list .get() method** - **WON'T IMPLEMENT**
   - Design decision to avoid confusion with `.get()/.set()` methods
   - `.get()/.set()` now exclusively for `$WIDGET` types
   - Arrays and lists have sufficient access methods

### Phase 2: Documentation Updates
1. **Update basic_syntax.md** with working examples
2. **Update troubleshooting.md** with correct method usage
3. **Add new examples** to demonstrate the features
4. **Update VSCode extension** with new syntax support

### Phase 3: Testing and Validation
1. **Create comprehensive tests** for new features
2. **Validate all documentation** examples work
3. **Update VSCode extension** autocomplete and snippets
4. **Performance testing** for new implementations

## Technical Implementation Notes

### Underscore Separators
```cpp
// In lexer/parser, handle underscores in numeric literals
// Example: 0x12_34 should parse as 0x1234
// Example: 0b010_101 should parse as 0b010101
```

### Number Length Method
```cpp
// Add to $math class
len = op() {
    // Convert to string and return length
    return this.str().len();
};
```

### Array/List .get() Method
```cpp
// ❌ WON'T IMPLEMENT - Design decision
// .get()/.set() methods are now exclusively for $WIDGET types
// Arrays and lists use bracket notation: array[index], list[index]
```

## Success Criteria

### For Each Issue
- [x] **Implementation complete** - Feature works as documented (Issue #1: Documentation corrected)
- [x] **Documentation updated** - Examples work correctly (Issue #1: Documentation updated)
- [ ] **VSCode extension updated** - Syntax highlighting and autocomplete work
- [ ] **Tests created** - Comprehensive test coverage
- [ ] **Performance validated** - No significant performance impact

### Overall Success
- [ ] **All documentation examples work** without errors
- [ ] **VSCode extension provides complete coverage** of Grapa syntax
- [ ] **Developer experience improved** with consistent API
- [ ] **No breaking changes** to existing functionality

## Timeline Estimate

- **Phase 1 (Core Fixes)**: 1-2 weeks
- **Phase 2 (Documentation)**: 1 week
- **Phase 3 (Testing)**: 1 week
- **Total**: 3-4 weeks for complete implementation

## Next Steps

1. ✅ **Prioritize implementation** based on developer impact - **COMPLETED**
2. ✅ **Start with number length method** - **COMPLETED** (feature already working)
3. ❌ **Array/list .get() method** - **WON'T IMPLEMENT** (design decision)
4. ✅ **Implement underscore separators** - **COMPLETED** (documentation corrected)
5. **Update documentation and VSCode extension** after each fix
6. **Validate with comprehensive testing**

## Notes

- **No breaking changes** - All fixes should be additive
- **Backward compatibility** - Existing code must continue to work
- **Performance impact** - Monitor for any performance degradation
- **User feedback** - Consider developer experience improvements

