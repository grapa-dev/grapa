# Function Operators
## op
Creating an $OP. See [$OP type](../type/op.md).

### Parameter Definition and Matching

Grapa's `op` function supports flexible parameter definition and calling with both traditional programming syntax and JSON-style syntax.

#### Parameter Definition
```grapa
/* Basic parameter definition */
f = op(a, b) { a + b; };

/* Parameters with default values */
f = op(a=1, b=2) { b**a; };

/* Mixed default and non-default parameters */
f = op(a, b=10, c) { a + b + c; };
```

#### Parameter Calling

**Named Parameters (Traditional Syntax):**
```grapa
f = op(a=1, b=2) { b**a; };
f(a=3, b=4);     /* → 64 */
f(b=5, a=2);     /* → 25 */
```

**Named Parameters (JSON-Style Syntax):**
```grapa
f = op(a=1, b=2) { b**a; };
f(a:3, b:4);     /* → 64 */
f(b:5, a:2);     /* → 25 */
```

**Positional Parameters:**
```grapa
f = op(a=1, b=2) { b**a; };
f(3, 4);         /* → 64 (a=3, b=4) */
f(5, 2);         /* → 32 (a=5, b=2) */
```

**Mixed Named and Positional:**
```grapa
f = op(a=1, b=2, c=3) { a + b + c; };
f(10, b:20);     /* → 33 (a=10, b=20, c=3) */
f(c:30, 5);      /* → 37 (a=5, b=2, c=30) */
```

#### Parameter Matching Rules

1. **Named Parameters**: Can use either `=` or `:` syntax
2. **Positional Parameters**: Assigned in order of definition
3. **Mixed Usage**: Named parameters can be specified in any order, positional parameters fill remaining slots
4. **Default Values**: Used when parameters are not provided
5. **JSON Compatibility**: `:` syntax allows seamless integration with JSON objects

#### Examples

```grapa
/* Complex parameter example */
calculator = op(operation="add", x=0, y=0) {
    if (operation == "add") { x + y; }
    else if (operation == "multiply") { x * y; }
    else if (operation == "power") { x**y; }
    else { 0; }
};

/* Various calling styles */
calculator(5, 3);                    /* → 8 (add) */
calculator(operation:"multiply", 5, 3); /* → 15 */
calculator(x:10, y:2, operation:"power"); /* → 100 */
calculator(operation="power", y:3, x:2);  /* → 8 */
```

### Parameter Behavior and Side Effects

Grapa uses pass-by-reference for all function parameters to maintain performance and avoid implementing a full garbage collection system. Default parameter values are immutable (each call gets a fresh copy), while passed parameters remain mutable.

#### Default Value Immutability

Default values are now immutable - each function call gets a fresh copy:

```grapa
counter = op(count=0) {
    count += 1;
    count;
};

counter();  /* 1 */
counter();  /* 1 - default value is not modified */
```

#### Parameter Modification

Parameters can be modified within functions, affecting the original values:

```grapa
increment = op(x) {
    x += 1;
    x;
};

value = 5;
increment(value);  /* Returns 6 */
value;            /* Now 6 */
```

#### When This Is Useful

This behavior is intentional and useful for:
- **Performance optimization** - avoiding unnecessary copying
- **In-place operations** - modifying data structures efficiently
- **C++-style programming** - similar to pass-by-reference

#### When to Be Careful

Be cautious when:
- **Default values are immutable** - each call gets a fresh copy
- **Passing shared data** - modifications affect all references
- **Writing pure functions** - avoid unintended side effects

#### Workaround for Value Copying

Use a copy function when you need to avoid side effects:

```grapa
/* Copy function to force value copying */
copy = op(x) { x; };

data = [1, 2, 3];
process = op(arr) {
    arr[0] = 999;
    arr;
};

process(copy(data));  /* Returns [999, 2, 3] */
data;                /* Still [1, 2, 3] - original unchanged */
```

## class
Creates a class that can be used to generate an instance of the class. The class definition is shared between all instances using the class. If information in the class is altered, a copy is made and the modified variable is added to the instance. The instance stores the class reference and any variables local to the instance. Classes can inherit 1 or more other classes. 

```
myC = class {myV = 0; myF = op(a){myV=a};};
myC2 = class (myC) {myV2 = 0; myF2 = op(a){myV2=a};};
myIns = obj myC2;
@myIns;
{}

myIns.myF(4);
myIns.myV;
4

myIns.myV2;
0

myIns;
{"myV":4}
```

## obj
Creating an instance of a class. See class.

## token
Used to define lexical rules. Currently only used to define special character handling like space and tab, where the default handling is to skip these characters so they do not need to be specified in the grammar. 

## rule
See [$RULE type](../type/rule.md)

## const
Sets a bit in the variable that prevents it from being changed. Will not be locked when accessed, which is useful for a global variable accssed by concurent threads.

```
x = const {one: "string one", two: "string two"};
```

Once set, the variable can not be modified. To turn off the const bit, use the following:
```
x.setconst(false);
x += (three:"th str");
x.setconst(true);
```

**Note:** `const` is intended for performance (caching/optimization), not for locking or thread safety. It prevents modification unless `setconst` is used, but does not synchronize access between threads. For explicit locking, see [$thread](../sys/thread.md).

## static
Compile-time option. The code to construct the array (or value) is executed at compile time (during script loading), not at runtime. This reduces repeated computation and improves performance, but does not affect thread safety or immutability.

```
f = op(s)
{
  x = static {one: "string one", two: "string two"};
  x[s];
};
f("two");
```

Here is the compiled function with static:
```
@<[op,@[@<assign,{x,{"one":"string one","two":"string two"}}>,@<search,{@<var,{x}>,@<createlist,{@<var,{s}>}>}>]],{s}>
```

Here is the compiled funciton without static:
```
@<[op,@[@<assign,{x,@<prepend,{@<createlist,{@<name,{two,"string two"}>}>,@<name,{one,"string one"}>}>}>,@<search,{@<var,{x}>,@<createlist,{@<var,{s}>}>}>]],{s}>
```

If the list needs to be constructed using a variable that is passed in, than do not use the static operator and the variable will be constructed at run time.

**Note:** `static` is for compile-time evaluation only. It does not make a value immutable or thread safe. For thread safety and locking, see [$thread](../sys/thread.md).

## literal
Any sequence starting with an ascii letter, and including numbers and '_'. 

## literal ([params])
Assumes literal is a variable reference for $OP or $CODE, and runs the routine passing in parameters specified. See $OP and $CODE.

```

---

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
