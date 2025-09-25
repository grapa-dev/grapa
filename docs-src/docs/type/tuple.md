# $TUPLE
References:
- [$LIST object](array.md)
- [$GOBJ object](gobj.md)

A $TUPLE is an immutable sequence that extends $LIST functionality. Unlike arrays, tuples cannot be modified after creation, making them ideal for representing fixed collections of data.

## Overview

Tuples provide the same access patterns as arrays but with immutability guarantees. They're useful for:
- **Fixed data structures** - Collections that shouldn't change
- **Return values** - Multiple values from functions
- **Data integrity** - Preventing accidental modifications
- **Performance** - Immutable structures can be optimized

## Creation

### Basic Tuple Creation
```grapa
/* Create tuple with parentheses */
point = (10, 20);           /* 2D point */
rgb = (255, 128, 64);       /* RGB color */
person = ("Alice", 30, "Engineer");  /* Person data */
```

### Tuple from Array
```grapa
/* Convert array to tuple */
arr = [1, 2, 3, 4, 5];
tup = (arr);                /* Convert to tuple */
```

## Access Patterns

### Index Access
```grapa
point = (10, 20);
x = point[0];               /* 10 */
y = point[1];               /* 20 */
```

### Negative Indexing
```grapa
rgb = (255, 128, 64);
red = rgb[-3];              /* 255 */
green = rgb[-2];            /* 128 */
blue = rgb[-1];             /* 64 */
```

### Length
```grapa
point = (10, 20);
len = point.len();          /* 2 */
```

## Operations

### Supported Operations
```grapa
point = (10, 20);

/* Access operations (same as array) */
x = point[0];               /* 10 */
len = point.len();          /* 2 */

/* Search operations */
index = point.search(20);   /* Find element position */
```

### Unsupported Operations
```grapa
point = (10, 20);

/* These do NOT work (immutability) */
point[0] = 15;              /* Error - cannot modify tuple */
point += 30;                /* Error - cannot add elements */
point -= point[0];          /* Error - cannot remove elements */
```

## Use Cases

### Function Return Values
```grapa
/* Return multiple values */
get_coordinates = op() {
    (10, 20);               /* Return tuple */
};

coords = get_coordinates(); /* (10, 20) */
x = coords[0];              /* 10 */
y = coords[1];              /* 20 */
```

### Fixed Data Structures
```grapa
/* Configuration data */
config = (8080, "localhost", true);

port = config[0];           /* 8080 */
host = config[1];           /* "localhost" */
debug = config[2];          /* true */
```

### Data Integrity
```grapa
/* Prevent accidental modification */
user_data = ("Alice", 30, "Engineer");

/* This would cause an error */
/* user_data[1] = 31; */    /* Cannot modify tuple */

/* Create new tuple instead */
updated_user = (user_data[0], 31, user_data[2]);
```

## Performance Considerations

### Advantages
- **Memory efficiency** - Immutable structures can be shared
- **Thread safety** - No modification means no race conditions
- **Optimization opportunities** - Compiler can optimize immutable data

### Trade-offs
- **No in-place modification** - Must create new tuples for changes
- **Memory allocation** - Creating new tuples requires memory allocation
- **Use case specific** - Not suitable for frequently changing data:

## Comparison with Other Types

| Feature | $TUPLE | $LIST | $GOBJ |
|---------|--------|--------|-------|
| **Mutability** | ❌ Immutable | ✅ Mutable | ✅ Mutable |
| **Index Access** | ✅ Yes | ✅ Yes | ✅ Yes |
| **Key-Value** | ❌ No | ❌ No | ✅ Yes |
| **Modification** | ❌ No | ✅ Yes | ✅ Yes |
| **Performance** | ✅ High | ✅ High | ⚠️ Variable |

## Best Practices

### When to Use Tuples
- **Fixed collections** - Data that shouldn't change
- **Return values** - Multiple values from functions
- **Configuration** - Settings and parameters
- **Data integrity** - Preventing accidental modifications

### When to Use Arrays Instead
- **Frequently changing data** - Collections that need modification
- **Dynamic collections** - Growing or shrinking data
- **In-place operations** - Operations that modify existing data

### When to Use Lists Instead
- **Key-value data** - Named fields and properties
- **JSON-like structures** - Object-oriented data
- **Named access** - Access by property names

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)


