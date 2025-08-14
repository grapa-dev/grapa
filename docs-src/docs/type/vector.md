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

### Broadcasting
```grapa
/* Automatic broadcasting for different-sized vectors */
vec1 = [1, 2, 3];
vec2 = [10];
result = vec1 + vec2;     /* [11, 12, 13] - vec2 broadcast to match vec1 */
```

## Integration with Other Types

### Conversion
```grapa
/* Convert between types */
arr = [1, 2, 3];
vec = arr.to_vector();    /* Convert array to vector */
arr2 = vec.to_array();    /* Convert vector back to array */
```

### Mixed Operations
```grapa
/* Operations with other data types */
vec = [1, 2, 3];
list = {a:10, b:20};
result = vec + list.values(); /* Vector + array values */
```

## Performance Considerations

- **Memory Efficiency**: Vectors use optimized memory layouts for numerical data
- **Parallel Processing**: Many operations automatically utilize multiple CPU cores
- **Cache Optimization**: Data layout optimized for CPU cache access patterns
- **Large Dataset Handling**: Efficient algorithms for vectors with millions of elements

## See also
- [$ARRAY object](array.md)
- [$LIST object](list.md)
- [Assignment Operators](../operators/assignment.md)
- [Mathematical Operations](../operators/math.md)

