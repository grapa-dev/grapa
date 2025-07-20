# Grapa Backlog

## High Priority

### 🚨 CRITICAL: ROW Table Index Corruption Bug
- [ ] **ROW Table Index Corruption Fix**: Fix critical bug where first record becomes unreadable after adding third record to ROW tables
  - **Issue**: `$file().table("ROW")` first record returns `{"error":-1}` after third record added
  - **Impact**: HIGH - ROW tables are unusable for production with more than 2 records
  - **Workaround**: Use COL tables instead of ROW tables (documented in troubleshooting)
  - **Test Script**: `test_row_bug_demo.grc` demonstrates the bug
  - **Debug Output**: Shows empty RPTR entries for first record after corruption
  - **Priority**: IMMEDIATE - blocking database functionality

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
- [ ] Complete API documentation
- [ ] Add more examples to user guide
- [ ] Create performance tuning guide

## Medium Priority

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

## Low Priority

### Experimental Features
- [ ] Consider adding support for async/await patterns
- [ ] Investigate adding support for coroutines
- [ ] Explore adding support for metaprogramming features

### Integration
- [ ] Add support for more external libraries
- [ ] Create bindings for popular frameworks
- [ ] Implement plugin system

## Completed Items

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