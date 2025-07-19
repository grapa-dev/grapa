# Grapa & GrapaPy Development TODO (Internal)

> **NOTE (July 2024):** As of July 2024, `maintainers/` is the single source of truth for all context, TODO, and WIP. The old `docs/internal/` directory is deprecated and no longer updated. For all current internal documentation, use `maintainers/` only.

**PURPOSE**: This file tracks items **committed to work on in the near term** (next 2-4 weeks). For items planned but not committed to soon, see `backlog.md`.

---

## ✅ **COMPLETED ITEMS (Recent Successes)**

### **GRAPHEME CLUSTERS (\\X) REGRESSION - FIXED AND VERIFIED** ✅
**Status**: ✅ **COMPLETE** - Newlines now included in \\X output  
**Completed**: December 2024  
**Source**: Manual verification testing revealed regression from documented behavior

#### Issue Description:
- **Documented Behavior**: `"é\\n😀".grep("\\\\X", "o")` should return `["é", "\\n", "😀"]`
- **Problem**: Newlines were missing from the output (regression from documented functionality)
- **Impact**: HIGH - This affected a documented core feature

#### Root Cause Analysis:
**IDENTIFIED**: The issue was in `source/grep/grep_unicode.cpp` lines 234-253

**Problem**: The grapheme cluster handling logic split input by delimiter first, then processed each line separately, which excluded newlines entirely from grapheme cluster processing.

#### Fix Applied:
**FIXED**: Modified the grapheme cluster logic to process the entire input as one string instead of splitting by delimiter, so that newlines are included as separate grapheme clusters.

**Changes Made**:
1. **`source/grep/grep_unicode.cpp`**: Changed from `split_by_delimiter(working_input, line_delim)` to processing `full_input` directly
2. **`source/grep/grapa_grep_unicode.cpp`**: Updated `extract_grapheme_cluster` function to properly handle newlines as separate clusters

#### Verification Results:
**✅ VERIFIED**: Test 10 in `test/grep/test_unicode_grapheme_clusters.grc` now passes:
- **Input**: `"é\\n😀"`
- **Pattern**: `\\X`, Options: `o`
- **Result**: `["é", "\\n", "😀"]` ✅
- **Expected**: `["é", "\\n", "😀"]` ✅

**Full Regression Test**: ✅ All critical functionality verified working correctly
**Production Readiness**: ✅ All production scenarios tested and passing

**Status**: ✅ **COMPLETE** - Grapheme clusters with newlines now work as documented

---

## 🎯 **CURRENT PRIORITIES (Next 2-4 weeks)**

### 1. **CRITICAL GREP REGRESSIONS** (COMPLETED)
**Status**: ✅ **FULLY FIXED** - Both line numbers and JSON output working correctly 
**Estimated Effort**: COMPLETED 
**Source**: Regression testing revealed broken functionality

#### Critical Bugs:
1. **Line Numbers (`n` option)**: ✅ **FIXED** - Now returns `["1:Hello world", "3:Hello universe"]`
2. **JSON Output (`j` option)**: ✅ **FIXED** - Now returns proper JSON format
3. **Test Validation Logic**: ✅ **IMPROVED** - Tests now properly validate actual content

**Next**: Grapheme clusters regression investigation (Item #1 above)

---

### 2. **Operator Bug Fixes** (HIGH PRIORITY - Week 1)
**Status**: Ready to start  
**Estimated Effort**: 1-2 weeks  
**Source**: From backlog Item #11

#### Critical Bugs to Fix:
1. **STR > INT behavior**: `"hello" > 5` returns TRUE (should be FALSE)
2. **Logical NOT FLOAT**: `!5.0` returns TRUE (should be FALSE)  
3. **Logical NOT complex**: `!(5 && 3)` returns FALSE (should be TRUE)
4. **STR <=> INT**: Returns numbers instead of errors

#### Design Decisions to Confirm:
- Array/List object ID comparison vs content comparison
- Type conversion philosophy across operators

#### Files to Update:
- `source/grapa/GrapaLibRule.cpp` - Fix operator implementations
- `test/operators/` - Add test cases for fixes
- `docs-site/docs/syntax/operator.md` - Update with fixes

---

### 2. **Cryptographic Features Stabilization** (HIGH PRIORITY - Week 2)
**Status**: Ready to start  
**Estimated Effort**: 1-2 weeks  
**Source**: From backlog Item #9

#### Critical Issues to Address:
1. **Fix AKS routing logic bug** - Add crash protection to experimental functions
2. **Fix staticprime() return value** - Improve error handling
3. **Document working features** - genprime(), isprime(), modpow(), modinv(), gcd()

#### Documentation Needs:
- Add to USE_CASES.md Section 3 (High-Precision Math & Scientific Computing)
- Create dedicated `docs/docs/cryptography.md` page
- Document OpenSSL integration and security benefits

#### Files to Update:
- `source/grapa/GrapaLibRule.cpp` - Fix crypto function bugs
- `docs-site/docs/cryptography.md` - Create crypto documentation
- `docs-site/docs/USE_CASES.md` - Add crypto use cases

---

### 3. **Optimization Implementation** (MEDIUM PRIORITY - Week 3)
**Status**: Ready to start  
**Estimated Effort**: 1-2 weeks  
**Source**: From TODO optimization backlog

#### Phase 1 Optimizations (High Impact, Easy Implementation):
1. **Arithmetic operators** (`+`, `-`, `*`, `/`, `%`, `**`, `*/`) - 50-80% runtime reduction
2. **Bitwise operators** (`&`, `|`, `^`, `<<`, `>>`, `~`) - 70-90% runtime reduction
3. **Comparison operators** (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`) - Constant folding
4. **Assignment operators** (`=`, `+=`, `++=`, `-=`) - String and numeric optimization

#### Reference:
- `maintainers/INTERNAL_NOTES/GRAPA_OPTIMIZATION_BACKLOG.md` - Detailed implementation roadmap

---

### 4. **Unicode Language Binding** (MEDIUM PRIORITY - Week 4)
**Status**: Ready to start  
**Estimated Effort**: 1-2 days  
**Source**: From TODO unicode binding

#### Tasks:
1. **Add `case_fold()` method** to `lib/grapa/$OBJ.grc`
2. **Connect to C++ implementation** in `source/grep/grapa_grep_unicode.hpp`
3. **Test Turkish I case folding** from Grapa scripts
4. **Make `upper()` and `lower()` Unicode-aware**

#### Success Criteria:
- `"İstanbul".case_fold()` returns `"istanbul"`
- `"Straße".case_fold()` returns `"strasse"`
- Integration with grep `i` option works correctly

---

### 5. **Grep Edge Case Fixes** (LOW PRIORITY - Future)
**Status**: Noted for future work  
**Estimated Effort**: 1-2 weeks  
**Source**: Manual verification testing

#### Minor Issues to Address:
1. **Grapheme clusters with newlines**: The `\X` pattern may not include newline characters in output as expected (regression from documented behavior)
2. **Context line precision**: Context options (A<n>, B<n>, C<n>) may include slightly different line counts compared to ripgrep in some edge cases
3. **Zero-length match edge cases**: Some zero-length match scenarios may return multiple empty strings instead of single empty string

#### Priority:
- **Medium**: Grapheme clusters with newlines (regression)
- **Low**: Context line precision (minor formatting difference)
- **Low**: Zero-length match edge cases (edge case, not affecting core functionality)

#### Documentation:
- Already noted in `docs/docs/GREP.md` under "Backlog Items"
- Tracked in `maintainers/TESTING_AND_QUALITY/TESTING.md`

---

## ✅ **RECENTLY COMPLETED**

### **Documentation Site Reorganization** (July 2024)
- **Status**: ✅ COMPLETED
- **Scope**: Broke out USE_CASES into individual subsections for better navigation
- **Changes**: 
  - Created `use_cases/` directory with 8 individual use case files
  - Moved Cryptography from top menu to USE_CASES subsection
  - Updated navigation structure for improved user experience
  - Removed old monolithic USE_CASES.md file
- **Benefits**: Better navigation, easier to find specific use cases, cleaner organization
- **Next**: Operator bug fixes (Item #1 above)

### **Operator Documentation Audit** (July 2024)
- **Status**: ✅ COMPLETED
- **Scope**: All 26 operators audited, 243 gaps identified, 5 bugs tracked
- **Documentation**: `docs-site/docs/syntax/operator.md` fully updated
- **Next**: Operator bug fixes (Item #1 above)

### **Documentation Site Deployment** (July 2024)
- **Status**: ✅ COMPLETED  
- **Scope**: Fixed GitHub Pages deployment, all pages working
- **URL**: https://grapa-dev.github.io/grapa/
- **Deployment Guide**: `maintainers/DEVELOPMENT/DOCUMENTATION_DEPLOYMENT_GUIDE.md` - Complete process for future deployments

### **Test Organization** (July 2024)
- **Status**: ✅ COMPLETED
- **Scope**: All test files organized into logical subdirectories
- **Structure**: database/, file_system/, core/, infrastructure/, use_cases/, python/, grep/

---

## 🔧 **MAINTENANCE TASKS**

### **Documentation Link Fixes**
**Status**: Ongoing  
**Priority**: LOW  
**Description**: Fix remaining broken links in documentation build

### **Test Organization Documentation**
**Status**: Pending  
**Priority**: LOW  
**Description**: Document new test structure for contributors

---

## Technical Debt

### 🔧 **Code Quality**
- **Debug Output**: Remember to disable `GRAPA_DEBUG_PRINTF` before committing
- **Test Monitoring**: Run full test suite after any changes
- **Documentation**: Keep docs in sync with implementation changes

### 📋 **Error Handling**
- **Error Handling**: Ensure proper error handling in all operations
- **Memory Management**: Verify no memory leaks in operations
- **Edge Cases**: Test with edge cases and malformed data

---

## How to Resume

### **Current Session Status** (July 2024)
- **Documentation Reorganization**: ✅ Complete
- **Operator Documentation**: ✅ Complete
- **Operator Bug Fixes**: ❌ Ready to start (Priority #1)
- **Crypto Stabilization**: ❌ Ready to start (Priority #2)
- **Optimization**: ❌ Ready to start (Priority #3)
- **Unicode Binding**: ❌ Ready to start (Priority #4)

### **Key Files for Current Session**
- `maintainers/DEVELOPMENT/backlog.md` - Long-term items not committed to soon
- `maintainers/INTERNAL_NOTES/GRAPA_OPERATORS_ANALYSIS.md` - Operator analysis
- `maintainers/INTERNAL_NOTES/GRAPA_OPTIMIZATION_BACKLOG.md` - Optimization roadmap
- `maintainers/DEVELOPMENT/TODO.md` - This file (committed items only)

### **Next Session Goals**
1. **Operator Bug Fixes** - Fix STR > INT, Logical NOT, STR <=> INT issues
2. **Crypto Stabilization** - Fix AKS routing, document working features
3. **Optimization Implementation** - Phase 1 performance improvements
4. **Unicode Language Binding** - Complete Unicode implementation

---

**Last Updated**: July 18, 2024  
**Current Priority**: Operator Bug Fixes  
**Status**: Documentation Complete, Ready for Implementation 