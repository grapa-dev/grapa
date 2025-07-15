# Ripgrep Parity Test Matrix

This document defines the set of test cases for systematic Grapa vs. ripgrep output comparison.

## Test Categories
- Basic literal matching
- Unicode and grapheme clusters
- Multiline patterns
- Lookaround assertions
- Option combinations (-o, -v, -n, -c, -A, -B, -C, etc.)
- Custom delimiters (where possible)
- Edge/error cases

## Test Case Table
| ID  | Input File         | Pattern         | Options         | Description                        |
|-----|--------------------|-----------------|-----------------|-------------------------------------|
| 1   | basic.txt          | foo             |                 | Simple literal match                |
| 2   | basic.txt          | bar             | -i              | Case-insensitive match              |
| 3   | unicode.txt        | café            |                 | Unicode literal                     |
| 4   | unicode.txt        | \X              | -o              | Grapheme cluster extraction         |
| 5   | multiline.txt      | ^foo.*bar$      | -m              | Multiline pattern                   |
| 6   | lookaround.txt     | foo(?=bar)      |                 | Lookahead assertion                 |
| 7   | basic.txt          | foo             | -v              | Invert match                       |
| 8   | basic.txt          | foo             | -n              | Line numbers                        |
| 9   | basic.txt          | foo             | -c              | Count only                          |
| 10  | context.txt        | foo             | -A1             | After context                       |
| 11  | context.txt        | foo             | -B1             | Before context                      |
| 12  | context.txt        | foo             | -C1             | Context (before & after)            |
| 13  | delimiter.txt      | foo             | -o --delim=|||  | Custom delimiter (if supported)     |
| 14  | error.txt          | (unclosed       |                 | Invalid pattern                     |
| 15  | unicode.txt        | \p{Han}+        |                 | Unicode script property             |
| ... | ...                | ...             | ...             | ...                                 |

## Notes
- For each case, run both Grapa and ripgrep, capture output, and compare.
- For custom delimiters, ripgrep may not support this directly; note as N/A if not applicable.
- For each difference, record as: expected, bug, or unsupported.

---
*Expand this table as new cases are identified during testing.* 