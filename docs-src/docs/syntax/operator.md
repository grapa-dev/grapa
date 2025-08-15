# Overview & Complete Reference: Operators

**See also:** [Operator Precedence Table](precedence.md)

## Overview

Grapa provides 30 operators across 6 categories, designed with a philosophy that programmers shouldn't have to care about type casting. Operators work intelligently with relevant data types, though some intentionally maintain mathematical purity by not converting types.


## New Data Types in Type Support Matrix

The Type Support Matrix now includes several additional data types that extend Grapa's operator capabilities:


### **VECTOR** and **TUPLE**
- **VECTOR**: Mathematical vectors and matrices with element-wise operations

- **TUPLE**: Immutable ordered collections similar to arrays but with different semantics

- **Support**: Most arithmetic operators (`+`, `-`, `*`, `/`, `**`, `*/`), assignment operators, and logical operators

- **Examples**: `[1,2,3] + [4,5,6]` (element-wise addition), `(1,2,3) * 2` (scalar multiplication)


### **RULE**
- **RULE**: Function definitions and executable code blocks
- **Support**: Assignment (`=`), equality comparison (`==`, `!=`), and logical operators

- **Examples**: `rule = op(x){x*2};`, `rule1 == rule2` (object ID comparison)


### **XML/TAG** and **XML/EL**
- **XML/TAG**: XML element tags with attributes and structure
- **XML/EL**: XML element content and nested elements
- **Support**: Equality comparison (`==`, `!=`), assignment (`=`), and logical operators

- **Usage**: These types represent the internal structure of XML objects, where operators work on the XML content rather than the XML object itself

- **Examples**: `xml1 == xml2` (object ID comparison), `tag1 = tag2` (assignment)


## Operator Categories

### 1. Arithmetic Operators
Pure mathematical operations with smart type promotion and matrix support.

### 2. Comparison Operators
Equality and relational comparisons with unified type handling.

### 3. Logical Operators
Boolean logic with perfect truthiness conversion.

### 4. Bitwise Operators
Bit-level operations with strict integer handling.

### 5. Assignment Operators
Variable assignment and compound assignment operations.

### 6. Special Operators
Conditional expressions, extension operations, and dot product.

## Type Support Matrix

### Arithmetic Operators

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `+` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | Smart type promotion |
| `-` | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | String difference calculation |
| `*` | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Matrix multiplication support |
| `/` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Smart type promotion |
| `%` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Modulo with modpow/modinv |
| `**` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Exponentiation |
| `*/` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Root operator (nth root) |

### Comparison Operators

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `==` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Object ID for complex types |
| `!=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Object ID for complex types |
| `<` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | No type conversion |
| `<=` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | No type conversion |
| `>` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Unified type handling |
| `>=` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Unified type handling |
| `<=>` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Unified type handling |

### Logical Operators

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `&&` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect implementation |
| `\|\|` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect implementation |
| `!` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect implementation |

### Bitwise Operators

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `&` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | Strict INT/RAW only |
| `\|` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | Strict INT/RAW only |
| `^` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | Strict INT/RAW only |
| `~` | ✅ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Matrix inversion support |
| `<<` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | FLOAT support |
| `>>` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | FLOAT support |

### Assignment Operators

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect assignment |
| `+=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ✅ | ✅ | ✅ | ❌ | ❌ | Append assignment |
| `-=` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Remove by index/key |
| `*=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Multiplication assignment |
| `/=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Division assignment |
| `%=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Modulo assignment |
| `**=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Power assignment |

### Special Operators

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `? :` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Conditional/ternary operator |
| `++=` | ❌ | ❌ | ✅ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Array/list extension |
| `.*` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Dot product |

## Arithmetic Operators

### Addition (`+`)

Performs addition with smart type promotion and string concatenation.

```grapa
/* Numeric addition */
5 + 3;                    /* 8 (INT) */
3.14 + 2.86;              /* 6.0 (FLOAT) */

/* String concatenation */
"Hello" + " " + "World";  /* "Hello World" (STR) */

/* Array concatenation */
[1,2] + [3,4];            /* [1,2,3,4] (ARRAY) */

/* List concatenation */
{a:1} + {b:2};            /* {a:1,b:2} (LIST) */

/* Vector operations */
[1,2,3] + [4,5,6];        /* [5,7,9] (VECTOR) */

/* Time arithmetic */
time() + 3600;            /* Add 1 hour to current time */
```

**Type Promotion Rules:**
- Numbers are promoted to the highest precision
- Strings concatenate with any type
- Arrays and lists merge their contents
- Vectors perform element-wise addition

### Subtraction (`-`)

Performs subtraction with smart type promotion.

```grapa
/* Numeric subtraction */
10 - 3;                   /* 7 (INT) */
5.5 - 2.3;                /* 3.2 (FLOAT) */

/* String difference */
"hello world" - "world";  /* "hello " (STR) */

/* Array removal */
[1,2,3,4] - [2,4];        /* [1,3] (ARRAY) */

/* Time arithmetic */
time() - 86400;           /* Subtract 1 day from current time */
```

### Multiplication (`*`)

Performs multiplication with matrix support.

```grapa
/* Numeric multiplication */
4 * 5;                    /* 20 (INT) */
2.5 * 3.0;                /* 7.5 (FLOAT) */

/* String repetition */
"ha" * 3;                 /* "hahaha" (STR) */

/* Array repetition */
[1,2] * 3;                /* [1,2,1,2,1,2] (ARRAY) */

/* Matrix multiplication */
[[1,2],[3,4]] * [[5,6],[7,8]];  /* Matrix product */

/* Vector scalar multiplication */
[1,2,3] * 2;              /* [2,4,6] (VECTOR) */
```

### Division (`/`)

Performs division with smart type promotion.

```grapa
/* Numeric division */
10 / 3;                   /* 3.333... (FLOAT) */
15 / 3;                   /* 5.0 (FLOAT) */

/* Array division */
[10,20,30] / 2;           /* [5,10,15] (ARRAY) */
```

### Modulo (`%`)

Performs modulo operation with cryptographic support.

```grapa
/* Numeric modulo */
17 % 5;                   /* 2 (INT) */

/* Cryptographic operations */
123456789 % 1000000007;   /* Large number modulo */
```

### Exponentiation (`**`)

Performs power operations.

```grapa
/* Numeric exponentiation */
2 ** 8;                   /* 256 (INT) */
3.14 ** 2;                /* 9.8596 (FLOAT) */

/* Array exponentiation */
[2,3,4] ** 2;             /* [4,9,16] (ARRAY) */

/* Vector operations */
[1,2,3] ** 3;             /* [1,8,27] (VECTOR) */
```

### Root Operator (`*/`)

Performs nth root calculations.

```grapa
/* Square root */
16 */ 2;                  /* 4.0 (FLOAT) */

/* Cube root */
27 */ 3;                  /* 3.0 (FLOAT) */

/* Array roots */
[16,25,36] */ 2;          /* [4,5,6] (ARRAY) */

/* Vector operations */
[8,27,64] */ 3;           /* [2,3,4] (VECTOR) */
```

## Comparison Operators

### Equality (`==`)

Performs equality comparison with object ID for complex types.

```grapa
/* Numeric equality */
5 == 5;                   /* true (BOOL) */
3.14 == 3.14;             /* true (BOOL) */

/* String equality */
"hello" == "hello";       /* true (BOOL) */

/* Array equality (object ID) */
arr1 = [1,2,3];
arr2 = [1,2,3];
arr1 == arr2;              /* false (different objects) */
arr1 == arr1;              /* true (same object) */

/* List equality (object ID) */
list1 = {a:1, b:2};
list2 = {a:1, b:2};
list1 == list2;            /* false (different objects) */
```

### Inequality (`!=`)

Performs inequality comparison.

```grapa
/* Numeric inequality */
5 != 3;                   /* true (BOOL) */
3.14 != 3.15;             /* true (BOOL) */

/* String inequality */
"hello" != "world";       /* true (BOOL) */
```

### Relational Operators (`<`, `<=`, `>`, `>=`)

Perform relational comparisons with no type conversion.

```grapa
/* Numeric comparisons */
5 < 10;                   /* true (BOOL) */
3.14 <= 3.14;             /* true (BOOL) */
10 > 5;                   /* true (BOOL) */
7 >= 7;                   /* true (BOOL) */

/* String comparisons */
"apple" < "banana";       /* true (BOOL) */
"zebra" > "apple";        /* true (BOOL) */

/* Time comparisons */
time1 = time();
time2 = time() + 3600;
time1 < time2;             /* true (BOOL) */
```

### Spaceship Operator (`<=>`)

Performs three-way comparison.

```grapa
/* Numeric comparison */
5 <=> 10;                 /* -1 (less than) */
10 <=> 5;                 /* 1 (greater than) */
5 <=> 5;                  /* 0 (equal) */

/* String comparison */
"apple" <=> "banana";     /* -1 (less than) */
"zebra" <=> "apple";      /* 1 (greater than) */
```

## Logical Operators

### Logical AND (`&&`)

Performs logical AND with perfect truthiness conversion.

```grapa
/* Boolean logic */
true && true;             /* true (BOOL) */
true && false;            /* false (BOOL) */

/* Truthiness conversion */
5 && 10;                  /* 10 (truthy values) */
0 && 5;                   /* 0 (falsy value) */
"" && "hello";            /* "" (falsy value) */
"hello" && "world";       /* "world" (truthy values) */

/* Array operations */
[1,2,3] && [4,5,6];       /* [4,5,6] (truthy arrays) */
[] && [1,2,3];            /* [] (falsy empty array) */
```

### Logical OR (`||`)

Performs logical OR with perfect truthiness conversion.

```grapa
/* Boolean logic */
true || false;            /* true (BOOL) */
false || true;            /* true (BOOL) */

/* Truthiness conversion */
5 || 10;                  /* 5 (first truthy value) */
0 || 5;                   /* 5 (second truthy value) */
"" || "hello";            /* "hello" (second truthy value) */

/* Array operations */
[1,2,3] || [4,5,6];       /* [1,2,3] (first truthy array) */
[] || [1,2,3];            /* [1,2,3] (second truthy array) */
```

### Logical NOT (`!`)

Performs logical NOT with perfect truthiness conversion.

```grapa
/* Boolean logic */
!true;                    /* false (BOOL) */
!false;                   /* true (BOOL) */

/* Truthiness conversion */
!5;                       /* false (truthy becomes false) */
!0;                       /* true (falsy becomes true) */
!"hello";                 /* false (truthy becomes false) */
!"";                      /* true (falsy becomes true) */

/* Array operations */
![1,2,3];                 /* false (truthy array becomes false) */
![];                      /* true (falsy empty array becomes true) */
```

## Bitwise Operators

### Bitwise AND (`&`)

Performs bitwise AND (strict INT/RAW only).

```grapa
/* Integer bitwise operations */
5 & 3;                    /* 1 (INT) */
15 & 7;                   /* 7 (INT) */

/* Raw data operations */
raw1 = raw("0xFF");
raw2 = raw("0x0F");
raw1 & raw2;               /* Raw bitwise AND */
```

### Bitwise OR (`|`)

Performs bitwise OR (strict INT/RAW only).

```grapa
/* Integer bitwise operations */
5 | 3;                    /* 7 (INT) */
15 | 7;                   /* 15 (INT) */

/* Raw data operations */
raw1 = raw("0xFF");
raw2 = raw("0x0F");
raw1 | raw2;               /* Raw bitwise OR */
```

### Bitwise XOR (`^`)

Performs bitwise XOR (strict INT/RAW only).

```grapa
/* Integer bitwise operations */
5 ^ 3;                    /* 6 (INT) */
15 ^ 7;                   /* 8 (INT) */

/* Raw data operations */
raw1 = raw("0xFF");
raw2 = raw("0x0F");
raw1 ^ raw2;               /* Raw bitwise XOR */
```

### Bitwise NOT (`~`)

Performs bitwise NOT with matrix inversion support.

```grapa
/* Integer bitwise operations */
~5;                       /* -6 (INT) */
~0;                       /* -1 (INT) */

/* Array operations */
~[1,2,3];                 /* [-2,-3,-4] (ARRAY) */

/* Matrix inversion */
~[[1,2],[3,4]];           /* Matrix inversion */
```

### Bit Shift Left (`<<`)

Performs left bit shift (supports FLOAT).

```grapa
/* Integer bit shift */
5 << 2;                   /* 20 (INT) */
10 << 1;                  /* 20 (INT) */

/* Float bit shift */
5.5 << 2;                 /* 22 (INT) */
```

### Bit Shift Right (`>>`)

Performs right bit shift (supports FLOAT).

```grapa
/* Integer bit shift */
20 >> 2;                  /* 5 (INT) */
10 >> 1;                  /* 5 (INT) */

/* Float bit shift */
20.0 >> 2;                /* 5 (INT) */
```

## Assignment Operators

### Assignment (`=`)

Performs perfect assignment for all types.

```grapa
/* Basic assignment */
x = 5;                    /* Assign integer */
y = "hello";              /* Assign string */
z = [1,2,3];              /* Assign array */
w = {a:1, b:2};           /* Assign list */

/* Object assignment */
obj = new();
obj.property = "value";    /* Object property assignment */

/* Rule assignment */
func = op(x){x*2};        /* Function assignment */
```

### Compound Assignment Operators

#### Add Assignment (`+=`)

Performs append assignment with comprehensive support.

```grapa
/* Numeric addition */
a = 5;
a += 3;                    /* 8 */

/* String concatenation */
s = "hello";
s += "world";              /* "helloworld" */

/* Array append */
arr = [1,2,3];
arr += 4;                  /* [1,2,3,4] */

/* List append */
list = {a:1, b:2};
list += {c:3};             /* {a:1, b:2, c:3} */
```

#### Subtract Assignment (`-=`)

Performs remove assignment with limited support.

```grapa
/* Numeric subtraction */
a = 10;
a -= 3;                    /* 7 */

/* Array remove by index */
arr = [1,2,3,4,5];
arr -= 2;                  /* [1,2,4,5] (removes element at index 2) */
```

#### Multiply Assignment (`*=`)

Performs multiplication assignment with numeric type support.

```grapa
/* Integer multiplication */
a = 10;
a *= 3;                    /* 30 */

/* Float multiplication */
b = 5.5;
b *= 2;                    /* 11.0 */
```

#### Divide Assignment (`/=`)

Performs division assignment with smart type promotion.

```grapa
/* Integer division */
a = 10;
a /= 2;                    /* 5.0 (promoted to float) */

/* Float division */
b = 10.0;
b /= 3;                    /* 3.333... */
```

#### Modulo Assignment (`%=`)

Performs modulo assignment with numeric type support.

```grapa
/* Integer modulo */
a = 10;
a %= 3;                    /* 1 */

/* Float modulo */
b = 10.5;
b %= 3;                    /* 1.5 */
```

#### Power Assignment (`**=`)

Performs exponentiation assignment with smart type promotion.

```grapa
/* Integer power */
a = 2;
a **= 3;                   /* 8 */

/* Float power */
b = 2.5;
b **= 2;                   /* 6.25 */
```

## Special Operators

### Ternary Conditional (`? :`)

Performs conditional expressions with implementation quirks.

```grapa
/* Basic ternary */
result = 5 > 3 ? "yes" : "no";  /* "yes" */

/* Nested ternary */
value = x > 0 ? "positive" : x < 0 ? "negative" : "zero";

/* Array conditional */
arr = condition ? [1,2,3] : [4,5,6];

/* Implementation quirks */
true ? : "yes";            /* "" (Form 1: inverted logic) */
false ? : "yes";           /* "yes" (Form 1: inverted logic) */
true ? "yes" : "no" : "maybe";  /* "maybe" (Form 3: unexpected) */
```

### Extend (`++=`)

Extends arrays and lists.

```grapa
/* Array extension */
arr = [1,2,3];
arr ++= [4,5,6];          /* [1,2,3,4,5,6] */

/* List extension */
list = {a:1, b:2};
list ++= {c:3, d:4};      /* {a:1, b:2, c:3, d:4} */

/* String extension */
str = "hello";
str ++= " world";         /* "hello world" */
```

### Dot Product (`.*`)

Performs dot product operations.

```grapa
/* Vector dot product */
[1,2,3] .* [4,5,6];       /* 32 (INT) */

/* Array dot product */
[1,2,3] .* [4,5,6];       /* 32 (INT) */

/* Matrix dot product */
[[1,2],[3,4]] .* [[5,6],[7,8]];  /* Matrix dot product */
```

## Type Support Analysis

### **Gaps**: 11 gaps identified (strict numeric and array types only)

## Best Practices

### Type Conversion
- **Addition (`+`)**: Converts numbers to strings for concatenation
- **Comparison operators**: Generally don't convert types (maintains mathematical purity)
- **Logical operators**: Perfect truthiness conversion
- **Bitwise operators**: Strict INT/RAW only (except `~` with matrix support)

### Error Handling
- **Most operators**: Return $ERR for unsupported type combinations
- **Array operations**: Often return empty arrays instead of errors
- **String operations**: May return unexpected results for unsupported combinations

### Performance Considerations
- **Object ID comparison**: Arrays and lists use object ID comparison for performance
- **Short-circuit evaluation**: Logical operators use short-circuit evaluation
- **Matrix operations**: Advanced linear algebra capabilities available

### Known Issues
- **Ternary quirks**: Form 1 has inverted logic, Form 3 has unexpected behavior
- **Array comparison**: Uses object ID instead of content comparison

### Float Comparison Behavior
- **Precision normalization**: Float comparisons automatically normalize precision settings
- **String-to-float**: `"55.3".float() == 55.3` works correctly due to precision normalization
- **Mixed comparisons**: Float literals and string-converted floats compare consistently
- **Adaptive tolerance**: String-to-float comparisons use adaptive tolerance that scales with number magnitude
- **Edge case handling**: Better support for very small and very large numbers in comparisons

## Advanced Features

### Matrix Operations
- **Multiplication (`*`)**: Supports matrix multiplication
- **Bitwise NOT (`~`)**: Supports matrix inversion
- **Root (`*/`)**: Supports vector operations

### Cryptographic Features
- **Modulo (`%`)**: Built-in modpow and modinv for cryptographic operations
- **Bitwise operators**: Strict integer handling for cryptographic applications

### Mathematical Sophistication
- **Root operator (`*/`)**: nth root calculations
- **Type promotion**: Smart type conversion for mathematical operations
- **Truthiness conversion**: Sophisticated boolean conversion rules

## Summary

Grapa provides 26 operators with sophisticated type handling and mathematical capabilities. All operators work as expected, with some intentional design decisions that users should be aware of. The operators are designed with a balance between mathematical purity and user convenience, with some intentionally not converting types to maintain mathematical correctness.
