---
tags:
  - user
  - highlevel
---
# $ARRAY
References:
- [$GOBJ object](list.md)
- [$VECTOR object](vector.md)
- [Array-Vector Interoperability](../advanced/array_vector_interoperability.md)

**`$ARRAY`** = `[]` syntax (positional, no names, indexed by position)

A positional array that stores values without named keys. Elements are accessed by their position (index) rather than by name. This follows traditional C-style array terminology where arrays are indexed collections.

### $ARRAY vs $GOBJ Comparison

| Feature | $ARRAY (`[]`) | $GOBJ (`{}`) |
|---------|---------------|--------------|
| **Syntax** | `[1, 2, 3]` | `{a:1, b:2, c:3}` |
| **Access** | `array[0]` | `list.a` or `list["a"]` |
| **Type** | Positional | Associative |
| **Keys** | Numeric indices | Named keys |
| **Order** | Position-based | Key-based |

**Note:** This differs from some other languages where `[]` is called a "list" and `{}` is called a "dictionary" or "object". Grapa follows traditional C terminology.

Supports an offset search if the contents of the array are $ID or $STR or $ARRAY values.
```
> months = ["JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"];
> months["MAR"];
2
> months["FFF"];
{"err":-1}
> months["FFF"].iferr(-1);
-1
```

If the elements are $ARRAY type, the first item of the array is used for the match.
```
> x = [["a",496],["b",964],["c",221]];
> x."b";
1
> x[x."b"];
["b",964]
> x.g
{"err":-1}
```

There isn't currently built in support for searching for an $ARRAY or $GOBJ item within an $ARRAY. Currently, this would require an itterator to scan and compare, or the use of a hash of the contents as a key.

### Mathematical Operations

Arrays automatically convert to vectors for mathematical operations, then convert back to arrays:

```grapa
/* Arithmetic operations (uses vector math internally) */
[1, 2, 3] + [4, 5, 6]         /* [5, 7, 9] */
[1, 2, 3, 4] * 2              /* [2, 4, 6, 8] */
[2, 4, 6] / 2                 /* [1, 2, 3] */

/* Function application using operators */
[1, 2, 3, 4] * [op(x){x*2}]   /* [2, 4, 6, 8] */
[1, 2, 3] + [op(x){x+10}]     /* [11, 12, 13] */

/* Matrix operations (converted to vectors internally) */
[1, 2, 3].dot([4, 5, 6])      /* 32 (dot product) */
[[1,2],[3,4]].reshape([4,1])  /* [[1],[2],[3],[4]] */
```

> **Note:** See [Array-Vector Interoperability](../advanced/array_vector_interoperability.md) for detailed information about automatic conversions.

### Array Operations

#### Element Access Methods

Arrays support both bracket notation and method-based access:

```grapa
arr = [1, 2, 3, 4, 5];

/* Bracket notation (direct access) */
first = arr[0];           /* 1 */
last = arr[-1];           /* 5 */

/* Method-based access */
first = arr.get(0);       /* 1 */
last = arr.get(-1);       /* 5 */

/* Field access methods */
first = arr.getfield(0);  /* 1 */
last = arr.getfield(-1);  /* 5 */
```

#### Assignment Operations (`=`)
```grapa
/* Direct element assignment */
arr = [3, 6, "hi"];
arr[1] = "by";            /* [3,"by","hi"] */

/* Assignment by negative index */
arr[-2] = "good";         /* [3,"good","hi"] */

/* Method-based assignment */
arr.set(1, "by");         /* [3,"by","hi"] */
arr.set(-2, "good");      /* [3,"good","hi"] */

/* Field assignment methods */
arr.setfield(1, "by");    /* [3,"by","hi"] */
arr.setfield(-2, "good"); /* [3,"good","hi"] */

/* Compound assignment on accessed elements */
arr[-2] += "dee";         /* [3,"gooddee","hi"] */
arr[0] += 8;              /* [11,"gooddee","hi"] */
```

#### Addition Operations (`+=`)
```grapa
/* Add single element */
arr = [1, 2, 3, 4, 5];
arr += 6;                 /* [1,2,3,4,5,6] */

/* Add multiple elements */
arr += [7, 8, 9];         /* [1,2,3,4,5,6,7,8,9] */

/* Insert at specific position */
arr += 10 arr[0];         /* [10,1,2,3,4,5,6,7,8,9] */
arr += 20 arr[2];         /* [10,1,20,2,3,4,5,6,7,8,9] */
```

#### Concatenation Operations (`++=`)
```grapa
/* Concatenate two arrays */
arr1 = [1, 2, 3];
arr2 = [4, 5, 6];
arr1 ++= arr2;            /* [1,2,3,4,5,6] */
```

#### Removal Operations (`-=`)
```grapa
arr = [1, 2, 3, 4, 5];

/* Remove by index */
arr -= arr[2];            /* [1,2,4,5] */

/* Remove by negative index */
arr -= arr[-1];           /* [1,2,4] */

/* Remove first element */
arr -= arr[0];            /* [2,4] */
```

#### Unsupported Operations
```grapa
arr = [1, 2, 3, 4, 5];

/* These do NOT work: */
arr -= 3;                 /* No effect - value-based removal not supported */
arr -= "2";               /* No effect - direct string removal not supported */
```

### Concatenation and Extension
```grapa
arr = [1, 2, 3];

/* Add elements */
arr += 4;                                 /* [1, 2, 3, 4] */

/* Extend with multiple elements */
arr ++= 5, 6;                             /* [1, 2, 3, 4, 5, 6] */

/* Insert at specific position */
arr ++= 7, 2;                             /* Insert at position 2 */

/* Remove elements */
arr -= 3;                                 /* Remove element with value 3 */
```

### Array Slicing

Arrays support slicing operations for extracting portions of elements:

#### Left Slicing (.left())
```grapa
arr = [1, 2, 3, 4, 5];

arr.left(3);                              /* [1, 2, 3] */
arr.left(-2);                             /* [1, 2, 3] */
```

#### Right Slicing (.right())
```grapa
arr = [1, 2, 3, 4, 5];

arr.right(3);                             /* [3, 4, 5] */
arr.right(-2);                            /* [4, 5] */
```

#### Middle Slicing (.mid())
```grapa
arr = [1, 2, 3, 4, 5];

arr.mid(1, 3);                            /* [2, 3, 4] */
arr.mid(-3, 2);                           /* [3, 4] */
```

> **Note:** Slicing methods work with any array size. For empty arrays, slicing returns an empty array. Negative indices count from the end.

### Array Rotation

Arrays support left and right rotation operations for reordering elements:

#### Left Rotation (.lrot())
```grapa
arr = [1, 2, 3, 4, 5];

arr.lrot();                              /* [2, 3, 4, 5, 1] */
arr.lrot(2);                             /* [3, 4, 5, 1, 2] */
arr.lrot(0);                             /* [1, 2, 3, 4, 5] (no change) */
```

#### Right Rotation (.rrot())
```grapa
arr = [1, 2, 3, 4, 5];

arr.rrot();                              /* [5, 1, 2, 3, 4] */
arr.rrot(2);                             /* [4, 5, 1, 2, 3] */
arr.rrot(0);                             /* [1, 2, 3, 4, 5] (no change) */
```

> **Note:** Rotation methods work with any array size. For empty arrays or single-element arrays, rotation has no effect.

### Advanced Element Finding (.findall())

The `.findall()` method provides **enterprise-grade querying capabilities** for ARRAY structures with support for complex patterns and logical operations.

#### Basic Queries
```grapa
arr = [{name:"Alice"}, {name:"Bob"}, {name:"Charlie"}];

/* Find objects with specific properties */
arr.findall({has:{name:"name"}})          /* Returns: [{"name":"Alice"},{"name":"Bob"},{"name":"Charlie"}] */

/* Find objects with specific property values */
arr.findall({has:{name:"name", value:"Bob"}}) /* Returns: [{"name":"Bob"}] */

/* Find by value only (for simple arrays) */
simple_arr = [1, 2, 3, 4, 5];
simple_arr.findall({has:{value:3}})       /* Returns: [] (not supported for simple values) */
```

#### Complex Nested Queries
```grapa
complex_arr = [{user:{name:"Alice", role:"admin"}}, {user:{name:"Bob", role:"user"}}];

/* Find with nested criteria */
complex_arr.findall({has:{name:"user", has:{name:"role", value:"admin"}}}) /* Returns: [{"user":{"name":"Alice","role":"admin"}}] */

/* Find with multiple nested criteria */
complex_arr.findall({has:{name:"user", has:{name:"name", value:"Bob"}}}) /* Returns: [{"user":{"name":"Bob","role":"user"}}] */

/* Find with deep nesting */
deep_arr = [{level1:{level2:{level3:{value:"deep"}}}}];
deep_arr.findall({has:{name:"level1", has:{name:"level2", has:{name:"level3", has:{name:"value", value:"deep"}}}}}) /* Returns: [{"level1":{"level2":{"level3":{"value":"deep"}}}}] */
```

#### Logical Operations
```grapa
arr = [{user:{name:"Alice", role:"admin"}}, {guest:{name:"Bob", role:"user"}}];

/* AND logic */
arr.findall({and:[{has:{name:"user"}}, {has:{name:"user", has:{name:"role", value:"admin"}}}]}) /* Returns: [{"user":{"name":"Alice","role":"admin"}}] */

/* OR logic */
arr.findall({or:[{has:{name:"user"}}, {has:{name:"guest"}}]}) /* Returns: [{"user":{"name":"Alice","role":"admin"}},{"guest":{"name":"Bob","role":"user"}}] */

/* NAND logic */
arr.findall({nand:[{has:{name:"user"}}, {has:{name:"user", has:{name:"role", value:"user"}}}]}) /* Returns: [{"user":{"name":"Alice","role":"admin"}},{"guest":{"name":"Bob","role":"user"}}] */
```

#### Working with Results
```grapa
arr = [{user:{name:"Alice", age:30}}, {admin:{name:"Bob", age:25}}];

/* Get all results */
results = arr.findall({has:{name:"user"}});
results.len()                              /* Returns: 1 */

/* Access individual results */
first = results[0];                        /* Returns: {"user":{"name":"Alice","age":30}} */

/* Access nested properties of results */
first.user.name                            /* Returns: "Alice" */
first.user.age                             /* Returns: 30 */
```

> **Parallelism Note:**
> Array operations like `.map()` and `.filter()` are parallel by default and hardened for ETL/data processing workloads. Context objects are passed by reference for enhanced performance, but **avoid mutating context objects** as this causes race conditions. Use `.map()` for pure computation and `.reduce()` for aggregation instead.

## Error Handling and Boundary Conditions

### **Array Access Behavior**

Arrays have **flexible boundary behavior** that differs from vectors:

```grapa
arr = [1, 2, 3];

/* Valid access - returns elements */
arr.get(0);               /* 1 */
arr.get(-1);              /* 3 */
arr.get(2);               /* 3 */

/* Boundary behavior - APPENDS instead of error */
arr.get(3);               /* Returns null (out of bounds) */
arr.set(3, 99);           /* APPENDS: [1, 2, 3, 99] */
arr.set(5, 88);           /* APPENDS with gaps: [1, 2, 3, 99, null, 88] */

/* Method-based access follows same rules */
arr.getfield(3);          /* Returns null (out of bounds) */
arr.setfield(3, 77);      /* APPENDS: [1, 2, 3, 99, null, 88, 77] */

/* IMPORTANT: .set()/.setfield() only append at the end */
/* For insertion at specific positions, use += and ++= operators */
arr = [1, 2, 3];
arr += 99 arr[1];         /* INSERT at position 1: [1, 99, 2, 3] */
arr ++= [88, 77] arr[0];  /* INSERT multiple at position 0: [88, 77, 1, 99, 2, 3] */
```

### **Key Differences from Vectors**

| Behavior | `$ARRAY` | `$VECTOR` |
|----------|----------|-----------|
| **Out-of-bounds Get** | Returns `null` | Returns `$ERR` |
| **Out-of-bounds Set** | **APPENDS** element | Returns `$ERR` |
| **Boundary Policy** | Flexible (grows) | Strict (fixed size) |
| **Use Case** | Dynamic collections | Mathematical structures |

### **Append vs. Error Behavior**

```grapa
arr = [1, 2, 3];

/* These APPEND (arrays grow dynamically) */
arr.set(3, 99);           /* [1, 2, 3, 99] */
arr.set(5, 88);           /* [1, 2, 3, 99, null, 88] */
arr.setfield(6, 77);      /* [1, 2, 3, 99, null, 88, 77] */

/* These return null (out of bounds get) */
result = arr.get(10);     /* null */
result = arr.getfield(10); /* null */

/* Check for out-of-bounds access */
if (arr.get(10) == null) {
    /* Handle out-of-bounds access */
}
```

### **Insertion vs. Appending**

**Important Distinction:**
- **`.set()/.setfield()`**: Only append at the end (or fill gaps)
- **`+=` and `++=` operators**: Insert at specific positions

```grapa
arr = [1, 2, 3];

/* .set() only appends at the end */
arr.set(3, 99);           /* [1, 2, 3, 99] - appends at end */
arr.set(5, 88);           /* [1, 2, 3, 99, null, 88] - fills gap */

/* += and ++= insert at specific positions */
arr = [1, 2, 3];
arr += 99 arr[1];         /* [1, 99, 2, 3] - inserts at position 1 */
arr ++= [88, 77] arr[0];  /* [88, 77, 1, 99, 2, 3] - inserts at position 0 */
```

### **Error Handling Patterns**

```grapa
arr = [1, 2, 3];

/* Safe access with null checking */
result = arr.get(5);
if (result == null) {
    result = "Index out of bounds";
}

/* Safe setting (always works - appends if needed) */
arr.set(5, 99);           /* Always succeeds, appends if needed */

/* Check array length before access */
if (index < arr.len()) {
    value = arr.get(index);
} else {
    value = "Index too large";
}
```

### **Comparison with Other Data Types**

```grapa
/* Array behavior (flexible) */
arr = [1, 2, 3];
arr.set(5, 99);           /* APPENDS: [1, 2, 3, null, null, 99] */

/* Vector behavior (strict) */
vec = [1, 2, 3].vector();
result = vec.set(5, 99);  /* Returns $ERR - strict bounds */

/* List behavior (flexible) */
list = {a:1, b:2, c:3};
list.set("d", 99);        /* APPENDS: {a:1, b:2, c:3, d:99} */
```

### **Best Practices**

1. **Use arrays for dynamic collections** where you need flexible sizing
2. **Use vectors for mathematical operations** where fixed dimensions matter
3. **Check for null returns** when accessing potentially out-of-bounds indices
4. **Use `+=` and `++=` for insertion** at specific positions
5. **Use `.set()/.setfield()` for appending** at the end or filling gaps
6. **Use `.len()` to check bounds** before accessing if you need strict behavior


