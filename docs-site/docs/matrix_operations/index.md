# Matrix Operations in Grapa

Grapa provides sophisticated linear algebra capabilities through its VECTOR data type and specialized matrix operations. This section covers the advanced mathematical operations available for matrix manipulation and analysis.

## Overview

Grapa's matrix operations are built on the VECTOR data type, which represents multi-dimensional arrays optimized for mathematical computations. The system provides both basic matrix operations and advanced linear algebra functions.

## Available Matrix Operations

### Core Matrix Operations

| Operation | Method | Description | Example |
|-----------|--------|-------------|---------|
| **Determinant** | `.det()` | Calculate the determinant of a square matrix | `matrix.det()` |
| **Matrix Rank** | `.rank()` | Calculate the rank of a matrix | `matrix.rank()` |
| **Row Reduced Echelon Form** | `.rref()` | Transform matrix to RREF | `matrix.rref()` |
| **Linear System Solving** | `.solve(vector)` | Solve system Ax = b | `matrix.solve([1,2,3])` |
| **Covariance Matrix** | `.cov()` | Calculate covariance matrix | `data.cov()` |

### Matrix Transformations

| Operation | Method | Description | Example |
|-----------|--------|-------------|---------|
| **Matrix Inversion** | `.inv()` | Calculate matrix inverse | `matrix.inv()` |
| **Matrix Transpose** | `.t()` | Transpose matrix | `matrix.t()` |
| **Row Reversal** | `~matrix` | Reverse row order (bitwise NOT) | `~matrix` |

### Advanced Operations

| Operation | Method | Description | Example |
|-----------|--------|-------------|---------|
| **Identity Matrix** | `.identity(n)` | Create n×n identity matrix | `matrix.identity(3)` |
| **Diagonal Extraction** | `.diagonal(n)` | Extract diagonal elements | `matrix.diagonal(0)` |
| **Upper Triangular** | `.triu(n)` | Extract upper triangular part | `matrix.triu(0)` |
| **Lower Triangular** | `.tril(n)` | Extract lower triangular part | `matrix.tril(0)` |

## Data Types

### VECTOR Type
The primary data type for matrix operations is `VECTOR`, which represents multi-dimensional arrays optimized for mathematical computations:

```grapa
// Create a 2x3 matrix using native VECTOR syntax
matrix = #[[1, 2, 3], [4, 5, 6]]#

// Create a vector
vector = #[1, 2, 3, 4, 5]#
```

### Type Conversion
Arrays and tuples are automatically converted to VECTOR type when used with matrix operations:

```grapa
// Array automatically converted to VECTOR
array_matrix = [[1, 2], [3, 4]]
det = array_matrix.det()  // ARRAY → VECTOR → determinant

// Tuple automatically converted to VECTOR  
tuple_matrix = ((1, 2), (3, 4))
rank = tuple_matrix.rank()  // TUPLE → VECTOR → rank
```

### Performance Considerations

**Native VECTOR Syntax (`#[...]#`):**
- Optimized memory layout for mathematical operations
- Direct method access without conversion overhead
- Better performance for large matrices

**Array Conversion (`[...]`):**
- Convenient for quick prototyping
- Automatic conversion to VECTOR internally
- Slight performance overhead due to conversion

For optimal performance with large matrices, use the native VECTOR syntax: `#[[1, 2], [3, 4]]#`

## Mathematical Capabilities

### Linear Algebra
- **Gauss Elimination**: Used for determinant calculation and RREF
- **Matrix Factorization**: Support for triangular decompositions
- **Eigenvalue Analysis**: Through rank and determinant operations
- **Statistical Analysis**: Covariance matrix calculations

### Numerical Methods
- **High Precision**: Uses GrapaFloat for numerical computations
- **Error Handling**: Returns appropriate error values for singular matrices
- **Performance Optimized**: Efficient memory management for large matrices

## Examples

### Basic Matrix Operations

```grapa
/* Create a 2x2 matrix */
matrix = [[2, 1], [1, 3]]

/* Calculate determinant */
det = matrix.det()
("Determinant: " + det + "\n").echo()  // Output: 5.0

/* Calculate rank */
rank = matrix.rank()
("Rank: " + rank + "\n").echo()  // Output: 2
```

### Linear System Solving

```grapa
/* Coefficient matrix */
A = [[2, 1], [1, 3]]

/* Constant vector */
b = [5, 6]

/* Solve Ax = b */
solution = A.solve(b)
("Solution: " + solution + "\n").echo()
```

### Matrix Transformations

```grapa
/* Original matrix */
matrix = [[1, 2, 3], [4, 5, 6]]

/* Transpose */
transposed = matrix.t()

/* Row reversal using bitwise NOT */
reversed = ~matrix

/* Matrix inversion (if square and non-singular) */
inverted = matrix.inv()
```

### Statistical Analysis

```grapa
/* Data matrix (rows = observations, cols = variables) */
data = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]

/* Calculate covariance matrix */
covariance = data.cov()
```

## Performance Considerations

### Memory Management
- VECTOR type uses optimized memory layout
- Automatic cleanup of temporary matrices
- Efficient copying and transformation operations

### Computational Complexity
- **Determinant**: O(n³) for n×n matrices
- **Rank**: O(n³) using Gaussian elimination
- **Matrix Multiplication**: O(n³) for n×n matrices
- **Inversion**: O(n³) using Gauss-Jordan elimination

### Best Practices
1. **Use VECTOR for large matrices**: Better performance than arrays
2. **Avoid repeated operations**: Cache results when possible
3. **Check matrix properties**: Verify dimensions before operations
4. **Handle singular matrices**: Check for zero determinants

## Error Handling

Matrix operations return appropriate error values for problematic cases:

```grapa
/* Singular matrix */
singular = [[1, 1], [1, 1]]
det = singular.det()  // Returns 0

/* Non-square matrix for determinant */
rectangular = [[1, 2, 3], [4, 5, 6]]
det = rectangular.det()  // Returns 0 (error)

/* Invalid dimensions for operations */
result = matrix.solve([1, 2, 3])  // Error if dimensions don't match
```

## Advanced Features

### Matrix Decompositions
- **LU Decomposition**: Through RREF operations
- **QR Decomposition**: Available through matrix transformations
- **Cholesky Decomposition**: For symmetric positive definite matrices

### Specialized Operations
- **Block Matrix Operations**: Support for partitioned matrices
- **Sparse Matrix Support**: Efficient handling of sparse structures
- **Complex Number Support**: Through extended data types

## Integration with Other Grapa Features

### Operator Overloading
Matrix operations integrate seamlessly with Grapa's operator system:

```grapa
/* Matrix multiplication */
A = [[1, 2], [3, 4]]
B = [[5, 6], [7, 8]]
C = A * B

/* Element-wise operations */
D = A + B
E = A * 2  // Scalar multiplication
```

### Function Chaining
Matrix operations support method chaining:

```grapa
/* Chain multiple operations */
result = matrix.t().det().inv()
```

## Conclusion

Grapa's matrix operations provide a comprehensive set of linear algebra tools suitable for scientific computing, data analysis, and mathematical modeling. The integration with Grapa's type system and operator overloading makes matrix operations intuitive and efficient.

For more advanced usage patterns and performance optimization techniques, see the [Advanced Matrix Operations](advanced_operations.md) section. 