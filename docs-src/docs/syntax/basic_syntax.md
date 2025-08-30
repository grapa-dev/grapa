---
tags:
  - user
  - highlevel
---
# Basic Grapa Syntax Guide

> **Canonical Reference:**
> This file is the canonical, empirically tested reference for Grapa syntax and access patterns. All code in documentation and tests must comply with these rules. If you discover any new rules or exceptions, update this file immediately.

> **Important: Access Patterns for Data Types (Tested, v0.0.39)**
>
> | Type      | .getfield("key") | .getfield(index) | Bracket Notation | Dot Notation | .len() | .size() | .get()/.set() |
> |-----------|:---------------:|:----------------:|:----------------:|:------------:|:------:|:-------:|:-------------:|
> | $ARRAY    |        ❌        |        ❌        |       ✅         |      —       |   ✅   |    ❌   |      ❌       |
> | $LIST     |        ❌        |        ❌        |       ✅         |     ✅       |   ✅   |    ❌   |      ❌       |
> | $OBJ      |        ❌        |        ❌        |       ✅         |     ✅       |   ❌   |    ❌   |      ❌       |
> | $file     |        ✅        |        ❌        |        —         |      —       |   ❌   |    ❌   |      ❌       |
> | $TABLE    |       ✅*        |        ❌        |        —         |      —       |   ❌   |    ❌   |      ❌       |
> | $WIDGET   |        —         |        —         |        —         |      —       |   —   |    —   |      ✅       |
>
> *$TABLE .getfield() requires two arguments: key and field.
> **`.get()/.set()` methods are exclusively for `$WIDGET` types.
>
> **Key Findings:**
> - **Arrays (`[]`)**: Use `array[index]` and `array.len()` for access and length
> - **Lists (`{}`)**: Use `list[key]` or `list.key` for access, `list.len()` for length
> - **Objects (class)**: Use `object.property` or `object[key]` for access
> - **`.getfield()/.setfield()` method**: Use for `$file` and `$TABLE` types
> - **`.get()/.set()` method**: Exclusively for `$WIDGET` types
> - **`.size()` method**: Not supported on any type (use `.len()` instead)
> - **`.keys()` method**: Not supported on `$LIST` (use iteration instead)
>
> **Recommended Patterns:**
> - For arrays: `array[index]` and `array.len()`
> - For lists: `list[key]` (preferred) or `list.key`, and `list.len()`
> - For objects: `object.property` (preferred) or `object[key]`
> - For files: `file.getfield(key)` and `file.setfield(key, value)`
> - For tables: `table.getfield(key, field)` and `table.setfield(key, value, field)`
> - For widgets: `widget.get(name, params)` and `widget.set(name, data)`
> - Avoid `.get()`, `.size()`, and `.keys()` on arrays, lists, and objects

> **Clarification on .get()/.set() Usage:**
> - **`.get()/.set()` for `$file` and `$TABLE`**: Use `.getfield()/.setfield()` instead
> - **`.get()/.set()` for `$WIDGET`**: These methods are exclusively for widget operations
> - **`.get()/.set()` for `$ARRAY`, `$LIST`, or `$OBJ`**: Not supported (use bracket/dot notation)
> - **Length**: Use `.len()` for arrays and lists, not `.size()`.
> - **Keys**: For lists, iterate manually instead of using `.keys()`.
> - **Design Decision**: `.get()/.set()` methods are now exclusively for `$WIDGET` types to avoid confusion

---

This guide covers the essential syntax patterns for writing Grapa code, based on working examples and best practices.

**See also:** [Operator Precedence Table](precedence.md), [Namespace Scoping](namespace_scoping.md)

## Variable Scoping

Grapa provides sophisticated namespace scoping with `$global`, `$this`, and `$local` identifiers. Proper use of local variables is crucial for thread safety and avoiding variable conflicts.

**Important**: All namespace accessors must use the `$` prefix. The old syntax without `$` (e.g., `global.x`, `this.x`, `local.x`) is no longer supported.

```grapa
/* ✅ RECOMMENDED - Declare local variables upfront */
myFunction = op() {
    $local.result = null;
    $local.temp = {};
    $local.i = 0;
    
    // Now safe to use variables without conflicts
    result = process();
    return result;
};

/* ✅ ALTERNATIVE - Reset $local to a list */
myFunction2 = op() {
    $local = {result: null, temp: {}, i: 0};
    
    // Now safe to use variables without conflicts
    result = process();
    return result;
};

/* ❌ AVOID - Potential conflicts with global scope */
badFunction = op() {
    result = process();  // Could overwrite global 'result'
    return result;
};
```

**See [Namespace Scoping](namespace_scoping.md) for comprehensive details on variable scoping, thread safety, and best practices.**

## Output and Echo

### Correct Echo Syntax

Use the `.echo()` method on strings, not function calls:

```grapa
/* ✅ Correct */
"Hello World".echo();
("Result: " + value.str()).echo();

/* ❌ Incorrect */
echo("Hello World");
echo("Result: " + value);
```

### Debug Output

Use the `.debug()` method for debug output that integrates with Grapa's debug system:

```grapa
/* ✅ Correct - Debug output */
"Processing data".debug(0);
"Database query".debug(1, "db");
"API response".debug(2, "api");

/* ❌ Incorrect */
debug("Processing data");
debug("Database query", 1);
```

**Note:** The `.debug()` method requires the `-d` flag to be enabled and respects component-specific debug levels. See [Debugging Guide](../debugging.md) for details.

### String Concatenation

**⚠️ RECOMMENDED: Use string interpolation instead of concatenation**

String interpolation is the preferred approach for combining strings and values:

```grapa
/* ✅ RECOMMENDED - String interpolation (preferred) */
"Name: ${name}, Age: ${age.str()}".interpolate().echo();
"File size: ${size} bytes".interpolate().echo();

/* ✅ With parameters */
"Name: ${name}, Age: ${age.str()}".interpolate({name:"Bob",age:23}).echo();

/* ✅ Complex expressions */
"Sum: ${x + y}, Product: ${x * y}".interpolate().echo();

/* ✅ Elegant function chaining */
"${'Hello'.upper()} ${'World'.lower()}".interpolate().echo();  /* "HELLO world" */
"Result: ${[1,2,3].filter(op(x){x>1}).str()}".interpolate().echo();  /* "Result: [2,3]" */

/* ✅ Test script output */
"✓ Test passed: ${test_name}\n".interpolate().echo();
"✗ Test failed: expected ${expected}, got ${actual}\n".interpolate().echo();
"Processing ${record_count} records...\n".interpolate().echo();
```

**String concatenation (if needed):**
Always wrap string concatenation expressions in parentheses:

```grapa
/* ✅ Correct - but prefer interpolation */
("Name: " + name.str() + ", Age: " + age.str()).echo();
("File size: " + size + " bytes").echo();

/* ❌ Incorrect */
"Name: " + name.str() + ", Age: " + age.str().echo();
"File size: " + size + " bytes".echo();
```

**Why use interpolation?**
- **More readable** - Template-style syntax is clearer
- **More powerful** - Supports expressions and script execution  
- **Less error-prone** - No operator precedence issues
- **More flexible** - Parameter passing and complex expressions
- **Elegant function chaining** - Provides solution for complex method chaining across different object types
- **Perfect for test scripts** - Clean output formatting without multiple `.echo()` calls

## Loops

### While Loops

Use `while` loops with explicit increment:

```grapa
/* ✅ Correct */
i = 1;
while (i <= 10) {
    ("Processing item " + i.str()).echo();
    i = i + 1;
}

/* ✅ Correct - For loops are now supported! */
for i from 1 to 10 {
    ("Processing item " + i).echo();
}

/* ✅ For-in loops for collections */
for item in [1, 2, 3, 4, 5] {
    ("Item: " + item).echo();
}

/* ✅ C-style for loops */
for (i = 1; i <= 10; i = i + 1) {
    ("Count: " + i).echo();
}

/* ✅ Do-while loops */
do {
    ("Executing at least once").echo();
} while (condition);
```

- **For loops are now fully supported** with multiple syntax variations
- `while` loops and `.range()`+functional methods remain valid alternatives
- The `.range()` method is native: `(10).range()` → `[0,1,2,3,4,5,6,7,8,9]`
- If more objects support `.get()` in the future, this guide will be updated.

### For Loop Syntax

Grapa supports multiple for loop variations:

#### Numeric Range Loops
```grapa
/* Basic range loop */
for i from 1 to 5 {
    ("Count: " + i).echo();
}

/* Range with step */
for i from 0 to 10 step 2 {
    ("Even: " + i).echo();
}

/* Reverse range */
for i from 10 to 1 step -1 {
    ("Countdown: " + i).echo();
}
```

#### Collection Loops
```grapa
/* Array iteration */
numbers = [1, 2, 3, 4, 5];
for item in numbers {
    ("Item: " + item).echo();
}

/* String iteration (character by character) */
text = "Hello";
for char in text {
    ("Character: " + char).echo();
}

/* List iteration */
data = {a:1, b:2, c:3};
for value in data {
    ("Value: " + value).echo();
}
```

#### C-Style Loops
```grapa
/* Traditional for loop */
for (i = 1; i <= 10; i = i + 1) {
    ("Count: " + i).echo();
}

/* Complex initialization */
for (i = 0, j = 10; i < j; i = i + 1, j = j - 1) {
    ("i=" + i + ", j=" + j).echo();
}
```

#### Do-While Loops
```grapa
/* Execute at least once */
do {
    ("Executing...").echo();
    user_input = get_user_input();
} while (user_input != "quit");
```

### Loop Examples

```grapa
/* Count from 1 to 5 */
i = 1;
while (i <= 5) {
    ("Count: " + i.str()).echo();
    i = i + 1;
}

/* Process array elements */
i = 0;
while (i < array.len()) {
    element = array[i];  /* Use bracket notation for arrays */
    ("Element " + i.str() + ": " + element.str()).echo();
    i = i + 1;
}
```

### Numbers

Grapa supports various number formats:

```grapa
/* Integer literals */
42;                 /* Decimal integer */
0x2A;              /* Hexadecimal integer */
0b101010;          /* Binary integer */

/* Float literals */
3.14;              /* Standard decimal notation */
3_14;              /* Underscore decimal notation (equivalent) */
2.5;               /* Standard decimal notation */
2_5;               /* Underscore decimal notation (equivalent) */

/* Verification */
(3.14 == 3_14).echo();    /* true */
(2.5 == 2_5).echo();      /* true */
```

**Float Decimal Separators:**
- **Dots (`.`)**: Standard decimal notation (e.g., `3.14`)
- **Underscores (`_`)**: Alternative decimal notation (e.g., `3_14`)
- **Interchangeable**: Both notations are completely equivalent
- **Single separator**: Only one decimal separator allowed per number (standard mathematical definition)
- **Modern support**: Aligns with other modern programming languages

### Number Method Calls and Dot Notation

Numbers can be used directly with dot notation for method calls:

```grapa
/* ✅ Correct - Direct number method calls */
20.random();      /* Works: Random number 0-19 */
3.14.floor();     /* Works: Floor to 3 */
42.str();         /* Works: Convert to string */
145.len();        /* Works: Length of number (3) */
145.echo();       /* Works: Output the number */
```

**Note:** Numbers work seamlessly with dot notation for method calls.

### Hex and Binary Literals

Grapa supports hexadecimal and binary literals with underscore separators for readability:

```grapa
/* Hexadecimal literals */
0x12;              /* 18 in decimal */
0xABCD;            /* 43981 in decimal */
0xAbCd;            /* 43981 in decimal (case insensitive) */
0x1234;            /* 4660 in decimal */
0x12.34;           /* 18.203125 in decimal (hex float) */
0x1234.5678;       /* 4660.33984375 in decimal (hex float) */

/* Binary literals */
0b010;             /* 2 in decimal */
0b1010;            /* 10 in decimal */
0b010101;          /* 21 in decimal */
0b010.011;         /* 2.375 in decimal (binary float) */
0b010101.011001;   /* 21.1875 in decimal (binary float) */

/* Type behavior */
0x12.type();       /* $INT */
0x12.34.type();    /* $FLOAT */
0b010.type();      /* $INT */
0b010.011.type();  /* $FLOAT */

/* Method calls work on hex/binary literals */
0x12.hex();        /* "12" (hex representation) */
0x12.int();        /* 18 (decimal value) */
0x12.float();      /* 18.0 (float conversion) */
0b010.bin();       /* "10" (binary representation) */
0b010.int();       /* 2 (decimal value) */
0b010.float();     /* 2.0 (float conversion) */
```

**Key Features:**
- **Case insensitive**: `0xABCD` and `0xAbCd` are equivalent
- **Float support**: Both hex and binary support decimal points
- **Type consistency**: Returns `$INT` for integers, `$FLOAT` for floats
- **Method support**: All number methods work on hex/binary literals
- **String parsing**: Use `.exec()` to parse hex/binary strings: `'0x12'.exec()` → `18`
- **Underscore separators**: Not yet implemented (planned feature for readability)

**String Conversion:**
```grapa
/* Parse hex/binary strings */
'0x12'.exec();           /* 18 */
'0x12.34'.exec();        /* 18.203125 */
'0b101'.exec();          /* 5 */
'0b101.011'.exec();      /* 5.375 */
'0x1234'.exec();         /* 4660 */
'0b010101'.exec();       /* 21 */
```

### Unsigned Number Methods (Cryptographic)

Grapa provides specialized unsigned methods for cryptographic applications and binary data processing. These methods treat negative numbers as unsigned integers using two's complement representation:

```grapa
/* Unsigned integer conversion */
(-1).uint();        /* 255 (8-bit unsigned equivalent) */
(-100).uint();      /* 156 (8-bit unsigned equivalent) */
(-1000).uint();     /* 64536 (16-bit unsigned equivalent) */
(255).uint();       /* 255 (no change for positive numbers) */

/* Unsigned raw representation (with 0x prefix) */
(-1).uraw();        /* 0xD6 (unsigned hex with 0x prefix) */
(255).uraw();       /* 0x0FF (with leading zero for consistency) */
(-100).uraw();      /* 0x9C (unsigned representation) */

/* Unsigned hex representation (without 0x prefix) */
(-1).uhex();        /* "D6" (unsigned hex without 0x prefix) */
(255).uhex();       /* "FF" (no change for positive numbers) */
(-100).uhex();      /* "9C" (unsigned representation) */

/* Unsigned binary representation */
(-1).ubin();        /* "11010110" (unsigned binary) */
(255).ubin();       /* "11111111" (no change for positive numbers) */
(-100).ubin();      /* "10011100" (unsigned representation) */
```

**Key Features:**
- **Two's complement conversion**: Negative numbers are converted to their unsigned equivalent
- **Cryptographic safety**: Prevents sign issues when working with large random numbers
- **Bit width preservation**: Maintains appropriate bit width for conversions
- **No effect on positives**: Positive numbers remain unchanged

**Cryptographic Applications:**
- **Large random number generation**: Handle numbers with leading bits set without sign issues
- **Key generation**: Ensure cryptographic keys are treated as unsigned values
- **Hash function inputs**: Prevent negative number interpretation in hash calculations
- **Binary data processing**: Work with raw bytes and network protocols

**Comparison with regular methods:**
```grapa
/* Regular methods preserve sign */
(-1).int();         /* -1 */
(-1).hex();         /* "-01" */
(-1).bin();         /* "-1" */
(-1).raw();         /* 0xD6 */

/* Unsigned methods convert to unsigned */
(-1).uint();        /* 255 */
(-1).uhex();        /* "D6" */
(-1).ubin();        /* "11010110" */
(-1).uraw();        /* 0xD6 */
```

**Important: For cryptographic output, prefer `.uhex()` over `.uraw()`**
```grapa
/* .uhex() provides clean hex string for cryptographic use */
0xFFFF.uhex();      /* "FFFF" (standard cryptographic format) */

/* .uraw() includes 0x prefix and leading zeros */
0xFFFF.uraw();      /* "0x0FFFF" (with formatting) */
```

**Critical: Cryptographic Function Output Interpretation**

When using cryptographic functions like `encode()` and `sign()`, the results are returned as `GrapaTokenType::RAW` data. This data may appear negative when displayed with `.hex()` if the high bit is set:

```grapa
/* Cryptographic function output */
md_keys = "md".genkeys({"digest": "sha256"});
message = "Hello World";
hash = message.encode(md_keys);

/* ❌ INCORRECT: May show negative hex if high bit is set */
hash.hex().echo();  /* May show: -5A6E592BF40BDFBFB5FEE8CC30484E6F29D39A40F4325CD4A84D88265260EB92 */

/* ✅ CORRECT: Use unsigned methods for cryptographic output */
hash.uint().hex().echo();  /* Shows: A591A6D40BF420404A011733CFB7B190D62C65BF0BCDA32B57B277D9AD9F146E */
hash.uhex().echo();        /* Shows: A591A6D40BF420404A011733CFB7B190D62C65BF0BCDA32B57B277D9AD9F146E */
```

**Why this happens:**
- **`encode()` and `sign()` return `GrapaTokenType::RAW` data**
- **`GrapaInt::FromBytes()` interprets `RAW` data with high bit set as negative**
- **Cryptographic outputs often have the high bit set** (e.g., SHA256 hashes)
- **Use `uint()` or `uhex()` for proper unsigned interpretation**

**Best practice for cryptographic code:**
```grapa
/* Always use unsigned methods for cryptographic operations */
hash = message.encode(md_keys);
signature = keys.sign(message);

/* For display and comparison */
hash_hex = hash.uhex();
signature_hex = signature.uhex();

/* For mathematical operations */
hash_uint = hash.uint();
result = hash_uint.modpow(exponent, modulus);
```

### Hex Representation Methods: `.hex()` vs `.raw()`

Grapa provides two different hex representation methods that serve different purposes:

**`.hex()` - Value Representation:**
```grapa
42.hex();           /* "2A" (value as hex) */
255.hex();          /* "FF" (value as hex) */
4.5.hex();          /* "4.8" (decimal value as hex) */
```

**`.raw()` - Internal Binary Representation:**
```grapa
42.raw();           /* "0x2A" (internal binary as hex) */
255.raw();          /* "0x0FF" (internal binary as hex) */
4.5.raw();          /* "0x00281000A09" (internal binary structure) */
```

**Key Differences:**
- **`.hex()`**: Converts the **value** to hex representation
- **`.raw()`**: Shows the **internal binary structure** as hex
- **For `$FLOAT`**: `.hex()` shows decimal value as hex, `.raw()` shows internal binary format
- **For `$INT`**: Both show similar results but `.raw()` includes `0x` prefix and leading zeros

### Sequence Generation with .range()

Use `.range()` to generate sequences of numbers:

```grapa
/* Generate sequence 0 to 9 */
seq = (10).range(0,1);  /* [0,1,2,3,4,5,6,7,8,9] */

/* Generate sequence with custom start and step */
seq = (10).range(1,2);  /* [1,3,5,7,9] */

/* Use .range() with functional methods for iteration */
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);  /* Sum 0-9 */

/* Parallel processing with thread count */
squares = (1000000).range(0,1).map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

- Use `.range()` instead of manual while loops for sequence generation
- Combine with `.map()`, `.filter()`, `.reduce()` for functional iteration
- Specify thread count for large arrays to avoid too many threads
- **Note:** Numbers work seamlessly with dot notation: `10.range()` works perfectly

## Functional Programming Methods

Grapa provides powerful functional programming methods for array and list processing:

### .map() - Transform Elements

```grapa
/* Transform each element */
numbers = [1, 2, 3, 4, 5];
doubled = numbers.map(op(x) { x * 2; });  /* [2, 4, 6, 8, 10] */

/* Transform with string operations */
names = ["john", "jane", "bob"];
uppercase = names.map(op(name) { name.upper(); });  /* ["JOHN", "JANE", "BOB"] */
casefolded = names.map(op(name) { name.casefold(); });  /* ["john", "jane", "bob"] */

/* Parallel processing with thread count */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### .filter() - Select Elements

```grapa
/* Filter even numbers */
numbers = [1, 2, 3, 4, 5, 6];
evens = numbers.filter(op(x) { x % 2 == 0; });  /* [2, 4, 6] */

/* Filter non-empty strings */
lines = ["hello", "", "world", "", "test"];
non_empty = lines.filter(op(line) { line.len() > 0; });  /* ["hello", "world", "test"] */

/* Parallel filtering */
large_data = (1000000).range(0,1);
filtered = large_data.filter(op(x) { x % 2 == 0; }, 8);  /* Limit to 8 threads */
```

### .reduce() - Accumulate Values

`.reduce()` accumulates values by applying a function to each element. The function takes two parameters: the accumulator (`acc`) and the current element (`x`).

#### **Initializer Behavior**
- **With initializer**: `acc` starts as the initializer, `x` is each array element
- **Without initializer**: `acc` starts as the first array element, `x` is each subsequent element

```grapa
/* Sum all numbers with initializer */
numbers = [1, 2, 3, 4, 5];
sum = numbers.reduce(op(acc, x) { acc + x; }, 0);  /* 15 */

/* Sum all numbers without initializer */
numbers = [1, 2, 3, 4, 5];
sum = numbers.reduce(op(acc, x) { acc + x; });  /* 15 */

/* Build a string */
words = ["hello", "world", "test"];
sentence = words.reduce(op(acc, word) { acc + " " + word; }, "");  /* " hello world test" */

/* Collect even numbers */
numbers = [1, 2, 3, 4, 5, 6];
evens = numbers.reduce(op(acc, x) {
    if (x % 2 == 0) { acc += x; };
    acc;
}, []);  /* [2, 4, 6] */
```

#### **Control Flow in .reduce()**
`.reduce()` supports all control flow statements with proper propagation (sequential processing):

```grapa
/* Break - terminates reduce and returns current accumulator */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) break; acc + x; });  /* Returns 1 */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) break; acc + x; }, 10);  /* Returns 11 */

/* Return - exits reduce and returns specified value */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) return 999; acc + x; });  /* Returns 999 */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) return 999; acc + x; }, 10);  /* Returns 999 */

/* Throw - propagates exception */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) throw "error"; acc + x; });  /* Throws "error" */

/* Control flow propagates through function calls */
func test() { 
    [1, 2, 3].reduce(op(acc, x) { if (x == 2) return 999; acc + x; }); 
    return 888; 
}; 
test();  /* Returns 999 */
```

**Important Notes:**
- **With initializer**: `acc` starts as initializer, `x` is each element
- **Without initializer**: `acc` starts as first element, `x` is each subsequent element
- **Control flow**: All control flow statements (break, return, throw) work and propagate correctly
- **Early termination**: Control flow can terminate reduce early and return appropriate values
- **Sequential processing**: `.reduce()` processes items sequentially like a loop

#### **Control Flow in .map() and .filter()**
`.map()` and `.filter()` support different control flow behaviors (parallel processing):

```grapa
/* Throw in map - collects all throws in array */
[1, 2, 3, 4, 5, 6, 7].map(op(x) {
    if (x % 3) throw("error");  /* Throws for items 1,2,4,5,7 */
    x % 3;
});  /* {"error","error",null,"error","error",null,"error"} */

/* Return in map - returns value for that specific item */
[1, 2, 3].map(op(x) {
    if (x == 2) return([3, 4]);  /* Returns [3,4] for item 2 */
    x * 2;
});  /* [2, [3,4], 6] */

/* Break in map - returns empty string for that specific item */
[1, 2, 3].map(op(x) {
    if (x == 2) break;  /* Returns "" for item 2 */
    x * 2;
});  /* [2, "", 6] */

/* Throw in filter - collects all throws in array */
[1, 2, 3, 4, 5, 6, 7].filter(op(x) {
    if (x % 3) throw("error");  /* Throws for items 1,2,4,5,7 */
    x % 2;
});  /* {"error","error",null,"error","error",null,"error"} */

/* Return in filter - returns value for that specific item */
[1, 2, 3].filter(op(x) {
    if (x == 2) return(false);  /* Excludes item 2 */
    x % 2;
});  /* [1, 3] */
```

**Important Notes:**
- **Parallel processing**: `.map()` and `.filter()` use multi-threaded processing
- **Throw behavior**: `.throw()` collects all results in array with `null` for non-throwing items
- **Return behavior**: `.return()` returns the value for that specific item only
- **Break behavior**: `.break()` returns empty string for that specific item only
- **Thread independence**: Each thread processes independently, control flow affects only that thread
- **Enhanced error collection**: Array length matches input, preserves all error information

### Method Chaining

Combine multiple functional methods:

```grapa
/* Filter, transform, then sum */
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
result = numbers
    .filter(op(x) { x % 2 == 0; })      /* [2, 4, 6, 8, 10] */
    .map(op(x) { x * x; })              /* [4, 16, 36, 64, 100] */
    .reduce(op(acc, x) { acc + x; }, 0); /* 220 */

/* Process with .range() */
result = (10).range(0,1)
    .filter(op(x) { x % 2 == 0; })      /* [0, 2, 4, 6, 8] */
    .map(op(x) { x * x; })              /* [0, 4, 16, 36, 64] */
    .reduce(op(acc, x) { acc + x; }, 0); /* 120 */
```



### Parallel Processing

Functional methods support parallel processing for large datasets:

```grapa
/* Large dataset processing */
data = (1000000).range(0,1);

/* Parallel map with 8 threads */
squares = data.map(op(x) { x * x; }, 8);

/* Parallel filter with 8 threads */
evens = data.filter(op(x) { x % 2 == 0; }, 8);

/* Parallel reduce with 4 threads */
sum = data.reduce(op(acc, x) { acc + x; }, 0, 4);
```

**Important Notes:**
- `.map()` and `.filter()` are parallel by default for large arrays
- Always specify thread count for very large datasets to avoid too many threads
- `.reduce()` can be parallel but requires careful consideration of the operation
- Method chaining is efficient and readable
- **Concurrency Coordination**: `.map()` and `.filter()` can be used as thread synchronization barriers

### Advanced Functional Programming: Complex Data Structures as Context

Grapa's functional methods support **any data structure as initializers**, enabling sophisticated state machines and context-aware operations. The operation function can be defined within the context of the data structure.

#### **Complex Initializers with State Objects**

```grapa
/* Stateful reduction with dynamic operation switching */
[1, 2, 3].reduce(op(x, y) {
    if (y == 2) x.o = op(a, b) { a * b; };  /* Switch to multiplication */
    x.st = x.o(x.st, y);  /* Apply current operation */
}, {st: 10, o: op(a, b) { a + b; }});  /* Initial state: {st: 10, o: add} */

/* Process: 10+1=11, 11*2=22, 22*3=66 */
/* Result: {"st": 66, "o": @<mul function>} */
```

#### **Context-Aware Operations**

```grapa
/* Map with context object */
[1, 2, 3].map(op(x, y) {
    y.count += 1;
    y.sum += x;
    x * y.multiplier;
}, {count: 0, sum: 0, multiplier: 2});

/* Filter with state tracking */
[1, 2, 3, 4, 5].filter(op(x, y) {
    y.seen += 1;
    if (y.seen <= 3) {  /* Only first 3 elements */
        y.sum += x;
        true;
    } else {
        false;
    };
}, {seen: 0, sum: 0});
```

#### **Dynamic Algorithm Switching**

```grapa
/* Reduce with algorithm selection based on data */
[1, 2, 3, 4, 5].reduce(op(x, y) {
    if (y > 3) {
        x.algorithm = "multiply";
        x.operation = op(a, b) { a * b; };
    };
    x.result = x.operation(x.result, y);
    x;
}, {
    result: 1,
    algorithm: "add",
    operation: op(a, b) { a + b; }
});
```

#### **State Machines and Pipelines**

```grapa
/* Complex state machine with multiple operations */
data = [1, 2, 3, 4, 5];
result = data.reduce(op(state, item) {
    /* Update counters */
    state.total += 1;
    state.sum += item;
    
    /* Dynamic operation based on state */
    if (state.total == 3) {
        state.operation = "multiply";
        state.func = op(a, b) { a * b; };
    };
    
    /* Apply current operation */
    state.result = state.func(state.result, item);
    state;
}, {
    total: 0,
    sum: 0,
    result: 1,
    operation: "add",
    func: op(a, b) { a + b; }
});
```

#### **Key Concepts**

1. **Any Data Structure**: Use objects, lists, arrays, or custom structures as initializers
2. **Context Evolution**: The data structure can evolve and change during the operation
3. **Dynamic Operations**: Functions can be stored and modified within the context
4. **State Machines**: Build complex state machines with multiple operations
5. **Pipeline Processing**: Create sophisticated data processing pipelines

#### **Use Cases**

- **Dynamic Algorithm Selection**: Switch algorithms based on data characteristics
- **State Tracking**: Maintain complex state across iterations
- **Pipeline Processing**: Build multi-stage data processing workflows
- **Configuration Management**: Use context objects for configuration and state
- **Complex Aggregations**: Perform sophisticated aggregations with multiple metrics
- **Worker Thread Coordination**: Coordinate multiple parallel workers
- **System Initialization**: Initialize multiple components in parallel
- **Resource Pool Management**: Coordinate multiple resource operations

#### **Concurrency Coordination with Functional Methods**

Grapa's `.map()` and `.filter()` methods can be used as **thread synchronization barriers** for coordinating multiple worker threads:

```grapa
/* Worker thread coordination */
workers = [1, 2, 3, 4, 5].map(op(worker_id) {
    /* Each worker does independent work */
    ("Worker " + worker_id.str() + " starting").echo();
    sleep(worker_id);  /* Simulate work */
    ("Worker " + worker_id.str() + " completed").echo();
    worker_id * 100;  /* Return result */
});
/* All workers complete before proceeding */
("All workers finished").echo();

/* Parallel task execution */
tasks = [
    op() { "Task A: Database query".echo(); sleep(2); "A done".echo(); },
    op() { "Task B: API call".echo(); sleep(1); "B done".echo(); },
    op() { "Task C: File processing".echo(); sleep(3); "C done".echo(); }
];
results = tasks.map(op(task) { task(); });
/* All tasks complete before proceeding */
("All tasks completed").echo();
```

#### **Why Dynamic Typing Enables This Flexibility**

Grapa's **dynamic typing** is a deliberate design choice that enables these advanced capabilities. Unlike strict type-constrained languages, Grapa leaves type handling to the implementation, allowing:

- **Runtime Type Evolution**: Data structures can change types during execution
- **Dynamic Function Assignment**: Operations can be swapped and modified at runtime  
- **Context-Aware Processing**: Any object can serve as context for any operation
- **Implementation-Driven Types**: Types emerge from how they're used, not predefined constraints

This design philosophy enables the sophisticated state machines and dynamic algorithms shown above, where the "type" of the context object evolves based on the data and operations being performed.

## Data Types and Access Patterns

### Array Type (`$ARRAY`)

Arrays are created with square brackets and support index-based access:

```grapa
/* Create arrays */
numbers = [1, 2, 3, 4, 5];
names = ["Alice", "Bob", "Charlie"];
mixed = [1, "text", true, 3.14];

/* Access elements */
first = numbers[0];      /* 1 */
second = names[1];       /* "Bob" */
length = numbers.len();  /* 5 */

/* Iterate over arrays */
i = 0;
while (i < numbers.len()) {
    ("Number " + i.str() + ": " + numbers[i]).echo();
    i += 1;
}
```

**Array Methods:**
- `array[index]` - Access element by index
- `array.len()` - Get array length
- `array.size()` - **Not supported** (use `.len()` instead)
- `array.get(index)` - **Not supported** (use bracket notation)

### List Type (`$LIST`)

Lists are created with curly braces and support key-based access:

```grapa
/* Create lists */
config = {"debug": true, "port": 3000, "host": "localhost"};
user = {"name": "John", "age": 30, "city": "NYC"};
nested = {"level1": {"level2": "value"}};

/* Access elements */
debug_mode = config["debug"];    /* true */
user_name = user.name;           /* "John" */
deep_value = nested["level1"]["level2"];  /* "value" */
length = config.len();           /* 3 */

/* Iterate over lists */
keys = ["debug", "port", "host"];
i = 0;
while (i < keys.len()) {
    key = keys[i];
    value = config[key];
    (key + ": " + value.str()).echo();
    i += 1;
}

/* Get all keys from a list */
config_keys = config.keys();     /* Returns ["debug", "port", "host"] */

/* Keys can be strings or identifiers */
mixed_obj = {a: 3, "b": 5, c: "value"};
mixed_keys = mixed_obj.keys();   /* Returns [a, "b", c] */

/* Iterate using .keys() */
all_keys = config.keys();
i = 0;
while (i < all_keys.len()) {
    key = all_keys[i];
    value = config[key];
    (key + ": " + value.str()).echo();
    i += 1;
}
```

**List Methods:**
- `list[key]` - Access element by key (preferred)
- `list.key` - Access element by key (alternative)
- `list.len()` - Get list length
- `list.get(key)` - **Not supported** (use bracket notation)
- `list.keys()` - Get array of all keys (supports both `$STR` and `$ID` keys)

### Object Type (`$OBJ`)

Objects are instances of classes and support property access:

```grapa
/* Define a class */
Person = class {
    name = "";
    age = 0;
    
    init = op(n, a) {
        name = n;
        age = a;
    };
    
    getInfo = op() {
        ("Name: " + name + ", Age: " + age.str()).echo();
    };
};

/* Create object instances */
person1 = obj Person;
person1.init("Alice", 25);
person2 = obj Person;
person2.init("Bob", 30);

/* Access properties */
name = person1.name;             /* "Alice" */
age = person1["age"];            /* 25 */
person1.getInfo();               /* Call method */
```

**Object Methods:**
- `object.property` - Access property (preferred)
- `object[key]` - Access property (alternative)
- `object.method()` - Call object methods
- `object.get(key)` - **Not supported** (use dot notation)

### Type Comparison Summary

| Feature | $ARRAY | $LIST | $OBJ |
|---------|--------|-------|------|
| Creation | `[1,2,3]` | `{"key":"value"}` | `obj Class` |
| Access | `array[index]` | `list[key]` or `list.key` | `object.property` |
| Length | `array.len()` | `list.len()` | Not available |
| Iteration | Index-based | Key-based | Property-based |
| Comparison | Element-by-element | Element-by-element | Property-by-property |
| Order Matters | ✅ Yes | ✅ Yes | ✅ Yes |
| Mixed Types | ❌ Never equal | ❌ Never equal | ❌ Never equal |

## Variable Assignment

### Basic Assignment

```grapa
/* Simple variables */
name = "John Doe";
age = 30;
salary = 75000.50;

/* Incrementing and appending */
v = v + 1;   /* Valid */
v += 1;      /* Idiomatic and preferred */
s = s + "x";   /* Valid */
s += "x";      /* Idiomatic and preferred */
```

### Namespace System

Grapa has a dynamic namespace system that's automatically managed by the execution tree:

#### Global Namespace
Use `$global` for variables that need to persist across function calls:

```grapa
/* Create global variable */
$global.my_table = {}.table("ROW");

/* Use global variable */
my_table.mkfield("name", "STR", "VAR");
```

#### Local Namespace
Each function execution creates its own namespace automatically:

```grapa
/* Function with local variables */
func = op(x) {
    local_var = x * 2;  /* This is in the function's local namespace */
    local_var.echo();
};

func(5);  /* Output: 10 */
```

#### Namespace Hierarchy
Variables can be accessed from parent namespaces:

```grapa
/* Global variable */
$global.config = {"debug": true, "timeout": 30};

/* Function can access global variables */
func = op() {
    if (config.debug) {
        ("Debug mode enabled").echo();
    }
    /* Local override */
local_config = {"debug": false};
    local_config.debug.echo();  /* false */
};

func();
```

#### Dynamic Namespace Creation
Namespaces are created automatically for each execution context:

```grapa
/* Each block creates a new namespace */
if (condition) {
    block_var = "value";  /* In block namespace */
} else {
    block_var = "other";  /* Different block namespace */
}
```

### Grapa's Destructuring Assignment

Grapa provides a powerful destructuring system using namespace concatenation with the `++=` operator. This is more flexible and powerful than traditional destructuring syntax.

#### Object Destructuring
```grapa
/* Destructure object properties into local namespace */
person = {name:"Alice", age:30, city:"New York"};
@local ++= person;
name.echo();    /* "Alice" */
age.echo();     /* 30 */
city.echo();    /* "New York" */

/* Destructure into global namespace */
@global ++= {debug:true, timeout:5000};
debug.echo();   /* true */
timeout.echo(); /* 5000 */
```

#### Array Destructuring
```grapa
/* Destructure array elements with custom variable names */
arr = [1, 2, 3, 4, 5];
@local ++= {first:arr[0], second:arr[1], rest:arr.range(2, arr.len())};
first.echo();   /* 1 */
second.echo();  /* 2 */
rest.echo();    /* [3, 4, 5] */

/* Function parameter destructuring */
func = op(params) {
    @local ++= params;  /* Destructure all parameters */
    name.echo();
    age.echo();
};

func({name:"Bob", age:25});
```

#### Namespace Options
```grapa
/* Local namespace (function scope) */
@local ++= {debug:true, timeout:5000};

/* Global namespace (persistent) */
@global ++= {config:true, version:"1.0"};

/* Object namespace (current object) */
@this ++= {name:"John", age:25};

/* Custom namespace */
@my_namespace ++= {data:123, flag:false};
```

#### Duplicate Handling
```grapa
/* References access the last added in case of duplicates */
@local ++= {a:1, b:2};
@local ++= {a:10, c:3};  /* 'a' now refers to 10, not 1 */

/* Remove duplicates if needed */
@$local.unique();  /* Removes duplicate entries */
```

#### Advanced Patterns
```grapa
/* Destructure nested objects */
nested = {user:{name:"Alice", profile:{age:30, role:"admin"}}};
@local ++= nested.user.profile;
age.echo();   /* 30 */
role.echo();  /* "admin" */

/* Destructure with transformations */
data = {x:10, y:20};
@local ++= {sum:data.x + data.y, product:data.x * data.y};
sum.echo();     /* 30 */
product.echo(); /* 200 */

/* Configuration destructuring */
@global ++= {debug:true, timeout:5000, retries:3};
debug.echo();   /* true */
timeout.echo(); /* 5000 */
retries.echo(); /* 3 */
```

**Advantages of Grapa's Destructuring:**
- **Namespace Control**: Explicit control over where variables go
- **Flexible**: Can destructure into any namespace (`@local`, `@global`, `@this`)
- **Powerful**: Can destructure multiple objects into the same namespace
- **Consistent**: Uses existing `++=` operator and namespace system
- **Duplicate Resolution**: Clear behavior for handling duplicates
- **Direct Access**: Variables become directly accessible without prefix

### Variable Indirection

Grapa supports **variable indirection** using `@` and `@@` operators, which is essential for implementing true native syntax and dynamic variable access:

#### Single Indirection (`@`)

The `@` operator dereferences a variable name to get its value:

```grapa
a = 5;
b = "a";
@b;        /* Returns "a" (the value of variable b) */
```

#### Double Indirection (`@@`)

The `@@` operator performs double dereferencing - first gets the value, then treats that value as another variable name:

```grapa
a = 5;
b = "a";
@@b;       /* Returns 5 (the value of variable a) */
```

#### Dynamic Variable Access in Functions

This enables dynamic variable access and modification in functions by passing variable names as strings:

```grapa
/* Function that can access any variable by name */
f = op(var_name) {
    return @@var_name;  /* Get value of variable named in var_name */
};

/* Function that can modify any variable by name */
f2 = op(var_name, new_value) {
    @var_name = new_value;  /* Set variable named in var_name */
};

/* Usage */
x = 10;
f("x");     /* Returns 10 */
f2("x", 20); /* Sets x to 20 */
f("x");     /* Returns 20 */
```

#### Passing Variable Names as Strings

You can pass variable names as strings to functions without passing the actual variables, then use `@` to access them:

```grapa
/* Function that works with variable names passed as strings */
update_config = op(setting_name, new_value) {
    /* Access the variable whose name is in setting_name */
    @setting_name = new_value;
    ("Updated " + setting_name + " to " + new_value).echo();
};

/* Function that reads multiple settings by name */
read_settings = op(setting_names) {
    result = {};
    setting_names.range().each(op(i) {
        setting_name = setting_names[i];
        result[setting_name] = @@setting_name;  /* Get value of variable named in setting_name */
    });
    return result;
};

/* Usage */
database_host = "localhost";
database_port = 5432;
max_connections = 100;

/* Update settings by passing names as strings */
update_config("database_host", "production.db.com");
update_config("max_connections", 200);

/* Read multiple settings by passing names as strings */
settings = read_settings(["database_host", "database_port", "max_connections"]);
/* Returns: {"database_host":"production.db.com","database_port":5432,"max_connections":200} */
```

**Key Benefits:**
- **Dynamic access**: Functions can work with any variable without knowing its name at compile time
- **String-based API**: Pass variable names as strings for flexible function interfaces
- **Batch operations**: Process multiple variables by passing arrays of names
- **Configuration management**: Update settings by name without hardcoding variable references
- **Language integration**: Essential for implementing interpreters and language bridges

#### Arbitrary Variable Names (Unicode Workaround)

Grapa supports using arbitrary strings as variable names, including Unicode characters, spaces, and special characters, through the `@this["variable name"]` syntax. This serves as a **workaround for Unicode variable names** until proper Unicode support is added to the `$ID` token system:

```grapa
/* Create variables with Unicode characters, spaces, and special characters */
@this["x y"] = {a: 1, b: "hi"};
@this["user-name"] = "John Doe";
@this["config.setting"] = true;
@this["π"] = 3.14159;
@this["用户设置"] = {"theme": "dark", "language": "中文"};
@this["café"] = "coffee shop";

/* Access these variables */
@"x y";                    /* Returns {"a":1,"b":"hi"} */
@"user-name";              /* Returns "John Doe" */
@"config.setting";         /* Returns true */
@"π";                      /* Returns 3.14159 */
@"用户设置";                /* Returns {"theme":"dark","language":"中文"} */
@"café";                   /* Returns "coffee shop" */

/* Modify properties of these variables */
(@"x y").a = 5;            /* Sets a to 5 */
@"用户设置".theme = "light"; /* Sets theme to "light" */
@"π" = 3.14;               /* Updates π to 3.14 */

/* String-based property access and modification */
@"x y"["b"] = "xy";        /* Bracket notation with string literal */
@"x y".b = "55";           /* Dot notation */
@"x y"."b" = 101;          /* Dot notation with string literal */
```
```

**Key Benefits:**
- **Unicode support**: Use Unicode characters in variable names (π, 用户设置, café, etc.)
- **Flexible naming**: Use spaces, hyphens, dots, and other characters in variable names
- **Dynamic access**: Access variables using string literals with `@"variable name"`
- **Property modification**: Modify properties of these variables using standard dot notation
- **Integration with indirection**: Works seamlessly with `@` and `@@` operators

**Use Cases:**
- **International development**: Use native language variable names (用户设置, настроики, etc.)
- **Mathematical programming**: Use mathematical symbols (π, θ, Σ, etc.)
- **Configuration management**: Use descriptive names like `@"database.connection.timeout"`
- **User data**: Store user preferences with natural names like `@"user preferences"`
- **API responses**: Map external data with original field names
- **Language integration**: Support variable names from other languages that don't follow Grapa's `$ID` rules

**Note**: This is a **temporary workaround** for Unicode variable names. Future versions of Grapa will support Unicode characters directly in `$ID` tokens, making this syntax unnecessary for simple Unicode variable names.

#### String-Based Property Access

When accessing variables through indirection, you can use various syntaxes for property access and modification:

```grapa
/* Create a variable with properties */
@this["x y"] = {a: "xy", b: "xy"};

/* Different ways to access and modify properties */
@"x y"["b"] = "xy";        /* Bracket notation with string literal */
@"x y".b = "55";           /* Dot notation with identifier */
@"x y"."b" = 101;          /* Dot notation with string literal */

/* All three syntaxes work for both reading and writing */
value1 = @"x y"["b"];      /* Bracket notation */
value2 = @"x y".b;         /* Dot notation with identifier */
value3 = @"x y"."b";       /* Dot notation with string literal */

/* Check the result */
@"x y";                    /* Returns {"a":"xy","b":101} */
```

**Property Access Syntax Options:**
- **Bracket notation**: `@"variable name"["property"]` - Most flexible, supports any string
- **Dot notation with identifier**: `@"variable name".property` - Clean syntax for simple property names
- **Dot notation with string literal**: `@"variable name"."property"` - Combines dot notation with string literals

**Benefits:**
- **Flexible property names**: Use bracket notation for properties with spaces or special characters
- **Consistent syntax**: All three forms work for both reading and writing
- **String-based access**: Property names can be stored in variables and used dynamically
- **Unicode support**: Property names can include Unicode characters

#### Importance for Native Syntax

Variable indirection is crucial for implementing **true native syntax** because:

- **Grammar tokens**: When the grammar system captures variable names as `$STR` tokens, you can use `@@` to access the actual variable
- **Dynamic evaluation**: Functions can work with variable names passed as parameters
- **Symbol tables**: Enables implementation of symbol tables and environments
- **Language integration**: Essential for implementing languages like LISP, PROLOG, and SQL that need dynamic variable access
- **Arbitrary names**: Support for variable names that don't conform to standard identifier rules

This feature enables Grapa's unique capability to add **true native syntax** for other programming languages through its rule-based grammar system.

### Const Protection

Grapa provides runtime const protection that prevents variable modification:

```grapa
/* Create const variable */
config = const {"debug": true, "port": 3000};

/* Cannot modify (will error) */
config.port = 8080;  /* Error: cannot modify const */

/* Can toggle const protection */
config.setconst(false);
config.port = 8080;  /* Now works */
config.setconst(true);  /* Make const again */

/* Const arrays */
colors = const ["red", "green", "blue"];
colors += "yellow";  /* Error: cannot modify const */

/* Performance optimization */
static_config = static {"api_url": "https://api.example.com"};
```

**Benefits of Grapa's const:**
- **Runtime protection**: Prevents actual modification at runtime
- **Dynamic control**: Can toggle const on/off as needed
- **Performance**: Optimized for caching and concurrent access
- **Thread safety**: Prevents blocking on access for concurrent threads

### Class System

Grapa provides a full object-oriented programming system with classes, inheritance, and polymorphism:

#### Basic Class Definition
```grapa
/* Define a class */
Person = class {
    name = "";
    age = 0;
    city = "";
    
    /* Manual initialization method - for classes without $new() constructor */
    init = op(n, a, c) {
        name = n;
        age = a;
        city = c;
    };
    
    /* Method */
    getInfo = op() {
        ("Name: " + name + ", Age: " + age.str() + ", City: " + city).echo();
    };
};

/* Create instance */
person = obj Person;
person.init("John", 30, "NYC");
person.getInfo();
```

#### Class Constructor with $new()

Grapa supports a single `$new()` constructor in class definitions that is automatically called when creating instances:

```grapa
/* Define a class with $new() constructor */
MyClass = class {
    n = 0;
    
    /* Constructor - automatically called when creating instances */
    $new = op(g:-1) {
        n = g;
    };
};

/* Create instances - constructor is called automatically */
x = MyClass();      /* n = -1 (default value) */
y = MyClass(4);     /* n = 4 */

x.n.echo();         /* Output: -1 */
y.n.echo();         /* Output: 4 */
```

**Key Points:**
- **Single Constructor**: Only one `$new()` method is supported per class
- **Automatic Invocation**: The constructor is called automatically when creating instances
- **Parameter Support**: The constructor can accept parameters with default values
- **Instance Creation**: Use `Class()` syntax instead of `obj Class` when using `$new()`

> **Important Note on Constructors and Destructors:**
> 
> **Constructors**: Grapa supports automatic constructors through the `$new()` method. When a class defines a `$new()` method, it is automatically called when creating instances using `Class()` syntax. For classes without `$new()`, you can use the manual `init()` method approach with `obj Class`.
> 
> **Destructors**: Grapa objects do **not** have destructors. Memory management is handled automatically by Grapa's garbage collector. There are no `~ClassName()` or `finalize()` methods.
> 
> **Current Behavior**: 
> - **With `$new()`**: Use `Class()` syntax for automatic constructor invocation
> - **Without `$new()`**: Use `obj Class` and manually call initialization methods like `init()`

#### Inheritance
```grapa
/* Base class */
Animal = class {
    name = "";
    speak = op() { ("Some sound").echo(); };
};

/* Derived class */
Dog = class (Animal) {
    breed = "";
    /* Override method */
speak = op() { ("Woof!").echo(); };
    fetch = op() { ("Fetching ball").echo(); };
};

/* Usage */
myDog = obj Dog;
myDog.name = "Buddy";
myDog.speak();  /* Output: Woof! */
```

#### Polymorphism
```grapa
/* Different animal types */
animals = [obj Dog, obj Animal];
i = 0;
while (i < animals.len()) {
    animals[i].speak();  /* Calls appropriate method */
    i += 1;
}
```

### Inheritance and Composition

Grapa supports full object-oriented programming features:

- **Inheritance**: Use `class (ParentClass)` to create subclasses
- **Method overriding**: Subclasses can override parent methods
- **Method sets**: Classes have sets of methods, similar to Go/Java
- **Composition**: Classes and objects can include other objects as properties
- **Function chaining**: Class methods support dot notation chaining (see [Function Chaining](#function-chaining))

#### Example
```grapa
Animal = class {
    speak = op() { "Some sound".echo(); };
};
Dog = class (Animal) {
    /* Method override */
    speak = op() { "Woof!".echo(); };
    /* New method */
    fetch = op() { "Fetching...".echo(); };
};
myDog = obj Dog;
myDog.speak();  /* Outputs: Woof! */
```

## String Operations

### String Methods

```grapa
/* Convert to string */
text = value.str();

/* String concatenation */
full_name = first_name + " " + last_name;

/* String with variables */
message = ("Hello " + name + ", you are " + age.str() + " years old");
```

### String Concatenation

```grapa
/* Both forms are valid, but += is preferred */
message = message + " world";
message += "!";
```

> **See Also:** [String Templates and Dynamic Construction](../type/str.md#string-templates-and-dynamic-construction) for advanced string construction patterns using templates, dynamic execution, and parameterized functions.

### String Literals

Always use double quotes for string literals:

```grapa
/* ✅ Correct */
name = "John Doe";
message = "Hello World";

/* ❌ Incorrect */
name = John Doe;
message = Hello World;
```

## Array and Object Access

### Array Access

Arrays (`$ARRAY`) support only bracket notation:

```grapa
arr = ["a", "b", "c"];
value = arr[1];        /* Returns "b" */
```

- Use bracket notation for `$ARRAY`.
- `.get()` is not supported for `$ARRAY`.

### Array Concatenation

Use the `+=` operator to concatenate arrays:

```grapa
arr1 = ["a", "b"];
arr2 = ["c", "d"];
arr1 += arr2;          /* arr1 is now ["a", "b", "c", "d"] */
```

- The `+=` operator appends all elements from the right array to the left array.
- This is simpler than using `.reduce()` or other functional methods.
- No spread operator (`...`) is needed or supported.

### Array Slicing with .range()

Use `.range()` for array slicing operations:

```grapa
arr = ["a", "b", "c", "d", "e"];

/* Get elements from index 1 to end */
slice1 = arr.mid(1, 4);  /* ["b", "c", "d", "e"] */

/* Get first 3 elements */
slice2 = arr.mid(0, 3);  /* ["a", "b", "c"] */

/* Get elements from index 1 to 3 */
slice3 = arr.mid(1, 2);  /* ["b", "c"] */

/* Alternative using .range() for sequence generation */
range1 = arr.range(1, 1);  /* [1, 2, 3, 4] - generates sequence, not array slice */
range2 = arr.range(0, 1);  /* [0, 1, 2] - generates sequence, not array slice */
range3 = arr.range(1, 1);  /* [1, 2] - generates sequence, not array slice */
```

- Use `.mid(start, count)` for array slicing (recommended)
- Use `.left(count)` and `.right(count)` for extracting from the beginning or end
- Use `.range(start, step)` for sequence generation (not array slicing)
- This replaces slice notation like `arr[1:]`, `arr[:3]`, `arr[1:3]`
- The count parameter in `.mid()` specifies how many elements to return
- Note: `.range()` generates new sequences, while `.mid()`, `.left()`, and `.right()` extract from existing arrays
- Note: `.left()`, `.right()`, and `.mid()` also work with `$LIST` types

### Array Methods

Grapa provides several useful array methods:

#### Array Comprehension Alternatives

Grapa provides functional methods that accomplish the same results as array comprehensions in other languages, with the added benefit of explicit threading control:

```grapa
/* Python: [x for x in range(10) if x % 2 == 0] */
evens = (10).range().filter(op(x){x%2==0});  // [0,2,4,6,8]

/* Python: [x * 2 for x in range(10) if x % 2 == 0] */
doubled = (10).range().filter(op(x){x%2==0}).map(op(x){x*2});  // [0,4,8,12,16]

/* With threading for performance */
large_result = (10000).range()
    .filter(op(x){x%2==0}, 8)      // 8 threads for filtering
    .map(op(x){x*x}, 8);           // 8 threads for mapping
```

**Key advantages:**
- **Explicit threading control** for performance optimization
- **Functional programming style** with chainable operations
- **Better performance** for large datasets
- **Memory efficient** processing

See [Array Comprehension Alternatives](../examples/array_comprehension_alternatives.grc) for comprehensive examples.

```grapa
/* Basic array operations */
arr = [3, 1, 4, 1, 5, 9, 2, 6];

/* Sort array */
sorted_arr = arr.sort();  /* [1, 1, 2, 3, 4, 5, 6, 9] */

/* Get array length */
length = arr.len();  /* 8 */

/* Join array elements into string */
words = ["hello", "world", "test"];
sentence = words.join(" ");  /* "hello world test" */

/* Split string into array */
text = "hello,world,test";
parts = text.split(",");  /* ["hello", "world", "test"] */
```

**Important Notes:**
- Use `.sort()` for deterministic test output when order is not guaranteed
- `.len()` returns the number of elements in an array
- `.join(delimiter)` combines array elements with the specified delimiter
- `.split(delimiter)` splits a string into an array using the delimiter

### List/Object Access

Lists (`$LIST`) and objects (`$OBJ`) support bracket and dot notation:

```grapa
obj = {"a": 11, "b": 22, "c": 33};
value = obj["b"];          /* Returns 22 (key access) */
value = obj.key;            /* Returns value for key 'key' if present */
value = obj."b";           /* Returns 22 (key access) */

/* $LIST supports both key and index access: */
value = obj[1];             /* Returns 22 (index access) */
value = obj["a"];           /* Returns 11 (key access) */
value = obj.a;              /* Returns 11 (dot notation key access) */
name = obj.getname(1);      /* Returns "b" (key name) */

/* ⚠️ IMPORTANT: Hyphens in property names require quotes */
/* The hyphen (-) is interpreted as a subtraction operator */
config = {ab:3, "a-b":4, a-b:5};  /* a-b becomes 5 (math operation) */
config.ab;     /* 3 */
config["a-b"]; /* 4 (correct) */
config."a-b";  /* 4 (also correct) */
/* config.a-b;  ❌ This would be interpreted as config.a - b (subtraction) */
```

- Use bracket and dot notation for `$LIST` and `$OBJ`.
- $LIST supports both key access (obj["key"], obj.key) and index access (obj[2]).
- `.get()` is not supported for `$LIST` or `$OBJ`.

### Duplicate Key Resolution

When objects have duplicate keys, Grapa follows "last value wins" behavior:

```grapa
obj = {name: "Alice", age: 30, name: "Bob"};
obj.name;                  /* Returns "Bob" (last value) */

obj2 = {a: 1, b: 2, a: 3, c: 4, b: 5};
obj2.a;                    /* Returns 3 (last value) */
obj2.b;                    /* Returns 5 (last value) */
obj2.c;                    /* Returns 4 (only value) */
```

- **Search Order**: Property resolution searches from the end of the object (tail) to the beginning (head)
- **Last Value Wins**: When duplicate keys exist, the last occurrence takes precedence
- **JSON Compatibility**: This matches the behavior of JSON in other programming languages
- **Index Access Unchanged**: Array-style indexing (`obj[0]`, `obj[1]`) is unaffected and still uses position-based access

### Property Assignment with Duplicate Keys

Property assignment also targets the last occurrence of duplicate keys:

```grapa
lst = {a: 1, b: 2, a: 3};
lst.a;                     /* Returns 3 (last value) */
lst.a = 6;                 /* Assigns to last occurrence of 'a' */
lst;                       /* Returns {"a":1, "b":2, "a":6} */
lst.a;                     /* Returns 6 (updated last value) */
```

- **Consistent Behavior**: Both property access and assignment work on the same key occurrence
- **Predictable**: Reading and writing the same property affects the same key
- **No Surprises**: Users can expect consistent behavior for duplicate keys

### .sort() and .unique() Functions

Both `.sort()` and `.unique()` support a comprehensive set of data types:

**Supported Data Types:**
- `$ARRAY` - Arrays: `[1, 2, 3]`
- `$TUPLE` - Tuples: `(1, 2, 3)`
- `$LIST` - Lists: `{a:1, b:2, c:3}`
- `$OBJ` - Objects: `{a:1, b:2, c:3}`
- `$XML` - XML structures: `<root><item>value</item></root>`
- `$TAG` - HTML/TAG structures: `<div><p>text</p></div>`
- `$EL` - Element structures
- `$OP` - Operator objects
- `$CODE` - Code objects

#### .sort() Function

Sorts elements in ascending order by default:

```grapa
/* Arrays and Tuples - sort by value */
[3, 1, 4, 1, 5, 9].sort();                /* Returns [1, 1, 3, 4, 5, 9] */
(3, 1, 4, 1, 5, 9).sort();                /* Returns (1, 1, 3, 4, 5, 9) */

/* Lists and Objects - sort by key */
{c:3, a:1, b:2}.sort();                   /* Returns {"a":1, "b":2, "c":3} */
```

**Advanced Sorting:**
```grapa
/* Custom comparison function */
[3, 1, 4, 1, 5, 9].sort(op(a,b){@a<=>@b;});  /* Custom sort logic */

/* Reverse sort */
[3, 1, 4, 1, 5, 9].sort(0, 1);            /* Descending order */
```

#### .unique() Function

Removes duplicates while preserving the "last value wins" principle:

**For Objects (`$OBJ`) and Lists (`$LIST`):**
```grapa
{a: 1, b: 2, a: 3, c: 4, b: 5}.unique();  /* Returns {"a": 3, "b": 5, "c": 4} */
```

- Removes duplicate keys, keeping the last value for each key
- Consistent with property access behavior

**For Arrays (`$ARRAY`) and Tuples (`$TUPLE`):**
```grapa
[1, 2, 1, 3, 2, 4].unique();              /* Returns [1, 2, 3, 4] */
['a', 1, 'a', 2, 1, 'b'].unique();        /* Returns [1, 2, "a", "b"] */
```

- Removes duplicate values, keeping the last occurrence of each value
- Works with mixed data types

**For XML and TAG structures:**
```grapa
<root><item>a</item><item>b</item><item>a</item></root>.unique();
/* Removes duplicate elements, keeping last occurrence */
```

### Function Chaining Limitations and Solutions

Grapa has limitations on chaining methods across different object types, but provides elegant solutions:

**Limitation:**
```grapa
/* ❌ Doesn't work - can't chain different object types */
"Hello".upper() + " World".lower().echo();  /* Error: can't chain + after .upper() */
```

**Solution - String Interpolation:**
```grapa
/* ✅ Works - use string interpolation for complex chaining */
"${'Hello'.upper()} ${'World'.lower()}".interpolate().echo();  /* "HELLO world" */

/* ✅ For complex operations, use intermediate variables */
filtered = [1,2,3,4,5].filter(op(x){x>2});
mapped = filtered.map(op(x){x*2});
"Result: ${mapped.str()}".interpolate().echo();  /* "Result: [6,8,10]" */
```

**Key Points:**
- **String interpolation** provides elegant solution for complex method chaining
- **Intermediate variables** help break down complex operations
- **`.interpolate()`** supports any valid Grapa expression inside `${}`

### $file and $TABLE Access

For `$file` and `$TABLE` objects, always use `.getfield()` and `.setfield()` methods:

```grapa
files = $file().ls();
file_info = files.getfield(0);   /* Correct */

table = {}.table("ROW");
table.mkfield("name", "STR", "VAR");
table.setfield("user1", "Alice", "name");
value = table.getfield("user1", "name");   /* Correct */
```

- Always use `.getfield()` and `.setfield()` for `$file` and `$TABLE`.
- Bracket and dot notation are not valid for `$file` and `$TABLE`.

### Type System Philosophy

Grapa uses **dynamic typing** as a fundamental design choice, not a limitation. This enables Grapa's core strengths:

### Dynamic Type System Benefits
- **Runtime flexibility**: Types are determined at execution time
- **Dynamic code execution**: Enables meta-programming and code generation
- **System integration**: Works with data of unknown types
- **Data processing**: Ideal for ETL and analysis tasks
- **Advanced functional programming**: Enables complex context objects and state machines
- **Implementation-driven types**: Types emerge from usage patterns, not predefined constraints

### Type Safety Through Runtime Checking
Grapa provides robust type safety through runtime checking and introspection:

```grapa
/* Check object types */
element = ["a", "b", "c"];
element.type();        /* Returns $ARRAY */

obj = {"a": 11, "b": 22, "c": 33};
obj.type();            /* Returns $LIST */

table = {}.table("ROW");
table.type();          /* Returns $TABLE */

/* Runtime type checking */
if (value.type() == $INT) {
    ("Value is an integer").echo();
} else if (value.type() == $STR) {
    ("Value is a string").echo();
}
```

### Type Conversion
Grapa provides explicit type conversion methods:

```grapa
/* String conversion */
value = 42;
text = value.str();    /* Convert to string */

/* Numeric conversion */
text = "123";
number = text.int();   /* Convert to integer */
decimal = text.float(); /* Convert to float */

/* Type checking before conversion */
if (input.type() == $STR) {
    result = input.int();
} else {
    result = input;    /* Already numeric */
}
```

## Function Calls

### Method Calls

```grapa
/* File operations */
fs.setfield("file.txt", "content");
content = fs.getfield("file.txt");

/* Table operations */
table.mkfield("name", "STR", "VAR");
table.setfield("key", "value", "field");
value = table.getfield("key", "field");
```

### Function Definitions with Default Parameters

```grapa
/* Function with default parameters */
f = op("x"=1, "y"=0) { x + y; };

/* Call with all parameters */
result1 = f(5, 3);  /* 8 */

/* Call with default for y */
result2 = f(5);     /* 6 (y defaults to 0) */

/* Call with defaults for both */
result3 = f();      /* 1 (x defaults to 1, y defaults to 0) */
```

- Use `op("param"=default_value)` for default parameters
- Parameters with defaults can be omitted when calling
- Parameters without defaults must be provided

### System Functions

```grapa
/* Time functions */
current_time = $TIME().utc();

/* File functions */
file_obj = $file();
table_obj = {}.table("ROW");
```

## Comments

Grapa supports both block comments and line comments for comprehensive code documentation:

### Comment Types

#### Block Comments
- **Standard block comments**: `/* ... */` - Multi-line comments
- **Documentation block comments**: `/** ... */` - Multi-line documentation comments

#### Line Comments  
- **Standard line comments**: `// ...` - Single-line comments terminated by newline
- **Documentation line comments**: `/// ...` - Single-line documentation comments terminated by newline

### Comment Rules

#### General Usage
All comment types can be used throughout your code:

```grapa
/* This is a standard block comment */
/** This is a documentation block comment */

// This is a standard line comment
/// This is a documentation line comment

x = 5; // Comments can be at end of lines
y = 10; /// Documentation comments too
```

#### Inside Code Blocks (`{}`)
Comments work the same way inside code blocks:

```grapa
/* ✅ All comment types work in code blocks */
if (condition) {
    /* Standard block comment */
    /** Documentation block comment */
    x = 5; // Standard line comment
    y = 10; /// Documentation line comment
}
```

#### Class and Function Definitions
All comment types work in class and function definitions:

```grapa
/* ✅ All comment types work in class definitions */
MyClass = class {
    /* Standard block comment */
    /** Documentation block comment */
    myMethod = op() {
        // Standard line comment
        /// Documentation line comment
        "Hello".echo(); // Comments at end of lines work too
    };
};
```

> **Note:** All comment types (`/* */`, `/** */`, `//`, `///`) are fully supported throughout Grapa code, including inside class definitions, function bodies, and at the end of lines. **`#` comments are not supported** and will cause syntax errors.

## Interactive Mode

When running Grapa in interactive mode (`grapa -i`), you can enter commands line by line. The interactive console provides special features for multi-line input:

### Line Continuation with Backslash

Use `\` (backslash) at the end of a line to continue the command on the next line:

```grapa
grapa: />a = \
5

grapa: />a
5
```

This allows you to enter multi-line commands without executing each line separately.

### Interactive Mode Commands

- **Exit interactive mode**: Enter `.` (single dot) on a line by itself
- **Line continuation**: Use `\` at the end of a line to continue on the next line
- **Multi-line blocks**: Use `$[` and `$]` for complex multi-line code blocks

### Example Interactive Session

```grapa
grapa: />name = "John"
grapa: />age = 25
grapa: />("Hello " + name + ", you are " + age + " years old").echo()
Hello John, you are 25 years old
grapa: />.
```

## Control Structures

### If Statements

```grapa
if (condition) {
    ("Condition is true").echo();
} else {
    ("Condition is false").echo();
}

/* Example with comparison */
if (age >= 18) {
    ("User is an adult").echo();
} else {
    ("User is a minor").echo();
}
```

### Truthy and Falsy Values

Grapa supports implicit truthy/falsy evaluation in conditions, similar to modern languages like Python and JavaScript:

#### Comprehensive Truthiness Chart

The following table shows all possible values and their truthiness evaluation in Grapa:

| Value Type | Example | Truthiness | `if (value)` Result | Notes |
|------------|---------|------------|-------------------|-------|
| **Boolean** | `true` | Truthy | Executes | Always truthy |
| **Boolean** | `false` | Falsy | Skips | Always falsy |
| **Integer** | `1`, `42`, `-1`, `-100` | Truthy | Executes | Any non-zero number |
| **Integer** | `0` | Falsy | Skips | Zero is falsy |
| **Float** | `1.5`, `-2.7`, `0.1` | Truthy | Executes | Any non-zero number |
| **Float** | `0.0` | Falsy | Skips | Zero is falsy |
| **String** | `"hello"`, `"0"`, `"false"` | Truthy | Executes | Any non-empty string |
| **String** | `""` | Falsy | Skips | Empty string is falsy |
| **Array** | `[1,2,3]`, `["a","b"]` | Truthy | Executes | Non-empty arrays |
| **Array** | `[]` | Falsy | Skips | Empty array is falsy |
| **List** | `{a:1, b:2}`, `{1,2,3}` | Truthy | Executes | Non-empty lists |
| **List** | `{}` | Falsy | Skips | Empty list is falsy |
| **Vector** | `#[1,2,3]#` | Truthy | Executes | Non-empty vectors |
| **Vector** | `#[]#` | Falsy | Skips | Empty vector is falsy |
| **Null** | `null` | Falsy | Skips | Null is always falsy |
| **Error** | `$ERR()`, `9/0`, `undefined_property` | Falsy | Skips | Error objects are falsy |
| **Function** | `some_function` | Truthy | Executes | Functions are truthy |
| **File** | `$file()` | Truthy | Executes | File objects are truthy |
| **Network** | `$net()` | Truthy | Executes | Network objects are truthy |
| **System** | `$sys()` | Truthy | Executes | System objects are truthy |

#### Truthy Values
```grapa
/* These values are considered truthy in conditions */
if (true) { /* Always truthy */ }
if (1) { /* Non-zero numbers */ }
if (-1) { /* Negative numbers */ }
if (3.14) { /* Non-zero floats */ }
if ("hello") { /* Non-empty strings */ }
if ("0") { /* String "0" is truthy (non-empty) */ }
if ("false") { /* String "false" is truthy (non-empty) */ }
if ([1, 2, 3]) { /* Non-empty arrays */ }
if ({a: 1, b: 2}) { /* Non-empty lists */ }
if (#[1,2,3]#) { /* Non-empty vectors */ }
if (some_function) { /* Functions */ }
if ($file()) { /* File objects */ }
if ($net()) { /* Network objects */ }
if ($sys()) { /* System objects */ }
```

#### Falsy Values
```grapa
/* These values are considered falsy in conditions */
if (false) { /* Always falsy */ }
if (0) { /* Zero */ }
if (0.0) { /* Zero float */ }
if ("") { /* Empty strings */ }
if ([]) { /* Empty arrays */ }
if ({}) { /* Empty lists */ }
if (#[]#) { /* Empty vectors */ }
if (null) { /* Null values */ }
if ($ERR()) { /* Error objects */ }
if (9/0) { /* Division by zero creates error */ }
if (undefined_property) { /* Undefined properties create errors */ }
```

#### Comparison Behavior Chart

The following table shows how different values compare to each other:

| Comparison | Result | Explanation |
|------------|--------|-------------|
| `0 == false` | `true` | Both are falsy values |
| `1 == true` | `false` | Different types, different values |
| `"" == false` | `true` | Both are falsy values |
| `"0" == 0` | `true` | String "0" converts to number 0 |
| `"1" == 1` | `true` | String "1" converts to number 1 |
| `"hello" == 0` | `false` | Non-numeric string doesn't equal 0 |
| `"hello" == 1` | `false` | Non-numeric string doesn't equal 1 |
| `null == false` | `true` | Both are falsy values |
| `null == 0` | `false` | Null and 0 are different types |
| `$ERR() == false` | `true` | Both are falsy values |
| `$ERR() == null` | `true` | Error objects are null-like |
| `0 == $ERR()` | `false` | Different types |

#### Practical Examples
```grapa
/* Check if user input exists */
if (user_input) {
    ("Processing: " + user_input).echo();
} else {
    ("No input provided").echo();
}

/* Check if array has items */
if (items) {
    ("Processing " + items.len() + " items").echo();
} else {
    ("No items to process").echo();
}

/* Check if configuration exists */
if (config) {
    ("Using configuration").echo();
} else {
    ("Using default settings").echo();
}

/* Check for errors */
if (result.type() == $ERR) {
    ("Error occurred: " + result).echo();
} else {
    ("Success: " + result).echo();
}

/* Ternary operator with truthy/falsy */
status = user_input ? "active" : "inactive";
message = items ? "Items found" : "No items";
error_msg = result.type() == $ERR ? "Failed" : "Success";
```

#### Error Handling Best Practices

```grapa
/* Correct way to check for errors */
result = some_operation();
if (result.type() == $ERR) {
    ("Error: " + result).echo();
} else {
    ("Success: " + result).echo();
}

/* Alternative: Check if result is truthy (not an error) */
result = some_operation();
if (result) {
    ("Success: " + result).echo();
} else {
    ("Error occurred").echo();
}

/* Check for specific error conditions */
if (result.type() == $ERR && result == "division by zero") {
    ("Cannot divide by zero").echo();
}
```

### Comparison Operators

```grapa
/* Equality */
if (name == "John") { }

/* Inequality */
if (age != 0) { }

/* Greater/Less than */
if (salary > 50000) { }
if (age < 18) { }

/* Greater/Less than or equal */
if (count >= 10) { }
if (value <= 100) { }

/* Data Structure Comparisons */
/* Arrays - element-by-element comparison */
if ([1,2,3] == [1,2,3]) { }  // true
if ([1,2] == [1,2,3]) { }    // false (different lengths)
if ([1,2,3] == [3,2,1]) { }  // false (different order)

/* Lists - element-by-element comparison */
if ({1,2,3} == {1,2,3}) { }  // true
if ({a:1, b:2} == {a:1, b:2}) { }  // true
if ({a:1, b:2} == {b:2, a:1}) { }  // false (order matters)

/* Vectors - structure comparison */
if (#[1,2,3]# == #[1,2,3]#) { }  // true
if (#[1,2]# == #[1,2,3]#) { }    // false (different lengths)

/* Mixed types are never equal */
if ([1,2,3] == {1,2,3}) { }  // false (array vs list)
if ([1,2,3] == #[1,2,3]#) { } // false (array vs vector)
```

### Switch Statements

Grapa provides switch statements for multiple condition checking:

#### Switch Statement Syntax

**Single command in case:**
```grapa
switch (x) {
    case 10: "x is 10".echo();
    case 5: "x is 5".echo();
    default: "x is something else".echo();
};
```

**Multiple commands in case (must use {}):**
```grapa
switch (x) {
    case 10: {
        "x is 10".echo();
        "Additional handling".echo();
    }
    case 5: {
        "x is 5".echo();
        "More handling".echo();
    }
    default: {
        "x is something else".echo();
        "Default handling".echo();
    }
};
```

#### Type Switching
```grapa
/* Switch based on type */
switch (value.type()) {
    case $STR: "String value".echo();
    case $INT: "Integer value".echo();
    case $ARRAY: "Array value".echo();
    case $LIST: "List value".echo();
    default: "Unknown type".echo();
};
```

#### Value Switching
```grapa
/* Switch based on value */
switch (status) {
    case "active": "User is active".echo();
    case "inactive": "User is inactive".echo();
    case "pending": "User is pending".echo();
    default: "Unknown status".echo();
};
```

#### Boolean Switching
```grapa
/* Switch based on boolean conditions */
switch(true) {
    case (score >= 90): "Grade: A".echo();
    case (score >= 80): "Grade: B".echo();
    case (score >= 70): "Grade: C".echo();
    case (score >= 60): "Grade: D".echo();
    default: "Grade: F".echo();
};
```

#### Switch with Complex Conditions
```grapa
/* Switch with complex expressions */
switch(true) {
    case (age < 18 && income < 30000): "Young and low income".echo();
    case (age >= 65): "Senior citizen".echo();
    case (income > 100000): "High income".echo();
    default: "Standard category".echo();
};
```

#### Variable Declaration in Switch Expression

Grapa's switch statement can declare and initialize variables directly in the switch expression, enabling advanced pattern matching and fuzzy matching:

```grapa
/* Declare variable in switch expression for advanced matching */
switch(a:[1,3,5]) {
    case (a[2]==5): "Array element matches".echo();
    default: "No match found".echo();
};
```

**Advanced Pattern Matching Examples:**

```grapa
/* Fuzzy string matching */
switch(input:"hello world") {
    case (input.grep("hello", "i")): "Contains 'hello' (case insensitive)".echo();
    case (input.len() > 10): "Long string".echo();
    default: "No pattern match".echo();
};

/* Complex data structure analysis */
switch(data:{name: "Alice", scores: [85, 92, 78]}) {
    case (data.scores.avg() >= 90): "High performer".echo();
    case (data.scores.avg() >= 80): "Good performer".echo();
    case (data.scores.min() < 70): "Needs improvement".echo();
    default: "Average performer".echo();
};

/* Array pattern matching */
switch(arr:[1,2,3,4,5]) {
    case (arr.len() == 5 && arr[0] == 1): "Five-element array starting with 1".echo();
    case (arr.sum() > 10): "Array sum greater than 10".echo();
    case (arr.grep(3)): "Array contains 3".echo();
    default: "No specific pattern".echo();
};
```

**Key Features:**
- **Implicit break**: No break statements required, execution stops after first match
- **Multiple commands**: Must be enclosed in `{}` when case contains multiple statements
- **Sequential evaluation**: Evaluates from first to last case, stops at first match
- **No fall-through**: Unlike C-style switches, Grapa doesn't fall through to next case
- **Variable declaration**: Can declare variables in switch expression for advanced matching
- **Local scope**: Variables are only available within the switch statement
- **Advanced matching**: Enables fuzzy matching, pattern recognition, and complex conditions

## Exception Handling

Grapa provides comprehensive exception handling with try/catch/finally blocks and throw statements.

### Basic Exception Handling
```grapa
/* Basic try/catch with finally */
try {
    throw 'error message';
} {
    catch 'error message': 'Caught specific error'.echo();
    default: 'Caught any other error'.echo();
} finally: 'Always executed'.echo();
```

### Exception with Variable Binding
```grapa
/* Throw with variable binding for detailed error handling */
try {
    throw (err:'error message');
} {
    catch (err.left(1)=='e'): 'Error starts with e: '.interpolate({msg=err}).echo();
    catch (err.len() > 10): 'Long error message'.echo();
    default: 'Caught other error'.echo();
} finally: 'Cleanup completed'.echo();
```

### Exception Handling Syntax

**Throw statements:**
```grapa
throw 'error message';                    // Simple throw
throw (err:'error message');              // With variable binding
```

**Try/catch/finally:**
```grapa
try {
    // Code that might throw
} {
    catch 'error': 'Handle specific error'.echo();
    catch (condition): 'Handle based on condition'.echo();
    default: 'Handle any other error'.echo();
} finally: 'Always execute'.echo();
```

**Key Features:**
- **Variable binding**: Variables declared in throw are accessible in catch blocks
- **Flexible matching**: Catch conditions can be constants, expressions, or complex patterns
- **Multiple catch blocks**: Can have multiple catch conditions
- **Finally block**: Always executes regardless of exception
- **Consistent syntax**: Follows the same pattern as switch/case statements
- **No parentheses required**: Simple constants don't need parentheses

## Unsupported Syntax Operators

Grapa does not support several modern JavaScript/TypeScript operators. Use explicit alternatives:

### Logical Assignment Operators

```grapa
/* ❌ Not supported */
x ||= y;    /* Logical OR assignment */
x &&= y;    /* Logical AND assignment */

/* ✅ Use explicit assignment */
/* For ||= */
if (!x) { x = y; }
/* For &&= */
if (x) { x = y; }
```

### Nullish Coalescing

```grapa
/* ❌ Not supported */
x ?? y;     /* Nullish coalescing */

/* ✅ Use explicit null check */
if (x == null) { x = y; }
```

### Optional Chaining

```grapa
/* ❌ Not supported */
obj?.prop?.sub;    /* Optional chaining */

/* ✅ Use explicit null checks */
if (obj && obj.prop) { obj.prop.sub; }
```

### Spread Operator

```grapa
/* ❌ Not supported */
[...arr1, ...arr2];    /* Spread operator */

/* ✅ Use += operator for arrays */
arr1 += arr2;          /* Array concatenation */
```

### Walrus Operator (Python)

```grapa
/* ❌ Not supported in Python */
if (x := f()) > 0:     /* Walrus operator */

/* ✅ Use separate assignment */
x = f();
if (x > 0) { ... }
```

## Error Handling

### Type Checking

```grapa
/* Check if result is an error */
if (result.type() == $ERR) {
    ("Operation failed").echo();
} else {
    ("Operation succeeded").echo();
}

/* Check for specific error responses */
if (result == {"error":-1}) {
    ("Operation returned error -1").echo();
}
```

### Error Fallback with .iferr()

Grapa provides a built-in error fallback mechanism using `.iferr()`:

```grapa
/* Simple error fallback */
result = risky_operation().iferr(0);  /* Returns 0 if operation fails */

/* Error fallback with default value */
value = (10/0).iferr(55);  /* Returns 55 since division by zero fails */

/* Array access with fallback */
months = ["Jan", "Feb", "Mar"];
value = months["FFF"].iferr(-1);  /* Returns -1 for invalid key */
```

- Use `.iferr(fallback_value)` for simple error handling
- Use `if (result.type() == $ERR)` for explicit error handling
- Use `.err()` to convert objects to `$ERR` type
- `.iferr()` is preferred for simple fallback scenarios

### Safe Property Access with .iferr()

**✅ RECOMMENDED: Use .iferr() for safe property access instead of optional chaining**

Grapa's `.iferr()` method provides superior functionality to optional chaining (`?.`) by allowing custom fallback values:

```grapa
/* Basic safe property access */
obj = {a: 4};
user = obj.user.iferr(null);  /* Returns null if user doesn't exist */

/* Chained safe property access */
profile = obj.user.iferr(null).profile.iferr("blank");  /* Returns "blank" if any step fails */

/* Deep nested property access */
nested = {level1: {level2: {level3: "deep value"}}};
value = nested.level1.level2.missing.iferr("not found");  /* Returns "not found" */

/* Array access with fallback */
arr = [1, 2, 3];
item = arr[10].iferr("out of bounds");  /* Returns "out of bounds" */

/* Method calls with fallback */
result = obj.missingMethod.iferr("no method");  /* Returns "no method" */

/* Complex nested scenario */
complex = {users: [{name: "Alice", profile: {age: 30}}]};
age = complex.users[1].name.iferr("no user");  /* Returns "no user" */
field = complex.users[0].profile.missing.iferr("no field");  /* Returns "no field" */
```

**Advantages over optional chaining (`?.`):**
- **Custom fallback values** - Specify exactly what to return on error
- **Chained fallbacks** - Different fallback values at each level
- **Works with arrays** - Handles out-of-bounds access gracefully
- **Works with methods** - Handles missing methods gracefully
- **More explicit** - You choose the error handling behavior

**Comparison with traditional null checking:**
```grapa
/* Traditional verbose approach */
if (complex.users && complex.users[0] && complex.users[0].profile && complex.users[0].profile.age) {
    age = complex.users[0].profile.age;
} else {
    age = "unknown";
}

/* .iferr() approach - much cleaner */
age = complex.users[0].profile.age.iferr("unknown");
```

### Nullish Coalescing with .ifnull()

**✅ RECOMMENDED: Use .ifnull() for nullish coalescing instead of `??` operator**

Grapa's `.ifnull()` method provides superior functionality to nullish coalescing (`??`) by treating a broader range of values as "nullish":

```grapa
/* Basic nullish coalescing */
x = null;
result = x.ifnull("default");  /* Returns "default" */

/* Number literals need parentheses */
result = (0).ifnull("default");  /* Returns "default" */
result = (42).ifnull("default"); /* Returns 42 */

/* Complex scenarios */
config = {port: 0, host: "", enabled: false};
port = config.port.ifnull(8080);      /* Returns 8080 */
host = config.host.ifnull("localhost"); /* Returns "localhost" */
enabled = config.enabled.ifnull(true);  /* Returns true */
```

**Values treated as nullish:**
- `null` values
- Empty strings (`""`)
- Zero integers (`0`)
- Zero floats (`0.0`)
- False booleans (`false`)
- Empty arrays (`[]`)
- Empty objects (`{}`)
- Error tokens (`$ERR()`, division by zero, etc.)

**Advantages over nullish coalescing (`??`):**
- **Broader nullish detection** - Treats zeros, empty collections, and errors as nullish
- **Custom fallback values** - Specify exactly what to return on nullish values
- **Works with all data types** - Not just null/undefined
- **More explicit control** - You choose what constitutes "nullish"

## Complete Example

```grapa
/* User Management System Example */

/* Create table in global namespace */
$global.users = {}.table("ROW");

/* Define table schema */
users.mkfield("name", "STR", "VAR");
users.mkfield("age", "INT");
users.mkfield("salary", "FLOAT", "FIX", 8);

/* Add user data */
users.set("user1", "John Doe", "name");
users.set("user1", 30, "age");
users.set("user1", 75000.50, "salary");

/* Retrieve and display user data */
name = users.get("user1", "name").str();
age = users.get("user1", "age").int();
salary = users.get("user1", "salary").float();

("User: " + name + ", Age: " + age.str() + ", Salary: " + salary.str()).echo();

/* Process multiple users */
i = 1;
while (i <= 5) {
    user_id = "user" + i.str();
    users.set(user_id, ("User " + i.str()), "name");
    users.set(user_id, (20 + i), "age");
    i = i + 1;
}

("Created " + i.str() + " users").echo();
```

## Common Patterns

### File Operations

```grapa
/* File system operations */
$global.fs = $file();

/* Create and write file */
fs.set("test.txt", "Hello World");
content = fs.get("test.txt").str();
("File content: " + content).echo();

/* Directory operations */
fs.mk("test_dir");
fs.cd("test_dir");
files = fs.ls();
fs.cd("..");
fs.rm("test_dir");
```

### Table Operations

```grapa
/* Table operations */
$global.table = {}.table("ROW");

/* Create fields */
table.mkfield("name", "STR", "VAR");
table.mkfield("age", "INT");
table.mkfield("value", "FLOAT", "FIX", 8);

/* Add data */
table.set("key1", "John", "name");
table.set("key1", 30, "age");
table.set("key1", 100.5, "value");

/* Retrieve data */
name = table.get("key1", "name");      /* $STR */
age = table.get("key1", "age");        /* $INT, no .int() needed */
value = table.get("key1", "value");    /* $FLOAT, no .float() needed */

/* RAW field type: can store any Grapa object (e.g., JSON, XML, $LIST, $ARRAY) */
table.mkfield("meta", "RAW");
table.set("key1", {"foo": 123, "bar": [1,2,3]}, "meta");
meta = table.get("key1", "meta");      /* Returns Grapa object automatically */
(meta.foo).echo();                      /* 123 */
(meta.bar[1]).echo();                   /* 2 */

/* Edge cases and error handling */
/* .get() returns $ERR if the key or field is missing */
missing = table.get("missing", "age");
(missing.type()).echo();                /* $ERR */

/* Type mismatch: setting INT/FLOAT to non-numeric values may yield undefined results */
table.set("user2", "not an int", "age");
("INT field with STR: " + table.get("user2", "age")).echo();

/* RAW field can store nested objects/arrays */
table.set("user3", {foo: {bar: [1,2,{baz: 99}]}, arr: [10,20]}, "meta");
nested = table.get("user3", "meta");
("Nested RAW type: " + nested.type()).echo();
("Nested RAW foo.bar[2].baz: " + nested.foo.bar[2].baz).echo();
("Nested RAW arr[1]: " + nested.arr[1]).echo();

/* Overwriting a field with a value of a different type does not change the field's declared type */
table.set("user4", 123, "age");
table.set("user4", "now a string", "age");
("INT field overwritten with STR: " + table.get("user4", "age")).echo();

/* RAW field set to null returns $SYSID (Grapa's null/empty object) */
table.set("user5", null, "meta");
("Null RAW: " + table.get("user5", "meta").type()).echo();

/* TODO: Review and ensure consistent handling of null, true, false across all field types. */

/* Delete a record (works for ROW, COL, GROUP) */
table.rm("key1");

/* List records after deletion */
("After delete: " + table.ls().join(",")).echo();
```

This guide covers the essential syntax patterns for writing correct Grapa code. Follow these patterns to avoid common syntax errors and write maintainable code. 

/*
# Grapa Basic Syntax: Key Rules

## Statement and Block Termination
- Every statement must end with a semicolon (`;`), including after the closing brace of every block (such as `if`, `else`, `while`, and function blocks).
- Example:
  while (cond) {
      ...
  };
  if (cond) {
      ...
  } else {
      ...
  };

## Array Manipulation
- To append to an array, use the `+=` operator.
  - Example:
    arr = [];
    arr += "foo";
    arr += "bar";
    /* arr is now ["foo", "bar"] */
- Do not use `.push()` or `.append()`—these are not valid in Grapa.

## Array Iteration and Sorting
- The `.sort()` function is especially useful in tests to ensure deterministic output when order is not guaranteed.
- Example:
  result = input.grep(pattern, options);
  result = result.sort();
  (result.join("\n")).echo();
*/ 

/*
# Grapa Syntax Quick Reference

## Essential Syntax Rules (Summary Table)
| Rule | Example |
|------|---------|
| Every statement ends with a semicolon (`;`) | `x = 5;` |
| Every block (after `}`) ends with a semicolon | `if (x) { ... };` |
| Use block comments (`/* */`) or line comments (`//`) | `/* comment */`, `// comment` |
| Documentation comments available (`/** */`, `///`) | `/** doc */`, `/// doc` |
| Comments work everywhere including at end of lines | `x = 5; // comment` |
| All comment types work in code blocks | `{ x = 5; // comment }` |
| Append to arrays with `+=` | `arr += "foo";` |
| No `.push()` or `.append()` |  |
| Access arrays/lists with `[index]` | `arr[0];` |
| Access object properties with dot or bracket notation | `obj.foo;`, `obj["foo"]` |
| Use `.echo()` for output | `"Hello".echo();` |
| Use `while` loops, not `for` | `while (cond) { ... };` |
| Use string interpolation (preferred) | `"${var}".interpolate().echo();` |
| Wrap string concatenations in parentheses (if needed) | `(str1 + str2).echo();` |
| Use .iferr() for safe property access | `obj.property.iferr("default")` |
| Use .ifnull() for nullish coalescing | `value.ifnull("default")` |
| Increment or append with += | `v += 1;`, `s += "x";` |
| Both = x + y and += y are valid | `v = v + 1;`, `v += 1;` |
| No logical assignment (`||=`, `&&=`) | Use explicit `if` statements |
| No nullish coalescing (`??`) | Use explicit null checks |
| No optional chaining (`?.`) | Use explicit null checks |
| No spread operator (`...`) | Use `+=` for arrays |

---
*/ 

/*
# Lexical Operators (Special Parsing Triggers)

There are several predefined lexical operators, most of which define how $ID, $INT, $FLOAT, $STR, etc, are processed and generate the corresponding tokens. There are also a few other lexical operators that will trigger special handling of the input stream. The following are two examples. Currently there is no way to define/change the lexical operators - this will come in some future version of Grapa.

## `$&`
  * Wrap the XML data in $& on either side to have Grapa parse the input as XML. These characters are special lexical triggers that modify the parsing engine token generation. The first instance turns the mode on and the second turns the mode off. Or use the encode("XML") function, which does the same thing.

## `$[` 
  * Wrap input in these characters to have the entire code block parsed in a single instance. Otherwise a '\n' or '\r' will be used to trigger parsing/execution and an error would result if the line is not valid on its own. Alternatively, put the code in a file, load the file contents, and execute the contents of the string.
*/ 

/*
# Default Grammar Rules

View a text version of the grammar rules loaded into Grapa on startup here:
Grapa Grammar

The system will first check for a match on the "start" rule, which is a global variable. If that global variable is of type $RULE, then it will become the first rule for scripts. This is an easy way to provide an override on command processing. If the "start" rule does not provide a match, then the system will evaluate using the "$start" rule.

The default rules may be subject to change.

If you modify the file, you can try it out by placing it in "lib/grapa/" under the same directory that the above command would have written the file to, and then restart Grapa. If Grapa finds this file with this name in that location, it will use that file instead of the default.

If you are familiar with YACC or BNF, following the grammar logic should be rather straightforward. Each rule is evaluated until a successful match, and all matches produce an execution plan where the code associated with each rule match is included in the execution plan. To optimize grammar resolution, rules are carefully constructed to avoid re-evaluating rules more than necessary by placing the most complex rules first and following with common rule patterns. It is also important to avoid infinite recursive situations—mostly this is avoided by not referencing the same rule as the first token to evaluate.

There are a few standard tokens that are defined—mostly the tokens provide special handling for either the lexical engine or the rules engine. One example is the space character, which when found sets a bit in the token to skip the value for rules evaluation. This can be changed—but then would require including the space token in the rules.
*/ 

/*
# Function Chaining
Any object that returns an object can be chained.

**Important**: For function chaining to work, the functions must be **class methods**. Functions defined outside of classes cannot be chained using the dot notation.

For example, "4.4".float(300,4).pi() will convert the string 4.4 to a float with 300 bit precision and will return pi() to the power of the input, at the precision specified in the input. So, a 400 bit precision of pi to the power of 4.4.

## Creating Chainable Functions

To make your own functions chainable, define them as class methods:

```grapa
/* Define a class with chainable methods */
StringProcessor = class {
    process = op(text) {
        text.trim().upper();  // Returns processed string
    };
    
    processCasefold = op(text) {
        text.trim().casefold();  // Returns case-folded string
    };
    
    reverse = op(text) {
        text.reverse();  // Returns reversed string
    };
    
    addPrefix = op(text, prefix) {
        prefix + " " + text;  // Returns string with prefix
    };
};

/* Create an instance and use chaining */
processor = obj StringProcessor;
result = processor.process("  hello world  ").reverse().addPrefix("Result:");
result.echo();  // Output: Result: DLROW OLLEH
```

## Using Built-in Chainable Methods

Many built-in types have chainable methods:

```grapa
/* String chaining */
result = "  hello world  ".trim().upper().replace("WORLD", "GRAPA");
result.echo();  // Output: HELLO GRAPA

/* String chaining with case folding */
result = "  İstanbul  ".trim().casefold().upper();
result.echo();  // Output: ISTANBUL

/* Array chaining */
result = [1, 2, 3, 4, 5].filter(op(x) { x % 2 == 0; }).map(op(x) { x * 2; });
result.echo();  // Output: [4, 8]
```

## Advanced Chaining with `$$` Variable

Grapa's chaining system allows you to insert custom operations into any chain using the `$$` variable (equivalent to `$0`) to reference the result of the previous operation.

### **Basic Chaining with `$$`**

If an existing function/command doesn't support chaining, an operation block can be inserted using `$$` to reference the previous result:

```grapa
/* Get length of list, generate that many bits, output in binary */
{1,2,3,5}.{(@$$).len()}.genbits().bin();
// Result: 1111

/* Alternative using op() function */
{1,2,3,5}.{(op(a){a.len()})(@$$)}.genbits().bin();
// Result: 1111

/* Using predefined function */
f = op(a){a.len()};
{1,2,3,5}.{f(@$$)}.genbits().bin();
// Result: 1111
```

### **Advanced String Chaining**

You can perform complex string transformations in chains:

```grapa
/* Simple transformation */
"hello".{@$$.upper()}.left(2);
// Result: HE

/* Complex transformation with multiple steps */
"hello".{x=@$$.upper();x.right(4);}.left(2);
// Result: EL
```

### **Multi-Step Processing**

Chain multiple operations with intermediate variables:

```grapa
/* Process data through multiple transformations */
data.{processed=@$$.clean();processed.filter(op(x){x>0});}.len();
```

**Note**: The `$$` variable represents the result of the previous operation in the chain. The `@` prefix dereferences the variable to get its actual value. `$$` is equivalent to `$0` - both represent the result of the previous operation in a chain.

**Key Concept**: `$$` is the **chain carrier** - it automatically passes the result of each operation to the next step in the chain. This enables seamless data flow through complex transformation pipelines.

**Important**: `@` is for **runtime evaluation** (getting actual values), while `$` is for **compile-time lexical processing** (modifying how the lexer processes tokens).
*/ 

## Advanced Syntax Features

Grapa provides several advanced syntax features that enhance expressiveness and developer experience. These features are designed to make Grapa feel natural to developers from Python, JavaScript, and other mainstream languages.

### **String Interpolation**

Grapa provides advanced string interpolation with expression evaluation and script execution:

```grapa
/* Basic variable interpolation */
name = "Alice";
greeting = "Hello, ${name}!".interpolate();  /* "Hello, Alice!" */

/* Expression evaluation */
x = 10;
y = 5;
result = "Sum: ${x + y}, Product: ${x * y}".interpolate();  /* "Sum: 15, Product: 50" */

/* Script execution */
script = "x * 2 + y";
x = 10;
y = 3;
result = "Result: ${op()(script)()}".interpolate();  /* "Result: 23" */

/* Parameterized templates */
template = "Hello ${name}, you are ${age} years old";
result = template.interpolate({name: "Bob", age: 30});  /* "Hello Bob, you are 30 years old" */
```

**Advanced Features:**
- **Expression Evaluation** - `${expression}` for direct code execution
- **Script Execution** - `${op()("script")()}` for complex script execution
- **Parameter Passing** - Pass parameters to interpolation context
- **Template Systems** - Reusable templates with parameter substitution

### **Ternary Operator**

Grapa supports the ternary operator for conditional expressions:

```grapa
/* Basic ternary operator */
result = condition ? value_if_true : value_if_false;

/* Nested ternary operators */
grade = score >= 90 ? "A" : 
        score >= 80 ? "B" : 
        score >= 70 ? "C" : "F";

/* With expressions */
max_value = a > b ? a : b;
status = user.is_active ? "Online" : "Offline";
```

### **Spaceship Operator**

Grapa supports the spaceship operator (`<=>`) for three-way comparison:

```grapa
/* Three-way comparison */
result = a <=> b;  /* Returns -1, 0, or 1 */

/* Sorting with spaceship operator */
sort_function = op(a, b) { a <=> b; };
sorted_array = [3, 1, 4, 1, 5].sort(sort_function);

/* Custom comparison */
compare_length = op(a, b) { a.len() <=> b.len(); };
sorted_by_length = ["cat", "dog", "elephant"].sort(compare_length);
```

### **Range Function**

Grapa provides a native range function for sequence generation:

```grapa
/* Basic range */
numbers = (10).range();  /* [0, 1, 2, 3, 4, 5, 6, 7, 8, 9] */

/* Range with start and step */
range1 = (10).range(1, 2);   /* [1, 3, 5, 7, 9] */
range2 = (10).range(5, 1);   /* [5, 6, 7, 8, 9] */

/* Parallel processing with range */
squares = (1000).range(0, 1).map(op(x) { x * x; }, 8);  /* Parallel computation */
```

### **Enhanced Assignment Operators**

Grapa provides enhanced assignment operators with position-based operations:

```grapa
/* Basic assignment operators */
x += 5;      /* x = x + 5 */
x -= 3;      /* x = x - 3 */
x *= 2;      /* x = x * 2 */
x /= 4;      /* x = x / 4 */

/* Array assignment operators */
arr = [1, 2, 3];
arr += 4;        /* [1, 2, 3, 4] - append */
arr ++= [5, 6];  /* [1, 2, 3, 4, 5, 6] - extend */
arr -= 2;        /* [1, 3, 4, 5, 6] - remove at index */

/* Position-based insertion */
arr = [1, 2, 3];
arr += 10 arr[0];   /* [10, 1, 2, 3] - insert at position 0 */
arr += 20 arr[1];   /* [10, 20, 1, 2, 3] - insert at position 1 */

/* List assignment operators */
list = {a: 1, b: 2};
list.c = 3;      /* {a: 1, b: 2, c: 3} */
list["d"] = 4;   /* {a: 1, b: 2, c: 3, d: 4} */

/* List position-based insertion */
list += (g:55) list[2];  /* Insert at position 2: {a:1, b:2, g:55, c:3} */
list += (h:66) list.b;   /* Insert at position of 'b': {a:1, h:66, b:2, c:3} */
```

### **Advanced Control Flow**

Grapa provides comprehensive control flow structures with proper propagation through all execution contexts:

#### **Conditional Statements**
```grapa
/* If-elseif-else */
if (score >= 90) {
    grade = "A";
} elseif (score >= 80) {
    grade = "B";
} elseif (score >= 70) {
    grade = "C";
} else {
    grade = "F";
}
```

#### **Loop Control Flow**
```grapa
/* While loops with break and continue */
counter = 0;
while (counter < 10) {
    if (counter == 5) {
        break;  /* Exit loop immediately */
    };
    if (counter == 2) {
        counter += 1;
        continue;  /* Skip to next iteration */
    };
    counter.echo();
    counter += 1;
}

/* For loops with control flow */
for (i = 0; i < 10; i += 1) {
    if (i == 5) {
        break;  /* Exit loop immediately */
    };
    if (i == 2) {
        continue;  /* Skip to next iteration */
    };
    i.echo();
}
```

#### **Switch Statements**
```grapa
/* Switch statements with control flow */
switch (day) {
    case 1: day_name = "Monday";;
    case 2: day_name = "Tuesday";;
    case 3: day_name = "Wednesday";;
    default: day_name = "Unknown";;
}

/* Switch with return statements */
switch (value) {
    case 1: return "one";;
    case 2: return "two";;
    default: return "unknown";;
}
```

#### **Function Control Flow**
```grapa
/* Function with return statements */
func calculate(x, y) {
    if (x < 0) {
        return 0;  /* Early return */
    };
    if (y == 0) {
        return x;  /* Early return */
    };
    return x / y;  /* Normal return */
};

/* Function with exit */
func critical_operation() {
    if (error_condition) {
        exit;  /* Terminate program */
    };
    return "success";
};
```

#### **Control Flow in Inline Code Blocks**
```grapa
/* Control flow in inline code blocks */
"hi".{if (true) return 999; x=@$$; x.len()}.range();  /* Returns 999 */

/* Control flow in function calls */
func test() { 
    "hi".{if (true) return 999; x=@$$; x.len()}.range(); 
    return 888; 
}; 
test();  /* Returns 999 */

/* Control flow in switch statements */
switch (1) { 
    case 1: "hi".{if (true) return 999; x=@$$; x.len()}.range(); 
    return 888; 
    default: return 777; 
};  /* Returns 999 */
```

#### **Control Flow Propagation**
All control flow statements properly propagate through all execution contexts:
- **Functions**: RETURN statements exit functions and propagate return values
- **Loops**: BREAK exits loops, CONTINUE skips to next iteration
- **Switches**: RETURN statements exit switch statements
- **Inline Code Blocks**: Control flow properly terminates block execution
- **Nested Contexts**: Control flow propagates through all nested execution contexts

### **Functional Programming Syntax**

Grapa provides elegant functional programming syntax:

```grapa
/* Lambda expressions */
double = op(x) { x * 2; };
square = op(x) { x * x; };

/* Function composition */
compose = op(f, g) { op(x) { f(g(x)); }; };
double_then_square = compose(square, double);

/* Functional methods */
numbers = [1, 2, 3, 4, 5];
doubled = numbers.map(double);
filtered = numbers.filter(op(x) { x > 2; });
sum = numbers.reduce(op(acc, x) { acc + x; }, 0);

/* Parallel processing */
parallel_result = numbers.map(op(x) { x * x; }, 4);  /* 4 threads */
```

### **Advanced Pattern Matching**

Grapa provides superior pattern matching capabilities:

```grapa
/* Regex pattern matching */
text = "Hello World 123";
matches = text.grep("\\d+");  /* Find all digits */

/* Boolean pattern matching */
has_digits = text.grep("\\d+").len() > 0;  /* true/false */
has_letters = text.grep("[a-zA-Z]+").len() > 0;  /* true/false */

/* Complex pattern matching */
email_pattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";
is_valid_email = email.grep(email_pattern).len() > 0;
```

### **Metaprogramming Syntax**

Grapa provides powerful metaprogramming capabilities:

```grapa
/* Dynamic code execution */
code_string = "x * 2 + y";
x = 10;
y = 5;
result = op()(code_string)();  /* 25 */

/* Execution tree manipulation */
func = op(x) { x * 2; };
tree = func.plan();  /* Get execution tree */
result = tree(5);    /* Execute tree with parameter */

/* Template generation */
template = "function ${name}(x) { return ${body}; }";
generated_code = template.interpolate({
    name: "double",
    body: "x * 2"
});
```

/*
# Dynamic Code Execution (Grapa's Core Meta-Programming Capability)

Grapa's most powerful feature is its ability to compile and execute code at runtime. Unlike most languages, Grapa provides **superior** dynamic code execution capabilities through execution trees that are both human-readable and directly executable.

## Using `$sys().eval()` for Immediate Evaluation
- Evaluates a string as Grapa code immediately
- Optionally accepts a parameter map for dynamic execution
- Perfect for one-off evaluations and user input processing

```grapa
/* Simple evaluation */
result = $sys().eval("5*3"); // result: 15

/* Evaluation with parameters */
result = $sys().eval("x + y", {"x": 2, "y": 4}); // result: 6

/* Complex expressions */
result = $sys().eval("(a + b) * c", {"a": 2, "b": 3, "c": 4}); // result: 20

/* User input processing */
user_input = "2 * (3 + 4)";
result = $sys().eval(user_input); // result: 14
```

## Using `op()` for Compiled Operations
- `op()(<string>)` parses the string into a $OP object (compiled execution tree)
- Creates reusable, optimized functions that can be executed multiple times
- Supports both positional and named parameters
- Enables advanced meta-programming patterns

```grapa
/* Basic compilation */
op_obj = op()("a + b");
result = op_obj({"a": 10, "b": 20}); // result: 30

/* One-line execution */
result = op()("5*3")(); // result: 15

/* With parameters */
func = op("name"=0)("'Hello, ' + name + '!'.echo();");
func("Grapa"); // Output: Hello, Grapa!

/* Dynamic function generation */
operations = ["add", "sub", "mul"];
funcs = {};
i = 0;
while (i < operations.len()) {
    op_name = operations.get(i);
    code = "a " + op_name + " b";
    funcs[op_name] = op("a"=0, "b"=0)(code);
    i += 1;
}

/* Execute generated functions */
funcs["add"](5, 3).echo();  // 8
funcs["mul"](5, 3).echo();  // 15
```

## Object and Class Extension

Grapa provides operators for extending objects and classes with new methods and properties:

### Extension Operators

- **`+=`** - Adds a **single item** to an object or class
- **`++=`** - Adds a **$LIST of items** to an object or class

**Note:** The `+=` operator is context-aware. When used on numeric types (`$INT`, `$FLOAT`), it performs addition. When used on collection types (`$ARRAY`, `$LIST`) or objects, it performs extension (adding methods/properties).

### Extending Individual Objects

```grapa
/* Create an object and add a method */
obj = {count: 0};
obj += (increment: op(amount: 1) { count += amount; });

/* Use the extended object */
obj.increment();      /* count becomes 1 */
obj.increment(5);     /* count becomes 6 */
```

### Extending Classes

```grapa
/* Define a custom class */
MyClass = class ($LIST) {
    value: 0;
    add: op(x) { value += x; };
};

/* Extend the class with multiple methods */
MyClass ++= {
    subtract: op(x) { value -= x; },
    getValue: op() { value; }
};

/* Create instances with extended functionality */
instance = MyClass();
instance.add(10);     /* value becomes 10 */
instance.subtract(3); /* value becomes 7 */
instance.getValue().echo(); /* outputs: 7 */
```

### Extending System Classes

```grapa
/* Load a system class into the namespace */
$ARRAY();

/* Extend the system class */
@$ARRAY += (customMethod: op() { 42; });
@$ARRAY ++= {
    lengthMethod: op() { $this.len(); },
    utilityMethod: op() { "Hello from extended array"; }
};

/* Use the extended system class */
arr = [1, 2, 3];
arr.customMethod().echo();    /* outputs: 42 */
arr.lengthMethod().echo();    /* outputs: 3 */
arr.utilityMethod().echo();   /* outputs: Hello from extended array */
```

## Language Syntax Extension

Grapa's dynamic code execution capabilities are complemented by its ability to extend the language syntax itself at runtime. You can define custom commands and functions that become part of the language grammar.

For comprehensive documentation on syntax extension, including custom commands, custom functions, scoping, and dynamic compilation, see [Language Syntax Extension](../type/rule.md#language-syntax-extension).

### Quick Syntax Extension Example

```grapa
// Define a custom function that becomes part of the language
custom_function = rule select $INT {op(p:$2){p*5}};

// Use it as if it were built into the language
select 4;        // Returns 20
x = select 8;    // x = 40
```

### Numbers

Grapa supports various number formats:

```grapa
/* Integer literals */
42;                 /* Decimal integer */
0x2A;              /* Hexadecimal integer */
0b101010;          /* Binary integer */

/* Float literals */
3.14;              /* Standard decimal notation */
3_14;              /* Underscore decimal notation (equivalent) */
2.5;               /* Standard decimal notation */
2_5;               /* Underscore decimal notation (equivalent) */

/* Verification */
(3.14 == 3_14).echo();    /* true */
(2.5 == 2_5).echo();      /* true */
```

**Float Decimal Separators:**
- **Dots (`.`)**: Standard decimal notation (e.g., `3.14`)
- **Underscores (`_`)**: Alternative decimal notation (e.g., `3_14`)
- **Interchangeable**: Both notations are completely equivalent
- **Single separator**: Only one decimal separator allowed per number (standard mathematical definition)
- **Modern support**: Aligns with other modern programming languages

/* Hex and binary floats */
0x12.34;           /* Hex float */
0b101.011;         /* Binary float */
```

// ... existing code ...