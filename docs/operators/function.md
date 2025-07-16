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
Sets a bit in the variable that prevents it from being changed. Will not be locked when accessed, which is useful for a global variable accssed by concurent threads.

```
x = const {one: "string one", two: "string two"};
```

Once set, the variable can not be modified. To turn off the const bit, use the following:
```
x.setconst(false);
x += (three:"th str");
x.setconst(true);
```

**Note:** `const` is intended for performance (caching/optimization), not for locking or thread safety. It prevents modification unless `setconst` is used, but does not synchronize access between threads. For explicit locking, see [$thread](../sys/thread.md).

## static
Compile-time option. The code to construct the array (or value) is executed at compile time (during script loading), not at runtime. This reduces repeated computation and improves performance, but does not affect thread safety or immutability.

```
f = op(s)
{
  x = static {one: "string one", two: "string two"};
  x[s];
};
f("two");
```

Here is the compiled function with static:
```
@<[op,@[@<assign,{x,{"one":"string one","two":"string two"}}>,@<search,{@<var,{x}>,@<createlist,{@<var,{s}>}>}>]],{s}>
```

Here is the compiled funciton without static:
```
@<[op,@[@<assign,{x,@<prepend,{@<createlist,{@<name,{two,"string two"}>}>,@<name,{one,"string one"}>}>}>,@<search,{@<var,{x}>,@<createlist,{@<var,{s}>}>}>]],{s}>
```

If the list needs to be constructed using a variable that is passed in, than do not use the static operator and the variable will be constructed at run time.

**Note:** `static` is for compile-time evaluation only. It does not make a value immutable or thread safe. For thread safety and locking, see [$thread](../sys/thread.md).

## literal
Any sequence starting with an ascii letter, and including numbers and '_'. 

## literal ([params])
Assumes literal is a variable reference for $OP or $CODE, and runs the routine passing in parameters specified. See $OP and $CODE.

```
