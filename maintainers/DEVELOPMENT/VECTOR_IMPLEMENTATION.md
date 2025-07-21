# $VECTOR Implementation and Capabilities (Maintainer Doc)

## Overview
$VECTOR is Grapa's high-performance, multi-dimensional, dense array/matrix type. It supports fast numeric operations, matrix algebra, and is optimized for ETL/data workloads. Unlike $ARRAY/$LIST, $VECTOR uses contiguous memory and requires rectangular shape.

## User-Facing Methods
| Method      | Signature/Params         | Description |
|-------------|-------------------------|-------------|
| t           | v.t()                   | Transpose (swap axes) |
| rref        | v.rref()                | Row-reduced echelon form |
| inv         | v.inv()                 | Matrix inverse |
| det         | v.det()                 | Determinant |
| rank        | v.rank()                | Matrix rank |
| solve       | v.solve()               | Solve linear system |
| cov         | v.cov(axis=null)        | Covariance matrix |
| sum         | v.sum(axis=null)        | Sum along axis |
| mean        | v.mean(axis=null)       | Mean along axis |
| shape       | v.shape()               | Get shape (dimensions) |
| reshape     | v.reshape(b)            | Reshape to new dims |
| norm        | v.norm()                | Vector/matrix norm |
| dot         | v.dot(b)                | Dot product |
| triu        | v.triu(b=null)          | Upper triangle |
| tril        | v.tril(b=null)          | Lower triangle |
| eigh        | v.eigh()                | Eigenvalues/vectors |

## Operator Support
- +, -, *, /, **, */ (elementwise, matrix, and vector operations)
- Chaining and parallel functional methods (e.g., .map(), .filter())

## Special Capabilities
- **CSV import/export:**
  - Can construct a $VECTOR from a CSV string or file (see GrapaVector::FROM with GrapaBYTE input)
  - Can export a $VECTOR to CSV (see GrapaVector::TO with GrapaBYTE output)
- **Column headings:**
  - $VECTOR supports column labels via mLabels (GrapaVector::mLabels)
  - Labels are parsed from CSV headers or can be set programmatically
- **Executable $OP in vector:**
  - $VECTOR can store $OP (operation/function) objects as elements (see GrapaVectorValue, GrapaRuleEvent with OP token)
  - Enables storing executable code in vector cells for advanced meta-programming

## C++ Implementation
- All methods are dispatched via GrapaLibRule.cpp (see GrapaLibraryRuleVectorEvent)
- Core logic in GrapaVector.cpp/h
- Parameter handling via GrapaRuleEvent, GrapaVectorParam
- Most methods support both 1D and 2D vectors, some support axis param
- Error handling: returns $ERR or empty for invalid shapes/inputs
- Parallelism: some methods are parallelized for ETL/data workloads

## Known Issues / Limitations
- Must be rectangular (all rows same length)
- No sparse support (dense only)
- Some advanced linear algebra (e.g., SVD, QR) not present
- Type safety: all elements must be numeric for most operations
- Error handling may be inconsistent for some edge cases
- Not all methods are fully parallelized

## Recommendations / TODOs
- Add more advanced linear algebra (SVD, QR, LU, etc.)
- Improve error handling and reporting
- Add sparse vector/matrix support
- Ensure all methods are parallelized where possible
- Expand CSV import/export options (delimiters, quoting, etc.)
- Document all edge cases and error returns

## What Should Be Updated in Public Docs
- Ensure all user-facing methods are documented in docs/type/vector.md
- Add missing examples for methods not currently shown
- Clarify error cases and shape requirements
- Add a summary table of all methods and their signatures
- Document CSV import/export, column headings, and $OP-in-vector capabilities for advanced users

---
This document should be updated as new features are added or issues are discovered. 