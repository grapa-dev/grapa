---
tags:
  - user
  - highlevel
---
# Assignment Operators

## **Simple vs Complex Assignment**

Grapa's assignment system is designed for both performance and flexibility:

### **Simple Variable Assignment**
```grapa
x = 10;           /* Direct variable assignment */
name = "John";    /* String assignment */
counter = 0;      /* Integer assignment */
```

### **Complex Assignment (Array/Object Access)**
```grapa
/* Array element assignment */
array[5] = 10;              /* Direct index assignment */
array[-1] = "last";         /* Negative index assignment */

/* Object property assignment */
object.property = "value";  /* Dot notation assignment */
object["property"] = "value"; /* Bracket notation assignment */

/* Nested assignment */
data[0].name = "Alice";     /* Nested object assignment */
config.database.host = "localhost";  /* Deep property assignment */

/* Compound assignment on accessed elements */
array[0] += 5;              /* Add to accessed element */
object.count += 1;          /* Increment accessed property */
```

**Performance Note:** Simple variable assignments are optimized for speed, while complex assignments (array indexing, object properties) provide flexibility but may be slightly slower.

### **System Variable Assignment**
Grapa supports special system variables with optimized handling:

```grapa
@this = value;      /* Current context */
@parent = value;    /* Parent context */
$global = value;    /* Global namespace */
$local = value;     /* Local namespace */
$this = value;      /* Current object */
$parent = value;    /* Parent object */
$root = value;      /* Root object */
$self = value;      /* Self reference */
```

### **System Namespace Protection**

Grapa uses the `$` prefix to protect system namespace. **Important**: The `$` prefix is reserved for system use only. Do not declare your own variables with `$` prefixes.

```grapa
/* User namespace (recommended) */
x = 5;                    /* User variable */
name = "hello";           /* User string */
my_data = [1, 2, 3];      /* User array */

/* System namespace (reserved for system use only) */
/* These are predefined system variables - do not create your own */
$global.config = {"debug": true};  /* Accessing system global namespace */
$local.temp = "value";             /* Accessing system local namespace */
```

**Guidelines:**
- **Use user namespace** (`x`, `"hello"`, `my_data`) for your own code
- **Never declare your own `$` variables** - the `$` prefix is reserved for system use
- **Access system variables** like `$global`, `$local`, `$this` as needed, but don't create new ones
- **Exception**: `$custom_command` and `$custom_function` are special system variables for language extension

## `=`
Assign.

## `+=`
Add item to source.

### Array/List Addition

**Arrays (`$LIST`):**
```grapa
arr = [1, 2, 3, 4, 5];
arr += 6;                 /* Add element to end: [1,2,3,4,5,6] */
arr += [7, 8];            /* Add multiple elements: [1,2,3,4,5,6,7,8] */
arr += 10 arr[0];         /* Insert at position: [10,1,2,3,4,5,6,7,8] */
arr += 20 arr[2];         /* Insert at position 2: [10,1,20,2,3,4,5,6,7,8] */
```

**Lists (`$GOBJ`):**
```grapa
list = {a:1, b:2, c:3};
list += (d:4);            /* Add single key-value pair: {a:1, b:2, c:3, d:4} */
list += {d:4, e:5};       /* Add multiple key-value pairs: {a:1, b:2, c:3, d:4, e:5} */
list += (f:6) list[0];    /* Insert at beginning: {f:6, a:1, b:2, c:3, d:4, e:5} */
list += (g:7) list[1];    /* Insert at position 1: {f:6, g:7, a:1, b:2, c:3, d:4, e:5} */
```

**Vectors (`$VECTOR`):**
```grapa
vec = [1, 2, 3];
vec += 4;                 /* Add element to end */
vec += [5, 6];            /* Add multiple elements */
```

**Widgets (`$WIDGET`):**
```grapa
widget = {name:"button", type:"click"};
widget += ("label", "Click Me");           /* Add widget with name and value */
widget += ("icon", "star.png", widget[0]); /* Add at specific position */
```

**XML/TAG Elements:**
```grapa
xml = <root><item>1</item></root>;
xml += <newitem>2</newitem>;               /* Add XML element */
xml += <child>3</child> xml[0];            /* Add at specific position */
```

## `++=`
Concatenate contents to source.

### Syntax Variations
The `++=` operator supports two syntax variations:

1. **Standard Concatenation**: `target ++= value`
2. **Position-Based Insertion**: `target ++= value position`

### Array/List Concatenation

**Arrays (`$LIST`):**
```grapa
arr1 = [1, 2, 3];
arr2 = [4, 5, 6];
arr1 ++= arr2;   /* Concatenate arrays: [1,2,3,4,5,6] */
arr1 ++= arr2 arr1[0];  /* Insert at beginning: [4,5,6,1,2,3,4,5,6] */
```

**Lists (`$GOBJ`):**
```grapa
list1 = {a:1, b:2};
list2 = {c:3, d:4};
list1 ++= list2; /* Concatenate lists: {"a":1,"b":2,"c":3,"d":4} */
list1 ++= list2 list1[0];  /* Insert at beginning: {"c":3,"d":4,"a":1,"b":2} */
```

**Vectors (`$VECTOR`):**
```grapa
/* Note: ++= operator for vectors is currently not working correctly */
/* Use alternative approaches for vector concatenation: */

/* For 1D vectors - convert to array, concatenate, convert back */
vec1 = #[1, 2, 3]#;
vec2 = #[4, 5, 6]#;
result = vec1.list() + vec2.list();  /* [1,2,3,4,5,6] */
vec_result = result.vector();          /* #[1,2,3,4,5,6]# */

/* For 2D vectors - use array conversion */
matrix1 = #[[1,2],[3,4]]#;
matrix2 = #[[5,6],[7,8]]#;
result = matrix1.list() + matrix2.list();  /* [[1,2],[3,4],[5,6],[7,8]] */
matrix_result = result.vector();             /* #[[1,2],[3,4],[5,6],[7,8]]# */
```

**Widgets (`$WIDGET`):**
```grapa
widget1 = {name:"button", type:"click"};
widget2 = {name:"input", type:"text"};
widget1 ++= widget2; /* Extend widget with another widget */
widget1 ++= widget2 widget1[0];  /* Insert at beginning */
```

**Rules:**
```grapa
x = rule $INT {op(a:$1){a}};
x ++= rule $STR {op(a:$1){a}};
x
/* Result: $INT {...}| $STR {...} */
```

See [Rule Composition and Concatenation](../type/rule.md#rule-composition-and-concatenation) for detailed examples.

## `-=`
Subtract item from source.

### Array/List Removal

**Arrays (`$LIST`):**
```grapa
arr = [1, 2, 3, 4, 5];
arr -= arr[2];   /* Remove by index: [1,2,4,5] */
arr -= arr[-1];  /* Remove last element: [1,2,4] */
```

**Lists (`$GOBJ`):**
```grapa
list = {a:1, b:2, c:3, d:4};
list -= list.c;          /* Remove by key reference: {"a":1,"b":2,"d":4} */
list -= list["c"];       /* Remove by key string: {"a":1,"b":2,"d":4} */
list -= list[-2];        /* Remove by negative index: {"a":1,"b":2,"d":4} */
list -= list[1];         /* Remove by positive index: {"a":1,"c":3,"d":4} */
```

**Important Notes:**
- **Key-based removal**: `list -= list.key` or `list -= list["key"]` removes the key-value pair
- **Index-based removal**: `list -= list[index]` removes the element at that position
- **Value-based removal**: Direct value removal (e.g., `list -= 5`) is **not supported**
- **String key removal**: Direct string removal (e.g., `list -= "key"`) is **not supported**

## `*=`
Multiply and assign.

### Numeric Multiplication Assignment
```grapa
/* Integer multiplication */
x = 10;
x *= 3;                    /* 30 */

/* Float multiplication */
y = 5.5;
y *= 2;                    /* 11.0 */

/* Mixed type multiplication (promotes to float) */
z = 10;
z *= 2.5;                  /* 25.0 */
```

**Type Support**: INT, FLOAT  
**Note**: Only numeric types are supported. String and other types will return $ERR.

## `/=`
Divide and assign.

### Numeric Division Assignment
```grapa
/* Integer division (promotes to float) */
x = 10;
x /= 2;                    /* 5.0 */

/* Float division */
y = 10.0;
y /= 3;                    /* 3.333... */

/* Mixed type division */
z = 10;
z /= 2.5;                  /* 4.0 */
```

**Type Support**: INT, FLOAT  
**Note**: Division always promotes to float for precision. Only numeric types are supported.

## `%=`
Modulo and assign.

### Numeric Modulo Assignment
```grapa
/* Integer modulo */
x = 10;
x %= 3;                    /* 1 */

/* Float modulo */
y = 10.5;
y %= 3;                    /* 1.5 */

/* Mixed type modulo */
z = 10;
z %= 3.5;                  /* 3.0 */
```

**Type Support**: INT, FLOAT  
**Note**: Only numeric types are supported. String and other types will return $ERR.

## `**=`
Power and assign.

### Numeric Power Assignment
```grapa
/* Integer power */
x = 2;
x **= 3;                   /* 8 */

/* Float power */
y = 2.5;
y **= 2;                   /* 6.25 */

/* Mixed type power (promotes to float) */
z = 2;
z **= 3.5;                 /* 11.313... */
```

**Type Support**: INT, FLOAT  
**Note**: Only numeric types are supported. String and other types will return $ERR.

## Advanced Operations

### Search and Filter Operations
Grapa provides several methods for searching and filtering data:

#### Filter Method (Recommended for JSON/Arrays)
```grapa
/* Search for specific values in arrays */
arr = [1, 2, 3, 4, 5, 3];
matches = arr.filter(op(x){x == 3;});  /* Find all elements equal to 3: [3, 3] */

/* Search for objects with specific properties */
data = [{id:1, name:"Alice"}, {id:2, name:"Bob"}, {id:3, name:"Alice"}];
alice_users = data.filter(op(item){item.name == "Alice";});  /* Find all users named Alice */

/* Search for objects with multiple criteria */
active_users = data.filter(op(item){item.name == "Alice" && item.id > 1;});
```

#### Grep Method (For Strings)
```grapa
/* Search for patterns in strings */
str = "hello world";
matches = str.grep("o", "o");  /* Find all 'o' characters: ["o", "o"] */
count = str.grep("o", "c")[0].int();  /* Count occurrences: 2 */
```

#### Find All Method (For Complex Structures)
```grapa
/* Find elements in XML structures */
xml = <root><item>1</item><item>2</item><item>1</item></root>;
matches = xml.findall(<item>1</item>);  /* Find all item elements with content "1" */

/* Find elements in nested structures */
nested = {users:[{id:1, name:"Alice"}, {id:2, name:"Bob"}]};
matches = nested.findall({name:"Alice"}); /* Find all objects with name "Alice" */
```

**Note**: 
- **`filter()`** is the most reliable method for searching JSON objects and arrays
- **`grep()`** is designed for string pattern matching
- **`findall()`** is designed for complex data structures like XML, TAG, and nested lists/arrays

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
