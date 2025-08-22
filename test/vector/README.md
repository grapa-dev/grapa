# Vector Performance Tests

This directory contains comprehensive performance tests and benchmarks for Grapa's vector operations.

## Test Files

### `vector_performance_benchmarks.grc`
Comprehensive benchmark suite that tests:
- Matrix multiplication performance across different sizes (2x2 to 100x100)
- Statistical function performance (covariance, eigenvalues, determinant)
- Memory usage analysis for large operations
- Performance comparison between different operation types
- Large dataset stress testing (200x200 matrices)
- Optimization recommendations based on results

### `vector_benchmark_simple.grc`
Simplified benchmark focused on key operations:
- Matrix multiplication timing
- Statistical function timing
- Basic operation comparison
- Memory usage estimation

## Performance Characteristics

Based on benchmark results, Grapa's vector operations show the following characteristics:

### Matrix Multiplication
- **Small matrices (< 10x10)**: Sub-millisecond performance
- **Medium matrices (10x10 - 50x50)**: Millisecond to second range
- **Large matrices (> 50x50)**: Exponential growth due to O(n³) complexity
- **200x200 matrices**: ~27 seconds for multiplication

### Statistical Functions
- **Covariance**: Relatively fast, scales well with matrix size
- **Determinant**: Moderate performance, reasonable for moderate sizes
- **Eigenvalues**: More computationally intensive, best for symmetric matrices
- **Rank calculation**: Fast operation

### Memory Usage
- **Estimated memory**: ~8 bytes per element for FLOAT precision
- **10x10 matrix**: ~0.8KB
- **50x50 matrix**: ~20KB
- **100x100 matrix**: ~80KB
- **200x200 matrix**: ~320KB

## Optimization Recommendations

### For Real-time Applications
- Use matrices smaller than 50x50
- Pre-allocate matrices when possible
- Consider breaking large matrices into smaller blocks

### For Large Datasets
- Monitor memory usage for matrices > 100x100
- Use memory-efficient operations
- Consider parallel processing for large datasets

### General Best Practices
- Use `.vector()` for mathematical operations
- Use `.array()` for general data manipulation
- Profile memory usage for long-running operations
- Use appropriate data types (INT vs FLOAT)

## Running the Tests

```bash
# Run comprehensive benchmarks
./grapa test/vector/vector_performance_benchmarks.grc

# Run simple benchmarks
./grapa test/vector/vector_benchmark_simple.grc
```

## Expected Results

The benchmarks will provide timing data for various operations and help identify:
- Performance bottlenecks
- Memory usage patterns
- Optimal matrix sizes for different use cases
- Areas for potential optimization

## Notes

- All timing measurements use microsecond precision
- Memory estimates are approximate based on 8 bytes per element
- Performance may vary based on system specifications
- Large matrix operations (> 100x100) may take significant time
