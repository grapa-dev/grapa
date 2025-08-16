# $OBJ.grc Documentation Analysis

## Overview

This document analyzes the coverage of documentation for methods defined in `lib/grapa/$OBJ.grc`. The `$OBJ` class contains methods that are available on all Grapa objects and provides the foundation for type conversion, string manipulation, functional programming, and other core operations.

## Methods in $OBJ.grc

### Type and Information Methods
- `type()` - Get type of object
- `describe()` - Get detailed description of object
- `raw()` - Get raw representation
- `uraw()` - Get unsigned raw representation
- `bool()` - Convert to boolean
- `int()` - Convert to integer
- `uint()` - Convert to unsigned integer
- `float()` - Convert to float
- `fix()` - Convert to fixed-point
- `setfloat()` - Set float precision
- `setfix()` - Set fixed-point precision
- `str()` - Convert to string
- `list()` - Convert to list
- `array()` - Convert to array
- `vector()` - Convert to vector
- `base()` - Convert to different base
- `hex()` - Convert to hexadecimal
- `uhex()` - Convert to unsigned hexadecimal
- `bin()` - Convert to binary
- `ubin()` - Convert to unsigned binary
- `time()` - Convert to time
- `bits()` - Get bits
- `bytes()` - Get bytes
- `len()` - Get length
- `isint()` - Check if integer
- `iferr()` - Error handling
- `echo()` - Output to console
- `string()` - String conversion
- `const()` - Get constant value
- `setconst()` - Set constant value

### String Manipulation Methods
- `left()` - Get left portion
- `right()` - Get right portion
- `mid()` - Get middle portion
- `midtrim()` - Get middle portion with trimming
- `rtrim()` - Trim right side
- `ltrim()` - Trim left side
- `trim()` - Trim both sides
- `lrot()` - Left rotation
- `rrot()` - Right rotation
- `lpad()` - Left padding
- `rpad()` - Right padding
- `reverse()` - Reverse string
- `replace()` - Replace text
- `interpolate()` - String interpolation
- `grep()` - Pattern matching
- `match()` - Regex matching
- `split()` - Split string
- `join()` - Join array
- `upper()` - Convert to uppercase
- `lower()` - Convert to lowercase
- `casefold()` - Case folding
- `findall()` - Find all matches

### Functional Programming Methods
- `map()` - Apply function to each element
- `filter()` - Filter elements
- `reduce()` - Reduce to single value
- `sort()` - Sort elements
- `argsort()` - Sort indices
- `unique()` - Get unique elements
- `group()` - Group elements
- `range()` - Generate range

### Bit and Binary Operations
- `setbit()` - Set bit
- `clearbit()` - Clear bit
- `genbits()` - Generate bits
- `genkeys()` - Generate keys

### Cryptographic Methods
- `encode()` - Encode data
- `decode()` - Decode data
- `sign()` - Sign data
- `signadd()` - Add signature
- `verify()` - Verify signature
- `verifyrecover()` - Verify and recover
- `secret()` - Handle secrets

### File and System Methods
- `setfile()` - Set file content
- `shell()` - Execute shell command
- `exec()` - Execute with callbacks
- `post()` - HTTP POST
- `getname()` - Get name

### Data Format Methods
- `json()` - Convert to JSON
- `xml()` - Convert to XML
- `html()` - Convert to HTML

### Matrix Operations
- `identity()` - Create identity matrix
- `diagonal()` - Get diagonal

## Documentation Coverage Analysis

### ✅ Well Documented Methods

#### Functional Programming (Good Coverage)
- **`.map()`** - Documented in multiple places:
  - `docs-src/docs/api_reference.md`
  - `docs-src/docs/syntax/basic_syntax.md`
  - `docs-src/docs/operators/loop.md`
  - `docs-src/docs/obj/transform.md`
  - `docs-src/docs/obj/iterate.md`

- **`.filter()`** - Documented in same locations as `.map()`

- **`.reduce()`** - Documented in same locations as `.map()`

#### String Methods (Partial Coverage)
- **`.len()`** - Documented in `docs-src/docs/api_reference.md`
- **`.upper()`** - Documented in `docs-src/docs/api_reference.md`
- **`.lower()`** - Documented in `docs-src/docs/api_reference.md`
- **`.casefold()`** - Documented in `docs-src/docs/api_reference.md`
- **`.trim()`** - Documented in `docs-src/docs/api_reference.md`
- **`.ltrim()`** - Documented in `docs-src/docs/api_reference.md`
- **`.rtrim()`** - Documented in `docs-src/docs/api_reference.md`
- **`.split()`** - Documented in `docs-src/docs/api_reference.md`
- **`.join()`** - Documented in `docs-src/docs/api_reference.md`
- **`.replace()`** - Documented in `docs-src/docs/api_reference.md`
- **`.grep()`** - Documented in `docs-src/docs/api_reference.md`

#### Type Conversion (Partial Coverage)
- **`.str()`** - Documented in `docs-src/docs/api_reference.md`
- **`.int()`** - Documented in `docs-src/docs/api_reference.md`
- **`.float()`** - Documented in `docs-src/docs/api_reference.md`
- **`.bool()`** - Documented in `docs-src/docs/api_reference.md`

#### Output Methods
- **`.echo()`** - Documented in `docs-src/docs/api_reference.md`

### ❌ Missing Documentation

#### Type and Information Methods (Mostly Missing)
- `type()` - **NOT DOCUMENTED**
- `describe()` - **NOT DOCUMENTED**
- `raw()` - **NOT DOCUMENTED**
- `uraw()` - **NOT DOCUMENTED**
- `uint()` - **NOT DOCUMENTED**
- `fix()` - **NOT DOCUMENTED**
- `setfloat()` - **NOT DOCUMENTED**
- `setfix()` - **NOT DOCUMENTED**
- `list()` - **NOT DOCUMENTED**
- `array()` - **NOT DOCUMENTED**
- `vector()` - **NOT DOCUMENTED**
- `base()` - **NOT DOCUMENTED**
- `hex()` - **NOT DOCUMENTED**
- `uhex()` - **NOT DOCUMENTED**
- `bin()` - **NOT DOCUMENTED**
- `ubin()` - **NOT DOCUMENTED**
- `time()` - **NOT DOCUMENTED**
- `bits()` - **NOT DOCUMENTED**
- `bytes()` - **NOT DOCUMENTED**
- `isint()` - **NOT DOCUMENTED**
- `iferr()` - **NOT DOCUMENTED**
- `string()` - **NOT DOCUMENTED**
- `const()` - **NOT DOCUMENTED**
- `setconst()` - **NOT DOCUMENTED**

#### String Manipulation Methods (Mostly Missing)
- `left()` - **NOT DOCUMENTED**
- `right()` - **NOT DOCUMENTED**
- `mid()` - **NOT DOCUMENTED**
- `midtrim()` - **NOT DOCUMENTED**
- `lrot()` - **NOT DOCUMENTED**
- `rrot()` - **NOT DOCUMENTED**
- `lpad()` - **NOT DOCUMENTED**
- `rpad()` - **NOT DOCUMENTED**
- `reverse()` - **NOT DOCUMENTED**
- `interpolate()` - **NOT DOCUMENTED** (though string interpolation is documented elsewhere)
- `match()` - **NOT DOCUMENTED**
- `findall()` - **NOT DOCUMENTED**

#### Functional Programming Methods (Mostly Missing)
- `sort()` - **NOT DOCUMENTED**
- `argsort()` - **NOT DOCUMENTED**
- `unique()` - **NOT DOCUMENTED**
- `group()` - **NOT DOCUMENTED**
- `range()` - **NOT DOCUMENTED** (though `.range()` is documented in basic syntax)

#### Bit and Binary Operations (All Missing)
- `setbit()` - **NOT DOCUMENTED**
- `clearbit()` - **NOT DOCUMENTED**
- `genbits()` - **NOT DOCUMENTED**
- `genkeys()` - **NOT DOCUMENTED**

#### Cryptographic Methods (All Missing)
- `encode()` - **NOT DOCUMENTED**
- `decode()` - **NOT DOCUMENTED**
- `sign()` - **NOT DOCUMENTED**
- `signadd()` - **NOT DOCUMENTED**
- `verify()` - **NOT DOCUMENTED**
- `verifyrecover()` - **NOT DOCUMENTED**
- `secret()` - **NOT DOCUMENTED**

#### File and System Methods (All Missing)
- `setfile()` - **NOT DOCUMENTED**
- `shell()` - **NOT DOCUMENTED**
- `exec()` - **NOT DOCUMENTED**
- `post()` - **NOT DOCUMENTED**
- `getname()` - **NOT DOCUMENTED**

#### Data Format Methods (All Missing)
- `json()` - **NOT DOCUMENTED**
- `xml()` - **NOT DOCUMENTED**
- `html()` - **NOT DOCUMENTED**

#### Matrix Operations (All Missing)
- `identity()` - **NOT DOCUMENTED**
- `diagonal()` - **NOT DOCUMENTED**

## Summary

### Documentation Coverage Statistics
- **Total Methods in $OBJ.grc**: ~70 methods
- **Well Documented**: ~15 methods (21%)
- **Partially Documented**: ~5 methods (7%)
- **Missing Documentation**: ~50 methods (72%)

### Priority Categories for Documentation

#### High Priority (Core Functionality)
1. **Type Information Methods** - `type()`, `describe()`, `raw()`
2. **String Manipulation** - `left()`, `right()`, `mid()`, `reverse()`, `match()`
3. **Functional Programming** - `sort()`, `unique()`, `group()`, `range()`
4. **Data Format** - `json()`, `xml()`, `html()`

#### Medium Priority (Specialized Features)
1. **Bit Operations** - `setbit()`, `clearbit()`, `genbits()`
2. **Matrix Operations** - `identity()`, `diagonal()`
3. **System Methods** - `shell()`, `exec()`, `post()`

#### Low Priority (Advanced Features)
1. **Cryptographic Methods** - `sign()`, `verify()`, `encode()`, `decode()`
2. **Advanced Type Conversion** - `fix()`, `setfloat()`, `setfix()`

## Recommendations

1. **Create comprehensive API documentation** for all missing methods
2. **Add examples** for each method showing common use cases
3. **Document parameter types and return values** for all methods
4. **Create method-specific documentation pages** for complex methods
5. **Add cross-references** between related methods
6. **Include error handling examples** for methods that can fail

## Files to Update

### Primary Documentation Files
- `docs-src/docs/api_reference.md` - Add missing method documentation
- `docs-src/docs/type/str.md` - Add string manipulation methods
- `docs-src/docs/type/list.md` - Add list/array conversion methods
- `docs-src/docs/type/array.md` - Add array manipulation methods

### New Documentation Files Needed
- `docs-src/docs/obj/type_conversion.md` - Type conversion methods
- `docs-src/docs/obj/string_manipulation.md` - String manipulation methods
- `docs-src/docs/obj/functional_programming.md` - Functional programming methods
- `docs-src/docs/obj/cryptography.md` - Cryptographic methods
- `docs-src/docs/obj/system_operations.md` - System and file operations

