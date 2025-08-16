# Rust-to-Grapa Migration Guide

> **Important: Access Patterns for .get() and Indexing (Tested, v0.0.39)**
>
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|
> | $ARRAY    |      ✗      |     ✗      |       ✓         |      —       |
> | $LIST     |      ✗      |     ✗      |       ✓         |     ✓       |
> | $file     |      ✓      |     ✗      |        —         |      —       |
> | $TABLE    |     ✓*      |     ✗      |        —         |      —       |
> | $OBJ      |      ✗      |     ✗      |       ✗         |     ✓       |
> *$TABLE .get() requires two arguments: key and field.
>
> - For $LIST and $OBJ, use bracket or dot notation (e.g., obj["key"], obj.key, obj[2]).
> - For $ARRAY, use bracket notation (e.g., arr[1]).
> - Only $file and $TABLE support .get().
> - This is based on direct testing in Grapa v0.0.39.

---

This guide helps Rust users transition to Grapa by mapping common Rust idioms, patterns, and code to their Grapa equivalents.

> **See Also:**
> - [Basic Syntax Guide](../syntax/basic_syntax.md)
> - [Operator Reference](../syntax/operator.md)

## Syntax Mapping Table

| Rust | Grapa |
|------|-------|
| `let mut x = 5;` | `x = 5;` |
| `x += 1;` | `x += 1;` |
| `let s = String::from("hi");` | `s = "hi";` |
| `s.push_str("!");` | `s += "!";` |
| `let arr = vec![1,2,3];` | `arr = [1, 2, 3];` |
| `arr[0]` | `arr[0]`<br>`arr.get(0)` |
| `let map = HashMap::new();` | `obj = {}` |
| `map["key"]` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| `for x in arr { ... }` | `for x in arr { ... }`<br>`i = 0; while (i < arr.len()) { x = arr[i]; ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if cond { ... } else { ... }` | `if (cond) { ... } else { ... }` |
| `fn f(x: i32) -> i32 { ... }` | `f = op(x) { ... };` |
| `/* comment */` (block only, own line) | `/* comment */` (block)<br>`/** comment */` (doc block)<br>`// comment` (line)<br>`/// comment` (doc line) |
| `Some(x)` / `None` | `x` / `null` |
| `Result<T, E>` | `value or $ERR` |
| `match x { ... }` | `if/else` chain |
| `arr.iter().map(|x| x+1)` | `arr.map(op(x) { x + 1; })` |
| `arr.iter().filter(|x| *x > 0)` | `arr.filter(op(x) { x > 0; })` |
| `arr.iter().fold(0, |a, x| a + x)` | `arr.reduce(op(a, x) { a + x; }, 0)` |
| `arr.len()` | `arr.len()` |
| `format!("{} {}", a, b)` | `("" + a.str() + " " + b.str())` or see [String Templates and Dynamic Construction](../type/str.md#string-templates-and-dynamic-construction) for advanced patterns |

> **Note:** Both `x = x + 1;` and `x += 1;` (and `s = s + "x";` and `s += "x";`) are valid in Grapa. The `+=` form is idiomatic and preferred in most cases.
>
> **String Interpolation:** For combining strings and values, use `"Hello ${name}".interpolate()` instead of `"Hello " + name`. String interpolation is more powerful and less error-prone than concatenation. It also provides elegant solutions for complex method chaining: `"${'hello'.upper()} ${'world'.lower()}".interpolate()`.
>
> **Nullish Coalescing:** For providing default values, use `value.ifnull("default")` instead of Rust's `unwrap_or()` or `?` operator. The `.ifnull()` method treats a broader range of values as nullish (including zeros, empty collections, and errors).

> **Note:** `.get("key")` is only for `$file` and `$TABLE`. For `$LIST`/`$OBJ`, use `obj["key"]`, `obj.key`, or `obj."key"`. For `$ARRAY`, use `arr[index]` or `arr.get(index)`.

## Access Patterns: Objects, Lists, Arrays, Files, and Tables

Below are all valid ways to access elements in Grapa data structures. See the canonical [Basic Syntax Guide](../syntax/basic_syntax.md) for the latest tested rules.

### $LIST and $OBJ

```grapa
obj = {"a": 1, "b": 2, "c": 3};

value = obj["b"];      /* Returns 2 */
value = obj.key;        /* Returns value for key 'key' if present */
value = obj."b";       /* Returns 2 */

/* $LIST only: */
value = obj[1];         /* Returns 2 (by index) */
name = obj.getname(1);  /* Returns "b" (key name at index 1) */
```

- Dot notation (`obj.key`) and bracket notation (`obj["key"]`) are both valid for $LIST/$OBJ.
- `.get()` is NOT valid for $LIST/$OBJ.

### $ARRAY

```grapa
arr = [10, 20, 30];

value = arr[1];         /* Returns 20 */
value = arr.get(1);     /* Returns 20 */
```

- Use bracket notation or `.get(index)` for $ARRAY.
- Dot notation and `.get("key")` are NOT valid for $ARRAY.

### $file

```grapa
files = $file().ls();
file_info = files.get(0);   /* Correct */
```

- Always use `.get(index)` for $file results.
- Bracket and dot notation are NOT valid for $file.

### $TABLE

```grapa
table = $file().table("ROW");
table.mkfield("name", "STR", "VAR");
table.set("user1", "Alice", "name");

value = table.get("user1", "name");   /* Correct */
```

- Always use `.get(key, field)` for $TABLE.
- Bracket and dot notation are NOT valid for $TABLE.

> **Reference Table:**
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|
> | $ARRAY    |      ✗      |     ✗      |       ✓         |      —       |
> | $LIST     |      ✗      |     ✗      |       ✓         |     ✓       |
> | $file     |      ✓      |     ✗      |        —         |      —       |
> | $TABLE    |     ✓*      |     ✗      |        —         |      —       |
> | $OBJ      |      ✗      |     ✗      |       ✗         |     ✓       |
> *$TABLE .get() requires two arguments: key and field.

See [Basic Syntax Guide](../syntax/basic_syntax.md) for empirical test results and future updates.

## Common Pitfalls
- ✅ **For loops now supported** - Use `for x in arr { ... }` for native iteration
- No `match`—use `if/else` chains
- No `.push()`/`.append()`—use `+=` for append
- ✅ **Line comments now supported** - Use `// comment` or `/// doc comment` in addition to block comments
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads
- Use `.iferr()` for simple error fallback; use `if (result.type() == $ERR)` only for explicit error handling

## Example Code Pairs

**Rust:**
```rust
// Sum squares of even numbers
let result = arr.iter().filter(|x| *x % 2 == 0).map(|x| x * x).sum::<i32>();
```
**Grapa:**
```grapa
result = arr.filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(a, b) { a + b; }, 0);
```

**Rust:**
```rust
// Read file lines
use std::fs;
let lines = fs::read_to_string("file.txt").unwrap().lines().collect::<Vec<_>>();
```
**Grapa:**
```grapa
lines = $file().read("file.txt").split("\n");
```

**Rust:**
```rust
// HashMap access
let value = map["key"];
```
**Grapa:**
```grapa
value = obj["key"];
value = obj.key;
value = obj."key";
```

**Rust:**
```rust
// File access (custom struct or map)
let value = file["key"];
```
**Grapa:**
```grapa
value = file.get("key");
```

**Rust:**
```rust
// Generate numbers 0..9
let seq: Vec<_> = (0..10).collect();
```
**Grapa:**
```grapa
seq = (10).range(0,1);
```

**Rust:**
```rust
// Sum numbers 0..9
let sum: i32 = (0..10).sum();
```
**Grapa:**
```grapa
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
```

**Rust:**
```rust
// Collect even numbers 0..9
let evens: Vec<_> = (0..10).filter(|x| x % 2 == 0).collect();
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
```

**Rust:**
```rust
// Error fallback
let result = some_operation().unwrap_or(0);
```
**Grapa:**
```grapa
result = some_operation().iferr(0);
```

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

## See Also
- [Basic Syntax Guide](../syntax/basic_syntax.md)
- [Operator Reference](../syntax/operator.md)

---

*If you have more Rust idioms you want mapped to Grapa, please open an issue or PR!* 

### Custom match() Function for Regex

Rust users often use `regex::Regex::is_match` for regex checks. You can define a similar function in Grapa:

```grapa
// Define a match function that returns true if the pattern is found
match = op("text"="", "pattern"="") {
    text.grep(pattern, "x").len() > 0;
};

// Usage
if (match("hello world", "world")) {
    "Found!".echo();
} else {
    "Not found.".echo();
}
```

This is a handy workaround until Grapa adds a native `.match()` method.

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - If more objects support `.get()` in the future, this guide will be updated. 

> **Comment Style:**
> - ✅ **Comprehensive comment support**: `/* */` (block), `/** */` (doc block), `//` (line), `///` (doc line)
> - Comments work everywhere including at end of lines and inside code blocks 

## Work-in-Progress (WIP) Items

Some Rust idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Ownership system**: `let x = String::new();` - Grapa has automatic memory management
- **Static typing**: `let x: i32` - Grapa uses dynamic typing by design (see note below)
- **Borrowing**: `&mut x` - Grapa has no borrowing, all values are owned
- **Lifetimes**: `'a` - Grapa has automatic lifetime management
- **Unsafe blocks**: `unsafe { }` - Grapa is memory-safe by design
- **Raw pointers**: `*const T` - Grapa has no raw pointers
- **FFI**: `extern "C"` - No direct FFI support
- **Const generics**: `[T; N]` - No compile-time const generics
- **Higher-ranked trait bounds**: `for<'a>` - No higher-ranked types
- **Inline assembly**: `asm!()` - No inline assembly
- **Global allocators**: - No custom allocator support

> **Important Note on Dynamic Typing:**
> Grapa's dynamic typing is a **fundamental design choice**, not a limitation. It enables Grapa's core strengths:
> - **Dynamic code execution** and meta-programming capabilities
> - **Runtime type introspection** with `.type()` method
> - **Flexible data processing** without compile-time type constraints
> 
> **Why Grapa Doesn't Need Static Type Annotations:**
> Grapa's design philosophy prioritizes **runtime flexibility** over compile-time guarantees. Static type annotations would fundamentally change Grapa's nature and eliminate its core advantages:
> - **Meta-programming**: Grapa can modify its own code structure at runtime, which static typing would prevent
> - **Dynamic grammar changes**: Grapa's executable BNF system requires runtime type flexibility
> - **System integration**: Grapa can work with any data format without predefined type definitions
> - **Data processing**: Grapa's strength is handling diverse, changing data structures dynamically
> - **System integration** that doesn't require type definitions
> 
> Grapa provides type safety through runtime checking and rich type introspection, which is often more flexible than static typing for data processing and system integration tasks.

### Nice to Have
These would improve developer experience but aren't essential:

- **Nullish coalescing**: `x ?? y` - Use `.ifnull()` for superior nullish coalescing: `x.ifnull(y)`
- **Traits**: `trait MyTrait` - Use object composition and duck typing

### Property Access and Safe Navigation

**Why Grapa Doesn't Need Optional Chaining and Property Operators:**
Grapa's property access mechanisms are superior to Rust's approach:

- **Optional Chaining**: Use `.iferr()` for superior safe property access with custom fallback values
- **Property Existence**: Use `.type() != $ERR` for more explicit existence checking
- **Property Enumeration**: Use manual key iteration for more controlled enumeration

**Grapa's Superior Property Access:**
```grapa
/* Optional chaining equivalent - BETTER */
/* Rust: obj.user?.profile?.name */
name = obj.user.iferr(null).profile.iferr(null).name.iferr("default");

/* Property existence equivalent - MORE EXPLICIT */
/* Rust: obj.contains_key("name") */
if (obj.name.type() != $ERR) {
    /* Property exists and is accessible */
}

/* Property enumeration equivalent - MORE CONTROLLED */
/* Rust: for (key, value) in obj.iter() */
keys = ["name", "age", "city"];
keys.map(op(key) {
    value = obj[key].iferr("not set");
    (key + ": " + value.str()).echo();
});
```

**Advantages over Rust's Approach:**
- **Custom fallback values** at each step (not just `None`)
- **Explicit existence checking** with `.type()` method
- **Controlled enumeration** without ownership complexity
- **Functional programming patterns** over imperative loops
- **Better error handling** with explicit fallback values
- **No ownership complexity** - automatic memory management
- **Generics**: `<T>` - Grapa has dynamic typing
- **Pattern matching**: `match x { Some(y) => y, None => 0 }` - Use `if/else` chains
- **Result types**: `Result<T, E>` - Use `.iferr()` or explicit error checking
- **Option types**: `Option<T>` - Use explicit null checks
- **Macros**: `macro_rules!` - Use Grapa's built-in code generation
- **Associated types**: `type Output` - Use regular types
- **Default implementations**: - Use regular method definitions
- **Trait bounds**: `where T: Display` - Use dynamic typing
- **Impl blocks**: `impl MyStruct` - Use object constructors
- **Derive macros**: `#[derive(Debug)]` - Use regular methods
- **Structs**: `struct MyStruct` - Use object constructors
- **Enums**: `enum MyEnum` - Use objects with type fields
- **Modules**: `mod my_module` - Use Grapa's file system
- **Crates**: - Use Grapa's library system
- **Cargo**: - Use Grapa's build system
- **Workspaces**: - Use directory organization
- **Features**: `[features]` - Use conditional compilation
- **Dependencies**: - Use Grapa's dependency management
- **Publishing**: - Use Grapa's distribution system
- **Documentation**: `///` - Use regular comments

### Rarely Used
These are advanced features that most developers won't miss:

- **Unions**: `union MyUnion` - Use regular types
- **Bit fields**: - Use bitwise operations
- **Inline assembly**: `asm!()` - Use Grapa's system calls
- **Global allocators**: - Use Grapa's memory management
- **Custom target specifications**: - Use Grapa's platform abstraction
- **Linker scripts**: - Use Grapa's build system
- **Platform intrinsics**: - Use Grapa's system integration
- **SIMD**: - Use Grapa's parallel processing
- **Atomic operations**: - Use Grapa's threading
- **Memory ordering**: - Use Grapa's memory model

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, Rust's ownership system is replaced by Grapa's automatic memory management, and Rust's async/await is replaced by Grapa's built-in parallel processing.

### Data Structures and Collections

**Why Grapa Doesn't Need Specialized Collections:**
Grapa's unified data structure approach is superior to Rust's specialized collections:

- **Sets**: Use `.unique()` method on arrays: `[1, 2, 1, 3, 2].unique()` → `[1, 2, 3]`
- **Maps**: Use `$LIST` objects: `{key1: "value1", key2: "value2"}`
- **Vectors**: Use `$ARRAY` with dynamic operations: `arr += new_element`
- **HashMaps**: Use `$LIST` objects with key-value pairs
- **Iterators**: Use functional methods (`.map()`, `.filter()`, `.reduce()`) which are thread-safe and parallel

**Grapa's Superior Collection Capabilities:**
```grapa
/* Set-like behavior */
set_like = [1, 2, 1, 3, 2].unique();  /* [1, 2, 3] */

/* Map-like behavior */
map_like = {key1: "value1", key2: "value2"};

/* Vector-like operations */
vector = [1, 2, 3];
vector += 4;        /* Push */
vector += 0 vector[0]; /* Insert at beginning */

/* Parallel functional operations */
squares = numbers.map(op(x) { x * x });           /* Parallel processing */
evens = numbers.filter(op(x) { x % 2 == 0 });     /* Thread-safe */
sum = numbers.reduce(op(a, b) { a + b }, 0);      /* Sequential reduction */
```

**Advantages over Rust's Collections:**
- **Unified syntax** across all data types
- **Parallel processing** built into functional methods
- **Cross-format compatibility** (works on `$ARRAY`, `$LIST`, `$OBJ`, `$XML`, etc.)
- **Simpler learning curve** - fewer specialized types to learn
- **Better performance** - optimized for Grapa's execution model
- **No ownership complexity** - automatic memory management

## See Also
- [Basic Syntax Guide](../syntax/basic_syntax.md)
- [Operator Reference](../syntax/operator.md)

---

*If you have more Rust idioms you want mapped to Grapa, please open an issue or PR!*

## Customizing Grapa for Familiar Syntax

If you prefer Rust-style macro calls, you can define your own `println()` function in Grapa:

```grapa
// Define a println function similar to Rust
println = op("value"=""){value.echo();};
println("Hello from Grapa!");
```

## Debugging and Logging

Grapa provides sophisticated debugging capabilities that go beyond Rust's println! and log macros. While Grapa doesn't have `file!()` and `line!()` macros, it offers superior debugging through the `.debug()` method with component targeting and level control:

### Basic Debug Output
```grapa
/* Standard output - equivalent to println!() */
"Hello World".echo();

/* Debug output with component targeting */
"Debug message".debug(1, "component");

/* Debug with different levels */
"Info message".debug(1, "info");
"Warning message".debug(2, "warning");
"Error message".debug(3, "error");
```

### Enabling Debug Mode
```grapa
/* Enable debug mode for current session */
$sys().putenv("GRAPA_SESSION_DEBUG_MODE", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");

/* Enable debug for specific components */
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database,grep,vector");

/* Enable all components at different levels */
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "grep:3,database:1,*:0");
```

### Command Line Debug Options
```bash
# Enable debug mode from command line
./grapa -d script.grc

# Enable debug with specific components
GRAPA_SESSION_DEBUG_COMPONENTS="debug:1" ./grapa -d script.grc

# Enable debug with multiple components
GRAPA_SESSION_DEBUG_COMPONENTS="database:2,grep:1,vector:0" ./grapa -d script.grc
```

### Debug vs Rust Comparison

| Rust | Grapa |
|------|-------|
| `println!("Hello")` | `"Hello".echo()` |
| `println!("Count: {}", count)` | `"Count: ${count}".interpolate().echo()` |
| `eprintln!("Error: {}", error)` | `"Error: ${error}".debug(3, "error")` |
| `log::info!("Info message")` | `"Info message".debug(1, "info")` |
| `log::warn!("Warning")` | `"Warning".debug(2, "warning")` |
| `log::error!("Error")` | `"Error".debug(3, "error")` |
| `println!("Processing {}/{}", i, total)` | `"Processing ${i}/${total}".interpolate().debug(1, "progress")` |

### Advanced Debug Patterns
```grapa
/* Debug with interpolation for clean output */
"Processing ${record_count} records...".debug(1, "process");

/* Debug with error handling */
result = some_operation();
if (result.type() == $ERR) {
    "Error occurred: ${result}".debug(3, "error");
} else {
    "Operation successful: ${result}".debug(1, "info");
};

/* Debug with component-specific formatting */
"Database query: ${query}".debug(2, "database");
"Network request: ${url}".debug(2, "network");
"File operation: ${filename}".debug(2, "filesystem");
```