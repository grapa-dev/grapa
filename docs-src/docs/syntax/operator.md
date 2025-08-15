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
Addition, subtraction, multiplication, division, modulo, exponentiation, and root operations with smart type promotion.

### 2. Comparison Operators  
Equality, inequality, and relational operators with unified type handling using the `DoComparison` static helper function. All comparison operators now provide consistent behavior and standardized return values.

### 3. Logical Operators
Boolean logic operations with sophisticated truthiness conversion.

### 4. Bitwise Operators
Bit-level operations with strict type requirements and advanced matrix support.

### 5. Assignment Operators
Variable assignment and compound assignment operations.

### 6. Special Operators
Extend, remove, dot operations, and ternary conditional expressions.

## Type Support Matrix

<div style="overflow-x: auto; overflow-y: auto; max-height: 600px;">

| Operator | INT | FLOAT | STR | BOOL | ARRAY | LIST | OBJ | ERR | TIME | RAW | VECTOR | TUPLE | RULE | XML/TAG | XML/EL | Notes |
|:---------|:---:|:-----:|:---:|:---:|:-----:|:----:|:---:|:---:|:----:|:---:|:------:|:-----:|:----:|:-------:|:------:|:------|
| `+` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | Smart type promotion |
| `-` | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | String difference calculation |
| `+=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ✅ | ✅ | ✅ | ❌ | ❌ | Append assignment |
| `-=` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Remove by index/key |
| `*=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Multiplication assignment |
| `/=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Division assignment |
| `%=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Modulo assignment |
| `**=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Power assignment |
| `*` | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Matrix multiplication support |
| `/` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Smart type promotion |
| `%` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Modulo with modpow/modinv |
| `**` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Exponentiation |
| `*/` | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Root operator (nth root) |
| `==` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Object ID for complex types |
| `!=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Object ID for complex types |
| `<` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | No type conversion |
| `<=` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | No type conversion |
| `>` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Unified type handling |
| `>=` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Unified type handling |
| `<=>` | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Unified type handling |
| `&&` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect implementation |
| `\|\|` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect implementation |
| `!` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect implementation |
| `&` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | Strict INT/RAW only |
| `\|` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | Strict INT/RAW only |
| `^` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | Strict INT/RAW only |
| `~` | ✅ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | Matrix inversion support |
| `<<` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | FLOAT support |
| `>>` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | FLOAT support |
| `=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Perfect assignment |
| `? :` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | Implementation quirks |
| `++` | ❌ | ❌ | ✅ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Array/list extension |
| `--` | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Array/list removal |
</div>

## Arithmetic Operators

### Addition (`+`)

Performs addition with smart type promotion and string concatenation.

```grapa
/* Numeric addition */
5 + 3;                    /* 8 (INT) */
5 + 3.14;                 /* 8.14... (FLOAT) */
3.14 + 5;                 /* 8.14... (FLOAT) */

/* String concatenation */
"hello" + "world";        /* "helloworld" */
"hello" + 5;              /* "hello5" */
5 + "world";              /* 5 (asymmetric - should be "5world") */

/* Boolean addition */
true + 5;                 /* 5 (boolean converted to number) */
false + 3.14;             /* 3.14... (boolean converted to number) */

/* Array operations */
[1,2,3] + [4,5,6];       /* [] (empty array - should concatenate) */
[1,2,3] + 4;              /* [] (empty array - should append) */

/* Vector operations */
[1,2,3] + [4,5,6];       /* [5,7,9] (element-wise addition) */
[1,2,3] + 5;              /* [6,7,8] (scalar addition) */

/* Tuple operations */
(1,2,3) + (4,5,6);       /* (5,7,9) (element-wise addition) */
(1,2,3) + 5;              /* (6,7,8) (scalar addition) */

/* Error handling */
$ERR + 5;                 /* "ERR5" (error converted to string) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, ERR, VECTOR, TUPLE  
**Gaps**: 12 gaps identified (asymmetric behavior, array operations)

### Subtraction (`-`)

Performs subtraction with type-aware behavior.

```grapa
/* Numeric subtraction */
10 - 3;                   /* 7 (INT) */
10.5 - 3;                 /* 7.5 (FLOAT) */
10 - 3.5;                 /* 6.5 (FLOAT) */

/* String subtraction (character difference) */
"hello" - "world";        /* -15 (character difference calculation) */

/* Array subtraction (remove by index) */
[1,2,3,4,5] - 2;         /* [1,2,4,5] (removes element at index 2) */
[1,2,3,4,5] - 2.0;       /* [1,2,4,5] (float index converted to int) */

/* Error cases */
5 - "hello";              /* $ERR (no string to number conversion) */
[1,2,3] - [4,5,6];       /* $ERR (no array subtraction) */
```

**Type Support**: INT, FLOAT, STR-STR, ARRAY-INT/FLOAT  
**Gaps**: 12 gaps identified (most combinations return $ERR)

### Multiplication (`*`)

Performs multiplication with matrix support and string repetition.

```grapa
/* Numeric multiplication */
5 * 3;                    /* 15 (INT) */
5 * 3.14;                 /* 15.7... (FLOAT) */
3.14 * 5;                 /* 15.7... (FLOAT) */

/* String repetition */
"hello" * 3;              /* "hellohellohello" */
3 * "world";              /* 3 (asymmetric - should be "worldworldworld") */

/* Array operations */
[1,2,3] * 2;              /* [1,2,3,1,2,3] (array repetition) */
2 * [1,2,3];              /* 2 (asymmetric - should repeat array) */

/* Matrix multiplication (advanced) */
matrix1 = [[1,2],[3,4]];
matrix2 = [[5,6],[7,8]];
matrix1 * matrix2;        /* [[19,22],[43,50]] (matrix multiplication) */

/* Vector operations */
[1,2,3] * [4,5,6];       /* [4,10,18] (element-wise multiplication) */
[1,2,3] * 2;              /* [2,4,6] (scalar multiplication) */

/* Tuple operations */
(1,2,3) * (4,5,6);       /* (4,10,18) (element-wise multiplication) */
(1,2,3) * 2;              /* (2,4,6) (scalar multiplication) */
```

**Type Support**: INT, FLOAT, STR, ARRAY, VECTOR, TUPLE  
**Gaps**: 18 gaps identified (asymmetric behavior, missing type combinations)

### Division (`/`)

Performs division with smart type promotion.

```grapa
/* Numeric division */
10 / 3;                   /* 3.333... (FLOAT) */
10.0 / 3;                 /* 3.333... (FLOAT) */
10 / 3.0;                 /* 3.333... (FLOAT) */

/* Array division */
[1,2,3,4,5] / 2;         /* [0.5,1,1.5,2,2.5] (element-wise division) */

/* Error cases */
5 / "hello";              /* $ERR (no string to number conversion) */
[1,2,3] / [4,5,6];       /* $ERR (no array division) */
```

**Type Support**: INT, FLOAT, ARRAY  
**Gaps**: 15 gaps identified (most combinations return $ERR)

### Modulo (`%`)

Performs modulo operation with cryptographic features.

```grapa
/* Numeric modulo */
10 % 3;                   /* 1 (INT) */
10.5 % 3;                 /* 1.5 (FLOAT) */
10 % 3.5;                 /* 3.0 (FLOAT) */

/* Cryptographic features */
base = 7;
exponent = 13;
modulus = 11;
base.modpow(exponent, modulus);  /* 2 (modular exponentiation) */

value = 3;
modulus = 11;
value.modinv(modulus);    /* 4 (modular multiplicative inverse) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 16 gaps identified (strict numeric types only)

### Exponentiation (`**`)

Performs exponentiation with smart type promotion.

```grapa
/* Numeric exponentiation */
2 ** 3;                   /* 8 (INT) */
2 ** 3.5;                 /* 11.313... (FLOAT) */
2.5 ** 3;                 /* 15.625 (FLOAT) */

/* Array exponentiation */
[1,2,3] ** 2;             /* [1,4,9] (element-wise exponentiation) */

/* Vector operations */
[1,2,3] ** 2;             /* [1,4,9] (element-wise exponentiation) */
[1,2,3] ** [2,3,4];       /* [1,8,81] (element-wise exponentiation) */

/* Tuple operations */
(1,2,3) ** 2;             /* (1,4,9) (element-wise exponentiation) */
(1,2,3) ** (2,3,4);       /* (1,8,81) (element-wise exponentiation) */

/* Error cases */
5 ** "hello";             /* $ERR (no string to number conversion) */
```

**Type Support**: INT, FLOAT, ARRAY, VECTOR, TUPLE  
**Gaps**: 15 gaps identified (most combinations return $ERR)

### Root (`*/`)

Performs nth root calculations with advanced mathematical support.

```grapa
/* Numeric root */
8 */ 3;                   /* 2 (cube root of 8) */
16 */ 2;                  /* 4 (square root of 16) */
27 */ 3.5;                /* 2.884... (3.5th root of 27) */

/* Array root (element-wise) */
[8,27,64] */ 3;          /* [2,3,4] (element-wise cube root) */
[16,25,36] */ 2;         /* [4,5,6] (element-wise square root) */

/* Vector operations */
vector = [1,2,3];
vector */ 2;              /* [1,1.414...,1.732...] (element-wise square root) */

/* Tuple operations */
tuple = (1,2,3);
tuple */ 2;               /* (1,1.414...,1.732...) (element-wise square root) */

/* Error cases */
"hello" */ 2;             /* $ERR (STR not supported) */
true */ 3;                /* $ERR (BOOL not supported) */
```

**Type Support**: INT, FLOAT, ARRAY, VECTOR, TUPLE  
**Gaps**: 13 gaps identified (strict numeric types only)

## Comparison Operators

### Equality (`==`)

Performs equality comparison with object ID comparison for complex types.

```grapa
/* Numeric equality */
5 == 5;                   /* true */
5.0 == 5;                 /* true */
5 == 5.0;                 /* true */

/* Float comparisons with precision normalization */
55.3 == 55.3;             /* true */
"55.3".float() == 55.3;   /* true (normalized precision) */
55.3 == "55.3".float();   /* true (normalized precision) */

/* String equality */
"hello" == "hello";       /* true */
"hello" == "world";       /* false */

/* Boolean equality */
true == true;             /* true */
true == 1;                /* true (truthiness conversion) */

/* Array/List equality (object ID comparison) */
[1,2,3] == [1,2,3];      /* true (same object) */
a = [1,2,3];
b = [1,2,3];
a == b;                   /* false (different objects) */

/* Object equality */
{a:1,b:2} == {a:1,b:2};  /* true (same object) */

/* Vector/Tuple equality (object ID comparison) */
[1,2,3] == [1,2,3];      /* true (same object) */
(1,2,3) == (1,2,3);      /* true (same object) */

/* XML/TAG/EL equality (object ID comparison) */
xml1 = <root><item>1</item></root>;
xml2 = <root><item>1</item></root>;
xml1 == xml2;             /* false (different objects) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 7 gaps identified (object ID vs content comparison)

### Inequality (`!=`)

Performs inequality comparison with object ID comparison for complex types.

```grapa
/* Numeric inequality */
5 != 3;                   /* true */
5.0 != 5;                 /* false */

/* String inequality */
"hello" != "world";       /* true */
"hello" != "hello";       /* false */

/* Array/List inequality (object ID comparison) */
[1,2,3] != [1,2,4];      /* false (same object ID) */
a = [1,2,3];
b = [1,2,4];
a != b;                   /* true (different objects) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 7 gaps identified (object ID vs content comparison)

### Less Than (`<`)

Performs less than comparison without type conversion.

```grapa
/* Numeric comparison */
3 < 5;                    /* true */
3.14 < 5;                 /* true */
5 < 3.14;                 /* false */

/* String comparison */
"apple" < "banana";       /* true (lexicographic) */
"hello" < "world";        /* true */

/* Error cases */
5 < "hello";              /* false (no type conversion) */
"hello" < 5;              /* false (no type conversion) */
```

**Type Support**: INT, FLOAT, STR  
**Gaps**: 8 gaps identified (no type conversion)

### Less Than or Equal (`<=`)

Performs less than or equal comparison without type conversion.

```grapa
/* Numeric comparison */
3 <= 5;                   /* true */
3 <= 3;                   /* true */
5 <= 3;                   /* false */

/* String comparison */
"apple" <= "banana";      /* true */
"hello" <= "hello";       /* true */

/* Error cases */
5 <= "hello";             /* false (no type conversion) */
```

**Type Support**: INT, FLOAT, STR  
**Gaps**: 8 gaps identified (no type conversion)

### Greater Than (`>`)

Performs greater than comparison with unified type handling.

```grapa
/* Numeric comparison */
5 > 3;                    /* true */
5.14 > 3;                 /* true */
3 > 5;                    /* false */

/* String comparison */
"banana" > "apple";       /* true */
"world" > "hello";        /* true */

/* Mixed type comparison */
"hello" > 5;              /* true (string vs numeric) */
5 > "hello";              /* true (numeric vs string) */
5 > "5";                  /* false (numeric string conversion) */
```

**Type Support**: INT, FLOAT, STR  
**Gaps**: 8 gaps identified (mixed type behavior)

### Greater Than or Equal (`>=`)

Performs greater than or equal comparison with unified type handling.

```grapa
/* Numeric comparison */
5 >= 3;                   /* true */
5 >= 5;                   /* true */
3 >= 5;                   /* false */

/* String comparison */
"banana" >= "apple";      /* true */
"hello" >= "hello";       /* true */

/* Mixed type comparison */
"hello" >= 5;             /* true (string vs numeric) */
5 >= "hello";             /* true (numeric vs string) */
5 >= "5";                 /* true (numeric string conversion) */
```

**Type Support**: INT, FLOAT, STR  
**Gaps**: 8 gaps identified (mixed type behavior)

### Spaceship (`<=>`)

Performs three-way comparison with unified type handling and standardized return values.

```grapa
/* Numeric comparison */
5 <=> 3;                  /* 1 (greater) */
3 <=> 5;                  /* -1 (less) */
5 <=> 5;                  /* 0 (equal) */

/* String comparison */
"banana" <=> "apple";     /* 1 (greater) */
"apple" <=> "banana";     /* -1 (less) */
"hello" <=> "hello";      /* 0 (equal) */

/* Mixed type comparison */
"hello" <=> 5;            /* 1 (string vs numeric) */
5 <=> "hello";            /* 1 (numeric vs string) */
5 <=> "5";                /* 0 (numeric string conversion) */
5.5 <=> "5.5";            /* 0 (float string conversion with tolerance) */
```

**Type Support**: INT, FLOAT, STR  
**Gaps**: 7 gaps identified (mixed type behavior)

**Note**: All comparison operators now use unified `DoComparison` static helper function, providing consistent behavior and standardized return values (-1, 0, 1 for spaceship operator).

## Logical Operators

### Logical AND (`&&`)

Performs logical AND with perfect truthiness conversion.

```grapa
/* Boolean logic */
true && true;             /* true */
true && false;            /* false */
false && true;            /* false */
false && false;           /* false */

/* Truthiness conversion */
5 && 3;                   /* 3 (both truthy) */
0 && 5;                   /* 0 (first falsy) */
5 && 0;                   /* 0 (second falsy) */
"" && "hello";            /* "" (first falsy) */
"hello" && "";            /* "" (second falsy) */

/* Short-circuit evaluation */
false && (5/0);           /* false (short-circuits) */
true && (5/0);            /* $ERR (evaluates second operand) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 0 gaps (perfect implementation)

### Logical OR (`||`)

Performs logical OR with perfect truthiness conversion.

```grapa
/* Boolean logic */
true || true;             /* true */
true || false;            /* true */
false || true;            /* true */
false || false;           /* false */

/* Truthiness conversion */
5 || 3;                   /* 5 (first truthy) */
0 || 5;                   /* 5 (second truthy) */
0 || 0;                   /* 0 (both falsy) */
"" || "hello";            /* "hello" (second truthy) */
"hello" || "";            /* "hello" (first truthy) */

/* Short-circuit evaluation */
true || (5/0);            /* true (short-circuits) */
false || (5/0);           /* $ERR (evaluates second operand) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 0 gaps (perfect implementation)

### Logical NOT (`!`)

Performs logical NOT with comprehensive truthiness conversion across all data types.

```grapa
/* Boolean logic */
!true;                     /* false */
!false;                    /* true */

/* Truthiness conversion */
!5;                        /* false (truthy) */
!0;                        /* true (falsy) */
!"hello";                  /* false (truthy) */
!"";                       /* true (falsy) */

/* Float values */
!5.0;                      /* false (truthy) */
!0.0;                      /* true (falsy) */
!(-5.0);                   /* false (truthy) */

/* Complex expressions */
!(5 && 3);                 /* false (truthy result) */
!(0 || 5);                 /* false (truthy result) */
!(true && false);          /* true (falsy result) */
!(false || true);          /* false (truthy result) */

/* Array and List values */
![1,2,3];                  /* false (non-empty array) */
![];                       /* true (empty array) */
!{a:1,b:2};               /* false (non-empty list) */
!{};                       /* true (empty list) */

/* Object and Null values */
!obj;                      /* true (object is falsy) */
!null;                     /* true (null is falsy) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 0 gaps (perfect implementation)

## Bitwise Operators

### Bitwise AND (`&`)

Performs bitwise AND with strict INT/RAW type requirements.

```grapa
/* Integer bitwise operations */
5 & 3;                     /* 1 (101 & 011 = 001) */
10 & 6;                    /* 2 (1010 & 0110 = 0010) */

/* Error cases */
5.5 & 3;                   /* $ERR (FLOAT not supported) */
"hello" & "world";         /* $ERR (STR not supported) */
```

**Type Support**: INT only  
**Gaps**: 12 gaps identified (strict INT/RAW only)

### Bitwise OR (`|`)

Performs bitwise OR with strict INT/RAW type requirements.

```grapa
/* Integer bitwise operations */
5 | 3;                     /* 7 (101 | 011 = 111) */
10 | 6;                    /* 14 (1010 | 0110 = 1110) */

/* Error cases */
5.5 | 3;                   /* $ERR (FLOAT not supported) */
"hello" | "world";         /* $ERR (STR not supported) */
```

**Type Support**: INT only  
**Gaps**: 12 gaps identified (strict INT/RAW only)

### Bitwise XOR (`^`)

Performs bitwise XOR with strict INT/RAW type requirements.

```grapa
/* Integer bitwise operations */
5 ^ 3;                     /* 6 (101 ^ 011 = 110) */
10 ^ 6;                    /* 12 (1010 ^ 0110 = 1100) */

/* Error cases */
5.5 ^ 3;                   /* $ERR (FLOAT not supported) */
"hello" ^ "world";         /* $ERR (STR not supported) */
```

**Type Support**: INT only  
**Gaps**: 12 gaps identified (strict INT/RAW only)

### Bitwise NOT (`~`)

Performs bitwise NOT with advanced matrix inversion support.

```grapa
/* Integer bitwise operations */
~5;                        /* -6 (bitwise complement) */
~0;                        /* -1 (bitwise complement) */

/* Matrix inversion (advanced) */
matrix = [[1,2],[3,4]];
~matrix;                   /* Matrix inversion operations */

/* Error cases */
~5.5;                      /* $ERR (FLOAT not supported) */
~"hello";                  /* $ERR (STR not supported) */
```

**Type Support**: INT, ARRAY (matrix operations)  
**Gaps**: 12 gaps identified (strict INT/RAW only, except matrix support)

### Left Shift (`<<`)

Performs left shift with FLOAT support.

```grapa
/* Integer left shift */
5 << 2;                    /* 20 (5 * 2^2) */
10 << 1;                   /* 20 (10 * 2^1) */

/* Float left shift */
5.5 << 2;                  /* 22.0 (5.5 * 2^2) */

/* Error cases */
5 << "hello";              /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 7 gaps identified (no STR support)

### Right Shift (`>>`)

Performs right shift with FLOAT support.

```grapa
/* Integer right shift */
20 >> 2;                   /* 5 (20 / 2^2) */
10 >> 1;                   /* 5 (10 / 2^1) */

/* Float right shift */
22.0 >> 2;                 /* 5.5 (22.0 / 2^2) */

/* Error cases */
20 >> "hello";             /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 7 gaps identified (no STR support)

## Assignment Operators

### Assignment (`=`)

Performs variable assignment with perfect implementation.

```grapa
/* Basic assignment */
a = 5;                     /* 5 */
b = "hello";               /* "hello" */
c = [1,2,3];               /* [1,2,3] */

/* Multiple assignment */
a = b = c = 5;             /* 5 (all variables set to 5) */

/* Object assignment */
obj = {name: "John", age: 30};  /* {name:"John", age:30} */

/* Vector/Tuple assignment */
vec = [1,2,3];                  /* [1,2,3] */
tup = (1,2,3);                  /* (1,2,3) */

/* Rule assignment */
rule = op(x){x*2};              /* Function rule */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 0 gaps (perfect implementation)

### Add Assignment (`+=`)

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
list = [1,2,3];
list += 4;                 /* [1,2,3,4] */

/* Object append (nested) */
obj = {name: "John"};
obj += {age: 30};          /* {name:"John",{age:30}} */

/* Vector/Tuple append */
vec = [1,2,3];
vec += 4;                  /* [1,2,3,4] */
tup = (1,2,3);
tup += 4;                  /* (1,2,3,4) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE  
**Gaps**: 12 gaps identified (asymmetric behavior)

### Subtract Assignment (`-=`)

Performs remove assignment with limited support.

```grapa
/* Numeric subtraction */
a = 10;
a -= 3;                    /* 7 */

/* Array remove by index */
arr = [1,2,3,4,5];
arr -= 2;                  /* [1,2,4,5] (removes element at index 2) */

/* Error cases */
s = "hello";
s -= "world";              /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT, ARRAY  
**Gaps**: 12 gaps identified (severely limited support)

### Multiply Assignment (`*=`)

Performs multiplication assignment with numeric type support.

```grapa
/* Integer multiplication */
a = 10;
a *= 3;                    /* 30 */

/* Float multiplication */
b = 5.5;
b *= 2;                    /* 11.0 */

/* Mixed type multiplication */
c = 10;
c *= 2.5;                  /* 25.0 (promoted to float) */

/* Error cases */
s = "hello";
s *= 3;                    /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 14 gaps identified (numeric types only)

### Divide Assignment (`/=`)

Performs division assignment with smart type promotion.

```grapa
/* Integer division */
a = 10;
a /= 2;                    /* 5.0 (promoted to float) */

/* Float division */
b = 10.0;
b /= 3;                    /* 3.333... */

/* Mixed type division */
c = 10;
c /= 2.5;                  /* 4.0 */

/* Error cases */
s = "hello";
s /= 2;                    /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 14 gaps identified (numeric types only)

### Modulo Assignment (`%=`)

Performs modulo assignment with numeric type support.

```grapa
/* Integer modulo */
a = 10;
a %= 3;                    /* 1 */

/* Float modulo */
b = 10.5;
b %= 3;                    /* 1.5 */

/* Mixed type modulo */
c = 10;
c %= 3.5;                  /* 3.0 */

/* Error cases */
s = "hello";
s %= 3;                    /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 14 gaps identified (numeric types only)

### Power Assignment (`**=`)

Performs exponentiation assignment with smart type promotion.

```grapa
/* Integer power */
a = 2;
a **= 3;                   /* 8 */

/* Float power */
b = 2.5;
b **= 2;                   /* 6.25 */

/* Mixed type power */
c = 2;
c **= 3.5;                 /* 11.313... (promoted to float) */

/* Error cases */
s = "hello";
s **= 2;                   /* $ERR (STR not supported) */
```

**Type Support**: INT, FLOAT  
**Gaps**: 14 gaps identified (numeric types only)

## Special Operators

### Ternary (`? :`)

Performs conditional expressions with implementation quirks.

```grapa
/* Standard ternary */
true ? "yes" : "no";       /* "yes" */
false ? "yes" : "no";      /* "no" */

/* Numeric conditions */
5 ? "positive" : "zero";   /* "positive" */
0 ? "positive" : "zero";   /* "zero" */

/* String conditions */
"hello" ? "non-empty" : "empty";  /* "non-empty" */
"" ? "non-empty" : "empty";       /* "empty" */

/* Complex conditions */
(5 > 3) ? "greater" : "less";     /* "greater" */

/* Nested ternary */
5 > 3 ? "big" : 5 > 1 ? "medium" : "small";  /* "big" */

/* Implementation quirks */
true ? : "yes";            /* "" (Form 1: inverted logic) */
false ? : "yes";           /* "yes" (Form 1: inverted logic) */
true ? "yes" : "no" : "maybe";  /* "maybe" (Form 3: unexpected) */

/* Vector/Tuple conditional */
[1,2,3] ? "vector" : "empty";  /* "vector" (non-empty vector is truthy) */
() ? "tuple" : "empty";        /* "empty" (empty tuple is falsy) */
```

**Type Support**: INT, FLOAT, STR, BOOL, ARRAY, LIST, OBJ, ERR, VECTOR, TUPLE, RULE, XML/TAG, XML/EL  
**Gaps**: 0 gaps (but has implementation quirks)

### Extend (`++`)

Performs array extension operations, adding elements to arrays and lists.

```grapa
/* Array extension */
[1,2,3] ++ 4;              /* [1,2,3,4] */
[1,2,3] ++ [4,5,6];       /* [1,2,3,4,5,6] */

/* List extension */
{1,2,3} ++ 4;              /* {1,2,3,4} */
{1,2,3} ++ {4,5,6};       /* {1,2,3,4,5,6} */

/* String extension */
"hello" ++ "world";        /* "helloworld" */
"hello" ++ 5;              /* "hello5" */

/* Error cases */
5 ++ 3;                    /* $ERR (INT not supported) */
true ++ false;             /* $ERR (BOOL not supported) */
```

**Type Support**: ARRAY, LIST, STR  
**Gaps**: 8 gaps identified (limited to collection types)

### Remove (`--`)

Performs array removal operations, removing elements from arrays and lists.

```grapa
/* Array removal by index */
[1,2,3,4,5] -- 2;          /* [1,2,4,5] (removes element at index 2) */
[1,2,3,4,5] -- 2.0;        /* [1,2,4,5] (float index converted to int) */

/* Array removal by value */
[1,2,3,2,5] -- 2;          /* [1,3,5] (removes all occurrences of value 2) */

/* List removal */
{1,2,3,4,5} -- 2;          /* {1,2,4,5} (removes element at index 2) */

/* Error cases */
5 -- 3;                    /* $ERR (INT not supported) */
"hello" -- "world";        /* $ERR (STR not supported) */
```

**Type Support**: ARRAY, LIST  
**Gaps**: 10 gaps identified (limited to collection types)

### Dot Product (`.*`)

Performs dot product operations for vectors and matrices.

```grapa
/* Vector dot product */
[1,2,3] .* [4,5,6];       /* 32 (1*4 + 2*5 + 3*6) */

/* Matrix dot product */
[[1,2],[3,4]] .* [[5,6],[7,8]];  /* [[19,22],[43,50]] */

/* Vector dot product */
[1,2,3] .* [4,5,6];       /* 32 (1*4 + 2*5 + 3*6) */

/* Tuple dot product */
(1,2,3) .* (4,5,6);       /* 32 (1*4 + 2*5 + 3*6) */

/* Scalar dot product */
5 .* 3;                    /* 15 (scalar multiplication) */

/* Error cases */
"hello" .* "world";        /* $ERR (STR not supported) */
[1,2,3] .* 5;              /* $ERR (mixed types not supported) */
```

**Type Support**: INT, FLOAT, ARRAY, VECTOR, TUPLE (vectors/matrices)  
**Gaps**: 11 gaps identified (strict numeric and array types only)

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
