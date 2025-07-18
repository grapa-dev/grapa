# $OP

/*
Design Influences:
Grapa's $OP and $CODE types are inspired by Lisp's list-based, lambda/bytecode execution model, enabling flexible, composable, and introspectable code structures. This design supports advanced meta-programming and dynamic language features, making Grapa inviting and usable for a broad audience—including Python users, educators, and language researchers. The overarching goal is to make Grapa (and GrapaPy) as accessible and powerful as possible for the widest range of users.
*/
This is the core type at the base of grapa. It does not have a class associated. Consider it as both a high level version of byte code (but represented as a list) and similar to a lambda in other languages. It's a variable that includes executable instructions. 

Understanding the inner workings of $OP is not necessary to use grapa - and this section can be skipped. This though is core to how grapa works, and an understanding is needed in order to extend the syntax of the language to support your own domain specific language extensions.

## Basic $OP Creation and Execution

Examples:
Commands | Results | Description
------------ | ------------- | -------------
(op(){4*2})(); | 8 | Creates $OP and executes.
(@<mul,{4,2}>)(); | 8 | Same but using $OP directly.

## Adding Parameters to $OP

Example: The op command is used for passing parameters into a function. When the op command is used, the name is replaced with an array where the first value is "op" and the second value is the function. This array is then followed by the parameter list. The parameters are used to initialize the namespace for the function.

Commands | Results
------------ | -------------
op(){1} | @<[op,1],{}>
op(a,b){a*b}; | @<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{a,b}>
op(){5%2}; | @<[op,@<mod,{5,2}>],{}>
op(){4*2}; | @<[op,8],{}>

In the last example above, the compiler implemented an optimization where it recognized that multiplying two constants could be completed at compile time.

## Execution Tree Structure

The `@<[op,body],params>` format represents the internal structure of a $OP:

- **`op`** - Operation type identifier
- **`body`** - The execution tree (can be nested operations)
- **`params`** - Parameter namespace `{}` containing default values

### Tree Components

| Component | Description | Example |
|-----------|-------------|---------|
| `@<assign,{var,value}>` | Variable assignment | `@<assign,{a,5}>` |
| `@<add,{left,right}>` | Addition operation | `@<add,{5,3}>` |
| `@<var,{name}>` | Variable reference | `@<var,{a}>` |
| `@<search,{object,method}>` | Method call | `@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>` |

## Direct Execution Tree Creation and Execution

You can create and execute $OP trees directly:

```grapa
/* Create a simple execution tree */
compiled = op()("a = 5 + 3; a.echo();");

/* Execute the compiled function */
compiled();
/* Result: 8 */

/* View the execution tree */
compiled;
/* Result: @<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}> */

/* Execute the tree directly */
@<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}>();
/* Result: 8 */
```

## Parameter Binding and Namespace Management

$OP functions can have parameters with default values that create a namespace:

```grapa
/* Create function with parameters and default values */
compiled = op("b"=0,"c"=0)("a = 5 + b + c; a.echo();");

/* Execute with no parameters (uses defaults) */
compiled();
/* Result: 5 */

/* Execute with one parameter */
compiled(3);
/* Result: 8 */

/* Execute with two parameters */
compiled(3,1);
/* Result: 9 */

/* View the execution tree with parameter namespace */
compiled;
/* Result: @<[op,@[@<assign,{a,@<add,{@<add,{5,@<var,{b}>}>,@<var,{c}>}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{"b":0,"c":0}> */

/* Execute the tree directly with parameters */
@<[op,@[@<assign,{a,@<add,{@<add,{5,@<var,{b}>}>,@<var,{c}>}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{"b":0,"c":0}>(3,2);
/* Result: 10 */
```

## Alternative Syntax for Function Creation

You can also create functions using block syntax:

```grapa
/* Using block syntax */
compiled = op("b"=0,"c"=0){a = 5 + b + c; a.echo();};

/* Execute with parameters */
compiled(3,4);
/* Result: 12 */

/* View the execution tree */
compiled;
/* Result: @<[op,@[@<assign,{a,@<add,{@<add,{5,@<var,{b}>}>,@<var,{c}>}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{"b":0,"c":0}> */
```

## Compile-Time Optimization

The Grapa compiler performs various optimizations during tree building:

### Constant Folding
```grapa
op(){4*2}; | @<[op,8],{}>
/* The compiler evaluates 4*2 at compile time */
```

### Expression Simplification
```grapa
op(){5+3}; | @<[op,8],{}>
/* Simple arithmetic is computed at compile time */
```

### Variable Assignment Optimization
```grapa
op(){a=5; a*2}; | @<[op,@[@<assign,{a,5}>,@<mul,{@<var,{a}>,2}>]],{}>
/* Assignment and multiplication preserved for runtime */
```

## Execution Tree Interpretation

When a $OP is executed, the interpreter:

1. **Creates namespace** from the parameter list `{"b":0,"c":0}`
2. **Binds parameters** passed during execution to the namespace
3. **Traverses the tree** executing each operation in sequence
4. **Returns the result** of the last operation

### Tree Traversal Example
```grapa
/* Tree: @<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}> */

/* Execution steps:
   1. Create empty namespace {}
   2. Execute @<assign,{a,@<add,{5,3}>}> → a = 8
   3. Execute @<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}> → 8.echo()
   4. Return result: 8
*/
```

## Advanced Usage Patterns

### Function Composition
```grapa
/* Create base function */
base = op(x){x * 2};

/* Create composed function */
composed = op(x){base(x) + 1};

/* Execute */
composed(5);
/* Result: 11 */
```

### Dynamic Function Creation
```grapa
/* Create function from string */
script = "result = input * 2; result";
dynamic_func = op("input"=0)(script);

/* Execute */
dynamic_func(7);
/* Result: 14 */
```

### Function Storage and Retrieval
```grapa
/* Store function in table */
table = $file().table("ROW");
table.mkfield("func", "OP");
table.set("my_func", op(x){x * x}, "func");

/* Retrieve and execute */
stored_func = table.get("my_func", "func");
stored_func(4);
/* Result: 16 */
```

## Performance Characteristics

- **Compiled functions** execute faster than interpreted scripts
- **Constant folding** reduces runtime computation
- **Tree optimization** improves execution efficiency
- **Parameter binding** is efficient with namespace caching
- **Direct tree execution** bypasses parsing overhead

## Integration with Other Types

$OP integrates with other Grapa types:

- **$CODE**: Sequences of $OP operations
- **$RULE**: Grammar rules that generate $OP trees
- **$TABLE**: Can store $OP objects as data
- **$file**: Can save/load compiled $OP objects

See the section on syntax for additional examples.