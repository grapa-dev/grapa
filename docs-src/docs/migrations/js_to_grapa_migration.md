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
> *$TABLE .getfield() requires two arguments: key and field.
>
> **Key Findings:**
> - **Arrays (`[]`)**: Use `array[index]` and `array.len()` for access and length
> - **Lists (`{}`)**: Use `list[key]` or `list.key` for access, `list.len()` for length
> - **Objects (class)**: Use `object.property` or `object[key]` for access
> - **`.getfield()/.setfield()` method**: Use for `$file` and `$TABLE` types
> - **`.get()/.set()` method**: Exclusively for `$WIDGET` types
> - **`.size()` method**: Not supported on any type (use `.len()` instead)
> - **`.keys()` method**: Not supported on `$LIST` (use iteration instead)

---

This guide helps JavaScript users transition to Grapa by mapping common JS idioms, patterns, and code to their Grapa equivalents.

> **Comment Style:**
> - ✅ **Comprehensive comment support**: `/* */` (block), `/** */` (doc block), `//` (line), `///` (doc line)
> - Comments work everywhere including at end of lines and inside code blocks

## Syntax Mapping Table

| JavaScript | Grapa |
|------------|-------|
| `for (let i=0; i<arr.length; i++)` | `for i in arr { ... }`<br>`i = 0; while (i < arr.len()) { ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if (cond) { ... }` | `if (cond) { ... }` |
| `function f(x) { ... }` | `f = op(x) { ... };` |
| `arr.push(x)` | `arr += x;` |
| `arr[index]` | `arr[index]` |
| `obj.key` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| (file access) | `file.get("key")` |
| `try { ... } catch { ... }` | `result.iferr(fallback)`<br>`if (result.type() == $ERR) { ... }` |
| `/* comment */` | `/* comment */` (block)<br>`/** comment */` (doc block)<br>`// comment` (line)<br>`/// comment` (doc line) |
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
| `const {a, b} = obj` | `@local ++= obj; a.echo(); b.echo();` |
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

> **Note:** `.getfield("key")` is for `$file` and `$TABLE`. `.get()/.set()` is for `$WIDGET`. For `$LIST`/`$OBJ`, use `obj["key"]`, `obj.key`, or `obj."key"`. For `$ARRAY`, use `arr[index]` (bracket notation only).

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
/* Note: .get(index) is not supported for arrays - use bracket notation */
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
table = {}.table("ROW");
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

See [Basic Syntax Guide](../syntax/basic_syntax.md) for empirical test results and future updates.

## Common Pitfalls
- ✅ **For loops now supported** - Use `for x in arr { ... }` for native iteration
- No `try/catch`—use `.iferr()` for fallback or check for `$ERR`
- No `.push()`/`.pop()`—use `+=` and manual index for pop
- ✅ **Line comments now supported** - Use `// comment` or `/// doc comment` in addition to block comments
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

## Customizing Grapa for Familiar Syntax

If you prefer JavaScript-style function calls, you can define your own `console_log()` function in Grapa:

```grapa
// Define a console_log function similar to JavaScript
console_log = op("value"=""){value.echo();};
console_log("Hello from Grapa!");
```

This can make migration easier for those used to JavaScript's `console.log()` or similar functions.

## Debugging and Logging

Grapa provides sophisticated debugging capabilities that go beyond JavaScript's console methods. While Grapa doesn't have `__FILE__` and `__LINE__` macros, it offers superior debugging through the `.debug()` method with component targeting and level control:

### Basic Debug Output
```grapa
/* Standard output - equivalent to console.log() */
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

### Debug vs Console Comparison

| JavaScript | Grapa |
|------------|-------|
| `console.log("Hello")` | `"Hello".echo()` |
| `console.debug("Debug info")` | `"Debug info".debug(1, "debug")` |
| `console.info("Info message")` | `"Info message".debug(1, "info")` |
| `console.warn("Warning")` | `"Warning".debug(2, "warning")` |
| `console.error("Error")` | `"Error".debug(3, "error")` |
| `console.log(\`Count: \${count}\`)` | `"Count: ${count}".interpolate().echo()` |
| `console.log(\`Processing \${i}/\${total}\`)` | `"Processing ${i}/${total}".interpolate().debug(1, "progress")` |

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

> **Advanced:** Grapa also allows advanced users to customize or extend the language syntax using `$RULE` or by modifying `$global` rules. This enables you to inject your own grammar or override built-in behaviors to match your preferred style. For most users, we recommend learning the canonical Grapa method syntax, but this flexibility is available if needed.

## Work-in-Progress (WIP) Items

Some JavaScript idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **with statement**: `with (obj)` - No with statement support
- **Labeled statements**: `label: statement` - No labeled statement support
- **Debugger statement**: - Grapa provides `.debug()` method for sophisticated debugging and logging (outputs to stderr, can be redirected at CLI level)
- **Strict mode**: - Grapa is strict by default
- **Hoisting**: - Grapa has different scoping rules
- **Closure scope**: - Grapa has different scoping rules
- **Prototype chain**: - Use `+=` for single item, `++=` for multiple items ($LIST)
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

**JavaScript equivalent:** Not possible - JavaScript bytecode is not human-readable

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
table = {}.table("ROW");
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

- **Template literals**: `` `Hello ${name}` `` - Use `"Hello ${name}".interpolate()` (preferred) or string concatenation: `"Hello " + name` or see [String Templates and Dynamic Construction](../type/str.md#string-templates-and-dynamic-construction) for advanced patterns
- **Method chaining**: `"hello".toUpperCase() + " world".toLowerCase()` - Use string interpolation: `"${'hello'.upper()} ${'world'.lower()}".interpolate()`
- **Destructuring**: `const {a, b} = obj` - Use Grapa's namespace destructuring: `@local ++= obj; a.echo(); b.echo();`
- **Spread operator**: `[...arr1, ...arr2]` - Use `+=` operator: `arr1 += arr2`
- **Optional chaining**: `obj?.prop?.sub` - Use `.iferr()` for superior safe property access with custom fallback values: `obj.prop.iferr(null).sub.iferr("default")`
- **Nullish coalescing**: `x ?? y` - Use `.ifnull()` for superior nullish coalescing: `x.ifnull(y)`
- **Logical assignment**: `x ||= y` - Use explicit assignment: `if (!x) { x = y; }` (no `||=` operator)
- **Class syntax**: `class MyClass` - Use `class MyClass {}` with `$new()` constructor or manual initialization
- **Private fields**: `#private` - Use naming conventions like `_private`
- **Getters/setters**: `get prop()` - Use regular methods
- **Symbols**: `Symbol('key')` - Use string keys
- **WeakMap/WeakSet**: - Use regular objects/arrays
- **Proxy**: - Use regular objects
- **Reflect**: - Use direct property access
- **Optional catch binding**: `catch {}` - Use explicit error handling
- **Modules**: `import/export` - Use Grapa's include system and dynamic class loading
- **Dynamic imports**: `import()` - Use `$file().read()` and `$sys().eval()`
- **Top-level await**: - Use regular execution
- **Class fields**: `class { field = 1 }` - Use `$new()` constructor or direct property assignment
- **Private methods**: `#method()` - Use naming conventions like `_method()`
- **Static fields**: `static field = 1` - Use module-level variables
- **Class expressions**: `const MyClass = class {}` - Use `class MyClass {}` with `$new()` constructor
- **Computed property names**: `{[key]: value}` - Use explicit assignment: `obj[key] = value;`
- **Method shorthand**: `{method() {}}` - Use regular method syntax
- **Property shorthand**: `{name}` - Use explicit assignment: `{name: name}`

### Rarely Used
These are advanced features that most developers won't miss:

- **Decorators**: `@decorator` - Use function composition
- **Meta-programming**: `eval()`, `Function()` - Use Grapa's built-in code generation
- **with statement**: `with (obj)` - Use explicit property access
- **Labeled statements**: `label: statement` - Use regular control flow
- **Debugger statement**: - Use Grapa's `.debug()` method: `"Debug info".debug(0)` (requires `-d` flag)
- **Strict mode**: - Grapa is strict by default
- **Hoisting**: - Grapa has different scoping rules
- **Closure scope**: - Grapa has different scoping rules
- **Prototype chain**: - Use `+=` for single item, `++=` for multiple items ($LIST)
- **Function constructors**: - Use regular functions

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, async/await patterns are replaced by Grapa's built-in parallel processing with `.map()` and `.filter()`.

## Notes
- See [Operators](../syntax/operator.md) and [System](../sys/sys.md) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions.

## See Also
- [Python Integration Guide](../python_integration.md)

---

*If you have more JavaScript idioms you want mapped to Grapa, please open an issue or PR!*

### Custom match() Function for Regex

JavaScript users often use `str.match` for regex checks. You can define a similar function in Grapa:

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

### Class Constructors

**JavaScript:**
```javascript
class MyClass {
    constructor(value = -1) {
        this.n = value;
    }
    
    getValue() {
        return this.n;
    }
}

const x = new MyClass();      // n = -1
const y = new MyClass(4);     // n = 4
```

**Grapa:**
```grapa
/* Using $new() constructor (automatic) */
MyClass = class {
    n = 0;
    
    /* Constructor - automatically called when creating instances */
    $new = op(value:-1) {
        n = value;
    };
    
    getValue = op() {
        n;
    };
};

/* Create instances - constructor called automatically */
x = MyClass();      /* n = -1 */
y = MyClass(4);     /* n = 4 */

x.getValue().echo();  /* Output: -1 */
y.getValue().echo();  /* Output: 4 */
```

**Alternative Manual Pattern:**
```grapa
/* Manual initialization (for classes without $new()) */
MyClass = class {
    n = 0;
    
    /* Manual init method - must be called explicitly */
    init = op(value:-1) {
        n = value;
    };
    
    getValue = op() {
        n;
    };
};

/* Create and manually initialize */
x = obj MyClass;
x.init();           /* n = -1 */
y = obj MyClass;
y.init(4);          /* n = 4 */
```

### Access Control and Visibility

**JavaScript:**
```javascript
class Example {
    constructor() {
        this.publicVar = "public";
        this._protectedVar = "protected";
        this.#privateVar = "private";
    }
    
    publicMethod() {
        return this.publicVar;
    }
    
    _protectedMethod() {
        return this._protectedVar;
    }
    
    #privateMethod() {
        return this.#privateVar;
    }
}
```

**Grapa:**
```grapa
/* Grapa doesn't need access modifiers - all properties are accessible */
/* This aligns with Grapa's late-binding, dynamic philosophy */
example = {
    publicVar: "public",
    protectedVar: "protected", 
    privateVar: "private"
};

/* All properties are accessible - Grapa trusts developers */
example.publicVar.echo();      /* "public" */
example.protectedVar.echo();   /* "protected" */
example.privateVar.echo();     /* "private" */
```

**Note:** Grapa intentionally omits access control modifiers. This aligns with its late-binding philosophy where all properties are accessible, promoting transparency and reducing complexity.

### Meta-programming and Code Generation

**JavaScript:**
```javascript
// eval() for dynamic code execution
const code = "console.log('Hello, World!')";
eval(code);

// Function constructor
const dynamicFunc = new Function('x', 'return x * 2');

// Dynamic property access
const obj = { a: 1, b: 2 };
const key = 'a';
console.log(obj[key]);
```

**Grapa:**
```grapa
/* Grapa has superior built-in meta-programming capabilities */
/* Dynamic code execution via $sys().eval() */
code = "('Hello, World!').echo();";
$sys().eval(code);

/* Dynamic function creation via op() */
dynamicFunc = op(@<x,{x * 2}>);
result = dynamicFunc(21);  /* 42 */

/* Dynamic property access */
obj = {a: 1, b: 2};
key = "a";
obj[key].echo();  /* 1 */

/* Rule-based code generation */
rule = @<pattern,{@<action,{result}>}>;
generated_code = rule("input");
```

**Note:** Grapa's execution tree architecture provides superior meta-programming capabilities compared to traditional eval() approaches. The language is designed around dynamic code generation and manipulation.

### Guard Statements and Early Returns

**JavaScript:**
```javascript
function processData(data) {
    if (!data) {
        return null;
    }
    
    if (data.length === 0) {
        return [];
    }
    
    // Process data...
    return processedResult;
}
```

**Grapa:**
```grapa
/* Grapa uses existing control flow for guard patterns */
processData = @<data,{
    iferr(data, return(null));
    ifnull(data, return(null));
    
    if(data.len() == 0, return([]));
    
    /* Process data... */
    return(processedResult);
}>;
```

**Note:** Guard statements are just coding patterns using existing control flow. Grapa's `iferr()`, `ifnull()`, `if()`, and `return()` provide all the functionality needed for guard patterns without requiring special syntax.

### Decorators and Function Composition

**JavaScript:**
```javascript
// Decorator function
function validate(target, propertyKey, descriptor) {
    const originalMethod = descriptor.value;
    descriptor.value = function(...args) {
        if (args[0] > 0) {
            return originalMethod.apply(this, args);
        } else {
            throw new Error("Invalid input");
        }
    };
    return descriptor;
}

class Example {
    @validate
    processData(data) {
        return data * 2;
    }
}
```

**Grapa:**
```grapa
/* Grapa uses explicit function composition instead of decorators */
validate = op(f) { op(x) { if (x > 0) f(x); else $ERR("Invalid input"); }; };

processData = op(data) { data * 2; };

/* Compose functions explicitly */
validatedProcess = validate(processData);
result = validatedProcess(5);
```

**Note:** Grapa intentionally omits decorators in favor of explicit function composition. This approach is more transparent, flexible, and aligns with Grapa's late-binding philosophy. You can see exactly what's happening and compose functions dynamically.

### Generics and Type Abstraction

**JavaScript:**
```javascript
// Generic-like functions with type checking
function sortArray(items) {
    return items.sort();
}

function filterArray(items, predicate) {
    return items.filter(predicate);
}

// Usage with different types
const numbers = [3, 1, 4, 1, 5];
const strings = ["banana", "apple", "cherry"];

const sortedNumbers = sortArray(numbers);
const sortedStrings = sortArray(strings);
```

**Grapa:**
```grapa
/* Grapa handles type complexity in C++ libraries, not in scripts */
/* All methods work on any type automatically */

/* Sorting works on any array type */
numbers = [3, 1, 4, 1, 5];
sortedNumbers = numbers.sort();

strings = ["banana", "apple", "cherry"];
sortedStrings = strings.sort();

/* Filtering works on any type */
evens = numbers.filter(op(x) { x % 2 == 0; });
longStrings = strings.filter(op(s) { s.len() > 5; });

/* Mapping works on any type */
doubled = numbers.map(op(x) { x * 2; });
uppercase = strings.map(op(s) { s.upper(); });
```

**Note:** Grapa abstracts type complexity into C++ libraries rather than exposing generics in scripts. This approach is simpler, more performant, and leverages Grapa's dynamic typing strengths. The same methods work on all types without explicit type parameters.

### Reflection and Introspection

**JavaScript:** `typeof`, `Object.keys()`, `Object.getOwnPropertyNames()`, `Reflect` API
**Grapa:** Enhanced `.describe()` method with configurable options

**Grapa's Superior Reflection:**
```grapa
/* JavaScript: typeof obj, Object.keys(obj), Object.getOwnPropertyNames(obj) */
/* Grapa: Single method with configurable options */
obj.describe();                    /* Basic type and structure info */
obj.describe({values: true});      /* Include actual values */
obj.describe({structure: true});   /* Include internal structure details */
obj.describe({format: 'json'});    /* Machine-readable JSON output */

/* Float structure inspection (unique to Grapa) */
(2.3).describe({structure: true});
/* Shows 7 internal components: sign, trunc, fix, exp, max, extra, data */

/* Object property inspection */
user = {name: 'Alice', age: 30};
user.describe({properties: true});
/* Returns: "List with 2 properties (keys: name, age)" */
```

**Advantages of Grapa's Approach:**
- **Single Method**: One `.describe()` method handles all reflection needs
- **Configurable**: Options control what information is included
- **Multiple Formats**: Text for humans, JSON for programs
- **Float Structure**: Complete internal component inspection (unique feature)
- **Type-Specific**: Detailed information for each data type
- **Performance**: Efficient implementation with minimal overhead