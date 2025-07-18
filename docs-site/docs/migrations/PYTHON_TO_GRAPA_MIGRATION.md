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
> - [Python Integration Guide](../PYTHON_INTEGRATION.md)
> - [Python Use Cases](PYTHON_USE_CASES.md)

## Syntax Mapping Table

| Python | Grapa |
|--------|-------|
| `for x in arr:` | `i = 0; while (i < arr.len()) { x = arr[i]; ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if cond:` | `if (cond) { ... }` |
| `def f(x): ...` | `f = op(x) { ... };` |
| `list.append(x)` | `arr += x;` |
| `dict['key']` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| `list[index]` | `arr[index]`<br>`arr.get(index)` |
| (file access) | `file.get("key")` |
| `try: ... except:` | `result.iferr(fallback)`<br>`if (result.type() == $ERR) { ... }` |
| `# comment` | `/* comment */` (block only, own line) |
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
| `range(n)` | `(n).range(0,1)` |
| `x = x + 1` | `x = x + 1;`<br>`x += 1;` (preferred) |
| `s = s + "x"` | `s = s + "x";`<br>`s += "x";` (preferred) |
| `lambda x: x * 2` | `op(x) { x * 2; }` |
| `def f(x=1): ...` | `f = op("x"=1) { ... };` |
| `def f(*args): ...` | `f = op(args) { ... };` |
| `f"Hello {name}"` | `"Hello " + name` |
| `if (x := f()) > 0:` | `x = f(); if (x > 0) { ... }` |
| `async def f(): await g()` | Use Grapa's built-in parallelism: `data.map(op(x) { process(x); }, 8)` |
| `(x*2 for x in arr)` | `arr.map(op(x) { x * 2; })` (Grapa is eager by default) |
| `with open() as f:` | Use `$file()` methods directly: `content = $file().read("file.txt")` |
| `@decorator` | Use function composition: `f = op() { decorator(original_func); }` |
| `arr1 + arr2` | `arr1 += arr2` |

> **Note:** Both `x = x + 1;` and `x += 1;` (and `s = s + "x";` and `s += "x";`) are valid in Grapa. The `+=` form is idiomatic and preferred in most cases.

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
- No `.get()`/`.set()` on lists/arrays—use `[]` for access (except for `$file`/`$TABLE`)
- No `//` or `#` comments—only block comments (`/* ... */`), always on their own line
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
evens = (10).range(0,1).reduce(op(acc, x) {
    if (x % 2 == 0) { acc += x; };
}, []);
```

**Python:**
```python
# Squares of 0..9
squares = [x*x for x in range(10)]
```
**Grapa:**
```grapa
squares = (10).range(0,1).map(op(x) { x * x; });
```

**Python:**
```python
# Even numbers 0..9
evens = [x for x in range(10) if x % 2 == 0]
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
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

## See Also
- [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users)
- [Python Integration Guide](../PYTHON_INTEGRATION.md)

---

*If you have more Python idioms you want mapped to Grapa, please open an issue or PR!* 

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$LIST`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$LIST`, and `$OBJ`.
> - If more objects support `.get()` in the future, this guide will be updated. 

> **Comment Style:**
> - Only block comments (`/* ... */`) are supported in Grapa, and must always be on their own line.
> - `//` and `#` comments are not supported and will cause errors. 

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

- **F-strings**: `f"Hello {name}"` - Use string concatenation: `"Hello " + name`
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

**Python equivalent:** Not possible - bytecode is not human-readable

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