# Operator

## Basic Function Creation and Execution

Commands | Results
------------ | -------------
f=op(){3%2};</br>f(); | 1
f=@<[op,@<mod,{3,2}>],{}>;</br>f(); | 1
f=op(a,b){a%b};</br>f(); | @<[op,@<mod,{@<var,{a}>,@<var,{b}>}>],{a,b}>
f(842,5); | 2
f=op(a,b,c){d=a%b;d*c;};</br>f(); | @<[op,@[@<assign,{d,@<mod,{@<var,{a}>,@<var,{b}>}>}>,@<mul,{@<var,{d}>,@<var,{c}>}>]],{a,b,c}>
f(842,5,9); | 18

## Direct Execution Tree Creation and Execution

You can create and execute $OP trees directly:

```grapa
/* Create function from string */
compiled = op()("a = 5 + 3; a.echo();");

/* Execute the function */
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

Functions can have parameters with default values:

```grapa
/* Create function with parameters and default values */
compiled = op("b"=0,"c"=0)("a = 5 + b + c; a.echo();");

/* Execute with different parameter combinations */
compiled();      /* Uses defaults: b=0, c=0 → Result: 5 */
compiled(3);     /* b=3, c=0 → Result: 8 */
compiled(3,1);   /* b=3, c=1 → Result: 9 */

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

## Script and Rule-Based Function Creation

You can also define an operation by providing a script and a rule. If a rule is not specified, the "start" and "$start" rules are used (which is the default entry point for the grapa language).

Commands | Results
------------ | -------------
op()("4*2"); | @<[op,8],{}>
op()("4*2",@$start); | @<[op,8],{}>
op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){a**b}}); | @<[op,@<[op,@<pow,{@<var,{a}>,@<var,{b}>}>],{"a":4,"b":2}>],{}>
f = op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){a**b}});</br>f(); | 16

Note in the last example the rule to use was defined and passed in as a parameter to the planer, the operation result assigned to a variable, and then the variable executed as a function.

## Rule-Based Function Creation

If the rules have already been defined, the following options to generate the execution plan - for either storing as a function or for debugging the rules.

Commands | Results | Description
------------ | ------------- | -------------
`r = rule $INT '*' $INT {op(a:$1,b:$3){a**b}};` |  | Rule for "int * int", with associated code that applies the power operator
f = op()("4*2",r);</br>f(); | 16 | Applying the rule to create a function, and running the function
f = r.plan("4*2");</br>f(); | 16 | Another way to apply the rule
(r.plan("4*2"))(); | 16 | Apply the rule and run the function in the same command

## Compile-Time Optimization Examples

The Grapa compiler performs various optimizations during tree building:

```grapa
/* Constant folding - arithmetic computed at compile time */
op(){4*2}; | @<[op,8],{}>

/* Expression simplification */
op(){5+3}; | @<[op,8],{}>

/* Variable assignment preserved for runtime */
op(){a=5; a*2}; | @<[op,@[@<assign,{a,5}>,@<mul,{@<var,{a}>,2}>]],{}>

/* Complex expression with variables */
op(x){x*2+1}; | @<[op,@<add,{@<mul,{@<var,{x}>,2}>},1}>],{x}>
```

## Execution Tree Interpretation

When a $OP is executed, the interpreter follows these steps:

1. **Create namespace** from the parameter list `{"b":0,"c":0}`
2. **Bind parameters** passed during execution to the namespace
3. **Traverse the tree** executing each operation in sequence
4. **Return the result** of the last operation

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

## Advanced Function Patterns

### Function Composition
```grapa
/* Create base functions */
double = op(x){x * 2};
add_one = op(x){x + 1};

/* Compose functions */
composed = op(x){add_one(double(x))};
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

## Performance Characteristics

- **Compiled functions** execute faster than interpreted scripts
- **Constant folding** reduces runtime computation
- **Tree optimization** improves execution efficiency
- **Parameter binding** is efficient with namespace caching
- **Direct tree execution** bypasses parsing overhead

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
