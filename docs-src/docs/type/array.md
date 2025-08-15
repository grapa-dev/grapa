---
tags:
  - user
  - highlevel
---
# $ARRAY
References:
- [$LIST object](list.md)
- [$VECTOR object](vector.md)

**`$ARRAY`** = `[]` syntax (positional, no names, indexed by position)

A positional array that stores values without named keys. Elements are accessed by their position (index) rather than by name. This follows traditional C-style array terminology where arrays are indexed collections.

### $ARRAY vs $LIST Comparison

| Feature | $ARRAY (`[]`) | $LIST (`{}`) |
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

There isn't currently built in support for searching for an $ARRAY or $LIST item within an $ARRAY. Currently, this would require an itterator to scan and compare, or the use of a hash of the contents as a key.

### Array Operations

#### Assignment Operations (`=`)
```grapa
/* Direct element assignment */
arr = [3, 6, "hi"];
arr[1] = "by";            /* [3,"by","hi"] */

/* Assignment by negative index */
arr[-2] = "good";         /* [3,"good","hi"] */

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
> Array operations like `.map()` and `.filter()` are parallel by default and hardened for ETL/data processing workloads.

### List Comprehension

Grapa supports native list comprehension for creating arrays from expressions evaluated over iterables:

```grapa
/* Basic list comprehension */
[x for x in 3]           /* [0, 1, 2] */

/* Complex expressions */
[x*2 for x in 3]         /* [0, 2, 4] */
[x+1 for x in 5]         /* [1, 2, 3, 4, 5] */

/* Conditional filtering */
[x for x in 5 if x > 1]  /* [2, 3, 4] */
[x for x in 10 if x % 2 == 0]  /* [0, 2, 4, 6, 8] */

/* Collection iteration */
arr = [10, 20, 30];
[x*2 for x in arr]       /* [20, 40, 60] */

/* String iteration */
[x for x in "abc"]       /* ["a", "b", "c"] */
[x.upper() for x in "abc"]  /* ["A", "B", "C"] */
```

**Key Features:**
- **Numeric ranges**: `[x for x in 5]` iterates from 0 to 4
- **Collections**: Works with arrays, lists, and strings
- **Complex expressions**: Any valid Grapa expression can be used
- **Conditional filtering**: Optional `if` clause for filtering
- **Variable scoping**: Loop variables don't affect outer scope
