# Condition Operators

## if

Syntax options:

* if (bool) statement;
* if (bool) statement; else statement;
* if (bool) statement; elseif statement; else statement;

Example:

```grapa
> if (1==0) "1==0\n".echo();  else "none\n".echo();
none

> if (1==0) "1==0\n".echo(); elseif (2==2) "2==2\n".echo(); else "none\n".echo();
2==2
```

## switch

Grapa's switch statement is **much more powerful** than traditional switch statements. Each `case` can contain a **full expression that returns true/false**, and evaluation stops at the first match.

**Basic switch without braces:**

```grapa
switch (x)
case 1: "One".echo();
case 2: "Two".echo();
default: "Other".echo();
```

**With parentheses (optional):**

```grapa
switch (x)
case (1): "One".echo();
case (2): "Two".echo();
default: "Other".echo();
```

**Complex expressions:**

```grapa
switch (data:{name: "Alice", scores: [85, 92, 78]})
case (data.scores.mean()[0] >= 90): "High performer".echo();
case (data.scores.mean()[0] >= 80): "Good performer".echo();
case (data.scores.min()[0] < 70): "Needs improvement".echo();
default: "Average performer".echo();
```

**Multiple commands in case (use {}):**

```grapa
switch (x)
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
```

### Boolean Expression Matching

```grapa
/* Each case can be a full boolean expression */:
switch (true)
case (x < 0): "Negative".echo();
case (x == 0): "Zero".echo();
case (x > 0): "Positive".echo();
default: "Unknown".echo();
```

### Complex Pattern Matching

```grapa
/* Pattern matching on data structures */
user = {name: "Alice", age: 25, role: "admin"};

switch (true)
case (user.role == "admin" && user.age >= 18): "Admin access granted".echo();
case (user.role == "user" && user.age >= 18): "User access granted".echo();
case (user.age < 18): "Access denied - too young".echo();
default: "Access denied - unknown role".echo();
```

### Variable Declaration in Switch Expression

Grapa's switch statement can declare and initialize variables directly in the switch expression, enabling advanced pattern matching and fuzzy matching:

```grapa
/* Declare variable in switch expression for advanced matching */
switch(a:[1,3,5])
case (a[2]==5): "Array element matches".echo();
default: "No match found".echo();
```

**Advanced Pattern Matching Examples:**

```grapa
/* Fuzzy string matching */
switch(input:"hello world")
case (input.grep("hello", "i")): "Contains 'hello' (case insensitive)".echo();:
case (input.len() > 10): "Long string".echo();
default: "No pattern match".echo();

/* Complex data structure analysis */
switch(data:{name: "Alice", scores: [85, 92, 78]})
case (data.scores.mean()[0] >= 90): "High performer".echo();
case (data.scores.mean()[0] >= 80): "Good performer".echo();
case (data.scores.min() < 70): "Needs improvement".echo();
default: "Average performer".echo();

/* Array pattern matching */
switch(arr:[1,2,3,4,5])
case (arr.len() == 5 && arr[0] == 1): "Five-element array starting with 1".echo();
case (arr.sum() > 10): "Array sum greater than 10".echo();
case (arr.grep(3)): "Array contains 3".echo();
default: "No specific pattern".echo();
```

**Key Benefits:**
- **Local scope**: Variables are only available within the switch statement
- **Advanced matching**: Enables fuzzy matching, pattern recognition, and complex conditions
- **Clean syntax**: No need to declare variables before the switch
- **Temporary variables**: Perfect for one-off calculations in switch logic

### Type-Based Matching

```grapa
/* Match based on data type */
switch (true)
case (data.type() == $STR): "String data".echo();
case (data.type() == $INT): "Integer data".echo();
case (data.type() == $LIST): "Array data".echo();
case (data.type() == $GOBJ): "List data".echo();
default: "Other data type".echo();
```

## Exception Handling: try/catch/finally/throw

Grapa provides comprehensive exception handling with try/catch/finally blocks and throw statements.


**Basic exception handling:**

```grapa
try throw 'error message'
catch 'error message': 'Caught specific error'.echo();
default: 'Caught any other error'.echo();
finally: 'Always executed'.echo();
```

**With semicolon after try:**

```grapa
try throw 'error message';
catch 'error message': 'Caught specific error'.echo();
default: 'Caught any other error'.echo();
finally: 'Always executed'.echo();
```

**Exception with variable binding:**

```grapa
try throw (err:'error message')
catch (err.left(1)=='e'): 'Error starts with e: '.interpolate({msg=err}).echo();
catch (err.len() > 10): 'Long error message'.echo();
default: 'Caught other error'.echo();
finally: 'Cleanup completed'.echo();
```

**Multiple commands in catch (use {}):**

```grapa
try throw (err:'error message')
catch (err.left(1)=='e'): {
    'Error starts with e: '.interpolate({msg=err}).echo();
    'Additional error handling'.echo();
}
catch (err.len() > 10): 'Long error message'.echo();
default: 'Caught other error'.echo();
finally: 'Cleanup completed'.echo();
```

### Object Variable Binding

```grapa
/* Direct object throw - works correctly */
user = {name: 'Alice', age: 30};
try throw (err:user)
default: ('User name: ' + err.name).echo();

/* Nested object with explicit copy - works correctly */
user = {name: 'Alice', age: 30};
try throw (err:{user: user.gobj()})
default: ('User name: ' + err.user.name).echo();

/* Nested object without copy - works correctly */
user = {name: 'Alice', age: 30};
try throw (err:{user: user})  /* Local variable references are now preserved */
default: ('User name: ' + err.user.name).echo();
```

**Note**: Local variable references in thrown objects are now automatically preserved. The `.gobj()` method is no longer required for reliable binding.

### Multiple Catch Blocks

```grapa
/* Multiple catch blocks with different conditions */
try throw 'network error'
catch 'network error': 'Network issue handled'.echo();
catch 'file error': 'File issue handled'.echo();
catch (true): 'Any other error handled'.echo();
finally: 'Resource cleanup'.echo();
```

### Exception Handling Syntax

**Basic throw:**

```grapa
throw 'error message';
throw (err:'error message');  // With variable binding
```

**Modern try/catch/finally syntax:**

```grapa
try throw 'error message'
catch 'error': 'Handle specific error'.echo();
catch (condition): 'Handle based on condition'.echo();
default: 'Handle any other error'.echo();
finally: 'Always execute'.echo();
```

**With semicolon after try:**

```grapa
try throw 'error message';
catch 'error': 'Handle specific error'.echo();
catch (condition): 'Handle based on condition'.echo();
default: 'Handle any other error'.echo();
finally: 'Always execute'.echo();
```

**Key Features:**

* **Variable binding**: Variables declared in throw are accessible in catch blocks
* **Flexible matching**: Catch conditions can be constants, expressions, or complex patterns
* **Multiple catch blocks**: Can have multiple catch conditions
* **Finally block**: Always executes regardless of exception
* **Consistent syntax**: Follows the same pattern as switch/case statements:
* **No parentheses required**: Simple constants don't need parentheses

### Sequential Evaluation (First Match Wins)

```grapa
/* Evaluation stops at first true condition */
switch (true)
case (x > 10): "Greater than 10".echo();  /* If x=15, only this executes */
case (x > 5): "Greater than 5".echo();    /* This won't execute if above is true */
case (x > 0): "Greater than 0".echo();    /* This won't execute if above is true */
default: "Zero or negative".echo();
```

**Key Features:**

* **Implicit break**: No break statements required, execution stops after first match
* **Multiple commands**: Must be enclosed in `{}` when case contains multiple statements:
* **Full expressions**: Each case can contain complex boolean expressions:
* **Sequential evaluation**: Evaluates from first to last case, stops at first match
* **No fall-through**: Unlike C-style switches, Grapa doesn't fall through to next case
* **Pattern matching**: Can match on data structure patterns, types, and conditions
* **Default case**: Optional catch-all for unmatched conditions

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
