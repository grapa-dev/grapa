# Indexing Removal Project Test Suite

This directory contains organized test scripts for the indexing removal project, which aims to resolve the critical ROW table index corruption bug by removing the broken indexing system.

## Project Overview

The indexing removal project involves:
- Moving the dictionary pointer from the first index record to reserved fields in the B-tree node
- Removing higher-level indexing functionality in GrapaDB and GrapaGroup
- Preserving function signatures while changing implementations to no-ops or table scans
- Updating debug functions and documentation

**CRITICAL ARCHITECTURE NOTE:**
- **Dictionary IS a tree** - `GetDataTypeRecord()` returns a tree reference
- **Dictionary scanning is O(n)** - `ListFields()` and `FindField()` scan through dictionary tree linearly
- **Field name lookups use linear search** - No indexing on field names within dictionary
- **Dictionary pointer migration** changes storage location but not access pattern

## Test Organization

Tests are organized by implementation phase to ensure systematic validation:

### Phase 0: Pre-Implementation Safety
- **Location**: `phase_0_pre_implementation/`
- **Purpose**: Safety and preparation tests
- **Tests Include**:
  - Index audit validation
  - Performance benchmarks
  - Memory leak detection
  - Backup and rollback verification
  - Logging framework validation
  - Feature flag testing

### Phase 1: Dictionary Pointer Migration
- **Location**: `phase_1_dictionary_migration/`
- **Purpose**: Validate dictionary pointer relocation
- **Tests Include**:
  - Dictionary pointer read/write operations
  - Big-endian conversion validation
  - Table creation/deletion with new dictionary location
  - Dictionary access pattern changes
  - Fallback mechanism testing

### Phase 2: Index System Removal
- **Location**: `phase_2_index_removal/`
- **Purpose**: Validate index function modifications
- **Tests Include**:
  - Index creation methods (no-op validation)
  - Index operation methods (table scan fallback)
  - Search operations (table scan only)
  - Enum renaming validation
  - Cursor index flag behavior

### Phase 3: GrapaGroup Index Cleanup
- **Location**: `phase_3_grapagroup_cleanup/`
- **Purpose**: Validate GrapaGroup index-related changes
- **Tests Include**:
  - CreateIndex call behavior (relying on no-op implementations)
  - SearchDb call behavior (relying on table scan implementations)
  - Constant renaming validation

### Phase 4: Debug Functions Update
- **Location**: `phase_4_debug_functions/`
- **Purpose**: Validate debug function modifications
- **Tests Include**:
  - Dump function behavior with new dictionary location
  - Dictionary access in debug functions
  - New debug function validation
  - Removed debug function verification

### Phase 5: Documentation Update
- **Location**: `phase_5_documentation/`
- **Purpose**: Validate documentation accuracy
- **Tests Include**:
  - Implementation documentation validation
  - User documentation accuracy
  - API documentation consistency

## Test Execution

Each phase folder contains `.grc` test scripts that can be executed using:

```bash
# Run all tests for a specific phase
grapa -f test/indexing_removal_project/phase_X_*/test_*.grc

# Run a specific test
grapa -f test/indexing_removal_project/phase_1_dictionary_migration/test_dictionary_pointer.grc

# Run all indexing removal tests
grapa -f test/indexing_removal_project/**/test_*.grc
```

## Test Creation Strategy

Tests are created upfront for each phase to:
1. **Define Success Criteria**: Clear validation of expected behavior
2. **Enable Regression Detection**: Identify when changes break existing functionality
3. **Support Rollback**: Verify that rollback procedures work correctly
4. **Document Requirements**: Serve as living documentation of expected behavior

## Related Documentation

- **Analysis Document**: `maintainers/RESEARCH_AND_ANALYSIS/INDEXING_REMOVAL_ANALYSIS.md`
- **Project Status**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **Bug Demo**: `test/test_row_bug_demo.grc`

## Notes

- All tests should be idempotent (safe to run multiple times)
- Tests should include both positive and negative test cases
- Performance tests should establish baselines before changes
- Debug tests should validate both old and new behavior during transition 