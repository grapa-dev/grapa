### Constants
#### true
true

#### false
false

#### null
null

### Stack
#### $$
There is a stack of a single value, the value of the result of the prior operation. This value can be accessed by accessing the $$ variable.

```
> 5*2
10

> @$$ * 3
30
```

### Assignment Operators
#### `=`
Assign.

#### `+=`
Add item to source.

#### `++=`
Concatenate contents to source.

#### `-=`
Subtract item from source.

### Math Operators
#### `+`
Add.

#### `-`
Subtract.

#### `**`
Power.

#### `*/`
Root.

```
> (9*9) */ 2
9

> (9*9) */ 2.0
9

> (9*9) */ 2.1
8.105921431777598477594162942004

> (9*9) */ 4.0
3

> (9*9) ** 0.5
9

> (9*9) ** 0.25
3
```


#### `*`
Multiply.

#### `/`
Divide.

#### `%`
Mod.

### Bit Operators
#### `<<` number
Bit shift left. An effet of multiplying by 2 for every bit shifted.

#### `>>` number
Bit shift left. An effect of dividing by 2 for every bit shifted.

#### `|`
Bitwise or.

#### `&`
Bitwise and.

#### `^`
Bitwise xor.

### Compare Operators
#### `||`
If either of two conditions are true.

```
((1==1) && (2==2) -> true
((1==1) && (2==3) -> true
((1==2) && (2==3) -> false
```

#### `&&`
Two conditions must be true.

```
((1==1) && (2==2) -> true
((1==1) && (2==3) -> false
```

#### `==`
Equal to.

#### '<=>'
Returns -1 if a<b, 0 if a=b, and 1 if a>b.

```
> "a"<=>"b"
-1
```

#### `!=`
Not equal to.

####  `<=`
Less than or equal to.

####  `>=`
Greater than or equal to.

#### `>`
Greater than.

#### `<`
Less than

### Modifier Operators
#### `-`
Negate.

-(4+2) -> -6

#### `!`
Not.

!true -> false

#### `~`
Bitwise not.

(~0x6C).hex() -> FFFFFF93

### Condition Operators
#### `?`

Syntax options:
* bool ? statement for true;
* bool ? statement for true : statement for false;
* bool ? : statement for false;
* (<0)|(0)|(>0) ? statement for -1 : statement for 0 : statement for 1;

Example:
```
> 1?hi:by
hi

> 0?hi:by
by

> -55?hi:by:there
hi

> ("a"<=>"b")?a:e:b
a
```

### Function Operators
#### op
Creating an $OP. See $OP.

#### class
Creates a class that can be used to generate an instance of the class. The class definition is shared between all instances using the class. If information in the class is altered, a copy is made and the modified variable is added to the instance. The instance stores the class reference and any variables local to the instance. Classes can inherit 1 or more other classes. 

```
myC = class {myV = 0; myF = op(a){myV=@a};};
myC2 = class (myC) {myV2 = 0; myF2 = op(a){myV2=@a};};
myIns = obj @myC2;
@myIns;
{}

@myIns.myF(4);
@myIns.myV;
4

@myIns.myV2;
0

@myIns;
{"myV":4}
```

#### obj
Creating an instance of a class. See class.

#### token
Used to define lexical rules. Currently only used to define special character handling like space and tab, where the default handling is to skip these characters so they do not need to be specified in the grammar. 

#### rule
See $RULE

#### const
Compile time option. Used primarily for $RULE. See $RULE.

#### literal
Any sequence starting with an ascii letter, and including numbers and '_'. 

#### literal ([params])
Assumes literal is a variable reference for $OP or $CODE, and runs the routine passing in parameters specified. See $OP and $CODE.

### Command Operators
#### $INCLUDE
Loads referenced file during compile time. Also useful for testing a library or class file by using $INCLUDE to reload the file.

```
$INCLUDE "lib/grapa/grapa.grc";
```

#### exit
Causes the command line shell / console to exit. Primarily used for a script that is initiated from the command line.

If used in the console, the exit will not happen until another command is issued from the shell. This is because everything is handled async.

To exit while in the console, enter a '.' character.

### Lexical Operators
There are several predefined lexical operators, most of which define how $ID, $INT, $FLOAT, $STR, etc, are processed and generate the corresponding tokens. There are also a few other lexical operators that will trigger special handling of the input stream. The following are two examples. Currently there is no way to define/change the lexical operators - this will come in some future version of grapa.

#### `$&`
  * Wrap the XML data in $& on either side to have grapa parse the input as XML. These characters are special lexical triggers that modify the parsing engine token generation. The first instance turns the mode on and the second turns the mode off. Or use the encode("XML") function, which does the same thing.
#### `$[`
  * Wrap input in these characters to have the entire code block parsed in a single instance. Otherwise a '\n' or '\r' will be used to trigger parsing/execution and an error would result if the line is not valid on it's own. Alternatively, put the code in a file, load the file contents, and execute the contents of the string.

### Conditions

#### if

Syntax options:
* if (bool) statement;
* if (bool) statement; else statement;
* if (bool) statement; elseif statement; else statement;

Example:
```
> if (1==0) "1==0\n".echo();  else "none\n".echo();
none

> if (1==0) "1==0\n".echo(); elseif (2==2) "2==2\n".echo(); else "none\n".echo();
2==2
```

#### switch

Syntax options:
* swtich (item) {case item1: statement; item2 statement; etc...;};
* swtich (item) {case item1: statement; item2 statement; etc...; default: statement;};

The following is a way to use a switch in place of if/ifelse/else sequence. The switch will compare until if arrives at a true.

```
> switch(true){case (1==0):"1==0\n".echo(); case (2==2): "2==2\n".echo(); default: "none\n".echo();};
2==2
```


### Loops

#### while
Sytax:
* while(bool) statement;

Example:
```
> i = 0; while (@i<5) {i+=1; (@i.str()+":").echo();}; "\n".echo();
1:2:3:4:5:
```


#### map
Sytax:
* list|array . map ( op );
* list|array . map ( op , param1, param2, etc);

Where op is:
* op (listvalue, param1, param2, etc) ( script )
* op (listvalue, param1, param2, et) { statement;}

Note: each item of the array will process in a separate thread. This is an easy way to add multi-threading to the processing, but beware and batch workoads in the array to keep the size of the array to the number of threads you want.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...esure thread safe by using lock/unlock (any variable can be used for lock/unlock).

Example:
```
> [1,2,3].map(op(n){@n*2});
[2,4,6]

> [1,2,3].map(op(n,p){@n*2+@p},5);
[7,9,11]
```

#### reduce
Sytax:
* list|array . filter ( op );
* list|array . reduce ( op , startvalue);
* list|array . reduce ( op , startvalue, param1, param2, etc);

Where op is:
* op (startvalue, listvalue, param1, param2, etc) ( script )

Reduce process sequentially, as processing is intended to augment the startvalue. If the startvalue is not provided, the first item in the list is used as the startvalue.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...esure thread safe by using lock/unlock (any variable can be used for lock/unlock).

Example:
```
> [1,2,3].reduce(op(s,n){@s*@n});
6

> [1,2,3].reduce(op(s,n){s+=@n*2},[]);
[2,4,6]

> [1,2,3].reduce(op(s,n,p){s+=@n*2+@p},[],5);
[7,9,11]
```

#### filter
Sytax:
* list|array . filter ( op );
* list|array . filter ( op , param1, param2, etc);

Where op is:
* op (listvalue, param1, param2, etc) ( script )
* op (listvalue, param1, param2, et) { statement;}

Note: each item of the array will process in a separate thread. This is an easy way to add multi-threading to the processing, but beware and batch workoads in the array to keep the size of the array to the number of threads you want.

Example:
```
> [1,2,3,4,5,6,7,8,9].filter(op(n){@n%2!=0});
[1,3,5,7,9]

> [1,2,3,4,5,6,7,8,9].filter(op(n,p){@n%@p!=0},3);
[1,2,4,5,7,8]
```
