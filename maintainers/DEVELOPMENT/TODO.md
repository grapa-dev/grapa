# Grapa & GrapaPy Development TODO (Internal)

> **NOTE (July 2024):** As of July 2024, `maintainers/` is the single source of truth for all context, TODO, and WIP. The old `docs/internal/` directory is deprecated and no longer updated. For all current internal documentation, use `maintainers/` only.

This file tracks all ongoing, planned, and recently completed tasks for Grapa and GrapaPy. It is updated regularly to ensure easy recovery after a crash or restart.

---

## Current Status (July 2024)

### ✅ **Recently Completed (July 2024)**
- **Documentation Site Deployment**: Fixed GitHub Pages deployment issues, all pages now working at https://grapa-dev.github.io/grapa/
- **Database Issue Investigation**: Resolved string key retrieval issue in ROW tables with documented workaround
- **Test Organization**: All test files organized into logical subdirectories
- **Internal Function Analysis**: Comprehensive analysis of 200+ internal Grapa functions completed
- **Operator Analysis**: Complete operator inventory with type support analysis
- **Optimization Backlog**: Created detailed optimization roadmap with implementation examples

---

## 🎯 **IMMEDIATE PRIORITIES**

### 1. **Operator Documentation Audit** (HIGH PRIORITY)
**Status**: Ready to start  
**Estimated Effort**: 2-3 days  
**Dependencies**: Core functionality understanding (pending code review)

#### Tasks:
1. **Audit all 25 operators** from BNF grammar against current documentation
2. **Document missing operators**: `*/` (root), `<=>` (cmp), `++` (extend), `--` (remove), `.*` (dot)
3. **Create comprehensive type support tables** for each operator
4. **Document array assignment behavior**: `+=` (nesting) vs `++=` (flattening)
5. **Update operator precedence guide** based on BNF grammar
6. **Add operator examples** to documentation

#### Files to Update:
- `docs-site/docs/operators/assignment.md` - Add array examples and type behavior
- `docs-site/docs/API_REFERENCE.md` - Expand operator sections with type support tables
- `docs-site/docs/syntax/operator.md` - Add comprehensive operator precedence and type rules
- `docs-site/docs/EXAMPLES.md` - Add operator examples

#### Context References:
- `maintainers/INTERNAL_NOTES/GRAPA_OPERATORS_ANALYSIS.md` - Complete operator analysis
- `lib/grapa/$grapa.grc` (lines 540-630) - BNF operator definitions
- `source/grapa/GrapaLibRule.cpp` - Operator implementations

---

## 🔧 **MEDIUM PRIORITY ITEMS**

### 2. **Optimization Implementation** (Phase 1)
**Status**: Ready to start  
**Estimated Effort**: 1-2 weeks  
**Priority**: MEDIUM

#### Phase 1 Optimizations (High Impact, Easy Implementation):
1. **Arithmetic operators** (`+`, `-`, `*`, `/`, `%`, `**`, `*/`) - 50-80% runtime reduction
2. **Bitwise operators** (`&`, `|`, `^`, `<<`, `>>`, `~`) - 70-90% runtime reduction
3. **Comparison operators** (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`) - Constant folding
4. **Assignment operators** (`=`, `+=`, `++=`, `-=`) - String and numeric optimization
5. **Type conversions** (`int`, `float`, `str`) - Compile-time conversion

#### Reference:
- `maintainers/INTERNAL_NOTES/GRAPA_OPTIMIZATION_BACKLOG.md` - Detailed implementation roadmap

### 3. **Unicode Language Binding**
**Status**: C++ Implementation Complete, Language Binding Missing  
**Priority**: MEDIUM  
**Estimated Effort**: 1-2 days  

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

## 📋 **BACKLOG ITEMS**

### 4. **$KEY Field Datatype Investigation**
**Priority**: LOW  
**Description**: Investigate allowing any datatype for the $KEY field instead of hardcoded string type  
**Status**: Pending  
**Notes**: Current workaround is to use .raw().int() or .raw().float() for numeric keys

### 5. **Language Feature Enhancements**
**Priority**: LOW  
**Description**: Add missing language features identified in roadmap  
**Status**: Pending  

#### Potential Features:
- `for`/`foreach` loops (currently only `while`)
- Exception handling (`try/catch`)
- String interpolation/formatting
- Module/import system
- List comprehensions

#### Reference:
- `maintainers/DEVELOPMENT/grapa_language_enhancement_roadmap.md` - Complete feature analysis

---

## 🔧 **MAINTENANCE TASKS**

### 6. **Documentation Link Fixes**
**Status**: Ongoing  
**Priority**: LOW  
**Description**: Fix remaining broken links in documentation build

### 7. **Test Organization Documentation**
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
- **Documentation**: ✅ Complete and deployed
- **Internal Analysis**: ✅ Complete (functions, operators, optimization)
- **Operator Documentation**: ❌ Needs audit - Ready to start
- **Optimization Implementation**: ❌ Ready to start
- **Unicode Language Binding**: ❌ Ready to start

### **Key Files for Current Session**
- `maintainers/INTERNAL_NOTES/GRAPA_OPERATORS_ANALYSIS.md` - Operator analysis
- `maintainers/INTERNAL_NOTES/GRAPA_OPTIMIZATION_BACKLOG.md` - Optimization roadmap
- `maintainers/DEVELOPMENT/TODO.md` - This file
- `maintainers/DEVELOPMENT/current_investigation_status.md` - Investigation status

### **Next Session Goals**
1. **Operator Documentation Audit** - Primary focus
2. **Core functionality understanding** - Review code you'll point me to
3. **Optimization implementation** - Phase 1 items
4. **Unicode language binding** - Complete implementation

---

**Last Updated**: July 18, 2024  
**Current Priority**: Operator Documentation Audit  
**Status**: Documentation Complete, Analysis Complete, Ready for Implementation 