# JavaScript-to-Grapa Migration Guide

This guide helps JavaScript users transition to Grapa by mapping common JS idioms, patterns, and code to their Grapa equivalents.

## Syntax Mapping Table

| JavaScript | Grapa |
|------------|-------|
| `for (let i=0; i<arr.length; i++)` | `i = 0; while (i < arr.len()) { ...; i = i + 1; }` |
| `if (cond) { ... }` | `if (cond) { ... }` |
| `function f(x) { ... }` | `f = op(x) { ... };` |
| `arr.push(x)` | `arr += x;` |
| `arr[index]` | `arr[index]` |
| `obj.key` | `obj.get("key")` |
| `try { ... } catch { ... }` | `if (result.type() == $ERR) { ... }` |
| `// comment` | `/* comment */` (block only) |
| `true/false` | `true/false` |
| `null/undefined` | `null` |
| `String(x)` | `x.str()` |
| `Number(x)` | `x.int()` or `x.float()` |
| `arr.length` | `arr.len()` |
| `arr.map(f)` | `arr.map(op(x) { f(x); })` |
| `arr.filter(f)` | `arr.filter(op(x) { f(x); })` |
| `arr.reduce(f, init)` | `arr.reduce(op(a, b) { f(a, b); }, init)` |
| `Array.from({length: n}, (_, i) => i)` | `(n).range(0,1)` |

## Common Pitfalls
- No `for`/`foreach` loops—use `while`
- No `try/catch`—check for `$ERR`
- No `.push()`/`.pop()`—use `+=` and manual index for pop
- No `//` comments—only block comments (`/* ... */`)
- No attribute-style access for object keys—use `.get()`
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops.
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks.
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads.

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
// Object property access
let value = obj.key;
```
**Grapa:**
```grapa
value = obj.get("key");
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
    if (i % 2 === 0) evens.push(i);
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
let squares = Array.from({length: 10}, (_, i) => i*i);
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

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```
> **Tip:** Grapa's parallel ETL/data processing is robust, production-ready, and a core design goal. Unlike JavaScript, you can use parallel methods like `.map()` and `.filter()` out of the box for high-throughput data tasks.

> **See Also:**
> - [Getting Started](GETTING_STARTED.md)
> - [Language Reference](FUNCTION_QUICK_REFERENCE.md)
> - [Examples](EXAMPLES.md)

---

*If you have more JavaScript idioms you want mapped to Grapa, please open an issue or PR!* 