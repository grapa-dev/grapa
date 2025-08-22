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
- [ ] **Exception Handling**: try/catch blocks for structured error handling
- [ ] **Control Flow Fixes**: Working return/break/continue statements
- [ ] **Property Access**: Improve attribute-style access for objects
- **Reference**: [`GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_SYNTAX_IMPROVEMENTS_PLAN.md)
- **Estimated Effort**: Major Release (3-6 months)
- **Dependencies**: None (foundational work)
- **Priority**: Highest among major releases - foundational for language adoption

### **2. Enhanced Debug Mode** - **DEVELOPMENT EXPERIENCE** 🔥 **HIGH PRIORITY**
- [ ] **Comprehensive Debug System**: Implement sophisticated debugging capabilities including execution tracing, variable inspection, performance profiling, and AST/bytecode dumping
- [ ] **Debug Infrastructure**: Core debug system with configurable debug levels and thread-safe debug context management
- [ ] **Execution Tracing**: Step-by-step execution tracking with variable state snapshots
- [ ] **Advanced Performance Profiling**: Memory usage tracking and detailed performance analysis (basic timing already available via `$TIME`)
- [ ] **Execution Tree Dumping**: Execution tree visualization and inspection
- [ ] **Advanced Variable Inspection**: Interactive debugging with variable snapshots and scope examination
- [ ] **Debug CLI Integration**: Enhanced debug options and output formats
- **Reference**: [`GRAPA_ENHANCED_DEBUG_MODE_PLAN.md`](../RESEARCH_AND_ANALYSIS/GRAPA_ENHANCED_DEBUG_MODE_PLAN.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: None (foundational work)
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



### **6. Advanced Language Features** - **MODERN LANGUAGE PARITY** 🟡 **MEDIUM PRIORITY**
- [ ] **Enhanced Reflection**: Runtime code inspection and modification
- [ ] **Extension System**: Extending existing types with new methods
- **Estimated Effort**: Major Release (4-6 months)
- **Dependencies**: Syntax Improvements (for error handling)
- **Priority**: Medium - modern language parity features

### **7. Optimization Implementation** - **PERFORMANCE** 🟢 **LOWER PRIORITY**
- [ ] **Performance improvements** for arithmetic, bitwise, comparison, and assignment operators
- **Status**: Requires extensive testing and its own release cycle
- **Reference**: [`GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
- **Estimated Effort**: Major Release (6-12 months)
- **Dependencies**: Core language features completion, Syntax Improvements (for error handling)
- **Priority**: Lower - performance optimization after core features are stable

---

## 🆕 **NEW INITIATIVES** (4 items)

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

### **`.exec()` Method Enhancement** 🔧 **PARAMETER SUPPORT REVISIT**
- **Status**: `.exec()` method has callback, set, get parameters designed in C++ but not exposed in Grapa syntax
- **Background**: Created before `op()()` was fully implemented, parameter handling designed but never fully exposed
- **Goal**: Revisit whether to enhance `.exec()` with parameter support or keep it simple as `op()(script)()` equivalent
- **Options**: 
  - Keep simple (current behavior) - equivalent to `op()(script)()`
  - Expose callback parameter for custom response handling
  - Expose set/get parameters for custom variable scope management
  - Full parameter support for specialized execution contexts
- **Priority**: Low - current functionality works well, enhancement would be nice-to-have
- **Estimated Effort**: 1-2 weeks (analysis and implementation)
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
- [ ] **Consider adding support for custom operators**

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
- [ ] **Create IDE plugins**
- **Note**: Debugging tools and profiling capabilities are covered in "Enhanced Debug Mode" major release
- **Note**: All tooling enhancements (IDE integration, debugging, profiling, AST dumping) are covered in this section and "Enhanced Debug Mode" major release
- **Note**: All debugging tools (stack traces, profiling, hot reloading, AST dumping) are comprehensively covered in "Enhanced Debug Mode" major release

### **Experimental Features**
- [ ] **Consider adding support for async/await patterns**
- [ ] **Investigate adding support for coroutines**
- [ ] **Explore adding support for metaprogramming features**

### **Future Ecosystem Features** 🔮 **LOW PRIORITY**
- [ ] **C++ Extension Mechanism** - System for developers to create and distribute C++ extensions to Grapa's Event libraries
  - **Scope**: Mechanism for third-party C++ extensions that integrate with Grapa's core Event system
  - **Distribution**: Future PyPI-like system for C++ extension distribution
  - **Timeline**: Only when Grapa gains significant traction and ecosystem
  - **Dependencies**: Requires significant user base and developer community
  - **Complexity**: Major architectural feature requiring distribution infrastructure
  - **Note**: This is a "nice to have" for future ecosystem growth, not a current priority

### **Integration**
- [ ] **Add support for more external libraries**
- [ ] **Create bindings for popular frameworks**
- [ ] **Implement plugin system**

### **Advanced Language Features**
- [ ] **IDE Integration**: Improve error messages and syntax highlighting

---

## 🔧 **INFRASTRUCTURE & PLATFORM** (1 item)

### **Platform-Aware Library Directory Implementation** 🔧 **INFRASTRUCTURE ENHANCEMENT**
- **Status**: Initial implementation complete but reverted due to compilation issues and testing challenges
- **Focus**: Implement platform-specific default library directory paths for better user experience
- **Context**: Currently uses development-focused paths, but production users need platform-appropriate paths
- **Current Search Order** (existing functionality):
  1. **Current Working Directory**: `{current_working_directory}/lib/grapa`
  2. **System Paths**: `/usr/lib/grapa` → `/usr/local/lib/grapa` 
  3. **Binary Directory**: `{binary_directory}/lib/grapa`
  4. **Static Library**: Built-in compressed library files
- **Proposed Platform Requirements**:
  - **Windows**: `%USERPROFILE%\.grapa\lib` → `%PROGRAMFILES%\Grapa\lib` → `{current_directory}\lib\grapa`
  - **macOS**: `~/Library/Application Support/Grapa/lib` → `/usr/local/lib/grapa` → `/opt/grapa/lib` → `{current_directory}/lib/grapa`
  - **Linux/BSD**: `~/.local/lib/grapa` → `/usr/local/lib/grapa` → `/usr/lib/grapa` → `{current_directory}/lib/grapa`
- **Implementation Details**:
  - Modify `gSystem->mLibDir` initialization in `source/grapa/GrapaLink.cpp`
  - Ensure proper ordering after `mHomeDir` initialization
  - Add proper `struct stat sb;` declaration for directory existence checks
  - Test automatic file loading mechanism with platform-aware paths
- **Challenges Identified**:
  - Variable scoping issues with `struct stat sb` declaration
  - Interaction with existing grammar file loading logic (`$grapa.grz`)
  - Testing automatic file loading mechanism complexity
- **Progress**: 
  - ✅ Platform-specific path logic designed and implemented
  - ✅ Build system compilation successful after fixes
  - ✅ Platform-aware directory detection working (`/Users/matichuk/Library/Application Support/Grapa/lib` on macOS)
  - ❌ Automatic file loading testing inconclusive (requires further investigation)
  - ❌ Changes reverted due to testing complexity and scope
- **Next Steps**: 
  - Revisit implementation with proper variable scoping from the start
  - Implement comprehensive testing strategy for automatic file loading
  - Ensure grammar file loading logic compatibility
  - Update documentation for new default paths
- **Documentation Updates Needed**:
  - `docs-src/docs/module_system.md` - Platform-specific search paths
  - `docs-src/docs/getting_started.md` - Platform-aware installation paths
  - `maintainers/IMPLEMENTATION/CORE_SYSTEM/AUTOMATIC_FILE_LOADING_MECHANISM.md` - Updated search paths
- **Reference**: Implementation was in `source/grapa/GrapaLink.cpp` with platform-specific `#ifdef` blocks
- **Priority**: Medium - improves user experience but not critical for core functionality
- **Estimated Effort**: 1-2 weeks (implementation + testing + documentation)
- **Risk Level**: Medium - affects core system initialization and file loading behavior

---

## 🔍 **INVESTIGATION ITEMS** (3 items)

### **SQL Syntax Injection Demo** 🔍 **INVESTIGATION NEEDED**
- **Status**: Initial investigation complete, design concerns identified
- **Focus**: Create working demonstration of custom SQL syntax injection using Grapa's grammar extension capabilities
- **Context**: Educational demonstration showing how `custom_command` and `custom_function` can extend Grapa's syntax
- **Current Issue**: Simple custom grammar rules work (e.g., `select $INT`), but complex SQL rules (e.g., `select $STR from $STR`) fail to parse properly
- **Design Concerns**: 
  - BNF rule design may not be optimal for SQL syntax
  - `findall` function doesn't work on `$TABLE` objects (only XML/LIST/ARRAY/TUPLE)
  - WHERE clause implementation would require manual iteration over `table.ls()`
  - Leverage opportunities limited - would need significant new C++ code for WHERE conditions
- **Progress**: 
  - ✅ Basic custom function demonstration working
  - ✅ Script compilation issues resolved
  - ✅ Core limitation identified (complex rule parsing)
  - ✅ Database operation analysis complete (ls/get/set leverage possible, findall not available)
- **Next Steps**: Reconsider BNF design approach, evaluate if SQL syntax is the right demonstration vehicle
- **Reference**: [`test/use_cases/sql_syntax_injection_table_demo.grc`](../../test/use_cases/sql_syntax_injection_table_demo.grc) - Current test script
- **Priority**: Low - educational/demonstration value, design reconsideration needed

### **ReplaceLocalQueue Optimization** 🔍 **INVESTIGATION NEEDED**
- **Status**: Function currently disabled, basic functionality working via complete copy approach
- **Focus**: Revisit the selective copying optimization for local variable scope exit handling
- **Context**: `ReplaceLocalQueue` function was designed to selectively copy only local variables that are referenced in returned objects, rather than doing complete copies
- **Current State**: 
  - Function is disabled with `return;` at the start (`GrapaState.cpp:5061`)
  - No calls to `ReplaceLocalQueue` exist in the codebase
  - Basic scope exit scenarios work correctly via complete copy approach
- **Original Intent**: 
  - Traverse returned objects to find references to local variables
  - Copy only the referenced local variables before scope exit
  - Avoid unnecessary copying of large objects when only small local variables are referenced
- **Complexity**: Implementation became complex and was abandoned in favor of simpler complete copy approach
- **Progress**: 
  - ✅ mVar field analysis completed
  - ✅ Scope exit behavior verified working correctly
  - ✅ ReplaceLocalQueue function identified as disabled optimization
  - ✅ Current complete copy approach confirmed sufficient for basic cases
- **Next Steps**: Evaluate if selective copying optimization is worth implementing, or if complete copy approach is sufficient
- **Reference**: [`maintainers/IMPLEMENTATION/CORE_SYSTEM/GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md`](../../maintainers/IMPLEMENTATION/CORE_SYSTEM/GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md) - Complete analysis
- **Priority**: Low - current implementation works correctly, optimization may not be necessary

### **ProcessPlan Control Flow Copy Optimization** 🔍 **INVESTIGATION NEEDED**
- **Status**: Automatic copy in control flow section removed, intelligent copy detection now working correctly
- **Focus**: Evaluate all automatic copies in ProcessPlan for performance optimization opportunities
- **Context**: The automatic copy in `if (result->mControlFlow)` section was interfering with intelligent copy detection system
- **Current State**: 
  - Automatic copy removed from control flow section in `GrapaScriptExec::ProcessPlan`
  - Intelligent copy detection now works properly for local variable references
  - Throw scenarios with local variable references now work correctly
- **Performance Impact**: 
  - Each automatic copy needs evaluation for necessity
  - Local variables are tagged appropriately for recursive inspection
  - Copy should only be made if local variable references are detected
- **Progress**: 
  - ✅ Throw scenario fixed by removing unnecessary automatic copy
  - ✅ Intelligent copy detection system now working as intended
  - ✅ mVar system properly detecting local variable references
- **Next Steps**: 
  - Audit all automatic copies in ProcessPlan for similar optimization opportunities
  - Implement recursive inspection to detect when copies are actually needed
  - Ensure performance optimization without breaking existing functionality
- **Optimization Ideas**: 
  - **mClean flag approach**: Add `mClean` boolean flag that is set to `true` after a copy is made, then checked before making subsequent copies to avoid redundant copying
  - **Recursive inspection**: Use existing `mVar` system to detect when local variable references are present and only copy when necessary
- **Priority**: Medium - performance optimization, but current system works correctly

---



---

# 📄 **Backlog Summary**

- **Immediate Next Steps**: 0 items - all active work completed ✅
- **Major Releases**: 7 items (prioritized by impact and dependencies)
- **New Initiatives**: 4 items (Wikipedia Article, Database Best Practices, Rules System Enhancement, LLM-Powered Dynamic IDE)
- **Medium Priority**: 7 categories with multiple items each
- **Low Priority**: 5 categories with experimental and future features
- **Investigation Items**: 1 item (SQL Syntax Injection Demo)

**Priority Order Rationale:**
1. **Grapa Syntax Improvements** - Critical for language adoption and developer experience
2. **Enhanced Debug Mode** - Advanced development capabilities (depends on Syntax Improvements)
3. **Distutils Removal and Shared Library Naming** - Critical for Python 3.13+ compatibility and build system consistency
4. **GrapaDB Enhancements** - Database-specific improvements
5. **Advanced Language Features** - Modern language parity features
6. **Optimization Implementation** - Performance optimization after core features are stable 