# Test Organization Summary

## Overview
This document tracks test file organization and cleanup tasks for the Grapa project.

## Grep-Related Test Files to Move to test/grep/

The following files in the test root are grep-related and should be moved to `test/grep/`:

- test_error_handling.grc
- test_invalid_regex.grc
- test_normalization_o.grc
- test_diacritic_insensitive_o.grc
- test_zero_length_o.grc
- test_o_option_comprehensive.grc
- test_missing_ripgrep_features.grc
- test_ripgrep_parity_features.grc
- test_advanced_context.grc
- test_parallel_grep_verification.grc
- test_search_strategy_features.grc
- test_performance_optimizations.grc
- test_lookaround_assertions.grc
- test_atomic_groups.grc
- test_grep_advanced.py
- test_grep_python_doc_examples.py

## Action Items
- [ ] Move all grep-related test files to `test/grep/` directory
- [ ] Update any references to these files in other documentation
- [ ] Verify all tests still pass after reorganization
- [ ] Update CI/CD scripts if they reference specific test file locations

## Notes
- This reorganization will improve test discoverability and organization
- All grep functionality tests should be co-located for easier maintenance
- Consider creating subdirectories within `test/grep/` if the number of files grows significantly 