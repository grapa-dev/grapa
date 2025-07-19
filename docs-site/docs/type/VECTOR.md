# VECTOR Type

The VECTOR type is Grapa's native data structure for mathematical operations, particularly matrix and linear algebra computations. It provides optimized performance for mathematical operations compared to regular arrays.

## Syntax

### Native VECTOR Creation
VECTOR objects use hash (`#`) delimiters to distinguish them from arrays:

```grapa
// Empty VECTOR
empty = #[]#

// 1D VECTOR
vector = #[1, 2, 3, 4, 5]#

// 2D VECTOR (matrix)
matrix = #[[1, 2], [3, 4]]#

// 3D VECTOR
tensor = #[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]#
```

### VECTOR vs Array Syntax

| Type | Syntax | Purpose |
|------|--------|---------|
| **Array** | `[1, 2, 3]` | General-purpose collection |
| **VECTOR** | `#[1, 2, 3]#` | Mathematical operations |
| **Tuple** | `(1, 2, 3)` | Immutable collection |

## Why Use Native VECTOR?

### Performance Benefits
- **Optimized Memory Layout**: VECTOR uses specialized memory structures for mathematical operations
- **Direct Method Access**: No conversion overhead when calling matrix methods
- **Mathematical Optimization**: Operations are optimized for VECTOR's internal representation

### Type Safety
- Ensures data structure is designed for mathematical operations
- Prevents accidental use of non-mathematical data types
- Clear intent in code

## Available Methods

### Core Matrix Operations

| Method | Description | Example |
|--------|-------------|---------|
| `.det()` | Calculate determinant | `matrix.det()` |
| `.rank()` | Calculate matrix rank | `matrix.rank()` |
| `.rref()` | Row Reduced Echelon Form | `matrix.rref()` |
| `.solve(vector)` | Solve linear system Ax = b | `matrix.solve([1,2,3])` |
| `.cov(axis)` | Covariance matrix | `data.cov(0)` |

### Matrix Transformations

| Method | Description | Example |
|--------|-------------|---------|
| `.inv()` | Matrix inversion | `matrix.inv()` |
| `.t()` | Matrix transpose | `matrix.t()` |
| `.reshape(b)` | Reshape matrix | `matrix.reshape([2,3])` |

### Statistical Operations

| Method | Description | Example |
|--------|-------------|---------|
| `.sum(axis)` | Sum along axis | `matrix.sum(0)` |
| `.mean(axis)` | Mean along axis | `matrix.mean(1)` |
| `.norm()` | Vector/matrix norm | `vector.norm()` |

### Advanced Operations

| Method | Description | Example |
|--------|-------------|---------|
| `.dot(b)` | Dot product | `vector1.dot(vector2)` |
| `.triu(b)` | Upper triangular | `matrix.triu(0)` |
| `.tril(b)` | Lower triangular | `matrix.tril(0)` |
| `.eigh()` | Eigenvalues/eigenvectors | `matrix.eigh()` |
| `.shape()` | Get dimensions | `matrix.shape()` |

## Array Conversion

While VECTOR is the native type for mathematical operations, arrays are automatically converted to VECTOR when used with matrix methods:

```grapa
// Array automatically converted to VECTOR
array_matrix = [[1, 2], [3, 4]]
det = array_matrix.det()    // ARRAY → VECTOR → determinant

// Tuple automatically converted to VECTOR
tuple_matrix = ((1, 2), (3, 4))
rank = tuple_matrix.rank()  // TUPLE → VECTOR → rank
```

### Performance Comparison

```grapa
// More efficient - direct VECTOR operations
matrix = #[[1, 2, 3], [4, 5, 6], [7, 8, 9]]#
result = matrix.det()  // Direct VECTOR determinant

// Less efficient - conversion overhead
array = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
result = array.det()   // ARRAY → VECTOR → determinant
```

## Operator Support

VECTOR objects support various mathematical operators:

```grapa
// Matrix operations
A = #[[1, 2], [3, 4]]#
B = #[[5, 6], [7, 8]]#
C = A * B              // Matrix multiplication
D = A + B              // Element-wise addition
E = A * 2              // Scalar multiplication

// Row reversal using bitwise NOT
reversed = ~A          // Reverse row order
```

## Examples

### Basic Matrix Operations

```grapa
/* Create a 2x2 matrix using native VECTOR syntax */
matrix = #[[2, 1], [1, 3]]#

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
A = #[[2, 1], [1, 3]]#

/* Constant vector */
b = #[5, 6]#

/* Solve Ax = b */
solution = A.solve(b)
("Solution: " + solution + "\n").echo()
```

### Statistical Analysis

```grapa
/* Data matrix (rows = observations, cols = variables) */
data = #[[1, 2, 3], [4, 5, 6], [7, 8, 9]]#

/* Calculate covariance matrix */
covariance = data.cov()
```

### Matrix Transformations

```grapa
/* Original matrix */
matrix = #[[1, 2, 3], [4, 5, 6]]#

/* Transpose */
transposed = matrix.t()

/* Row reversal using bitwise NOT */
reversed = ~matrix

/* Matrix inversion (if square and non-singular) */
inverted = matrix.inv()
```

## Best Practices

### When to Use Native VECTOR

**Use `#[...]#` syntax when:**
- Doing intensive matrix operations
- Working with large matrices
- Performance is critical
- You want to ensure mathematical optimization
- Writing mathematical algorithms

**Use `[...]` syntax when:**
- Quick prototyping
- Small matrices
- When you need array-specific operations
- When the conversion overhead is negligible

### Memory Management

```grapa
// VECTOR uses optimized memory layout
matrix = #[[1, 2, 3], [4, 5, 6], [7, 8, 9]]#

// Large matrices benefit most from VECTOR type
large_matrix = #[/* large data */]#
result = large_matrix.det()  // Optimized for VECTOR
```

### Error Handling

```grapa
/* Singular matrix */
singular = #[[1, 1], [1, 1]]#
det = singular.det()  // Returns 0

/* Non-square matrix for determinant */
rectangular = #[[1, 2, 3], [4, 5, 6]]#
det = rectangular.det()  // Returns 0 (error)

/* Invalid dimensions for operations */
result = matrix.solve([1, 2, 3])  // Error if dimensions don't match
```

## Integration with Other Types

### Type Conversion

```grapa
// Convert other types to VECTOR
array = [1, 2, 3, 4, 5]
vector = array.vector()  // Convert array to VECTOR

// VECTOR to other types
matrix = #[[1, 2], [3, 4]]#
array_result = matrix.array()  // Convert VECTOR to array
```

### Function Chaining

```grapa
/* Chain multiple VECTOR operations */
result = matrix.t().det().inv()

/* Combine with other operations */
processed = data.vector().cov().t()
```

## Conclusion

The VECTOR type is Grapa's specialized data structure for mathematical operations. While arrays can be automatically converted to VECTOR for convenience, using the native `#[...]#` syntax provides better performance and clearer intent for mathematical computations.

For serious mathematical work, especially with large matrices or performance-critical applications, always use the native VECTOR syntax to take advantage of Grapa's optimized mathematical operations. 