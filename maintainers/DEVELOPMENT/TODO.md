# Grapa & GrapaPy Persistent TODO (Internal)

> **NOTE (2024-12):** As of December 2024, `maintainers/` is the single source of truth for all context, TODO, and WIP. The old `docs/internal/` directory is deprecated and no longer updated. For all current internal documentation, use `maintainers/` only.

This file tracks all ongoing, planned, and recently completed tasks for Grapa and GrapaPy. It is updated regularly to ensure easy recovery after a crash or restart.

---

## Updated Action Plan (2024-12)

**Step 0:**
- Ensure `maintainers/` is the source of truth (done).
- Add a note to `docs/internal/README.md` redirecting to `maintainers/` (done).

**Step 1:**
- Continue all context, TODO, and WIP tracking in `maintainers/` only.

**Step 2:**
- Fix broken links and anchors in `docs-site/`.

**Step 3:**
- Complete Unicode case folding implementation (moved to backlog).

---

## High Priority TODO Items

### 🎯 **IMMEDIATE PRIORITIES**
- **To be determined based on current project needs**
- **Focus on core functionality and user-facing improvements**

---

## Medium Priority TODO Items

### 🔧 **Documentation and User Experience**
- **Status**: Ongoing
- **Priority**: MEDIUM
- **Focus**: Improve user onboarding, fix broken links, enhance examples

#### Tasks:
1. **Fix broken links and anchors in `docs-site/`**
2. **Improve user onboarding experience**
3. **Enhance code examples and tutorials**
4. **Cross-link documentation for better discoverability**

---

## Backlog Items

### 🔧 **Unicode Case Folding Language Binding**

**Status**: C++ Implementation Complete, Documentation Complete, Language Binding Missing  
**Priority**: BACKLOG  
**Estimated Effort**: 1-2 days  

#### Tasks:
1. **Implement `case_fold()` method in Grapa language**
   - Add method to `lib/grapa/$OBJ.grc` alongside existing `upper()` and `lower()` methods
   - Create language binding to expose C++ `UnicodeString::case_fold()` functionality
   - Ensure proper string handling and return types

2. **Test the language binding**
   - Create test script with Turkish I characters (İ, ı)
   - Test German sharp S (ß) case folding
   - Test Greek final sigma (ς, σ) case folding
   - Verify bidirectional matching works correctly

3. **Integration testing**
   - Test `case_fold()` with grep functionality
   - Verify case-insensitive matching works with Unicode characters
   - Test performance with large Unicode datasets

#### Success Criteria:
- `"İstanbul".case_fold()` returns `"istanbul"` (not empty string)
- `"Straße".case_fold()` returns `"strasse"`
- `"γράμμα".case_fold()` returns `"γράμμα"` (proper case folding)
- Integration with grep `i` option works correctly

#### Files to Modify:
- `lib/grapa/$OBJ.grc` - Add `case_fold` method
- `test/grep/test_unicode_case_folding.grc` - Create comprehensive test suite
- `maintainers/INTERNAL_NOTES/UNICODE_CASE_FOLDING_ISSUE.md` - Update status

### 🔧 **Unicode-Aware String Methods**

**Status**: Not Started  
**Priority**: BACKLOG  
**Estimated Effort**: 2-3 days  

#### Tasks:
1. **Update `upper()` method to be Unicode-aware**
   - Replace ASCII-only implementation with Unicode-aware version
   - Handle Turkish I characters correctly
   - Handle other special Unicode cases

2. **Update `lower()` method to be Unicode-aware**
   - Replace ASCII-only implementation with Unicode-aware version
   - Ensure consistency with case folding implementation

3. **Add comprehensive test suite**
   - Test all Unicode cases with `upper()` and `lower()`
   - Verify consistency with `case_fold()` behavior

#### Success Criteria:
- `"İstanbul".upper()` returns `"İSTANBUL"` (not `"İSTANBUL"`)
- `"Straße".lower()` returns `"straße"` (not `"straße"`)
- All methods consistent with Unicode standards

### 📊 **Performance Testing and Optimization**

**Status**: Not Started  
**Priority**: BACKLOG  
**Estimated Effort**: 1-2 days  

#### Tasks:
1. **Create performance benchmarks**
   - Test case folding with large Unicode datasets
   - Compare performance with utf8proc-based approach
   - Benchmark grep operations with Unicode case folding

2. **Optimize if needed**
   - Identify performance bottlenecks
   - Optimize lookup table access patterns
   - Consider caching strategies

#### Success Criteria:
- Case folding performance acceptable for large datasets
- No significant performance regression in grep operations
- Performance metrics documented

---

## Recently Completed Items

### ✅ **Unicode Case Folding C++ Implementation** (December 2024)
- **Status**: COMPLETED
- **Description**: Implemented lookup table-based Unicode case folding in C++
- **Files**: `source/grep/grapa_grep_unicode.hpp`
- **Features**: Turkish I, German sharp S, Greek final sigma, common accented characters
- **Approach**: Composed forms for better regex compatibility

### ✅ **Unicode Case Folding Documentation** (December 2024)
- **Status**: COMPLETED
- **Description**: Comprehensive documentation updates for Unicode case folding
- **Files**: `docs-site/docs/grep.md`, `docs-site/docs/obj/grep_python.md`, `docs-site/docs/grep/quick_reference.md`, `docs-site/docs/EXAMPLES.md`, `docs-site/docs/TESTING.md`
- **Coverage**: Complete examples, implementation notes, testing patterns

### ✅ **Issue Tracking and Roadmap Updates** (December 2024)
- **Status**: COMPLETED
- **Description**: Updated maintainers documentation with current status
- **Files**: `maintainers/INTERNAL_NOTES/UNICODE_CASE_FOLDING_ISSUE.md`, `maintainers/DEVELOPMENT/grapa_language_enhancement_roadmap.md`, `maintainers/DEVELOPMENT/TODO.md`
- **Content**: Current status, next steps, technical details

---

## Technical Debt

### 🔧 **Maintenance Tasks**
- **Debug Output**: Remember to disable `GRAPA_DEBUG_PRINTF` before committing
- **Test Monitoring**: Run full test suite after any changes
- **Documentation**: Keep docs in sync with implementation changes

### 📋 **Code Quality**
- **Error Handling**: Ensure proper error handling in all operations
- **Memory Management**: Verify no memory leaks in operations
- **Edge Cases**: Test with edge cases and malformed data

---

## How to Resume

### **Current Session Status** (December 2024)
- **C++ Implementation**: ✅ Complete and working
- **Documentation**: ✅ Complete and comprehensive
- **Language Binding**: ❌ Missing - In backlog
- **Test Suite**: Ready to create when needed

### **Key Files for Future Sessions**
- `lib/grapa/$OBJ.grc` - Add `case_fold` method (when needed)
- `test/grep/test_unicode_case_folding.grc` - Create test suite (when needed)
- `maintainers/INTERNAL_NOTES/CURRENT_CONTEXT.md` - Current status
- `maintainers/DEVELOPMENT/TODO.md` - This file

### **Next Session Goals**
- Focus on immediate project priorities
- Address documentation and user experience improvements
- Work on backlog items as time permits

---

**Last Updated**: December 2024  
**Current Priority**: Documentation and User Experience Improvements  
**Status**: C++ Complete, Documentation Complete, Language Binding in Backlog 