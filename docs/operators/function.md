# Function Operators
## op
Creating an $OP. See [$OP type](../type/OP.md).

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
See [$RULE type](../type/RULE.md)

## const
Compile time option. Used primarily for $RULE. See $RULE.

## static
Sets a bit in the variable that prevents it from being changed. Will not be locked when accessed, which is useful for a global variable accssed by concurent threads.

```
x = static {one: "string one", two: "string two"};
```


## literal
Any sequence starting with an ascii letter, and including numbers and '_'. 

## literal ([params])
Assumes literal is a variable reference for $OP or $CODE, and runs the routine passing in parameters specified. See $OP and $CODE.
