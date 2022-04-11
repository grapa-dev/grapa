# Precedence

| Operator| Description | Associativity
|--|--|--|
| `a` `@a` | token, contents of token | right->left |
| `a()` | function call | right->left |
| `a.b` `a[b]` | lookup | right->left |
| `+a` `-a` `!a` `~a` | Unary plus, minus, not, bitwise not | right->left |
| `a*b` `a/b` `a%b` | mult, div, rem | left->right |
| `a+b` `a-b` | add, sub | left->right |
| `<<` `>>` | bitwise shift | left->right |
| `<=>` | 3 way compare | left->right |
| `<` `<=` `>` `>=` | Lt, LtEq, Gt, GtEq | left->right |
| `==` `!=` | eq, neq | left->right |
| `&` | bitwise and | N/A |
| `^` | bitwise xor | N/A |
| \| | bitwise or | N/A |
| `&&` | and | N/A |
| \|\| | or | N/A |
| `a?b:c` `a?b:c:d` | Ternary conditionals | right->left |
| `=` `+=` `-=` `++=` | Assignments | right->left |
