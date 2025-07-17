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
> - [Basic Syntax Guide](syntax/basic_syntax.md)
> - [Operator Reference](syntax/operator.md)

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
| `for x in arr { ... }` | `i = 0; while (i < arr.len()) { x = arr[i]; ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if cond { ... } else { ... }` | `if (cond) { ... } else { ... }` |
| `fn f(x: i32) -> i32 { ... }` | `f = op(x) { ... };` |
| `/* comment */` (block only, own line) | `/* comment */` (block only, own line) |
| `Some(x)` / `None` | `x` / `null` |
| `Result<T, E>` | `value or $ERR` |
| `match x { ... }` | `if/else` chain |
| `arr.iter().map(|x| x+1)` | `arr.map(op(x) { x + 1; })` |
| `arr.iter().filter(|x| *x > 0)` | `arr.filter(op(x) { x > 0; })` |
| `arr.iter().fold(0, |a, x| a + x)` | `arr.reduce(op(a, x) { a + x; }, 0)` |
| `arr.len()` | `arr.len()` |
| `format!("{} {}", a, b)` | `("" + a.str() + " " + b.str())` |

> **Note:** Both `x = x + 1;` and `x += 1;` (and `s = s + "x";` and `s += "x";`) are valid in Grapa. The `+=` form is idiomatic and preferred in most cases.

> **Note:** `.get("key")` is only for `$file` and `$TABLE`. For `$LIST`/`$OBJ`, use `obj["key"]`, `obj.key`, or `obj."key"`. For `$ARRAY`, use `arr[index]` or `arr.get(index)`.

## Access Patterns: Objects, Lists, Arrays, Files, and Tables

Below are all valid ways to access elements in Grapa data structures. See the canonical [Basic Syntax Guide](syntax/basic_syntax.md) for the latest tested rules.

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

See [Basic Syntax Guide](syntax/basic_syntax.md) for empirical test results and future updates.

## Common Pitfalls
- No `for`/`foreach` loops—use `while` or `.range()`+functional methods
- No `match`—use `if/else` chains
- No `.push()`/`.append()`—use `+=` for append
- No `/* comment */` comments—only block comments (`/* ... */`), always on their own line
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
- [Basic Syntax Guide](syntax/basic_syntax.md)
- [Operator Reference](syntax/operator.md)

---

*If you have more Rust idioms you want mapped to Grapa, please open an issue or PR!* 

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - If more objects support `.get()` in the future, this guide will be updated. 

> **Comment Style:**
> - Only block comments (`/* ... */`) are supported in Grapa, and must always be on their own line.
> - `//` and `#` comments are not supported and will cause errors. 

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
> - **System integration** that doesn't require type definitions
> 
> Grapa provides type safety through runtime checking and rich type introspection, which is often more flexible than static typing for data processing and system integration tasks.

### Nice to Have
These would improve developer experience but aren't essential:

- **Traits**: `trait MyTrait` - Use object composition and duck typing
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

## See Also
- [Basic Syntax Guide](syntax/basic_syntax.md)
- [Operator Reference](syntax/operator.md)

---

*If you have more Rust idioms you want mapped to Grapa, please open an issue or PR!*