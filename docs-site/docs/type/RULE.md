# $RULE

/*
Design Influences:
Grapa's $RULE type is inspired by Prolog's executable BNF and logic programming paradigm, enabling dynamic, mutable grammars and rule-based execution. This design empowers users to experiment with language syntax and parsing, making Grapa especially valuable for educators, researchers, and anyone interested in language design. The overarching goal is to make Grapa (and GrapaPy) as inviting and usable as possible for the broadest audience.
*/
References:
- [$RULE object](../lib/grapa/$RULE.grc)

This datatype is basis of the grapa language. The syntax of the language is implemented as a set of global rule variables that are accessible and changeable - making the grapa language syntax dynamically mutable, either globally, or modified within a specific function by creating local variable rules that override the global rules. Rules variables can also be defined to support parsing of a domain specific language, or defining a data ETL task as a language by defining the rules for the data and applying the data to the rules - in the same way a language would be defined.

There are three basic steps:
* Define the rules (rules may reference other rules) with code to execute for each rule option.
* Apply raw data to the rule to generate an execution plan.
* Execute the plan, with parameters if specified.

Example system rule:
```
> @$start
rule <$command_list> $SYSID("BE") | <$command_list> ';' | <$command_list>
```

The above is the entry point to the grapa language. This entry point is a global variable that can be updated...so take caution as it is the default entry point all scripts. It is also possible to navigate the language syntax by looking up each rule that is lined from any other rule.

Example of defining a custom rule, and applying the rule:
```
> x = rule $INT $INT {op(a:$1,b:$2){a*b}} | $INT {op(a:$1){a}}
> (op()("4",x))()
4

> (op()("4 3",x))()
12

> op()("4",x)
@<[op,@<[op,@<var,{a}>],{"a":4}>],{}>

> op()("4 3",x)
@<[op,@<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{"a":4,"b":3}>],{}>
```

To simplify creating rules that depend on matching on a predefined list of values, create a list and reference the list in the rules. If the rule matches, the value of the matched item is passed in as the token value.

```
> t = {x:"matched on x",y:5};
> r = rule $INT @t $INT {op(a:$2){a}};
> (op()("1 x 2",r))();
matched on x

> (op()("1 y 2",r))();
5
```

If the lookup needs to be against something other than a list (maybe checking a database or even a web service), an operation can be added to a rule token that will run at compile time if the rule matches. Note that this will be called during the planning phase (or compile time) every time the rule is evaluated - so take caution on attaching a heavy workload. The result of the operation will be used as the result of the token matched. 

```
> x = rule $STR {@<lit,{$1}>};
> r = rule $INT <x,op(a:$1){a.len()}> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
1
```

Rules can also be embedded. Unfortunately, the current grammar requires an operation for a rule if the rule is to return any value - it doesn't make any assumptions. So as of the current release an embedded rule of (x|y) would match on x or y, but with no operation attached to x or y the result would not produce an output for the match. In some future version, this will be addressed to return a default. For now, an operation needs to be inserted if you want a value to be returned.

If the token handler returns an $ERR object, it will cause the rule to fail. So it not only has the ability to add additional logic to processing the token, is also can also serve as an additional component to the planning engine by validating the token against the intent of the rule - something that could not be done at a later phase and could only be done during the planning/compile phase. 

```
> r = rule $INT (x{@<lit,{"found x"}>}|y{@<lit,{"found y"}>}) $INT {op(a:$2){a}};
> (op()("44 y 22",r))();
found y
```

If there is a well known end sequence, and you want to collect all the tokens in between, you can insert an empty rule. All of the tokens, including any white space, will be passed on. The risk of using this in a rule is that if the end token does not exist, all tokens will be processed in the empty rule. 

```
> r = rule $INT <> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
[" ","x"," "]
```

The stop rule for a catchall can be either a token, a rule reference, or an embedded rule.

```
> e = rule $INT {@<lit,{$1}>} | $ID {@<lit,{$1}>};
> r = rule $INT <> <e> {op(a:$3){a}};
> (op()("44 'x' 22",r))();
22

> (op()("44 'x' hi",r))();
hi

> r = rule $INT <> ($INT{@<lit,{$1}>}|$ID{@<lit,{$1}>}) {op(a:$3){a}};
> (op()("44 'x' 22",r))();
22

> (op()("44 'x' hi",r))();
hi
```

A operation can also be associated with the empty rule above, which can process the tokens and either pass through the list, or pass through some transformed version. The following is an example f transforming.

```
> r = rule $INT <op(a:$1){a.len().str()+" raw characters:"+a.join("")}> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
3 raw characters: x
```

If the token handler returns an $ERR object, the corresponding rule option will fail and the next rule option will be evaluated. The following is an example of causing the first rule option to fail.

```
> r = rule $INT <op(a:$1){$ERR()}> $INT {op(a:$2){a}} | $INT <> $INT {op(a:$2){a}};
> (op()("44 'x' 22",r))();
[" ","x"," "]
```

The catchall token can also be used to add support for comments in a grammar. The following is used in the grapa syntax. This rule is then inserted in places in the grammar where comments can be included. 

```
$starcomment = rule ('/' '*') <> ('*' '/');
```

Rules can include both right recursion and left recursion.  The following is an example of right recursion. Note that an internal function "prepend" is used to build the list. This is because grapa uses recursive decent, which results in the last match being processed first.

```
> r1 = rule $ID <r1> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};
> (op()("a b c",r1))();
[a,b,c]
```

But right recursion may not be desirable in some cases. One example is order of operations for additon and subtraction, where the evaluation needs to happen from left to right. Here is an example using right recursion. 

```
r1 = rule 
    $INT '+' <r1>             {@<add,{$1,$3}>} 
  | $INT '-' <r1>              {@<sub,{$1,$3}>} 
  | $INT                            {@<lit,{$1}>}
  ;

> op()("5-3+2",r1);
@<[op,@<sub,{5,@<add,{3,2}>}>],{}>

> (op()("5-3+2",r1))();
0
```

Note that for "5-3+2", the additon of 3+2 happens first resulting in 5-(3+2), which produces the wrong answer. 

Left recursion is required to address this.

```
r2 = rule 
    <r2> '+' $INT       {@<add,{$1,$3}>} 
  | <r2> '-' $INT        {@<sub,{$1,$3}>} 
  | $INT                      {@<lit,{$1}>}
  ;

> op()("5-3+2",r2);
@<[op,@<add,{@<sub,{5,3}>,2}>],{}>

> (op()("5-3+2",r2))();
4
```

Note that with left recursion, the subtraction happens first resulting in (5-3)+2, which produces the correct answer.

For those interested in how to modify a recursive decent parser to support left recursion, see the following on how this was addressed for Python:

https://medium.com/@gvanrossum_83706/left-recursive-peg-grammars-65dab3c580e1

A simular approach is used for Grapa - but with a few improvements on the appropach to support more complex scenarios than what's required to support the Python syntax alone.

```
