# BACKLOG

**Current Date:** August 2025 (use `date` command to verify current date)

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).

---

## 🎯 **ACTIVE PRIORITIES** (Next Implementation Cycle)

### 🔥 **HIGH PRIORITY** (2 items)

#### **CLI Enhancement (Phase 2)**
- [ ] **Performance Options**: Add `-j/--jobs <N>` for parallel worker control and `--no-parallel` to disable parallelism
- [ ] **Environment Management**: Add `-E/--env <VAR=value>` for setting environment variables and `--env-file <file>` for loading environment files
- [ ] **Error Handling**: Add `--strict` mode (fail on warnings), `--continue` (continue on errors), and `--max-errors <N>` (stop after N errors)
- [ ] **Advanced Debugging**: Add `--trace` for execution tracing, `--dump-ast` for showing parsed AST, and `--dump-bytecode` for showing compiled bytecode
- [ ] **Performance Profiling**: Add `--profile` option for performance analysis and optimization insights

#### **Core Language Features**
- [ ] **String Comparison Distance Function**: Create dedicated function for detailed string comparison distance information (like current `<=>` operator behavior) for fuzzy matching applications
- [ ] **GrapaDB Float Comparison Support**: Add float comparison support to GrapaDB for index keys
- [ ] **Line Comments**: Add support for `//` and `#` line comments
- [ ] **Loop Syntax**: Add `for` loop as syntactic sugar for `.range()` patterns
- [ ] **Error Handling**: Improve `.iferr()` ergonomics and exception-based error handling

---

## 🆕 **NEW INITIATIVES** (2 items)

### **Database Engineering Best Practices Review** ⏳ **PENDING REVIEW**
- **Status**: Extracted valuable patterns from database development experience
- **Focus**: Debugging methodologies, testing strategies, implementation patterns
- **Next Steps**: Review extracted content, identify integration opportunities, update documentation
- **Reference**: [`../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md`](../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md)

### **Wikipedia Article Creation** ✅ **READY TO IMPLEMENT**
- **Status**: Article content prepared and ready for submission
- **Goal**: Improve Grapa's discoverability and establish official Wikipedia presence
- **Next Steps**: Submit article to Wikipedia, monitor feedback, maintain article
- **Reference**: `wikipedia_grapa_article.txt` - Complete article with instructions

---

## 🟡 **MEDIUM PRIORITY** (6 categories)

### **Performance & Optimization**
- [ ] **Optimization Implementation**: Performance improvements for arithmetic, bitwise, comparison, and assignment operators
  - **Status**: Requires extensive testing and its own release cycle
  - **Reference**: [`../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
- [ ] **Optimize large array operations**
- [ ] **Improve memory management for long-running scripts**
- [ ] **Add caching for frequently accessed data structures**
- [ ] **Performance Benchmarking**: Implement systematic performance comparison with ripgrep and other tools

### **Documentation**
- [ ] **Maintain and periodically audit API documentation for completeness and cross-linking**
- [ ] **Continue to add more real-world examples and user recipes to the user guide and examples section**
- [ ] **Create a dedicated performance tuning guide for advanced users**
- [ ] **Review and improve comment handling in Grapa**
- [ ] **Audit and update all code paths for correct field metadata lifecycle handling**
- [ ] **Add a high-level architecture and feature map diagram to the About or Home page**

### **Language Features**
- [ ] **Add support for more mathematical functions**
- [ ] **Implement advanced string manipulation functions**
- [ ] **Add support for regular expressions**
- [ ] **Consider adding support for custom operators**
- [ ] **Module System**: Design basic import/include system
- [ ] **Property Access**: Improve attribute-style access for objects
- [ ] **Boolean Logic**: Add implicit truthy/falsy support

### **Database Features**
- [ ] **Add support for more database backends**
- [ ] **Implement connection pooling**
- [ ] **Add support for transactions**
- [ ] **Improve query optimization**
- [ ] **Multi-Field Set/Get for Records**: Implement batch .set()/.get() operations for records with many fields
- [ ] **Expose custom index creation and management** to the Grapa language and CLI

### **Test Coverage Expansion**
- [ ] **Multiline + Lookaround Combinations**: Expand testing for complex scenarios
- [ ] **Option Matrix Completion**: Test all possible 4+ flag combinations
- [ ] **Performance Regression Testing**: Implement performance benchmarks vs ripgrep

### **Academic Documentation Gaps**
- [ ] **Grammar Mutation System Documentation**: Create comprehensive documentation for $RULE structure implementation
- [ ] **Weighted BTree Performance Analysis**: Document algorithmic complexity, performance benchmarks
- [ ] **Storage Model Comparison**: Create detailed comparison of COL, ROW, GROUP models

---

## 🟢 **LOW PRIORITY** (4 categories)

### **Development Tools**
- [ ] **Create debugging tools**
- [ ] **Add profiling capabilities**
- [ ] **Implement better error reporting**
- [ ] **Create IDE plugins**

### **Experimental Features**
- [ ] **Consider adding support for async/await patterns**
- [ ] **Investigate adding support for coroutines**
- [ ] **Explore adding support for metaprogramming features**

### **Integration**
- [ ] **Add support for more external libraries**
- [ ] **Create bindings for popular frameworks**
- [ ] **Implement plugin system**

### **Advanced Language Features**
- [ ] **Regular Expressions**: Add first-class regex support
- [ ] **Advanced Loops**: Add `foreach` and `range` constructs
- [ ] **IDE Integration**: Improve error messages and syntax highlighting

---

## ✅ **RECENTLY COMPLETED** (August 2025)

### **Major Achievements**
- ✅ **100% RIPGREP COMPATIBILITY** - Complete feature parity achieved
- ✅ **Custom Delimiter Edge Cases** - All 4 issues resolved (lookaround assertions, Unicode script properties, grapheme clusters, word boundaries)
- ✅ **Unicode Language Binding** - `case_fold()` method implemented and working
- ✅ **String Interpolation** - Grapa's existing capabilities are superior to traditional string interpolation
- ✅ **CLI Standardization** - Replaced non-standard options, removed conflicts, moved GUI options
- ✅ **Debug Statements Removed** - Production code cleaned up

### **Technical Improvements**
- ✅ **Missing Operator Documentation** - Completed comprehensive documentation for all 5 missing operators
- ✅ **Static Comparison Helper Function** - Implemented `DoComparison` static helper function
- ✅ **Comparison Operator Refactoring** - Refactored all comparison event classes
- ✅ **Float Comparison Fixes** - Fixed precision normalization and mixed type comparisons
- ✅ **String vs Number Comparison** - Improved handling with proper numeric conversion
- ✅ **CLI Interface Improvement** - Completed best-in-class CLI interface

---

## 📋 **Future/Long-Term Tasks**
- User-defined classes/objects: Improve documentation and idiomatic usage
- Continue documentation structure improvements: See [DOCS_STRUCTURE_IMPROVEMENT_PLAN.md](DEVELOPMENT/DOCS_STRUCTURE_IMPROVEMENT_PLAN.md)
- Evaluate all instances of null, true, and false handling in Grapa for consistency
- Automate running the full regression suite as part of CI

---

# 📄 **Backlog Summary**

- **Active Priorities**: 2 HIGH priority items (CLI Enhancement Phase 2, Core Language Features)
- **New Initiatives**: 2 items (Database Engineering Best Practices, Wikipedia Article)
- **Medium Priority**: 6 categories with multiple items each
- **Low Priority**: 4 categories with experimental and future features
- **Recently Completed**: 15+ major achievements documented

**For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).** 