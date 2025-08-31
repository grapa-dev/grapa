---
tags:
  - user
  - highlevel
---
# Vector Type (`$VECTOR`)

Vectors in Grapa are multi-dimensional data structures that support mathematical operations, statistical functions, and linear algebra operations. Vectors can contain mixed data types and automatically handle type conversions during operations.

## Vector Creation

```grapa
/* Create vectors from arrays */
vec1 = [1, 2, 3, 4, 5].vector();  /* 1D vector */
vec2 = [[1, 2, 3], [4, 5, 6]].vector();  /* 2D vector (matrix) */

/* Direct vector syntax */
vec3 = #[1, 2, 3, 4, 5]#;  /* 1D vector */
matrix = #[[1, 2, 3], [4, 5, 6]]#;  /* 2D vector */

/* Mixed data types */
mixed_vec = #[1, "hello", true, [1, 2, 3]]#;  /* Vectors can contain any Grapa type */
```

## Basic Vector Operations

### **Mathematical Operations**
```grapa
vec1 = #[1, 2, 3, 4, 5]#;
vec2 = #[2, 3, 4, 5, 6]#;

/* Element-wise operations */
sum = vec1 + vec2;        /* #[3, 5, 7, 9, 11]# */
diff = vec1 - vec2;       /* #[-1, -1, -1, -1, -1]# */
product = vec1 * vec2;    /* #[2, 6, 12, 20, 30]# */
quotient = vec1 / vec2;   /* #[0.5, 0.666..., 0.75, 0.8, 0.833...]# */

/* Power and root operations */
powered = vec1 ** 2;      /* #[1, 4, 9, 16, 25]# */
rooted = vec1 */ 2;       /* #[1, 1.414..., 1.732..., 2, 2.236...]# */

/* Scalar operations */
scaled = vec1 * 2;        /* #[2, 4, 6, 8, 10]# */
shifted = vec1 + 5;       /* #[6, 7, 8, 9, 10]# */
```

### **Statistical Functions**

#### **Basic Statistics**
```grapa
vec = #[1, 2, 3, 4, 5]#;

sum_val = vec.sum();      /* #[15]# */
mean_val = vec.mean();    /* #[3]# */
min_val = vec.min();      /* #[1]# */
max_val = vec.max();      /* #[5]# */
std_val = vec.std();      /* #[1.5811388300841896659994467722163]# */
var_val = vec.var();      /* #[2.5]# */
```

#### **Advanced Statistics**
```grapa
vec = #[1, 2, 3, 4, 5]#;

norm_val = vec.norm();    /* #[7.4161984870956629487113974408007]# */
median_val = vec.median(); /* #[3]# */
mode_val = vec.mode();    /* #[3]# - most frequent value */

/* Percentiles and quantiles */
percentile_25 = vec.percentile(0.25);  /* #[2]# */
quantile_75 = vec.quantile(0.75);      /* #[4]# */

/* Distribution analysis */
skew_val = vec.skew();    /* #[0.0]# - distribution symmetry */
kurt_val = vec.kurtosis(); /* #[-1.912]# - distribution "tailedness" */
```

#### **2D Vector Statistics (Matrices)**
```grapa
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]].vector();

/* Axis-based operations */
sum_rows = matrix.sum(0);     /* Sum along rows: #[12, 15, 18]# */
sum_cols = matrix.sum(1);     /* Sum along columns: #[6, 15, 24]# */
mean_rows = matrix.mean(0);   /* Mean along rows: #[4, 5, 6]# */
std_cols = matrix.std(1);     /* Standard deviation along columns: #[1, 1, 1]# */
```

### **Linear Algebra Operations**

#### **Matrix Operations**
```grapa
matrix = [[1, 2], [3, 4]].vector();

/* Basic matrix operations */
det_val = matrix.det();       /* Determinant: -2.0 */
rank_val = matrix.rank();     /* Matrix rank: 2 */
inv_matrix = matrix.inv();    /* Inverse: #[[-2,1],[1.5,-0.5]]# */
t_matrix = matrix.t();        /* Transpose: #[[1,3],[2,4]]# */

/* Triangular matrices */
triu_matrix = matrix.triu();  /* Upper triangular: #[[1,2],[0,4]]# */
tril_matrix = matrix.tril();  /* Lower triangular: #[[1,0],[3,4]]# */

/* Diagonal operations */
diag_vals = matrix.diag();    /* Extract diagonal: #[1,4]# */

/* Create diagonal matrix from vector */
vec = #[1, 2, 3]#;
diag_matrix = vec.diag();     /* Create diagonal matrix: #[[1,0,0],[0,2,0],[0,0,3]]# */

/* Alternative creative approach using identity matrix */
diag_matrix2 = 3.identity() * vec;  /* Same result using element-wise multiplication */
```

#### **Advanced Linear Algebra**
```grapa
matrix = [[1, 2], [3, 4]].vector();

/* Eigenvalue decomposition */
eigh_result = matrix.eigh();
/* Returns: {"w":#[eigenvalues]#, "v":#[eigenvectors]#} */

/* Linear system solving */
/* Matrix must be n×n+1 where last column is the right-hand side */
augmented_matrix = [[1,2,5],[3,4,6]].vector();  /* [A|b] format */
solution = augmented_matrix.solve();  /* Solves Ax = b */
```

#### **Dot Product and Matrix Multiplication**
```grapa
vec1 = #[1, 2, 3]#;
vec2 = #[4, 5, 6]#;

/* Vector dot product (returns scalar) */
dot_result = vec1.dot(vec2);  /* #[32]# - traditional dot product */

/* Matrix multiplication */
matrix1 = [[1, 2], [3, 4]].vector();
matrix2 = [[5, 6], [7, 8]].vector();
result = matrix1.dot(matrix2);  /* Matrix multiplication: #[[19,22],[43,50]]# */

/* Vector-matrix multiplication */
vec = [1, 2].vector();
matrix = [[3, 4], [5, 6]].vector();
result = vec.dot(matrix);  /* Vector-matrix: #[13,16]# */
```

### **Shape and Reshaping**
```grapa
vec = #[1, 2, 3, 4, 5, 6]#;

/* Get shape */
shape = vec.shape();          /* #[6]# for 1D, #[rows,cols]# for 2D */

/* Reshape vector */
reshaped = vec.reshape([2, 3]);  /* #[[1,2,3],[4,5,6]]# */
reshaped_auto = vec.reshape([2, -1]);  /* Automatic dimension inference */
```

### **Sorting and Searching**
```grapa
vec = #[3, 1, 4, 1, 5, 9, 2, 6]#;

/* Basic sorting */
sorted = vec.sort();          /* #[1, 1, 2, 3, 4, 5, 6, 9]# */

/* Argsort (indices of sorted elements) */
argsorted = vec.argsort();    /* #[1, 3, 6, 0, 2, 4, 7, 5]# */

/* Unique values */
unique_vals = vec.array().unique().vector();   /* Convert to array, get unique, convert back to vector */
```

### **Covariance and Correlation**
```grapa
data = [[1, 2, 3], [4, 5, 6], [7, 8, 9]].vector();

/* Covariance matrix */
cov_matrix = data.cov();      /* #[[1,1,1],[1,1,1],[1,1,1]]# */
```

## Advanced Vector Features

### **Function Application**
Vectors support applying custom functions to elements:

```grapa
vec = #[1, 2, 3, 4, 5]#;

/* Apply custom function */
doubled = vec * [op(x){x * 2}];  /* #[2, 4, 6, 8, 10]# */

/* Random number generation */
random_vec = #[1, 1, 1, 1, 1]# * [op(x){32.random()}];  /* Random values 0-31 */

/* Dynamic range random generation */
dynamic_random = #[5, 10, 24, 64]# * [op(x){x.random()}];  /* Each element: random 0 to x */

/* Pattern generation */
alternating = #[1, 1, 1, 1, 1]# * [op(x, i){i % 2 == 0 ? 1 : -1}];  /* #[1, -1, 1, -1, 1]# */
```

### **Creative Vector Generation**
```grapa
/* Generate zeros and ones */
zeros = #[1, 1, 1, 1, 1]# * [op(x){0}];        /* #[0, 0, 0, 0, 0]# */
ones = #[1, 1, 1, 1, 1]# * [op(x){1}];         /* #[1, 1, 1, 1, 1]# */

/* Generate sequences */
sequence = #[1, 1, 1, 1, 1]# * [op(x, i){i + 1}];  /* #[1, 2, 3, 4, 5]# */
```

### **Creative Vector Multiplication Techniques**
Grapa's flexible multiplication operators enable creative matrix and vector operations:

```grapa
/* Create diagonal matrices from vectors */
vec = #[1, 2, 3]#;

/* Method 1: Using enhanced .diag() function */
diag_matrix = vec.diag();  /* #[[1,0,0],[0,2,0],[0,0,3]]# */

/* Method 2: Using identity matrix multiplication */
diag_matrix2 = 3.identity() * vec;  /* Same result using element-wise multiplication */

/* Method 3: Using function application with identity */
diag_matrix3 = 3.identity() * [op(x, i){i < vec.len() ? vec[i] : 0}];

/* Create custom patterns using multiplication */
/* Multiply identity matrix by custom function */
pattern_matrix = 3.identity() * [op(x, i){i + 1}];  /* #[[1,0,0],[0,2,0],[0,0,3]]# */

/* Create sparse matrices */
sparse = 3.identity() * [op(x, i){i == 1 ? 5 : 0}];  /* #[[0,0,0],[0,5,0],[0,0,0]]# */

/* Element-wise matrix operations */
matrix1 = [[1, 2], [3, 4]].vector();
matrix2 = [[5, 6], [7, 8]].vector();
element_wise = matrix1 * matrix2;  /* #[[5,12],[21,32]]# - element-wise multiplication */
```

### **Vector Splitting (2D Matrices Only)**
```grapa
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]].vector();

/* Split into parts */
row_parts = matrix.split(null, 2, 0);  /* Split into 2 row groups */
col_parts = matrix.split(null, 2, 1);  /* Split into 2 column groups */
full_split = matrix.split(null, 4, 0); /* Split into 4 individual rows */

/* Parameters: split(delim, num, axis) */
/* - delim: delimiter (usually null for vectors) */
/* - num: number of parts to split into */
/* - axis: 0 for rows, 1 for columns */
```

## Vector Concatenation

```grapa
vec1 = #[1, 2, 3]#;
vec2 = #[4, 5, 6]#;

/* Use array concatenation and convert back to vector */
combined = vec1.array() ++ vec2.array();  /* Combine arrays: [1,2,3,4,5,6] */
combined_vec = combined.vector();         /* Convert back to vector */
```

## Type Flexibility

Grapa vectors are highly flexible with data types:

```grapa
/* Mixed data types */
mixed = #[1, "hello", true, [1, 2, 3]]#;

/* Automatic type conversion during operations */
numeric = #[1, 2, 3]# + #[4, 5, 6]#;  /* #[5, 7, 9]# */

/* Best assumptions for operations */
result = #[1, 2, 3]# * 2;  /* #[2, 4, 6]# */
```

## Performance Considerations

- **Memory Efficiency**: Vectors use optimized memory layouts for numerical operations
- **Type Coercion**: Automatic type conversion may impact performance for mixed-type vectors
- **Large Operations**: For very large vectors, consider using axis-based operations to reduce memory usage

## Error Handling

```grapa
/* Handle insufficient data */
vec = #[1, 2]#;
skew_val = vec.skew();  /* Returns error - need at least 3 values */

/* Handle singular matrices */
singular_matrix = [[1, 1], [1, 1]].vector();
inv_result = singular_matrix.inv();  /* Returns error - matrix is singular */

/* Handle unsupported operations */
vec = #[1, 2, 3]#;
mod_result = vec % 2;    /* Returns error - modulo not supported for vectors */
dot_result = vec .* vec; /* Returns null - dot product operator not supported */
bit_result = ~vec;       /* Returns null - bitwise operations not supported */
```

## Best Practices

1. **Use appropriate data types**: Use vectors for numerical operations, arrays for general data
2. **Leverage function application**: Use `[op(x){...}]` for custom element-wise operations
3. **Understand axis parameters**: Use `axis=0` for rows, `axis=1` for columns in 2D operations
4. **Combine features creatively**: Use function application with `random()` for random number generation
5. **Use array conversion**: For concatenation, convert to arrays, concatenate, then convert back to vectors
6. **Know operator limitations**: Use `.dot()` method instead of `.*` operator for dot products
7. **Avoid unsupported operations**: Modulo (`%`), bitwise operations (`~`, `&`, `|`, `^`), and dot product operator (`.*`) are not supported

## Operator Support

### **Supported Mathematical Operators**

| Operator | Description | Example | Result |
|:---------|:------------|:--------|:-------|
| `+` | Element-wise addition | `#[1,2,3]# + #[4,5,6]#` | `#[5,7,9]#` |
| `-` | Element-wise subtraction | `#[1,2,3]# - #[4,5,6]#` | `#[-3,-3,-3]#` |
| `*` | Element-wise multiplication | `#[1,2,3]# * #[4,5,6]#` | `#[4,10,18]#` |
| `/` | Element-wise division | `#[1,2,3]# / #[4,5,6]#` | `#[0.25,0.4,0.5]#` |
| `**` | Element-wise power | `#[1,2,3]# ** 2` | `#[1,4,9]#` |
| `*/` | Element-wise root | `#[4,9,16]# */ 2` | `#[2,3,4]#` |

### **Scalar Operations**

| Operation | Example | Result |
|:----------|:--------|:-------|
| Scalar multiplication | `#[1,2,3]# * 2` | `#[2,4,6]#` |
| Scalar addition | `#[1,2,3]# + 5` | `#[6,7,8]#` |
| Scalar power | `#[1,2,3]# ** 2` | `#[1,4,9]#` |

### **Unsupported Operators**

| Operator | Reason | Alternative |
|:---------|:-------|:------------|
| `%` | Modulo not implemented for vectors | Use function application: `vec * [op(x){x % 2}]` |
| `.*` | Dot product operator not supported | Use `.dot()` method: `vec1.dot(vec2)` |
| `~` | Bitwise operations not supported | Use function application: `vec * [op(x){~x}]` |
| `&`, `\|`, `^` | Bitwise operations not supported | Use function application for element-wise operations |

## Complete Function Reference

### **Statistical Functions**
- `.sum(axis=null)` - Sum of elements
- `.mean(axis=null)` - Arithmetic mean
- `.min(axis=null)` - Minimum value
- `.max(axis=null)` - Maximum value
- `.std(axis=null)` - Standard deviation
- `.var(axis=null)` - Variance
- `.norm(axis=null)` - Vector norm
- `.median(axis=null)` - Median value
- `.mode(axis=null)` - Most frequent value
- `.percentile(q, axis=null)` - Percentile
- `.quantile(q, axis=null)` - Quantile
- `.skew(axis=null)` - Skewness
- `.kurtosis(axis=null)` - Kurtosis

### **Linear Algebra Functions**
- `.det()` - Matrix determinant
- `.rank()` - Matrix rank
- `.inv()` - Matrix inverse
- `.t()` - Matrix transpose
- `.triu(offset=null)` - Upper triangular matrix
- `.tril(offset=null)` - Lower triangular matrix
- `.diag(offset=null)` - Extract diagonal from 2D matrices or create diagonal matrix from 1D vectors
- `.eigh()` - Eigenvalue decomposition
- `.solve()` - Linear system solver (matrix must be n×n+1 with last column as right-hand side)
- `.dot(other)` - Dot product (1D vectors) or matrix multiplication (2D matrices)
- `.cov(axis=null)` - Covariance matrix

### **Shape and Structure Functions**
- `.shape()` - Get vector shape
- `.reshape(shape)` - Reshape vector
- `.split(delim, num, axis)` - Split 2D matrices

### **Sorting and Searching Functions**
- `.sort(axis=null, order=null, kind=null)` - Sort elements
- `.argsort(axis=null, order=null, kind=null)` - Sort indices
- `.unique()` - Unique elements (use `vec.array().unique().vector()`)

### **Utility Functions**
- `.array()` - Convert to array
- `.vector()` - Convert to vector (from array)

> **Note:** All vector operations work seamlessly with Grapa's flexible type system, allowing for creative combinations of features to achieve complex data transformations and mathematical operations.

