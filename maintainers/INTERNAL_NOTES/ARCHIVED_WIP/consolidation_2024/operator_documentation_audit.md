# Operator Documentation Audit

## Overview

This document tracks the comprehensive audit and update of Grapa operator documentation based on source code analysis.

**Status**: Ready to start  
**Priority**: HIGH  
**Estimated Effort**: 2-3 days  
**Dependencies**: Core functionality understanding (pending code review)

## Background

Based on the comprehensive operator analysis in `maintainers/INTERNAL_NOTES/GRAPA_OPERATORS_ANALYSIS.md`, significant gaps have been identified in the current operator documentation:

### Key Findings
- **5 Missing Operators**: `*/` (root), `<=>` (cmp), `++` (extend), `--` (remove), `.*` (dot)
- **Incomplete Type Support**: Missing type combination tables for operators
- **Array Assignment Behavior**: `+=` (nesting) vs `++=` (flattening) not documented
- **Operator Precedence**: Needs update based on BNF grammar

## Audit Scope

### 1. **Missing Operator Documentation**
Document the 5 operators currently missing from user documentation:

| Operator | Name | Function | Status |
|----------|------|----------|--------|
| `*/` | root | Mathematical root | ❌ Missing |
| `<=>` | cmp | Three-way comparison | ❌ Missing |
| `++` | extend | Array extension | ❌ Missing |
| `--` | remove | Array removal | ❌ Missing |
| `.*` | dot | Dot product | ❌ Missing |

### 2. **Type Support Documentation**
Create comprehensive type support tables for all operators:

#### Arithmetic Operators
- `+`, `-`, `*`, `/`, `%`, `**`, `*/`
- Type combinations: INT, FLOAT, STR, VECTOR, ARRAY, WIDGET
- Type conversion rules and examples

#### Assignment Operators
- `=`, `+=`, `++=`, `-=`, `*=`, `/=`, `%=`
- Array assignment behavior: `+=` (nesting) vs `++=` (flattening)
- Type-specific assignment rules

#### Comparison Operators
- `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`
- Type comparison behavior
- Boolean conversion rules

#### Bitwise Operators
- `&`, `|`, `^`, `<<`, `>>`, `~`
- Integer type support
- Type conversion behavior

#### Logical Operators
- `&&`, `||`, `!`
- Boolean conversion rules
- Short-circuiting behavior

### 3. **Operator Precedence**
Update operator precedence documentation based on BNF grammar in `lib/grapa/$grapa.grc` (lines 540-630).

### 4. **Examples and Use Cases**
Add comprehensive examples for:
- Array operations and type combinations
- Type conversion scenarios
- Common operator patterns
- Edge cases and gotchas

## Files to Update

### Primary Documentation Files
1. **`docs-site/docs/operators/assignment.md`**
   - Add array assignment examples
   - Document `+=` vs `++=` behavior
   - Add type-specific assignment rules

2. **`docs-site/docs/API_REFERENCE.md`**
   - Expand operator sections
   - Add type support tables
   - Include missing operators

3. **`docs-site/docs/syntax/operator.md`**
   - Update operator precedence
   - Add comprehensive type rules
   - Document operator behavior

4. **`docs-site/docs/EXAMPLES.md`**
   - Add operator examples
   - Include type combination examples
   - Show common patterns

### New Files to Create
1. **`docs-site/docs/operators/`** - New operator documentation directory
   - `arithmetic.md` - Arithmetic operator documentation
   - `comparison.md` - Comparison operator documentation
   - `bitwise.md` - Bitwise operator documentation
   - `logical.md` - Logical operator documentation
   - `missing.md` - Documentation for missing operators

## Context References

### Source Code Analysis
- **`maintainers/INTERNAL_NOTES/GRAPA_OPERATORS_ANALYSIS.md`** - Complete operator analysis
- **`lib/grapa/$grapa.grc`** (lines 540-630) - BNF operator definitions
- **`source/grapa/GrapaLibRule.cpp`** - Operator implementations

### Key Implementation Sections
- **Assignment operators**: Lines 4204-4500
- **Arithmetic operators**: Lines 11722-12000
- **Comparison operators**: Lines 17395-17821

## Specific Examples to Document

### Array Assignment Behavior
```grapa
// Array assignment examples
array1 = [1, 2, 3];
array2 = [4, 5, 6];
array1 += array2;    // Result: [1, 2, 3, [4, 5, 6]] (nested)
array1 ++= array2;   // Result: [1, 2, 3, 4, 5, 6] (flattened)
```

### Type Combination Examples
```grapa
// Type combination examples
str = "hello" + 42;  // Result: "hello42" (string conversion)
vec = [1, 2] + [3, 4]; // Result: [1, 2, 3, 4] (vector addition)
widget = $widget() + $widget(); // Result: combined widget
```

### Missing Operator Examples
```grapa
// Root operator
result = 16 */ 2;    // Result: 4 (square root)

// Three-way comparison
result = 5 <=> 3;    // Result: 1 (greater)
result = 3 <=> 5;    // Result: -1 (less)
result = 5 <=> 5;    // Result: 0 (equal)

// Array extension
array = [1, 2, 3];
array ++ [4, 5];     // Result: [1, 2, 3, 4, 5]

// Array removal
array = [1, 2, 3, 4, 5];
array -- [3, 4];     // Result: [1, 2, 5]

// Dot product
result = [1, 2, 3] .* [4, 5, 6]; // Result: 32
```

## Success Criteria

### Documentation Completeness
- [ ] All 25 operators documented
- [ ] Type support tables complete for all operators
- [ ] Array assignment behavior clearly explained
- [ ] Operator precedence updated and accurate
- [ ] Comprehensive examples provided

### Code Examples
- [ ] All examples empirically validated
- [ ] Type conversion examples included
- [ ] Edge cases documented
- [ ] Common patterns demonstrated

### User Experience
- [ ] Clear, understandable explanations
- [ ] Consistent formatting and structure
- [ ] Cross-references between related operators
- [ ] Integration with existing documentation

## Progress Tracking

### Phase 1: Analysis and Planning
- [x] Operator inventory complete
- [x] Source code analysis complete
- [x] Gap analysis complete
- [ ] Core functionality understanding (pending)

### Phase 2: Documentation Updates
- [ ] Update assignment operator documentation
- [ ] Create missing operator documentation
- [ ] Update API reference
- [ ] Update syntax documentation

### Phase 3: Examples and Validation
- [ ] Add comprehensive examples
- [ ] Validate all examples empirically
- [ ] Cross-reference and link documentation
- [ ] Final review and testing

## Notes

- All documentation must be empirically validated against actual Grapa implementation
- Examples should be tested to ensure they work as documented
- Type support tables should be comprehensive and accurate
- Operator precedence should match the BNF grammar exactly
- Array assignment behavior is particularly important for user understanding 