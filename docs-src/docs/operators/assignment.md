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
array[5] = 10;              /* Array element assignment */
object.property = "value";  /* Object property assignment */
data[0].name = "Alice";     /* Nested object assignment */
config.database.host = "localhost";  /* Deep property assignment */
```

**Performance Note:** Simple variable assignments are optimized for speed, while complex assignments (array indexing, object properties) provide flexibility but may be slightly slower.

### **System Variable Assignment**
Grapa supports special system variables with optimized handling:

```grapa
@this = value;      /* Current context */
@parent = value;    /* Parent context */
@global = value;    /* Global namespace */
@local = value;     /* Local namespace */
$this = value;      /* Current object */
$parent = value;    /* Parent object */
$root = value;      /* Root object */
$self = value;      /* Self reference */
```

### **System Namespace Protection**

Grapa uses the `$` prefix to protect system namespace:

```grapa
/* User namespace (recommended) */
x = 5;                    /* User variable */
name = "hello";           /* User string */

/* System namespace (reserved) */
$x = 5;                   /* System variable */
$name = "hello";          /* System string */
```

**Guidelines:**
- **Use user namespace** (`x`, `"hello"`) for your own code
- **Avoid system namespace** (`$x`, `$"hello"`) unless necessary
- **Both work** but system namespace is protected to prevent accidental overrides

## `=`
Assign.

## `+=`
Add item to source.

### Array/List Addition

**Arrays (`$ARRAY`):**
```grapa
arr = [1, 2, 3, 4, 5];
arr += 6;                 /* Add element to end: [1,2,3,4,5,6] */
arr += [7, 8];            /* Add multiple elements: [1,2,3,4,5,6,7,8] */
arr += 10 arr[0];         /* Insert at position: [10,1,2,3,4,5,6,7,8] */
arr += 20 arr[2];         /* Insert at position 2: [10,1,20,2,3,4,5,6,7,8] */
```

**Lists (`$LIST`):**
```grapa
list = {a:1, b:2, c:3};
list += (d:4);            /* Add single key-value pair: {"a":1,"b":2,"c":3,"d":4} */
list += {d:4, e:5};       /* Add multiple key-value pairs: {"a":1,"b":2,"c":3,"d":4,"e":5} */
list += (f:6) list[0];    /* Insert at beginning: {"f":6,"a":1,"b":2,"c":3,"d":4,"e":5} */
list += (g:7) list[1];    /* Insert at position 1: {"f":6,"g":7,"a":1,"b":2,"c":3,"d":4,"e":5} */
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

### Array/List Concatenation

**Arrays (`$ARRAY`):**
```grapa
arr1 = [1, 2, 3];
arr2 = [4, 5, 6];
arr1 ++= arr2;   /* Concatenate arrays: [1,2,3,4,5,6] */
```

**Lists (`$LIST`):**
```grapa
list1 = {a:1, b:2};
list2 = {c:3, d:4};
list1 ++= list2; /* Concatenate lists: {"a":1,"b":2,"c":3,"d":4} */
```

**Vectors (`$VECTOR`):**
```grapa
vec1 = [1, 2, 3];
vec2 = [4, 5, 6];
vec1 ++= vec2;   /* Extend vector: [1,2,3,4,5,6] */
```

**Widgets (`$WIDGET`):**
```grapa
widget1 = {name:"button", type:"click"};
widget2 = {name:"input", type:"text"};
widget1 ++= widget2; /* Extend widget with another widget */
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

**Arrays (`$ARRAY`):**
```grapa
arr = [1, 2, 3, 4, 5];
arr -= arr[2];   /* Remove by index: [1,2,4,5] */
arr -= arr[-1];  /* Remove last element: [1,2,4] */
```

**Lists (`$LIST`):**
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

## `/=`
Divide and assign.

## `%=`
Modulo and assign.

## `**=`
Power and assign.

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
