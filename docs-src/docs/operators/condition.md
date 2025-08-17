# Condition Operators

## if

Syntax options:
* if (bool) statement;
* if (bool) statement; else statement;
* if (bool) statement; elseif statement; else statement;

Example:
```
> if (1==0) "1==0\n".echo();  else "none\n".echo();
none

> if (1==0) "1==0\n".echo(); elseif (2==2) "2==2\n".echo(); else "none\n".echo();
2==2
```

## switch

Syntax options:
* switch (item) {case condition1: statement; case condition2: statement; etc...;};
* switch (item) {case condition1: statement; case condition2: statement; etc...; default: statement;};

Grapa's switch statement is **much more powerful** than traditional switch statements. Each `case` can contain a **full expression that returns true/false**, and evaluation stops at the first match.

### Basic Value Matching
```grapa
/* Traditional value matching */
switch (x) {
    case 1: "One".echo();
    case 2: "Two".echo();
    default: "Other".echo();
};
```

### Boolean Expression Matching
```grapa
/* Each case can be a full boolean expression */
switch (true) {
    case (x < 0): "Negative".echo();
    case (x == 0): "Zero".echo();
    case (x > 0): "Positive".echo();
    default: "Unknown".echo();
};
```

### Complex Pattern Matching
```grapa
/* Pattern matching on data structures */
user = {name: "Alice", age: 25, role: "admin"};

switch (true) {
    case (user.role == "admin" && user.age >= 18): "Admin access granted".echo();
    case (user.role == "user" && user.age >= 18): "User access granted".echo();
    case (user.age < 18): "Access denied - too young".echo();
    default: "Access denied - unknown role".echo();
};
```

### Type-Based Matching
```grapa
/* Match based on data type */
switch (true) {
    case (data.type() == $STR): "String data".echo();
    case (data.type() == $INT): "Integer data".echo();
    case (data.type() == $ARRAY): "Array data".echo();
    case (data.type() == $LIST): "List data".echo();
    default: "Other data type".echo();
};
```

### Sequential Evaluation (First Match Wins)
```grapa
/* Evaluation stops at first true condition */
switch (true) {
    case (x > 10): "Greater than 10".echo();  /* If x=15, only this executes */
    case (x > 5): "Greater than 5".echo();    /* This won't execute if above is true */
    case (x > 0): "Greater than 0".echo();    /* This won't execute if above is true */
    default: "Zero or negative".echo();
};
```

**Key Features:**
- **Full expressions**: Each case can contain complex boolean expressions
- **Sequential evaluation**: Evaluates from first to last case, stops at first match
- **No fall-through**: Unlike C-style switches, Grapa doesn't fall through to next case
- **Pattern matching**: Can match on data structure patterns, types, and conditions
- **Default case**: Optional catch-all for unmatched conditions

## `?`

Syntax options:
* bool ? statement for true;
* bool ? statement for true : statement for false;
* bool ? : statement for false;
* (<0)|(0)|(>0) ? statement for -1 : statement for 0 : statement for 1;

Example:
```
> 1?hi:by
hi

> 0?hi:by
by

> -55?hi:by:there
hi

> ("a"<=>"b")?a:e:b
a
```

---

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
