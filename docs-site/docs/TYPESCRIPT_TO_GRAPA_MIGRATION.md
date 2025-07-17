# TypeScript-to-Grapa Migration Guide

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
> - For $LIST and $OBJ, use bracket or dot notation (e.g., obj["key"], obj.key).
> - For $ARRAY, use bracket notation (e.g., arr[1]).
> - Only $file and $TABLE support .get().
> - This is based on direct testing in Grapa v0.0.39.

---

This guide helps TypeScript users transition to Grapa by mapping common TypeScript idioms, patterns, and code to their Grapa equivalents.

> **See Also:**
> - [Basic Syntax Guide](syntax/basic_syntax.md)
> - [Operator Reference](syntax/operator.md)

## Syntax Mapping Table

| TypeScript | Grapa |
|------------|-------|
| `let x: number = 5;` | `x = 5;` |
| `x += 1;` | `x += 1;` |
| `let s: string = "hi";` | `s = "hi";` |
| `s += "!";` | `s += "!";` |
| `let arr: number[] = [1,2,3];` | `arr = [1, 2, 3];` |
| `arr[0]` | `arr[0]`<br>`arr.get(0)` |
| `let obj: {[key: string]: number} = {};` | `obj = {}` |
| `obj["key"]` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| `for (let i=0; i<arr.length; i++)` | `i = 0; while (i < arr.len()) { ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if (cond) { ... } else { ... }` | `if (cond) { ... } else { ... }` |
| `function f(x: number): number { ... }` | `f = op(x) { ... };` |
| `/* comment */` (block only, own line) | `/* comment */` (block only, own line) |
| `null/undefined` | `null` |
| `arr.map(f)` | `arr.map(op(x) { f(x); })` |
| `arr.filter(f)` | `arr.filter(op(x) { f(x); })` |
| `arr.reduce(f, init)` | `arr.reduce(op(a, b) { f(a, b); }, init)` |
| `arr.length` | `arr.len()` |
| `x = x + 1` | `x = x + 1;`<br>`x += 1;` (preferred) |
| `s = s + "x"` | `s = s + "x";`<br>`s += "x";` (preferred) |

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
- No `.push()`/`.pop()`—use `+=` and manual index for pop
- No `/* comment */` comments—only block comments (`/* ... */`), always on their own line
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads
- Use `.iferr()` for simple error fallback; use `if (result.type() == $ERR)` only for explicit error handling

## Example Code Pairs

**TypeScript:**
```ts
// Sum squares of even numbers
let result = arr.filter(x => x % 2 === 0).map(x => x * x).reduce((a, b) => a + b, 0);
```
**Grapa:**
```grapa
result = arr.filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(a, b) { a + b; }, 0);
```

**TypeScript:**
```ts
// Read file lines
let lines = fs.readFileSync('file.txt', 'utf-8').split('\n');
```
**Grapa:**
```grapa
lines = $file().read("file.txt").split("\n");
```

**TypeScript:**
```ts
// Object access
let value = obj["key"];
```
**Grapa:**
```grapa
value = obj["key"];
value = obj.key;
value = obj."key";
```

**TypeScript:**
```ts
// File access
let value = file["key"];
```
**Grapa:**
```grapa
value = file.get("key");
```

**TypeScript:**
```ts
// Generate numbers 0..9
let seq = Array.from({length: 10}, (_, i) => i);
```
**Grapa:**
```grapa
seq = (10).range(0,1);
```

**TypeScript:**
```ts
// Sum numbers 0..9
let sum = Array.from({length: 10}, (_, i) => i).reduce((a, x) => a + x, 0);
```
**Grapa:**
```grapa
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
```

**TypeScript:**
```ts
// Collect even numbers 0..9
let evens = Array.from({length: 10}, (_, i) => i).filter(x => x % 2 === 0);
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
```

**TypeScript:**
```ts
// Error fallback
let result = someOperation() ?? 0;
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

*If you have more TypeScript idioms you want mapped to Grapa, please open an issue or PR!* 

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - If more objects support `.get()` in the future, this guide will be updated. 

> **Comment Style:**
> - Only block comments (`/* ... */`) are supported in Grapa, and must always be on their own line.
> - `//` and `#` comments are not supported and will cause errors. 

## Work-in-Progress (WIP) Items

Some TypeScript idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Static typing**: `let x: number` - Grapa uses dynamic typing by design (see note below)
- **Type aliases**: `type MyType = string` - No type alias system
- **Union types**: `string | number` - No union type system
- **Intersection types**: `A & B` - No intersection type system
- **Literal types**: `"hello"` - No literal type system
- **Template literal types**: `` `Hello ${string}` `` - No template literal types
- **Conditional types**: `T extends U ? X : Y` - No conditional type system
- **Mapped types**: `{ [K in keyof T]: T[K] }` - No mapped type system
- **Utility types**: `Partial<T>`, `Required<T>` - No utility type system
- **Namespace**: `namespace MyNamespace` - Grapa has dynamic namespaces (see note below)
- **Declaration merging**: - No declaration merging
- **Const assertions**: `as const` - Grapa has superior runtime const protection (see note below)

> **Important Note on Dynamic Typing:**
> Grapa's dynamic typing is a **fundamental design choice**, not a limitation. It enables Grapa's core strengths:
> - **Dynamic code execution** and meta-programming capabilities
> - **Runtime type introspection** with `.type()` method
> - **Flexible data processing** without compile-time type constraints
> - **System integration** that doesn't require type definitions
> 
> Grapa provides type safety through runtime checking and rich type introspection, which is often more flexible than static typing for data processing and system integration tasks.

> **Important Note on Namespaces:**
> Grapa has a **dynamic namespace system** that's more flexible than TypeScript's static namespaces:
> - **$global namespace**: Global variables accessible everywhere
> - **$local namespace**: Function-local variables (automatic)
> - **Dynamic namespace creation**: Namespaces are created automatically for each execution context
> - **No explicit namespace declarations**: Namespaces are managed automatically by the execution tree
> - **Hierarchical access**: Variables can be accessed from parent namespaces

> **Important Note on Const Protection:**
> Grapa's `const` operator provides **superior runtime protection** compared to TypeScript's `as const`:
> - **Runtime modification prevention**: Grapa's const prevents actual modification at runtime
> - **Dynamic control**: Can toggle const on/off with `.setconst(false/true)`
> - **Performance optimization**: Designed for caching and optimization
> - **Thread safety**: Prevents blocking on access for concurrent threads
> - **No static type narrowing**: Grapa uses dynamic typing, so no compile-time literal type inference

### Nice to Have
These would improve developer experience but aren't essential:

- **Interfaces**: `interface MyInterface` - Use object composition and duck typing
- **Generics**: `<T>` - Grapa has dynamic typing
- **Type guards**: `x is string` - Use `.type()` checks
- **Type assertions**: `x as string` - Use explicit conversion
- **Modules**: `import/export` - Use Grapa's file system
- **Decorators**: `@decorator` - Use function composition
- **Enums**: `enum MyEnum` - Use objects with constants
- **Class syntax**: `class MyClass` - Use object constructors
- **Access modifiers**: `private`, `public` - Use naming conventions
- **Abstract classes**: `abstract class` - Use regular classes
- **Method overloading**: - Use different function names
- **Index signatures**: `[key: string]: any` - Use regular objects
- **Call signatures**: - Use regular functions
- **Construct signatures**: - Use object constructors
- **Generic constraints**: `T extends U` - Use runtime checks
- **Default type parameters**: `<T = string>` - Use default values
- **Infer types**: `infer U` - Use dynamic typing

### Rarely Used
These are advanced features that most developers won't miss:

- **Conditional types**: - Use runtime checks
- **Template literal types**: - Use regular strings
- **Mapped types**: - Use object manipulation
- **Utility types**: - Use object manipulation
- **Declaration merging**: - Use object composition
- **Ambient declarations**: `declare` - Use regular definitions
- **Triple-slash directives**: - Use regular imports
- **Module augmentation**: - Use object composition
- **Type-only imports**: `import type` - Use regular imports
- **Satisfies operator**: - Use regular assignment

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, TypeScript's static typing is replaced by Grapa's dynamic typing with runtime type checking via `.type()`.

## See Also
- [Basic Syntax Guide](syntax/basic_syntax.md)
- [Operator Reference](syntax/operator.md)

---

*If you have more TypeScript idioms you want mapped to Grapa, please open an issue or PR!*