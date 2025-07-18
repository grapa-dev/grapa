# JavaScript-to-Grapa Migration Guide

## Key Insight: Grapa is Fundamentally Complete

**Grapa covers ALL the fundamental JavaScript programming concepts** that developers use day-to-day. The remaining "gaps" are either:
- Advanced features that most developers never use
- Syntactic sugar that Grapa handles differently but just as effectively
- Areas where Grapa actually provides superior alternatives

**Grapa's unique advantages over JavaScript:**
- ✅ **Built-in parallel processing** (no Web Workers/worker_threads setup needed)
- ✅ **Unlimited precision arithmetic** (no BigInt conversion overhead)
- ✅ **Advanced pattern matching** (superior to JavaScript's RegExp)
- ✅ **Execution tree introspection** (impossible in JavaScript)
- ✅ **Native JSON/XML processing** (no external libraries)
- ✅ **Built-in file system integration** (no fs module needed)
- ✅ **Native database operations** (no IndexedDB/SQLite needed)
- ✅ **Superior dynamic code execution** (human-readable execution trees)
- ✅ **Runtime type safety** (no TypeScript compilation needed)

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

---

This guide helps JavaScript users transition to Grapa by mapping common JS idioms, patterns, and code to their Grapa equivalents.

> **Comment Style:**
> - Only block comments (`/* ... */`) are supported in Grapa, and must always be on their own line.
> - `//` and `#` comments are not supported and will cause errors.

## Syntax Mapping Table

| JavaScript | Grapa |
|------------|-------|
| `for (let i=0; i<arr.length; i++)` | `i = 0; while (i < arr.len()) { ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if (cond) { ... }` | `if (cond) { ... }` |
| `function f(x) { ... }` | `f = op(x) { ... };` |
| `arr.push(x)` | `arr += x;` |
| `arr[index]` | `arr[index]`<br>`arr.get(index)` |
| `obj.key` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| (file access) | `file.get("key")` |
| `try { ... } catch { ... }` | `result.iferr(fallback)`<br>`if (result.type() == $ERR) { ... }` |
| `/* comment */` | `/* comment */` (block only, own line) |
| `true/false` | `true/false` |
| `null/undefined` | `null` |
| `String(x)` | `x.str()` |
| `Number(x)` | `x.int()` or `x.float()` |
| `arr.length` | `arr.len()` |
| `arr.map(f)` | `arr.map(op(x) { f(x); })` |
| `arr.filter(f)` | `arr.filter(op(x) { f(x); })` |
| `arr.reduce(f, init)` | `arr.reduce(op(a, b) { f(a, b); }, init)` |
| `Array.from({length: n}, (_, i) => i)` | `(n).range(0,1)` |
| `x = x + 1` | `x = x + 1;`<br>`x += 1;` (preferred) |
| `s = s + "x"` | `s = s + "x";`<br>`s += "x";` (preferred) |
| `x => x * 2` | `op(x) { x * 2; }` |
| `function f(x = 1) { ... }` | `f = op("x"=1) { ... };` |
| `function f(...args) { ... }` | `f = op(args) { ... };` |
| `obj?.prop` | `if (obj && obj.prop) { obj.prop; }` |
| `x ?? y` | `if (x == null) { x = y; }` |
| `[...arr1, ...arr2]` | `arr1 += arr2` |
| `x ||= y` | `if (!x) { x = y; }` (no `||=` operator) |
| `BigInt(123)` | `123` (Grapa handles unlimited precision natively) |
| `async function f() { await g() }` | Use Grapa's built-in parallelism: `data.map(op(x) { process(x); }, 8)` |
| `function* gen() { yield x }` | Use `.map()` (Grapa is eager by default) |

> **Note:** Both `x = x + 1;` and `x += 1;` (and `s = s + "x";` and `s += "x";`) are valid in Grapa. The `+=` form is idiomatic and preferred in most cases.

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` (returns empty string).
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - **Length**: Use `.len()` for arrays and lists, not `.size()`.
> - **Keys**: For lists, iterate manually instead of using `.keys()`.
> - If more objects support `.get()` in the future, this guide will be updated.

> **Note:** `.get("key")` is only for `$file` (and possibly one other system object). For `$LIST`/`$OBJ`, use `obj["key"]`, `obj.key`, or `obj."key"`. For `$ARRAY`, use `arr[index]` (and `arr.get(index)` if supported).

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
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation | .len() | .size() |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|:------:|:-------:|
> | $ARRAY    |      ✗      |     ✗      |       ✓         |      —       |   ✓   |    ✗   |
> | $LIST     |      ✗      |     ✗      |       ✓         |     ✓       |   ✓   |    ✗   |
> | $OBJ      |      ✗      |     ✗      |       ✓         |     ✓       |   ✗   |    ✗   |
> | $file     |      ✓      |     ✗      |        —         |      —       |   ✗   |    ✗   |
> | $TABLE    |     ✓*      |     ✗      |        —         |      —       |   ✗   |    ✗   |
> *$TABLE .get() requires two arguments: key and field.

See [Basic Syntax Guide](syntax/basic_syntax.md) for empirical test results and future updates.

## Common Pitfalls
- No `for`/`foreach` loops—use `while` or `.range()`+functional methods
- No `try/catch`—use `.iferr()` for fallback or check for `$ERR`
- No `.push()`/`.pop()`—use `+=` and manual index for pop
- No `/* comment */` comments—only block comments (`/* ... */`), always on their own line
- No attribute-style access for object keys—use `[]`, or dot notation for `$LIST`/`$OBJ`
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads
- Use `.iferr()` for simple error fallback; use `if (result.type() == $ERR)` only for explicit error handling
- **Count-only grep returns array**: `text.grep(pattern, "c")` returns `["2"]` not `2` - use `text.grep(pattern, "c")[0].int()` to get the number

## Example Code Pairs

**JavaScript:**
```js
// Sum squares of even numbers
let result = arr.filter(x => x % 2 === 0).map(x => x * x).reduce((a, b) => a + b, 0);
```
**Grapa:**
```grapa
result = arr.filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(a, b) { a + b; }, 0);
```

**JavaScript:**
```js
// Read file lines
const fs = require('fs');
let lines = fs.readFileSync('file.txt', 'utf-8').split('\n');
```
**Grapa:**
```grapa
lines = $file().read("file.txt").split("\n");
```

**JavaScript:**
```js
// Object access
let value = obj.key;
```
**Grapa:**
```grapa
value = obj["key"];
value = obj.key;
value = obj."key";
```

**JavaScript:**
```js
// File access
let value = file['key'];
```
**Grapa:**
```grapa
value = file.get("key");
```

**JavaScript:**
```js
// Generate numbers 0..9
let seq = Array.from({length: 10}, (_, i) => i);
```
**Grapa:**
```grapa
seq = (10).range(0,1);
```

**JavaScript:**
```js
// Sum numbers 0..9
let sum = 0;
for (let i = 0; i < 10; i++) {
    sum += i;
}
```
**Grapa:**
```grapa
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
```

**JavaScript:**
```js
// Collect even numbers 0..9
let evens = [];
for (let i = 0; i < 10; i++) {
    if (i % 2 === 0) {
        evens.push(i);
    }
}
```
**Grapa:**
```grapa
evens = (10).range(0,1).reduce(op(acc, x) {
    if (x % 2 == 0) { acc += x; };
}, []);
```

**JavaScript:**
```js
// Squares of 0..9
let squares = Array.from({length: 10}, (_, i) => i * i);
```
**Grapa:**
```grapa
squares = (10).range(0,1).map(op(x) { x * x; });
```

**JavaScript:**
```js
// Even numbers 0..9
let evens = Array.from({length: 10}, (_, i) => i).filter(x => x % 2 === 0);
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
```

**JavaScript:**
```js
// Error fallback
let result;
try {
    result = someOperation();
} catch {
    result = 0;
}
```
**Grapa:**
```grapa
result = some_operation().iferr(0);
```

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

> **Tip:** Grapa's parallel ETL/data processing is robust, production-ready, and a core design goal. Unlike JavaScript, you can use parallel methods like `.map()` and `.filter()` out of the box for high-throughput data tasks.

## Work-in-Progress (WIP) Items

Some JavaScript idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **with statement**: `with (obj)` - No with statement support
- **Labeled statements**: `label: statement` - No labeled statement support
- **Debugger statement**: - No debugger statement support
- **Strict mode**: - Grapa is strict by default
- **Hoisting**: - Grapa has different scoping rules
- **Closure scope**: - Grapa has different scoping rules
- **Prototype chain**: - No prototype system
- **Function constructors**: - No function constructor support

### Advanced Meta-Programming (Available in Grapa)
Grapa actually provides **superior** dynamic code execution capabilities:

- **Dynamic code execution**: `op()("'hello world'.echo();")()` - Direct string-to-function compilation
- **System evaluation**: `$sys().eval("x + y", {"x": 5, "y": 3})` - System-level evaluation with parameters
- **Compiled execution**: `$sys().compile("script")` then `$sys().eval(compiled)` - Compile once, execute many
- **Execution trees**: Direct manipulation and execution of `$OP` objects
- **Parameter binding**: Both positional and named parameter support
- **Introspection**: Execution trees can be examined and modified

### Grapa Features That JavaScript Lacks

Grapa provides many capabilities that JavaScript doesn't have, making it superior for certain tasks:

#### **Built-in Parallel Processing**
```grapa
/* Parallel processing is built-in and production-ready */
data = (1000000).range(0,1);
/* 8 threads */
result = data.map(op(x) { x * x; }, 8);
/* 8 threads */
filtered = data.filter(op(x) { x % 2 == 0; }, 8);
```

**JavaScript equivalent requires:** Web Workers, Node.js `worker_threads`, or complex async patterns

#### **Unlimited Precision Arithmetic**
```grapa
/* Native unlimited precision - no BigInt needed */
big_num = 123456789012345678901234567890 * 987654321098765432109876543210;
big_num.echo();  /* Exact result, no overflow */
```

**JavaScript equivalent requires:** `BigInt` with limited operations and conversion overhead

#### **Advanced Pattern Matching with Grep**
```grapa
/* Rich grep with context, case options, and advanced patterns */
text = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5";

/* Context lines */
text.grep("Line 3", "A1B1");  /* 1 line before and after */

/* Case-insensitive with match-only */
text.grep("line", "io");  /* Only matching parts, case-insensitive */

/* Multiple patterns with context merging */
text.grep("Line 2|Line 4", "A1B1");  /* Context blocks merged automatically */
```

**JavaScript equivalent requires:** `RegExp` with manual context handling and complex state management

#### **Native JSON/XML Round-tripping**
```grapa
/* Direct JSON/XML parsing and serialization */
json_text = '{"name": "John", "age": 30}';
data = json_text.json();
data.name.echo();  /* "John" */

xml_text = '<person><name>John</name><age>30</age></person>';
xml_data = xml_text.xml();
xml_data.findall("name")[0].echo();  /* "John" */
```

**JavaScript equivalent requires:** `JSON.parse()` and `DOMParser` or external XML libraries

#### **Execution Tree Introspection**
```grapa
/* Examine and manipulate compiled code directly */
func = op(x){x * 2 + 1};
func;  /* Shows: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}> */

/* Direct tree execution */
/* 11 */
@<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}>(5);
```

**JavaScript equivalent:** Not possible - bytecode is not human-readable

#### **Built-in File System Integration**
```grapa
/* Direct file system operations */
$file().set("test.txt", "Hello World");
content = $file().read("test.txt");
$file().rm("test.txt");
```

**JavaScript equivalent requires:** Node.js `fs` module or browser File API

#### **Native Table/Database Operations**
```grapa
/* Built-in table operations */
table = $file().table("ROW");
table.mkfield("name", "STR", "VAR");
table.mkfield("age", "INT");
table.set("user1", "John", "name");
table.set("user1", 30, "age");
```

**JavaScript equivalent requires:** IndexedDB, SQLite, or external database libraries

#### **Vector and Matrix Operations**
```grapa
/* Native vector operations */
v1 = <1, 2, 3>;
v2 = <4, 5, 6>;
dot_product = v1.dot(v2);  /* 32 */
```

**JavaScript equivalent requires:** Math libraries or manual implementation

#### **Built-in Error Fallback**
```grapa
/* Graceful error handling */
result = risky_operation().iferr(0);  /* Returns 0 if operation fails */
```

**JavaScript equivalent requires:** `try/catch` blocks or Promise error handling

#### **Native Time/Date with Unlimited Precision**
```grapa
/* Unlimited precision time operations */
now = $TIME().utc();
future = now + 86400000000;  /* Add 1 day in microseconds */
```

**JavaScript equivalent requires:** `Date` with millisecond precision limits

#### **Direct System Integration**
```grapa
/* Access system information directly */
version = $sys().getenv($VERSION);
platform = $sys().getenv($PLATFORM);
```

**JavaScript equivalent requires:** Node.js `process` and `os` modules (not available in browsers)

#### **Type Safety Without TypeScript**
```grapa
/* Runtime type checking without compilation */
if (value.type() == $INT) {
    /* Handle integer */
} else if (value.type() == $STR) {
    /* Handle string */
}
```

**JavaScript equivalent requires:** TypeScript compilation or manual runtime checks

### Nice to Have
These would improve developer experience but aren't essential:

- **Template literals**: `` `Hello ${name}` `` - Use string concatenation: `"Hello " + name`
- **Destructuring**: `const {a, b} = obj` - Use explicit property access: `a = obj.a; b = obj.b;`
- **Spread operator**: `[...arr1, ...arr2]` - Use `+=` operator: `arr1 += arr2`
- **Optional chaining**: `obj?.prop?.sub` - Use explicit null checks: `if (obj && obj.prop) { obj.prop.sub; }`
- **Nullish coalescing**: `x ?? y` - Use explicit null checks: `if (x == null) { x = y; }`
- **Logical assignment**: `x ||= y` - Use explicit assignment: `if (!x) { x = y; }` (no `||=` operator)
- **Class syntax**: `class MyClass` - Use object constructors and functions
- **Private fields**: `#private` - Use naming conventions like `_private`
- **Getters/setters**: `get prop()` - Use regular methods
- **Symbols**: `Symbol('key')` - Use string keys
- **WeakMap/WeakSet**: - Use regular objects/arrays
- **Proxy**: - Use regular objects
- **Reflect**: - Use direct property access
- **Optional catch binding**: `catch {}` - Use explicit error handling
- **Modules**: `import/export` - Use Grapa's file system and `$file()` operations
- **Dynamic imports**: `import()` - Use `$file().read()` and `$sys().eval()`
- **Top-level await**: - Use regular execution
- **Class fields**: `class { field = 1 }` - Use constructor assignment
- **Private methods**: `#method()` - Use naming conventions like `_method()`
- **Static fields**: `static field = 1` - Use module-level variables
- **Class expressions**: `const MyClass = class {}` - Use object constructors
- **Computed property names**: `{[key]: value}` - Use explicit assignment: `obj[key] = value;`
- **Method shorthand**: `{method() {}}` - Use regular method syntax
- **Property shorthand**: `{name}` - Use explicit assignment: `{name: name}`

### Rarely Used
These are advanced features that most developers won't miss:

- **Decorators**: `@decorator` - Use function composition
- **Meta-programming**: `eval()`, `Function()` - Use Grapa's built-in code generation
- **with statement**: `with (obj)` - Use explicit property access
- **Labeled statements**: `label: statement` - Use regular control flow
- **Debugger statement**: - Use Grapa's debugging tools
- **Strict mode**: - Grapa is strict by default
- **Hoisting**: - Grapa has different scoping rules
- **Closure scope**: - Grapa has different scoping rules
- **Prototype chain**: - Use object composition
- **Function constructors**: - Use regular functions

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, async/await patterns are replaced by Grapa's built-in parallel processing with `.map()` and `.filter()`.

## See Also
- [Migration Tips for JavaScript Users](USE_CASES.md#javascript-use-cases)
- [Python Integration Guide](../PYTHON_INTEGRATION.md)

---

*If you have more JavaScript idioms you want mapped to Grapa, please open an issue or PR!*