---
tags:
  - user
  - highlevel
  - api
  - methods
---

# $OBJ Methods Reference

> **Universal Object Methods:** All Grapa objects support these methods through the `$OBJ` class. These methods provide type conversion, manipulation, and utility functions that work on any data type.

## Grapa Data Type Terminology

Grapa uses C-style terminology for data structures:

- **`$ARRAY`** = `[]` syntax (positional, no names, indexed by position)
- **`$LIST`** = `{}` syntax (associative, with names/keys)
- **`$XML`** = `<tag>content</tag>` syntax (structured markup)

**Note:** This differs from some other languages where `[]` is called a "list" and `{}` is called a "dictionary" or "object". Grapa follows traditional C terminology where arrays are positional and lists are associative.

## Table of Contents
- [Type and Information Methods](#type-and-information-methods)
- [Type Conversion Methods](#type-conversion-methods)
- [String Manipulation Methods](#string-manipulation-methods)
- [Array and List Methods](#array-and-list-methods)
- [Functional Programming Methods](#functional-programming-methods)
- [Search and Pattern Methods](#search-and-pattern-methods)
- [Bit Manipulation Methods](#bit-manipulation-methods)
- [Cryptography Methods](#cryptography-methods)
- [File System Methods](#file-system-methods)
- [Utility Methods](#utility-methods)

---

## Type and Information Methods

### `.type()`
Returns the type of the object as a string.

```grapa
42.type();           /* Returns: "$INT" */
"hello".type();      /* Returns: "$STR" */
[1,2,3].type();      /* Returns: "$ARRAY" */
```

### `.describe()`
Returns detailed information about the object (currently returns `{"error":-1}`).

```grapa
obj.describe();      /* Returns detailed object information */
```

### `.raw()`
Returns the raw bytes representation of the object. For numbers, this shows the **internal binary representation** as hex.

```grapa
"Hello".raw();       /* Returns: 0x48656C6C6F */
42.raw();           /* Returns: 0x2A */
255.raw();          /* Returns: 0x0FF */
4.5.raw();          /* Returns: 0x00281000A09 (internal binary structure) */
```

**Note:** For `$FLOAT` types, `.raw()` shows the internal binary representation (sign, exponent, mantissa) as hex, not the decimal value.

### `.uraw()`
Returns the unsigned raw bytes representation of the object. This method treats negative numbers as unsigned integers using two's complement representation, which is essential for cryptographic applications and binary data processing.

```grapa
(-1).uraw();        /* Returns: 0xD6 (unsigned hex with 0x prefix) */
(255).uraw();       /* Returns: 0x0FF (with leading zero for consistency) */
(-100).uraw();      /* Returns: 0x9C (unsigned representation) */
```

**Key Features:**
- **Two's complement conversion**: Negative numbers are converted to their unsigned equivalent
- **Consistent formatting**: Always includes `0x` prefix and leading zeros for consistency
- **Cryptographic applications**: Essential for handling large numbers that might be interpreted as negative
- **Binary data processing**: Useful for working with raw bytes and network protocols

**Use Cases:**
- Binary file format parsing
- Network protocol data handling
- Low-level system programming
- Debugging and inspection (for cryptographic output, prefer `.uhex()`)

**Note: For cryptographic key output, prefer `.uhex()` over `.uraw()`**
```grapa
// .uraw() includes 0x prefix and leading zeros
key.uraw();         /* Returns: "0x0FFFF" */

// .uhex() provides clean hex string for cryptographic use
key.uhex();         /* Returns: "FFFF" */
```

### `.bool()`
Converts the object to a boolean value.

```grapa
"hello".bool();     /* Returns: true */
"".bool();          /* Returns: false */
0.bool();           /* Returns: false */
42.bool();          /* Returns: true */
```

### `.len()`
Returns the length of the object.

```grapa
"hello".len();      /* Returns: 5 */
[1,2,3].len();      /* Returns: 3 */
```

**Behavior by Type:**
- **Strings**: Returns Unicode character count (grapheme clusters)
- **Arrays/Lists**: Returns number of elements
- **Numbers**: Returns the length of the string representation (e.g., "42" has length 2)
- **Objects**: Returns number of properties
- **Other types**: Returns element count or 0

**Unicode Support:**
For strings, `.len()` counts Unicode characters (grapheme clusters), not bytes:
```grapa
"hello".len();      /* Returns: 5 (5 characters) */
"héllo".len();      /* Returns: 5 (5 characters, not 6 bytes) */
"é".len();          /* Returns: 1 (1 character, not 2 bytes) */
"🚀".len();         /* Returns: 1 (1 emoji, not 4 bytes) */
"👨‍👩‍👧‍👦".len();   /* Returns: 4 (4 grapheme clusters) */
```

---

## Type Conversion Methods

### `.int()`
Converts the object to an integer.

```grapa
"42".int();         /* Returns: 42 */
3.14.int();         /* Returns: 3 */
```

### `.uint()`
Converts the object to an unsigned integer. This method is crucial for cryptographic applications where you need to handle large numbers as unsigned values without sign issues.

```grapa
(-1).uint();        /* Returns: 255 (8-bit unsigned equivalent) */
(-100).uint();      /* Returns: 156 (8-bit unsigned equivalent) */
(-1000).uint();     /* Returns: 64536 (16-bit unsigned equivalent) */
(-2147483648).uint(); /* Returns: 2147483648 (32-bit unsigned equivalent) */
(255).uint();       /* Returns: 255 (no change for positive numbers) */
```

**Key Features:**
- **Two's complement conversion**: Negative numbers are converted to their unsigned equivalent
- **Bit width preservation**: Maintains the appropriate bit width for the conversion
- **No effect on positives**: Positive numbers remain unchanged
- **Cryptographic safety**: Prevents sign issues when working with large random numbers

**Cryptographic Applications:**
- **Large random number generation**: Handle numbers with leading bits set without sign issues
- **Key generation**: Ensure cryptographic keys are treated as unsigned values
- **Hash function inputs**: Prevent negative number interpretation in hash calculations
- **Bit manipulation**: Work with bit patterns that might be interpreted as negative

**Example Workflow:**
```grapa
// Generate a large random number (might have leading bit set)
large_random = 0x80000000 + some_random_bits;

// Convert to unsigned for cryptographic operations
unsigned_value = large_random.uint();

// Compare with other unsigned values
is_equal = (unsigned_value == 0x80000000);
```

### `.float(precision, mode)`
Converts the object to a float with specified precision and mode.

```grapa
"3.14".float();     /* Returns: 3.14 */
"3.14".float(2, 0); /* Returns: 3.0 (2 decimal places) */
"3.14".float(4, 0); /* Returns: 3.1400 (4 decimal places) */
```

**Parameters:**
- `precision` (default: 0) - Number of decimal places
- `mode` (default: 0) - Formatting mode

### `.fix(precision, mode)`
Converts the object to a fixed-point number.

```grapa
"3.14".fix(2, 0);   /* Returns: 3.14 */
"3.14".fix(4, 0);   /* Returns: 3.1400 */
```

**Parameters:**
- `precision` (default: 0) - Number of decimal places
- `mode` (default: 0) - Formatting mode

### `.setfloat(precision, mode)`
Sets the object to a float value with specified precision and mode.

```grapa
obj.setfloat(2, 0); /* Sets object to float with 2 decimal places */
```

**Parameters:**
- `precision` (default: 0) - Number of decimal places
- `mode` (default: 0) - Formatting mode

### `.setfix(precision, mode)`
Sets the object to a fixed-point value.

```grapa
obj.setfix(2, 0);   /* Sets object to fixed-point with 2 decimal places */
```

**Parameters:**
- `precision` (default: 0) - Number of decimal places
- `mode` (default: 0) - Formatting mode

### `.str(format)`
Converts the object to a string with optional formatting.

```grapa
42.str();           /* Returns: "42" */
3.14.str();         /* Returns: "3.14" */
```

**Parameters:**
- `format` (default: null) - Format string

### `.list()`
Converts the object to a list.

```grapa
[1,2,3].list();     /* Returns: @<list,{1,2,3}> */
```

### `.array()`
Converts the object to an array.

```grapa
obj.array();        /* Returns array representation */
```

### `.vector(dim)`
Converts the object to a vector with specified dimensions.

```grapa
[1,2,3,4,5].vector(2); /* Returns: #[1,2,3,4,5]# */
[1,2,3,4,5].vector(3); /* Returns: #[1,2,3,4,5]# */
```

**Parameters:**
- `dim` (default: 0) - Vector dimensions

### `.base(radix)`
Converts the object to a different base representation.

```grapa
42.base(2);         /* Returns: "101010" */
42.base(8);         /* Returns: "52" */
42.base(16);        /* Returns: "2A" */
```

**Parameters:**
- `radix` (default: 10) - Base for conversion

### `.hex()`
Converts the object to hexadecimal representation. For numbers, this converts the **value** to hex.

```grapa
42.hex();           /* Returns: "2A" */
255.hex();          /* Returns: "FF" */
4.5.hex();          /* Returns: "4.8" (decimal value as hex) */
```

**Note:** For `$FLOAT` types, `.hex()` converts the decimal value to hex representation, not the internal binary structure.

### `.uhex()`
Converts the object to unsigned hexadecimal representation. This method treats negative numbers as unsigned integers and returns hex without the `0x` prefix.

```grapa
(-1).uhex();        /* Returns: "D6" (unsigned hex without 0x prefix) */
(255).uhex();       /* Returns: "FF" (no change for positive numbers) */
(-100).uhex();      /* Returns: "9C" (unsigned representation) */
(-1000).uhex();     /* Returns: "FC18" (16-bit unsigned hex) */
```

**Key Features:**
- **Two's complement conversion**: Negative numbers are converted to their unsigned equivalent
- **No 0x prefix**: Returns clean hex string without prefix (unlike `.uraw()`)
- **Consistent with `.hex()`**: Same format as regular `.hex()` but for unsigned values
- **Cryptographic applications**: Essential for hex representation of unsigned values

**Comparison with other hex methods:**
```grapa
(-1).hex();         /* Returns: "-01" (signed hex with minus sign) */
(-1).uhex();        /* Returns: "D6" (unsigned hex without sign) */
(-1).uraw();        /* Returns: "0xD6" (unsigned hex with 0x prefix) */
```

**Use Cases:**
- **Cryptographic key representation**: Display keys in standard hex format (preferred over `.uraw()`)
- **Hash value output**: Show hash results as unsigned hex strings
- **Binary data inspection**: View raw data as unsigned hex
- **Protocol debugging**: Examine network protocol data in hex format

**Important: For cryptographic output, use `.uhex()` instead of `.uraw()`**
```grapa
// For cryptographic applications, prefer .uhex() over .uraw()
key = 0xFFFF;
key.uhex();         /* Returns: "FFFF" (clean hex string) */
key.uraw();         /* Returns: "0x0FFFF" (with 0x prefix and leading zero) */

// .uhex() provides standard cryptographic hex format
// .uraw() includes formatting that may not be desired
```

### `.bin()`
Converts the object to binary representation.

```grapa
42.bin();           /* Returns: "101010" */
```

### `.ubin()`
Converts the object to unsigned binary representation. This method treats negative numbers as unsigned integers and returns binary without sign indicators.

```grapa
(-1).ubin();        /* Returns: "11010110" (unsigned binary) */
(255).ubin();       /* Returns: "11111111" (no change for positive numbers) */
(-100).ubin();      /* Returns: "10011100" (unsigned representation) */
(-1000).ubin();     /* Returns: "11111100100011000" (16-bit unsigned binary) */
```

**Key Features:**
- **Two's complement conversion**: Negative numbers are converted to their unsigned equivalent
- **No sign indicators**: Returns clean binary string without minus signs
- **Consistent with `.bin()`**: Same format as regular `.bin()` but for unsigned values
- **Bit manipulation**: Essential for cryptographic bit operations

**Comparison with other binary methods:**
```grapa
(-1).bin();         /* Returns: "-1" (signed binary with minus sign) */
(-1).ubin();        /* Returns: "11010110" (unsigned binary without sign) */
```

**Use Cases:**
- **Cryptographic bit operations**: Work with binary representations for encryption/decryption
- **Hash function implementation**: Manipulate binary data for hash calculations
- **Binary protocol analysis**: Examine binary protocol data
- **Bit-level debugging**: Debug cryptographic algorithms at the bit level
- **Random number generation**: Work with binary representations of random values

**Example: Bit manipulation for cryptography**
```grapa
// Generate a random number that might be negative
random_value = generate_random_number();

// Convert to unsigned binary for bit operations
binary_rep = random_value.ubin();

// Perform bit-level operations
bit_count = binary_rep.grep("1", "c")[0].int();
```

### `.time()`
Converts the object to a time representation.

```grapa
obj.time();         /* Returns time representation */
```

### `.bits()`
Returns the bit representation of the object.

```grapa
obj.bits();         /* Returns bit representation */
```

### `.bytes()`
Returns the byte count of the object's internal storage.

```grapa
"hello".bytes();    /* Returns: 5 (byte count) */
42.bytes();         /* Returns: 0 (byte storage size) */
1.2.bytes();        /* Returns: 29 (byte storage size) */
```

**Behavior by Type:**
- **Strings**: Returns byte count (same as `.len()` for ASCII strings)
- **Numbers**: Returns the byte size of internal storage
- **Arrays/Lists**: Returns `{"error":-1}` (not supported)
- **Objects**: Returns `{"error":-1}` (not supported)

**Note**: For numbers, this returns the size of the internal binary representation, not the string length.

### `.raw()`
Returns the raw binary representation of the object as a `$RAW` type.

```grapa
42.raw();           /* Returns: 0x2A (hex representation) */
1.2.raw();          /* Returns: 0x00281000A09... (internal bytes) */
"hello".raw();      /* Returns: 0x68656C6C6F (hex of "hello") */
```

**Behavior by Type:**
- **Numbers**: Returns internal binary representation as hex
- **Strings**: Returns string bytes as hex
- **Other types**: Returns internal representation as hex

### **Comparison: `.len()` vs `.bytes()` vs `.raw()`**

| Method | Purpose | String | Number | Array/List |
|--------|---------|--------|--------|------------|
| `.len()` | Logical length | Unicode character count | String length | Element count |
| `.bytes()` | Storage size | Byte count | Internal bytes | Not supported |
| `.raw()` | Raw data | Hex bytes | Hex bytes | Hex bytes |

**Examples:**
```grapa
value = 42;
value.len();        /* Returns: 2 (length of "42") */
value.bytes();      /* Returns: 0 (internal storage size) */
value.raw();        /* Returns: 0x2A (hex representation) */

text = "hello";
text.len();         /* Returns: 5 (character count) */
text.bytes();       /* Returns: 5 (byte count) */
text.raw();         /* Returns: 0x68656C6C6F (hex) */

unicode_text = "héllo 🚀";
unicode_text.len(); /* Returns: 7 (Unicode character count) */
unicode_text.bytes(); /* Returns: 11 (byte count) */
unicode_text.raw();   /* Returns: 0x68C3A96C6C6F20F09F9A80 (hex) */

float_val = 1.2;
float_val.len();    /* Returns: 3 (length of "1.2") */
float_val.bytes();  /* Returns: 29 (internal storage size) */
float_val.raw();    /* Returns: 0x00281000A09... (hex) */
```

**Key Relationship**: For some types, `.raw().len() == .bytes()` because `.raw()` returns the internal bytes and `.len()` on those bytes gives the byte count.

### `.setconst(value)`
Sets the object to a constant value.

```grapa
obj.setconst(100);  /* Sets object to constant value 100 */
obj.setconst("new value"); /* Sets object to constant string */
```

**Parameters:**
- `value` - The value to set as constant

---

## String Manipulation Methods

### `.left(count)`
Returns the leftmost characters of a string. **Unicode-aware**: Counts Unicode characters (grapheme clusters), not bytes.

```grapa
"Hello, World!".left(5);   /* Returns: "Hello" */
"héllo".left(3);          /* Returns: "hél" (3 Unicode characters) */
"🚀héllo".left(3);        /* Returns: "🚀hé" (3 Unicode characters) */
"👨‍👩‍👧‍👦".left(2);       /* Returns: "👨‍👩‍" (2 Unicode characters) */
```

**Parameters:**
- `count` (default: 0) - Number of Unicode characters to return (negative counts from right)

**Unicode Support:** This function now properly handles Unicode characters, emoji, and complex grapheme clusters by counting characters rather than bytes.

### `.right(count)`
Returns the rightmost characters of a string. **Unicode-aware**: Counts Unicode characters (grapheme clusters), not bytes.

```grapa
"Hello, World!".right(5);  /* Returns: "orld!" */
"héllo".right(3);         /* Returns: "llo" (3 Unicode characters) */
"🚀héllo".right(3);       /* Returns: "llo" (3 Unicode characters) */
"👨‍👩‍👧‍👦".right(2);      /* Returns: "👧‍👦" (2 Unicode characters) */
```

**Parameters:**
- `count` (default: 0) - Number of Unicode characters to return (negative counts from left)

**Unicode Support:** This function now properly handles Unicode characters, emoji, and complex grapheme clusters by counting characters rather than bytes.

### `.mid(start, count)`
Returns a substring starting at the specified position. **Unicode-aware**: Counts Unicode characters (grapheme clusters), not bytes.

```grapa
"Hello, World!".mid(2, 5); /* Returns: "llo, " */
"héllo".mid(1, 3);        /* Returns: "éll" (3 Unicode characters from position 1) */
"🚀héllo".mid(1, 3);      /* Returns: "hél" (3 Unicode characters from position 1) */
"👨‍👩‍👧‍👦".mid(1, 2);     /* Returns: "👩‍👧‍" (2 Unicode characters from position 1) */
```

**Parameters:**
- `start` (default: 0) - Starting position (Unicode character position)
- `count` (default: 0) - Number of Unicode characters to return

**Unicode Support:** This function now properly handles Unicode characters, emoji, and complex grapheme clusters by counting characters rather than bytes.

### `.midtrim(items, offset, size)`
Trims and extracts a portion of the object based on items, offset, and size.

```grapa
"Hello, World!".midtrim([1,2,3], 2, 3); /* Returns: {} */
"Hello, World!".midtrim([1,2,3], 0, 5); /* Returns: {} */
```

**Parameters:**
- `items` (default: null) - Items to use for trimming
- `offset` (default: 0) - Offset position
- `size` (default: 0) - Size of the portion to extract

### `.rtrim(chars)`
Removes characters from the right end of a string.

```grapa
"Hello, World!".rtrim();      /* Returns: "Hello, World!" */
"Hello, World!".rtrim("!");   /* Returns: "Hello, World" */
```

**Parameters:**
- `chars` (default: null) - Characters to remove (default: whitespace)

### `.ltrim(chars)`
Removes characters from the left end of a string.

```grapa
"Hello, World!".ltrim();      /* Returns: "Hello, World!" */
"Hello, World!".ltrim("H");   /* Returns: "ello, World!" */
```

**Parameters:**
- `chars` (default: null) - Characters to remove (default: whitespace)

### `.trim(chars)`
Removes characters from both ends of a string.

```grapa
"Hello, World!".trim();       /* Returns: "Hello, World!" */
"Hello, World!".trim("H!");   /* Returns: "ello, World" */
```

**Parameters:**
- `chars` (default: null) - Characters to remove (default: whitespace)

### `.lrot(count)`
Rotates elements left by the specified number of positions. Works with `$ARRAY`, `$LIST`, `$TUPLE`, and `$XML` data types.

```grapa
/* ✅ CORRECT: Use with $ARRAY and $LIST */
[1,2,3,4,5].lrot(2);     /* Returns: [3,4,5,1,2] - positional array */
{a:1,b:2,c:3}.lrot(2);   /* Returns: {"c":3,"a":1,"b":2} - associative list */

/* ❌ INCORRECT: Don't work with strings */
"Hello".lrot(2);         /* Returns: {"error":-1} */
```

**Parameters:**
- `count` - Number of positions to rotate (default: 1)

**Data Types:** `$ARRAY`, `$LIST`, `$TUPLE`, `$XML`

### `.rrot(count)`
Rotates elements right by the specified number of positions. Works with `$ARRAY`, `$LIST`, `$TUPLE`, and `$XML` data types.

```grapa
/* ✅ CORRECT: Use with $ARRAY and $LIST */
[1,2,3,4,5].rrot(2);     /* Returns: [4,5,1,2,3] - positional array */
{a:1,b:2,c:3}.rrot(2);   /* Returns: {"b":2,"c":3,"a":1} - associative list */

/* ❌ INCORRECT: Don't work with strings */
"Hello".rrot(2);         /* Returns: {"error":-1} */
```

**Parameters:**
- `count` - Number of positions to rotate (default: 1)

**Data Types:** `$ARRAY`, `$LIST`, `$TUPLE`, `$XML`

### `.lpad(width, char)`
Pads the string on the left to the specified width. **Unicode-aware**: Counts Unicode characters (grapheme clusters), not bytes.

```grapa
"Hello".lpad(20, "*"); /* Returns: "***************Hello" */
"Hello".lpad(15, " "); /* Returns: "          Hello" */
"héllo".lpad(10, " "); /* Returns: "     héllo" (5 Unicode characters) */
"héllo".lpad(10, "é"); /* Returns: "éééééhéllo" (5 Unicode characters) */
"🚀héllo".lpad(10, "🚀"); /* Returns: "🚀🚀🚀🚀🚀héllo" (5 Unicode characters) */
```

**Parameters:**
- `width` - Target width in Unicode characters
- `char` - Character to use for padding (default: space)

**Unicode Support:** This function now properly handles Unicode characters, emoji, and complex grapheme clusters by counting characters rather than bytes.

### `.rpad(width, char)`
Pads the string on the right to the specified width. **Unicode-aware**: Counts Unicode characters (grapheme clusters), not bytes.

```grapa
"Hello".rpad(20, "*"); /* Returns: "Hello***************" */
"Hello".rpad(15, " "); /* Returns: "Hello          " */
"héllo".rpad(10, " "); /* Returns: "héllo     " (5 Unicode characters) */
"héllo".rpad(10, "é"); /* Returns: "hélloééééé" (5 Unicode characters) */
"🚀héllo".rpad(10, "🚀"); /* Returns: "héllo🚀🚀🚀🚀🚀" (5 Unicode characters) */
```

**Parameters:**
- `width` - Target width in Unicode characters
- `char` - Character to use for padding (default: space)

**Unicode Support:** This function now properly handles Unicode characters, emoji, and complex grapheme clusters by counting characters rather than bytes.

### `.reverse()`
Reverses the order of elements in an array or characters in a string. **Unicode-aware**: For strings, reverses Unicode characters (grapheme clusters), not bytes.

```grapa
[1,2,3,4,5].reverse(); /* Returns: [5,4,3,2,1] */
"Hello".reverse();     /* Returns: "olleH" */
"héllo".reverse();     /* Returns: "olléh" (Unicode characters reversed) */
"🚀héllo".reverse();   /* Returns: "olléh🚀" (emoji and Unicode characters reversed) */
"👨‍👩‍👧‍👦".reverse();  /* Returns: "👦👧‍👩‍👨‍" (complex emoji sequences reversed) */
```

**Unicode Support:** For strings, this function now properly handles Unicode characters, emoji, and complex grapheme clusters by reversing characters rather than bytes.

### `.replace(old, new)`
Replaces occurrences of old text with new text.

```grapa
"Hello, World!".replace("World", "Grapa"); /* Returns: "Hello, Grapa!" */
"Hello, World!".replace("o", "0");         /* Returns: "Hell0, W0rld!" */
```

**Parameters:**
- `old` (default: null) - Text to replace
- `new` (default: null) - Replacement text

### `.interpolate(vars)`
Interpolates variables into a string template.

```grapa
"Hello ${name}".interpolate({"name": "Grapa"}); /* Returns: "Hello Grapa" */
"Value: ${value}".interpolate({"value": 42});   /* Returns: "Value: 42" */
```

**Parameters:**
- `vars` (default: null) - Variables to interpolate

### `.upper()`
Converts the string to uppercase.

```grapa
"Hello, World!".upper(); /* Returns: "HELLO, WORLD!" */
```

### `.lower()`
Converts the string to lowercase.

```grapa
"Hello, World!".lower(); /* Returns: "hello, world!" */
```

### `.casefold()`
Converts the string to Unicode case folded form for case-insensitive comparisons.

```grapa
"Hello, World!".casefold(); /* Returns: "hello, world!" */
```

---

## Array and List Methods

### `.split(delim, max, axis)`
Splits a string into an array using the specified delimiter.

```grapa
"Hello, World!".split(", ");           /* Returns: ["Hello","World!"] */
"Hello, World!".split(" ", 3, 0);      /* Returns: ["Hello,","World!"] */
```

**Parameters:**
- `delim` (default: null) - Delimiter string
- `max` (default: null) - Maximum number of splits
- `axis` (default: null) - Axis for splitting

### `.join(delim, axis)`
Joins array elements into a string using the specified delimiter.

```grapa
[1,2,3,4,5].join(", ");    /* Returns: "1, 2, 3, 4, 5" */
[1,2,3,4,5].join("-", 0);  /* Returns: "1-2-3-4-5" */
```

**Parameters:**
- `delim` (default: null) - Delimiter string
- `axis` (default: null) - Axis for joining

### `.diagonal(offset)`
Returns the diagonal of a matrix or array.

```grapa
/* For 2D arrays/matrices */
matrix = [[1,2,3], [4,5,6], [7,8,9]];
matrix.diagonal();         /* Returns: [1,5,9] (main diagonal) */
matrix.diagonal(1);        /* Returns: [2,6] (upper diagonal) */
matrix.diagonal(-1);       /* Returns: [4,8] (lower diagonal) */

/* For 1D arrays - returns the array itself */
[1,2,3,4,5].diagonal();   /* Returns: [1,2,3,4,5] */
```

**Parameters:**
- `offset` (default: 0) - Diagonal offset (0 = main diagonal, positive = upper, negative = lower)

### `.identity()`
Returns the identity matrix.

```grapa
obj.identity();            /* Returns identity matrix */
```

### `.sort(axis, order, kind)`
Sorts elements across multiple data types: `$ARRAY`, `$TUPLE`, `$LIST`, `$OBJ`, `$XML`, `$TAG`, `$EL`, `$OP`, `$CODE`.

```grapa
/* Arrays and Tuples - sort by value */
[3,1,4,1,5].sort();        /* Returns: [1,1,3,4,5] */
[3,1,4,1,5].sort(0, 1);    /* Returns: [5,4,3,1,1] */
(3,1,4,1,5).sort();        /* Returns: (1,1,3,4,5) */

/* Lists and Objects - sort by key */
{c:3,a:1,b:2}.sort();      /* Returns: {"a":1,"b":2,"c":3} */
```

**Parameters:**
- `axis` (default: null) - Axis to sort along
- `order` (default: null) - Sort order (1 for descending)
- `kind` (default: null) - Sort algorithm

### `.argsort(axis, order, kind)`
Returns the indices that would sort the array.

```grapa
[3,1,4,1,5].argsort();     /* Returns: [1,3,0,2,4] */
[3,1,4,1,5].argsort(0, 1); /* Returns: [4,2,0,1,3] */
```

**Parameters:**
- `axis` (default: null) - Axis to sort along
- `order` (default: null) - Sort order (1 for descending)
- `kind` (default: null) - Sort algorithm

### `.unique(axis)`
Returns unique elements across multiple data types: `$ARRAY`, `$TUPLE`, `$LIST`, `$OBJ`, `$XML`, `$TAG`, `$EL`, `$OP`, `$CODE`.

```grapa
/* Arrays and Tuples - remove duplicate values */
[1,2,2,3,3,3].unique();    /* Returns: [1,2,3] */
[1,2,2,3,3,3].unique(1);   /* Returns: [1,2,3] */
(1,2,2,3,3,3).unique();    /* Returns: (1,2,3) */

/* Lists and Objects - remove duplicate keys, keeping last value */
{a:1,b:2,a:3,c:4,b:5}.unique();  /* Returns: {"a":3,"b":5,"c":4} */
```

**Parameters:**
- `axis` (default: null) - Axis for uniqueness

### `.group(axis, func, data)`
Groups elements by a function.

```grapa
[1,2,2,3,3,3].group();     /* Returns: [1,2,2,3,3,3] */
[1,2,2,3,3,3].group(0, op(x) { x % 2; }, [1,2,3,4,5,6]); /* Groups by even/odd */
```

**Parameters:**
- `axis` (default: null) - Axis for grouping
- `func` (default: null) - Grouping function
- `data` (default: null) - Additional data for grouping

---

## Functional Programming Methods

### `.map(func, data, threads)`
Applies a function to each element of an array (parallel by default).

```grapa
[1,2,3,4,5].map(op(x) { x * 2; });     /* Returns: [2,4,6,8,10] */
[1,2,3,4,5].map(op(x) { x + 1; });     /* Returns: [2,3,4,5,6] */
```

**Parameters:**
- `func` - Function to apply to each element
- `data` (default: null) - Additional data for the function
- `threads` (default: null) - Number of threads for parallel execution

### `.filter(func, data, threads)`
Filters elements based on a predicate function (parallel by default).

```grapa
[1,2,3,4,5].filter(op(x) { x > 2; });      /* Returns: [3,4,5] */
[1,2,3,4,5].filter(op(x) { x % 2 == 0; }); /* Returns: [2,4] */
```

**Parameters:**
- `func` - Predicate function
- `data` (default: null) - Additional data for the function
- `threads` (default: null) - Number of threads for parallel execution

### `.reduce(func, init, data)`
Reduces an array to a single value using a function.

```grapa
[1,2,3,4,5].reduce(op(acc, x) { acc + x; }, 0); /* Returns: 15 */
[1,2,3,4,5].reduce(op(acc, x) { acc * x; }, 1); /* Returns: 120 */
```

**Parameters:**
- `func` - Reduction function
- `init` (default: null) - Initial value
- `data` (default: null) - Additional data for the function

### `.range(stop, step)`
Generates a range of numbers.

```grapa
(10).range();        /* Returns: [0,1,2,3,4,5,6,7,8,9] */
(10).range(10, 2);   /* Returns: [10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40] */
```

**Parameters:**
- `stop` (default: null) - End value (exclusive)
- `step` (default: null) - Step size

---

## Search and Pattern Methods

### `.grep(pattern, flags, start, end, count, offset)`
Searches for patterns in text using regex.

```grapa
"Hello, World!".grep("World");                    /* Returns: ["World"] */
"Hello, World!".grep("o", "g");                  /* Returns: ["o", "o"] */
"Hello, World!".grep("o", "g", 0, 10, 5, 0);     /* Returns: ["o", "o"] */
```

**Parameters:**
- `pattern` (default: null) - Regex pattern to search for
- `flags` (default: null) - Search flags (g=global, i=case-insensitive, etc.)
- `start` (default: null) - Start position
- `end` (default: null) - End position
- `count` (default: null) - Maximum number of matches
- `offset` (default: null) - Offset for results

### `.match(pattern, flags, start, end, count, offset)`
Matches patterns in text using regex.

```grapa
"Hello, World!".match("World");                   /* Returns: ["World"] */
"Hello, World!".match("o", "g");                  /* Returns: ["o", "o"] */
"Hello, World!".match("o", "g", 0, 10, 5, 0);    /* Returns: ["o", "o"] */
```

**Parameters:**
- `pattern` (default: null) - Regex pattern to match
- `flags` (default: null) - Match flags (g=global, i=case-insensitive, etc.)
- `start` (default: null) - Start position
- `end` (default: null) - End position
- `count` (default: null) - Maximum number of matches
- `offset` (default: null) - Offset for results

### `.findall(pattern)`
Finds all occurrences of a pattern in complex data structures. Works with `$XML`, `$LIST`, and `$ARRAY` data types using specific query patterns.

```grapa
/* ✅ CORRECT: Use with $XML, $LIST, and $ARRAY */
xml_data = <root><item>Hello</item><item>World</item></root>;
xml_data.findall({name:"item"});                  /* Returns: <item>Hello</item><item>World</item> */

list_data = {a:1, b:2, c:3};
list_data.findall({has:{name:"a"}});              /* Returns: {"list_data":{"a":1,"b":2,"c":3}} */

array_data = [{name:"Alice", age:30}, {name:"Bob", age:25}];
array_data.findall({has:{name:"name"}});          /* Returns: [{"name":"Alice","age":30},{"name":"Bob","age":25}] */

/* ❌ INCORRECT: Don't work with simple strings */
"Hello, World!".findall("o");                     /* Returns: {"error":-1} */
```

**Parameters:**
- `pattern` - Query pattern object (varies by data type)

**Data Types:** `$XML`, `$LIST`, `$ARRAY`

**Query Patterns:**
- **XML**: `{name:"tag"}` - Find elements by tag name
- **LIST**: `{has:{name:"key"}}` - Find objects containing specific keys
- **ARRAY**: `{has:{name:"key"}}` - Find objects containing specific keys

---

## Bit Manipulation Methods

### `.setbit(pos)`
Sets a bit at the specified position.

```grapa
42.setbit(1);       /* Returns: 42 */
42.setbit(3);       /* Returns: 42 */
```

**Parameters:**
- `pos` (default: 0) - Bit position to set

### `.clearbit(pos)`
Clears a bit at the specified position.

```grapa
42.clearbit(1);     /* Returns: 40 */
42.clearbit(3);     /* Returns: 34 */
```

**Parameters:**
- `pos` (default: 0) - Bit position to clear

### `.genbits()`
Generates bits from the object.

```grapa
obj.genbits();      /* Returns generated bits */
```

### `.genkeys(params)`
Generates keys from the object.

```grapa
obj.genkeys();      /* Returns generated keys */
```

**Parameters:**
- `params` (default: null) - Parameters for key generation

---

## Cryptography Methods

### `.secret()`
Performs Diffie-Hellman key exchange and cryptographic operations.

```grapa
/* Basic secret generation */
key = obj.secret();        /* Returns cryptographic key */

/* With specific parameters */
key = obj.secret("RSA");   /* Returns RSA key pair */
key = obj.secret("EC");    /* Returns Elliptic Curve key */
```

**Parameters:**
- `algorithm` (optional) - Cryptographic algorithm ("RSA", "EC", "DH")

**Security Notes:**
- This method is part of Grapa's cryptographic infrastructure
- Requires proper key management and secure parameter handling
- Should be used with appropriate error handling for production systems
- For advanced cryptographic operations, consider using `.encode()` and `.decode()` methods

### `.encode(method, params)`
Encodes the object using the specified method.

```grapa
"Hello, World!".encode("base64"); /* Returns: "SGVsbG8sIFdvcmxkIQ==" */
"Hello, World!".encode("hex");    /* Returns: {"error":-1} */
```

**Parameters:**
- `method` - Encoding method (base64, hex, etc.)
- `data` - Additional data for encoding

### `.decode(method, data)`
Decodes the object using the specified method.

```grapa
"SGVsbG8sIFdvcmxkIQ==".decode("base64"); /* Returns: "Hello, World!" */
"0xEBB4E96607D2AF2D2105".decode("hex");  /* Returns: {"error":-1} */
```

**Parameters:**
- `method` - Decoding method (base64, hex, etc.)
- `data` - Additional data for decoding

### `.sign(method, params)`
Signs the object using the specified method.

```grapa
"Hello, World!".sign("sha256"); /* Returns: {"error":-1} */
"Hello, World!".sign("md5");    /* Returns: {"error":-1} */
```

**Parameters:**
- `method` - Signing method (sha256, md5, etc.)
- `params` (default: null) - Additional parameters

### `.signadd(method, params)`
Adds a signature to the object.

```grapa
"Hello, World!".signadd("sha256"); /* Returns: {"error":-1} */
"Hello, World!".signadd("md5");    /* Returns: {"error":-1} */
```

**Parameters:**
- `method` - Signing method
- `params` (default: null) - Additional parameters

### `.verify(method, data, params)`
Verifies a signature on the object.

```grapa
"Hello, World!".verify("sha256", "data"); /* Returns: {"error":-1} */
"Hello, World!".verify("md5", "data");    /* Returns: {"error":-1} */
```

**Parameters:**
- `method` - Verification method
- `data` (default: null) - Data to verify against
- `params` (default: null) - Additional parameters

### `.verifyrecover(method, params)`
Recovers data from a signature.

```grapa
"Hello, World!".verifyrecover("sha256"); /* Returns: {"error":-1} */
"Hello, World!".verifyrecover("md5");    /* Returns: {"error":-1} */
```

**Parameters:**
- `method` - Recovery method
- `params` (default: null) - Additional parameters

### `.secret(key)`
Performs secret key operations (may require additional setup).

```grapa
"Hello, World!".secret("key"); /* Returns: {"error":-1} or secret result */
42.secret("key");              /* Returns: {"error":-1} or secret result */
```

**Parameters:**
- `key` (default: null) - Secret key

---

## File System Methods

### `.setfile(mode, path, data)`
Sets file content or properties.

```grapa
"content".setfile("test.txt", "w"); /* Returns: {"error":-1} */
"content".setfile("test.txt", "a"); /* Returns: {"error":-1} */
```

**Parameters:**
- `mode` - File mode (w=write, a=append, etc.)
- `path` - File path
- `data` - Data to write

---

## Utility Methods

### `.exec()`
Executes a string as Grapa code, parsing and evaluating it as if it were typed directly in the console. This is functionally equivalent to `op()(script)()`.

```grapa
/* Basic number parsing */
"5".exec();                          /* Returns: 5 */
"5.5".exec();                        /* Returns: 5.5 */

/* Hex and binary literal parsing */
"0x12".exec();                       /* Returns: 18 */
"0x5.f".exec();                      /* Returns: 5.9375 */
"0b101".exec();                      /* Returns: 5 */
"0b101.011".exec();                  /* Returns: 5.375 */

/* Mathematical expressions */
"5 + 3".exec();                      /* Returns: 8 */
"2 * 4".exec();                      /* Returns: 8 */
"10 / 2".exec();                     /* Returns: 5 */
"2 ^ 3".exec();                      /* Returns: 8 */

/* Variable assignment and usage */
"x = 5".exec();                      /* Assigns x = 5 */
"x".exec();                          /* Returns: 5 */
"y = x + 3".exec();                  /* Assigns y = 8 */

/* Function calls */
"op(x){x*2}(5)".exec();              /* Returns: 10 */
"op(x,y){x+y}(3,4)".exec();          /* Returns: 7 */

/* Data structures */
"[1,2,3]".exec();                    /* Returns: [1,2,3] */
"{a:1,b:2}".exec();                  /* Returns: {"a":1,"b":2} */
"[1,2,3].len()".exec();              /* Returns: 3 */

/* Error handling */
"invalid syntax".exec();              /* Returns: {"error":-1} */
"5 +".exec();                        /* Returns: [5,{"match":"+","fail":""}] */

/* Comparison with op()() */
script = "5 + 3";
script.exec();                        /* Returns: 8 */
op()(script)();                       /* Returns: 8 (equivalent) */
```

**Relationship to `op()()`:**
- `script.exec()` is functionally equivalent to `op()(script)()`
- Both use the same underlying Grapa parser and execution engine
- Both support the same syntax including hex/binary literals, expressions, functions, etc.
- `.exec()` is a more convenient syntax for the same functionality

**Key Features:**
- **Full Grapa Parser**: Parses any valid Grapa syntax including literals, expressions, functions, and data structures
- **Hex/Binary Support**: Automatically recognizes and parses hex (`0x`) and binary (`0b`) literals
- **Variable Scope**: Executes in the current namespace, allowing variable assignment and retrieval
- **Error Handling**: Returns structured error information for invalid syntax
- **Expression Evaluation**: Supports complex mathematical and logical expressions
- **Function Execution**: Can execute function definitions and calls
- **Data Structure Creation**: Parses arrays, objects, and their method calls
- **Customizable Execution**: Optional callback, set, and get functions for custom behavior

**Use Cases:**
- **String-to-Number Conversion**: Parse hex/binary strings to numbers
- **Dynamic Code Execution**: Execute code stored in strings
- **Configuration Parsing**: Parse configuration strings as Grapa expressions
- **User Input Processing**: Safely execute user-provided expressions
- **Template Evaluation**: Evaluate expressions embedded in strings
- **Custom Execution Context**: Use callback/set/get for specialized execution environments

### `.getname(index)`
Gets the name of an object property.

```grapa
obj.getname();       /* Returns: property name */
obj.getname(1);      /* Returns: property name at index 1 */
```

**Parameters:**
- `index` (default: null) - Index of the property

**Advanced Usage - Recursive Keys Function:**
```grapa
/* Create a recursive .keys() function using .getname() */
keys = op(lst){lst.reduce(op(acc,x){if(x.type()==$LIST){acc += keys(x);}else{acc += 'x'.getname();}},[]);};

/* Usage examples */
obj = {name:"Alice", age:30, test:{a:1,b:2}, city:"NYC"};
keys(obj);  /* Returns: [name,age,[a,b],city] */

nested = {level1:{level2:{level3:"value"}}};
keys(nested);  /* Returns: [level1,[level2,[level3]]] */
```

This demonstrates how `.getname()` can be used with `.reduce()` to create powerful object introspection utilities.

### `.iferr(error)`
Provides error fallback for operations.

```grapa
obj.iferr("error occurred"); /* Returns: "error occurred" if operation fails */
```

**Parameters:**
- `error` - Error message or fallback value

### `.echo()`
Outputs the object to the console.

```grapa
"Hello, World!".echo(); /* Outputs: Hello, World! */
42.echo();              /* Outputs: 42 */
```

### `.string()`
Converts the object to a string representation.

```grapa
obj.string();           /* Returns string representation */
```

### `.const()`
Gets the constant value of the object.

```grapa
obj.const();            /* Returns constant value */
```

### `.json()`
Converts the object to JSON format.

```grapa
obj.json();             /* Returns JSON representation */
```

### `.xml()`
Converts the object to XML format.

```grapa
obj.xml();              /* Returns XML representation */
```

### `.html()`
Converts the object to HTML format.

```grapa
obj.html();             /* Returns HTML representation */
```

### `.shell()`
Executes the object as a shell command.

```grapa
obj.shell();            /* Executes as shell command */
```

### `.post()`
Performs a POST operation with the object.

```grapa
obj.post();             /* Performs POST operation */
```

### `.isint()`
Checks if the object is an integer.

```grapa
obj.isint();            /* Returns: true/false */
```

---

## Usage Examples

### Type Conversion Chain
```grapa
/* Convert string to float with precision, then to base-16 */
result = "3.14159".float(4, 0).base(16);
result.echo();  /* Outputs: 3.1416 in hex */
```

### String Processing Pipeline
```grapa
/* Process text through multiple operations */
text = "  Hello, World!  ";
processed = text.trim().upper().replace("WORLD", "GRAPA");
processed.echo();  /* Outputs: HELLO, GRAPA! */
```

### Array Processing with Functional Methods
```grapa
/* Process array with map, filter, and reduce */
data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
result = data
    .filter(op(x) { x % 2 == 0; })      /* Keep even numbers */
    .map(op(x) { x * x; })              /* Square each number */
    .reduce(op(acc, x) { acc + x; }, 0); /* Sum all squares */
result.echo();  /* Outputs: 220 */
```

### Error Handling with .iferr()
```grapa
/* Safe access to potentially problematic operations */
value = risky_operation().iferr("default value");
value.echo();  /* Outputs: result or "default value" */
```

### Pattern Matching with .grep()
```grapa
/* Find all email addresses in text */
text = "Contact us at john@example.com or jane@test.org";
emails = text.grep("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}", "g");
emails.echo();  /* Outputs: ["john@example.com", "jane@test.org"] */
```

---

## Notes

- **Parallel Execution**: `.map()` and `.filter()` are parallel by default for large arrays
- **Error Handling**: Use `.iferr()` for simple error fallbacks
- **Type Safety**: Always check types with `.type()` before using type-specific methods
- **Performance**: For large datasets, specify thread counts in parallel operations
- **Cryptography**: Some crypto methods may require additional setup or dependencies

[Back to Top](#obj-methods-reference)
