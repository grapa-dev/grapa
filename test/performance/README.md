# Performance Threshold Tests

This directory contains comprehensive tests to determine the optimal threshold between using `{}` (linked list) and `{}.table()` (BTree-based database) for different dataset sizes.

## Test Files

### 1. `linked_list_vs_btree_performance.grc`
**Main performance comparison test**
- Tests datasets of sizes: 100, 500, 1000, 2000, 5000, 10000, 20000 items
- Measures search and insert performance for both implementations
- Calculates performance ratios and identifies crossover points
- Provides detailed recommendations based on results

### 2. `memory_usage_test.grc`
**Memory usage and creation time analysis**
- Tests memory consumption patterns for both implementations
- Measures creation time overhead
- Analyzes access pattern performance
- Helps identify memory-related thresholds

### 3. `search_intensity_test.grc`
**Search frequency impact analysis**
- Tests different search intensities (100, 500, 1000, 2000, 5000 searches)
- Determines how search frequency affects the optimal choice
- Identifies thresholds based on usage patterns
- Provides recommendations for different search scenarios

### 4. `run_all_performance_tests.grc`
**Master test runner**
- Executes all performance tests in sequence
- Generates comprehensive final report
- Provides consolidated recommendations
- Creates a complete performance analysis

## Running the Tests

### Run Individual Tests
```bash
./grapa test/performance/linked_list_vs_btree_performance.grc
./grapa test/performance/memory_usage_test.grc
./grapa test/performance/search_intensity_test.grc
```

### Run All Tests
```bash
./grapa test/performance/run_all_performance_tests.grc
```

## Expected Results

Based on the analysis, the performance characteristics are:

- **`{}` (Linked List) and `[]` (Array)**: Very fast for most use cases due to double-linked list implementation
- **`{}.table()` (BTree) and `$file`**: Use when persistence, order, range queries, or memory efficiency at scale are important

**Key Insight**: Don't add indexing to `$LIST` or `$ARRAY` - the indexing overhead would likely make them slower, not faster.

## Test Methodology

1. **Data Generation**: Creates test datasets with realistic key-value pairs
2. **Performance Measurement**: Uses `$time()` for accurate timing
3. **Multiple Iterations**: Tests various dataset sizes and operation counts
4. **Statistical Analysis**: Calculates ratios and identifies trends
5. **Comprehensive Reporting**: Provides detailed results and recommendations

## Key Metrics Measured

- **Search Performance**: Time to find items by key
- **Insert Performance**: Time to add new items
- **Creation Time**: Time to build the initial data structure
- **Memory Usage**: Memory consumption patterns
- **Access Patterns**: Performance of different access scenarios

## Interpretation Guidelines

- **Ratio < 1.0**: BTree is faster
- **Ratio > 1.0**: Linked list is faster
- **Crossover Point**: Where BTree becomes more efficient
- **Search Intensity**: How search frequency affects the threshold

## Use Case Recommendations

- **Configuration Data**: Use `{}` (linked list) - fast and simple
- **User Data**: Use `{}` (linked list) unless you need persistence → then use `{}.table()`
- **Cache Data**: Use `{}` (linked list) - frequent access, moderate size
- **Log Data**: Use `{}.table()` or `$file` - large datasets, persistence needed
- **Range Queries**: Use `{}.table()` - BTree supports efficient range operations
- **Ordered Data**: Use `{}.table()` - maintains sort order efficiently
- **Memory Efficiency at Scale**: Use `{}.table()` - better memory usage for large datasets

## Notes

- Tests are designed to be realistic and representative of actual usage
- Results may vary based on system performance and memory availability
- Consider running tests multiple times for statistical significance
- Adjust test parameters based on your specific use case requirements
