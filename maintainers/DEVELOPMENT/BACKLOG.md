# BACKLOG

This backlog tracks all future, long-term, and queued tasks for the Grapa project. For active and in-progress work, see [`DEVELOPMENT/CURRENT_STATUS.md`](DEVELOPMENT/CURRENT_STATUS.md). Items are grouped by priority and area. Completed items are listed at the end for reference.

---

## 📋 Future/Long-Term Tasks
- User-defined classes/objects: Improve documentation and idiomatic usage
- CLI Enhancement Phase 2: Performance options, environment management, error handling
- Continue documentation structure improvements: See [DOCS_STRUCTURE_IMPROVEMENT_PLAN.md](DEVELOPMENT/DOCS_STRUCTURE_IMPROVEMENT_PLAN.md)

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

## ✅ Completed Items

### Recent Achievements
- [x] **Missing Operator Documentation**: Completed comprehensive documentation for all 5 missing operators (`++`, `--`, `.*`, `*/`, `<=>`) with examples, error cases, and type support matrices
- [x] **Static Comparison Helper Function**: Implemented `DoComparison` static helper function that unifies all comparison operators (`==`, `!=`, `>`, `<`, `>=`, `<=`, `<=>`) using a spaceship operator pattern
- [x] **Comparison Operator Refactoring**: Refactored all comparison event classes to use the static helper function, reducing code duplication and improving maintainability
- [x] **Float Comparison Fixes**: Fixed float comparison bugs including precision normalization and mixed type comparisons
- [x] **String vs Number Comparison**: Improved handling of string vs number comparisons with proper numeric conversion attempts

### Previous Releases
- [x] Basic language implementation
- [x] Database integration
- [x] File system operations
- [x] Network operations
- [x] Mathematical functions
- [x] String manipulation functions 

---

# 📄 Backlog Summary

- This backlog is the source for all queued, future, and long-term tasks.
- For active and in-progress work, see [`DEVELOPMENT/CURRENT_STATUS.md`](DEVELOPMENT/CURRENT_STATUS.md).
- Completed items are retained for historical context and release notes. 