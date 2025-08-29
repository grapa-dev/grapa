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
data = "Name,Age,City\nAlice,25,NYC\nBob,30,LA".vector();
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

### Vector Manipulation Methods
```grapa
vec = [1, 2, 3, 4, 5, 6].vector();

/* Element extraction */
left = vec.left(3);       /* Extract leftmost 3 elements: #[1,2,3]# */
right = vec.right(3);     /* Extract rightmost 3 elements: #[4,5,6]# */

/* Vector splitting */
parts = vec.split(2);     /* Split into 2 parts: [#[1,2,3]#, #[4,5,6]#] */
parts_axis1 = vec.split(2, 1);  /* Split along axis 1 */

/* Shape operations */
shape = vec.shape();      /* Get dimensions: [6] */
reshaped = vec.reshape([2, 3]);  /* Reshape to 2x3: #[[1,2,3],[4,5,6]]# */
transposed = vec.t();     /* Matrix transpose (for 2D vectors) */

/* Statistical operations (2D vectors only) */
total = vec.sum();        /* Sum all elements (2D vectors only) */
average = vec.mean();     /* Mean of all elements (2D vectors only) */
det_value = vec.det();    /* Determinant (for square matrices) */
rank_value = vec.rank();  /* Matrix rank */
cov_matrix = vec.cov();   /* Covariance matrix */

/* Matrix operations */
inverse = vec.inv();      /* Matrix inverse */
solution = vec.solve(b);  /* Solve linear system */
eigenvalues = vec.eig();  /* Eigenvalues */
upper_tri = vec.triu();   /* Upper triangular */
lower_tri = vec.tril();   /* Lower triangular */
dot_product = vec1.dot(vec2);  /* Dot product */
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

/* Statistical functions (2D vectors only) */
vec.sum();                /* Sum of all elements (2D vectors only) */
vec.mean();               /* Arithmetic mean (2D vectors only) */
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

> **Note**: Vector header support has excellent Unicode capabilities. It can handle UTF-8 BOM (Byte Order Mark) in CSV files, and header names with Unicode characters (including Cyrillic, accented characters, etc.) are fully preserved and accessible using the header extraction methods.
>
> **Encoding Support**: CSV to vector conversion supports UTF-8 encoding only. While UTF-16 BOMs are detected and stripped, the content must be UTF-8 encoded for proper processing. Files with UTF-16 content should be converted to UTF-8 before processing.
>
> **Note**: If you encounter garbled text in your vector data, it may indicate that your CSV file is encoded in UTF-16 or another non-UTF-8 encoding. Convert the file to UTF-8 encoding before processing.

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

### Header Extraction and Manipulation
```grapa
/* Extract header names from CSV vector */
keys = op(lst){lst.reduce(op(acc,x){if(x.type()==$LIST){acc += keys(x);}else{acc += 'x'.getname();}},[]);};

csv = "Name,Value\nAlice,100\nBob,200";
vec = csv.vector();
arr = vec.array();
first = arr[0];
headers = keys(first);  /* ["Name","Value"] */

/* Complete header extraction function */
extract_headers = op(csv_string) {
    keys = op(lst){lst.reduce(op(acc,x){if(x.type()==$LIST){acc += keys(x);}else{acc += 'x'.getname();}},[]);};
    vec = csv_string.vector();
    arr = vec.array();
    first = arr[0];
    keys(first);
};

/* Usage */
headers = extract_headers("Name,Value\nAlice,100\nBob,200");
/* Result: ["Name","Value"] */

/* Access data by header name */
first_row = arr[0];
name_value = first_row.Name;      /* "Alice" */
value_value = first_row.Value;    /* 100 */

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
empty_sum = empty_vec.sum();  /* Returns {"error":-1} */
empty_mean = empty_vec.mean(); /* Returns {"error":-1} */

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

### Dimensionality Requirements

**Important**: Some vector operations have specific dimensionality requirements:

#### **2D-Only Operations**
The following operations currently only work on 2-dimensional vectors (matrices):
- `.sum()` - Sum along specified axis
- `.mean()` - Mean along specified axis
- `.cov()` - Covariance matrix calculation

**1D vectors will return `{"error":-1}` for these operations.**

```grapa
/* 1D vector - operations fail */
vec_1d = #[1, 2, 3, 4, 5]#;
result = vec_1d.sum();  /* Returns {"error":-1} */

/* 2D vector - operations work */
vec_2d = [[1, 2, 3, 4, 5]].vector();
result = vec_2d.sum();  /* Returns #[15]# */

/* Alternative for 1D vectors */
vec_1d = #[1, 2, 3, 4, 5]#;
sum_manual = vec_1d.reduce(op(acc, x){acc + x}, 0);  /* Manual sum */
mean_manual = sum_manual / vec_1d.len();             /* Manual mean */
```

**Future Enhancement**: Support for 1D vector operations is planned to improve usability for common statistical calculations.

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

## Data Types and Usage Examples

### Working with Different Data Types

Grapa vectors can contain mixed data types, making them ideal for real-world data processing scenarios:

#### **Numeric Vectors**
```grapa
/* Integer vectors (2D required for sum/mean) */
int_vec = #[1, 2, 3, 4, 5]#;
/* Note: .sum() and .mean() require 2D vectors */
int_vec_2d = [[1, 2, 3, 4, 5]].vector();
sum = int_vec_2d.sum();                 /* #[15]# */
mean = int_vec_2d.mean();               /* #[3.0]# */

/* Float vectors with high precision */
precise_vec = #[1.123456789, 2.987654321, 3.141592653]#;
product = precise_vec.reduce(op(acc,x){acc*x}, 1);  /* Exact precision maintained */

/* Large number calculations */
large_nums = #[123456789012345678901234567890, 987654321098765432109876543210]#;
result = large_nums[0] * large_nums[1];  /* No overflow - unlimited precision */
```

#### **String Vectors**
```grapa
/* Text processing */
names = #["Alice", "Bob", "Charlie", "Diana"]#;
upper_names = names.map(op(x){x.upper()});  /* ["ALICE", "BOB", "CHARLIE", "DIANA"] */

/* String concatenation */
greetings = #["Hello", "Hi", "Hey"]#;
people = #["Alice", "Bob", "Charlie"]#;
messages = greetings + " " + people;  /* ["Hello Alice", "Hi Bob", "Hey Charlie"] */

/* String filtering */
long_names = names.filter(op(x){x.len() > 4});  /* ["Alice", "Charlie", "Diana"] */
```

#### **Mixed Type Vectors**
```grapa
/* Real-world data with mixed types */
mixed_data = #[1, "hello", 3.14, true, [1,2,3]]#;

/* Type-aware processing */
numbers = mixed_data.filter(op(x){x.type() == $INT || x.type() == $FLOAT});
strings = mixed_data.filter(op(x){x.type() == $STR});
booleans = mixed_data.filter(op(x){x.type() == $BOOL});
```

#### **Boolean Vectors**
```grapa
/* Logical operations */
flags = #[true, false, true, true, false]#;
all_true = flags.reduce(op(acc,x){acc && x}, true);    /* false */
any_true = flags.reduce(op(acc,x){acc || x}, false);   /* true */
true_count = flags.filter(op(x){x}).len();             /* 3 */
```

### CSV Data Processing Examples

#### **Basic CSV Import**
```grapa
/* Simple CSV with headers */
csv_data = "Name,Age,City,Active\nAlice,25,NYC,true\nBob,30,LA,false\nCharlie,35,Chicago,true";
data = csv_data.vector();

/* Access by column name */
names = data["Name"];        /* ["Alice", "Bob", "Charlie"] */
ages = data["Age"];          /* [25, 30, 35] */
cities = data["City"];       /* ["NYC", "LA", "Chicago"] */
active = data["Active"];     /* [true, false, true] */
```

#### **CSV with Mixed Data Types**
```grapa
/* Complex CSV with various data types */
complex_csv = "ID,Name,Score,LastLogin,Preferences\n1,Alice,95.5,2024-01-15,{theme:dark}\n2,Bob,87.2,2024-01-10,{theme:light}\n3,Charlie,92.8,2024-01-12,{theme:auto}";
data = complex_csv.vector();

/* Extract and process different column types */
ids = data["ID"];                    /* [1, 2, 3] - integers */
names = data["Name"];                /* ["Alice", "Bob", "Charlie"] - strings */
scores = data["Score"];              /* [95.5, 87.2, 92.8] - floats */
logins = data["LastLogin"];          /* ["2024-01-15", "2024-01-10", "2024-01-12"] - dates */
prefs = data["Preferences"];         /* [{theme:dark}, {theme:light}, {theme:auto}] - objects */
```

#### **CSV Data Analysis**
```grapa
/* Statistical analysis of CSV data */
sales_data = "Product,Q1,Q2,Q3,Q4\nWidget,100,120,110,130\nGadget,80,90,85,95\nTool,150,160,155,165";
data = sales_data.vector();

/* Calculate totals by product */
products = data["Product"];
q1_sales = data["Q1"];
q2_sales = data["Q2"];
q3_sales = data["Q3"];
q4_sales = data["Q4"];

/* Total sales for each product */
total_sales = q1_sales + q2_sales + q3_sales + q4_sales;  /* [460, 350, 630] */

/* Average quarterly sales */
avg_q1 = q1_sales.mean();  /* 110.0 */
avg_q2 = q2_sales.mean();  /* 123.333... */
avg_q3 = q3_sales.mean();  /* 116.666... */
avg_q4 = q4_sales.mean();  /* 130.0 */

/* Best performing product */
max_sales = total_sales.max();  /* 630 */
best_product = products[total_sales.find(max_sales)];  /* "Tool" */
```

### Matrix Operations Examples

#### **2D Matrix Creation**
```grapa
/* Create 2D matrix */
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]].vector();

/* Matrix operations */
transposed = matrix.t();           /* Transpose matrix */
determinant = matrix.det();        /* Calculate determinant */
inverse = matrix.inv();           /* Matrix inverse */

/* Row and column operations */
first_row = matrix[0];            /* [1, 2, 3] */
first_col = matrix.column(0);     /* [1, 4, 7] */
```

#### **Matrix with Headers**
```grapa
/* Create matrix with row and column headers */
header_matrix = "Row,A,B,C\nX,1,2,3\nY,4,5,6\nZ,7,8,9";
matrix = header_matrix.vector();

/* Access by header names */
row_headers = matrix.row_headers();  /* ["X", "Y", "Z"] */
col_headers = matrix.column_headers();  /* ["A", "B", "C"] */

/* Access specific cells by header */
cell_a_x = matrix["A"]["X"];  /* 1 */
cell_b_y = matrix["B"]["Y"];  /* 5 */
```

### Advanced Usage Examples

#### **Conditional Vector Operations**
```grapa
/* Apply conditional logic to vectors */
temperatures = #[15, 25, 30, 5, 35, 10]#;
weather = temperatures.map(op(t){
    if (t < 10) "Cold"
    else if (t < 20) "Cool"
    else if (t < 30) "Warm"
    else "Hot"
});  /* ["Cool", "Warm", "Hot", "Cold", "Hot", "Cold"] */
```

#### **Vector Aggregation**
```grapa
/* Group and aggregate data */
sales = #[100, 150, 200, 120, 180, 90]#;
categories = #["A", "B", "A", "C", "B", "A"]#;

/* Group by category and sum */
grouped = {};
for (i = 0; i < sales.len(); i++) {
    cat = categories[i];
    sale = sales[i];
    if (grouped.has(cat)) {
        grouped[cat] += sale;
    } else {
        grouped[cat] = sale;
    }
}
/* Result: {A: 390, B: 330, C: 120} */
```

#### **Vector Validation**
```grapa
/* Validate vector data */
grades = #[85, 92, 78, 105, 88, 95]#;

/* Check for valid grade range (0-100) */
valid_grades = grades.filter(op(g){g >= 0 && g <= 100});  /* [85, 92, 78, 88, 95] */
invalid_grades = grades.filter(op(g){g < 0 || g > 100});  /* [105] */

/* Calculate statistics only for valid grades */
avg_grade = valid_grades.mean();  /* 87.6 */
```

### Performance Guidelines

#### **Best Practices**
1. **Use Appropriate Data Types**
   - Use `$INT` for whole numbers
   - Use `$FLOAT` for decimal numbers
   - Use `$STR` for text data
   - Use `$BOOL` for true/false values

2. **Efficient Operations**
   - Use vector methods instead of loops when possible
   - Batch operations on multiple vectors
   - Avoid repeated type conversions

3. **Memory Management**
   - Pre-allocate vectors for large datasets
   - Monitor memory usage for very large vectors
   - Use appropriate precision for your needs

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

