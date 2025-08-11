# BACKLOG

**Current Date:** August 2025 (use `date` command to verify current date)

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).

---

## 🎯 **IMMEDIATE NEXT STEPS** (Pull to current_status.md)

**Status:** No immediate items - all active work completed ✅

**Next Priority:** When ready for new development, consider pulling from Major Releases section below.

---

## 🚀 **MAJOR RELEASES** (6 items)

### **1. Grapa Syntax Improvements** - **LANGUAGE ADOPTION FOUNDATION** 🔥 **HIGHEST PRIORITY**
- [ ] **Core Control Flow Fixes**: Fix `return` and `break` statements - both are currently non-functional
- [ ] **Line Comments**: Add support for `//` and `#` line comments
- [ ] **For Loop Syntax**: Add `for` loop as syntactic sugar for `.range()` patterns
- [ ] **Exception Handling**: Add `try/catch` blocks and exception handling
- [ ] **String Interpolation**: Add template literals and string interpolation
- [ ] **Module System**: Design basic import/include system
- [ ] **Property Access**: Improve attribute-style access for objects
- **Reference**: [`GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (3-6 months)
- **Dependencies**: None (foundational work)
- **Priority**: Highest among major releases - foundational for language adoption

### **2. Enhanced Debug Mode** - **DEVELOPMENT EXPERIENCE** 🔥 **HIGH PRIORITY**
- [ ] **Comprehensive Debug System**: Implement sophisticated debugging capabilities including execution tracing, variable inspection, performance profiling, and AST/bytecode dumping
- [ ] **Debug Infrastructure**: Core debug system with configurable debug levels and thread-safe debug context management
- [ ] **Execution Tracing**: Step-by-step execution tracking with variable state snapshots
- [ ] **Performance Profiling**: Built-in performance timing and memory usage tracking
- [ ] **AST/Bytecode Dumping**: Execution tree visualization and bytecode inspection
- [ ] **Debug CLI Integration**: Enhanced debug options and output formats
- **Reference**: [`GRAPA_ENHANCED_DEBUG_MODE_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_ENHANCED_DEBUG_MODE_PLAN.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: Syntax Improvements (for error handling)
- **Priority**: High - enables advanced development capabilities

### **3. Distutils Removal and Shared Library Naming** - **BUILD SYSTEM MODERNIZATION** 🔥 **HIGH PRIORITY**
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

### **4. GrapaDB Enhancements** - **DATABASE SYSTEM** 🟡 **MEDIUM PRIORITY**
- [ ] **Performance Optimization**: Index performance enhancement, memory management optimization, query optimization
- [ ] **Advanced Features**: Transaction support, advanced querying, data validation and constraints
- [ ] **Scalability and Reliability**: Large dataset support, backup and recovery, concurrent access
- [ ] **Integration and Testing**: Language integration, performance monitoring, comprehensive testing
- **Reference**: [`GRAPA_DB_ENHANCEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_DB_ENHANCEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (4-8 months)
- **Dependencies**: Syntax Improvements (for error handling)
- **Priority**: Medium - database-specific enhancements

### **5. String Comparison Distance Function** - **UTILITY FEATURE** 🟡 **MEDIUM PRIORITY**
- [ ] **Core Distance Algorithms**: Implement edit distance (Levenshtein), Hamming distance, and basic similarity scoring
- [ ] **Advanced Similarity Algorithms**: Implement Jaro-Winkler, Cosine similarity, and N-gram similarity
- [ ] **Language Integration**: Add distance and similarity methods to GrapaCHAR class and language binding
- [ ] **Performance Optimization**: Optimize algorithms for large strings and memory efficiency
- [ ] **Testing and Documentation**: Comprehensive test suite, documentation, and examples
- **Reference**: [`GRAPA_STRING_DISTANCE_FUNCTION_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_STRING_DISTANCE_FUNCTION_PLAN.md)
- **Estimated Effort**: Major Release (3-6 months)
- **Dependencies**: Core language features completion
- **Priority**: Medium - utility feature enhancement

### **6. Optimization Implementation** - **PERFORMANCE** 🟢 **LOWER PRIORITY**
- [ ] **Performance improvements** for arithmetic, bitwise, comparison, and assignment operators
- **Status**: Requires extensive testing and its own release cycle
- **Reference**: [`GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling)
- **Priority**: Lower - performance optimization after core features are stable

---

## 🆕 **NEW INITIATIVES** (3 items)

### **Wikipedia Article Creation** ✅ **READY TO IMPLEMENT**
- **Status**: Article content prepared and ready for submission
- **Goal**: Improve Grapa's discoverability and establish official Wikipedia presence
- **Next Steps**: Submit article to Wikipedia, monitor feedback, maintain article
- **Reference**: `wikipedia_grapa_article.txt` - Complete article with instructions
- **Priority**: Medium - marketing and visibility

### **Database Engineering Best Practices Review** ⏳ **PENDING REVIEW**
- **Status**: Extracted valuable patterns from database development experience
- **Focus**: Debugging methodologies, testing strategies, implementation patterns
- **Next Steps**: Review extracted content, identify integration opportunities, update documentation
- **Reference**: [`../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md`](../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md)

### **Grapa Rules System Enhancement Roadmap** 🔧 **COMPREHENSIVE PLANNING**
- **Status**: Comprehensive roadmap created with 12 major enhancements across 4 phases
- **Focus**: Modern language features, developer experience, advanced capabilities
- **Timeline**: 16-week phased implementation (4 phases, 4 weeks each)
- **Priority**: High - foundational for language adoption and developer experience
- **Reference**: [`../IMPLEMENTATION/RULES_SYSTEM_ENHANCEMENT_ROADMAP.md`](../IMPLEMENTATION/RULES_SYSTEM_ENHANCEMENT_ROADMAP.md)
- **Next Steps**: Prioritize Phase 1 items, begin exception handling implementation

### **LLM-Powered Dynamic IDE for Grapa** 🤖 **REVOLUTIONARY RESEARCH**
- **Status**: Comprehensive concept document created with detailed implementation strategy
- **Focus**: Revolutionary IDE that uses LLMs to understand Grapa's dynamic syntax in real-time
- **Timeline**: 2-3 year phased implementation (REPL → VSCode Extension → Full IDE)
- **Priority**: Research/Experimental - could revolutionize IDE development for dynamic languages
- **Reference**: [`../IMPLEMENTATION/LLM_POWERED_DYNAMIC_IDE_CONCEPT.md`](../IMPLEMENTATION/LLM_POWERED_DYNAMIC_IDE_CONCEPT.md)
- **Next Steps**: Evaluate feasibility, consider Phase 1 proof of concept
- **Risk Level**: High - ambitious but potentially groundbreaking

---

## 🟡 **MEDIUM PRIORITY** (7 categories)

### **Infrastructure & Dependencies**
- [ ] **OpenSSL 3.0 Migration**: Upgrade from OpenSSL 1.1.1 to OpenSSL 3.0 for security improvements and modern cryptographic standards
  - **Security Benefits**: Latest security patches, improved cryptographic algorithms, better TLS support
  - **Compatibility**: Ensure all cryptographic functions work with OpenSSL 3.0 APIs
  - **Testing**: Comprehensive testing of all crypto operations after migration
  - **Dependencies**: Update build scripts, dependency management, and platform-specific configurations
  - **Risk Assessment**: Medium - requires careful API compatibility testing
  - **Estimated Effort**: 2-4 weeks
  - **Priority**: High within medium priority - security and compatibility upgrade
- [ ] **FLTK 1.4.4 Migration**: Upgrade from FLTK 1.3.11 to FLTK 1.4.4 for improved compatibility and support
  - **Official Recommendation**: FLTK team explicitly recommends 1.4.4 over 1.3.11
  - **End of Life**: 1.3.11 is considered "old software" and may not compile on new platforms
  - **Compatibility**: FLTK states "many older apps can use it with little or no change"
  - **Benefits**: Bug fixes, improvements, and future platform compatibility
  - **Testing**: Verify GUI components (My_Text_Console, My_Prompt_Editor) work correctly
  - **Dependencies**: Update dep/fltk-1.3.11-source.tar.gz to fltk-1.4.4-source.tar.gz
  - **Risk Assessment**: Low - FLTK maintains API compatibility between 1.3.x and 1.4.x
  - **Estimated Effort**: 1-2 weeks
  - **Priority**: Medium - recommended upgrade for long-term maintenance

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

## 🔍 **INVESTIGATION ITEMS** (1 item)

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

---

## ✅ **RECENTLY COMPLETED** (August 2025)

### **Major Achievements**
- ✅ **100% RIPGREP COMPATIBILITY** - Complete feature parity achieved
- ✅ **Custom Delimiter Edge Cases** - All 4 issues resolved (lookaround assertions, Unicode script properties, grapheme clusters, word boundaries)
- ✅ **Unicode Language Binding** - `case_fold()` method implemented and working
- ✅ **String Interpolation** - Grapa's existing capabilities are superior to traditional string interpolation
- ✅ **CLI Standardization** - Replaced non-standard options, removed conflicts, moved GUI options
- ✅ **Debug Statements Removed** - Production code cleaned up
- ✅ **Session Debug Output Stream Separation** - `SendError()` method implemented and validated
- ✅ **Debug Output Documentation** - Comprehensive documentation updates completed

### **Technical Improvements**
- ✅ **Missing Operator Documentation** - Completed comprehensive documentation for all 5 missing operators
- ✅ **Static Comparison Helper Function** - Implemented `DoComparison` static helper function
- ✅ **Comparison Operator Refactoring** - Refactored all comparison event classes
- ✅ **Float Comparison Fixes** - Fixed precision normalization and mixed type comparisons
- ✅ **String vs Number Comparison** - Improved handling with proper numeric conversion
- ✅ **CLI Interface Improvement** - Completed best-in-class CLI interface

### **Completed Investigations**
- ✅ **Programming Language Naming Investigation** - Decision made to retain "Grapa" branding

---

# 📄 **Backlog Summary**

- **Immediate Next Steps**: 0 items - all active work completed ✅
- **Major Releases**: 6 items (prioritized by impact and dependencies)
- **New Initiatives**: 2 items (Wikipedia Article, Database Best Practices)
- **Medium Priority**: 7 categories with multiple items each
- **Low Priority**: 4 categories with experimental and future features
- **Investigation Items**: 1 item (SQL Syntax Injection Demo)
- **Recently Completed**: 15+ major achievements documented

**Priority Order Rationale:**
1. **Grapa Syntax Improvements** - Critical for language adoption and developer experience
2. **Enhanced Debug Mode** - Advanced development capabilities (depends on Syntax Improvements)
3. **Distutils Removal and Shared Library Naming** - Critical for Python 3.13+ compatibility and build system consistency
4. **GrapaDB Enhancements** - Database-specific improvements
5. **String Comparison Distance Function** - Utility feature enhancement
6. **Optimization Implementation** - Performance optimization after core features are stable 