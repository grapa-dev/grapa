# $VECTOR

A `$VECTOR` is a high-performance, multi-dimensional, dense array/matrix type in Grapa. It is ideal for fast numeric operations, matrix algebra, and ETL/data workloads. Unlike `$ARRAY` and `$LIST`, a `$VECTOR` requires all rows to have the same number of columns (rectangular shape) and is optimized for large, in-memory data.

## Construction
- **From arrays/lists:**
  ```grapa
  #[[1, 2], [3, 4]]#
  #["a", "b", "c"]#
  [1, 2, 3].vector();
  ```
- **From CSV/delimited text:**
  ```grapa
  csv = "a,b,c\n1,2,3\n4,5,6";
  v = csv.vector();
  ```
- **From single values:**
  ```grapa
  #[42]#
  ```
- **Rectangularity required:** All rows must have the same length.

## Key Features
- Fast, dense, in-memory storage for numeric and string data
- Supports 1D and 2D (and higher) shapes
- Supports column labels (parsed from CSV header or set programmatically)
- Can store executable `$OP` objects for advanced functional programming
- Parallelized `.map()`, `.filter()`, and other methods for ETL workloads

## Common Operations
| Method      | Signature/Example                | Description |
|-------------|----------------------------------|-------------|
| `.t()`      | `v.t()`                          | Transpose (swap axes) |
| `.rref()`   | `v.rref()`                       | Row-reduced echelon form |
| `.inv()`    | `v.inv()`                        | Matrix inverse (square only) |
| `.det()`    | `v.det()`                        | Determinant |
| `.rank()`   | `v.rank()`                       | Matrix rank |
| `.solve()`  | `v.solve()`                      | Solve linear system |
| `.cov()`    | `v.cov(axis=0)`                  | Covariance matrix |
| `.sum()`    | `v.sum(axis=0)`                  | Sum along axis |
| `.mean()`   | `v.mean(axis=0)`                 | Mean along axis |
| `.shape()`  | `v.shape()`                      | Get shape (dimensions) |
| `.reshape()`| `v.reshape([rows, cols])`        | Reshape to new dims |
| `.dot()`    | `v.dot(other)`                   | Dot product |
| `.triu()`   | `v.triu(n=0)`                    | Upper triangle |
| `.tril()`   | `v.tril(n=0)`                    | Lower triangle |
| `.eigh()`   | `v.eigh()`                       | Eigenvalues/vectors |
| `.array()`  | `v.array()`                      | Convert to $ARRAY |
| `.tuple()`  | `v.tuple()`                      | Convert to $TUPLE |
| `.to()`     | `v.to(",")`                     | Export to CSV/delimited text |
| `.from()`   | `str.from()`                     | Import from CSV/delimited text |
| `.map()`    | `v.map(op(x){x*2;})`             | Elementwise map (parallelized) |
| `.filter()` | `v.filter(op(x){x>0;})`          | Filter elements (parallelized) |
| `.reduce()` | `v.reduce(op(a,b){a+b;}, 0)`     | Reduce elements |

## Math and Operator Support
- Elementwise: `+`, `-`, `*`, `/`, `**` (power)
- Matrix: `.dot()`, `.inv()`, `.det()`, `.rref()`, `.solve()`, etc.
- Chaining: All methods can be chained for expressive data pipelines

## CSV Import/Export
- Use `.from()` to import from CSV/delimited text (auto-detects delimiter, supports quoted fields, header row for labels)
- Use `.to()` to export to CSV/delimited text (custom delimiter, includes labels if present)
- Nulls are exported as empty fields

## Labels (Column Headings)
- If the first row of a CSV is non-numeric, it is used as column labels
- Labels can be set programmatically
- Access by index: `v[0][1]` (row 0, column 1)
- Access by label: not yet supported (use index)

## Executable `$OP` in Vector
- `$VECTOR` can store `$OP` objects (compiled functions) as elements
- Use with `.map()`, `.filter()`, `.sort(op, ...)`, etc.
- Example:
  ```grapa
  v = #[1, 2, 3]#;
  v2 = v.map(op(x){x*2;});  /* [2, 4, 6] */
  ```

## Examples
```grapa
/* Construction */
v = #[[2,5],[3,8]]#;

/* Math operations */
v2 = v + 2;           /* #[[4,7],[5,10]]# */
v3 = v * 2 + 1;       /* #[[5,11],[7,17]]# */
v4 = v ** 2;          /* #[[4,25],[9,64]]# */

/* Transpose */
v.t();                /* #[[2,3],[5,8]]# */

/* Row-reduced echelon form */
v.rref();

/* Determinant */
v.det();

/* Dot product */
v.dot(#[4, 5]#);

/* CSV import/export */
csv = "a,b\n1,2\n3,4";
v = csv.vector();
v.to(",");

/* Functional programming */
v.map(op(x){x*2;});
v.filter(op(x){x>2;});
v.reduce(op(a,b){a+b;}, 0);
```

## Best Practices & Common Pitfalls
- Always ensure vectors are rectangular (all rows same length)
- Use bracket notation for element access: `v[0][1]`
- Use `.len()` for array/list length, not `.size()`
- Use `.map()`, `.filter()`, `.reduce()` for functional operations
- For large data, specify thread count in `.map()`/`.filter()` for parallelism
- Use `.iferr()` for error fallback: `v.inv().iferr([])`
- Avoid using `.get()` for $VECTOR (not supported)

## Error Handling
- Most methods return `$ERR` or empty result on invalid shapes, types, or parameters
- Use `.iferr(default)` for fallback values

> **Note:**
> All code samples are empirically validated and follow the canonical [Basic Syntax Guide](../syntax/basic_syntax.md).

---
For advanced details, see the [Deep Expert Implementation Overview](../deep_expert_implementation_overview.md).

