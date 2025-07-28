# Function Operators
## op
Creating an $OP. See [$OP type](../type/op.md).

## class
Creates a class that can be used to generate an instance of the class. The class definition is shared between all instances using the class. If information in the class is altered, a copy is made and the modified variable is added to the instance. The instance stores the class reference and any variables local to the instance. Classes can inherit 1 or more other classes. 

### Basic Class Definition
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

### Advanced Class Method Definition
For built-in classes and advanced use cases, methods can be defined using the execution tree syntax:

```grapa
@global["$myclass"]
= class ($OBJ) {
    /* Basic method with direct parameters */
    simple = @<"simple_method",{this}>;
    
    /* Method with parameters */
    set = @<[op,@<"my_set",{this,@<var,{name}>,@<var,{value}>}>],{name,value}>;
    
    /* Method with default values */
    get = @<[op,@<"my_get",{this,@<var,{name}>,@<var,{field}>}>],{name,field:"$VALUE"}>;
    
    /* Method with multiple defaults */
    list = @<[op,@<"my_list",{this,@<var,{path}>,@<var,{fields}>,@<var,{threads}>}>],{path:null,fields:null,threads:null}>;
};
```

#### Method Definition Syntax
- **`this`**: Reference to the current object (not `@<this>`)
- **`@<var,{param}>`**: Parameter reference
- **`{param1,param2}`**: Parameter list without defaults
- **`{param1:default,param2:null}`**: Parameter list with defaults

#### Key Patterns
- Use `this` directly, not `@<this>`
- Quote method names: `"method_name"`
- Parameters: `{this,@<var,{param}>}`
- Defaults: `{param:default_value}`

#### Examples from Built-in Classes
```grapa
/* From $file class */
table = @<[op,@<"file_table",{this,@<var,{p}>}>],{p}>;
set = @<[op,@<"file_set",{this,@<var,{name}>,@<var,{p}>,@<var,{d}>}>],{name,p,d}>;

/* From $OBJ class */
str = @<[op,@<str,{this,@<var,{format}>}>],{format:null}>;
len = @<len,{this}>;
```

### Enhanced Debug Functionality
The `$unified` class provides enhanced debug capabilities for database inspection:

```grapa
/* Basic debug output */
u = $unified();
u.set("user1", "Alice");
u.set("user2", "Bob");
u.debug();

/* Output shows complete database structure */
=== GrapaUnifiedLocalDatabase Debug Information ===
Storage Type: GRAPADBX
Storage URL: 
Storage Path: default.dbx
GrapaDBX Information:
  First Tree: 3
  Root Type: 0
  GrapaDBX Available: Yes

=== FULL DATABASE STRUCTURE ===
DumpTree Result:
LIST start (3) key=0 type=GROUP parent=0 size=0 weight=1
| | SU64 key=0 value=0
LIST ended (3) key=0 type=GROUP weight=1

DumpGroup Result:
LIST start (3) key=0 type=GROUP parent=0 size=0 weight=1
| | SU64 key=0 value=0
LIST ended (3) key=0 type=GROUP weight=1
```

#### Debug Output Features
- **High-level Information**: Storage type, URL, path, and availability
- **Full Database Structure**: Complete B-tree structure with hierarchy
- **Tree Visualization**: Shows LIST, TREE, SU64 elements with relationships
- **Weight Information**: Tree complexity and size metrics
- **Structure Comparison**: Output format matches GrapaDB examples

## obj
Creating an instance of a class. See class.

## token
Used to define lexical rules. Currently only used to define special character handling like space and tab, where the default handling is to skip these characters so they do not need to be specified in the grammar. 

## rule
See [$RULE type](../type/rule.md)

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

---

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
