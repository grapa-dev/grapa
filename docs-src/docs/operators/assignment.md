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

## `++=`
Concatenate contents to source.

For rules, this operator concatenates additional rule alternatives, allowing dynamic grammar construction:

```
> x = rule $INT {op(a:$1){a}};
> x ++= rule $STR {op(a:$1){a}};
> x
$INT {@<[op,@<var,{a}>],{"a":$1}>}| $STR {@<[op,@<var,{a}>],{"a":$1}>}
```

See [Rule Composition and Concatenation](../type/rule.md#rule-composition-and-concatenation) for detailed examples.

## `-=`
Subtract item from source.

## `*=`
Multiply and assign.

## `/=`
Divide and assign.

## `%=`
Modulo and assign.

## `**=`
Power and assign.

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
