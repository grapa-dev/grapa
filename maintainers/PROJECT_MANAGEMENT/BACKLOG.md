# BACKLOG

**Current Date:** August 2025 (use `date` command to verify current date)

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).

---

## 🎯 **IMMEDIATE NEXT STEPS** (Pull to current_status.md)

**Note:** CLI Enhancement Phase 2 has been moved to [`CURRENT_STATUS.md`](CURRENT_STATUS.md) as the active priority.

**Next Priority:** Session-Specific Environment Variables - Implementation ready, see [`SESSION_SPECIFIC_ENVIRONMENT_VARIABLES_PLAN.md`](../RESEARCH_AND_ANALYSIS/SESSION_SPECIFIC_ENVIRONMENT_VARIABLES_PLAN.md)

---

## 🚀 **MAJOR RELEASES** (5 items)

### **2. Grapa Syntax Improvements** - **LANGUAGE ADOPTION FOUNDATION**
- [ ] **Core Control Flow Fixes**: Fix `return` and `break` statements - both are currently non-functional
- [ ] **Line Comments**: Add support for `//` and `#` line comments
- [ ] **For Loop Syntax**: Add `for` loop as syntactic sugar for `.range()` patterns
- [ ] **Exception Handling**: Add `try/catch` blocks and exception handling
- [ ] **String Interpolation**: Add template literals and string interpolation
- [ ] **Module System**: Design basic import/include system
- [ ] **Property Access**: Improve attribute-style access for objects
- **Reference**: [`GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (3-6 months)
- **Dependencies**: CLI Enhancement Phase 2 (error reporting)
- **Priority**: Highest among major releases - foundational for language adoption

### **3. Enhanced Debug Mode** - **DEVELOPMENT EXPERIENCE**
- [ ] **Comprehensive Debug System**: Implement sophisticated debugging capabilities including execution tracing, variable inspection, performance profiling, and AST/bytecode dumping
- [ ] **Debug Infrastructure**: Core debug system with configurable debug levels and thread-safe debug context management
- [ ] **Execution Tracing**: Step-by-step execution tracking with variable state snapshots
- [ ] **Performance Profiling**: Built-in performance timing and memory usage tracking
- [ ] **AST/Bytecode Dumping**: Execution tree visualization and bytecode inspection
- [ ] **Debug CLI Integration**: Enhanced debug options and output formats
- **Reference**: [`GRAPA_ENHANCED_DEBUG_MODE_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_ENHANCED_DEBUG_MODE_PLAN.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: CLI Enhancement Phase 2 completion, Syntax Improvements (for error handling)
- **Priority**: High - enables advanced development capabilities

### **4. GrapaDB Enhancements** - **DATABASE SYSTEM**
- [ ] **Performance Optimization**: Index performance enhancement, memory management optimization, query optimization
- [ ] **Advanced Features**: Transaction support, advanced querying, data validation and constraints
- [ ] **Scalability and Reliability**: Large dataset support, backup and recovery, concurrent access
- [ ] **Integration and Testing**: Language integration, performance monitoring, comprehensive testing
- **Reference**: [`GRAPA_DB_ENHANCEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_DB_ENHANCEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (4-8 months)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling)
- **Priority**: Medium - database-specific enhancements

### **5. String Comparison Distance Function** - **UTILITY FEATURE**
- [ ] **Core Distance Algorithms**: Implement edit distance (Levenshtein), Hamming distance, and basic similarity scoring
- [ ] **Advanced Similarity Algorithms**: Implement Jaro-Winkler, Cosine similarity, and N-gram similarity
- [ ] **Language Integration**: Add distance and similarity methods to GrapaCHAR class and language binding
- [ ] **Performance Optimization**: Optimize algorithms for large strings and memory efficiency
- [ ] **Testing and Documentation**: Comprehensive test suite, documentation, and examples
- **Reference**: [`GRAPA_STRING_DISTANCE_FUNCTION_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_STRING_DISTANCE_FUNCTION_PLAN.md)
- **Estimated Effort**: Major Release (3-6 months)
- **Dependencies**: Core language features completion
- **Priority**: Medium - utility feature enhancement

### **6. Optimization Implementation** - **PERFORMANCE**
- [ ] **Performance improvements** for arithmetic, bitwise, comparison, and assignment operators
- **Status**: Requires extensive testing and its own release cycle
- **Reference**: [`GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling)
- **Priority**: Lower - performance optimization after core features are stable

### **7. Distutils Removal and Shared Library Naming** - **BUILD SYSTEM MODERNIZATION**
- [ ] **Distutils Dependency Removal**: Replace deprecated `distutils` with `setuptools` for Python 3.13+ compatibility
- [ ] **Shared Library Naming Convention**: Implement consistent `_static` vs `_shared` naming across all platforms
- [ ] **Unified Library Locations**: Move all libraries to `source/grapa-lib/{platform}/` directory
- [ ] **Mac Build Process Update**: Remove separate `grapa-other` directory, use consistent naming
- [ ] **Cross-Platform Consistency**: Ensure Linux, AWS, and Windows use same naming patterns
- **Reference**: [`DISTUTILS_REMOVAL_PLAN.md`](../RESEARCH_AND_ANALYSIS/DISTUTILS_REMOVAL_PLAN.md)
- **Estimated Effort**: Major Release (4-6 weeks)
- **Dependencies**: None (foundational build system work)
- **Priority**: High - critical for Python 3.13+ compatibility and build system consistency
- **Risk Level**: Medium - affects all platforms and build processes

---

## 🆕 **NEW INITIATIVES** (3 items)

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

### **SQL Syntax Injection Demo** 🔍 **INVESTIGATION NEEDED**
- **Status**: Initial investigation complete, core issue identified
- **Focus**: Create working demonstration of custom SQL syntax injection using Grapa's grammar extension capabilities
- **Context**: Educational demonstration showing how `custom_command` and `custom_function` can extend Grapa's syntax
- **Current Issue**: Simple custom grammar rules work (e.g., `select $INT`), but complex SQL rules (e.g., `select $STR from $STR`) fail to parse properly
- **Progress**: 
  - ✅ Basic custom function demonstration working
  - ✅ Script compilation issues resolved
  - ✅ Core limitation identified (complex rule parsing)
- **Next Steps**: Investigate rule syntax limitations, create working SQL demo or document limitations
- **Reference**: [`test/use_cases/sql_syntax_injection_table_demo.grc`](../../test/use_cases/sql_syntax_injection_table_demo.grc) - Current test script
- **Priority**: Low - educational/demonstration value

### **Programming Language Naming Investigation** ✅ **COMPLETED**
- **Status**: Investigation completed, decision finalized
- **Goal**: Evaluate whether to retain "Grapa" or consider rebranding alternatives
- **Scope**: Domain availability analysis, naming patterns research, rebranding strategy evaluation
- **Key Findings**: Decision made to retain "Grapa" - zero migration cost, clear namespace control, low collision risk, thematic relevance
- **Reference**: [`PROGRAMMING_LANGUAGE_NAMING_INVESTIGATION.md`](../RESEARCH_AND_ANALYSIS/PROGRAMMING_LANGUAGE_NAMING_INVESTIGATION.md)
- **Estimated Effort**: Completed (2-3 months research)
- **Dependencies**: None (independent research initiative)
- **Priority**: Completed - strategic branding decision made
- **Risk Level**: Mitigated - no rebranding required
- **Next Steps**: Implement brand strengthening plan (domain registration, SEO optimization, Wikipedia preparation)

---

## 🟡 **MEDIUM PRIORITY** (6 categories)

### **Performance & Optimization**
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

# 📄 **Backlog Summary**

- **Immediate Next Steps**: 0 items (CLI Enhancement Phase 2 moved to current_status.md)
- **Major Releases**: 6 items (Grapa Syntax Improvements, Enhanced Debug Mode, GrapaDB Enhancements, String Comparison Distance Function, Optimization Implementation, Distutils Removal and Shared Library Naming)
- **New Initiatives**: 2 items (Database Engineering Best Practices, Wikipedia Article)
- **Medium Priority**: 6 categories with multiple items each
- **Low Priority**: 4 categories with experimental and future features
- **Recently Completed**: 15+ major achievements documented

**Priority Order Rationale:**
1. **Grapa Syntax Improvements** - Critical for language adoption and developer experience
2. **Enhanced Debug Mode** - Advanced development capabilities (depends on CLI Phase 2)
3. **GrapaDB Enhancements** - Database-specific improvements
4. **String Comparison Distance Function** - Utility feature enhancement
5. **Distutils Removal and Shared Library Naming** - Critical for Python 3.13+ compatibility and build system consistency
6. **Optimization Implementation** - Performance optimization after core features are stable 