# Base Types

References:
- [$OP](type/OP.md)
- [$CODE](type/CODE.md)

## Overview

$OP and $CODE provide the foundation of Grapa's execution model. While most users interact with the high-level language defined in `$grapa.grc`, all syntax ultimately compiles down to combinations of these two core types, which support an execution tree linking library calls.

## For Most Users

If you're new to Grapa, you typically won't need to work directly with $OP and $CODE. Instead, focus on the high-level language features described in the [Getting Started](GETTING_STARTED.md) guide and [Examples](EXAMPLES.md).

## For Language Extenders

$OP and $CODE are essential for understanding how to extend the language or create custom grammar rules. An $OP represents a single operation (like a function call), while a $CODE represents a sequence of operations.

## Execution Tree Architecture

Grapa uses an execution tree model where all code compiles to `$OP` objects with the format `@<[op,body],params>`. This tree structure serves as Grapa's equivalent of bytecode, but is more flexible and can be directly executed.

### Tree Structure Components

```grapa
@<[op,body],params>
```

- **`op`**: Operation type identifier
- **`body`**: Execution tree (can be nested operations)
- **`params`**: Parameter namespace with default values

### Example: From High-Level to Execution Tree

```grapa
/* High-level syntax */
f = op(x){x * 2 + 1};

/* Execution tree representation */
f;
/* Result: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}> */

/* Direct execution */
f(5);
/* Result: 11 */

/* Direct tree execution */
@<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}>(5);
/* Result: 11 */
```

## Compilation and Optimization Process

During compilation, Grapa's BNF grammar generates execution trees while applying optimizations:

### 1. Parse Phase
High-level syntax is parsed according to grammar rules in `$grapa.grc`.

### 2. Tree Building
The parser constructs execution trees using `$OP` and `$CODE` objects.

### 3. Optimization Phase
As the tree is built, optimization functions discover and optimize patterns:

```grapa
/* Before optimization */
op(){4 * 2}; 
/* Tree: @<[op,@<mul,{4,2}>],{}> */

/* After optimization (constant folding) */
op(){4 * 2};
/* Tree: @<[op,8],{}> */
```

### 4. Execution
The optimized tree is executed by the interpreter.

## Function Creation and Parameter Binding

Functions in Grapa are `$OP` objects that can be assigned to variables and executed:

```grapa
/* Create function with parameters */
compiled = op("b"=0,"c"=0)("a = 5 + b + c; a.echo();");

/* Execute with different parameter combinations */
compiled();      /* Uses defaults: b=0, c=0 → Result: 5 */
compiled(3);     /* b=3, c=0 → Result: 8 */
compiled(3,1);   /* b=3, c=1 → Result: 9 */

/* View the execution tree */
compiled;
/* Result: @<[op,@[@<assign,{a,@<add,{@<add,{5,@<var,{b}>}>,@<var,{c}>}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{"b":0,"c":0}> */
```

## The following creates an $OP that takes a parameter x and adds the numbers from 1 to x, and returns the result. 

```grapa
op(x){i=1;n=0;while(i<=x){n+=i;i+=1;};n;}
```

**Execution Tree:**
```
@<
  [op,
    @[
      @<assign,{i,1}>,
      @<assign,{n,0}>,
      @<while,
      {
        @<lteq,
        {
          @<var,{i}>,
          @<var,{x}>
        }>,
        @<scope,
        {
          @[
            @<assignappend,{n,@<var,{i}>}>,
            @<assignappend,{i,1}>
          ]
        }>
      }>,
      @<var,{n}>
    ]
  ],
  {x}
>
```

## Tree Operations and Components

### Assignment Operations
```grapa
@<assign,{variable,value}>  /* Variable assignment */
@<assignappend,{variable,value}>  /* Append assignment */
```

### Arithmetic Operations
```grapa
@<add,{left,right}>     /* Addition */
@<sub,{left,right}>     /* Subtraction */
@<mul,{left,right}>     /* Multiplication */
@<div,{left,right}>     /* Division */
@<mod,{left,right}>     /* Modulo */
```

### Variable References
```grapa
@<var,{name}>  /* Variable reference */
```

### Method Calls
```grapa
@<search,{object,@<createlist,{@<name,{method,null}>}>}>  /* Method call */
```

### Control Flow
```grapa
@<while,{condition,body}>  /* While loop */
@<scope,{body}>           /* Scope block */
```

## Integration with System Functions

$OP objects integrate with `$sys` functions:

```grapa
/* Compile script to $OP */
compiled = $sys().compile("a = 5 + 3; a.echo();");

/* Execute compiled $OP */
$sys().eval(compiled);
/* Result: 8 */

/* Direct $OP creation */
direct_op = op()("a = 5 + 3; a.echo();");
direct_op();
/* Result: 8 */
```

## Performance and Optimization

### Compile-Time Optimizations
- **Constant folding**: `4 * 2` → `8`
- **Expression simplification**: `5 + 3` → `8`
- **Dead code elimination**: Remove unreachable code
- **Function inlining**: Inline small functions

### Runtime Optimizations
- **Namespace caching**: Parameter namespaces are cached
- **Tree traversal optimization**: Efficient tree walking
- **Memory management**: Automatic cleanup of temporary objects

## Advanced Usage

### Dynamic Code Generation
```grapa
/* Generate code dynamically */
operation = "add";
left = 5;
right = 3;
code = left + " " + operation + " " + right;
func = op()(code);
result = func();
/* Result: 8 */
```

### Function Composition
```grapa
/* Compose functions */
double = op(x){x * 2};
add_one = op(x){x + 1};
composed = op(x){add_one(double(x))};
composed(5);
/* Result: 11 */
```

### Meta-Programming
```grapa
/* Create functions from data */
operations = ["add", "sub", "mul"];
funcs = [];
i = 0;
while (i < operations.len()) {
    op_name = operations.get(i);
    func = op(a,b)("a " + op_name + " b");
    funcs.append(func);
    i = i + 1;
}

/* Execute generated functions */
funcs.get(0)(5,3);  /* add: 8 */
funcs.get(1)(5,3);  /* sub: 2 */
funcs.get(2)(5,3);  /* mul: 15 */
```

## Debugging Execution Trees

To debug execution trees:

```grapa
/* View the tree structure */
func = op(x,y){x * y + 1};
func;
/* Shows the complete execution tree */

/* Execute step by step */
simple = op(){5 + 3};
simple;
/* Shows: @<[op,8],{}> (optimized) */

complex = op(x){x * 2 + 1};
complex;
/* Shows: @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}> */
```

## Best Practices

1. **Use high-level syntax** for most programming tasks
2. **Access execution trees** for debugging and optimization
3. **Leverage compile-time optimization** by using constants where possible
4. **Understand tree structure** for language extension development
5. **Use direct tree execution** for performance-critical code

## Conclusion

$OP and $CODE provide the foundation for Grapa's execution model. While most users work with high-level syntax, understanding these base types is essential for:

- Language extension development
- Performance optimization
- Debugging complex code
- Meta-programming applications
- Understanding Grapa's architecture

The execution tree model provides both the flexibility of interpreted code and the performance benefits of compiled code, making Grapa suitable for both rapid development and high-performance applications.

## Function-Local Variables and $local.

In Grapa, use the `$local.` prefix only on the first assignment of a variable that should be local to a function. All subsequent references in the same function can use the variable name directly. This ensures the variable is scoped to the current function and prevents accidental shadowing or overwriting of variables from parent or calling functions. This is especially important for common variable names like `total`, `sum`, `count`, etc.

Example:

```grapa
test_sum = op() {
    $local.total = 0;
    $local.i = 0;
    while (i < 10) {
        total = total + i;
        i = i + 1;
    };
    total;
};
```

This is analogous to using `var` or `let` in JavaScript, or local variable declarations in other languages.

**Best Practice:** Always use `$local.` for the first assignment of variables that should not leak outside the function or collide with variables in other scopes. After the first assignment, use the variable name directly.

> **Warning:** Only use `$local.` on the first assignment at the top of a function. Do **not** use `$local.` inside loops, if-blocks, or other inner blocks. Each `{ ... }` block creates a new namespace in Grapa, so using `$local.` inside a loop or block will create a new variable that shadows the outer one, leading to bugs. Always declare all your function-local variables with `$local.` at the top of the function, and use the variable name directly everywhere else.
