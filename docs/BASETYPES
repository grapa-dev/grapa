# $OP
This is the core type at the base of grapa. It does not have a class associated. Consider it as both a high level version of byte code (but represented as a list) and simular to a lambda in other languages. It's a variable that includes executable instructions. 

Understanding the inner workings of $OP is not necessary to use grapa - and this section can be skipped. This though is core to how grapa works, and an understanding is needed in order to extend the syntax of the language to support your own domain specific language extensions.

Examples:
Commands | Results | Description
------------ | ------------- | -------------
(op(){4*2})(); | 8 | Creates $OP and executes.
(@<mul,{4,2}>)(); | 8 | Same but using $OP directly.

Adding parameters to $OP.

Example: The op command is used for passing parameters into a function. When the op command is used, the name is replaced with an array where the first value is "op" and the second value is the function. This array is than followed by the parameter list. The parameters are used to initialize the namespace for the function.

Commands | Results
------------ | -------------
op(){1} | @<[op,1],{}>
op(a,b){@a*@b}; | @<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{a,b}>
op(){5%2}; | @<[op,@<mod,{5,2}>],{}>
op(){4*2}; | @<[op,8],{}>

In the last example above, the compiler implemented an optimization where it recognized that multiplying two constants could be completed at compile time.

See the section on syntax for additional examples.

# $CODE
A sequence of $OP items to be processed in sequence.  Normally you would use $CODE directly. It is typically embedded in an $OP type. The planner typically wraps the result in an $OP along with parameter handling for the function - but this could change at some point where it could be either $OP or $CODE. 

Example: Assigns 2 to a, than evaluates a*3, the result is 6
Commands | Results
------------ | -------------
f=@[1,2];</br>f(); | 2
f=op(){a=2;@a*3;};</br>@f; | @<[op,@[@<assign,{a,2}>,@<mul,{@<var,{a}>,3}>]],{}>
f(); | 6


