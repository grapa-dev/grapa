# BACKLOG

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md). Items are grouped by priority and area. Completed items are listed at the end for reference.

---

## 🆕 **NEW: Database Engineering Best Practices Review**

### Status: ⏳ **PENDING REVIEW** - Extracted valuable patterns from ROW bug investigation

**Latest Update**: Created comprehensive database engineering best practices document (`../RESEARCH_AND_ANALYSIS/DATABASE_ENGINEERING_BEST_PRACTICES.md`) that extracts valuable debugging methodologies, testing strategies, and implementation patterns from the ROW Table Index Corruption Bug investigation. This document preserves general engineering knowledge while removing specific bug details.

### 📋 **Review Tasks**
- [ ] **Review Extracted Content**: Evaluate the extracted patterns and methodologies for completeness and accuracy
- [ ] **Identify Integration Opportunities**: Determine how these best practices can be integrated into ongoing development
- [ ] **Update Related Documentation**: Incorporate relevant patterns into existing implementation guides
- [ ] **Training and Adoption**: Consider how these methodologies can be adopted by the development team

### 🎯 **Success Criteria**
- [ ] **Content Validation**: All extracted patterns are accurate and applicable
- [ ] **Integration Planning**: Clear plan for incorporating best practices into development workflow
- [ ] **Documentation Updates**: Related docs updated to reference new best practices
- [ ] **Team Adoption**: Development team aware of and using new methodologies

---

## 🆕 **NEW: Wikipedia Article Creation**

### Status: ✅ **READY TO IMPLEMENT** - Article content prepared and ready for submission

**Latest Update**: Complete Wikipedia article content has been prepared and is ready for submission to Wikipedia. This will improve Grapa's discoverability and establish official presence on Wikipedia.

### 📋 **Implementation Plan**

**Phase 1: Wikipedia Article Submission** ✅ **READY**
- [x] **Article Content Prepared**: Complete Wikipedia article with proper formatting
- [x] **Instructions Created**: Step-by-step guide for Wikipedia submission
- [x] **Disambiguation Strategy**: Prepared for existing "Grapa (village)" article
- [ ] **Submit Article**: Post article to Wikipedia using prepared content
- [ ] **Monitor Article**: Track article status and any editorial feedback
- [ ] **Respond to Feedback**: Address any Wikipedia editor comments or requests

**Phase 2: Article Maintenance** ⏳ **PENDING**
- [ ] **Regular Updates**: Keep article current with new releases and features
- [ ] **External Link Monitoring**: Ensure all links remain functional
- [ ] **Community Engagement**: Respond to Wikipedia community questions
- [ ] **Citation Management**: Add proper citations as needed

### 🎯 **Success Criteria**
- ✅ **Article Content**: Complete and well-formatted Wikipedia article ready
- [ ] **Article Published**: Successfully submitted and approved on Wikipedia
- [ ] **Discoverability**: Improved search engine visibility for "Grapa programming language"
- [ ] **Community Recognition**: Article becomes reference for Grapa information

### 🔗 **Related Documentation**
- **Article Content**: `wikipedia_grapa_article.txt` - Complete article with instructions
- **Submission Guide**: Step-by-step instructions for Wikipedia posting
- **Disambiguation Strategy**: Handle existing "Grapa (village)" article

### 📊 **Expected Outcomes**
- **Search Visibility**: Wikipedia articles typically rank high in search results
- **Credibility**: Official Wikipedia presence establishes project legitimacy
- **Discoverability**: Users searching "grep" or "programming language" may find Grapa
- **Community Awareness**: Wikipedia editors and readers discover the project

---

## 🚨 **CRITICAL PRIORITY** (Immediate Action Required)

### Database System Issues
*No critical database issues at this time.*

### Production Readiness Issues
- [ ] **Custom Delimiter Edge Cases**: Fix custom delimiter implementation issues that currently cause test failures
  - **Impact**: High - affects any production use with custom delimiters
  - **Need**: Fix the underlying implementation

---

## 🔥 **HIGH PRIORITY** (Next Implementation Cycle)

### Unicode Language Binding
- [ ] **Unicode Language Binding**: Add `case_fold()`, Unicode-aware string methods, Turkish I support.
  - **Status**: C++ implementation works, but Grapa language binding missing
  - **Reference**: [`../RESEARCH_AND_ANALYSIS/UNICODE_CASE_FOLDING_ISSUE.md`](../RESEARCH_AND_ANALYSIS/UNICODE_CASE_FOLDING_ISSUE.md)

### String Interpolation
- [ ] **String Interpolation**: Implement template literal-style interpolation and expression evaluation in strings.

### Cryptographic Features Stabilization (AKS, etc.)
- [ ] **Cryptographic Features Stabilization (AKS, etc.)**: Fix AKS routing logic, improve error handling, document crypto features, update docs and code.

### Optimization Implementation
- [ ] **Optimization Implementation**: Performance improvements for arithmetic, bitwise, comparison, and assignment operators.
  - **Reference**: [`../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md`](../RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md)
  - **Focus**: Mathematical functions, type conversion functions, string operations

### CLI Enhancement (Phase 2)
- [ ] **Performance Options**: Add `-j/--jobs <N>` for parallel worker control and `--no-parallel` to disable parallelism
- [ ] **Environment Management**: Add `-E/--env <VAR=value>` for setting environment variables and `--env-file <file>` for loading environment files
- [ ] **Error Handling**: Add `--strict` mode (fail on warnings), `--continue` (continue on errors), and `--max-errors <N>` (stop after N errors)
- [ ] **Advanced Debugging**: Add `--trace` for execution tracing, `--dump-ast` for showing parsed AST, and `--dump-bytecode` for showing compiled bytecode
- [ ] **Performance Profiling**: Add `--profile` option for performance analysis and optimization insights

### CLI Standardization ✅ **COMPLETED**
- [x] **Replace Non-Standard Options**: Replace `-ccmd`, `-cfile`, `-ccin`, `-argcin` with standard `-c`, `-f`, `-s`, `-S` ✅ **COMPLETED**
- [x] **Remove Conflicting Options**: Remove `-c, --console` and `-e, --env` conflicts ✅ **COMPLETED**
- [x] **Move GUI Options**: Move `-w` and `-wfile` to separate GUI tool ✅ **COMPLETED**
- [x] **Remove Internal Options**: Remove `-argv` and `-` internal use options ✅ **COMPLETED**

### Core Language Features
- [ ] **String Comparison Distance Function**: Consider creating a dedicated function that returns detailed string comparison distance information (like current `<=>` operator behavior) for applications that need fuzzy matching or detailed string analysis
- [ ] **GrapaDB Float Comparison Support**: Add float comparison support to GrapaDB so that float values can be used as index keys. Investigate if current raw binary data comparison is sufficient or if specialized float comparison logic is needed
- [ ] **Line Comments**: Add support for `//` and `#` line comments
  - **Reference**: [`../DEVELOPMENT/LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md`](../DEVELOPMENT/LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md)
- [ ] **Loop Syntax**: Add `for` loop as syntactic sugar for `.range()` patterns
- [ ] **Error Handling**: Improve `.iferr()` ergonomics and exception-based error handling
- [ ] **Recursion Safety Limits**: Investigate and implement safety mechanisms for deep recursion (e.g., factorial(1000) causes bus error). Consider stack depth limits, timeout mechanisms, or graceful degradation for complex recursive operations
  - **Impact**: High - prevents system crashes from deep recursion
  - **Investigation Needed**: Determine best approach (stack limits, timeouts, or graceful failure)
  - **Reference**: Current error handling utilities detect but don't prevent these crashes

---

## 🟡 **MEDIUM PRIORITY**

### Performance & Optimization
- [ ] **Optimize large array operations**
- [ ] **Improve memory management for long-running scripts**
- [ ] **Add caching for frequently accessed data structures**
- [ ] **Performance Benchmarking**: Implement systematic performance comparison with ripgrep and other tools
  - **Focus**: Memory usage comparison, CPU usage patterns, scalability with very large files

### Documentation
- [ ] **Maintain and periodically audit API documentation for completeness and cross-linking** (major sections are now complete; ongoing improvements only)
- [ ] **Continue to add more real-world examples and user recipes to the user guide and examples section**
- [ ] **Create a dedicated performance tuning guide for advanced users**
- [ ] **Ensure onboarding and About pages remain up-to-date as the public docs evolve** (see About page and onboarding map)
- [ ] **Review and improve comment handling in Grapa** (see [`../IMPLEMENTATION/COMMENT_HANDLING_GUIDE.md`](../IMPLEMENTATION/COMMENT_HANDLING_GUIDE.md) for current rules, BNF analysis, and empirical findings)
  - Ensure block comments are handled consistently with the BNF and user expectations.
  - Address any parser bugs or inconsistencies discovered during empirical testing.
- [ ] **Audit and update all code paths for correct field metadata lifecycle handling**. See [`../IMPLEMENTATION/FIELD_METADATA_LIFECYCLE.md`](../IMPLEMENTATION/FIELD_METADATA_LIFECYCLE.md) for details and open items.
- [ ] **Review, validate, and formalize all build and runtime dependencies from BUILD_DEPENDANCIES.md for inclusion in public-facing and onboarding documentation.**
- [ ] **Add a high-level architecture and feature map diagram to the About or Home page** (currently a placeholder exists in about.md)
- [x] **Audit and improve cross-linking between major sections** (API Reference, Language Reference, Use Cases, Examples); add "See also"/"Related topics" to key pages – Complete as of July 2024
- [x] **Create/expand "Advanced Topics" section for advanced features** (meta-programming, custom grammar, etc.) – Complete as of July 2024
- [ ] **Ongoing: Periodically review maintainer/internal docs to ensure no internal details leak into user-facing docs as new content is added**
- [ ] **File Content Handling Documentation**: Create comprehensive documentation and helper functions for handling different file types (text, binary, unknown encoding) in GrapaPy
  - **Focus**: Text files (UTF-8, ASCII, other encodings), binary files, mixed content, encoding detection
  - **Deliverables**: Helper functions for safe text decoding, encoding detection, binary file handling
  - **Impact**: Medium - improves user experience for file operations without making assumptions about file types
- [ ] **GrapaPy Namespace Management Documentation**: Document proper usage of `$global` for data persistence across calls and clarify that `$` prefix is reserved for system use
  - **Focus**: When to use `$global` vs regular variables, system namespace reservation, data persistence patterns
  - **Deliverables**: Clear documentation of namespace rules, examples of proper `$global` usage, warnings about `$` prefix reservation
  - **Impact**: Medium - prevents user confusion and misuse of system namespaces
- [ ] **Error Response Documentation Enhancement**: Document both native Grapa error system and GrapaPy error handling patterns
  - **Focus**: 
    - **Native Grapa**: `$ERR` type (same as `$LIST`), `.iferr()` method, `result.type()==$ERR` checks, absence of try/catch
    - **GrapaPy**: `{"error":-1}` response handling, error detection utilities, safe evaluation patterns
  - **Deliverables**: 
    - Native Grapa error handling documentation with `result.len()`, `result[0]`, `.iferr()` examples
    - GrapaPy error handling utilities documentation and integration examples
    - Clear distinction between native language errors and Python integration errors
  - **Impact**: Medium - improves debugging and error handling for both native Grapa and GrapaPy users
- [ ] **Functional Programming Patterns Documentation**: Explore and document common functional patterns in Grapa including performance monitoring, error handling, and other functional programming techniques
  - **Focus**: Performance monitoring as function parameters, functional error handling patterns, higher-order functions, functional composition
  - **Deliverables**: Documentation of functional patterns, performance monitoring examples, functional programming best practices
  - **Impact**: Medium - helps users leverage Grapa's functional nature for custom solutions

### Language Features
- [ ] **Add support for more mathematical functions**
- [ ] **Implement advanced string manipulation functions**
- [ ] **Add support for regular expressions**
- [ ] **Consider adding support for custom operators**
- [ ] **Module System**: Design basic import/include system
- [ ] **Property Access**: Improve attribute-style access for objects
- [ ] **Boolean Logic**: Add implicit truthy/falsy support

### Database Features
- [ ] **Add support for more database backends**
- [ ] **Implement connection pooling**
- [ ] **Add support for transactions**
- [ ] **Improve query optimization**
- [ ] **Multi-Field Set/Get for Records**: Investigate and implement a way to set and get multiple fields at once for a record (batch .set()/.get()). This will improve performance for records with many fields. Consider supporting $LIST or similar structures for batch operations. Note: RAW field type can store any Grapa data type, including $LIST (enhanced JSON), which may be leveraged for this feature. Not immediate priority; to be reviewed after ROW corruption issue is fixed.
- [ ] **Expose custom index creation and management** (including multi-field indexes) to the Grapa language and CLI. Currently, only the default $KEY index is created automatically; custom indexes can only be created via the C++ API. Add Grapa language/CLI commands for user-defined indexes.

### Test Coverage Expansion
- [ ] **Multiline + Lookaround Combinations**: Expand testing for complex scenarios like lookaround across multiple lines with custom delimiters, nested lookaround assertions in multiline mode, lookaround with Unicode grapheme cluster boundaries, performance with large multiline inputs
- [ ] **Option Matrix Completion**: Test all possible 4+ flag combinations, edge case combinations (e.g., `oxv` - exact + invert), performance impact of complex flag combinations
- [ ] **Performance Regression Testing**: Implement performance benchmarks vs ripgrep, memory usage comparison, CPU usage patterns, scalability with very large files

### Academic Documentation Gaps
- [ ] **Grammar Mutation System Documentation**: Create comprehensive documentation for $RULE structure implementation, runtime grammar compilation, scope management, conflict resolution, performance characteristics
  - **Reference**: [`../ACADEMIC_OUTREACH/IMPLEMENTATION_GAPS_ANALYSIS.md`](../ACADEMIC_OUTREACH/IMPLEMENTATION_GAPS_ANALYSIS.md)
- [ ] **Weighted BTree Performance Analysis**: Document algorithmic complexity, performance benchmarks, memory usage patterns, fragmentation impact, update performance characteristics
- [ ] **Storage Model Comparison**: Create detailed comparison of COL, ROW, GROUP models with performance characteristics, implementation details, query optimization strategies, migration approaches

---

## 🟢 **LOW PRIORITY**

### Development Tools
- [ ] **Create debugging tools**
- [ ] **Add profiling capabilities**
- [ ] **Implement better error reporting**
- [ ] **Create IDE plugins**

### Experimental Features
- [ ] **Consider adding support for async/await patterns**
- [ ] **Investigate adding support for coroutines**
- [ ] **Explore adding support for metaprogramming features**

### Integration
- [ ] **Add support for more external libraries**
- [ ] **Create bindings for popular frameworks**
- [ ] **Implement plugin system**

### Advanced Language Features
- [ ] **Regular Expressions**: Add first-class regex support
- [ ] **Advanced Loops**: Add `foreach` and `range` constructs
- [ ] **IDE Integration**: Improve error messages and syntax highlighting

---

## Priority Order

### 🔥 **CRITICAL PRIORITY**
1. **Database system improvements**: General database functionality enhancements
2. **Custom Delimiter Edge Cases**: Fix custom delimiter implementation issues

### 🚨 **HIGH PRIORITY**
3. **Unicode Language Binding**: Add `case_fold()`, Unicode-aware string methods, Turkish I support
4. **String Interpolation**: Implement template literal-style interpolation and expression evaluation in strings
5. **Cryptographic Features Stabilization**: Fix AKS routing logic, improve error handling, document crypto features
6. **Optimization Implementation**: Performance improvements for arithmetic, bitwise, comparison, and assignment operators
7. **CLI Enhancement (Phase 2)**: Performance options, environment management, error handling, advanced debugging, performance profiling
8. **CLI Standardization**: Replace non-standard options, remove conflicts, move GUI options
9. **Core Language Features**: Line comments, loop syntax, error handling improvements

### 🔶 **MEDIUM PRIORITY**
10. **Performance & Optimization**: Large array operations, memory management, caching, performance benchmarking
11. **Documentation**: API documentation audit, real-world examples, performance tuning guide, comment handling review
12. **Language Features**: Mathematical functions, string manipulation, regular expressions, custom operators, module system
13. **Database Features**: Additional backends, connection pooling, transactions, query optimization, multi-field operations
14. **Test Coverage Expansion**: Multiline + lookaround combinations, option matrix completion, performance regression testing
15. **Academic Documentation Gaps**: Grammar mutation system, weighted BTree performance, storage model comparison

### 🔷 **LOW PRIORITY**
16. **Development Tools**: Debugging tools, profiling capabilities, error reporting, IDE plugins
17. **Experimental Features**: Async/await patterns, coroutines, metaprogramming features
18. **Integration**: External libraries, framework bindings, plugin system
19. **Advanced Language Features**: First-class regex support, advanced loops, IDE integration

---

## ✅ Completed Items

### Recent Achievements
- [x] **Missing Operator Documentation**: Completed comprehensive documentation for all 5 missing operators (`++`, `--`, `.*`, `*/`, `<=>`) with examples, error cases, and type support matrices
- [x] **Static Comparison Helper Function**: Implemented `DoComparison` static helper function that unifies all comparison operators (`==`, `!=`, `>`, `<`, `>=`, `<=`, `<=>`) using a spaceship operator pattern
- [x] **Comparison Operator Refactoring**: Refactored all comparison event classes to use the static helper function, reducing code duplication and improving maintainability
- [x] **Float Comparison Fixes**: Fixed float comparison bugs including precision normalization and mixed type comparisons
- [x] **String vs Number Comparison**: Improved handling of string vs number comparisons with proper numeric conversion attempts
- [x] **CLI Interface Improvement**: Completed best-in-class CLI interface matching Python standards and user expectations
- [Completed] Database system improvements: Various database functionality enhancements completed inserts.

### Previous Releases
- [x] Basic language implementation
- [x] Database integration
- [x] File system operations
- [x] Network operations
- [x] Mathematical functions
- [x] String manipulation functions

---

## 📋 Future/Long-Term Tasks
- User-defined classes/objects: Improve documentation and idiomatic usage
- CLI Enhancement Phase 2: Performance options, environment management, error handling
- Continue documentation structure improvements: See [DOCS_STRUCTURE_IMPROVEMENT_PLAN.md](DEVELOPMENT/DOCS_STRUCTURE_IMPROVEMENT_PLAN.md)
- Evaluate all instances of null, true, and false handling in Grapa (especially for .get(), .set(), and field assignment) to ensure consistency across all types and operations. Reference the recent RAW/null test as an example.
- Automate running the full regression suite (run_tests_comprehensive.grc and Python integration tests) as part of CI to ensure all features and integrations are validated on every change.

---

# 📄 Backlog Summary

- This backlog is the source for all queued, future, and long-term tasks.
- For active and in-progress work, see [`CURRENT_STATUS.md`](CURRENT_STATUS.md).
- Completed items are retained for historical context and release notes. 