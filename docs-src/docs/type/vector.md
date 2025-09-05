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

/* Descending order sorting */
sorted_desc = vec.sort(null, 1);  /* #[9, 6, 5, 4, 3, 2, 1, 1]# */

/* Argsort (indices of sorted elements) */
argsorted = vec.argsort();    /* #[1, 3, 6, 0, 2, 4, 7, 5]# */

/* Argsort with descending order */
argsorted_desc = vec.argsort(null, 1);  /* #[5, 7, 4, 2, 0, 6, 3, 1]# */

/* 2D matrix sorting */
matrix = #[[3,1,4],[1,5,9],[2,6,0]]#;

/* Sort rows (axis=0) */
rows_sorted = matrix.sort(0);  /* Sort each row independently */

/* Sort columns (axis=1) */
cols_sorted = matrix.sort(1);  /* Sort each column independently */

/* Custom sorting functions (4th parameter) */
vec.sort(null, null, null, op(a,b){a-b});     /* Custom ascending */
vec.sort(null, null, null, op(a,b){b-a});     /* Custom descending */
vec.sort(null, null, null, op(a,b){a<=>b});   /* Spaceship operator */

/* Order vector for custom reordering */
order_indices = #[2, 0, 1, 3, 4, 5, 6, 7]#.vector();
vec.sort(null, order_indices);  /* Reorder based on custom indices */

/* Complete parameter reference */
/* sort(axis, order, kind, custom_function) */
/* - axis: 0 for rows, 1 for columns, null for default */
/* - order: null/0 for ascending, 1 for descending, OR vector of indices */
/* - kind: null/0 for signed, 1 for unsigned comparison */
/* - custom_function: op(a,b){...} for custom comparison logic */

/* Unique values */
unique_vals = vec.array().unique().vector();   /* Convert to array, get unique, convert back to vector */

### **Advanced Sorting Features**

#### **Custom Comparison Functions**
```grapa
/* Custom sorting with comparison functions */
vec = #[3, 1, 4, 1, 5, 9, 2, 6]#;

/* Standard ascending sort */
vec.sort(null, null, null, op(a,b){a-b});

/* Standard descending sort */
vec.sort(null, null, null, op(a,b){b-a});

/* Using spaceship operator */
vec.sort(null, null, null, op(a,b){a<=>b});

/* Complex custom logic */
vec.sort(null, null, null, op(a,b){
    if (a % 2 == 0 && b % 2 == 1) return -1;  /* Even numbers first */
    if (a % 2 == 1 && b % 2 == 0) return 1;   /* Odd numbers last */
    return a - b;  /* Within each group, sort numerically */
});
```

#### **Custom Order Vectors**
```grapa
/* Reorder using custom index vectors */
vec = #[3, 1, 4, 1, 5, 9, 2, 6]#;

/* Custom reordering by indices */
order_indices = #[2, 0, 1, 3, 4, 5, 6, 7]#.vector();
reordered = vec.sort(null, order_indices);  /* #[4,3,1,1,5,9,2,6]# */

/* Reverse order */
reverse_order = #[7,6,5,4,3,2,1,0]#.vector();
reversed = vec.sort(null, reverse_order);  /* #[6,2,9,5,1,4,1,3]# */
```

#### **2D Matrix Advanced Sorting**
```grapa
matrix = #[[3,1,4],[1,5,9],[2,6,0]]#;

/* Sort by specific column */
matrix.sort(0, null, null, op(a,b){a[1] - b[1]});  /* Sort by second column */

/* Sort by multiple criteria */
matrix.sort(0, null, null, op(a,b){
    if (a[0] != b[0]) return a[0] - b[0];  /* First by first column */
    return a[1] - b[1];  /* Then by second column */
});

/* Custom column reordering and selection */
matrix = #[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]#;

/* Reorder columns */
col_order = #[2, 0, 1]#.vector();  /* Reorder columns */
reordered = matrix.sort(1, col_order);  /* Returns object format with selected columns */

/* Extract specific columns (non-consecutive) */
first_third = matrix.sort(1, [0, 2], null, null);  /* Extract columns 0 and 2, skip 1 */
/* Returns: [{"0":1,"2":3}, {"0":5,"2":7}, {"0":9,"2":11}] */

/* Extract columns in custom order */
custom_order = matrix.sort(1, [3, 1, 0], null, null);  /* Columns 3, 1, 0 in that order */
/* Returns: [{"3":4,"1":2,"0":1}, {"3":8,"1":6,"0":5}, {"3":12,"1":10,"0":9}] */

/* Advanced column selection - more flexible than .left()/.right() */
/* Traditional methods only work with consecutive columns */
traditional = matrix.left(2);  /* #[1,2],[5,6],[9,10]# - consecutive only */

/* Sort method allows non-consecutive selection */
non_consecutive = matrix.sort(1, [0, 2], null, null);  /* Columns 0 and 2, skip 1 */
```

#### **Signed vs Unsigned Comparison**
```grapa
vec = #[3, -1, 4, -5, 9, -2, 6]#;

/* Signed comparison (default) */
signed_sort = vec.sort();  /* #[-5,-2,-1,3,4,6,9]# */

/* Unsigned comparison */
unsigned_sort = vec.sort(null, null, 1);  /* #[-1,-2,3,4,-5,6,9]# */
/* Negative numbers treated as if they were positive */
```

#### **Advanced Column Selection and Reordering**
The sort function with `axis=1` and custom order provides powerful column selection capabilities:

```grapa
matrix = #[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]#;

/* Method 1: Extract consecutive columns (traditional approach) */
first_2 = matrix.left(2);     /* #[[1,2],[5,6],[9,10]]# */
last_2 = matrix.right(2);     /* #[[3,4],[7,8],[11,12]]# */

/* Method 2: Extract non-consecutive columns (advanced approach) */
cols_0_2 = matrix.sort(1, [0, 2], null, null);  /* Extract columns 0 and 2, skip 1 */
/* Returns: [{"0":1,"2":3}, {"0":5,"2":7}, {"0":9,"2":11}] */

/* Method 3: Custom column ordering */
cols_3_1_0 = matrix.sort(1, [3, 1, 0], null, null);  /* Columns 3, 1, 0 in that order */
/* Returns: [{"3":4,"1":2,"0":1}, {"3":8,"1":6,"0":5}, {"3":12,"1":10,"0":9}] */

/* Method 4: Row selection using transpose */
transposed = matrix.t();  /* #[[1,5,9],[2,6,10],[3,7,11],[4,8,12]]# */
rows_0_2 = transposed.sort(1, [0, 2], null, null);  /* Extract rows 0 and 2 from original */
/* Returns: [{"0":1,"2":3}, {"0":5,"2":7}, {"0":9,"2":11}] */
```

**Key Advantages:**
- **Non-consecutive selection**: Pick columns 0 and 2 while skipping column 1
- **Custom ordering**: Arrange columns in any sequence
- **Advanced split capability**: More flexible than basic `.left()`/`.right()` methods
- **Row selection**: Use transpose + sort for row selection

**Output Format:**
- Returns **object format** with column indices as keys
- Each row becomes an object with selected columns as key-value pairs
- May need conversion to array/vector format for further processing

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

### **Advanced Array Operations Using Function Application**

Grapa's function application system enables implementation of many advanced array operations:

#### **Array Creation Functions**
```grapa
/* Zeros - create vector of zeros */
zeros = #[1,1,1,1,1]# * [op(x){0}];  /* #[0,0,0,0,0]# */

/* Ones - create vector of ones */
ones = #[1,1,1,1,1]# * [op(x){1}];   /* #[1,1,1,1,1]# */

/* Arange - create sequence */
arange = #[1,1,1,1,1]# * [op(x, i){i + 1}];  /* #[1,2,3,4,5]# */

/* Full - create vector with custom value */
full = #[1,1,1,1,1]# * [op(x){42}];   /* #[42,42,42,42,42]# */

/* Linspace-like - create evenly spaced values */
linspace = #[1,1,1,1,1]# * [op(x, i){i * 2.5}];  /* #[0,2.5,5,7.5,10]# */
```

#### **Shape Manipulation**
```grapa
/* Squeeze - remove single-dimensional axes */
matrix = #[[1],[2],[3]]#;
squeezed = matrix.reshape([3]);  /* #[1,2,3]# */

/* Expand dims - add single-dimensional axes */
vec = #[1,2,3]#;
expanded = vec.reshape([1,3]);   /* #[[1,2,3]]# */
expanded_2d = vec.reshape([3,1]); /* #[[1],[2],[3]]# */
```

#### **Conditional Selection (Where)**
```grapa
vec = #[1,2,3,4,5,6,7,8,9,10]#;

/* Filter elements meeting condition */
filtered = vec.array().filter(op(x){x > 5}).vector();  /* #[6,7,8,9,10]# */

/* Complex condition: between 3 and 7, and even */
filtered_complex = vec.array().filter(op(x){x >= 3 && x <= 7 && x % 2 == 0}).vector();  /* #[4,6]# */

/* Multiple ranges */
ranges = vec.array().filter(op(x){(x >= 2 && x <= 4) || (x >= 8 && x <= 10)}).vector();  /* #[2,3,4,8,9,10]# */

/* Conditional replacement */
replaced = vec * [op(x){x > 5 ? x * 2 : x}];  /* #[1,2,3,4,5,12,14,16,18,20]# */
```

#### **Mathematical Operations**
```grapa
vec = #[1.2, 2.7, 3.1, 4.9, 5.5]#;

/* Clip - limit values to range */
clipped = vec * [op(x){x < 3 ? 3 : (x > 7 ? 7 : x)}];  /* #[3,3,3.1,4.9,5.5]# */

/* Floor - round down to integer */
floored = vec * [op(x){x.int()}];  /* #[1,2,3,4,5]# */

/* Ceil - round up to integer */
ceiled = vec * [op(x){x - x.int() > 0 ? x.int() + 1 : x.int()}];  /* #[2,3,4,5,6]# */

/* Round - round to nearest integer */
rounded = vec * [op(x){(x + 0.5).int()}];  /* #[1,3,3,5,6]# */

/* Absolute value */
abs_vals = vec * [op(x){x < 0 ? -x : x}];

/* Sign function */
signs = vec * [op(x){x > 0 ? 1 : (x < 0 ? -1 : 0)}];
```

#### **Advanced Linear Algebra**
```grapa
matrix = #[[1,2,3],[4,5,6],[7,8,9]]#;

/* Trace - sum of diagonal elements */
trace = matrix.diag().sum();

/* Transpose */
transposed = matrix.t();

/* Element-wise operations */
squared = matrix * matrix;  /* Element-wise square */
sqrt_matrix = matrix * [op(x){x.sqrt()}];  /* Element-wise square root */
```

#### **Random Number Generation**
```grapa
/* Uniform random */
uniform = #[1,1,1,1,1]# * [op(x){x.random()}];  /* Random 0 to x */

/* Normal-like distribution */
normal_like = #[1,1,1,1,1]# * [op(x){(x.random() + x.random() + x.random()) / 3}];

/* Random integers */
random_ints = #[1,1,1,1,1]# * [op(x){(x.random()).int()}];

/* Seeded-like behavior (using position) */
seeded = #[1,1,1,1,1]# * [op(x, i){(i * 12345).random()}];
```

#### **Complex Transformations**
```grapa
vec = #[1,2,3,4,5,6,7,8,9,10]#;

/* Multiple transformations in one pass */
transformed = vec * [op(x){
    if (x < 3) return x * 2;
    if (x > 7) return x / 2;
    return x + 10;
}];  /* #[2,4,13,14,15,16,17,4,4.5,5]# */

/* Pattern generation */
pattern = #[1,1,1,1,1]# * [op(x, i){i % 2 == 0 ? 1 : -1}];  /* #[1,-1,1,-1,1]# */

/* Conditional masking */
masked = vec * [op(x){x % 2 == 0 ? x : null}];  /* #[0,2,0,4,0,6,0,8,0,10]# */
```

**Key Advantages:**
- **Flexibility**: Implement complex operations with custom logic
- **Performance**: Direct vector operations without intermediate conversions
- **Composability**: Chain multiple operations together
- **Expressiveness**: Go beyond basic mathematical operations
- **No Dependencies**: All operations use built-in Grapa capabilities

### **Vector Utility Operations**

```grapa
vec = #[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]#;

/* Extract leftmost elements */
left3 = vec.left(3);      /* #[1, 2, 3]# */

/* Extract rightmost elements */
right3 = vec.right(3);    /* #[8, 9, 10]# */

/* Reverse vector elements */
reversed = vec.array().reverse().vector(); /* #[10, 9, 8, 7, 6, 5, 4, 3, 2, 1]# */

/* Combine utility operations */
first_half = vec.left(5);           /* #[1, 2, 3, 4, 5]# */
second_half = vec.right(5);         /* #[6, 7, 8, 9, 10]# */
reversed_first = first_half.array().reverse().vector(); /* #[5, 4, 3, 2, 1]# */

/* 2D matrix utility operations */
matrix = #[[1, 2, 3], [4, 5, 6], [7, 8, 9]]#;
left_cols = matrix.left(2);         /* #[[1, 2], [4, 5], [7, 8]]# */
right_cols = matrix.right(1);       /* #[[3], [6], [9]]# */
reversed_matrix = matrix.array().reverse().vector(); /* #[[7, 8, 9], [4, 5, 6], [1, 2, 3]]# */
```
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

#### **Method 1: Array Conversion (Recommended for 1D vectors)**
```grapa
vec1 = #[1, 2, 3]#;
vec2 = #[4, 5, 6]#;

/* Use array concatenation and convert back to vector */
combined = vec1.array() ++ vec2.array();  /* Combine arrays: [1,2,3,4,5,6] */
combined_vec = combined.vector();         /* Convert back to vector */
```

#### **Method 2: Join Function (Recommended for 2D matrices)**
```grapa
matrix1 = #[[1, 2], [3, 4]]#;
matrix2 = #[[5, 6], [7, 8]]#;

/* Vertical join (stack matrices) */
vertical_result = [matrix1, matrix2].join(null, 0);  /* #[[1,2],[3,4],[5,6],[7,8]]# */

/* Horizontal join (concatenate side by side) */
horizontal_result = [matrix1, matrix2].join(null, 1);  /* #[[1,2,5,6],[3,4,7,8]]# */
```

#### **Method 3: ++= Operator (Now Working)**
```grapa
matrix1 = #[[1, 2], [3, 4]]#;
matrix2 = #[[5, 6], [7, 8]]#;

/* ✅ WORKING: Extends matrix correctly */
matrix1 ++= matrix2;  /* Results in: #[[1,2],[3,4],[5,6],[7,8]]# */
```

**Join Function Parameters:**
- **Input**: Array of 2D vectors (e.g., `[vec1, vec2, vec3]`)
- **delim**: Delimiter (usually `null` for vectors)
- **axis**: 
  - `0` (default): Vertical join - stack matrices on top of each other
  - `1`: Horizontal join - concatenate matrices side by side

**Requirements:**
- All vectors must be **2D matrices**
- For vertical join: same number of columns
- For horizontal join: same number of rows

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

/* Handle incompatible dimensions */
vec1 = #[1, 2, 3]#;
vec2 = #[4, 5]#;
result = vec1 + vec2;    /* Returns error - dimension mismatch */

/* Handle invalid utility operations */
vec = #[1, 2, 3]#;
result = vec.left(5);    /* Returns error - count exceeds vector length */
result = vec.right(-1);  /* Returns error - negative count not allowed */
```

## Type Conversion and Compatibility

Grapa vectors handle type conversion automatically during operations:

```grapa
/* Mixed type operations */
vec1 = #[1, 2, 3]#;           /* Integer vector */
vec2 = #[4.5, 5.5, 6.5]#;     /* Float vector */
result = vec1 + vec2;         /* #[5.5, 7.5, 9.5]# - automatic float conversion */

/* Scalar operations with type conversion */
vec = #[1, 2, 3]#;
result1 = vec + 2.5;          /* #[3.5, 4.5, 5.5]# - float result */
result2 = vec * 2;            /* #[2, 4, 6]# - integer result */

/* Array to vector conversion */
arr = [1, 2, 3, 4, 5];
vec = arr.vector();           /* #[1, 2, 3, 4, 5]# */

/* Vector to array conversion */
vec = #[1, 2, 3, 4, 5]#;
arr = vec.array();            /* [1, 2, 3, 4, 5] */

/* Complex type handling */
mixed_vec = #[1, "hello", true, [1, 2, 3]]#;  /* Mixed types supported */
```

## Best Practices

1. **Use appropriate data types**: Use vectors for numerical operations, arrays for general data
2. **Leverage function application**: Use `[op(x){...}]` for custom element-wise operations
3. **Understand axis parameters**: Use `axis=0` for rows, `axis=1` for columns in 2D operations
4. **Combine features creatively**: Use function application with `random()` for random number generation
5. **Use array conversion**: For 1D vector concatenation, convert to arrays, concatenate, then convert back to vectors
6. **Use join function**: For 2D matrix concatenation, use `[matrix1, matrix2].join(null, axis)` instead of `++=` operator
7. **Know operator limitations**: Use `.dot()` method instead of `.*` operator for dot products
8. **Avoid unsupported operations**: Modulo (`%`), bitwise operations (`~`, `&`, `|`, `^`), and dot product operator (`.*`) are not supported
9. **Use operators, not method calls**: Vector operations use operators (`+`, `*`, `**`) rather than method calls (`.add()`, `.mul()`, `.pow()`)
10. **Use ++= operator for 2D vectors**: The `++=` operator now works correctly for 2D vector extension
11. **Leverage function application**: Use `vec * [op(x){...}]` to implement advanced operations like clip, round, where, etc.
12. **Explore creative solutions**: Many "missing" functions can be implemented using Grapa's flexible function application system

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

### **Important Note: Operators vs Method Calls**

Vector mathematical operations use **operators**, not method calls:

```grapa
/* ✅ CORRECT - Use operators */
vec1 = #[1, 2, 3]#;
vec2 = #[4, 5, 6]#;
result1 = vec1 + vec2;    /* Element-wise addition */
result2 = vec1 * vec2;    /* Element-wise multiplication */
result3 = vec1 ** 2;      /* Element-wise power */

/* ❌ INCORRECT - Method calls don't work */
result4 = vec1.add(vec2); /* Returns error */
result5 = vec1.mul(vec2); /* Returns error */
result6 = vec1.pow(2);    /* Returns error */
```

The `add()`, `mul()`, `pow()` functions in the C++ backend are internal methods used by operator event handlers and are not exposed as Grapa language methods.

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
- `.join(delim, axis)` - Join 2D matrices (use `[vec1, vec2].join(null, axis)`)

### **Sorting and Searching Functions**
- `.sort(axis=null, order=null, kind=null, custom_function=null)` - Sort elements
  - `axis`: 0 for rows, 1 for columns, null for default
  - `order`: null/0 for ascending, 1 for descending, OR vector of indices for custom reordering
  - `kind`: null/0 for signed, 1 for unsigned comparison
  - `custom_function`: op(a,b){...} for custom comparison logic
- `.argsort(axis=null, order=null, kind=null, custom_function=null)` - Sort indices
  - `axis`: 0 for rows, 1 for columns, null for default
  - `order`: null/0 for ascending, 1 for descending, OR vector of indices for custom reordering
  - `kind`: null/0 for signed, 1 for unsigned comparison
  - `custom_function`: op(a,b){...} for custom comparison logic
- `.unique()` - Unique elements (use `vec.array().unique().vector()`)

### **Utility Functions**
- `.array()` - Convert to array
- `.vector()` - Convert to vector (from array)
- `.left(count)` - Extract leftmost elements
- `.right(count)` - Extract rightmost elements
- `.reverse()` - Reverse vector elements (use `vec.array().reverse().vector()`)

> **Note:** All vector operations work seamlessly with Grapa's flexible type system, allowing for creative combinations of features to achieve complex data transformations and mathematical operations.

