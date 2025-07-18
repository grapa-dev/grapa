# Python-to-Grapa Migration Guide

This guide helps Python users transition to Grapa by mapping common Python idioms, patterns, and code to their Grapa equivalents.

> **See Also:**
> - [GrapaPy Quickstart](GRAPAPY_INTEGRATION.md#quickstart-for-python-users)
> - [Python Use Cases](PYTHON_USE_CASES.md)

## Syntax Mapping Table

| Python | Grapa |
|--------|-------|
| `for x in arr:` | `i = 0; while (i < arr.len()) { x = arr[i]; ...; i = i + 1; }` |
| `if cond:` | `if (cond) { ... }` |
| `def f(x): ...` | `f = op(x) { ... };` |
| `list.append(x)` | `arr += x;` |
| `dict['key']` | `obj.get("key")` |
| `list[index]` | `arr[index]` |
| `try: ... except:` | `if (result.type() == $ERR) { ... }` |
| `# comment` | `/* comment */` (block only) |
| `True/False` | `true/false` |
| `None` | `null` |
| `str(x)` | `x.str()` |
| `int(x)` | `x.int()` |
| `float(x)` | `x.float()` |
| `len(arr)` | `arr.len()` |
| `map(f, arr)` | `arr.map(op(x) { f(x); })` |
| `filter(f, arr)` | `arr.filter(op(x) { f(x); })` |
| `reduce(f, arr, init)` | `arr.reduce(op(a, b) { f(a, b); }, init)` |
| `range(n)` | `(n).range(0,1)` |

## Common Pitfalls
- No `for`/`foreach` loops—use `while`
- No `try/catch`—check for `$ERR`
- No `.get()`/`.set()` on lists/arrays—use `[]` for access
- No `//` or `#` comments—only block comments (`/* ... */`)
- No attribute-style access for dict/list keys—use `[]` or `.get()`
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops.
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks.
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads.

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
value = obj.get("key");
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

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

> **Tip:** Grapa's parallel ETL/data processing is robust, production-ready, and a core design goal. Unlike Python, you can use parallel methods like `.map()` and `.filter()` out of the box for high-throughput data tasks.

## See Also
- [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users)
- [Quickstart for Python Users](GRAPAPY_INTEGRATION.md#quickstart-for-python-users)

---

*If you have more Python idioms you want mapped to Grapa, please open an issue or PR!* 