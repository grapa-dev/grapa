# Compare Operators

## `==`
Equal to. Returns `true` if operands are equal, `false` otherwise.

```grapa
5 == 5        // true
5 == 3        // false
"hello" == "hello"  // true
5 == "5"      // true (numeric string conversion)
```

## `!=`
Not equal to. Returns `true` if operands are not equal, `false` otherwise.

```grapa
5 != 3        // true
5 != 5        // false
"hello" != "world"  // true
5 != "5"      // false (numeric string conversion)
```

## `<=>`
Spaceship operator. Returns -1 if left < right, 0 if equal, 1 if left > right.

```grapa
5 <=> 3       // 1 (5 is greater than 3)
3 <=> 5       // -1 (3 is less than 5)
5 <=> 5       // 0 (equal)
"hello" <=> "world"  // -1 (lexicographic comparison)
"world" <=> "hello"  // 1 (lexicographic comparison)
5 <=> "5"     // 0 (numeric string conversion)
5 <=> "hello" // 1 (numeric vs string comparison)
```

**Note**: All comparison operators now use unified logic and return standardized values. String comparisons return -1/0/1 instead of varying string comparison distances.

## `<=`
Less than or equal to.

```grapa
3 <= 5        // true
5 <= 5        // true
5 <= 3        // false
"abc" <= "def"  // true
```

## `>=`
Greater than or equal to.

```grapa
5 >= 3        // true
5 >= 5        // true
3 >= 5        // false
"def" >= "abc"  // true
```

## `>`
Greater than.

```grapa
5 > 3         // true
5 > 5         // false
3 > 5         // false
"def" > "abc" // true
```

## `<`
Less than.

```grapa
3 < 5         // true
5 < 5         // false
5 < 3         // false
"abc" < "def" // true
```

## `&&`
Logical AND. Two conditions must be true.

```grapa
((1==1) && (2==2)) -> true
((1==1) && (2==3)) -> false
((1==2) && (2==3)) -> false
```

## `||`
Logical OR. If either of two conditions are true.

```grapa
((1==1) || (2==2)) -> true
((1==1) || (2==3)) -> true
((1==2) || (2==3)) -> false
```

# Modifier Operators

## `-`
Negate.

```grapa
-(4+2) -> -6
```

## `!`
Not.

```grapa
!true -> false
!false -> true
```

## `~`
Bitwise not.

```grapa
(~0x6C).hex() -> FFFFFF93
```

# Type Handling

All comparison operators now support mixed type comparisons with intelligent type conversion:

- **Numeric strings**: Automatically converted to numbers for comparison
- **Non-numeric strings**: Fall back to string comparison
- **Float vs String**: Uses adaptive tolerance for numeric strings
- **Bool comparisons**: Handled with proper truthiness logic
- **Null comparisons**: Properly handled for equality and ordering

**Examples**:
```grapa
5 == "5"      // true (numeric conversion)
5.5 == "5.5"  // true (float conversion with tolerance)
5 > "hello"   // true (numeric vs string)
"hello" > 5   // true (string vs numeric)
```

---

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
