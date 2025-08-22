---
tags:
  - user
  - highlevel
---
# $VECTOR

A high-performance vector type optimized for numerical operations and large datasets.

## Overview

`$VECTOR` is a specialized data type designed for efficient numerical computations and large-scale data processing. It provides optimized operations for mathematical operations and memory management.

## Basic Operations

### Creation
```grapa
vec = [1, 2, 3, 4, 5];    /* Create vector from array */
vec = (1, 2, 3, 4, 5);    /* Create vector from tuple */
```

### Addition Operations (`+=`)
```grapa
vec = [1, 2, 3];
vec += 4;                 /* Add single element: [1,2,3,4] */
vec += [5, 6];            /* Add multiple elements: [1,2,3,4,5,6] */
```

### Concatenation Operations (`++=`)
```grapa
vec1 = [1, 2, 3];
vec2 = [4, 5, 6];
vec1 ++= vec2;            /* Extend vector: [1,2,3,4,5,6] */
```

### Mathematical Operations
```grapa
vec1 = [1, 2, 3];
vec2 = [4, 5, 6];

/* Vector arithmetic */
sum = vec1 + vec2;        /* Element-wise addition */
diff = vec1 - vec2;       /* Element-wise subtraction */
product = vec1 * vec2;    /* Element-wise multiplication */
quotient = vec1 / vec2;   /* Element-wise division */

/* Scalar operations */
scaled = vec1 * 2;        /* Scale vector by scalar */
shifted = vec1 + 10;      /* Add scalar to all elements */
```

## Performance Characteristics

- **Optimized Memory Layout**: Designed for cache-friendly access patterns
- **Parallel Operations**: Many operations are automatically parallelized
- **Large Dataset Support**: Efficient handling of vectors with millions of elements
- **Mathematical Optimization**: Specialized for numerical computations

## Performance Guidelines

### Matrix Size Recommendations

| Use Case | Recommended Size | Performance Expectation |
|----------|------------------|------------------------|
| Real-time | < 50x50         | Sub-second response    |
| Interactive | < 100x100       | < 5 second response    |
| Batch processing | < 500x500     | < 1 minute response    |
| Large-scale | > 500x500       | Consider alternatives   |

### Operation Performance

| Operation Type | Recommended Matrix Size | Performance Notes |
|----------------|-------------------------|-------------------|
| Matrix multiplication | < 100x100 | O(n³) complexity |
| Statistical functions | < 200x200 | Good performance |
| Basic operations | Any size | Very fast |
| Eigenvalue calculation | < 50x50 | Computationally intensive |

### Memory Usage

Estimated memory usage based on 8 bytes per element:

| Matrix Size | Elements | Estimated Memory | Memory Category |
|-------------|----------|------------------|-----------------|
| 10x10       | 100      | 0.8 KB           | Very Small      |
| 50x50       | 2,500    | 20 KB            | Small           |
| 100x100     | 10,000   | 80 KB            | Medium          |
| 200x200     | 40,000   | 320 KB           | Large           |
| 500x500     | 250,000  | 2 MB             | Very Large      |

## Use Cases

### Data Science
```grapa
/* Statistical operations */
data = [1.2, 3.4, 5.6, 7.8, 9.0];
mean = data.sum() / data.len();
variance = data.map(op(x){(x - mean)**2;}).sum() / data.len();
```

### Signal Processing
```grapa
/* Signal operations */
signal = [0.1, 0.3, 0.5, 0.7, 0.9];
filtered = signal.map(op(x){x * 0.5;});
```

### Machine Learning
```grapa
/* Feature vectors */
features = [1.0, 2.0, 3.0, 4.0];
weights = [0.1, 0.2, 0.3, 0.4];
prediction = features.dot(weights);
```

For comprehensive machine learning capabilities, see [Machine Learning Guide](../machine_learning.md) which covers:
- Linear regression (normal equation, gradient descent, ridge regression)
- Statistical analysis and feature preprocessing
- Model evaluation and performance metrics
- Complete implementation examples

## Advanced Features

### Vector Operations
```grapa
vec = [1, 2, 3, 4, 5];

/* Statistical functions */
vec.sum();                /* Sum of all elements */
vec.mean();               /* Arithmetic mean */
vec.std();                /* Standard deviation */
vec.min();                /* Minimum value */
vec.max();                /* Maximum value */

/* Mathematical functions */
vec.abs();                /* Absolute values */
vec.sqrt();               /* Square root of each element */
vec.pow(2);               /* Each element raised to power */
vec.exp();                /* Exponential of each element */
vec.log();                /* Natural logarithm of each element */
```

### Matrix Operations
```grapa
mat = [[1, 2], [3, 4]];

/* Basic matrix operations */
mat.t();                  /* Transpose matrix: [[1,3],[2,4]] */
mat.inv();                /* Matrix inverse: [[-2,1],[1.5,-0.5]] */
mat.det();                /* Determinant: -2.0 */
mat.rank();               /* Matrix rank: 2 */
mat.rref();               /* Reduced row echelon form */

/* Linear algebra */
mat.solve([5, 6]);        /* Solve linear system Ax = b */
mat.dot([1, 2]);          /* Dot product: [17,39] */
mat.norm();               /* Matrix/vector norm */

/* Matrix decomposition */
mat.eigh();               /* Eigenvalues and eigenvectors */

/* Matrix structure */
mat.triu();               /* Upper triangular part */
mat.tril();               /* Lower triangular part */
mat.triu(1);              /* Upper triangular with offset */
mat.tril(-1);             /* Lower triangular with offset */
```

### Multi-dimensional Operations
```grapa
/* Statistical operations with axis parameter */
data_3d = [[[1,2],[3,4]], [[5,6],[7,8]]];

data_3d.sum();            /* Sum all elements */
data_3d.sum(0);           /* Sum along axis 0 */
data_3d.sum(1);           /* Sum along axis 1 */
data_3d.sum(2);           /* Sum along axis 2 */

data_3d.mean();           /* Mean of all elements */
data_3d.mean(0);          /* Mean along axis 0 */
data_3d.cov();            /* Covariance matrix */
data_3d.cov(0);           /* Covariance along axis 0 */

/* Shape and reshaping */
data_3d.shape();          /* Get dimensions [2, 2, 2] */
data_3d.reshape([4, 2]);  /* Reshape to 4x2 matrix */
```

### Broadcasting
```grapa
/* Automatic broadcasting for different-sized vectors */
vec1 = [1, 2, 3];
vec2 = [10];
result = vec1 + vec2;     /* [11, 12, 13] - vec2 broadcast to match vec1 */
```

### Comparison Operators
```grapa
vec1 = [1, 2, 3];
vec2 = [1, 2, 3];
vec3 = [1, 2, 4];

/* Equality comparison */
vec1 == vec2;             /* true - same content and dimensions */
vec1 == vec3;             /* false - different content */

/* Inequality comparison */
vec1 != vec3;             /* true - different content */
vec1 != vec2;             /* false - same content */

/* Order comparison (lexicographic) */
vec1 < vec3;              /* true - vec1 comes before vec3 */
vec1 <= vec2;             /* true - vec1 equals vec2 */
vec3 > vec1;              /* true - vec3 comes after vec1 */
vec2 >= vec1;             /* true - vec2 equals vec1 */

/* Type handling */
vec1 == [1, 2, 3];        /* false - different types (vector vs array) */
vec1 == [1, 2, 3].vector(); /* true - after conversion */
vec1.array() == [1, 2, 3];  /* true - after conversion */
vec1 == {a:1, b:2};       /* false - different data structure */
```

**Comparison Rules:**
- **Element-by-element comparison** with dimension checking
- **Type safety**: Vectors only compare equal to other vectors
- **Lexicographic ordering** for `<`, `>`, `<=`, `>=` operators
- **Null handling**: Empty vectors compare correctly
- **Nested support**: Vectors containing other data structures compare recursively

## Edge Case Behavior

### Empty Vectors and Matrices
```grapa
empty_vec = [].vector();
empty_sum = empty_vec.sum();  /* Returns null */
empty_mean = empty_vec.mean(); /* Returns null */

empty_mat = [[]].vector();
empty_det = empty_mat.det();  /* Returns 0.0 */
```

### Singular Matrices
```grapa
singular_mat = [[1, 1], [1, 1]].vector();
det_result = singular_mat.det();  /* Returns 0.0 */
inv_result = singular_mat.inv();  /* Returns null */
```

### Non-square Matrices
```grapa
rect_mat = [[1, 2, 3], [4, 5, 6]].vector();
rect_det = rect_mat.det();  /* Returns 0.0 */
rect_inv = rect_mat.inv();  /* Returns null */
```

### Incompatible Operations
```grapa
mat_a = [[1, 2], [3, 4]].vector();
mat_b = [[1, 2, 3], [4, 5, 6]].vector();
result = mat_a.dot(mat_b);  /* Returns null - incompatible dimensions */
```

### Extreme Values
```grapa
large_mat = [[1e15, 2e15], [3e15, 4e15]].vector();
large_det = large_mat.det();  /* Handled correctly */

small_mat = [[1e-15, 2e-15], [3e-15, 4e-15]].vector();
small_det = small_mat.det();  /* Handled correctly */
```

**Edge Case Characteristics:**
- **Graceful Handling**: All edge cases handled without crashes
- **Null Returns**: Invalid operations return null instead of errors
- **Mathematical Correctness**: Singular matrices and special cases handled correctly
- **Performance**: Edge cases perform excellently (0ms for most operations)

## Integration with Other Types

### Conversion
```grapa
/* Convert between types */
arr = [1, 2, 3];
vec = arr.vector();       /* Convert array to vector */
arr2 = vec.array();       /* Convert vector back to array */

/* Multi-dimensional arrays */
arr_2d = [[1, 2], [3, 4]];
vec_2d = arr_2d.vector(); /* Convert 2D array to vector */

/* Interchangeable comparisons */
vec = #[1, 2, 3]#;
arr = [1, 2, 3];
vec == arr.vector();      /* true - after conversion */
vec.array() == arr;       /* true - after conversion */
```

**Conversion Rules:**
- **Vector to Array**: Always succeeds - vectors can always convert to arrays
- **Array to Vector**: Only succeeds for properly structured arrays
  - ✅ Regular arrays: `[1, 2, 3]` → `#[1, 2, 3]#`
  - ✅ Multi-dimensional: `[[1,2],[3,4]]` → `#[[1,2],[3,4]]#`
  - ❌ Irregular arrays: `[[1,2],[3]]` → `$ERR` (ragged arrays)
  - ❌ Mixed structures: `[1,[2,3],4]` → `$ERR` (inconsistent nesting)

### Mixed Operations
```grapa
/* Operations with other data types */
vec = [1, 2, 3];
list = {a:10, b:20};
result = vec + list.values(); /* Vector + array values */
```

## Performance Optimization

### Best Practices

1. **Use Appropriate Matrix Sizes**
   - Keep matrices < 50x50 for real-time applications
   - Use < 100x100 for interactive applications
   - Consider alternatives for matrices > 500x500

2. **Pre-allocate When Possible**
   - Create matrices once and reuse them
   - Avoid repeated creation of large matrices

3. **Choose Data Types Wisely**
   - Use INT for integer data
   - Use FLOAT for decimal data
   - Consider precision vs performance trade-offs

4. **Optimize for Your Use Case**
   - Real-time: Use smaller matrices and break large problems into blocks
   - Batch processing: Monitor memory usage for large datasets
   - Interactive: Balance performance with user experience

### Memory Management

- **Monitor Memory Usage**: Be aware of memory requirements for large matrices
- **Use Memory-Efficient Operations**: Consider memory usage for long-running operations
- **Profile Memory**: Monitor memory usage for large datasets

## Examples

For comprehensive examples of all vector operations, see [Vector Operations Examples](../examples/vector_operations.grc) which demonstrates:

- Basic vector creation and conversion
- Matrix operations and linear algebra
- Statistical functions with axis support
- Multi-dimensional operations
- Performance considerations
- Error handling patterns
- Real-world use cases

## See also
- [$ARRAY object](array.md)
- [$LIST object](list.md)
- [Assignment Operators](../operators/assignment.md)
- [Mathematical Operations](../operators/math.md)

