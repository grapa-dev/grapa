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

# Data Structure Comparisons

## Arrays (`[]`)
Arrays are compared element-by-element in order. Length is compared first, then elements.

```grapa
[1,2,3] == [1,2,3]    // true
[1,2] == [1,2,3]      // false (different lengths)
[1,2,3] == [3,2,1]    // false (different order)
[1,2,3] == [1,2,4]    // false (different values)
[] == []               // true (empty arrays)
[] == [1]              // false
```

## Lists (`{}`)
Lists are compared element-by-element in order. For key-value pairs, keys are compared first, then values.

```grapa
{1,2,3} == {1,2,3}    // true
{1,2} == {1,2,3}      // false (different lengths)
{1,2,3} == {3,2,1}    // false (different order)
{a:1, b:2} == {a:1, b:2}        // true
{a:1, b:2} == {b:2, a:1}        // false (order matters)
{a:1, b:2} == {a:1, b:3}        // false (different values)
{} == {}               // true (empty lists)
{} == {1}              // false
```

## Vectors (`#[ ]#`)
Vectors are compared using their internal structure. Currently supports basic comparison.

```grapa
#[1,2,3]# == #[1,2,3]#    // true
#[1,2]# == #[1,2,3]#      // false (different lengths)
#[1,2,3]# == #[3,2,1]#    // false (different order)
```

## XML Structures
XML structures are compared recursively, element-by-element.

```grapa
xml1 = <root><item>1</item><item>2</item></root>;
xml2 = <root><item>1</item><item>2</item></root>;
xml1 == xml2    // true
```

## Mixed Type Comparisons
Different data structure types are never equal:

```grapa
[1,2,3] == {1,2,3}      // false (array vs list)
[1,2,3] == #[1,2,3]#    // false (array vs vector)
{1,2,3} == #[1,2,3]#    // false (list vs vector)
```

## Nested Structures
Nested data structures are compared recursively:

```grapa
[[1,2],[3,4]] == [[1,2],[3,4]]    // true
[[1,2],[3,4]] == [[1,2],[3,5]]    // false
{a:[1,2], b:{x:1}} == {a:[1,2], b:{x:1}}    // true
```

# Type Handling

All comparison operators now support mixed type comparisons with intelligent type conversion:

- **Numeric strings**: Automatically converted to numbers for comparison
- **Non-numeric strings**: Fall back to string comparison
- **Float vs String**: Uses adaptive tolerance for numeric strings
- **Bool comparisons**: Handled with proper truthiness logic
- **Null comparisons**: Properly handled for equality and ordering
- **PTR types**: Properly dereferenced for comparison

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
