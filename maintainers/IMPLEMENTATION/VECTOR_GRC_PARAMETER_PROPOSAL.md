# $VECTOR.grc Parameter Renaming Proposal

## Overview
This document proposes improved parameter names for methods in `$VECTOR.grc` that currently have ambiguous parameter identifiers.

## Methods Requiring Parameter Renaming

### 1. reshape(b)
**Current parameters:**
- `b` - Shape/dimensions array

**Proposed parameters:**
- `shape` - Shape/dimensions array

**Rationale:** Based on C++ implementation analysis:
- `GrapaError ReShape(GrapaRuleEvent* shape)` - The C++ method takes a `shape` parameter
- The parameter represents the new shape/dimensions for the vector
- `shape` is the standard mathematical terminology for array dimensions

### 2. dot(b)
**Current parameters:**
- `b` - Other vector/matrix for dot product

**Proposed parameters:**
- `other` - Other vector/matrix for dot product

**Rationale:** Based on C++ implementation analysis:
- `GrapaError Dot(GrapaScriptExec* pScriptExec, GrapaNames* pNameSpace, GrapaVector& bi, GrapaVector& result)` - The C++ method takes a `bi` (second vector) parameter
- The parameter represents the other vector/matrix to compute dot product with
- `other` is clear and unambiguous

### 3. triu(b)
**Current parameters:**
- `b` - Diagonal offset for upper triangular

**Proposed parameters:**
- `offset` - Diagonal offset for upper triangular

**Rationale:** Based on C++ implementation analysis:
- `bool TriU(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result)` - The C++ method takes an `n` parameter representing the diagonal offset
- The parameter controls which diagonal elements to include in the upper triangular matrix
- `offset` clearly indicates this is a diagonal offset parameter

### 4. tril(b)
**Current parameters:**
- `b` - Diagonal offset for lower triangular

**Proposed parameters:**
- `offset` - Diagonal offset for lower triangular

**Rationale:** Based on C++ implementation analysis:
- `bool TriL(GrapaScriptExec* pScriptExec, s64 n, GrapaVector& result)` - The C++ method takes an `n` parameter representing the diagonal offset
- The parameter controls which diagonal elements to include in the lower triangular matrix
- `offset` clearly indicates this is a diagonal offset parameter

## Methods with Good Parameter Names (No Changes Needed)

The following methods already have descriptive parameter names and do not need changes:
- `cov(axis)` - Descriptive
- `sum(axis)` - Descriptive
- `mean(axis)` - Descriptive

## Implementation Plan

1. **Baseline Testing** ✅ - Completed
2. **Parameter Renaming** - Update `lib/grapa/$VECTOR.grc`
3. **Regression Testing** - Create and run test with new parameter names
4. **Documentation Update** - Update `docs-src/docs/type/vector.md`

## Expected Changes in $VECTOR.grc

```diff
- reshape = @<[op,@<"reshape",{@<this,{}>,@<var,{b}>}>],{"b":null}>;
+ reshape = @<[op,@<"reshape",{@<this,{}>,@<var,{shape}>}>],{"shape":null}>;

- dot = @<[op,@<"dot",{@<this,{}>,@<var,{b}>}>],{"b":null}>;
+ dot = @<[op,@<"dot",{@<this,{}>,@<var,{other}>}>],{"other":null}>;

- triu = @<[op,@<"triu",{@<this,{}>,@<var,{b}>}>],{"b":null}>;
+ triu = @<[op,@<"triu",{@<this,{}>,@<var,{offset}>}>],{"offset":null}>;

- tril = @<[op,@<"tril",{@<this,{}>,@<var,{b}>}>],{"b":null}>;
+ tril = @<[op,@<"tril",{@<this,{}>,@<var,{offset}>}>],{"offset":null}>;
```

## Impact Assessment

- **Low Risk**: Only parameter names are changing, not functionality
- **Backward Compatibility**: No breaking changes to method signatures
- **Documentation**: Will need updates to reflect new parameter names
- **Testing**: Comprehensive baseline and regression tests will validate no functional changes

