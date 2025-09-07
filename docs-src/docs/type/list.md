# $LIST
References:

**`$LIST`** = `{}` syntax (associative, with names/keys)

An associative list that stores key-value pairs. Elements are accessed by their names (keys) rather than by position. This follows traditional terminology where lists are associative collections with named entries.

### $LIST vs $ARRAY Comparison

| Feature | $LIST (`{}`) | $ARRAY (`[]`) |
|---------|--------------|---------------|
| **Syntax** | `{a:1, b:2, c:3}` | `[1, 2, 3]` |
| **Access** | `list.a` or `list["a"]` | `array[0]` |
| **Type** | Associative | Positional |
| **Keys** | Named keys | Numeric indices |
| **Order** | Key-based | Position-based |

**Note:** This differs from some other languages where `[]` is called a "list" and `{}` is called a "dictionary" or "object". Grapa follows traditional C terminology.

## When to Use $LIST vs Other Data Types

### **Use `{}` (Linked List) When:**
- **Small to medium datasets** (< 100-500 items typically)
- **Frequent modifications** (insertions, deletions, updates)
- **In-memory operations** only
- **Simple key-value storage** without complex queries
- **Configuration data, user preferences, cache data**

### **Use `{}.table()` (In-Memory BTree) When:**
- **Large datasets** (> 100-500 items) where memory efficiency matters
- **Range queries** or ordered data access needed
- **Complex queries** across multiple fields
- **Memory efficiency at scale** is important
- **Still in-memory** but need better performance for large datasets

### **Use `$file()` (Persistent Storage) When:**
- **Data persistence** is required (survives program restarts)
- **Very large datasets** that don't fit in memory
- **Disk-based storage** with BTree indexing
- **Long-term data storage** and retrieval
- **File system integration** needed

### **Performance Characteristics:**
- **`{}` (Linked List)**: Very fast for small datasets due to optimized double-linked list implementation
- **`{}.table()` (BTree)**: Slower than `{}` for small datasets but more memory-efficient for large datasets
- **`$file()` (Persistent)**: Disk I/O overhead but provides persistence and handles unlimited data size

**Key Insight**: Don't add indexing to `{}` - the overhead would likely make it slower, not faster. The double-linked list implementation is already highly optimized. 

Action | Example | Result
------------ | ------------- | -------------
Create | {a:1, b:2, c:3} | {"a":1,"b":2,"c":3}
Access | {a:1, b:2, c:3}.a</br>{a:1, b:2, c:3}[1]</br>{a:1, b:2, c:3}[-1] | 1</br>1</br>3
Assign | x = {a:1, b:2, c:3};</br>x.b = "x";</br>x["b"] = "by";</br>x[-2] = 1234; | </br></br>{"a":1,"b":"x","c":3}</br>{"a":1,"b":"by","c":3}</br>{"a":1,"b":1234,"c":3}
Append | x = {a:1, b:2};</br>x += (c:3);</br>x; | </br></br>{"a":1, "b":2, "c":3}
Append | x = {a:1, b:2};</br>x ++= {c:3,d:4};</br>x; | </br></br>{"a":1, "b":2, "c":3, "d":4}
Insert | x = {a:1, b:2};</br>x += (c:3) x[0];</br>x; | </br></br>{"c":3,"a":1,"b":2}
Count | {a:1, b:2, c:3}.len() | 3
Remove | x = {a:1, b:2, c:3};</br>x -= x[1];</br>x; | </br></br>{"a":1, "c":3}

### Advanced List Operations

#### Element Access Methods

Lists support both bracket notation and method-based access:

```grapa
list = {a:1, b:2, c:3};

/* Bracket notation (direct access) */
value = list["a"];        /* 1 */
value = list.a;           /* 1 */
value = list[0];          /* 1 (by index) */
value = list[-1];         /* 3 (by negative index) */

/* Method-based access */
value = list.get("a");    /* 1 */
value = list.get(0);      /* 1 (by index) */
value = list.get(-1);     /* 3 (by negative index) */

/* Field access methods */
value = list.getfield("a"); /* 1 */
value = list.getfield(0);   /* 1 (by index) */
value = list.getfield(-1);  /* 3 (by negative index) */
```

#### Assignment Operations (`=`)
```grapa
/* Direct property assignment */
list = {a:1, b:2, c:3};
list.b = "x";             /* {"a":1,"b":"x","c":3} */
list["b"] = "by";         /* {"a":1,"b":"by","c":3} */

/* Assignment by index */
list[1] = 55;             /* {"a":1,"b":55,"c":3} */

/* Assignment by negative index */
list[-2] = 1234;          /* {"a":1,"b":1234,"c":3} */

/* Method-based assignment */
list.set("b", "x");       /* {"a":1,"b":"x","c":3} */
list.set(1, 55);          /* {"a":1,"b":55,"c":3} */
list.set(-2, 1234);       /* {"a":1,"b":1234,"c":3} */

/* Field assignment methods */
list.setfield("b", "x");  /* {"a":1,"b":"x","c":3} */
list.setfield(1, 55);     /* {"a":1,"b":55,"c":3} */
list.setfield(-2, 1234);  /* {"a":1,"b":1234,"c":3} */

/* Compound assignment on accessed elements */
list.b += "dee";          /* {"a":1,"b":"bydee","c":3} */
list[0] += 8;             /* {"a":9,"b":"bydee","c":3} */
```

#### Addition Operations (`+=`)
```grapa
/* Add single key-value pair */
list = {a:1, b:2};
list += (c:3);           /* {"a":1,"b":2,"c":3} */

/* Add multiple key-value pairs */
list += {d:4, e:5};      /* {"a":1,"b":2,"c":3,"d":4,"e":5} */

/* Insert at specific position */
list += (f:6) list[0];   /* {"f":6,"a":1,"b":2,"c":3,"d":4,"e":5} */
```

#### Concatenation Operations (`++=`)
```grapa
/* Concatenate two lists */
list1 = {a:1, b:2};
list2 = {c:3, d:4};
list1 ++= list2;         /* {"a":1,"b":2,"c":3,"d":4} */
```

#### Removal Operations (`-=`)
```grapa
list = {a:1, b:2, c:3, d:4};

/* Remove by key reference */
list -= list.c;          /* {"a":1,"b":2,"d":4} */

/* Remove by key string */
list -= list["b"];       /* {"a":1,"d":4} */

/* Remove by positive index */
list -= list[0];         /* {"d":4} */

/* Remove by negative index */
list -= list[-1];        /* {} */
```

#### Unsupported Operations
```grapa
list = {a:1, b:2, c:3};

/* These do NOT work: */
list -= 2;               /* No effect - value-based removal not supported */
list -= "b";             /* No effect - direct string removal not supported */
list -= list.b;          /* Error - trying to remove value, not key */
```

### Concatenation and Extension
```grapa
list = {a:1, b:2, c:3};

/* Add elements */
list += {d:4};                            /* {a:1, b:2, c:3, d:4} */

/* Extend with multiple elements */
list ++= {e:5, f:6};                      /* {a:1, b:2, c:3, d:4, e:5, f:6} */

/* Insert at specific position */
list ++= {g:7}, 2;                        /* Insert at position 2 */

/* Remove elements */
list -= {b:2};                            /* Remove element with key "b" */
```

### List Slicing

Lists support slicing operations for extracting portions of key-value pairs:

#### Left Slicing (.left())
```grapa
list = {a:1, b:2, c:3, d:4, e:5};

list.left(3);                              /* {a:1, b:2, c:3} */
list.left(-2);                             /* {a:1, b:2, c:3} */
```

#### Right Slicing (.right())
```grapa
list = {a:1, b:2, c:3, d:4, e:5};

list.right(3);                             /* {c:3, d:4, e:5} */
list.right(-2);                            /* {d:4, e:5} */
```

#### Middle Slicing (.mid())
```grapa
list = {a:1, b:2, c:3, d:4, e:5};

list.mid(1, 3);                            /* {b:2, c:3, d:4} */
list.mid(-3, 2);                           /* {c:3, d:4} */
```

> **Note:** Slicing methods work with any list size. For empty lists, slicing returns an empty list. Negative indices count from the end.

### List Rotation

Lists support left and right rotation operations for reordering key-value pairs:

#### Left Rotation (.lrot())
```grapa
list = {a:1, b:2, c:3, d:4};

list.lrot();                              /* {b:2, c:3, d:4, a:1} */
list.lrot(2);                             /* {c:3, d:4, a:1, b:2} */
list.lrot(0);                             /* {a:1, b:2, c:3, d:4} (no change) */
```

#### Right Rotation (.rrot())
```grapa
list = {a:1, b:2, c:3, d:4};

list.rrot();                              /* {d:4, a:1, b:2, c:3} */
list.rrot(2);                             /* {c:3, d:4, a:1, b:2} */
list.rrot(0);                             /* {a:1, b:2, c:3, d:4} (no change) */
```

> **Note:** Rotation methods preserve all key-value pairs while reordering them. For empty lists or single-element lists, rotation has no effect.

### Advanced Element Finding (.findall())

The `.findall()` method provides **enterprise-grade querying capabilities** for LIST structures with support for complex patterns and logical operations.

#### Basic Queries
```grapa
data = {name:"Alice", age:30, city:"New York"};

/* Find by property existence */
data.findall({has:{name:"age"}})          /* Returns: {"data":{"name":"Alice","age":30}} */

/* Find by property value */
data.findall({has:{name:"name", value:"Alice"}}) /* Returns: {"data":{"name":"Alice","age":30}} */

/* Find by value only */
data.findall({has:{value:"Alice"}})       /* Returns: {"data":{"name":"Alice","age":30}} */
```

#### Complex Nested Queries
```grapa
nested = {user:{name:"Alice", age:30}, admin:{name:"Bob", age:25}};

/* Find nested objects */
nested.findall({has:{name:"user"}})       /* Returns: {"nested":{"user":{"name":"Alice","age":30},"admin":{"name":"Bob","age":25}}} */

/* Find with nested criteria */
nested.findall({has:{name:"user", has:{name:"name", value:"Alice"}}}) /* Returns: {"nested":{"user":{"name":"Alice","age":30},"admin":{"name":"Bob","age":25}}} */

/* Find with multiple nested criteria */
nested.findall({has:{name:"user", has:{name:"age", value:30}}}) /* Returns: {"nested":{"user":{"name":"Alice","age":30},"admin":{"name":"Bob","age":25}}} */
```

#### Logical Operations
```grapa
data = {user:{name:"Alice", role:"admin"}, guest:{name:"Bob", role:"user"}};

/* AND logic */
data.findall({and:[{has:{name:"user"}}, {has:{name:"user", has:{name:"role", value:"admin"}}}]}) /* Returns: {"data":{"user":{"name":"Alice","role":"admin"},"guest":{"name":"Bob","role":"user"}}} */

/* OR logic */
data.findall({or:[{has:{name:"user"}}, {has:{name:"guest"}}]}) /* Returns: {"data":{"user":{"name":"Alice","role":"admin"},"guest":{"name":"Bob","role":"user"}}} */

/* NAND logic */
data.findall({nand:[{has:{name:"user"}}, {has:{name:"user", has:{name:"role", value:"user"}}}]}) /* Returns: {"data":{"user":{"name":"Alice","role":"admin"},"guest":{"name":"Bob","role":"user"}}} */
```

#### Working with Results
```grapa
data = {user:{name:"Alice", age:30}, admin:{name:"Bob", age:25}};

/* Get all results */
results = data.findall({has:{name:"user"}});
results.len()                              /* Returns: 1 */

/* Access individual results */
first = results[0];                        /* Returns: {"user":{"name":"Alice","age":30}} */

/* Access nested properties of results */
first.user.name                            /* Returns: "Alice" */
first.user.age                             /* Returns: 30 */
```

## Error Handling and Boundary Conditions

### **List Access Behavior**

Lists have **flexible boundary behavior** similar to arrays:

```grapa
list = {a:1, b:2, c:3};

/* Valid access - returns elements */
list.get("a");             /* 1 */
list.get(0);               /* 1 */
list.get(-1);              /* 3 */

/* Boundary behavior - APPENDS instead of error */
list.get("d");             /* Returns null (key doesn't exist) */
list.set("d", 99);         /* APPENDS: {a:1, b:2, c:3, d:99} */
list.set("f", 88);         /* APPENDS: {a:1, b:2, c:3, d:99, f:88} */

/* Method-based access follows same rules */
list.getfield("d");        /* Returns null (key doesn't exist) */
list.setfield("d", 77);    /* APPENDS: {a:1, b:2, c:3, d:99, f:88, d:77} */

/* IMPORTANT: .set()/.setfield() only append at the end */
/* For insertion at specific positions, use += and ++= operators */
list = {a:1, b:2, c:3};
list += (d:99) list[1];    /* INSERT at position 1: {a:1, d:99, b:2, c:3} */
list ++= {e:88, f:77} list[0]; /* INSERT multiple at position 0: {e:88, f:77, a:1, d:99, b:2, c:3} */
```

### **Key Differences from Vectors**

| Behavior | `$LIST` | `$VECTOR` |
|----------|---------|-----------|
| **Out-of-bounds Get** | Returns `null` | Returns `$ERR` |
| **Out-of-bounds Set** | **APPENDS** element | Returns `$ERR` |
| **Boundary Policy** | Flexible (grows) | Strict (fixed size) |
| **Use Case** | Dynamic key-value storage | Mathematical structures |

### **Append vs. Error Behavior**

```grapa
list = {a:1, b:2, c:3};

/* These APPEND (lists grow dynamically) */
list.set("d", 99);         /* {a:1, b:2, c:3, d:99} */
list.set("f", 88);         /* {a:1, b:2, c:3, d:99, f:88} */
list.setfield("g", 77);    /* {a:1, b:2, c:3, d:99, f:88, g:77} */

/* These return null (key doesn't exist) */
result = list.get("z");    /* null */
result = list.getfield("z"); /* null */

/* Check for missing keys */
if (list.get("z") == null) {
    /* Handle missing key */
}
```

### **Insertion vs. Appending**

**Important Distinction:**
- **`.set()/.setfield()`**: Only append at the end
- **`+=` and `++=` operators**: Insert at specific positions

```grapa
list = {a:1, b:2, c:3};

/* .set() only appends at the end */
list.set("d", 99);         /* {a:1, b:2, c:3, d:99} - appends at end */

/* += and ++= insert at specific positions */
list = {a:1, b:2, c:3};
list += (d:99) list[1];    /* {a:1, d:99, b:2, c:3} - inserts at position 1 */
list ++= {e:88, f:77} list[0]; /* {e:88, f:77, a:1, d:99, b:2, c:3} - inserts at position 0 */
```

### **Error Handling Patterns**

```grapa
list = {a:1, b:2, c:3};

/* Safe access with null checking */
result = list.get("missing_key");
if (result == null) {
    result = "Key not found";
}

/* Safe setting (always works - appends if needed) */
list.set("new_key", 99);   /* Always succeeds, appends if needed */

/* Check if key exists before access */
if (list.get("key") != null) {
    value = list.get("key");
} else {
    value = "Key doesn't exist";
}
```

### **Comparison with Other Data Types**

```grapa
/* List behavior (flexible) */
list = {a:1, b:2, c:3};
list.set("d", 99);         /* APPENDS: {a:1, b:2, c:3, d:99} */

/* Array behavior (flexible) */
arr = [1, 2, 3];
arr.set(3, 99);            /* APPENDS: [1, 2, 3, 99] */

/* Vector behavior (strict) */
vec = [1, 2, 3].vector();
result = vec.set(3, 99);   /* Returns $ERR - strict bounds */
```

### **Best Practices**

1. **Use lists for dynamic key-value storage** where you need flexible sizing
2. **Use vectors for mathematical operations** where fixed dimensions matter
3. **Check for null returns** when accessing potentially missing keys
4. **Use `+=` and `++=` for insertion** at specific positions
5. **Use `.set()/.setfield()` for appending** at the end
6. **Use key existence checks** before accessing if you need strict behavior
