---
tags:
  - user
  - highlevel
---
# $VECTOR

References:
- [Array-Vector Interoperability](../advanced/array_vector_interoperability.md)

A high-performance vector type optimized for numerical operations and large datasets.

## Overview

`$VECTOR` is a specialized data type designed for efficient numerical computations and large-scale data processing. It provides optimized operations for mathematical operations and memory management.

## Basic Operations

### Creation
```grapa
vec = #[1, 2, 3, 4, 5]#;  /* Create vector directly */
vec = [1, 2, 3, 4, 5].vector();  /* Convert array to vector */
vec = $vector([1, 2, 3, 4, 5]);  /* Explicit vector creation */

/* Create from CSV with headers */
data = $vector("Name,Age,City\nAlice,25,NYC\nBob,30,LA");
/* Result: 2D vector with column headers preserved */
```

### Function Application

Vectors support function application using operators, similar to arrays:

```grapa
/* Function application using operators */
#[1, 2, 3, 4]# * [op(x){x*2}]   /* [2, 4, 6, 8] */
#[1, 2, 3]# + [op(x){x+10}]     /* [11, 12, 13] */

/* Complex functions */
data = #[1, 2, 3, 4, 5]#;
squares = data * [op(x){x*x}];   /* [1, 4, 9, 16, 25] */

/* Conditional operations */
values = #[1, -2, 3, -4, 5]#;
abs_vals = values * [op(x){x < 0 ? -x : x}]; /* [1, 2, 3, 4, 5] */
```

> **Note:** Function application works seamlessly with vectors, allowing custom operations to be applied element-wise. See [Array-Vector Interoperability](../advanced/array_vector_interoperability.md) for more details on how arrays and vectors work together.

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

## Header Row Support

Vectors provide sophisticated support for header rows in 2D data structures, making them ideal for working with CSV files and tabular data.

### Automatic Header Detection
```grapa
/* CSV data with headers - automatically detected */
sales_data = $vector("Product,Q1,Q2,Q3,Q4\nWidget,100,120,110,130\nGadget,80,90,85,95");

/* Headers are preserved and can be used for column access */
q1_sales = sales_data["Q1"];  /* Access by column name */
products = sales_data["Product"];  /* Get product names */
```

### Header-Aware Operations
```grapa
/* Create vector with explicit headers */
data = $vector("Name,Score,Grade\nAlice,95,A\nBob,87,B\nCharlie,92,A");

/* Sort by column name */
sorted_by_score = data.sort("Score");  /* Sort by Score column */
sorted_by_name = data.sort("Name");    /* Sort by Name column */

/* Statistical operations respect headers */
avg_score = data["Score"].mean();      /* Average of Score column */
grade_counts = data["Grade"].unique();  /* Unique grades */
```

### CSV Import/Export with Headers
```grapa
/* Import CSV file with headers */
csv_data = $file().read("data.csv");
vector_data = $vector(csv_data);       /* Headers automatically detected */

/* Export vector with headers preserved */
csv_output = vector_data.csv();        /* Convert back to CSV with headers */
$file().write("output.csv", csv_output);
```

> **Note**: Vector header support has limited Unicode capabilities. It can handle UTF-8 BOM (Byte Order Mark) in CSV files, but header names are processed as raw bytes rather than Unicode characters. For full Unicode support in string operations, use Grapa's string functions instead.

### Working with Mixed Data Types
```grapa
/* Vector handles mixed column types */
mixed_data = $vector("ID,Name,Active,Score\n1,Alice,true,95.5\n2,Bob,false,87.2");

/* Access typed columns */
ids = mixed_data["ID"];           /* Numeric column */
names = mixed_data["Name"];       /* String column */  
active = mixed_data["Active"];    /* Boolean column */
scores = mixed_data["Score"];     /* Float column */

/* Filter using column names */
active_users = mixed_data.filter(op(row){row["Active"] == true});
high_scores = mixed_data.filter(op(row){row["Score"] > 90});
```

### Header Manipulation
```grapa
/* Check if vector has headers */
has_headers = data.headers().len() > 0;

/* Get all header names */
column_names = data.headers();     /* Returns array of header names */

/* Rename headers */
data.rename_headers(["ID", "Full_Name", "Is_Active", "Test_Score"]);

/* Add new column with header */
data["Grade"] = data["Score"].map(op(score){
    score >= 90 ? "A" : (score >= 80 ? "B" : "C");
});
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

