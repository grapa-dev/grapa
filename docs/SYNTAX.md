# Default Language Rules
To view a text version of the rules loaded into grapa on startup, run the following at the prompt:

```
> $sys().getenv("$STATICLIB")."$grapa.grc".str()
```

The system will first check for a match on the "start" rule. Which is a global variable...if that global variable is of type $RULE, than it will be come the first rule for scripts. This is an easy way to provide an override on command processing. If "start" rule does not provide a match, than the system will evaluage using the "$start" rule. 

The default rules may be subject to change, and so are not listed here. Use the above command to retrieve/view.  To write the result to a file, use the following:

```
> $file().set("grapa.txt",$sys().getenv("$STATICLIB")."$grapa.grc".str())
```

This will place the file in the current working directory. If you want to place in a different directly, refer to the $file class for commands to do that.

If you modify the file, you can try it out by placing it in "lib/grapa/" under the same directory that the above command would have written the file to, and then restart grapa. If grapa finds this file with this name in that location, it will use that file instead of the default. 

If you are familiar with YACC or BNF, following the grammar logic should be rather straight forward. Each rule is evaluated until a successful match, and all matches produce an execution play where the code associated with each rule match is included in the execution plan. To optimize grammar resolution, rules carefully constructed to avoid re-evaluating rules more than necessary by placing the most complex rules first and following with common rule patters. It is also important to avoid infinite recursive situations - mostly this is avoided by not referencing the same rule as the first token to evaluate. 

There are a few standard tokens that are defined - mostly the tokens provide special handling for either the lexical engine or the rules engine. One example is the space character, which when found a bit is set in the token skip the value for rules evaluation. This can be changed - but then would require including including the space token in the rules. 

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

# Operator

Commands | Results
------------ | -------------
f=op(){3%2};</br>f(); | 1
f=@<[op,@<mod,{3,2}>],{}>;</br>f(); | 1
f=op(a,b){@a%@b};</br>f(); | @<[op,@<mod,{@<var,{a}>,@<var,{b}>}>],{a,b}>
f(842,5); | 2
f=op(a,b,c){d=@a%@b;@d*@c;};</br>f(); | @<[op,@[@<assign,{d,@<mod,{@<var,{a}>,@<var,{b}>}>}>,@<mul,{@<var,{d}>,@<var,{c}>}>]],{a,b,c}>
f(842,5,9); | 18

You can also define an operation by providing a script and a rule. If a rule is not specified, the "start" and "$start" rules are used (which is the default entry point for the grapa language).

Commands | Results
------------ | -------------
op()("4*2"); | @<[op,8],{}>
op()("4*2",@$start); | @<[op,8],{}>
op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){@a**@b}}); | @<[op,@<[op,@<pow,{@<var,{a}>,@<var,{b}>}>],{"a":4,"b":2}>],{}>
f = op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){@a**@b}});</br>f(); | 16

Note in the last example the rule to use was defined and passed in as a parameter to the planer, the operation result assigned to a variable, and then the variable executed as a function.

If the rules have already been defined, the following options to generate the execution plan - for either storing as a function or for debugging the rules.

Commands | Results | Description
------------ | ------------- | -------------
`r = rule $INT '*' $INT {op(a:$1,b:$3){@a**@b}};` |  | Rule for "int * int", with associated code that applies the power operator
f = op()("4*2",@r);</br>f(); | 16 | Applying the rule to create a function, and running the function
f = @r.plan("4*2");</br>f(); | 16 | Another way to apply the rule
(@r.plan("4*2"))(); | 16 | Apply the rule and run the function in the same command

# Function Chaining
Any object that returns an object can be chained.

For example "4.4".float(300,4).pi() will convert the string 4.4 to a float with 300 bit precision and will return pi() to the power of the input, at the precision specified in the input. So, a 400 bit precision of pi to the power of 4.4. 

If an existing function/command doesn't support chaining, an OP can be inserted in the chain to make it work using the $$ variable for the result of the prior operation. For example, the following will get the length of a list, generate that many bits, and output the result in binary form. The 3 examples all accomplish the same result.
```
> {1,2,3,5}.{(@$$).len()}.genbits().bin()
1111

> {1,2,3,5}.{(op(a){@a.len()})(@$$)}.genbits().bin()
1111

> f = op(a){@a.len()};
> {1,2,3,5}.f(@$$).genbits().bin();
1111
```
