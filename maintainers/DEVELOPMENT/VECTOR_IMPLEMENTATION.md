# $VECTOR Implementation and Capabilities (Maintainer Doc)

## Overview
$VECTOR is Grapa's high-performance, multi-dimensional, dense array/matrix type. It supports fast numeric operations, matrix algebra, and is optimized for ETL/data workloads. Unlike $ARRAY/$LIST, $VECTOR uses contiguous memory and requires rectangular shape.

## Construction and Supported Input Types
- Can be constructed from:
  - $ARRAY, $TUPLE, $LIST (arbitrary depth, must be rectangular)
  - $STR (CSV or delimited text, with auto-detection of delimiter, header, and BOM)
  - $VECTOR (copy/clone)
  - $INT, $FLOAT (single value vector)
- CSV import supports:
  - Auto-detection of delimiter (comma or tab)
  - Optional header row (labels)
  - Quoted fields (with double-quote escaping)
  - UTF-8 BOM handling
  - Null/empty fields
- Labels (column headings) are parsed from the first row if non-numeric, or can be set programmatically. Stored in mLabels.
- Supports storing $OP (operation/function) objects as elements, enabling executable code in vector cells.

## Data Type Handling
- Elements can be $INT, $FLOAT, $STR, or $OP (operation/function)
- Type promotion: numeric operations require all elements to be $INT/$FLOAT; $STR/$OP are handled as special cases
- Nulls are supported and tracked per element
- Output types:
  - Most methods return $VECTOR, $ARRAY, $TUPLE, or $STR depending on context and method
  - .to()/.from() can convert between $VECTOR and $STR (CSV), $ARRAY, $TUPLE

## Methods and Operators (from GrapaVector.cpp and GrapaLibRule.cpp)
| Method      | Signature/Params         | Input Types | Output Type | Description/Notes |
|-------------|-------------------------|-------------|-------------|------------------|
| t           | v.t()                   | VECTOR      | VECTOR      | Transpose (swap axes) |
| transpose   | v.transpose()           | VECTOR      | VECTOR      | Alias for t() |
| rref        | v.rref()                | VECTOR      | VECTOR      | Row-reduced echelon form |
| inv         | v.inv()                 | VECTOR      | VECTOR      | Matrix inverse (square only) |
| det         | v.det()                 | VECTOR      | FLOAT       | Determinant (square only) |
| rank        | v.rank()                | VECTOR      | INT         | Matrix rank |
| solve       | v.solve()               | VECTOR      | VECTOR      | Solve linear system (augmented matrix) |
| cov         | v.cov(axis=0/1)         | VECTOR      | VECTOR      | Covariance matrix (axis param) |
| sum         | v.sum(axis=0/1)         | VECTOR      | VECTOR      | Sum along axis |
| mean        | v.mean(axis=0/1)        | VECTOR      | VECTOR      | Mean along axis |
| shape       | v.shape()               | VECTOR      | ARRAY       | Get shape (dimensions) |
| reshape     | v.reshape(shape)        | VECTOR,ARRAY| VECTOR      | Reshape to new dims |
| norm        | v.norm()                | VECTOR      | FLOAT       | Vector/matrix norm |
| dot         | v.dot(b)                | VECTOR      | VECTOR      | Dot product |
| triu        | v.triu(n=0)             | VECTOR      | VECTOR      | Upper triangle (n offset) |
| tril        | v.tril(n=0)             | VECTOR      | VECTOR      | Lower triangle (n offset) |
| eigh        | v.eigh()                | VECTOR      | (w,p) VECTOR| Eigenvalues/vectors |
| left        | v.left(n)               | VECTOR      | VECTOR      | Leftmost n elements/columns |
| right       | v.right(n)              | VECTOR      | VECTOR      | Rightmost n elements/columns |
| split       | v.split(n,axis=0/1)     | VECTOR      | ARRAY       | Split along axis |
| join        | v.join(array)           | ARRAY       | VECTOR      | Vertical join |
| joinh       | v.joinh(array)          | ARRAY       | VECTOR      | Horizontal join |
| reverse     | v.reverse()             | VECTOR      | VECTOR      | Reverse rows |
| ref         | v.ref(reduce=true/false)| VECTOR      | VECTOR      | Row echelon form (optionally reduced) |
| identity    | v.identity(n)           | INT         | VECTOR      | Identity matrix |
| diagonal    | v.diagonal(n)           | VECTOR      | VECTOR      | Diagonal extraction |
| get         | v.get(i)                | VECTOR,INT  | INT/FLOAT/STR/OP| Get element |
| set         | v.set(i, val)           | VECTOR,INT,VAL| VECTOR    | Set element |
| to          | v.to(delim)             | VECTOR,STR  | STR         | Export to CSV/delimited text |
| from        | v.from(str,delim)       | STR         | VECTOR      | Import from CSV/delimited text |
| array       | v.array()               | VECTOR      | ARRAY       | Convert to $ARRAY |
| tuple       | v.tuple()               | VECTOR      | TUPLE       | Convert to $TUPLE |
| list        | v.list()                | VECTOR      | LIST        | Convert to $LIST |
| raw         | v.raw()                 | VECTOR      | RAW         | Raw bytes |
| str         | v.str()                 | VECTOR      | STR         | String representation |
| mid         | v.mid(start,len)        | VECTOR      | VECTOR      | Subvector |
| midtrim     | v.midtrim(start,len)    | VECTOR      | VECTOR      | Subvector with trim |
| ltrim/rtrim/trim | v.ltrim() etc.     | VECTOR      | VECTOR      | Trim operations |
| sort        | v.sort(op,axis,desc)    | VECTOR      | VECTOR      | Sort rows/columns, custom op |
| unique      | v.unique()              | VECTOR      | VECTOR      | Unique rows/columns |
| map/filter/reduce| v.map(op) etc.     | VECTOR      | VECTOR      | Functional ops (via $OP) |
| pow         | v.pow(b)                | VECTOR      | VECTOR      | Elementwise power |
| add/sub     | v.add(b)/v.sub(b)       | VECTOR      | VECTOR      | Elementwise add/sub |
| mul/div     | v.mul(b)/v.div(b)       | VECTOR      | VECTOR      | Elementwise mul/div |

## Operator Support
- +, -, *, /, **, */ (elementwise, matrix, and vector operations)
- Chaining and parallel functional methods (e.g., .map(), .filter(), .reduce())
- Custom $OP can be passed for sort, map, filter, reduce, etc.

## Edge Cases and Error Handling
- All input arrays/tuples/lists must be rectangular (same length per row)
- Type promotion: mixed numeric types promoted to $FLOAT
- $STR/$OP elements are not valid for most numeric operations (error or null result)
- Nulls are preserved and propagated
- Axis parameter: 0 (rows), 1 (columns) for axis-aware methods
- Parallelism: some methods are parallelized for ETL/data workloads
- Error returns: methods return $ERR or null on invalid shapes, types, or parameters

## CSV Import/Export Details
- Import: auto-detects delimiter, handles quoted fields, BOM, nulls, and header row
- Export: supports custom delimiter, quoting, and label row
- Nulls exported as empty fields
- $OP and non-scalar types exported as quoted string representations

## Executable $OP in Vector
- $VECTOR can store $OP (operation/function) objects as elements
- $OP is parsed from CSV using special quoting (=""..."")
- $OP is executed in functional methods (map, filter, sort, etc.)
- $OP elements are not valid for numeric operations (error/null)

## Limitations and TODOs
- Only dense, rectangular vectors supported (no sparse)
- No support for ragged/jagged arrays
- Some advanced linear algebra (SVD, QR, LU) not implemented
- Error handling may be inconsistent for some edge cases
- Not all methods are fully parallelized
- Type safety: numeric operations require all elements to be $INT/$FLOAT
- CSV import/export: limited delimiter/quoting options

## Table: All Methods, Signatures, Types, Notes
(See above table for full list)

## What Should Be Updated in Public Docs
- Ensure all user-facing methods are documented in docs/type/vector.md
- Add missing examples for methods not currently shown
- Clarify error cases, type requirements, and axis handling
- Add a summary table of all methods and their signatures
- Document CSV import/export, label handling, and $OP-in-vector capabilities for advanced users

---
This document is based on direct inspection of GrapaVector.cpp and GrapaLibRule.cpp. Update as new features or edge cases are discovered. 