# BACKLOG

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`DEVELOPMENT/CURRENT_STATUS.md`](DEVELOPMENT/CURRENT_STATUS.md). Items are grouped by priority and area. Completed items are listed at the end for reference.

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

## 🚨 Immediate/High Priority

### Unicode Language Binding
- [ ] **Unicode Language Binding**: Add `case_fold()`, Unicode-aware string methods, Turkish I support.

### String Interpolation
- [ ] **String Interpolation**: Implement template literal-style interpolation and expression evaluation in strings.

### Cryptographic Features Stabilization (AKS, etc.)
- [ ] **Cryptographic Features Stabilization (AKS, etc.)**: Fix AKS routing logic, improve error handling, document crypto features, update docs and code.

### Optimization Implementation
- [ ] **Optimization Implementation**: Performance improvements for arithmetic, bitwise, comparison, and assignment operators.

### CLI Enhancement (Phase 2)
- [ ] **Performance Options**: Add `-j/--jobs <N>` for parallel worker control and `--no-parallel` to disable parallelism
- [ ] **Environment Management**: Add `-E/--env <VAR=value>` for setting environment variables and `--env-file <file>` for loading environment files
- [ ] **Error Handling**: Add `--strict` mode (fail on warnings), `--continue` (continue on errors), and `--max-errors <N>` (stop after N errors)
- [ ] **Advanced Debugging**: Add `--trace` for execution tracing, `--dump-ast` for showing parsed AST, and `--dump-bytecode` for showing compiled bytecode
- [ ] **Performance Profiling**: Add `--profile` option for performance analysis and optimization insights

### Core Language Features
- [ ] **String Comparison Distance Function**: Consider creating a dedicated function that returns detailed string comparison distance information (like current `<=>` operator behavior) for applications that need fuzzy matching or detailed string analysis
- [ ] **GrapaDB Float Comparison Support**: Add float comparison support to GrapaDB so that float values can be used as index keys. Investigate if current raw binary data comparison is sufficient or if specialized float comparison logic is needed
- [ ] Grapa Language Programmer Friendliness & Adoption Plan: See [PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md](PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md) for a comprehensive audit, migration review, pain points, and proposals to improve comment handling, loop syntax, string interpolation, onboarding, and overall language adoption.

### Performance & Optimization
- [ ] Optimize large array operations
- [ ] Improve memory management for long-running scripts
- [ ] Add caching for frequently accessed data structures

### Documentation
- [ ] Maintain and periodically audit API documentation for completeness and cross-linking (major sections are now complete; ongoing improvements only)
- [ ] Continue to add more real-world examples and user recipes to the user guide and examples section
- [ ] Create a dedicated performance tuning guide for advanced users
- [ ] Ensure onboarding and About pages remain up-to-date as the public docs evolve (see About page and onboarding map)
- [ ] Review and improve comment handling in Grapa (see DEVELOPMENT/COMMENT_HANDLING_GUIDE.md for current rules, BNF analysis, and empirical findings)
  - Ensure block comments are handled consistently with the BNF and user expectations.
  - Address any parser bugs or inconsistencies discovered during empirical testing.
- Audit and update all code paths for correct field metadata lifecycle handling. See [IMPLEMENTATION/FIELD_METADATA_LIFECYCLE.md] for details and open items.
- [ ] **Review, validate, and formalize all build and runtime dependencies from BUILD_DEPENDANCIES.md for inclusion in public-facing and onboarding documentation.**
- [ ] Add a high-level architecture and feature map diagram to the About or Home page (currently a placeholder exists in about.md)
- [x] Audit and improve cross-linking between major sections (API Reference, Language Reference, Use Cases, Examples); add "See also"/"Related topics" to key pages – Complete as of July 2024
- [x] Create/expand "Advanced Topics" section for advanced features (meta-programming, custom grammar, etc.) – Complete as of July 2024
- [ ] Ongoing: Periodically review maintainer/internal docs to ensure no internal details leak into user-facing docs as new content is added

---

## 🟡 Medium Priority

### Language Features
- [ ] Add support for more mathematical functions
- [ ] Implement advanced string manipulation functions
- [ ] Add support for regular expressions
- [ ] Consider adding support for custom operators

### Database Features
- [ ] Add support for more database backends
- [ ] Implement connection pooling
- [ ] Add support for transactions
- [ ] Improve query optimization
- [ ] Multi-Field Set/Get for Records: Investigate and implement a way to set and get multiple fields at once for a record (batch .set()/.get()). This will improve performance for records with many fields. Consider supporting $LIST or similar structures for batch operations. Note: RAW field type can store any Grapa data type, including $LIST (enhanced JSON), which may be leveraged for this feature. Not immediate priority; to be reviewed after ROW corruption issue is fixed.
- [ ] Expose custom index creation and management (including multi-field indexes) to the Grapa language and CLI. Currently, only the default $KEY index is created automatically; custom indexes can only be created via the C++ API. Add Grapa language/CLI commands for user-defined indexes.

### Development Tools
- [ ] Create debugging tools
- [ ] Add profiling capabilities
- [ ] Implement better error reporting
- [ ] Create IDE plugins

---

## 🟢 Low Priority

### Experimental Features
- [ ] Consider adding support for async/await patterns
- [ ] Investigate adding support for coroutines
- [ ] Explore adding support for metaprogramming features

### Integration
- [ ] Add support for more external libraries
- [ ] Create bindings for popular frameworks
- [ ] Implement plugin system

---

## Priority Order

### 🔥 **HIGH PRIORITY**

1. **Unicode Language Binding**: Add `case_fold()`, Unicode-aware string methods, Turkish I support
2. **String Interpolation**: Implement template literal-style interpolation and expression evaluation in strings
3. **Cryptographic Features Stabilization**: Fix AKS routing logic, improve error handling, document crypto features
4. **Optimization Implementation**: Performance improvements for arithmetic, bitwise, comparison, and assignment operators
5. **CLI Enhancement (Phase 2)**: Performance options, environment management, error handling, advanced debugging, performance profiling

### 🔶 **MEDIUM PRIORITY**

6. **Core Language Features**: String comparison distance function, GrapaDB float comparison support, programmer friendliness plan
7. **Performance & Optimization**: Large array operations, memory management, caching
8. **Documentation**: API documentation audit, real-world examples, performance tuning guide, comment handling review
9. **Language Features**: Mathematical functions, string manipulation, regular expressions, custom operators
10. **Database Features**: Additional backends, connection pooling, transactions, query optimization, multi-field operations

### 🔷 **LOW PRIORITY**

11. **Development Tools**: Debugging tools, profiling capabilities, error reporting, IDE plugins
12. **Experimental Features**: Async/await patterns, coroutines, metaprogramming features
13. **Integration**: External libraries, framework bindings, plugin system

---

## ✅ Completed Items

### Recent Achievements
- [x] **Missing Operator Documentation**: Completed comprehensive documentation for all 5 missing operators (`++`, `--`, `.*`, `*/`, `<=>`) with examples, error cases, and type support matrices
- [x] **Static Comparison Helper Function**: Implemented `DoComparison` static helper function that unifies all comparison operators (`==`, `!=`, `>`, `<`, `>=`, `<=`, `<=>`) using a spaceship operator pattern
- [x] **Comparison Operator Refactoring**: Refactored all comparison event classes to use the static helper function, reducing code duplication and improving maintainability
- [x] **Float Comparison Fixes**: Fixed float comparison bugs including precision normalization and mixed type comparisons
- [x] **String vs Number Comparison**: Improved handling of string vs number comparisons with proper numeric conversion attempts
- [Completed] ROW table index bug: Root cause was index entry value not set to record reference (recCursor.mValue). Fixed in SetRecordField for ROW, COL, and GROUP tables. Tests confirm no corruption after multiple inserts.

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
- For active and in-progress work, see [`DEVELOPMENT/CURRENT_STATUS.md`](DEVELOPMENT/CURRENT_STATUS.md).
- Completed items are retained for historical context and release notes. 