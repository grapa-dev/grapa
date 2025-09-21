---
tags:
  - user
  - highlevel
  - api
---

# Grapa API Reference

> **For High-Level Grapa Programmers:** This reference covers the functions and types you'll use most often when writing Grapa scripts and applications.

> **For Advanced Users:** See the maintainer documentation for low-level functions, grammar development, and internal APIs.

## Table of Contents
- [Core Language](#core-language)
- [Dynamic Code Execution](#dynamic-code-execution)
- [Variables & Scope](#variables-scope)
- [Data Structures](#data-structures)
- [Search & Analysis](#search-analysis)
- [Control Flow](#control-flow)
- [Functional Programming](#functional-programming)
- [Type & I/O](#type-io)
- [Math Operations](#math-operations)
- [String Operations](#string-operations)
- [Array/Matrix Operations](#arraymatrix-operations)
- [File System](#file-system)
- [Time & Date](#time-date)
- [AI/ML Model Integration](#aiml-model-integration)
- [Python Integration](#python-integration)
- [Universal Object Methods](#universal-object-methods)

---

## Core Language

Essential language constructs for writing Grapa scripts.

### Comments
Grapa supports both block comments (`/* ... */`) and line comments (`// ...`). `#` comments are not supported.

**Comment Rules:**
- Comments can be used anywhere in your code, including at the end of lines
- Comments work inside `{}` blocks, class definitions, and function bodies

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

Grapa uses dynamic typing with rich runtime type introspection. **Everything in Grapa is data** - functions, classes, primitives, and everything else are just different data types that can be manipulated, stored, and passed around.

#### **Dynamic Typing with Runtime Safety**

Grapa's type system provides the flexibility of dynamic typing with the safety of runtime type checking:

```grapa
/* Dynamic type assignment */
x = 42;        /* $INT */
x = "hello";   /* $STR */
x = [1, 2, 3]; /* $LIST */

/* Runtime type checking */
if (x.type() == $LIST) {
    /* Safe to use array methods */
    result = x.map(op(item) { item * 2; });
}
```

#### **Type Introspection**

Every value in Grapa supports type introspection:

```grapa
/* Get type information */
$local.value = 42;
type = $local.value.type();  /* $INT */

/* Type comparison */
if ($local.value.type() == $INT) {
    /* Handle integer */
} elseif ($local.value.type() == $STR) {
    /* Handle string */
}
```

#### **Unified Data Model**

Grapa's unified data model means everything is a data type:

```grapa
/* Functions are data */
func = op(x) { x * 2; };
func_type = func.type();  /* $OP */

/* Classes are data */
class_obj = $LIST;
class_type = class_obj.type();  /* $CLASS */

/* Rules are data */
rule_obj = rule <$expression> '+' <$expression> {@<add,{$1,$3}>};
rule_type = rule_obj.type();  /* $RULE */
```

#### **Type Conversion**

Grapa provides automatic and explicit type conversion:

```grapa
/* Automatic conversion */
result = "42".int() + 8;  /* 50 */

/* Explicit conversion */
str_value = 42.str();     /* "42" */
int_value = "42".int();   /* 42 */
float_value = "3.14".float(); /* 3.14 */
```

#### **Type Safety Patterns**

Best practices for type safety in Grapa:

```grapa
/* Defensive programming */
process_data = op(data) {
    if (data.type() != $LIST) {
        return $ERR("Expected array");
    }
    
    /* Safe to process as array */
    return data.map(op(item) { item * 2; });
};

/* Type checking with fallbacks */
safe_get = op(obj, key) {
    if (obj.type() != $GOBJ) {
        return null;
    }
    
    $local.value = obj[key];
    if ($local.value.type() == $ERR) {
        return null;
    }
    
    return $local.value;
};
```

#### **Core Type Methods**

- `.type()` - Get the type of an object (returns `$STR`)
- `.str()` - Convert to string
- `.int()` - Convert to integer
- `.float()` - Convert to float
- `.bool()` - Convert to boolean

### Language Extensibility
Grapa's most powerful feature is its ability to extend the language syntax at runtime. You can define custom commands and functions that become part of the language grammar.

**Key Concepts:**
- **`$custom_command`**: For commands that perform actions (no return value)
- **`$custom_function`**: For functions that return values
- **Scoping**: Local (function-level) or global (permanent) syntax extensions
- **Dynamic Compilation**: New syntax applies to code compiled with `op(parse)()`

```grapa
/* Define custom syntax */
$custom_function = rule select $INT {op(p:$2){p*5}};

/* Use it as if it were built into the language */
select 4;        // Returns 20
x = select 8;    // x = 40
```

For comprehensive documentation, see [Language Syntax Extension](type/rule.md#language-syntax-extension).

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
    $local.local_var = x * 2;  /* In function's local namespace */
    global_var = config;  /* Access global namespace */
    $local.local_var.echo();
};
```

### Basic Operations
- `.echo()` - Output text to console (method on strings/values)
- `.debug(level, component?)` - Output debug messages (requires `-d` flag)
- `.\n` - Exit console (in interactive mode)
- `sleep(seconds)` - Pause execution

### Comments
```grapa
/* This is a block comment */
/* Both block and line comments are supported in Grapa */
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

### Object Merging Operators
- `+=` - Append/nest second object within first
- `++=` - Merge/flatten properties from both objects. For rules, concatenates additional rule alternatives (see [Rule Composition](type/rule.md#rule-composition-and-concatenation))

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

/* Object merging */
user = {"name": "John"};
profile = {"age": 30, "email": "john@example.com"};

/* Append (nested) */
user += profile;
user.echo();  /* {"name":"John",{"age":30,"email":"john@example.com"}} */

/* Merge (flattened) */
user = {"name": "John"};
user ++= profile;
user.echo();  /* {"name":"John","age":30,"email":"john@example.com"} */
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

Control program execution and logic flow with proper propagation through all execution contexts.

### Conditionals
```grapa
if (condition) {
    /* code */
} else {
    /* code */
}
```

### Loops with Control Flow
```grapa
/* While loop with break and continue */
i = 0;
while (i < 5) {
    if (i == 3) {
        break;  /* Exit loop immediately */
    };
    if (i == 1) {
        i += 1;
        continue;  /* Skip to next iteration */
    };
    ("Iteration " + i.str()).echo();
    i += 1;
};

/* For loop with control flow */
for (i = 0; i < 5; i += 1) {
    if (i == 3) {
        break;  /* Exit loop immediately */
    };
    if (i == 1) {
        continue;  /* Skip to next iteration */
    };
    ("Iteration " + i.str()).echo();
};
```

### Functions with Control Flow
```grapa
/* Function with return statements */
my_function = op(a, b) {
    if (a < 0) {
        return 0;  /* Early return */
    };
    if (b == 0) {
        return a;  /* Early return */
    };
    return a / b;  /* Normal return */
};

/* Function with exit */
critical_function = op() {
    if (error_condition) {
        exit;  /* Terminate program */
    };
    return "success";
};
```

### Switch Statements
```grapa
/* Switch with control flow */
switch (value) {
    case 1: return "one";;
    case 2: return "two";;
    default: return "unknown";;
}
```

### Control Flow in Inline Code Blocks
```grapa
/* Control flow in inline code blocks */
"hi".{if (true) return 999; x=@$$; x.len()}.range();  /* Returns 999 */

/* Control flow propagates through all contexts */
func test() { 
    "hi".{if (true) return 999; x=@$$; x.len()}.range(); 
    return 888; 
}; 
test();  /* Returns 999 */
```

### Control Flow Propagation
All control flow statements properly propagate through all execution contexts:
- **Functions**: RETURN statements exit functions and propagate return values
- **Loops**: BREAK exits loops, CONTINUE skips to next iteration  
- **Switches**: RETURN statements exit switch statements
- **Inline Code Blocks**: Control flow properly terminates block execution
- **Reduce Operations**: Control flow properly terminates reduce and propagates values
- **Nested Contexts**: Control flow propagates through all nested execution contexts

[Back to Top](#grapa-api-reference)

## Functional Programming

Parallel and sequential data processing functions.

### Map, Filter, Reduce
- `map(data, function)` - Apply function to each element (parallel)
- `filter(data, function)` - Filter elements (parallel)
- `reduce(data, function, [initial])` - Reduce to single value (sequential)

### Examples
```grapa
data = [1, 2, 3, 4, 5];

/* Map (parallel) */
doubled = data.map(op(x) { x * 2; });

/* Filter (parallel) */
evans = data.filter(op(x) { x % 2 == 0; });

/* Reduce (sequential) - with initializer */
sum = data.reduce(op(acc, x) { acc + x; }, 0);

/* Reduce (sequential) - without initializer */
sum = data.reduce(op(acc, x) { acc + x; });

/* Reduce with control flow */
result = data.reduce(op(acc, x) { 
    if (x == 3) return 999;  /* Early return */
    if (x == 4) break;       /* Early termination */
    acc + x; 
}, 0);
```

> **Note:** `map` and `filter` are parallel by default and production-ready for high-throughput data processing. `reduce` supports all control flow statements (break, return, throw) with proper propagation. `map` and `filter` support `.throw()` (collects all results in array), `.return()` (returns value for specific item), and `.break()` (returns empty string for specific item). All functional methods now have complete control flow support with enhanced error collection.

### Advanced Functional Programming: Complex Context Objects

Functional methods support **any data structure as initializers**, enabling sophisticated state machines and context-aware operations:

```grapa
/* Stateful reduction with dynamic operation switching */
[1, 2, 3].reduce(op(x, y) {
    if (y == 2) x.o = op(a, b) { a * b; };  /* Switch to multiplication */
    x.st = x.o(x.st, y);  /* Apply current operation */
}, {st: 10, o: op(a, b) { a + b; }});  /* Initial state object */

/* WARNING: Context object mutations cause race conditions */
[1, 2, 3].map(op(x, y) {
    y.count += 1;        /* Race condition: multiple threads may read same value */
    y.sum += x;          /* Race condition: multiple threads may read same value */
    x * y.multiplier;
}, {count: 0, sum: 0, multiplier: 2});

/* Better approach: Use .map() for computation, .reduce() for aggregation */
[1, 2, 3].map(op(x, y) {
    x * y.multiplier;    /* Pure computation - no mutations */
}, {multiplier: 2}).reduce(op(a, b) {
    a += b;              /* Sequential aggregation */
});

/* Filter with read-only context (passed by reference) */
[1, 2, 3, 4, 5].filter(op(x, y) {
    x > y.threshold;     /* Pure filtering - no mutations */
}, {threshold: 2});
```

**Key Capabilities:**
- **Any Data Structure**: Use objects, lists, arrays as initializers
- **Context Evolution**: Data structures can evolve during operations
- **Dynamic Operations**: Functions can be stored and modified within context
- **State Machines**: Build complex state machines with multiple operations
- **Pipeline Processing**: Create sophisticated data processing workflows

**Design Philosophy:**
Grapa's **dynamic typing** enables this flexibility by leaving type handling to the implementation rather than enforcing strict type constraints. This allows runtime type evolution, dynamic function assignment, and context-aware processing where types emerge from usage patterns.

### Concurrency Coordination with Functional Methods

Grapa's `.map()` and `.filter()` methods can be used as **thread synchronization barriers** for coordinating multiple worker threads:

```grapa
/* Worker thread coordination */
workers = [1, 2, 3, 4, 5].map(op(worker_id) {
    /* Each worker does independent work */
    ("Worker " + worker_id.str() + " starting").echo();
    sleep(worker_id);  /* Simulate work */
    ("Worker " + worker_id.str() + " completed").echo();
    worker_id * 100;  /* Return result */
});
/* All workers complete before proceeding */
("All workers finished").echo();

/* Parallel task execution */
tasks = [
    op() { "Task A: Database query".echo(); sleep(2); "A done".echo(); },
    op() { "Task B: API call".echo(); sleep(1); "B done".echo(); },
    op() { "Task C: File processing".echo(); sleep(3); "C done".echo(); }
];
results = tasks.map(op(task) { task(); });
/* All tasks complete before proceeding */
("All tasks completed").echo();
```

**Key Benefits:**
- **Structured Concurrency**: Automatic synchronization barrier
- **Worker Coordination**: Perfect for parallel task execution
- **Resource Management**: Coordinate multiple resource operations
- **System Initialization**: Parallel component initialization
- **No Manual Thread Management**: Automatic coordination and cleanup

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
$local.value = "42";
number = int($local.value);
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
- `.casefold()` - Convert to Unicode case folded form (for case-insensitive comparisons)
- `.trim([chars])` - Remove characters from both ends (default: space)
- `.ltrim([chars])` - Remove characters from left end (default: space)  
- `.rtrim([chars])` - Remove characters from right end (default: space)
- `.split(delimiter)` - Split string
- `.join(array)` - Join array into string
- `.replace(old, new)` - Replace text
- `.grep(pattern, options)` - Search with regex

> **See Also:** [String Templates and Dynamic Construction](type/str.md#string-templates-and-dynamic-construction) for advanced string construction patterns using templates, dynamic execution, and parameterized functions.

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
- `$file().getfield(path, field)` - Read structured data field
- `$file().setfield(path, field, content)` - Write structured data field
- `$file().set($stdout, content)` - Write to standard output
- `$file().set($stderr, content)` - Write to standard error
- `$file().ls(path)` - List directory contents
- `$file().info(path)` - Get file information

### Examples
```grapa
/* Read file */
content = $file().get("data.txt");

/* Write file */
$file().set("output.txt", "Hello from Grapa!");

/* Structured data operations */
$file().setfield("user1", "name", "John Doe");
$file().setfield("user1", "age", 30);
name = $file().getfield("user1", "name");
age = $file().getfield("user1", "age");

/* Write to standard streams */
$file().set($stdout, "Output message\n");
$file().set($stderr, "Error message\n");

/* List files */
files = $file().ls(".");
for (file in files) {
    full_path = file["$PATH"] + "/" + file["$KEY"];
    (file["$TYPE"] + ": " + full_path).echo();
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

---

## AI/ML Model Integration

Grapa provides native support for AI/ML models through the `$MODEL` data type, currently supporting LLAMA.cpp-based models for text generation.

### Core Functions

#### **`$MODEL()`**
Creates a new model instance.

```grapa
model = $MODEL();
```

#### **`.load(path, backend)`**
Loads a model from the specified file path.

**Parameters:**
- `path` (string): Path to the model file (GGUF format)
- `backend` (string, optional): Backend to use ("llama" is currently the only supported backend)

**Returns:** Error code (0 for success)

```grapa
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf", "llama");
if (result == 0) {
    "Model loaded successfully!".echo();
}
```

#### **`.gen(prompt, params)`**
Generates text using the loaded model.

**Parameters:**
- `prompt` (string): Input text prompt
- `params` (object, optional): Generation parameters to override defaults

**Returns:** Generated text string

```grapa
/* Basic generation */
response = model.gen("Hello, how are you?");

/* Generation with custom parameters */
response = model.gen("Explain AI", {
    "temperature": 0.8,
    "max_tokens": 100
});
```

#### **`.info()`**
Returns information about the current model state.

**Returns:** Object containing model information

```grapa
info = model.info();
("Model loaded: " + info.loaded.str()).echo();
("Backend: " + info.backend.str()).echo();
("Path: " + info.path.str()).echo();
```

#### **`.params()`**
Returns current generation parameters.

```grapa
params = model.params();
("Current temperature: " + params.temperature.str()).echo();
("Max tokens: " + params.max_tokens.str()).echo();
```

#### **`.params(parameters)`**
Sets generation parameters using a `$GOBJ` collection.

```grapa
model.params({
    "temperature": 0.7,
    "max_tokens": 50,
    "verbose": 2
});
```

#### **`.unload()`**
Unloads the current model and frees memory.

**Returns:** Error code (0 for success)

```grapa
result = model.unload();
if (result == 0) {
    "Model unloaded successfully".echo();
}
```

### Generation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `temperature` | float | 0.7 | Controls randomness (0.0 = deterministic, 1.0+ = more random) |
| `max_tokens` | int | 10 | Maximum number of tokens to generate |
| `top_k` | int | 40 | Number of top tokens to consider |
| `top_p` | float | 0.9 | Nucleus sampling threshold |
| `repeat_penalty` | float | 1.1 | Penalty for repeating tokens |
| `seed` | int | -1 | Random seed (-1 for random) |
| `context_size` | int | 2048 | Context window size |
| `verbose` | int | 0 | Logging verbosity (0=silent, 1=errors, 2=warnings, 3=info, 4=debug) |

### Complete Example

```grapa
/* Complete AI model usage example */
model = $MODEL();

/* Configure parameters */
model.params({
    "temperature": 0.8,
    "max_tokens": 100,
    "verbose": 0
});

/* Load model */
result = model.load("models/qwen2.5-7b-instruct-q3_k_m.gguf");
if (result == 0) {
    "Model loaded successfully!".echo();
    
    /* Generate text */
    response = model.gen("What is artificial intelligence?");
    ("Response: " + response.str()).echo();
    
    /* Generate with different parameters */
    creative_response = model.gen("Write a poem about space", {
        "temperature": 1.2,
        "max_tokens": 150
    });
    ("Creative response: " + creative_response.str()).echo();
} else {
    ("Failed to load model: " + result.str()).echo();
}

/* Clean up */
model.unload();
```

### Model Formats

Currently supports:
- **GGUF**: The standard format for LLAMA.cpp models
  - Efficient quantization (Q2_K, Q3_K_M, Q4_K_M, Q5_K_M, Q6_K, Q8_0, FP16)
  - Cross-platform compatibility
  - Metadata inclusion

### Backend Support

- **LLAMA.cpp**: For GGUF format models
  - Supports various quantization levels
  - Optimized for CPU inference
  - Thread-safe operations

### Performance Considerations

- **Memory Usage**: Large models (7B+ parameters) require significant RAM
- **Loading Time**: Initial model loading can take several seconds
- **Generation Speed**: Depends on model size and hardware capabilities
- **Context Size**: Larger context windows use more memory

### Error Handling

```grapa
model = $MODEL();
result = model.load("model.gguf");
if (result != 0) {
    ("Error loading model: " + result.str()).echo();
    return;
}

/* Use model safely */
response = model.gen("Hello");
```

### Thread Safety

The `$MODEL` type is designed to be thread-safe, allowing multiple model instances to be used concurrently.

```grapa
/* Safe to use multiple instances */
model1 = $MODEL();
model2 = $MODEL();

model1.load("model.gguf");
model2.load("model.gguf");

/* These can run concurrently */
response1 = model1.gen("Hello");
response2 = model2.gen("Hi");
```

For complete documentation, see:
- [$MODEL Data Type](type/model.md) - Complete type reference
- [Model Examples Guide](model_examples.md) - Comprehensive usage examples
- [Model Download Guide](model_download.md) - How to download models
- [Model API Reference](api/model.md) - Complete API documentation
- [Model Basic Example](examples/model_basic_example.grc) - Basic usage example
- [Model Parameter Example](examples/model_parameter_example.grc) - Parameter configuration
- [Model Error Handling Example](examples/model_error_handling_example.grc) - Error handling patterns

[Back to Top](#grapa-api-reference)

---

## Python Integration

GrapaPy provides a Python extension that exposes Grapa's core functionality directly to Python applications.

### Installation
```bash
pip install grapapy
```

**Note:** GrapaPy includes built-in dependency checking. If installation fails due to missing system dependencies, the installer will provide detailed error messages and installation instructions for your platform.

### Core Functions

#### **grapapy.eval(script, params, rule, profile)**
Evaluate Grapa code from Python with optional parameters.

```python
import grapapy

# Basic evaluation
result = grapapy.eval("'Hello, World!'.echo();")
print(result)  # "Hello, World!"

# With parameters
result = grapapy.eval("x + y", {"x": 5, "y": 3})
print(result)  # 8
```

#### **grapapy.compile(script, rule, profile)**
Pre-compile Grapa code for performance.

```python
# Compile for reuse
compiled = grapapy.compile("input * 2 + offset")
result = grapapy.eval(compiled, {"input": 10, "offset": 5})
print(result)  # 25
```

#### **grapapy.grep(input, pattern, options, delim, normstr, procstr, num_workers)**
Direct access to Grapa's Unicode-aware grep functionality.

**Parameters:**
- `input` - Text to search in
- `pattern` - Regex pattern to search for
- `options` - Grep options (e.g., "o" for match-only, "i" for case-insensitive)
- `delim` - Line delimiter (default: "\n")
- `normstr` - Unicode normalization ("NONE", "NFC", "NFD", "NFKC", "NFKD")
- `procstr` - Processing mode ("UNICODE", "BINARY")
- `num_workers` - Number of worker threads (0 for auto)

```python
import grapapy

text = "Hello world\nGoodbye world\nPython programming"

# Basic search
matches = grapapy.grep(text, "world")
print(matches)  # ['Hello world', 'Goodbye world']

# Match-only option
matches = grapapy.grep(text, "world", "o")
print(matches)  # ['world', 'world']

# Case-insensitive search
matches = grapapy.grep(text, "python", "i")
print(matches)  # ['Python programming']

# Advanced options
matches = grapapy.grep(text, "\\b\\w{6}\\b", "o")  # 6-letter words
print(matches)  # ['Goodbye', 'Python']
```

### Class-Based Interface

#### **grapapy.grapa()**
Create a Grapa instance for stateful operations.

```python
import grapapy

# Create instance
g = grapapy.grapa()

# Variables persist between calls
g.eval("x = 42;")
result = g.eval("x * 2;")
print(result)  # 84

# Use grep with instance
matches = g.grep("hello world", "hello")
print(matches)  # ['hello world']
```

### Execution Models

#### **Direct Execution (No Parameters)**
Variables persist between calls:
```python
g = grapapy.grapa()
g.eval("b = 6;")
result = g.eval("b;")  # Returns 6
```

#### **Parameterized Execution (With Parameters)**
Variables are local and don't persist:
```python
g.eval("c = 9;", {'x': 5})
result = g.eval("c;")  # Returns {"error":-1} - variable not found

# Use $global for persistence with parameters
g.eval("$global.persistent_var = 10;", {'x': 5})
result = g.eval("persistent_var;")  # Returns 10
```

### Advanced Features

#### **Unicode-Aware Text Processing**
```python
# Unicode normalization
text = "café naïve résumé"
matches = grapapy.grep(text, "cafe", "i", "", "NFD")  # Normalized search

# Diacritic-insensitive search
matches = grapapy.grep(text, "cafe", "d")  # Matches "café"
```

#### **Performance Optimization**
```python
# Parallel processing
large_text = "..." * 10000
matches = grapapy.grep(large_text, "pattern", "", "", "", "", 4)  # 4 workers
```

#### **Error Handling**
```python
try:
    result = grapapy.grep("text", "[invalid regex")
    if isinstance(result, dict) and "error" in result:
        print(f"Grep error: {result['error']}")
except Exception as e:
    print(f"Python error: {e}")
```

### Integration Examples

#### **Data Science Workflow**
```python
import grapapy
import pandas as pd

# Extract data with grep
log_data = grapapy.grep(log_file_content, "ERROR|WARNING", "o")
df = pd.DataFrame({"messages": log_data})

# Process with Grapa
g = grapapy.grapa()
g.eval("processed_data = [];")
for message in log_data:
    g.eval("processed_data += message.upper();", {"message": message})
```

#### **Web Application**
```python
from flask import Flask
import grapapy

app = Flask(__name__)

@app.route('/search')
def search():
    query = request.args.get('q')
    content = get_content()
    matches = grapapy.grep(content, query, "i")
    return jsonify({"matches": matches})
```

### Best Practices

1. **Use class-based interface** for stateful operations
2. **Pre-compile frequently used code** for better performance
3. **Handle errors gracefully** - grep can return error dictionaries
4. **Use appropriate Unicode normalization** for international text
5. **Leverage parallel processing** for large text operations

[Back to Top](#grapa-api-reference)

---

## Universal Object Methods

All Grapa objects support a comprehensive set of methods through the `$OBJ` class. These methods provide type conversion, manipulation, and utility functions that work on any data type.

### Key Method Categories

- **Type Conversion**: `.float()`, `.int()`, `.str()`, `.bool()`, `.base()`
- **String Manipulation**: `.left()`, `.right()`, `.mid()`, `.trim()`, `.replace()`, `.interpolate()`
- **Array Operations**: `.split()`, `.join()`, `.sort()`, `.unique()`, `.group()`
- **Functional Programming**: `.map()`, `.filter()`, `.reduce()`, `.range()`
- **Search & Pattern**: `.grep()`, `.match()`, `.findall()`
- **Bit Manipulation**: `.setbit()`, `.clearbit()`, `.genbits()`
- **Cryptography**: `.encode()`, `.decode()`, `.sign()`, `.verify()`, `.secret()`
- **Universal Access**: `.get()`, `.set()` - works across `$LIST`, `$GOBJ`, `$OBJ`, `$file`, `$TABLE`, `$WIDGET`
- **Flexible Field Access**: `.getfield()`, `.setfield()` - universal methods (mix/match of names and indices for `$LIST`, `$GOBJ`, and `$OBJ` non-system classes only)
- **Utilities**: `.echo()`, `.debug()`, `.iferr()`, `.exec()`, `.getname()`

### Quick Examples

```grapa
/* Type conversion with precision */
result = "3.14159".float(4, 0).base(16);

/* String processing pipeline */
text = "  Hello, World!  ";
processed = text.trim().upper().replace("WORLD", "GRAPA");

/* Functional array processing */
data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
result = data
    .filter(op(x) { x % 2 == 0; })
    .map(op(x) { x * x; })
    .reduce(op(acc, x) { acc + x; }, 0);
```

### Universal .get()/.set() Methods

The universal `.get()` and `.set()` methods provide consistent access patterns across multiple data types:

```grapa
/* $LIST examples */
my_array = [1, 2, 3, 4, 5];
my_array.set(2, 99);
$local.value = my_array.get(2);  /* 99 */

/* $GOBJ examples */
my_list = {"name": "Alice", "age": 30};
my_list.set("name", "Bob");
name = my_list.get("name");  /* "Bob" */

/* $OBJ examples */
my_obj = {};
my_obj.set("property", "value");
$local.value = my_obj.get("property");  /* "value" */

/* $file examples */
my_file = $file();
my_file.set("test.txt", "Hello World");
content = my_file.get("test.txt");  /* "Hello World" */

/* $TABLE examples */
my_table = {}.table("ROW");
my_table.mkfield("name", "STR", "VAR");
my_table.set("user1", "name", "Alice");
name = my_table.get("user1", "name");  /* "Alice" */
```

**Supported Types:**
- **`$LIST`**: `array.get(index)`, `array.set(index, value)` - 0-based indexing
- **`$GOBJ`**: `list.get(key)`, `list.set(key, value)`
- **`$OBJ`**: `obj.get(key)`, `obj.set(key, value)`
- **`$file`**: `file.get(key)`, `file.set(key, value)`
- **`$TABLE`**: `table.get(key, field)`, `table.set(key, field, value)`
- **`$WIDGET`**: `widget.get(name, params)`, `widget.set(name, data)`

For complete documentation of all `$OBJ` methods with their parameters and examples, see **[$OBJ Methods Reference](type/obj_methods.md)**.

[Back to Top](#grapa-api-reference)