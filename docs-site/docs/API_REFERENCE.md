---
tags:
  - user
  - highlevel
  - api
---

# Grapa API Reference

> **For High-Level Grapa Programmers:** This reference covers the functions and types you'll use most often when writing Grapa scripts and applications.

> **For Advanced Users:** See [System Functions (Advanced)](../maintainers/IMPLEMENTATION/SYSTEM_FUNCTIONS.md) for low-level functions, grammar development, and internal APIs.

## Table of Contents
- [Core Language](#core-language)
- [Dynamic Code Execution](#dynamic-code-execution)
- [Variables & Scope](#variables-scope)
- [Data Structures](#data-structures)
- [Search & Analysis](#search--analysis)
- [Control Flow](#control-flow)
- [Functional Programming](#functional-programming)
- [Type & I/O](#type--io)
- [Math Operations](#math-operations)
- [String Operations](#string-operations)
- [Array/Matrix Operations](#arraymatrix-operations)
- [File System](#file-system)
- [Time & Date](#time--date)
- [Usage Examples](#usage-examples)

---

## Core Language

Essential language constructs for writing Grapa scripts.

### Comments
Grapa only supports block comments (`/* ... */`). Line comments (`// ...`) are not supported.

**Comment Rules:**
- Outside `{}` blocks: Comments can be at the end of lines
- Inside `{}` blocks: Comments must be on their own line
- Class/function definitions: Comments must be on their own line

```grapa
/* ✅ Correct */
x = 5;  /* This works outside {} */

if (condition) {
    /* This comment is correct */
    x = 5;
}

/* ❌ Incorrect */
if (condition) {
    x = 5;  /* This will cause a syntax error */
}
```

### Type System
Grapa uses dynamic typing with rich runtime type introspection:

- `.type()` - Get the type of an object (returns `$STR`)
- `.str()` - Convert to string
- `.int()` - Convert to integer
- `.float()` - Convert to float
- `.bool()` - Convert to boolean

```grapa
/* Type checking */
if (value.type() == $INT) { ... }
if (value.type() == $STR) { ... }
if (value.type() == $ARRAY) { ... }

/* Type conversion */
text = number.str();
number = text.int();
```

### Namespace System
Grapa has a dynamic namespace system managed by the execution tree:

- `$global` - Global namespace for persistent variables
- `$local` - Local namespace (automatic for each function)
- **Automatic namespace creation** for each execution context
- **Hierarchical access** to variables from parent namespaces

```grapa
/* Global variables */
$global.config = {"debug": true};

/* Function with local namespace */
func = op(x) {
    local_var = x * 2;  /* In function's local namespace */
    global_var = config;  /* Access global namespace */
    local_var.echo();
};
```

### Basic Operations
- `.echo()` - Output text to console (method on strings/values)
- `.\n` - Exit console (in interactive mode)
- `sleep(seconds)` - Pause execution

### Comments
```grapa
/* This is a block comment */
/* Line comments are not supported in Grapa */
```

[Back to Top](#grapa-api-reference)

## Dynamic Code Execution

Grapa's most powerful feature is its ability to compile and execute code at runtime. This enables advanced meta-programming patterns.

### Core Functions
- `op()(script)` - Compile string to executable function
- `$sys().eval(script, params)` - Evaluate script with parameters
- `$sys().compile(script)` - Pre-compile script for performance

### Examples
```grapa
/* Direct string execution */
op()("'Hello, World!'.echo();")();
/* Output: Hello, World! */

/* With parameters */
func = op("name"=0)("'Hello, ' + name + '!'.echo();");
func("Grapa");
/* Output: Hello, Grapa! */

/* System-level evaluation */
result = $sys().eval("x + y", {"x": 5, "y": 3});
result.echo();  /* 8 */

/* Compiled execution for performance */
compiled = $sys().compile("input * 2 + offset");
result = $sys().eval(compiled, {"input": 10, "offset": 5});
result.echo();  /* 25 */

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
funcs["add"](5, 3).echo();  /* 8 */
funcs["mul"](5, 3).echo();  /* 15 */
```

> **Note:** This is Grapa's core meta-programming capability, making it superior to most languages for dynamic code execution.

[Back to Top](#grapa-api-reference)

## Variables & Scope

Variable declaration, assignment, and scope management.

### Assignment
- `=` - Basic assignment
- `+=` - Append to arrays
- `global` - Declare global variable
- `local` - Declare local variable
- `const` - Declare constant

### Examples
```grapa
name = "Grapa";
count = 42;
numbers = [1, 2, 3];
numbers += 4;  /* Append to array */
```

[Back to Top](#grapa-api-reference)

## Data Structures

Create and manipulate arrays, lists, and objects.

### Array Operations
- `[item1, item2, ...]` - Create array
- `+=` - Append to array
- `len()` - Get array length
- `sum()` - Sum array elements
- `mean()` - Calculate mean

### List/Object Operations
- `{key: value, ...}` - Create list/object
- `.key` - Access object property
- `.keys()` - Get object keys
- `.values()` - Get object values

### Examples
```grapa
/* Arrays */
numbers = [1, 2, 3, 4, 5];
numbers += 6;
total = numbers.sum();
average = numbers.mean();

/* Objects */
config = {"host": "localhost", "port": 8080};
host = config.host;
```

[Back to Top](#grapa-api-reference)

## Search & Analysis

Search, sort, and analyze data.

### Search Functions
- `grep(pattern, options)` - Search text with regex
- `search(data, pattern)` - Search in data structures
- `findall(text, pattern)` - Find all matches

### Analysis Functions
- `sort(data)` - Sort data
- `unique(data)` - Remove duplicates
- `group(data, key)` - Group data by key

### Examples
```grapa
text = "Hello world\nGoodbye world";
matches = text.grep("world", "o");  /* ["world", "world"] */

data = [3, 1, 4, 1, 5];
sorted_data = data.sort();  /* [1, 1, 3, 4, 5] */
unique_data = data.unique();  /* [3, 1, 4, 5] */
```

[Back to Top](#grapa-api-reference)

## Control Flow

Control program execution and logic flow.

### Conditionals
```grapa
if (condition) {
    /* code */
} else {
    /* code */
}
```

### Loops
```grapa
/* While loop */
i = 0;
while (i < 5) {
    ("Iteration " + i.str()).echo();
    i += 1;
};

/* For-loop equivalent with range and map */
(5).range(0,1).map(op(i) { ("Iteration " + i.str()).echo(); });
```

### Functions
```grapa
/* Define function */
my_function = op(a, b) {
    a + b;
};

/* Call function */
result = my_function(10, 20);
```

[Back to Top](#grapa-api-reference)

## Functional Programming

Parallel and sequential data processing functions.

### Map, Filter, Reduce
- `map(data, function)` - Apply function to each element (parallel)
- `filter(data, function)` - Filter elements (parallel)
- `reduce(data, function, initial)` - Reduce to single value (sequential)

### Examples
```grapa
data = [1, 2, 3, 4, 5];

/* Map (parallel) */
doubled = data.map(op(x) { x * 2; });

/* Filter (parallel) */
evans = data.filter(op(x) { x % 2 == 0; });

/* Reduce (sequential) */
sum = data.reduce(op(acc, x) { acc + x; }, 0);
```

> **Note:** `map` and `filter` are parallel by default and production-ready for high-throughput data processing.

[Back to Top](#grapa-api-reference)

## Type & I/O

Type checking, conversion, and input/output operations.

### Type Functions
- `type(value)` - Get type of value
- `int(value)` - Convert to integer
- `float(value)` - Convert to float
- `str(value)` - Convert to string
- `bool(value)` - Convert to boolean

### I/O Functions
- `.echo()` - Output to console (method on strings/values)
- `prompt(message)` - Get user input

### Examples
```grapa
value = "42";
number = int(value);
text = str(number);

name = prompt("Enter your name: ");
("Hello, " + name).echo();
```

[Back to Top](#grapa-api-reference)

## Math Operations

Mathematical functions and operations.

### Basic Math
- `+`, `-`, `*`, `/` - Arithmetic operators
- `pow(base, exponent)` - Power function
- `abs(value)` - Absolute value
- `mod(value, divisor)` - Modulo operation

### Constants
- `pi` - Pi constant
- `e` - Euler's number

### Examples
```grapa
result = 4 * 3;
power = pow(2, 10);
absolute = abs(-42);
remainder = mod(17, 5);
```

[Back to Top](#grapa-api-reference)

## String Operations

String manipulation and analysis.

### String Methods
- `.len()` - Get string length
- `.upper()` - Convert to uppercase
- `.lower()` - Convert to lowercase
- `.trim()` - Remove whitespace
- `.split(delimiter)` - Split string
- `.join(array)` - Join array into string
- `.replace(old, new)` - Replace text
- `.grep(pattern, options)` - Search with regex

### Examples
```grapa
text = "  Hello, World!  ";
length = text.len();
upper = text.trim().upper();
words = text.split(", ");
joined = words.join(" - ");
```

[Back to Top](#grapa-api-reference)

## Array/Matrix Operations

Array and matrix manipulation.

### Array Functions
- `shape(array)` - Get array dimensions
- `reshape(array, dimensions)` - Reshape array
- `sum(array)` - Sum elements
- `mean(array)` - Calculate mean

### Matrix Functions
- `dot(matrix1, matrix2)` - Matrix multiplication
- `t(matrix)` - Transpose matrix

### Examples
```grapa
data = [1, 2, 3, 4, 5, 6];
matrix = reshape(data, [2, 3]);
transposed = t(matrix);
```

[Back to Top](#grapa-api-reference)

## File System

File and directory operations.

### File Operations
- `$file().get(path)` - Read file content
- `$file().set(path, content)` - Write to file
- `$file().ls(path)` - List directory contents
- `$file().info(path)` - Get file information

### Examples
```grapa
/* Read file */
content = $file().get("data.txt").str();

/* Write file */
$file().set("output.txt", "Hello from Grapa!");

/* List files */
files = $file().ls(".");
for (file in files) {
    ("File: " + file.name).echo();
}
```

[Back to Top](#grapa-api-reference)

## Time & Date

Time and date utilities.

### Time Functions
- `$time()` - Get current timestamp
- `$time().format(format)` - Format timestamp
- `$time().add(seconds)` - Add time
- `$time().sub(seconds)` - Subtract time

### Examples
```grapa
now = $time();
formatted = now.format("%Y-%m-%d %H:%M:%S");
tomorrow = now.add(86400);  /* Add 24 hours */
```

[Back to Top](#grapa-api-reference)