# $OBJ.grc Method Name Analysis

## Overview

This document analyzes the method names in `lib/grapa/$OBJ.grc` to evaluate their appropriateness, intuitiveness, and alignment with modern programming language conventions.

## Method Name Analysis by Category

### Type and Information Methods

#### ✅ **Well-Named Methods**
- **`type()`** - Perfect. Standard in most languages (Python, JavaScript, etc.)
- **`describe()`** - Good. Clear and descriptive
- **`raw()`** - Good. Common term for raw data representation
- **`bool()`** - Perfect. Standard type conversion name
- **`int()`** - Perfect. Standard type conversion name
- **`float()`** - Perfect. Standard type conversion name
- **`str()`** - Perfect. Standard type conversion name (Python-style)
- **`len()`** - Perfect. Standard length method (Python-style)
- **`echo()`** - Good. Clear output method

#### ⚠️ **Questionable Names**
- **`uraw()`** - **POOR**. Unclear what "u" means (unsigned?). Should be `raw_unsigned()` or `raw_unsigned()`
- **`uint()`** - **POOR**. Should be `int_unsigned()` or `to_unsigned_int()`
- **`uhex()`** - **POOR**. Should be `hex_unsigned()` or `to_unsigned_hex()`
- **`ubin()`** - **POOR**. Should be `bin_unsigned()` or `to_unsigned_bin()`
- **`string()`** - **REDUNDANT**. Same as `str()`. Should be removed
- **`const()`** - **POOR**. Unclear. Should be `get_constant()` or `is_constant()`
- **`setconst()`** - **POOR**. Should be `set_constant()`

#### ❌ **Poor Names**
- **`fix()`** - **VERY POOR**. Unclear meaning. Should be `to_fixed_point()` or `fixed_point()`
- **`setfloat()`** - **POOR**. Should be `set_float_precision()` or `float_precision()`
- **`setfix()`** - **VERY POOR**. Should be `set_fixed_point_precision()`
- **`list()`** - **AMBIGUOUS**. Could mean "convert to list" or "get list". Should be `to_list()`
- **`array()`** - **AMBIGUOUS**. Should be `to_array()`
- **`vector()`** - **AMBIGUOUS**. Should be `to_vector()`
- **`time()`** - **AMBIGUOUS**. Should be `to_time()` or `as_time()`
- **`bits()`** - **POOR**. Should be `to_bits()` or `get_bits()`
- **`bytes()`** - **POOR**. Should be `to_bytes()` or `get_bytes()`

### String Manipulation Methods

#### ✅ **Well-Named Methods**
- **`left()`** - Good. Clear and intuitive
- **`right()`** - Good. Clear and intuitive
- **`mid()`** - Good. Clear substring extraction
- **`trim()`** - Perfect. Standard string method
- **`ltrim()`** - Good. Clear left trim
- **`rtrim()`** - Good. Clear right trim
- **`upper()`** - Perfect. Standard string method
- **`lower()`** - Perfect. Standard string method
- **`casefold()`** - Good. Python-style case folding
- **`reverse()`** - Perfect. Clear and intuitive
- **`replace()`** - Perfect. Standard string method
- **`split()`** - Perfect. Standard string method
- **`join()`** - Perfect. Standard string method
- **`grep()`** - Good. Unix-style pattern matching
- **`match()`** - Good. Regex matching

#### ⚠️ **Questionable Names**
- **`midtrim()`** - **POOR**. Should be `mid_trim()` or `substring_trim()`
- **`lrot()`** - **POOR**. Should be `rotate_left()` or `left_rotate()`
- **`rrot()`** - **POOR**. Should be `rotate_right()` or `right_rotate()`
- **`lpad()`** - **POOR**. Should be `pad_left()` or `left_pad()`
- **`rpad()`** - **POOR**. Should be `pad_right()` or `right_pad()`
- **`findall()`** - **GOOD** but could be `find_all()` for consistency

### Functional Programming Methods

#### ✅ **Well-Named Methods**
- **`map()`** - Perfect. Standard functional programming method
- **`filter()`** - Perfect. Standard functional programming method
- **`reduce()`** - Perfect. Standard functional programming method
- **`sort()`** - Perfect. Standard sorting method
- **`unique()`** - Good. Clear deduplication method
- **`range()`** - Perfect. Standard range generation

#### ⚠️ **Questionable Names**
- **`argsort()`** - **POOR**. Should be `sort_indices()` or `sorted_indices()`
- **`group()`** - **AMBIGUOUS**. Should be `group_by()` or `group_elements()`

### Bit and Binary Operations

#### ❌ **Poor Names**
- **`setbit()`** - **POOR**. Should be `set_bit()` or `bit_set()`
- **`clearbit()`** - **POOR**. Should be `clear_bit()` or `bit_clear()`
- **`genbits()`** - **POOR**. Should be `generate_bits()` or `create_bits()`
- **`genkeys()`** - **POOR**. Should be `generate_keys()` or `create_keys()`

### Cryptographic Methods

#### ⚠️ **Questionable Names**
- **`encode()`** - **AMBIGUOUS**. Should be `encode_data()` or `crypto_encode()`
- **`decode()`** - **AMBIGUOUS**. Should be `decode_data()` or `crypto_decode()`
- **`sign()`** - **GOOD** but could be `sign_data()` for clarity
- **`signadd()`** - **POOR**. Should be `add_signature()` or `signature_add()`
- **`verify()`** - **GOOD** but could be `verify_signature()` for clarity
- **`verifyrecover()`** - **POOR**. Should be `verify_and_recover()` or `recover_from_signature()`
- **`secret()`** - **POOR**. Should be `get_secret()` or `create_secret()`

### File and System Methods

#### ⚠️ **Questionable Names**
- **`setfile()`** - **POOR**. Should be `write_file()` or `save_to_file()`
- **`shell()`** - **GOOD** but could be `execute_shell()` for clarity
- **`exec()`** - **GOOD** but could be `execute()` for clarity
- **`post()`** - **POOR**. Should be `http_post()` or `post_request()`
- **`getname()`** - **POOR**. Should be `get_name()` or `extract_name()`

### Data Format Methods

#### ✅ **Well-Named Methods**
- **`json()`** - Perfect. Clear format conversion
- **`xml()`** - Perfect. Clear format conversion
- **`html()`** - Perfect. Clear format conversion

### Matrix Operations

#### ✅ **Well-Named Methods**
- **`identity()`** - Perfect. Standard matrix operation
- **`diagonal()`** - Perfect. Standard matrix operation

### Error Handling

#### ✅ **Well-Named Methods**
- **`iferr()`** - **GOOD** but could be `if_error()` or `on_error()` for clarity
- **`isint()`** - **GOOD** but could be `is_int()` for consistency

## Recommendations for Method Name Improvements

### High Priority Renames (Most Confusing)

1. **`fix()`** → **`to_fixed_point()`**
   - Current name is completely unclear
   - No modern language uses "fix" for fixed-point

2. **`setfloat()`** → **`set_float_precision()`**
   - Current name suggests setting a float value, not precision

3. **`setfix()`** → **`set_fixed_point_precision()`**
   - Same issue as above

4. **`uraw()`, `uint()`, `uhex()`, `ubin()`** → **`raw_unsigned()`, `int_unsigned()`, etc.**
   - "u" prefix is unclear and not standard

5. **`argsort()`** → **`sort_indices()`**
   - More descriptive of what it actually does

### Medium Priority Renames

1. **`midtrim()`** → **`substring_trim()`**
2. **`lrot()`, `rrot()`** → **`rotate_left()`, `rotate_right()`**
3. **`lpad()`, `rpad()`** → **`pad_left()`, `pad_right()`**
4. **`setbit()`, `clearbit()`** → **`set_bit()`, `clear_bit()`**
5. **`genbits()`, `genkeys()`** → **`generate_bits()`, `generate_keys()`**

### Low Priority Renames (Minor Improvements)

1. **`string()`** → **Remove** (redundant with `str()`)
2. **`const()`** → **`get_constant()`**
3. **`setconst()`** → **`set_constant()`**
4. **`list()`, `array()`, `vector()`** → **`to_list()`, `to_array()`, `to_vector()`**
5. **`time()`, `bits()`, `bytes()`** → **`to_time()`, `to_bits()`, `to_bytes()`**

## Modern Programming Language Comparison

### Python-Style Names (Recommended)
- `type()`, `str()`, `int()`, `float()`, `bool()` ✅
- `len()`, `upper()`, `lower()`, `split()`, `join()` ✅
- `map()`, `filter()`, `reduce()`, `sort()` ✅

### JavaScript-Style Names (Alternative)
- `toString()`, `toInt()`, `toFloat()`, `toBoolean()` (instead of `str()`, `int()`, etc.)
- `length` (instead of `len()`)
- `toUpperCase()`, `toLowerCase()` (instead of `upper()`, `lower()`)

### Rust-Style Names (Alternative)
- `to_string()`, `to_int()`, `to_float()` (snake_case)
- `len()`, `to_uppercase()`, `to_lowercase()`

## Implementation Considerations

### Backward Compatibility
- **Option 1**: Add aliases for new names while keeping old names
- **Option 2**: Deprecate old names with warnings
- **Option 3**: Breaking change (not recommended)

### Documentation Strategy
- Document both old and new names during transition
- Provide migration examples
- Update all examples to use new names

### Testing Strategy
- Test both old and new method names
- Ensure identical functionality
- Performance comparison between old and new implementations

## Conclusion

**Overall Assessment**: About 60% of method names are well-named and intuitive, 25% need minor improvements, and 15% need significant renaming.

**Priority**: Focus on the high-priority renames first, especially the completely unclear names like `fix()`, `setfloat()`, and the "u" prefixed methods.

**Recommendation**: Adopt Python-style naming conventions as they are most intuitive for modern developers and align well with Grapa's existing good method names.
