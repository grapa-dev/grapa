# Grapa Operators Analysis

This document provides a comprehensive analysis of all operators defined in Grapa's BNF grammar and their type support based on source code examination.

## Operators Defined in BNF Grammar

### Assignment Operators
| Operator | BNF Name | Description | Source Function |
|----------|----------|-------------|-----------------|
| `=` | assign | Basic assignment | `HandleAssign` |
| `+=` | assignappend | Append assignment | `HandleAssignAppend` |
| `++=` | assignextend | Extend assignment | `HandleAssignExtend` |
| `-=` | pop | Pop assignment | `HandlePop` |

### Arithmetic Operators
| Operator | BNF Name | Description | Source Function |
|----------|----------|-------------|-----------------|
| `+` | add | Addition | `HandleAdd` |
| `-` | sub | Subtraction | `HandleSub` |
| `*` | mul | Multiplication | `HandleMul` |
| `/` | div | Division | `HandleDiv` |
| `%` | mod | Modulo | `HandleMod` |
| `**` | pow | Power | `HandlePow` |
| `*/` | root | Root | `HandleRoot` |

### Comparison Operators
| Operator | BNF Name | Description | Source Function |
|----------|----------|-------------|-----------------|
| `==` | eq | Equal | `HandleEq` |
| `!=` | neq | Not equal | `HandleNeq` |
| `<` | lt | Less than | `HandleLt` |
| `<=` | lteq | Less than or equal | `HandleLteq` |
| `>` | gt | Greater than | `HandleGt` |
| `>=` | gteq | Greater than or equal | `HandleGteq` |
| `<=>` | cmp | Three-way comparison | `HandleCmp` |

### Bitwise Operators
| Operator | BNF Name | Description | Source Function |
|----------|----------|-------------|-----------------|
| `&` | band | Bitwise AND | `HandleBand` |
| `|` | bor | Bitwise OR | `HandleBor` |
| `^` | xor | Bitwise XOR | `HandleXor` |
| `<<` | bsl | Bit shift left | `HandleBsl` |
| `>>` | bsr | Bit shift right | `HandleBsr` |
| `~` | inv | Bitwise NOT | `HandleInv` |

### Logical Operators
| Operator | BNF Name | Description | Source Function |
|----------|----------|-------------|-----------------|
| `&&` | and | Logical AND | `HandleAnd` |
| `||` | or | Logical OR | `HandleOr` |
| `!` | not | Logical NOT | `HandleNot` |

### Other Operators
| Operator | BNF Name | Description | Source Function |
|----------|----------|-------------|-----------------|
| `++` | extend | Extend | `HandleExtend` |
| `--` | remove | Remove | `HandleRemove` |
| `.*` | dot | Dot product | `HandleDot` |

## Type Support Analysis

### Addition Operator (`+`) - Comprehensive Type Support

Based on source code analysis of `GrapaLibraryRuleAddEvent::Run`:

#### Supported Type Combinations:
1. **String Operations**:
   - `STR + STR` → String concatenation
   - `STR + INT` → String + integer converted to string
   - `STR + FLOAT` → String + float converted to string
   - `STR + TIME` → String + time converted to string

2. **Numeric Operations**:
   - `INT + INT` → Integer addition
   - `INT + FLOAT` → Float addition (promotes to float)
   - `FLOAT + INT` → Float addition
   - `FLOAT + FLOAT` → Float addition
   - `TIME + INT` → Time addition
   - `TIME + TIME` → Time addition

3. **Vector/Array Operations**:
   - `VECTOR + VECTOR` → Vector addition
   - `VECTOR + ARRAY` → Vector + array elements
   - `VECTOR + TUPLE` → Vector + tuple elements
   - `VECTOR + INT` → Vector + scalar
   - `VECTOR + FLOAT` → Vector + scalar
   - `ARRAY + VECTOR` → Array + vector elements
   - `ARRAY + ARRAY` → Array + array elements
   - `ARRAY + TUPLE` → Array + tuple elements
   - `ARRAY + INT` → Array + scalar
   - `ARRAY + FLOAT` → Array + scalar

4. **Widget Operations**:
   - `WIDGET + WIDGET` → Widget combination

5. **Raw Data Operations**:
   - `RAW + RAW` → Raw data concatenation

### Subtraction Operator (`-`) - Limited Type Support

Based on source code analysis of `GrapaLibraryRuleSubEvent::Run`:

#### Supported Type Combinations:
1. **Numeric Operations**:
   - `INT - INT` → Integer subtraction
   - `INT - FLOAT` → Float subtraction
   - `FLOAT - INT` → Float subtraction
   - `FLOAT - FLOAT` → Float subtraction
   - `TIME - TIME` → Time difference (returns INT)
   - `TIME - INT` → Time subtraction
   - `INT - TIME` → Integer subtraction

2. **String Operations**:
   - `STR - STR` → String comparison (returns comparison result)

3. **Vector Operations**:
   - `VECTOR - VECTOR` → Vector subtraction
   - `VECTOR - ARRAY` → Vector - array elements
   - `VECTOR - TUPLE` → Vector - tuple elements
   - `VECTOR - INT` → Vector - scalar
   - `VECTOR - FLOAT` → Vector - scalar
   - `ARRAY - VECTOR` → Array - vector elements
   - `ARRAY - ARRAY` → Array - array elements
   - `ARRAY - TUPLE` → Array - tuple elements
   - `ARRAY - INT` → Array - scalar
   - `ARRAY - FLOAT` → Array - scalar

### Comparison Operators - Comprehensive Type Support

Based on source code analysis of comparison operators:

#### Equal (`==`) and Not Equal (`!=`):
- **Numeric**: `INT`, `FLOAT`, `TIME` comparisons
- **String**: `STR` comparisons
- **Boolean**: `BOOL` comparisons
- **Vector**: `VECTOR` comparisons
- **Array**: `ARRAY` comparisons
- **Tuple**: `TUPLE` comparisons

#### Less Than (`<`) and Greater Than (`>`):
- **Numeric**: `INT`, `FLOAT`, `TIME` comparisons
- **String**: `STR` lexicographic comparisons
- **Vector**: `VECTOR` element-wise comparisons

### Bitwise Operators - Integer Support

All bitwise operators (`&`, `|`, `^`, `<<`, `>>`, `~`) support:
- **INT** operands
- **SYSINT** operands
- Automatic type conversion from other numeric types

### Logical Operators - Boolean Support

Logical operators (`&&`, `||`, `!`) support:
- **BOOL** operands
- **INT** operands (non-zero = true, zero = false)
- **FLOAT** operands (non-zero = true, zero = false)
- **STR** operands (non-empty = true, empty = false)

## Documentation Gaps Identified

### 1. Missing Operator Documentation
The following operators are defined in BNF but lack comprehensive documentation:
- **`*/` (root)** - Mathematical root operation
- **`<=>` (cmp)** - Three-way comparison operator
- **`++` (extend)** - Extend operation
- **`--` (remove)** - Remove operation
- **`.*` (dot)** - Dot product operation

### 2. Incomplete Type Support Documentation
Current documentation doesn't fully cover:
- **Vector operations** with arithmetic operators
- **Array operations** with arithmetic operators
- **Widget operations** with addition
- **Time arithmetic** operations
- **Raw data** operations

### 3. Missing Assignment Operator Documentation
The assignment operators need better documentation:
- **`+=` (assignappend)** - Append assignment behavior
- **`++=` (assignextend)** - Extend assignment behavior
- **`-=` (pop)** - Pop assignment behavior

### 4. Type Conversion Rules
Documentation should clarify:
- **Automatic type promotion** (INT → FLOAT)
- **String conversion** rules for numeric types
- **Vector/Array conversion** rules
- **Widget combination** rules

## Optimization Opportunities

### High-Priority Optimizations
1. **Constant folding** for all arithmetic operators
2. **Type-specific optimizations** for common type combinations
3. **Vector operation optimizations** for large arrays
4. **String operation optimizations** for concatenation

### Medium-Priority Optimizations
1. **Comparison operator optimizations** for sorted data
2. **Bitwise operation optimizations** for integer operations
3. **Logical operator short-circuiting** optimizations

## Recommendations

### 1. Update Operator Documentation
- Add comprehensive type support tables for each operator
- Document type conversion rules and precedence
- Include examples for all supported type combinations

### 2. Add Missing Operator Documentation
- Document the `root` operator (`*/`)
- Document the three-way comparison operator (`<=>`)
- Document extend/remove operations (`++`, `--`)
- Document dot product operation (`.*`)

### 3. Improve Type Documentation
- Add type conversion rules to type documentation
- Document vector/array arithmetic operations
- Document widget combination operations
- Document time arithmetic operations

### 4. Create Operator Precedence Guide
- Document operator precedence based on BNF grammar
- Include examples of complex expressions
- Show how parentheses affect evaluation order

## Source Code References

### Key Files:
- **BNF Grammar**: `lib/grapa/$grapa.grc` (lines 540-630)
- **Operator Implementations**: `source/grapa/GrapaLibRule.cpp`
- **Function Table**: Lines 2282-2569 in `GrapaLibRule.cpp`

### Key Implementation Patterns:
- **Type checking**: `r1.vVal->mValue.mToken == GrapaTokenType::[TYPE]`
- **Type conversion**: `FromBytes()` and `getBytes()` methods
- **Vector operations**: `GrapaVector` class methods
- **Error handling**: `Error()` function for unsupported operations 