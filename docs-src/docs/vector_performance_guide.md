# Vector Performance Optimization Guide

## Overview

This guide provides comprehensive performance optimization recommendations for Grapa's vector operations, based on systematic benchmarking and analysis. It covers performance characteristics, optimization strategies, and best practices for different use cases.

## Performance Characteristics

### Algorithm Complexities

| Operation | Complexity | Performance Impact |
|-----------|------------|-------------------|
| Matrix multiplication | O(n³) | High for large matrices |
| Determinant calculation | O(n³) | High for large matrices |
| Eigenvalue calculation | O(n³) with iterations | Very high for large matrices |
| Covariance calculation | O(n²m) | Moderate for large matrices |
| Basic operations (sum, mean) | O(n²) | Low for all sizes |
| Transpose | O(n²) | Very low for all sizes |

### Performance Benchmarks

| Matrix Size | Creation (ms) | Multiplication (ms) | Determinant (ms) | Memory (KB) |
|-------------|---------------|---------------------|------------------|-------------|
| 10x10       | <1           | <1                 | <1               | 0.8         |
| 50x50       | <1           | <1                 | <1               | 20          |
| 100x100     | <1           | <1                 | <1               | 80          |
| 200x200     | <1           | ~200               | <1               | 320         |
| 500x500     | <1           | ~1,200             | <1               | 2,000       |
| 10,000x1    | <1           | ~530               | N/A              | 80          |

**Note**: Performance scales linearly with data size (~0.053ms per sample for linear regression operations).

## Optimization Strategies

### 1. Matrix Size Optimization

#### Real-time Applications
```grapa
/* For real-time applications, keep matrices small */
// ✅ Good - Fast response
small_matrix = [[1, 2], [3, 4]].vector();
result = small_matrix.dot(small_matrix);  // < 1ms

// ⚠️ Moderate - Good performance for most use cases
large_matrix = create_large_matrix(100);  // ~200ms for 100x100
result = large_matrix.dot(large_matrix);
```

**Recommendations:**
- Keep matrices < 50x50 for sub-second response
- Use < 200x200 for interactive applications (good performance)
- Use < 500x500 for batch processing (reasonable performance)
- Consider breaking very large problems (> 1000x1000) into smaller blocks

#### Batch Processing
```grapa
/* For batch processing, monitor memory usage */
// ✅ Good - Manageable memory
medium_matrix = create_matrix(200);  // 320KB memory
result = medium_matrix.cov();        // Good performance

// ⚠️ Monitor - Large memory usage
large_matrix = create_matrix(500);   // 2MB memory
result = large_matrix.cov();         // Monitor performance
```

### 2. Algorithm Selection

#### Matrix Multiplication
```grapa
/* Use appropriate algorithms for your use case */:
// For small matrices - standard multiplication is fine
small_result = mat_a.dot(mat_b);  // Fast for < 100x100

// For large matrices - consider alternatives
if (mat_a.shape().getfield(0) > 100) {
    // Consider breaking into smaller blocks
    result = block_multiply(mat_a, mat_b);
} else {
    result = mat_a.dot(mat_b);
}
```

#### Statistical Functions
```grapa
/* Choose efficient statistical operations */
// ✅ Fast - Good for all sizes
sum_result = data.sum();
mean_result = data.mean();

// ⚠️ Moderate - Good for < 200x200
cov_result = data.cov();

// ❌ Slow - Use only for < 50x50
eigen_result = data.eigh();
```

### 3. Memory Management

#### Pre-allocation
```grapa
/* Pre-allocate matrices when possible */
// ✅ Good - Reuse allocated memory
matrix = create_matrix(50);
for (i = 0; i < 1000; i++) {
    result = matrix.dot(matrix);  // Reuse same matrix
}

// ❌ Poor - Repeated allocation
for (i = 0; i < 1000; i++) {
    matrix = create_matrix(50);   // Allocate each time
    result = matrix.dot(matrix);
}
```

#### Memory Monitoring
```grapa
/* Monitor memory usage for large operations */
large_matrix = create_matrix(200);
estimated_memory = large_matrix.shape().reduce(op(acc, dim){acc * dim}, 1) * 8 / 1024;
("Estimated memory usage: " + estimated_memory + " KB").echo();
```

### 4. Data Type Optimization

#### Choose Appropriate Types
```grapa
/* Use INT for integer data, FLOAT for decimal */
// ✅ Good - Use INT for integer data
integer_data = [1, 2, 3, 4, 5].vector();

// ✅ Good - Use FLOAT for decimal data
decimal_data = [1.5, 2.7, 3.2, 4.1, 5.9].vector();

// ⚠️ Consider - Precision vs performance trade-off
high_precision = [1.123456789, 2.987654321].vector();
```

### 5. Precision Optimization

#### System-Level Precision Control
For performance-critical applications, you can significantly improve vector operation speed by reducing floating-point precision:

```grapa
/* Set system precision for performance optimization */
// ✅ High performance - 32-bit precision (~2x faster)
32.setfloat(0);
result = large_matrix.dot(large_matrix);  // Much faster

// ✅ Balanced - 64-bit precision (good speed/accuracy)
64.setfloat(0);
result = large_matrix.dot(large_matrix);  // Good performance

// ✅ High accuracy - 128-bit precision (default)
128.setfloat(0);
result = large_matrix.dot(large_matrix);  // Maximum accuracy
```

**Performance Impact:**
- **32-bit precision**: ~2x faster than 128-bit precision
- **64-bit precision**: ~1.1x faster than 128-bit precision
- **128-bit precision**: Maximum accuracy (default)

#### Fixed-Point vs Floating-Point Accuracy
Grapa automatically switches between floating-point and fixed-point representations depending on the mathematical function, with internal optimizations in `GrapaFloat.cpp` that choose the best representation for each operation. You can set a system preference:

```grapa
/* Set system preference for floating-point representation */
32.setfloat(0);  // Sets system preference, but Grapa optimizes internally
result = matrix.dot(matrix);  // Grapa chooses optimal representation per operation

/* Set system preference for fixed-point representation */
32.setfix(0);    // Sets system preference, but Grapa optimizes internally
result = matrix.dot(matrix);  // Grapa chooses optimal representation per operation
```

**System Behavior:**
- **Automatic optimization**: Grapa's internal functions automatically choose the best representation for each mathematical operation
- **System preference**: `setfloat()` and `setfix()` set a system preference, but Grapa overrides this when it knows better
- **Same bit precision**: Performance is similar, accuracy is optimized per operation
- **Default recommendation**: Use `setfloat()` as the default unless you have specific requirements:

**When the Choice Matters:**
- **Financial calculations**: May benefit from `setfix()` preference for decimal precision
- **Scientific calculations**: May benefit from `setfloat()` preference for large dynamic ranges
- **Most applications**: The system default choice is sufficient due to internal optimizations:

#### Precision Performance Example
```grapa
/* Linear regression with different precision settings */
n_samples = 10000;

// 32-bit precision - Fast (Grapa optimizes representation internally)
32.setfloat(0);
start_time = $TIME().utc();
result_32bit = perform_linear_regression(n_samples);
time_32bit = start_time.ms();  // ~277ms

// 128-bit precision - Maximum accuracy (Grapa optimizes representation internally)
128.setfloat(0);
start_time = $TIME().utc();
result_128bit = perform_linear_regression(n_samples);
time_128bit = start_time.ms();  // ~529ms

// 32-bit is ~1.9x faster with minimal accuracy loss
```

## Use Case Optimization

### Real-time Applications

**Requirements:** Sub-second response time
**Recommended Matrix Size:** < 50x50

```grapa
/* Real-time optimization strategies */
// 1. Use small matrices
small_matrix = [[1, 2], [3, 4]].vector();

// 2. Pre-compute when possible
precomputed_result = expensive_operation(small_matrix);

// 3. Use fast operations
fast_result = small_matrix.sum();  // O(n²) - very fast

// 4. Avoid expensive operations
// ❌ Avoid in real-time
eigen_result = small_matrix.eigh();  // O(n³) with iterations
```

### Interactive Applications

**Requirements:** < 5 second response time
**Recommended Matrix Size:** < 200x200

```grapa
/* Interactive optimization strategies */
// 1. Use moderate matrix sizes
medium_matrix = create_matrix(50);

// 2. Provide progress feedback
("Computing...").echo();
result = medium_matrix.dot(medium_matrix);
("Complete!").echo();

// 3. Use appropriate operations
cov_result = medium_matrix.cov();  // Good performance
```

### Batch Processing

**Requirements:** Efficient processing of large datasets
**Recommended Matrix Size:** < 1000x1000

```grapa
/* Batch processing optimization strategies */
// 1. Monitor memory usage
large_matrix = create_matrix(200);
memory_usage = estimate_memory(large_matrix);

// 2. Use memory-efficient operations
sum_result = large_matrix.sum();  // Memory efficient

// 3. Consider breaking large problems
if (large_matrix.shape().getfield(0) > 200) {
    result = process_in_blocks(large_matrix);
} else {
    result = process_directly(large_matrix);
}
```

### Data Science Applications

**Requirements:** Accurate results with reasonable performance
**Recommended Matrix Size:** < 500x500

```grapa
/* Data science optimization strategies */
// 1. Use appropriate statistical functions
data = load_dataset();
cov_matrix = data.cov();  // Good for data analysis

// 2. Consider data characteristics
if (is_sparse(data)) {
    result = sparse_operations(data);
} else {
    result = dense_operations(data);
}

// 3. Use efficient algorithms
// For correlation analysis
correlation = data.cov();  // More efficient than manual calculation
```

## Edge Case Performance

### Empty and Small Matrices
```grapa
/* Edge cases perform excellently */
empty_vec = [].vector();
empty_sum = empty_vec.sum();  // Returns {"error":-1} for empty vectors

small_mat = [[1]].vector();
small_det = small_mat.det();  // 0ms - very fast
```

### Special Matrix Types
```grapa
/* Special matrices are optimized */
identity = [[1, 0], [0, 1]].vector();
id_det = identity.det();  // 0ms - very fast

sparse = [[1, 0, 0], [0, 1, 0], [0, 0, 1]].vector();
sparse_det = sparse.det();  // 0ms - very fast
```

### Extreme Values
```grapa
/* Extreme values handled efficiently */
large_nums = [[1e15, 2e15], [3e15, 4e15]].vector();
large_det = large_nums.det();  // Handled correctly

small_nums = [[1e-15, 2e-15], [3e-15, 4e-15]].vector();
small_det = small_nums.det();  // Handled correctly
```

## Performance Monitoring

### Timing Operations
```grapa
/* Monitor operation performance */
start_time = $TIME().utc();
result = matrix.dot(matrix);
end_time = $TIME().utc();
operation_time = (($TIME().utc() - start_time) / 1000000).int();
("Operation took: " + operation_time + "ms").echo();
```

### Memory Estimation
```grapa
/* Estimate memory usage */
estimate_memory = op(matrix) {
    elements = matrix.shape().reduce(op(acc, dim){acc * dim}, 1);
    bytes = elements * 8;  // 8 bytes per element
    kb = bytes / 1024;
    kb;
};

memory_usage = estimate_memory(my_matrix);
("Estimated memory: " + memory_usage + " KB").echo();
```

## Best Practices Summary

### Do's
- ✅ Use matrices < 50x50 for real-time applications
- ✅ Use matrices < 200x200 for interactive applications (good performance)
- ✅ Use matrices < 1000x1000 for batch processing
- ✅ Pre-allocate matrices when possible
- ✅ Use appropriate data types (INT vs FLOAT)
- ✅ Monitor memory usage for large matrices
- ✅ Use fast operations (sum, mean) for large datasets
- ✅ Consider breaking very large problems (> 1000x1000) into smaller blocks
- ✅ Use sequential loops for large datasets with simple operations
- ✅ Use parallel `.map()/.filter()` for smaller datasets with complex operations
- ✅ Prefer `.reduce()` for large datasets when possible (more efficient)
- ✅ Use `32.setfloat(0)` for machine learning applications requiring maximum speed
- ✅ Use `64.setfloat(0)` for balanced speed/accuracy in most applications
- ✅ Use `128.setfloat(0)` for applications requiring maximum precision

### Don'ts
- ❌ Don't use very large matrices (> 1000x1000) for real-time applications
- ❌ Don't repeatedly allocate large matrices
- ❌ Don't use eigenvalue calculations for large matrices (> 100x100)
- ❌ Don't ignore memory usage for large datasets
- ❌ Don't use expensive operations when fast alternatives exist
- ❌ Don't use `.map()/.filter()` on very large datasets with simple operations
- ❌ Don't ignore the copy overhead of parallel operations on large datasets

### Performance Checklist

Before using vector operations, consider:

1. **Matrix Size**: Is it appropriate for your use case?
2. **Operation Type**: Are you using the most efficient operation?
3. **Memory Usage**: Do you have sufficient memory?
4. **Data Types**: Are you using appropriate data types?
5. **Pre-allocation**: Can you reuse allocated matrices?
6. **Precision Settings**: Can you use lower precision for better performance?
7. **Monitoring**: Are you tracking performance and memory usage?

## Real-World Performance Validation

### Linear Regression Example Results
Recent testing with a real-world linear regression implementation demonstrates Grapa's vector performance:

| Dataset Size | Training Time (128-bit) | Training Time (32-bit) | Performance |
|--------------|------------------------|------------------------|-------------|
| 100 samples  | 5.642ms               | ~3ms (estimated)       | ~0.056ms per sample |
| 10,000 samples | 529.003ms           | 276.634ms              | ~0.053ms per sample |

**Key Insights:**
- **Linear scaling**: Performance scales predictably with data size
- **Real-world ready**: 10,000 samples processed in under 1 second
- **Machine learning capable**: Suitable for practical ML applications
- **Consistent performance**: ~0.053ms per sample across different dataset sizes
- **Precision optimization**: 32-bit precision provides ~1.9x speedup with minimal accuracy loss

## Conclusion

Grapa's vector operations provide **excellent performance for real-world use cases**, including machine learning applications. By following these optimization guidelines, you can achieve:

- **Sub-second response** for real-time applications
- **Efficient processing** for batch operations (10K+ samples in <1 second)
- **Optimal memory usage** for large datasets
- **Robust error handling** for edge cases
- **Machine learning ready** performance for practical applications

The key is choosing the right matrix size and operations for your specific use case, while monitoring performance and memory usage appropriately. **Grapa's vector implementation is sufficiently fast for real-world machine learning and data science applications.**
