# Python-to-Grapa Migration Guide

## Key Insight: Grapa is Fundamentally Complete

**Grapa covers ALL the fundamental Python programming concepts** that developers use day-to-day. The remaining "gaps" are either:
- Advanced features that most developers never use
- Syntactic sugar that Grapa handles differently but just as effectively
- Areas where Grapa actually provides superior alternatives

**Grapa's unique advantages over Python:**
- ✅ **Built-in parallel processing** (no multiprocessing setup needed)
- ✅ **Unlimited precision arithmetic** (no decimal/mpmath imports)
- ✅ **Advanced pattern matching** (superior to Python's re module)
- ✅ **Execution tree introspection** (impossible in Python)
- ✅ **Native JSON/XML processing** (no external libraries)
- ✅ **Built-in file system integration** (no pathlib/os imports)
- ✅ **Native database operations** (no sqlite3/pandas needed)
- ✅ **Superior dynamic code execution** (human-readable execution trees)

> **Important: Access Patterns for Data Types (Tested, v0.0.39)**
>
> **Recent Improvements (Phase 1 & 2):**
> - ✅ **Native for loops**: `for x in arr { ... }` - Direct Python-like iteration
> - ✅ **Comprehensive comments**: `/* */`, `/** */`, `//`, `///` - Full commenting support
> - ✅ **String interpolation**: `"Hello ${name}".interpolate()` - Template literal support
> - ✅ **Enhanced operators**: `*=`, `/=`, `%=`, `**=` - Compound assignment operators
> - ✅ **List comprehension**: `[x*2 for x in arr]` - Native list comprehension
> - ✅ **Regex matching**: `text.match(pattern)` - Boolean pattern matching
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

This guide helps Python users transition to Grapa by mapping common Python idioms, patterns, and code to their Grapa equivalents.

> **See Also:**
> - [Python Integration Guide](../python_integration.md)
> - [Python Use Cases](../python_use_cases.md)

## Syntax Mapping Table

| Python | Grapa |
|--------|-------|
| `for x in arr:` | `for x in arr { ... }`<br>`i = 0; while (i < arr.len()) { x = arr[i]; ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if cond:` | `if (cond) { ... }` |
| `def f(x): ...` | `f = op(x) { ... };` |
| `list.append(x)` | `arr += x;` |
| `dict['key']` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| `list[index]` | `arr[index]`<br>`arr.get(index)` |
| (file access) | `file.get("key")` |
| `try: ... except:` | `result.iferr(fallback)`<br>`if (result.type() == $ERR) { ... }` |
| `# comment` | `/* comment */` (block)<br>`/** comment */` (doc block)<br>`// comment` (line)<br>`/// comment` (doc line) |
| `True/False` | `true/false` |
| `None` | `null` |
| `str(x)` | `x.str()` |
| `int(x)` | `x.int()` |
| `float(x)` | `x.float()` |
| `len(arr)` | `arr.len()` |
| `map(f, arr)` | `arr.map(op(x) { f(x); })` |
| `filter(f, arr)` | `arr.filter(op(x) { f(x); })` |
| `reduce(f, arr, init)` | `arr.reduce(op(a, b) { f(a, b); }, init)` |
| `re.findall(pattern, text)` | `text.grep(pattern, "o")` |
| `len(re.findall(pattern, text))` | `text.grep(pattern, "c")[0].int()` |
| `re.match(pattern, text)` | `text.match(pattern)` |
| `range(n)` | `(n).range(0,1)` |
| `x = x + 1` | `x = x + 1;`<br>`x += 1;` (preferred) |
| `x = x * 2` | `x = x * 2;`<br>`x *= 2;` (preferred) |
| `x = x / 2` | `x = x / 2;`<br>`x /= 2;` (preferred) |
| `x = x % 3` | `x = x % 3;`<br>`x %= 3;` (preferred) |
| `x = x ** 2` | `x = x ** 2;`<br>`x **= 2;` (preferred) |
| `s = s + "x"` | `s = s + "x";`<br>`s += "x";` (preferred) |
| `lambda x: x * 2` | `op(x) { x * 2; }` |
| `def f(x=1): ...` | `f = op("x"=1) { ... };` |
| `def f(*args): ...` | `f = op(args) { ... };` |
| `f"Hello {name}"` | `"Hello ${name}".interpolate()`<br>`"Hello " + name` (concatenation) |
| `if (x := f()) > 0:` | `x = f(); if (x > 0) { ... }` |
| `async def f(): await g()` | Use Grapa's built-in parallelism: `data.map(op(x) { process(x); }, 8)` |
| `(x*2 for x in arr)` | `arr.map(op(x) { x * 2; })` (Grapa is eager by default)<br>`[x*2 for x in arr]` (list comprehension)<br>`[x*2 for x in arr]` (native list comprehension) |
| `with open() as f:` | Use `$file()` methods directly: `content = $file().read("file.txt")` |
| `@decorator` | Use function composition: `f = op() { decorator(original_func); }` |
| `arr1 + arr2` | `arr1 += arr2` |
| `{a, b} = obj` | `@local ++= obj; a.echo(); b.echo();` |

> **Note:** Both `x = x + 1;` and `x += 1;` (and `s = s + "x";` and `s += "x";`) are valid in Grapa. The `+=` form is idiomatic and preferred in most cases.
>
> **String Interpolation:** For combining strings and values, use `"Hello ${name}".interpolate()` instead of `"Hello " + name`. String interpolation is more powerful and less error-prone than concatenation.
>
> **Nullish Coalescing:** For providing default values, use `value.ifnull("default")` instead of `value or "default"`. The `.ifnull()` method treats a broader range of values as nullish (including zeros, empty collections, and errors).

> **Note:** `.get("key")` is only for `$file` (and possibly one other system object). For `$LIST`/`$OBJ`, use `obj["key"]`, `obj.key`, or `obj."key"`. For `$ARRAY`, use `arr[index]` (and `arr.get(index)` if supported).

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
- No `.get()`/`.set()` on lists/arrays—use `[]` for access (except for `$file`/$TABLE`)
- ✅ **Line comments now supported** - Use `// comment` or `/// doc comment` in addition to block comments
- No attribute-style access for dict/list keys—use `[]`, or dot notation for `$LIST`/`$OBJ`
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads
- Use `.iferr()` for simple error fallback; use `if (result.type() == $ERR)` only for explicit error handling
- **Count-only grep returns array**: `text.grep(pattern, "c")` returns `["2"]` not `2` - use `text.grep(pattern, "c")[0].int()` to get the number

## Example Code Pairs

**Python:**
```python
# Sum squares of even numbers
result = sum(x*x for x in arr if x % 2 == 0)
```
**Grapa:**
```grapa
result = arr.filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(a, b) { a + b; }, 0);
```

**Python:**
```python
# Read file lines
with open('file.txt') as f:
    lines = f.readlines()
```
**Grapa:**
```grapa
lines = $file().read("file.txt").split("\n");
```

**Python:**
```python
# Dictionary access
value = d['key']
```
**Grapa:**
```grapa
value = obj["key"];
value = obj.key;
value = obj."key";
```

**Python:**
```python
# File access
value = file['key']
```
**Grapa:**
```grapa
value = file.get("key");
```

**Python:**
```python
# Generate numbers 0..9
seq = list(range(10))
```
**Grapa:**
```grapa
seq = (10).range(0,1);
```

**Python:**
```python
# Sum numbers 0..9
sum = 0
for i in range(10):
    sum += i
```
**Grapa:**
```grapa
/* Using native for loop (new!) */
sum = 0;
for i in 10 {
    sum += i;
};

/* Or using functional approach */
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
```

**Python:**
```python
# Collect even numbers 0..9
evens = []
for i in range(10):
    if i % 2 == 0:
        evens.append(i)
```
**Grapa:**
```grapa
/* Using native for loop (new!) */
evens = [];
for i in 10 {
    if (i % 2 == 0) {
        evens += i;
    };
};

/* Or using functional approach */
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
```

**Python:**
```python
# Squares of 0..9
squares = [x*x for x in range(10)]
```
**Grapa:**
```grapa
squares = (10).range(0,1).map(op(x) { x * x; });
/* Or using list comprehension */
squares = [x*x for x in 10];
```

**Python:**
```python
# Even numbers 0..9
evens = [x for x in range(10) if x % 2 == 0]
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
/* Or using list comprehension */
evens = [x for x in 10 if x % 2 == 0];
```

**Python:**
```python
# Error fallback
try:
    result = some_operation()
except:
    result = 0
```
**Grapa:**
```grapa
result = some_operation().iferr(0);
```

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

> **Tip:** Grapa's parallel ETL/data processing is robust, production-ready, and a core design goal. Unlike Python, you can use parallel methods like `.map()` and `.filter()` out of the box for high-throughput data tasks.

## Notes
- See [Operators](../syntax/operator.md) and [System](../sys/sys.md) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions.

## See Also
- [Migration Tips for Python Users](../python_use_cases.md#migration-tips-for-python-users)
- [Python Integration Guide](../python_integration.md)

---

*If you have more Python idioms you want mapped to Grapa, please open an issue or PR!* 

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - If more objects support `.get()` in the future, this guide will be updated. 

> **Comment Style:**
> - ✅ **Comprehensive comment support**: `/* */` (block), `/** */` (doc block), `//` (line), `///` (doc line)
> - Comments work everywhere including at end of lines and inside code blocks 

## Customizing Grapa for Familiar Syntax

If you prefer Python-style function calls, you can define your own `print()` function in Grapa:

```grapa
# Define a print function similar to Python
print = op("value"=""){value.echo();};
print("Hello from Grapa!");
```

This can make migration easier for those used to Python's `print()` or similar functions.

## Debugging and Logging

Grapa provides sophisticated debugging capabilities that go beyond Python's simple `print()` statements. While Grapa doesn't have `__FILE__` and `__LINE__` macros, it offers superior debugging through the `.debug()` method with component targeting and level control:

### Basic Debug Output
```grapa
/* Standard output - equivalent to Python's print() */
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

### Debug vs Echo Comparison

| Method | Purpose | Debug Integration | Output Location |
|--------|---------|-------------------|-----------------|
| `.echo()` | General output | No | Standard output |
| `.debug()` | Debug output | Yes | Debug stream (stderr) |

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

### Python Migration Examples

| Python | Grapa |
|--------|-------|
| `print("Hello")` | `"Hello".echo()` |
| `print(f"Count: {count}")` | `"Count: ${count}".interpolate().echo()` |
| `logging.debug("Debug info")` | `"Debug info".debug(1, "debug")` |
| `logging.info("Info message")` | `"Info message".debug(1, "info")` |
| `logging.warning("Warning")` | `"Warning".debug(2, "warning")` |
| `logging.error("Error")` | `"Error".debug(3, "error")` |
| `print(f"Processing {i}/{total}")` | `"Processing ${i}/${total}".interpolate().debug(1, "progress")` |

> **Advanced:** Grapa also allows advanced users to customize or extend the language syntax using `$RULE` or by modifying `$global` rules. This enables you to inject your own grammar or override built-in behaviors to match your preferred style. For most users, we recommend learning the canonical Grapa method syntax, but this flexibility is available if needed.

## Work-in-Progress (WIP) Items

Some Python idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Custom metaclasses**: - No metaclass system
- **Descriptor protocol**: - No descriptor system
- **Contextlib utilities**: - No context manager protocol
- **Pickle protocol**: - No serialization protocol
- **Shelve**: - No persistent storage protocol
- **Multiprocessing**: - No process-based parallelism (only thread-based)

### Nice to Have
These would improve developer experience but aren't essential:

- **Optional chaining**: `obj?.user?.profile` - Use `.iferr()` for superior safe property access with custom fallback values: `obj.user.iferr(null).profile.iferr("default")`
- **Nullish coalescing**: `x ?? y` - Use `.ifnull()` for superior nullish coalescing: `x.ifnull(y)`
- **F-strings**: `f"Hello {name}"` - Use string interpolation: `"Hello ${name}".interpolate()` or see [String Templates and Dynamic Construction](../type/str.md#string-templates-and-dynamic-construction) for advanced patterns
- **Function chaining**: `"hello".upper() + " world".lower()` - Use string interpolation: `"${'hello'.upper()} ${'world'.lower()}".interpolate()`
- **Walrus operator**: `if (x := f()) > 0:` - Use separate assignment: `x = f(); if (x > 0) { ... }`
- **Match statements**: `match x:` - Use `if/else` chains
- **Structural pattern matching**: - Use explicit property access
- **Dataclasses**: `@dataclass` - Use regular object construction
- **Enums**: `from enum import Enum` - Use constants or objects
- **Named tuples**: `from collections import namedtuple` - Use objects with properties
- **Property decorators**: `@property` - Use regular methods
- **Class methods**: `@classmethod` - Use regular functions
- **Static methods**: `@staticmethod` - Use regular functions
- **Abstract base classes**: - Use regular classes
- **Metaclasses**: - Use regular classes
- **Slots**: `__slots__` - Grapa objects are optimized automatically

### Advanced Meta-Programming (Available in Grapa)
Grapa actually provides **superior** dynamic code execution capabilities:

- **Dynamic code execution**: `op()("'hello world'.echo();")()` - Direct string-to-function compilation
- **System evaluation**: `$sys().eval("x + y", {"x": 5, "y": 3})` - System-level evaluation with parameters
- **Compiled execution**: `$sys().compile("script")` then `$sys().eval(compiled)` - Compile once, execute many
- **Execution trees**: Direct manipulation and execution of `$OP` objects
- **Parameter binding**: Both positional and named parameter support
- **Introspection**: Execution trees can be examined and modified

### Grapa Features That Python Lacks

Grapa provides many capabilities that Python doesn't have, making it superior for certain tasks:

#### **Built-in Parallel Processing**
```grapa
/* Parallel processing is built-in and production-ready */
data = (1000000).range(0,1);
/* 8 threads */
result = data.map(op(x) { x * x; }, 8);
/* 8 threads */
filtered = data.filter(op(x) { x % 2 == 0; }, 8);
```

**Python equivalent requires:** `multiprocessing`, `concurrent.futures`, or `asyncio` with significant setup

#### **Unlimited Precision Arithmetic**
```grapa
/* Native unlimited precision - no imports needed */
big_num = 123456789012345678901234567890 * 987654321098765432109876543210;
big_num.echo();  /* Exact result, no overflow */
```

**Python equivalent requires:** `decimal.Decimal` or `mpmath` with precision limits

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

**Python equivalent requires:** `re` module with manual context handling

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

**Python equivalent requires:** `json` and `xml.etree.ElementTree` modules

#### **Execution Tree Introspection**
```grapa
/* Examine and manipulate compiled code directly */
func = op(x){x * 2 + 1};
func;  /* Shows: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}> */

/* Direct tree execution */
/* 11 */
@<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}>(5);
```

**Python equivalent:** Not possible - Python bytecode is not human-readable

#### **Built-in File System Integration**
```grapa
/* Direct file system operations */
$file().set("test.txt", "Hello World");
content = $file().read("test.txt");
$file().rm("test.txt");
```

**Python equivalent requires:** `pathlib` or `os` module

#### **Native Table/Database Operations**
```grapa
/* Built-in table operations */
table = $file().table("ROW");
table.mkfield("name", "STR", "VAR");
table.mkfield("age", "INT");
table.set("user1", "John", "name");
table.set("user1", 30, "age");
```

**Python equivalent requires:** `sqlite3`, `pandas`, or external database libraries

#### **Vector and Matrix Operations**
```grapa
/* Native vector operations */
v1 = <1, 2, 3>;
v2 = <4, 5, 6>;
dot_product = v1.dot(v2);  /* 32 */
```

**Python equivalent requires:** `numpy` library

#### **Built-in Error Fallback**
```grapa
/* Graceful error handling */
result = risky_operation().iferr(0);  /* Returns 0 if operation fails */
```

**Python equivalent requires:** `try/except` blocks

#### **Native Time/Date with Unlimited Precision**
```grapa
/* Unlimited precision time operations */
now = $TIME().utc();
future = now + 86400000000;  /* Add 1 day in microseconds */
```

**Python equivalent requires:** `datetime` with precision limits

#### **Direct System Integration**
```grapa
/* Access system information directly */
version = $sys().getenv($VERSION);
platform = $sys().getenv($PLATFORM);
```

**Python equivalent requires:** `sys`, `platform`, and `os` modules

### Rarely Used
These are advanced features that most developers won't miss:

- **Metaprogramming**: `exec()`, `eval()` - Use Grapa's built-in code generation
- **Descriptors**: - Use regular properties
- **Custom metaclasses**: - Use regular classes
- **Contextlib utilities**: - Use direct method calls
- **Inspect module**: - Use Grapa's reflection capabilities
- **Ast module**: - Use Grapa's built-in parsing
- **Dis module**: - Use Grapa's execution tracing
- **Pickle**: - Use Grapa's serialization
- **Shelve**: - Use Grapa's persistent storage
- **Multiprocessing**: - Use Grapa's built-in parallelism

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, async/await patterns are replaced by Grapa's built-in parallel processing with `.map()` and `.filter()`.

> **Note:** Grapa grep returns results as a structured array (list of strings), not a concatenated string. This is different from most CLI tools and allows for more precise, programmatic handling. This affects edge cases (like empty pattern matches and invert matches) and should be considered when migrating scripts or tests.

### Custom match() Function for Regex

Python users often use `re.match` or `re.search` for regex checks. You can define a similar function in Grapa:

```grapa
# Define a match function that returns true if the pattern is found
match = op("text"="", "pattern"="") {
    text.grep(pattern, "x").len() > 0;
};

# Usage
if (match("hello world", "world")) {
    "Found!".echo();
} else {
    "Not found.".echo();
}
```

This is a handy workaround until Grapa adds a native `.match()` method.

### Access Control and Visibility

**Python:**
```python
class Example:
    def __init__(self):
        self.public_var = "public"
        self._protected_var = "protected"
        self.__private_var = "private"
    
    def public_method(self):
        return self.public_var
    
    def _protected_method(self):
        return self._protected_var
    
    def __private_method(self):
        return self.__private_var
```

**Grapa:**
```grapa
/* Grapa doesn't need access modifiers - all properties are accessible */
/* This aligns with Grapa's late-binding, dynamic philosophy */
example = {
    public_var: "public",
    protected_var: "protected", 
    private_var: "private"
};

/* All properties are accessible - Grapa trusts developers */
example.public_var.echo();      /* "public" */
example.protected_var.echo();   /* "protected" */
example.private_var.echo();     /* "private" */
```

**Note:** Grapa intentionally omits access control modifiers. This aligns with its late-binding philosophy where all properties are accessible, promoting transparency and reducing complexity.

### Meta-programming and Code Generation

**Python:**
```python
# eval() for dynamic code execution
code = "print('Hello, World!')"
eval(code)

# exec() for statement execution
exec("x = 42")

# Function creation
dynamic_func = lambda x: x * 2
```

**Grapa:**
```grapa
/* Grapa has superior built-in meta-programming capabilities */
/* Dynamic code execution via $sys().eval() */
code = "('Hello, World!').echo();";
$sys().eval(code);

/* Dynamic function creation via op() */
dynamic_func = op(@<x,{x * 2}>);
result = dynamic_func(21);  /* 42 */

/* Rule-based code generation */
rule = @<pattern,{@<action,{result}>}>;
generated_code = rule("input");
```

**Note:** Grapa's execution tree architecture provides superior meta-programming capabilities compared to traditional eval() approaches. The language is designed around dynamic code generation and manipulation.

### Guard Statements and Early Returns

**Python:**
```python
def process_data(data):
    if not data:
        return None
    
    if len(data) == 0:
        return []
    
    # Process data...
    return processed_result
```

**Grapa:**
```grapa
/* Grapa uses existing control flow for guard patterns */
process_data = @<data,{
    iferr(data, return(null));
    ifnull(data, return(null));
    
    if(data.len() == 0, return([]));
    
    /* Process data... */
    return(processed_result);
}>;
```

**Note:** Guard statements are just coding patterns using existing control flow. Grapa's `iferr()`, `ifnull()`, `if()`, and `return()` provide all the functionality needed for guard patterns without requiring special syntax.

### Decorators and Function Composition

**Python:**
```python
@validate
@cache
@log
def process_data(data):
    return data * 2

# Usage
result = process_data(5)
```

**Grapa:**
```grapa
/* Grapa uses explicit function composition instead of decorators */
validate = op(f) { op(x) { if (x > 0) f(x); else $ERR("Invalid input"); }; };
cache = op(f) { 
    cache_data = {};
    op(x) { 
        if (cache_data[x]) cache_data[x];
        else cache_data[x] = f(x);
    };
};
log = op(f) { op(x) { ("Calling " + f.str()).echo(); result = f(x); ("Result: " + result.str()).echo(); result; }; };

process_data = op(data) { data * 2; };

/* Compose functions explicitly */
validated_process = validate(process_data);
cached_process = cache(process_data);
logged_process = log(process_data);

/* Or chain them */
final_process = log(cache(validate(process_data)));
result = final_process(5);
```

**Note:** Grapa intentionally omits decorators in favor of explicit function composition. This approach is more transparent, flexible, and aligns with Grapa's late-binding philosophy. You can see exactly what's happening and compose functions dynamically.

### Generics and Type Abstraction

**Python:**
```python
from typing import TypeVar, List

T = TypeVar('T')

def sort_list(items: List[T]) -> List[T]:
    return sorted(items)

def filter_list(items: List[T], predicate) -> List[T]:
    return [item for item in items if predicate(item)]
```

**Grapa:**
```grapa
/* Grapa handles type complexity in C++ libraries, not in scripts */
/* All methods work on any type automatically */

/* Sorting works on any array type */
numbers = [3, 1, 4, 1, 5];
sorted_numbers = numbers.sort();

strings = ["banana", "apple", "cherry"];
sorted_strings = strings.sort();

/* Filtering works on any type */
evens = numbers.filter(op(x) { x % 2 == 0; });
long_strings = strings.filter(op(s) { s.len() > 5; });

/* Mapping works on any type */
doubled = numbers.map(op(x) { x * 2; });
uppercase = strings.map(op(s) { s.upper(); });
```

**Note:** Grapa abstracts type complexity into C++ libraries rather than exposing generics in scripts. This approach is simpler, more performant, and leverages Grapa's dynamic typing strengths. The same methods work on all types without explicit type parameters.

## Notes
- See [Operators](../syntax/operator.md) and [System](../sys/sys.md) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions.

### Reflection and Introspection

**Python:** `type()`, `dir()`, `getattr()`, `hasattr()`, `inspect` module
**Grapa:** Enhanced `.describe()` method with configurable options

**Grapa's Superior Reflection:**
```grapa
/* Python: type(obj), dir(obj), getattr(obj, 'attr', default) */
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