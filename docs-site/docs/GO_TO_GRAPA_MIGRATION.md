# Go-to-Grapa Migration Guide

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

This guide helps Go users transition to Grapa by mapping common Go idioms, patterns, and code to their Grapa equivalents.

> **See Also:**
> - [Basic Syntax Guide](syntax/basic_syntax.md)
> - [Operator Reference](syntax/operator.md)

## Syntax Mapping Table

| Go | Grapa |
|----|-------|
| `var x int = 5` | `x = 5;` |
| `x += 1` | `x += 1;` |
| `var s string = "hi"` | `s = "hi";` |
| `s += "!"` | `s += "!";` |
| `arr := []int{1,2,3}` | `arr = [1, 2, 3];` |
| `arr[0]` | `arr[0]`<br>`arr.get(0)` |
| `m := map[string]int{}` | `obj = {}` |
| `m["key"]` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| `for i := 0; i < len(arr); i++ { ... }` | `i = 0; while (i < arr.len()) { ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if cond { ... } else { ... }` | `if (cond) { ... } else { ... }` |
| `func f(x int) int { ... }` | `f = op(x) { ... };` |
| `/* comment */` (block only, own line) | `/* comment */` (block only, own line) |
| `nil` | `null` |
| `len(arr)` | `arr.len()` |
| `append(arr, x)` | `arr += x;` |
| `arr = arr[1:]` | `arr = arr.range(1, arr.len());` |
| `arr = arr[:n]` | `arr = arr.range(0, n);` |
| `arr = arr[1:n]` | `arr = arr.range(1, n);` |
| `arr = arr[:0]` | `arr = [];` |
| `arr = make([]int, n)` | `arr = (n).range(0,1);` |
| `range arr` | `arr.map(op(x) { ... })` |
| `range m` | `obj.map(op(k, v) { ... })` |
| `err != nil` | `result.type() == $ERR` |
| `defer` | `// not directly supported` |

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
- No `for`/`range` loops—use `while` or `.range()`+functional methods
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

**Go:**
```go
// Sum squares of even numbers
result := 0
for _, x := range arr {
    if x%2 == 0 {
        result += x * x
    }
}
```
**Grapa:**
```grapa
result = arr.filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(a, b) { a + b; }, 0);
```

**Go:**
```go
// Read file lines
lines, _ := ioutil.ReadFile("file.txt")
lineArr := strings.Split(string(lines), "\n")
```
**Grapa:**
```grapa
lines = $file().read("file.txt").split("\n");
```

**Go:**
```go
// Map access
value := m["key"]
```
**Grapa:**
```grapa
value = obj["key"];
value = obj.key;
value = obj."key";
```

**Go:**
```go
// File access (custom struct or map)
value := file["key"]
```
**Grapa:**
```grapa
value = file.get("key");
```

**Go:**
```go
// Generate numbers 0..9
seq := make([]int, 10)
for i := range seq {
    seq[i] = i
}
```
**Grapa:**
```grapa
seq = (10).range(0,1);
```

**Go:**
```go
// Sum numbers 0..9
sum := 0
for i := 0; i < 10; i++ {
    sum += i
}
```
**Grapa:**
```grapa
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
```

**Go:**
```go
// Collect even numbers 0..9
evens := []int{}
for i := 0; i < 10; i++ {
    if i%2 == 0 {
        evens = append(evens, i)
    }
}
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
```

**Go:**
```go
// Error fallback
result, err := someOperation()
if err != nil {
    result = 0
}
```
**Grapa:**
```grapa
result = someOperation().iferr(0);
```

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

## See Also
- [Basic Syntax Guide](syntax/basic_syntax.md)
- [Operator Reference](syntax/operator.md)

---

*If you have more Go idioms you want mapped to Grapa, please open an issue or PR!* 

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - Only `while` loops and `.range()`+functional methods are valid for iteration in Grapa. `for` loops are not supported.
> - If more objects support `.get()` in the future, this guide will be updated.

> **Comment Style:**
> - Only block comments (`/* ... */`) are supported in Grapa, and must always be on their own line.
> - `//` and `#` comments are not supported and will cause errors. 

## Work-in-Progress (WIP) Items

Some Go idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Static typing**: `var x int` - Grapa uses dynamic typing by design (see note below)
- **Pointers**: `*int` - Grapa has no pointers, all values are owned
- **Channels**: `chan int` - No channel-based concurrency
- **Goroutines**: `go func()` - No goroutine-based concurrency
- **Select statements**: `select` - No select-based concurrency
- **Defer**: `defer cleanup()` - No defer mechanism
- **Panic/recover**: `panic()`, `recover()` - No panic/recover mechanism
- **Embedding**: `type Child struct { Parent }` - No struct embedding
- **Package system**: `package main` - No package system
- **Import system**: `import "fmt"` - No import system
- **Build tags**: `// +build` - No build tag system
- **CGO**: - No CGO support
- **Assembly**: - No assembly support
- **Reflection**: `reflect` - Limited reflection capabilities

> **Important Note on Dynamic Typing:**
> Grapa's dynamic typing is a **fundamental design choice**, not a limitation. It enables Grapa's core strengths:
> - **Dynamic code execution** and meta-programming capabilities
> - **Runtime type introspection** with `.type()` method
> - **Flexible data processing** without compile-time type constraints
> - **System integration** that doesn't require type definitions
> 
> Grapa provides type safety through runtime checking and rich type introspection, which is often more flexible than static typing for data processing and system integration tasks.
>
> **Important Note on Class System:**
> Grapa's class system is **superior** to Go's structs, providing full object-oriented programming capabilities:
> - **Classes with methods**: Define data structures with attached functions
> - **Inheritance**: Create class hierarchies with `class (ParentClass)`
> - **Polymorphism**: Method overriding and dynamic dispatch
> - **Constructors**: Custom initialization functions
> - **Runtime modification**: Add methods and properties dynamically
> - **Type safety**: Runtime type checking and introspection
>
> **Important Note on Switch Statements:**
> Grapa **does have switch statements** that can be used for type switching:
> ```grapa
> /* Type switching with switch statement */
> switch (x.type()) {
>     case $STR: "string".echo();
>     case $INT: "int".echo();
>     case $ARRAY: "array".echo();
>     default: "unknown type".echo();
> };
> 
> /* Boolean switching (alternative to if/else chains) */
> switch(true) {
>     case (x > 10): "large".echo();
>     case (x > 5): "medium".echo();
>     default: "small".echo();
> };
> ```
>
> **Important Note on Class System:**
> Grapa's class system is **comprehensive** and provides all OOP features:
> - **Methods**: Classes can have methods attached to them
> - **Method sets**: Classes have method sets for interface-like behavior
> - **Inheritance**: Use `class (ParentClass)` for inheritance hierarchies
> - **Polymorphism**: Method overriding and dynamic dispatch
> - **Embedding**: Inheritance provides embedding-like functionality
> - **Composition**: Object composition is also supported
> 
> ```grapa
> /* Example: Class with methods and inheritance */
> Animal = class {
>     name = "";
>     speak = op() { "Some sound".echo(); };
> };
> 
> Dog = class (Animal) {
>     /* Method override */
>     speak = op() { "Woof!".echo(); };
>     /* New method */
>     fetch = op() { "Fetching...".echo(); };
> };
> 
> myDog = obj Dog;
> myDog.name = "Rex";
> myDog.speak();  /* Outputs: Woof! */
> myDog.fetch();  /* Outputs: Fetching... */
> ```

### Nice to Have
These would improve developer experience but aren't essential:

- **Interfaces**: `type MyInterface interface` - Use object composition and duck typing
- **Structs**: `type MyStruct struct` - Grapa has superior class system
  > **Note:** Grapa's class system provides full OOP capabilities including inheritance, polymorphism, methods, and runtime modification. See the "Important Note on Class System" section above for details and examples.
- **Methods**: `func (r Receiver) Method()` - Grapa classes have methods
  > **Note:** Grapa classes support methods with full OOP capabilities. See the "Important Note on Class System" section above for details and examples.
- **Type assertions**: `x.(string)` - Use `.type()` checks
- **Type switches**: `switch x.(type)` - Use `switch` with `.type()`
  > **Note:** Grapa supports type switching using `switch` statements with `.type()` method. See the "Important Note on Switch Statements" section above for examples.
- **Generics**: `func F[T any](x T)` - Grapa has dynamic typing
- **Type constraints**: `T comparable` - Use runtime checks
- **Type sets**: `~int` - Use regular types
- **Union types**: `int | string` - Use dynamic typing
- **Type inference**: - Use explicit types
- **Method sets**: - Grapa classes have method sets
  > **Note:** Grapa classes support method sets for interface-like behavior. See the "Important Note on Class System" section above for details.
- **Interface satisfaction**: - Use duck typing
- **Embedding**: - Grapa classes support inheritance
  > **Note:** Grapa classes support inheritance which provides embedding-like functionality. See the "Important Note on Class System" section above for details.
- **Composition**: - Use object composition
- **Inheritance**: - Grapa classes support inheritance
  > **Note:** Grapa classes support full inheritance hierarchies. See the "Important Note on Class System" section above for details and examples.

### Rarely Used
These are advanced features that most developers won't miss:

- **Unsafe package**: `unsafe` - Grapa is memory-safe by design
- **Runtime**: `runtime` - Use Grapa's system integration
- **CGO**: - Use Grapa's system integration
- **Assembly**: - Use Grapa's system calls
- **Build constraints**: - Use conditional compilation
- **Module system**: - Use Grapa's file system
- **Vendor directories**: - Use Grapa's dependency management
- **Go workspaces**: - Use directory organization
- **Go modules**: - Use Grapa's file system
- **Go toolchain**: - Use Grapa's build system

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, Go's goroutines are replaced by Grapa's built-in parallel processing with `.map()` and `.filter()`, and Go's channels are replaced by Grapa's threading and synchronization.

## See Also
- [Basic Syntax Guide](syntax/basic_syntax.md)
- [Operator Reference](syntax/operator.md)

---

*If you have more Go idioms you want mapped to Grapa, please open an issue or PR!*