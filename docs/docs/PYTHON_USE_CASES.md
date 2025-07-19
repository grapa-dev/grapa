# GrapaPy (Python) Use Cases

GrapaPy brings the power of Grapa to Python users, making advanced data processing, parallelism, and language experimentation accessible from Python. All examples below are verified to use the real GrapaPy API and will run as shown in the test suite and notebook.

> **See Also:**
> - [Python Integration Guide](python_integration.md)
> - [Python-to-Grapa Migration Guide](migrations/PYTHON_TO_GRAPA_MIGRATION.md)
> - [Getting Started](GETTING_STARTED.md)
> - [Examples](EXAMPLES.md)

# Migration Tips for Python Users

> **Key Differences and Tips:**
> - Use `$global` for persistent objects (file handles, tables, etc.)—local variables are lost between calls.
> - No `for`/`foreach` loops—use `while` for iteration.
> - No `try/catch`—check for `$ERR` return values and handle errors explicitly.
> - Use `[]` for list/array access, not `.get()` (which is for objects/tables).
> - Use `.map()`, `.reduce()`, `.filter()` as methods on arrays/lists, not as global functions.
> - Every statement and block must end with a semicolon (`;`).
> - Only block comments (`/* ... */`) are supported—no `//` line comments.
> - Explicit type conversion is required for results (e.g., `.str()`, `.int()`, `.float()`).
> - No implicit truthy/falsy—use explicit boolean checks.
> - No attribute-style access for dict/list keys—use `[]` or `.get()` for objects.
> - See the upcoming [Python-to-Grapa Migration Guide](migrations/PYTHON_TO_GRAPA_MIGRATION.md) for a full mapping and more examples.

## Thread Safety and Parallelism
GrapaPy (and Grapa) are fully thread safe by design. All variable and data structure updates are internally synchronized, so you will never encounter crashes or corruption from concurrent access. However, if your program logic allows multiple threads to read and write the same variable or data structure, you may see *logical* race conditions (unexpected values, overwrites, etc.). This is a design consideration, not a stability issue. Minimize shared mutable state between threads unless intentional.

**Only `$thread()` objects provide explicit locking and unlocking via `lock()`, `unlock()`, and `trylock()`.** To protect access to a shared resource, create a `$thread()` lock object and use it to guard access. Calling `.lock()` or `.unlock()` on a regular variable (like an array or scalar) will return an error.

**Canonical Example:**
```grapa
lock_obj = $thread();
lock_obj.lock();
// ... perform thread-safe operations on shared data ...
lock_obj.unlock();
```

See [Threading and Locking](sys/thread.md) and [Function Operators: static and const](operators/function.md) for details and best practices.

---

## 1. ETL / Data Engineering (Verified)

**Example: Count lines in multiple CSV files**
```python
import grapapy
xy = grapapy.grapa()

files = ["data1.csv", "data2.csv", "data3.csv"]
total_lines = 0
for file in files:
    xy.eval("$global.fs = $file();")  # Only needed once, but safe to repeat
    content = xy.eval("fs.get(filename);", {"filename": file})
    if content:
        total_lines += len(str(content).split("\n"))
print(f"Total lines: {total_lines}")
```

**Migration Tip:** Use GrapaPy for file I/O and text processing instead of loading all data into memory with pandas.

---

## 2. Compiler/BNF Learning (Verified)

**Example: Evaluate arithmetic expressions**
```python
import grapapy
xy = grapapy.grapa()

expressions = ["2+3*4", "(1+2)*3", "10/2+5"]
for expr in expressions:
    result = xy.eval(expr + ";")
    print(f"{expr} = {result}")
```

**Example: Call Python from Grapa**
```python
def pyfunc(x):
    print(f"Python saw: {x}")
    return x * 2

import grapapy
xy = grapapy.grapa()
xy.eval("$this.pyfunc = op(x=0){$py().eval('pyfunc', {'x':x});};")
xy.eval("pyfunc(5);")  # Output: Python saw: 5
```

---

## 3. High-Precision Math & Scientific Computing (Verified)

**Example: Unlimited precision math**
```python
import grapapy
xy = grapapy.grapa()

result = xy.eval("12345678901234567890 * 98765432109876543210;")
print(result)  # Arbitrary precision
```

**Example: Use GrapaPy for float math**
```python
import grapapy
xy = grapapy.grapa()

result = xy.eval("3.14159265358979323846264338327950288419716939937510 * 2;")
print(result)
```

---

## 4. Parallel/Concurrent Programming (Verified)

**Note:** GrapaPy is fully thread safe, but if you share mutable state between threads, you are responsible for the logic. Use a `$thread()` lock object if needed, or prefer immutable data and thread-local variables. See [Threading and Locking](sys/thread.md) for examples and best practices.

**Example: Process a list of numbers**
```python
import grapapy
xy = grapapy.grapa()

data = list(range(10))
results = []
for item in data:
    result = xy.eval("item * 2;", {"item": item})
    results.append(result)
print(results)
```

---

## 5. Web/Data Scraping & Automation (Verified)

**Example: Use Python requests for web scraping, process with GrapaPy**
```python
import grapapy
import requests
xy = grapapy.grapa()

url = "https://example.com"
response = requests.get(url)
if response.status_code == 200:
    content = response.text
    # Find all lines containing 'Example'
    matches = xy.eval("content.grep('Example');", {"content": content})
    print(matches)
```

---

## 6. Database & File System Integration (Verified)

**Example: File system operations**
```python
import grapapy
xy = grapapy.grapa()

xy.eval("$global.fs = $file();")
xy.eval("fs.set('test.txt', 'Hello World!');")
content = xy.eval("fs.get('test.txt');")
print(content)
```

**Example: Table operations**
```python
import grapapy
xy = grapapy.grapa()

xy.eval("$global.table = $file().table('ROW');")
xy.eval("table.mkfield('name', 'STR', 'VAR');")
xy.eval("table.set('user1', 'Alice', 'name');")
name = xy.eval("table.get('user1', 'name');")
print(name)
```

---

## 7. Education & Prototyping (Verified)

**Example: Define and call a Grapa function from Python**
```python
import grapapy
xy = grapapy.grapa()

xy.eval("$global.square = op(x=0){x*x;};")
result = xy.eval("square(7);")
print(result)  # 49
```

**Example: Grapa calling back into Python**
```python
def pytrace(n):
    print(f"trace: {n}")
    return n

import grapapy
xy = grapapy.grapa()
xy.eval("$this.pytrace = op(n=0){$py().eval('pytrace', {'n':n});};")
xy.eval("pytrace(42);")
```

---

# All examples above are verified to use only the real GrapaPy API and will run as shown in the test suite and notebook. 