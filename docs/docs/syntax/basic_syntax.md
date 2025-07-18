---
tags:
  - user
  - highlevel
---
# Basic Grapa Syntax Guide

> **Canonical Reference:**
> This file is the canonical, empirically tested reference for Grapa syntax and access patterns. All code in documentation and tests must comply with these rules. If you discover any new rules or exceptions, update this file immediately.

> **Important: Access Patterns for Data Types (Tested, v0.0.39)**
>
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation | .len() | .size() |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|:------:|:-------:|
> | $ARRAY    |      ❌      |     ❌      |       ✅         |      —       |   ✅   |    ❌   |
> | $LIST     |      ❌      |     ❌      |       ✅         |     ✅       |   ✅   |    ❌   |
> | $OBJ      |      ❌      |     ❌      |       ✅         |     ✅       |   ❌   |    ❌   |
> | $file     |      ✅      |     ❌      |        —         |      —       |   ❌   |    ❌   |
> | $TABLE    |     ✅*      |     ❌      |        —         |      —       |   ❌   |    ❌   |
>
> *$TABLE .get() requires two arguments: key and field.
>
> **Key Findings:**
> - **Arrays (`[]`)**: Use `array[index]` and `array.len()` for access and length
> - **Lists (`{}`)**: Use `list[key]` or `list.key` for access, `list.len()` for length
> - **Objects (class)**: Use `object.property` or `object[key]` for access
> - **`.get()` method**: Only works on `$file` and `$TABLE` types
> - **`.size()` method**: Not supported on any type (use `.len()` instead)
> - **`.keys()` method**: Not supported on `$LIST` (use iteration instead)
>
> **Recommended Patterns:**
> - For arrays: `array[index]` and `array.len()`
> - For lists: `list[key]` (preferred) or `list.key`, and `list.len()`
> - For objects: `object.property` (preferred) or `object[key]`
> - Avoid `.get()`, `.size()`, and `.keys()` on arrays, lists, and objects

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` (returns empty string).
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - **Length**: Use `.len()` for arrays and lists, not `.size()`.
> - **Keys**: For lists, iterate manually instead of using `.keys()`.
> - If more objects support `.get()` in the future, this guide will be updated.

---

This guide covers the essential syntax patterns for writing Grapa code, based on working examples and best practices.

**See also:** [Operator Precedence Table](precedence.md)

## Output and Echo

### Correct Echo Syntax

Use the `.echo()` method on strings, not function calls:

```grapa
/* ✅ Correct */
"Hello World".echo();
("Result: " + value.str()).echo();

/* ❌ Incorrect */
echo("Hello World");
echo("Result: " + value);
```

### String Concatenation

Always wrap string concatenation expressions in parentheses:

```grapa
/* ✅ Correct */
("Name: " + name.str() + ", Age: " + age.str()).echo();
("File size: " + size + " bytes").echo();

/* ❌ Incorrect */
"Name: " + name.str() + ", Age: " + age.str().echo();
"File size: " + size + " bytes".echo();
```

## Loops

### While Loops

Use `while` loops with explicit increment:

```grapa
/* ✅ Correct */
i = 1;
while (i <= 10) {
    ("Processing item " + i.str()).echo();
    i = i + 1;
}

/* ❌ Incorrect */
for i in 1..10 {
    echo("Processing item " + i);
}
```

- Only `while` loops and `.range()`+functional methods are valid for iteration in Grapa.
- `for` loops are not supported.
- If more objects support `.get()` in the future, this guide will be updated.

### Loop Examples

```grapa
/* Count from 1 to 5 */
i = 1;
while (i <= 5) {
    ("Count: " + i.str()).echo();
    i = i + 1;
}

/* Process array elements */
i = 0;
while (i < array.len()) {
    element = array[i];  /* Use bracket notation for arrays */
    ("Element " + i.str() + ": " + element.str()).echo();
    i = i + 1;
}
```

### Sequence Generation with .range()

Use `.range()` to generate sequences of numbers:

```grapa
/* Generate sequence 0 to 9 */
seq = (10).range(0,1);  /* [0,1,2,3,4,5,6,7,8,9] */

/* Generate sequence with custom start and step */
seq = (10).range(1,2);  /* [1,3,5,7,9] */

/* Use .range() with functional methods for iteration */
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);  /* Sum 0-9 */

/* Parallel processing with thread count */
squares = (1000000).range(0,1).map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

- Use `.range()` instead of manual while loops for sequence generation
- Combine with `.map()`, `.filter()`, `.reduce()` for functional iteration
- Specify thread count for large arrays to avoid too many threads

## Functional Programming Methods

Grapa provides powerful functional programming methods for array and list processing:

### .map() - Transform Elements

```grapa
/* Transform each element */
numbers = [1, 2, 3, 4, 5];
doubled = numbers.map(op(x) { x * 2; });  /* [2, 4, 6, 8, 10] */

/* Transform with string operations */
names = ["john", "jane", "bob"];
uppercase = names.map(op(name) { name.upper(); });  /* ["JOHN", "JANE", "BOB"] */

/* Parallel processing with thread count */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### .filter() - Select Elements

```grapa
/* Filter even numbers */
numbers = [1, 2, 3, 4, 5, 6];
evens = numbers.filter(op(x) { x % 2 == 0; });  /* [2, 4, 6] */

/* Filter non-empty strings */
lines = ["hello", "", "world", "", "test"];
non_empty = lines.filter(op(line) { line.len() > 0; });  /* ["hello", "world", "test"] */

/* Parallel filtering */
large_data = (1000000).range(0,1);
filtered = large_data.filter(op(x) { x % 2 == 0; }, 8);  /* Limit to 8 threads */
```

### .reduce() - Accumulate Values

```grapa
/* Sum all numbers */
numbers = [1, 2, 3, 4, 5];
sum = numbers.reduce(op(acc, x) { acc + x; }, 0);  /* 15 */

/* Build a string */
words = ["hello", "world", "test"];
sentence = words.reduce(op(acc, word) { acc + " " + word; }, "");  /* " hello world test" */

/* Collect even numbers */
numbers = [1, 2, 3, 4, 5, 6];
evens = numbers.reduce(op(acc, x) {
    if (x % 2 == 0) { acc += x; };
    acc;
}, []);  /* [2, 4, 6] */
```

### Method Chaining

Combine multiple functional methods:

```grapa
/* Filter, transform, then sum */
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
result = numbers
    .filter(op(x) { x % 2 == 0; })      /* [2, 4, 6, 8, 10] */
    .map(op(x) { x * x; })              /* [4, 16, 36, 64, 100] */
    .reduce(op(acc, x) { acc + x; }, 0); /* 220 */

/* Process with .range() */
result = (10).range(0,1)
    .filter(op(x) { x % 2 == 0; })      /* [0, 2, 4, 6, 8] */
    .map(op(x) { x * x; })              /* [0, 4, 16, 36, 64] */
    .reduce(op(acc, x) { acc + x; }, 0); /* 120 */
```

### Parallel Processing

Functional methods support parallel processing for large datasets:

```grapa
/* Large dataset processing */
data = (1000000).range(0,1);

/* Parallel map with 8 threads */
squares = data.map(op(x) { x * x; }, 8);

/* Parallel filter with 8 threads */
evens = data.filter(op(x) { x % 2 == 0; }, 8);

/* Parallel reduce with 4 threads */
sum = data.reduce(op(acc, x) { acc + x; }, 0, 4);
```

**Important Notes:**
- `.map()` and `.filter()` are parallel by default for large arrays
- Always specify thread count for very large datasets to avoid too many threads
- `.reduce()` can be parallel but requires careful consideration of the operation
- Method chaining is efficient and readable

## Data Types and Access Patterns

### Array Type (`$ARRAY`)

Arrays are created with square brackets and support index-based access:

```grapa
/* Create arrays */
numbers = [1, 2, 3, 4, 5];
names = ["Alice", "Bob", "Charlie"];
mixed = [1, "text", true, 3.14];

/* Access elements */
first = numbers[0];      /* 1 */
second = names[1];       /* "Bob" */
length = numbers.len();  /* 5 */

/* Iterate over arrays */
i = 0;
while (i < numbers.len()) {
    ("Number " + i.str() + ": " + numbers[i]).echo();
    i += 1;
}
```

**Array Methods:**
- `array[index]` - Access element by index
- `array.len()` - Get array length
- `array.size()` - **Not supported** (use `.len()` instead)
- `array.get(index)` - **Not supported** (use bracket notation)

### List Type (`$LIST`)

Lists are created with curly braces and support key-based access:

```grapa
/* Create lists */
config = {"debug": true, "port": 3000, "host": "localhost"};
user = {"name": "John", "age": 30, "city": "NYC"};
nested = {"level1": {"level2": "value"}};

/* Access elements */
debug_mode = config["debug"];    /* true */
user_name = user.name;           /* "John" */
deep_value = nested["level1"]["level2"];  /* "value" */
length = config.len();           /* 3 */

/* Iterate over lists */
keys = ["debug", "port", "host"];
i = 0;
while (i < keys.len()) {
    key = keys[i];
    value = config[key];
    (key + ": " + value.str()).echo();
    i += 1;
}
```

**List Methods:**
- `list[key]` - Access element by key (preferred)
- `list.key` - Access element by key (alternative)
- `list.len()` - Get list length
- `list.get(key)` - **Not supported** (use bracket notation)
- `list.keys()` - **Not supported** (iterate manually)

### Object Type (`$OBJ`)

Objects are instances of classes and support property access:

```grapa
/* Define a class */
Person = class {
    name = "";
    age = 0;
    
    init = op(n, a) {
        name = n;
        age = a;
    };
    
    getInfo = op() {
        ("Name: " + name + ", Age: " + age.str()).echo();
    };
};

/* Create object instances */
person1 = obj Person;
person1.init("Alice", 25);
person2 = obj Person;
person2.init("Bob", 30);

/* Access properties */
name = person1.name;             /* "Alice" */
age = person1["age"];            /* 25 */
person1.getInfo();               /* Call method */
```

**Object Methods:**
- `object.property` - Access property (preferred)
- `object[key]` - Access property (alternative)
- `object.method()` - Call object methods
- `object.get(key)` - **Not supported** (use dot notation)

### Type Comparison Summary

| Feature | $ARRAY | $LIST | $OBJ |
|---------|--------|-------|------|
| Creation | `[1,2,3]` | `{"key":"value"}` | `obj Class` |
| Access | `array[index]` | `list[key]` or `list.key` | `object.property` |
| Length | `array.len()` | `list.len()` | Not available |
| Iteration | Index-based | Key-based | Property-based |

## Variable Assignment

### Basic Assignment

```grapa
/* Simple variables */
name = "John Doe";
age = 30;
salary = 75000.50;

/* Incrementing and appending */
v = v + 1;   /* Valid */
v += 1;      /* Idiomatic and preferred */
s = s + "x";   /* Valid */
s += "x";      /* Idiomatic and preferred */
```

### Namespace System

Grapa has a dynamic namespace system that's automatically managed by the execution tree:

#### Global Namespace
Use `$global` for variables that need to persist across function calls:

```grapa
/* Create global variable */
$global.my_table = $file().table("ROW");

/* Use global variable */
my_table.mkfield("name", "STR", "VAR");
```

#### Local Namespace
Each function execution creates its own namespace automatically:

```grapa
/* Function with local variables */
func = op(x) {
    local_var = x * 2;  /* This is in the function's local namespace */
    local_var.echo();
};

func(5);  /* Output: 10 */
```

#### Namespace Hierarchy
Variables can be accessed from parent namespaces:

```grapa
/* Global variable */
$global.config = {"debug": true, "timeout": 30};

/* Function can access global variables */
func = op() {
    if (config.debug) {
        ("Debug mode enabled").echo();
    }
    /* Local override */
local_config = {"debug": false};
    local_config.debug.echo();  /* false */
};

func();
```

#### Dynamic Namespace Creation
Namespaces are created automatically for each execution context:

```grapa
/* Each block creates a new namespace */
if (condition) {
    block_var = "value";  /* In block namespace */
} else {
    block_var = "other";  /* Different block namespace */
}
```

### Const Protection

Grapa provides runtime const protection that prevents variable modification:

```grapa
/* Create const variable */
config = const {"debug": true, "port": 3000};

/* Cannot modify (will error) */
config.port = 8080;  /* Error: cannot modify const */

/* Can toggle const protection */
config.setconst(false);
config.port = 8080;  /* Now works */
config.setconst(true);  /* Make const again */

/* Const arrays */
colors = const ["red", "green", "blue"];
colors += "yellow";  /* Error: cannot modify const */

/* Performance optimization */
static_config = static {"api_url": "https://api.example.com"};
```

**Benefits of Grapa's const:**
- **Runtime protection**: Prevents actual modification at runtime
- **Dynamic control**: Can toggle const on/off as needed
- **Performance**: Optimized for caching and concurrent access
- **Thread safety**: Prevents blocking on access for concurrent threads

### Class System

Grapa provides a full object-oriented programming system with classes, inheritance, and polymorphism:

#### Basic Class Definition
```grapa
/* Define a class */
Person = class {
    name = "";
    age = 0;
    city = "";
    
    /* Constructor */
    init = op(n, a, c) {
        name = n;
        age = a;
        city = c;
    };
    
    /* Method */
    getInfo = op() {
        ("Name: " + name + ", Age: " + age.str() + ", City: " + city).echo();
    };
};

/* Create instance */
person = obj Person;
person.init("John", 30, "NYC");
person.getInfo();
```

#### Inheritance
```grapa
/* Base class */
Animal = class {
    name = "";
    speak = op() { ("Some sound").echo(); };
};

/* Derived class */
Dog = class (Animal) {
    breed = "";
    /* Override method */
speak = op() { ("Woof!").echo(); };
    fetch = op() { ("Fetching ball").echo(); };
};

/* Usage */
myDog = obj Dog;
myDog.name = "Buddy";
myDog.speak();  /* Output: Woof! */
```

#### Polymorphism
```grapa
/* Different animal types */
animals = [obj Dog, obj Animal];
i = 0;
while (i < animals.len()) {
    animals[i].speak();  /* Calls appropriate method */
    i += 1;
}
```

### Inheritance and Composition

Grapa supports full object-oriented programming features:

- **Inheritance**: Use `class (ParentClass)` to create subclasses
- **Method overriding**: Subclasses can override parent methods
- **Method sets**: Classes have sets of methods, similar to Go/Java
- **Composition**: Classes and objects can include other objects as properties

#### Example
```grapa
Animal = class {
    speak = op() { "Some sound".echo(); };
};
Dog = class (Animal) {
    /* Method override */
    speak = op() { "Woof!".echo(); };
    /* New method */
    fetch = op() { "Fetching...".echo(); };
};
myDog = obj Dog;
myDog.speak();  /* Outputs: Woof! */
```

## String Operations

### String Methods

```grapa
/* Convert to string */
text = value.str();

/* String concatenation */
full_name = first_name + " " + last_name;

/* String with variables */
message = ("Hello " + name + ", you are " + age.str() + " years old");
```

### String Concatenation

```grapa
/* Both forms are valid, but += is preferred */
message = message + " world";
message += "!";
```

### String Literals

Always use double quotes for string literals:

```grapa
/* ✅ Correct */
name = "John Doe";
message = "Hello World";

/* ❌ Incorrect */
name = John Doe;
message = Hello World;
```

## Array and Object Access

### Array Access

Arrays (`$ARRAY`) support only bracket notation:

```grapa
arr = ["a", "b", "c"];
value = arr[1];        /* Returns "b" */
```

- Use bracket notation for `$ARRAY`.
- `.get()` is not supported for `$ARRAY`.

### Array Concatenation

Use the `+=` operator to concatenate arrays:

```grapa
arr1 = ["a", "b"];
arr2 = ["c", "d"];
arr1 += arr2;          /* arr1 is now ["a", "b", "c", "d"] */
```

- The `+=` operator appends all elements from the right array to the left array.
- This is simpler than using `.reduce()` or other functional methods.
- No spread operator (`...`) is needed or supported.

### Array Slicing with .range()

Use `.range()` for array slicing operations:

```grapa
arr = ["a", "b", "c", "d", "e"];

/* Get elements from index 1 to end */
slice1 = arr.range(1, arr.len());  /* ["b", "c", "d", "e"] */

/* Get first 3 elements */
slice2 = arr.range(0, 3);  /* ["a", "b", "c"] */

/* Get elements from index 1 to 3 */
slice3 = arr.range(1, 3);  /* ["b", "c"] */
```

- Use `.range(start, end)` for array slicing
- This replaces slice notation like `arr[1:]`, `arr[:3]`, `arr[1:3]`

### Array Methods

Grapa provides several useful array methods:

```grapa
/* Basic array operations */
arr = [3, 1, 4, 1, 5, 9, 2, 6];

/* Sort array */
sorted_arr = arr.sort();  /* [1, 1, 2, 3, 4, 5, 6, 9] */

/* Get array length */
length = arr.len();  /* 8 */

/* Join array elements into string */
words = ["hello", "world", "test"];
sentence = words.join(" ");  /* "hello world test" */

/* Split string into array */
text = "hello,world,test";
parts = text.split(",");  /* ["hello", "world", "test"] */
```

**Important Notes:**
- Use `.sort()` for deterministic test output when order is not guaranteed
- `.len()` returns the number of elements in an array
- `.join(delimiter)` combines array elements with the specified delimiter
- `.split(delimiter)` splits a string into an array using the delimiter

### List/Object Access

Lists (`$LIST`) and objects (`$OBJ`) support bracket and dot notation:

```grapa
obj = {"a": 11, "b": 22, "c": 33};
value = obj["b"];          /* Returns 22 (key access) */
value = obj.key;            /* Returns value for key 'key' if present */
value = obj."b";           /* Returns 22 (key access) */

/* $LIST supports both key and index access: */
value = obj[1];             /* Returns 22 (index access) */
value = obj["a"];           /* Returns 11 (key access) */
value = obj.a;              /* Returns 11 (dot notation key access) */
name = obj.getname(1);      /* Returns "b" (key name) */
```

- Use bracket and dot notation for `$LIST` and `$OBJ`.
- $LIST supports both key access (obj["key"], obj.key) and index access (obj[2]).
- `.get()` is not supported for `$LIST` or `$OBJ`.

### $file and $TABLE Access

For `$file` and `$TABLE` objects, always use `.get()` method:

```grapa
files = $file().ls();
file_info = files.get(0);   /* Correct */

table = $file().table("ROW");
table.mkfield("name", "STR", "VAR");
table.set("user1", "Alice", "name");
value = table.get("user1", "name");   /* Correct */
```

- Always use `.get()` for `$file` and `$TABLE`.
- Bracket and dot notation are not valid for `$file` and `$TABLE`.

### Type System Philosophy

Grapa uses **dynamic typing** as a fundamental design choice, not a limitation. This enables Grapa's core strengths:

### Dynamic Type System Benefits
- **Runtime flexibility**: Types are determined at execution time
- **Dynamic code execution**: Enables meta-programming and code generation
- **System integration**: Works with data of unknown types
- **Data processing**: Ideal for ETL and analysis tasks

### Type Safety Through Runtime Checking
Grapa provides robust type safety through runtime checking and introspection:

```grapa
/* Check object types */
element = ["a", "b", "c"];
element.type();        /* Returns $ARRAY */

obj = {"a": 11, "b": 22, "c": 33};
obj.type();            /* Returns $LIST */

table = $file().table("ROW");
table.type();          /* Returns $TABLE */

/* Runtime type checking */
if (value.type() == $INT) {
    ("Value is an integer").echo();
} else if (value.type() == $STR) {
    ("Value is a string").echo();
}
```

### Type Conversion
Grapa provides explicit type conversion methods:

```grapa
/* String conversion */
value = 42;
text = value.str();    /* Convert to string */

/* Numeric conversion */
text = "123";
number = text.int();   /* Convert to integer */
decimal = text.float(); /* Convert to float */

/* Type checking before conversion */
if (input.type() == $STR) {
    result = input.int();
} else {
    result = input;    /* Already numeric */
}
```

## Function Calls

### Method Calls

```grapa
/* File operations */
fs.set("file.txt", "content");
content = fs.get("file.txt").str();

/* Table operations */
table.mkfield("name", "STR", "VAR");
table.set("key", "value", "field");
value = table.get("key", "field").str();
```

### Function Definitions with Default Parameters

```grapa
/* Function with default parameters */
f = op("x"=1, "y"=0) { x + y; };

/* Call with all parameters */
result1 = f(5, 3);  /* 8 */

/* Call with default for y */
result2 = f(5);     /* 6 (y defaults to 0) */

/* Call with defaults for both */
result3 = f();      /* 1 (x defaults to 1, y defaults to 0) */
```

- Use `op("param"=default_value)` for default parameters
- Parameters with defaults can be omitted when calling
- Parameters without defaults must be provided

### System Functions

```grapa
/* Time functions */
current_time = $TIME().utc();

/* File functions */
file_obj = $file();
table_obj = $file().table("ROW");
```

## Comments

Grapa only supports block comments (`/* ... */`). Line comments (`// ...`) are not supported.

### Comment Rules

#### Outside Code Blocks
Comments can be placed at the end of lines when not inside `{}` blocks:

```grapa
/* This is a block comment */
x = 5;  /* This works outside {} blocks */
y = 10; /* This also works */
```

#### Inside Code Blocks (`{}`)
When inside `{}` blocks, comments **must** be on their own line:

```grapa
/* ✅ Correct: Comments on their own line */
if (condition) {
    /* This comment is correct */
    x = 5;
    /* Another correct comment */
    y = 10;
}

/* ❌ Incorrect: Comments at end of lines in {} blocks */
if (condition) {
    x = 5;  /* This will cause a syntax error */
    y = 10; /* This will also cause a syntax error */
}
```

#### Class and Function Definitions
The same rule applies to class and function definitions:

```grapa
/* ✅ Correct */
MyClass = class {
    /* Method definition */
    myMethod = op() {
        /* Method body */
        "Hello".echo();
    };
};

/* ❌ Incorrect */
MyClass = class {
    myMethod = op() {
        "Hello".echo();  /* This will cause a syntax error */
    };
};
```

## Control Structures

### If Statements

```grapa
if (condition) {
    ("Condition is true").echo();
} else {
    ("Condition is false").echo();
}

/* Example with comparison */
if (age >= 18) {
    ("User is an adult").echo();
} else {
    ("User is a minor").echo();
}
```

### Comparison Operators

```grapa
/* Equality */
if (name == "John") { }

/* Inequality */
if (age != 0) { }

/* Greater/Less than */
if (salary > 50000) { }
if (age < 18) { }

/* Greater/Less than or equal */
if (count >= 10) { }
if (value <= 100) { }
```

### Switch Statements

Grapa provides switch statements for multiple condition checking:

#### Type Switching
```grapa
/* Switch based on type */
switch (value.type()) {
    case $STR: "String value".echo();
    case $INT: "Integer value".echo();
    case $ARRAY: "Array value".echo();
    case $LIST: "List value".echo();
    default: "Unknown type".echo();
};
```

#### Value Switching
```grapa
/* Switch based on value */
switch (status) {
    case "active": "User is active".echo();
    case "inactive": "User is inactive".echo();
    case "pending": "User is pending".echo();
    default: "Unknown status".echo();
};
```

#### Boolean Switching
```grapa
/* Switch based on boolean conditions */
switch(true) {
    case (score >= 90): "Grade: A".echo();
    case (score >= 80): "Grade: B".echo();
    case (score >= 70): "Grade: C".echo();
    case (score >= 60): "Grade: D".echo();
    default: "Grade: F".echo();
};
```

#### Switch with Complex Conditions
```grapa
/* Switch with complex expressions */
switch(true) {
    case (age < 18 && income < 30000): "Young and low income".echo();
    case (age >= 65): "Senior citizen".echo();
    case (income > 100000): "High income".echo();
    default: "Standard category".echo();
};
```

## Unsupported Syntax Operators

Grapa does not support several modern JavaScript/TypeScript operators. Use explicit alternatives:

### Logical Assignment Operators

```grapa
/* ❌ Not supported */
x ||= y;    /* Logical OR assignment */
x &&= y;    /* Logical AND assignment */

/* ✅ Use explicit assignment */
/* For ||= */
if (!x) { x = y; }
/* For &&= */
if (x) { x = y; }
```

### Nullish Coalescing

```grapa
/* ❌ Not supported */
x ?? y;     /* Nullish coalescing */

/* ✅ Use explicit null check */
if (x == null) { x = y; }
```

### Optional Chaining

```grapa
/* ❌ Not supported */
obj?.prop?.sub;    /* Optional chaining */

/* ✅ Use explicit null checks */
if (obj && obj.prop) { obj.prop.sub; }
```

### Spread Operator

```grapa
/* ❌ Not supported */
[...arr1, ...arr2];    /* Spread operator */

/* ✅ Use += operator for arrays */
arr1 += arr2;          /* Array concatenation */
```

### Walrus Operator (Python)

```grapa
/* ❌ Not supported in Python */
if (x := f()) > 0:     /* Walrus operator */

/* ✅ Use separate assignment */
x = f();
if (x > 0) { ... }
```

## Error Handling

### Type Checking

```grapa
/* Check if result is an error */
if (result.type() == $ERR) {
    ("Operation failed").echo();
} else {
    ("Operation succeeded").echo();
}

/* Check for specific error responses */
if (result == {"error":-1}) {
    ("Operation returned error -1").echo();
}
```

### Error Fallback with .iferr()

Grapa provides a built-in error fallback mechanism using `.iferr()`:

```grapa
/* Simple error fallback */
result = risky_operation().iferr(0);  /* Returns 0 if operation fails */

/* Error fallback with default value */
value = (10/0).iferr(55);  /* Returns 55 since division by zero fails */

/* Array access with fallback */
months = ["Jan", "Feb", "Mar"];
value = months["FFF"].iferr(-1);  /* Returns -1 for invalid key */
```

- Use `.iferr(fallback_value)` for simple error handling
- Use `if (result.type() == $ERR)` for explicit error handling
- `.iferr()` is preferred for simple fallback scenarios

## Complete Example

```grapa
/* User Management System Example */

/* Create table in global namespace */
$global.users = $file().table("ROW");

/* Define table schema */
users.mkfield("name", "STR", "VAR");
users.mkfield("age", "INT");
users.mkfield("salary", "FLOAT", "FIX", 8);

/* Add user data */
users.set("user1", "John Doe", "name");
users.set("user1", 30, "age");
users.set("user1", 75000.50, "salary");

/* Retrieve and display user data */
name = users.get("user1", "name").str();
age = users.get("user1", "age").int();
salary = users.get("user1", "salary").float();

("User: " + name + ", Age: " + age.str() + ", Salary: " + salary.str()).echo();

/* Process multiple users */
i = 1;
while (i <= 5) {
    user_id = "user" + i.str();
    users.set(user_id, ("User " + i.str()), "name");
    users.set(user_id, (20 + i), "age");
    i = i + 1;
}

("Created " + i.str() + " users").echo();
```

## Common Patterns

### File Operations

```grapa
/* File system operations */
$global.fs = $file();

/* Create and write file */
fs.set("test.txt", "Hello World");
content = fs.get("test.txt").str();
("File content: " + content).echo();

/* Directory operations */
fs.mk("test_dir");
fs.cd("test_dir");
files = fs.ls();
fs.cd("..");
fs.rm("test_dir");
```

### Table Operations

```grapa
/* Table operations */
$global.table = $file().table("ROW");

/* Create fields */
table.mkfield("name", "STR", "VAR");
table.mkfield("age", "INT");
table.mkfield("value", "FLOAT", "FIX", 8);

/* Add data */
table.set("key1", "John", "name");
table.set("key1", 30, "age");
table.set("key1", 100.5, "value");

/* Retrieve data */
name = table.get("key1", "name").str();
age = table.get("key1", "age").int();
value = table.get("key1", "value").float();
```

This guide covers the essential syntax patterns for writing correct Grapa code. Follow these patterns to avoid common syntax errors and write maintainable code. 

/*
# Grapa Basic Syntax: Key Rules

## Statement and Block Termination
- Every statement must end with a semicolon (`;`), including after the closing brace of every block (such as `if`, `else`, `while`, and function blocks).
- Example:
  while (cond) {
      ...
  };
  if (cond) {
      ...
  } else {
      ...
  };

## Array Manipulation
- To append to an array, use the `+=` operator.
  - Example:
    arr = [];
    arr += "foo";
    arr += "bar";
    /* arr is now ["foo", "bar"] */
- Do not use `.push()` or `.append()`—these are not valid in Grapa.

## Array Iteration and Sorting
- The `.sort()` function is especially useful in tests to ensure deterministic output when order is not guaranteed.
- Example:
  result = input.grep(pattern, options);
  result = result.sort();
  (result.join("\n")).echo();
*/ 

/*
# Grapa Syntax Quick Reference

## Essential Syntax Rules (Summary Table)
| Rule | Example |
|------|---------|
| Every statement ends with a semicolon (`;`) | `x = 5;` |
| Every block (after `}`) ends with a semicolon | `if (x) { ... };` |
| Use block comments only | `/* comment */` |
| Do not use line comments (`// ...`) |  |
| Within `{}` blocks, comments must be on their own line | `{ /* comment */ x = 5; }` |
| Do not put comments at the end of lines in `{}` blocks | `{ x = 5; /* wrong */ }` |
| Append to arrays with `+=` | `arr += "foo";` |
| No `.push()` or `.append()` |  |
| Access arrays/lists with `[index]` | `arr[0];` |
| Access object properties with `.get("key")` | `obj.get("foo");` |
| Use `.echo()` for output | `"Hello".echo();` |
| Use `while` loops, not `for` | `while (cond) { ... };` |
| Wrap string concatenations in parentheses | `(str1 + str2).echo();` |
| Increment or append with += | `v += 1;`, `s += "x";` |
| Both = x + y and += y are valid | `v = v + 1;`, `v += 1;` |
| No logical assignment (`||=`, `&&=`) | Use explicit `if` statements |
| No nullish coalescing (`??`) | Use explicit null checks |
| No optional chaining (`?.`) | Use explicit null checks |
| No spread operator (`...`) | Use `+=` for arrays |

---
*/ 

/*
# Lexical Operators (Special Parsing Triggers)

There are several predefined lexical operators, most of which define how $ID, $INT, $FLOAT, $STR, etc, are processed and generate the corresponding tokens. There are also a few other lexical operators that will trigger special handling of the input stream. The following are two examples. Currently there is no way to define/change the lexical operators - this will come in some future version of Grapa.

## `$&`
  * Wrap the XML data in $& on either side to have Grapa parse the input as XML. These characters are special lexical triggers that modify the parsing engine token generation. The first instance turns the mode on and the second turns the mode off. Or use the encode("XML") function, which does the same thing.

## `$[` 
  * Wrap input in these characters to have the entire code block parsed in a single instance. Otherwise a '\n' or '\r' will be used to trigger parsing/execution and an error would result if the line is not valid on its own. Alternatively, put the code in a file, load the file contents, and execute the contents of the string.
*/ 

/*
# Default Grammar Rules

View a text version of the grammar rules loaded into Grapa on startup here:
Grapa Grammar

The system will first check for a match on the "start" rule, which is a global variable. If that global variable is of type $RULE, then it will become the first rule for scripts. This is an easy way to provide an override on command processing. If the "start" rule does not provide a match, then the system will evaluate using the "$start" rule.

The default rules may be subject to change.

If you modify the file, you can try it out by placing it in "lib/grapa/" under the same directory that the above command would have written the file to, and then restart Grapa. If Grapa finds this file with this name in that location, it will use that file instead of the default.

If you are familiar with YACC or BNF, following the grammar logic should be rather straightforward. Each rule is evaluated until a successful match, and all matches produce an execution plan where the code associated with each rule match is included in the execution plan. To optimize grammar resolution, rules are carefully constructed to avoid re-evaluating rules more than necessary by placing the most complex rules first and following with common rule patterns. It is also important to avoid infinite recursive situations—mostly this is avoided by not referencing the same rule as the first token to evaluate.

There are a few standard tokens that are defined—mostly the tokens provide special handling for either the lexical engine or the rules engine. One example is the space character, which when found sets a bit in the token to skip the value for rules evaluation. This can be changed—but then would require including the space token in the rules.
*/ 

/*
# Function Chaining
Any object that returns an object can be chained.

For example, "4.4".float(300,4).pi() will convert the string 4.4 to a float with 300 bit precision and will return pi() to the power of the input, at the precision specified in the input. So, a 400 bit precision of pi to the power of 4.4.

If an existing function/command doesn't support chaining, an OP can be inserted in the chain to make it work using the $$ variable for the result of the prior operation. For example, the following will get the length of a list, generate that many bits, and output the result in binary form. The 3 examples all accomplish the same result.
```
> {1,2,3,5}.{(@$$).len()}.genbits().bin();
1111

> {1,2,3,5}.{(op(a){a.len()})(@$$)}.genbits().bin();
1111

> f = op(a){a.len()};
> {1,2,3,5}.{f(@$$)}.genbits().bin();
1111
```
*/ 

/*
# Dynamic Code Execution (Grapa's Core Meta-Programming Capability)

Grapa's most powerful feature is its ability to compile and execute code at runtime. Unlike most languages, Grapa provides **superior** dynamic code execution capabilities through execution trees that are both human-readable and directly executable.

## Using `$sys().eval()` for Immediate Evaluation
- Evaluates a string as Grapa code immediately
- Optionally accepts a parameter map for dynamic execution
- Perfect for one-off evaluations and user input processing

```grapa
/* Simple evaluation */
result = $sys().eval("5*3"); // result: 15

/* Evaluation with parameters */
result = $sys().eval("x + y", {"x": 2, "y": 4}); // result: 6

/* Complex expressions */
result = $sys().eval("(a + b) * c", {"a": 2, "b": 3, "c": 4}); // result: 20

/* User input processing */
user_input = "2 * (3 + 4)";
result = $sys().eval(user_input); // result: 14
```

## Using `op()` for Compiled Operations
- `op()(<string>)` parses the string into a $OP object (compiled execution tree)
- Creates reusable, optimized functions that can be executed multiple times
- Supports both positional and named parameters
- Enables advanced meta-programming patterns

```grapa
/* Basic compilation */
op_obj = op()("a + b");
result = op_obj({"a": 10, "b": 20}); // result: 30

/* One-line execution */
result = op()("5*3")(); // result: 15

/* With parameters */
func = op("name"=0)("'Hello, ' + name + '!'.echo();");
func("Grapa"); // Output: Hello, Grapa!

/* Dynamic function generation */
operations = ["add", "sub", "mul"];
funcs = {};
i = 0;
while (i < operations.len()) {
    op_name = operations.get(i);
    code = "a " + op_name + " b";
    funcs[op_name] = op("a"=0, "b"=0)(code);
    i += 1;
}

/* Execute generated functions */
funcs["add"](5, 3).echo();  // 8
funcs["mul"](5, 3).echo();  // 15
```

## Using `$sys().compile()` for Performance
- Pre-compiles scripts for maximum performance
- Creates $OP objects that can be executed with `$sys().eval()`
- Ideal for frequently executed code

```grapa
/* Compile once, execute many times */
compiled = $sys().compile("input * 2 + offset");

/* Execute with different parameters */
result1 = $sys().eval(compiled, {"input": 10, "offset": 5}); // 25
result2 = $sys().eval(compiled, {"input": 20, "offset": 10}); // 50
```

## Advanced Meta-Programming Patterns

### Template-Based Code Generation
```grapa
/* Create reusable templates */
template = "result = base * multiplier + offset; result";
process = op("base"=0, "multiplier"=1, "offset"=0)(template);
process(10, 2, 5).echo(); // 25
```

### Configuration-Driven Functions
```grapa
/* Generate functions from configuration */
config = {"operation": "add", "default": 0};
code = "a " + config.operation + " b";
func = op("a"=config.default, "b"=config.default)(code);
func(5, 3).echo(); // 8
```

## When to Use Each Method
- **`$sys().eval()`**: One-off evaluation, user input processing, simple expressions
- **`op()`**: Reusable functions, meta-programming, dynamic code generation
- **`$sys().compile()`**: Performance-critical code, frequently executed operations

## Why Grapa's Dynamic Execution is Superior
1. **Human-readable execution trees**: Unlike bytecode, $OP objects can be examined and understood
2. **Direct manipulation**: Execution trees can be created, modified, and executed directly
3. **Compile-time optimization**: Constant folding and expression simplification
4. **Parameter binding**: Both positional and named parameters with default values
5. **Type safety**: Execution trees maintain type information
6. **Performance**: Compiled execution trees are highly optimized

See also: [Advanced Scripting](../grc_scripts.md), [$OP Type](../type/OP.md), [System Functions](../sys/sys.md)
*/ 