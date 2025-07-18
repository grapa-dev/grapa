# $CODE

/*
Design Influences:
$CODE, as a sequence of $OP items, draws inspiration from Lisp's code-as-data paradigm, supporting composable and introspectable execution trees. This approach is chosen to make Grapa accessible and powerful for a broad audience, with a special focus on usability for Python programmers and educators.
*/
A sequence of $OP items to be processed in sequence.  Normally you would use $CODE directly. It is typically embedded in an $OP type. The planner typically wraps the result in an $OP along with parameter handling for the function - but this could change at some point where it could be either $OP or $CODE. 

Example: Assigns 2 to a, than evaluates a*3, the result is 6
Commands | Results
------------ | -------------
f=@[1,2];</br>f(); | 2
f=op(){a=2;a*3;};</br>f; | @<[op,@[@<assign,{a,2}>,@<mul,{@<var,{a}>,3}>]],{}>
f(); | 6