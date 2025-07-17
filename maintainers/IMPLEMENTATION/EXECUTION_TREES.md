---
tags:
  - maintainer
  - lowlevel
  - implementation
  - execution
---

# Execution Trees in Grapa

## Overview

Grapa uses an execution tree model where all code compiles to `$OP` objects with the format `@<[op,body],params>`. This tree structure serves as Grapa's equivalent of bytecode, but is more flexible and can be directly executed. Understanding execution trees is essential for language extension development, performance optimization, and debugging.

## Tree Structure

### Basic Format

```grapa
@<[op,body],params>
```

- **`op`**: Operation type identifier
- **`body`**: Execution tree (can be nested operations)
- **`params`**: Parameter namespace with default values

### Tree Components

| Component | Description | Example |
|-----------|-------------|---------|
| `@<assign,{var,value}>` | Variable assignment | `@<assign,{a,5}>` |
| `@<add,{left,right}>` | Addition operation | `@<add,{5,3}>` |
| `@<var,{name}>` | Variable reference | `@<var,{a}>` |
| `@<search,{object,method}>` | Method call | `@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>` |
| `@<while,{condition,body}>` | While loop | `@<while,{@<lteq,{@<var,{i}>,10}>},@<scope,{body}>}>` |
| `@<scope,{body}>` | Scope block | `@<scope,{@[operations]}>` |

## From High-Level to Execution Tree

### Simple Example

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

### Complex Example

```grapa
/* High-level syntax */
compiled = op("b"=0,"c"=0)("a = 5 + b + c; a.echo();");

/* Execute with different parameter combinations */
compiled();      /* Uses defaults: b=0, c=0 → Result: 5 */
compiled(3);     /* b=3, c=0 → Result: 8 */
compiled(3,1);   /* b=3, c=1 → Result: 9 */

/* View the execution tree */
compiled;
/* Result: @<[op,@[@<assign,{a,@<add,{@<add,{5,@<var,{b}>}>,@<var,{c}>}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{"b":0,"c":0}> */
```

## Compilation and Optimization Process

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

## Optimization Examples

### Constant Folding
```grapa
/* Arithmetic with constants */
op(){4 * 2}; | @<[op,8],{}>
op(){5 + 3}; | @<[op,8],{}>
op(){10 / 2}; | @<[op,5],{}>
```

### Expression Simplification
```grapa
/* Complex constant expressions */
op(){2 * 3 + 4}; | @<[op,10],{}>
op(){(5 + 3) * 2}; | @<[op,16],{}>
```

### Variable Preservation
```grapa
/* Variables preserved for runtime */
op(x){x * 2}; | @<[op,@<mul,{@<var,{x}>,2}>],{x}>
op(a,b){a + b}; | @<[op,@<add,{@<var,{a}>,@<var,{b}>}>],{a,b}>
```

## Tree Operations and Components

### Assignment Operations
```grapa
@<assign,{variable,value}>      /* Variable assignment */
@<assignappend,{variable,value}>  /* Append assignment */
```

### Arithmetic Operations
```grapa
@<add,{left,right}>     /* Addition */
@<sub,{left,right}>     /* Subtraction */
@<mul,{left,right}>     /* Multiplication */
@<div,{left,right}>     /* Division */
@<mod,{left,right}>     /* Modulo */
@<pow,{base,exponent}>  /* Power */
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
@<if,{condition,then,else}>  /* If statement */
```

## Parameter Binding and Namespace Management

### Parameter Namespace
```grapa
/* Function with parameters */
f = op("x"=0,"y"=0){x + y};

/* View parameter namespace */
f;
/* Result: @<[op,@<add,{@<var,{x}>,@<var,{y}>}>],{"x":0,"y":0}> */

/* Execute with parameters */
f(5,3);
/* Result: 8 */
```

### Namespace Binding Process
1. **Create namespace** from parameter list `{"x":0,"y":0}`
2. **Bind parameters** passed during execution
3. **Execute tree** with bound namespace
4. **Return result**

## Direct Tree Execution

You can execute trees directly without creating functions:

```grapa
/* Direct execution of simple tree */
@<[op,8],{}>();
/* Result: 8 */

/* Direct execution with parameters */
@<[op,@<add,{@<var,{x}>,@<var,{y}>}>],{"x":0,"y":0}>(5,3);
/* Result: 8 */

/* Complex tree execution */
@<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}>();
/* Result: 8 */
```

## Integration with System Functions

### Compilation Integration
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

### Rule-Based Compilation
```grapa
/* Create custom rule */
r = rule $INT '*' $INT {op(a:$1,b:$3){a**b}};

/* Apply rule to create function */
f = op()("4*2", r);
f();
/* Result: 16 */

/* Alternative syntax */
f = r.plan("4*2");
f();
/* Result: 16 */
```

## Advanced Usage Patterns

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

### Performance Characteristics
- **Compiled functions** execute faster than interpreted scripts
- **Constant folding** reduces runtime computation
- **Tree optimization** improves execution efficiency
- **Parameter binding** is efficient with namespace caching
- **Direct tree execution** bypasses parsing overhead

## Debugging Execution Trees

### Viewing Tree Structure
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

### Tree Traversal Debugging
```grapa
/* Tree: @<[op,@[@<assign,{a,@<add,{5,3}>}>,@<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}>]],{}> */

/* Execution steps:
   1. Create empty namespace {}
   2. Execute @<assign,{a,@<add,{5,3}>}> → a = 8
   3. Execute @<search,{@<var,{a}>,@<createlist,{@<name,{echo,null}>}>}> → 8.echo()
   4. Return result: 8
*/
```

## Best Practices

1. **Use high-level syntax** for most programming tasks
2. **Access execution trees** for debugging and optimization
3. **Leverage compile-time optimization** by using constants where possible
4. **Understand tree structure** for language extension development
5. **Use direct tree execution** for performance-critical code
6. **Document complex trees** for maintainability
7. **Test tree execution** independently of high-level syntax

## Conclusion

Execution trees are the foundation of Grapa's execution model. While most users work with high-level syntax, understanding execution trees is essential for:

- Language extension development
- Performance optimization
- Debugging complex code
- Meta-programming applications
- Understanding Grapa's architecture

The execution tree model provides both the flexibility of interpreted code and the performance benefits of compiled code, making Grapa suitable for both rapid development and high-performance applications.

## Related Documentation

- [$OP Type](type/OP.md) - Core $OP type documentation
- [$CODE Type](type/CODE.md) - Sequence of operations
- [Base Types](BASETYPES.md) - Foundation types overview
- [Operator Syntax](syntax/operator.md) - Operator usage patterns
- [System Functions](sys/sys.md) - Compilation and evaluation functions 