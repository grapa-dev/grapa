# Operator Precedence in Grapa

Operator precedence determines the order in which parts of an expression are evaluated. Understanding precedence is essential for writing correct and predictable Grapa code, especially in complex expressions.

**Key Points:**
- Higher precedence operators are evaluated before lower precedence ones.
- Parentheses `()` can always be used to override precedence and clarify intent.
- All statements and blocks in Grapa must end with a semicolon (`;`).
- Only block comments (`/* ... */`) are allowed.

---

## How to Read This Table
- **Operator:** The symbol(s) or syntax for the operation.
- **Description:** What the operator does.
- **Associativity:** The direction in which operators of the same precedence are evaluated (left-to-right or right-to-left).

---

| Operator | Description | Associativity |
|--|--|--|
| `a` `@a` | token, contents of token | right-to-left |
| `a()` | function call | right-to-left |
| `a.b` `a[b]` | lookup | right-to-left |
| `+a` `-a` `!a` `~a` | Unary plus, minus, not, bitwise not | right-to-left |
| `a*b` `a/b` `a%b` | multiply, divide, remainder | left-to-right |
| `a+b` `a-b` | add, subtract | left-to-right |
| `<<` `>>` | bitwise shift | left-to-right |
| `<=>` | 3-way compare | left-to-right |
| `<` `<=` `>` `>=` | less/greater than (or equal) | left-to-right |
| `==` `!=` | equal, not equal | left-to-right |
| `&` | bitwise and | left-to-right |
| `^` | bitwise xor | left-to-right |
| `|` | bitwise or | left-to-right |
| `&&` | logical and | left-to-right |
| `||` | logical or | left-to-right |
| `a?b:c` `a?b:c:d` | ternary conditionals | right-to-left |
| `=` `+=` `-=` `++=` | assignments | right-to-left |

---

## Examples

```grapa
result = 2 + 3 * 4;      /* result is 14, not 20 */
result = (2 + 3) * 4;    /* result is 20 */
value = 5;
value += 2 * 3;          /* value is 11, not 21 */
flag = 1 < 2 && 3 > 2;   /* flag is true */
```

---

## Grapa-Specific Notes
- Every statement and block must end with a semicolon (`;`).
- Only block comments (`/* ... */`) are allowed.
- Use parentheses to clarify precedence in complex expressions.
- For more on syntax, see the [Syntax Quick Reference](basic_syntax.md).

---

## Related Documentation
- [Syntax Quick Reference](basic_syntax.md)
- [Operator Reference](operator.md)
