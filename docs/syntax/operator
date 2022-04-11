# Operator

Commands | Results
------------ | -------------
f=op(){3%2};</br>f(); | 1
f=@<[op,@<mod,{3,2}>],{}>;</br>f(); | 1
f=op(a,b){a%b};</br>f(); | @<[op,@<mod,{@<var,{a}>,@<var,{b}>}>],{a,b}>
f(842,5); | 2
f=op(a,b,c){d=a%b;d*c;};</br>f(); | @<[op,@[@<assign,{d,@<mod,{@<var,{a}>,@<var,{b}>}>}>,@<mul,{@<var,{d}>,@<var,{c}>}>]],{a,b,c}>
f(842,5,9); | 18

You can also define an operation by providing a script and a rule. If a rule is not specified, the "start" and "$start" rules are used (which is the default entry point for the grapa language).

Commands | Results
------------ | -------------
op()("4*2"); | @<[op,8],{}>
op()("4*2",@$start); | @<[op,8],{}>
op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){a**b}}); | @<[op,@<[op,@<pow,{@<var,{a}>,@<var,{b}>}>],{"a":4,"b":2}>],{}>
f = op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){a**b}});</br>f(); | 16

Note in the last example the rule to use was defined and passed in as a parameter to the planer, the operation result assigned to a variable, and then the variable executed as a function.

If the rules have already been defined, the following options to generate the execution plan - for either storing as a function or for debugging the rules.

Commands | Results | Description
------------ | ------------- | -------------
`r = rule $INT '*' $INT {op(a:$1,b:$3){a**b}};` |  | Rule for "int * int", with associated code that applies the power operator
f = op()("4*2",r);</br>f(); | 16 | Applying the rule to create a function, and running the function
f = r.plan("4*2");</br>f(); | 16 | Another way to apply the rule
(r.plan("4*2"))(); | 16 | Apply the rule and run the function in the same command

# Function Chaining
Any object that returns an object can be chained.

For example "4.4".float(300,4).pi() will convert the string 4.4 to a float with 300 bit precision and will return pi() to the power of the input, at the precision specified in the input. So, a 400 bit precision of pi to the power of 4.4. 

If an existing function/command doesn't support chaining, an OP can be inserted in the chain to make it work using the $$ variable for the result of the prior operation. For example, the following will get the length of a list, generate that many bits, and output the result in binary form. The 3 examples all accomplish the same result.
```
> {1,2,3,5}.{(@$$).len()}.genbits().bin();
1111

> {1,2,3,5}.{(op(a){a.len()})(@$$)}.genbits().bin();
1111

> f = op(a){a.len()};
> {1,2,3,5}.{f(@$$)}.genbits().bin();
1111
```
