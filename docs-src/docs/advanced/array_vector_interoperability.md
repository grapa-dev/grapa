# Array and Vector Interoperability

Grapa provides seamless interoperability between `$LIST` and `$VECTOR` types, automatically converting between them to leverage the best capabilities of each type while maintaining intuitive user behavior.

## Overview

While `$LIST` and `$VECTOR` are distinct types in Grapa, they work together transparently:

- **`$LIST`**: General-purpose collections with flexible indexing and iteration
- **`$VECTOR`**: Mathematical structures optimized for linear algebra operations

The Grapa runtime automatically converts between these types when needed, allowing you to use mathematical operations on arrays and array-like operations on vectors without explicit conversion.

## Automatic Conversions

### Array to Vector Conversions

Arrays are automatically converted to vectors when:

1. **Mathematical Operations**: Arithmetic operators (`+`, `-`, `*`, `/`, `^`)
2. **Function Application**: Using operators with functions (e.g., `[1,2,3] * [op(x){x*2}]`)
3. **Matrix Operations**: Linear algebra methods like `.dot()`, `.solve()`, etc.

```grapa
// These all trigger ARRAY → VECTOR conversion internally
[1,2,3,4] * 2                    // Scalar multiplication
[1,2,3] + [4,5,6]               // Element-wise addition
[1,2,3,4] * [op(x){x*2}]        // Function application
[1,2,3].dot([4,5,6])            // Dot product
```

### Vector to Array Conversions

Vectors are converted back to arrays when:

1. **Method Results**: Many vector methods return array results for consistency
2. **Explicit Conversion**: Using the `.list()` method
3. **User Expectations**: Operations that users expect to return arrays

```grapa
vector = #[1,2,3,4]#            // Create vector
array = vector.list()          // Explicit conversion
reshaped = [1,2,3,4].reshape([2,2])  // Returns array, not vector
```

## Function Application

One of the most powerful features is function application using operators. **⚠️ CRITICAL**: You must use **function references**, not **function calls**.

### Function References vs Function Calls

```grapa
// Define a function
f = op(x) { x + 1; };

// ✅ CORRECT: Function reference - stores function for later execution
[1,2,3,4] * [f]                 // Result: [2,3,4,5]
[1,2,3,4] * [@f]                // Result: [2,3,4,5] (explicit reference)

// ❌ INCORRECT: Function call - evaluates immediately, returns null
[1,2,3,4] * [f(x)]              // Result: [0,0,0,0] (x is undefined)
[1,2,3,4] * [f()]               // Result: [0,0,0,0] (no parameters)
```

### Array Function Application

```grapa
// Apply function to each element
[1,2,3,4] * [op(x){x*2}]        // Result: [2,4,6,8]
[1,2,3,4] + [op(x){x+10}]       // Result: [11,12,13,14]
[2,4,6,8] / [op(x){x/2}]        // Result: [1,2,3,4]

// More complex functions
numbers = [1,2,3,4,5]
squares = numbers * [op(x){x*x}] // Result: [1,4,9,16,25]

// Conditional operations
data = [1,-2,3,-4,5]
abs_values = data * [op(x){x < 0 ? -x : x}] // Result: [1,2,3,4,5]

// Using named function references
double = op(x) { x * 2; };
doubled = [1,2,3,4] * [double]  // Result: [2,4,6,8]
```

### Vector Function Application

Vectors support the same function application as arrays:

```grapa
vector = #[1,2,3,4]#
result = vector * [op(x){x*3}]   // Each element multiplied by 3

// Using function references
triple = op(x) { x * 3; };
tripled = vector * [triple]      // Result: #[3,6,9,12]#
```

### Advanced Vector Operations

Vectors support additional advanced operations that work seamlessly with functions:

#### Custom Sorting
```grapa
// Custom comparison functions for flexible sorting
#[3,1,2]#.sort(op(a,b){a-b})     // Sort ascending
#[3,1,2]#.sort(op(a,b){b-a})     // Sort descending

// Complex sorting criteria
names = #["Alice", "bob", "Charlie"]#
names.sort(op(a,b){a.upper().cmp(b.upper())})  // Case-insensitive sort
```

#### Smart Vector Creation
```grapa
// Vector creation from various sources
"1,2,3".vector()                 // From comma-separated string
[1,2,3].vector()                 // From array
#[1,2,3]#                        // From existing vector

// Handles complex expressions during creation
$vector("1+2,3*4,5/2")          // Evaluates: #[3,12,2.5]#
```

#### Seamless Type Conversion
```grapa
// Automatic conversion maintains functionality
array_data = [1,2,3,4]
vector_result = array_data * [op(x){x*2}]  // Works transparently

// Explicit conversions when needed
vector = [1,2,3].vector()        // Array to vector
array = #[1,2,3]#.list()        // Vector to array
```

## Implementation Details for Users

### Performance Considerations

1. **Conversion Overhead**: While automatic, conversions have computational cost
2. **Memory Usage**: Temporary vectors/arrays are created during conversion
3. **Optimization**: Grapa optimizes common patterns to minimize conversions

### When to Use Each Type

**Use `$LIST` when:**
- General data manipulation and iteration
- Mixed data types
- Building collections dynamically
- Interfacing with JSON/XML data

**Use `$VECTOR` when:**
- Mathematical computations
- Linear algebra operations
- Large numerical datasets
- Performance-critical mathematical code

```grapa
// Array for general data
mixed_data = ["name", 42, 3.14, true]

// Vector for mathematics  
matrix = #[[1,2],[3,4]]#
result = matrix.inv()  // Matrix inverse
```

## Method Availability

### Methods That Trigger Conversions

| Method | Input Type | Conversion | Output Type |
|--------|------------|------------|-------------|
| `.reshape()` | `$LIST` | → `$VECTOR` → | `$LIST` |
| `.triu()` | `$LIST` | → `$VECTOR` → | `$LIST` |
| `.tril()` | `$LIST` | → `$VECTOR` → | `$LIST` |
| `.dot()` | `$LIST` | → `$VECTOR` → | `$LIST` |
| Arithmetic ops | `$LIST` | → `$VECTOR` → | `$LIST` |
| `.list()` | `$VECTOR` | → | `$LIST` |

### Native Methods

Some methods work directly without conversion:

```grapa
// Array methods (no conversion)
[1,2,3].len()           // Length
[1,2,3].map(op(x){x*2}) // Map function

// Vector methods (no conversion)  
#[1,2,3]#.shape()       // Shape information
#[1,2,3]#.norm()        // Vector norm
```

## Best Practices

1. **Let Grapa Handle Conversions**: Don't manually convert unless necessary
2. **Use Appropriate Types**: Start with the type that matches your use case
3. **Chain Operations**: Multiple operations on the same data type are more efficient
4. **Function Application**: Leverage operator-based function application for clean code

```grapa
// Good: Let Grapa handle conversions
data = [1,2,3,4,5]
processed = data * [op(x){x*2}] + [op(x){x+1}]

// Good: Use appropriate starting type
matrix = #[[1,2],[3,4]]#  // Start with vector for math
result = matrix.inv().det()

// Less efficient: Manual conversions
data = [1,2,3]
vector = $vector(data)    // Unnecessary explicit conversion
result = vector.norm()
```

## Error Handling

Conversion failures result in `$ERR` objects:

```grapa
// Invalid vector operations return errors
result = [1,2,3].inv()  // Error: not a square matrix
invalid = ["a","b"].dot([1,2])  // Error: non-numeric data
```

## See Also

- [Array Type Documentation](../type/array.md)
- [Vector Type Documentation](../type/vector.md)
- [Mathematical Operations](../math/linear_algebra.md)
- [Function Application Patterns](../patterns/functional_programming.md)
