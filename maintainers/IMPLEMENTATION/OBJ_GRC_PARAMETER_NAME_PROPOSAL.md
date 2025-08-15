# $OBJ.grc Parameter Name Proposal

## Overview

This document proposes improved parameter names for all methods in `lib/grapa/$OBJ.grc`. The goal is to use shorter, more intuitive parameter names that maintain clarity while following modern programming conventions.

## Current Parameter Analysis

Based on the `$OBJ.grc` file, here are the current parameter names and proposed improvements:

### Type and Information Methods

#### `float(b, c)` → `float(precision, mode)`
- **Current**: `b` (precision), `c` (mode)
- **Proposed**: `precision` (number of decimal places), `mode` (rounding mode)
- **Rationale**: Clear purpose of each parameter

#### `fix(b, c)` → `fix(precision, mode)`
- **Current**: `b` (precision), `c` (mode)
- **Proposed**: `precision` (fixed-point precision), `mode` (rounding mode)
- **Rationale**: Same as float, consistent naming

#### `setfloat(b, c)` → `setfloat(precision, mode)`
- **Current**: `b` (precision), `c` (mode)
- **Proposed**: `precision` (float precision), `mode` (rounding mode)
- **Rationale**: Consistent with float/fix naming

#### `setfix(b, c)` → `setfix(precision, mode)`
- **Current**: `b` (precision), `c` (mode)
- **Proposed**: `precision` (fixed-point precision), `mode` (rounding mode)
- **Rationale**: Consistent naming pattern

#### `str(format)` → `str(format)`
- **Current**: `format` (format string)
- **Proposed**: `format` (format string)
- **Rationale**: Already clear

#### `vector(dim)` → `vector(dim)`
- **Current**: `dim` (dimension)
- **Proposed**: `dim` (dimension)
- **Rationale**: Already clear

#### `base(b)` → `base(radix)`
- **Current**: `b` (base/radix)
- **Proposed**: `radix` (number base)
- **Rationale**: More descriptive than `b`

#### `setconst(b)` → `setconst(value)`
- **Current**: `b` (value)
- **Proposed**: `value` (constant value)
- **Rationale**: Clear purpose

### String Manipulation Methods

#### `left(b)` → `left(count)`
- **Current**: `b` (count)
- **Proposed**: `count` (number of characters)
- **Rationale**: Clear purpose

#### `right(b)` → `right(count)`
- **Current**: `b` (count)
- **Proposed**: `count` (number of characters)
- **Rationale**: Clear purpose

#### `mid(b, c)` → `mid(start, count)`
- **Current**: `b` (start), `c` (count)
- **Proposed**: `start` (start position), `count` (number of characters)
- **Rationale**: Clear purpose of each parameter

#### `midtrim(items, offset, blocksize)` → `midtrim(items, offset, size)`
- **Current**: `items` (items), `offset` (offset), `blocksize` (block size)
- **Proposed**: `items` (items), `offset` (offset), `size` (block size)
- **Rationale**: `size` is shorter than `blocksize`

#### `rtrim(b)` → `rtrim(chars)`
- **Current**: `b` (characters to trim)
- **Proposed**: `chars` (characters to trim)
- **Rationale**: Clear purpose

#### `ltrim(b)` → `ltrim(chars)`
- **Current**: `b` (characters to trim)
- **Proposed**: `chars` (characters to trim)
- **Rationale**: Clear purpose

#### `trim(b)` → `trim(chars)`
- **Current**: `b` (characters to trim)
- **Proposed**: `chars` (characters to trim)
- **Rationale**: Clear purpose

#### `lrot(b)` → `lrot(count)`
- **Current**: `b` (count)
- **Proposed**: `count` (rotation count)
- **Rationale**: Clear purpose

#### `rrot(b)` → `rrot(count)`
- **Current**: `b` (count)
- **Proposed**: `count` (rotation count)
- **Rationale**: Clear purpose

#### `lpad(b, c)` → `lpad(width, char)`
- **Current**: `b` (width), `c` (character)
- **Proposed**: `width` (total width), `char` (padding character)
- **Rationale**: Clear purpose

#### `rpad(b, c)` → `rpad(width, char)`
- **Current**: `b` (width), `c` (character)
- **Proposed**: `width` (total width), `char` (padding character)
- **Rationale**: Clear purpose

#### `replace(b, c)` → `replace(old, new)`
- **Current**: `b` (old), `c` (new)
- **Proposed**: `old` (old text), `new` (new text)
- **Rationale**: Clear purpose

#### `interpolate(b)` → `interpolate(vars)`
- **Current**: `b` (variables)
- **Proposed**: `vars` (variables for interpolation)
- **Rationale**: Clear purpose

#### `grep(b, c, d, e, f, g)` → `grep(pattern, flags, start, end, count, offset)`
- **Current**: `b` (pattern), `c` (flags), `d` (start), `e` (end), `f` (count), `g` (offset)
- **Proposed**: `pattern` (regex pattern), `flags` (regex flags), `start` (start pos), `end` (end pos), `count` (max matches), `offset` (offset)
- **Rationale**: Clear purpose of each parameter

#### `match(b, c, d, e, f, g)` → `match(pattern, flags, start, end, count, offset)`
- **Current**: `b` (pattern), `c` (flags), `d` (start), `e` (end), `f` (count), `g` (offset)
- **Proposed**: `pattern` (regex pattern), `flags` (regex flags), `start` (start pos), `end` (end pos), `count` (max matches), `offset` (offset)
- **Rationale**: Same as grep

#### `split(sep, max, axis)` → `split(delim, max, axis)`
- **Current**: `sep` (separator), `max` (max splits), `axis` (axis)
- **Proposed**: `delim` (delimiter), `max` (max splits), `axis` (axis)
- **Rationale**: `delim` is more common than `sep`

#### `join(sep, axis)` → `join(delim, axis)`
- **Current**: `sep` (separator), `axis` (axis)
- **Proposed**: `delim` (delimiter), `axis` (axis)
- **Rationale**: Consistent with split

#### `findall(term)` → `findall(pattern)`
- **Current**: `term` (search term)
- **Proposed**: `pattern` (search pattern)
- **Rationale**: More descriptive

### Functional Programming Methods

#### `map(callback, data, threads)` → `map(func, data, threads)`
- **Current**: `callback` (callback function), `data` (data), `threads` (thread count)
- **Proposed**: `func` (function), `data` (data), `threads` (thread count)
- **Rationale**: `func` is shorter and clearer

#### `filter(callback, data, threads)` → `filter(func, data, threads)`
- **Current**: `callback` (callback function), `data` (data), `threads` (thread count)
- **Proposed**: `func` (function), `data` (data), `threads` (thread count)
- **Rationale**: Consistent with map

#### `reduce(callback, first, data)` → `reduce(func, init, data)`
- **Current**: `callback` (callback function), `first` (first value), `data` (data)
- **Proposed**: `func` (function), `init` (initial value), `data` (data)
- **Rationale**: `init` is clearer than `first`

#### `sort(axis, order, kind)` → `sort(axis, order, kind)`
- **Current**: `axis` (axis), `order` (order), `kind` (kind)
- **Proposed**: `axis` (axis), `order` (order), `kind` (kind)
- **Rationale**: Already clear

#### `argsort(axis, order, kind)` → `argsort(axis, order, kind)`
- **Current**: `axis` (axis), `order` (order), `kind` (kind)
- **Proposed**: `axis` (axis), `order` (order), `kind` (kind)
- **Rationale**: Already clear

#### `unique(b)` → `unique(axis)`
- **Current**: `b` (axis)
- **Proposed**: `axis` (axis)
- **Rationale**: Clear purpose

#### `group(b, c, d)` → `group(axis, func, data)`
- **Current**: `b` (axis), `c` (function), `d` (data)
- **Proposed**: `axis` (axis), `func` (grouping function), `data` (data)
- **Rationale**: Clear purpose

#### `range(stop, step)` → `range(stop, step)`
- **Current**: `stop` (stop), `step` (step)
- **Proposed**: `stop` (stop), `step` (step)
- **Rationale**: Already clear

### Bit and Binary Operations

#### `setbit(b)` → `setbit(pos)`
- **Current**: `b` (position)
- **Proposed**: `pos` (bit position)
- **Rationale**: Clear purpose

#### `clearbit(b)` → `clearbit(pos)`
- **Current**: `b` (position)
- **Proposed**: `pos` (bit position)
- **Rationale**: Clear purpose

#### `genkeys(params)` → `genkeys(params)`
- **Current**: `params` (parameters)
- **Proposed**: `params` (parameters)
- **Rationale**: Already clear

### Cryptographic Methods

#### `encode(alg, data)` → `encode(method, data)`
- **Current**: `alg` (algorithm), `data` (data)
- **Proposed**: `method` (encoding method), `data` (data)
- **Rationale**: `method` is more intuitive

#### `decode(alg, data)` → `decode(method, data)`
- **Current**: `alg` (algorithm), `data` (data)
- **Proposed**: `method` (decoding method), `data` (data)
- **Rationale**: Consistent with encode

#### `sign(alg, params)` → `sign(method, params)`
- **Current**: `alg` (algorithm), `params` (parameters)
- **Proposed**: `method` (signing method), `params` (parameters)
- **Rationale**: Consistent naming

#### `signadd(alg, params)` → `signadd(method, params)`
- **Current**: `alg` (algorithm), `params` (parameters)
- **Proposed**: `method` (signing method), `params` (parameters)
- **Rationale**: Consistent naming

#### `verify(alg, data, params)` → `verify(method, data, params)`
- **Current**: `alg` (algorithm), `data` (data), `params` (parameters)
- **Proposed**: `method` (verification method), `data` (data), `params` (parameters)
- **Rationale**: Consistent naming

#### `verifyrecover(alg, params)` → `verifyrecover(method, params)`
- **Current**: `alg` (algorithm), `params` (parameters)
- **Proposed**: `method` (verification method), `params` (parameters)
- **Rationale**: Consistent naming

#### `secret(key)` → `secret(key)`
- **Current**: `key` (key)
- **Proposed**: `key` (key)
- **Rationale**: Already clear

### File and System Methods

#### `setfile(o, p, d)` → `setfile(mode, path, data)`
- **Current**: `o` (mode), `p` (path), `d` (data)
- **Proposed**: `mode` (file mode), `path` (file path), `data` (data)
- **Rationale**: Clear purpose of each parameter

#### `exec(cb, set, get)` → `exec(callback, set, get)`
- **Current**: `cb` (callback), `set` (set), `get` (get)
- **Proposed**: `callback` (callback function), `set` (set function), `get` (get function)
- **Rationale**: Clear purpose

#### `getname(b)` → `getname(index)`
- **Current**: `b` (index)
- **Proposed**: `index` (name index)
- **Rationale**: Clear purpose

### Matrix Operations

#### `diagonal(b)` → `diagonal(offset)`
- **Current**: `b` (offset)
- **Proposed**: `offset` (diagonal offset)
- **Rationale**: Clear purpose

## Summary of Changes

### High Priority Changes (Most Confusing)
1. **Single letter parameters** → **Descriptive names**
   - `b`, `c`, `d`, `e`, `f`, `g` → `count`, `start`, `end`, `pattern`, etc.
2. **Abbreviated names** → **Clear names**
   - `sep` → `delim`
   - `cb` → `callback`
   - `alg` → `method`

### Medium Priority Changes
1. **Consistent naming patterns**
   - All string methods use `chars` for character parameters
   - All numeric methods use `precision` and `mode`
   - All functional methods use `func` for function parameters

### Low Priority Changes
1. **Minor improvements**
   - `blocksize` → `size`
   - `callback` → `func` (where appropriate)

## Implementation Plan

### Phase 1: Update $OBJ.grc
1. Update all parameter names in `lib/grapa/$OBJ.grc`
2. Maintain backward compatibility by keeping old parameter names as aliases
3. Test with basic functionality

### Phase 2: Update Documentation
1. Update all documentation to use new parameter names
2. Provide migration examples
3. Update API reference

### Phase 3: Deprecation
1. Add deprecation warnings for old parameter names
2. Remove old parameter names in future version

## Benefits

1. **Improved Readability**: Clear parameter names make code self-documenting
2. **Better IDE Support**: IDEs can provide better autocomplete and hints
3. **Reduced Errors**: Less confusion about parameter order and purpose
4. **Modern Standards**: Aligns with modern programming language conventions
5. **Consistency**: Uniform naming patterns across all methods

## Migration Strategy

1. **Backward Compatibility**: Keep old parameter names working
2. **Gradual Migration**: Encourage use of new names in documentation
3. **Deprecation Timeline**: Plan for removal in future major version
4. **Migration Tools**: Provide scripts to update existing code
