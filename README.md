# grapa
grapa - a grammar parser language / environment for processing data for ETL workflows, or experimenting with new language constructs. 

## Introduction
The unique approach of grapa is that it includes native syntax for JSON/XML and BNF style grammers for defining data processing rules. The same grammer engine used to assist with parsing data is the same engine used to process the language. The syntax of the language is fully customizable (even at runtime if that's of interest) as the language rules are stored as global variables. If for example, you'd like to extend the langue to include some prefered syntax , that enhancement would function as native syntax. 

Underneith the syntax, there are several libraries that provide the lower level heavy lifting for strings, execution flow, files, networking, threads, classes with inheritance, ecryiption (RSA, AES, SHA, etc), math, database, etc. The grammer syntax you define can either invoke routines written in the existing syntax, or call lower level libraries. The native integer and float support uses unlimited percision (or limited by CPU/memory). 

Execution of all scripts (from code or compiled) run the same accross all platforms, including the unlimted floating point routines. And will work the same on either big endian or little endian systems. 

## Setup
### Installation
Suported platforms include Win10, MacOS, Linux.

To instally copy the zip for the target platform, unzip, and copy to the exectutable to an apprpriate directory. If needed, update the system PATH. The binaries are located in the "bin" directory in this GitHub repository.

There will be an installer for each of the platforms...but for now, drag and drop of the binary works.
Linux: Place in "/usr/bin" (or somewhere accessable). 
Windows: Place in a location that is in the PATH, or update PATH to the location the application is copied to.
Mac: Place in "/usr/local/bin" - first time running the tool will take about 20 seconds as the OS scans the executable. 

### Command line options

Once copyed into a location (and PATH updated if needed), you'll be able to run the tool using "grapa" from a terminal or command sheel from any location. Running the tool with "grapa --env" will display a few of the directories where the tool will look for information.

Other command line options:
```
        -h,--help       :Show this help message
        -v,--version    :Show version
        -q,--quite      :Suppress header
        -env            :Show environment details
        -ccmd script    :Run from script
        -cfile file     :Run from file
        -ccin           :Run from stdin
        -argcin         :Places std:cin into $ARGCIN environment variable (use with -ccmd|-cfile)
        -argv           :Places proceeding args into $ARGV environment variable
```

Example: Performas a SHAKE256 hash of the string "thisisatest".
```
grapa -ccmd "'thisisatest'.encode(SHAKE256)" -q
94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

Example: Same, but takes input from cin. 
```
echo "thisisatest".encode(SHAKE256) | grapa -ccin -q
94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

# Data Types

## Base Type

### $OP
This is the core type at the base of grapa. It does not have a class associated. Consider it as both a high level version of byte code (but represented as a list) and simular to a lambda in other languages. It's a variable that includes executable instructions. 

Understanding the inner workings of $OP is not neccessary to use grapa - and this section can be skipped. This though is core to how grapa works, and an understanding is needed in order to extend the syntax of the language to support your own domain specific language extentions.

Examples:
Commands | Results | Description
------------ | ------------- | -------------
f=()[mul,{2,3}];</br>f(); | 6 | Calls the "mul" library passing 2 and 3, the result is 6.
op(){4%2}; | ()[[op,()[mod,{4,2}]],{}] | Operation created to call mod function with 4 and 2
op(){4*2}; | ()[[op,8],{}] | Operation created to multiply 4 and 2, and optimizer reduced during planning
f=op(){4*2};</br>f(); | 8 | Assign operation to a variable, and call the variable to get the result
(op(){4*2})(); | 8 | Create and run operation in a single command

See the section on syntax for additional examples.

### $CODE
A sequence of $OP items to be processed in sequence. 

Examples:
Commands | Results | Description
------------ | ------------- | -------------
f=()<1,2>;</br>f(); | 2 | Processes 1 and then 2, and 2 is the final result.
f=()<()[assign,{a,2}],()[mul,{()[var,{a}],3}]>;</br>f(); | 6 | Assignes 2 to a, than evaluates a*3, the result is 6;

Normally you would use $CODE directly. It is typically embedded in an $OP type. The planner typically wraps the result in an $OP along with parameter handling for the function - but this could change at some point where it could be either $OP or $CODE. 

## System Class Types

### $BOOL
The following are equivalent in comparisons:
- True: true, 1
- False: false, null

Examples:
Commands | Results
------------ | -------------
1==true; | 1
null==false; | 1
1==false; | </code>
"55"==(55).str(); | 1
"5.54"==(5.54).str(); | 1
"5.53"==(5.54).str(); | 

### $STR
ASCII only - unicode will eventually be added. Can initalize with either double quotes or single quotes - which is easier than escaping a string that includes a quoted string - such as "this 'is' a test", or 'this "is" a test'. $STR inherits the $obj class - see $obj for functions supported. 

Examples:
Commands | Results
------------ | -------------
"this is a test".len(); | 14
"this is a test".split(""); | ["t","h","i","s"," ","i","s"," ","a"," ","t","e","s","t"]
"this is a test".raw(); | 7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996

### $INT
Supports signed and unsigned. Unsigned is essentally an $INT that is not negative. Given this is unlimited precision, a separate $UINT type is not required (grapa originally had one, but it's been removed as it's essentially redundant and adds unncessary complexity). The reason other languages have a $UINT type is to handle to bit overflow issue where the high order bit may become set during math operations. In grapa, when the high order bit is set, it automatically increases the precision and there is no overflow. Still, there is sometimes a need for specific $UINT type handling of raw bytes, and to support this there is a uint() function to ensure that a raw byte stream with a high order bit is not incorectly interpreted as a negative number. 

The $INT class inherits $obj class - see the the $obj class for additional functions supported.

Examples:
Commands | Results
------------ | -------------
52322; | 52322
"52322".int(); | 52322
(52322).raw(); | 00CC62

#### genprime
#### isprime
#### random
#### genrsa
#### e
#### pi
#### log
#### ln
#### sin
#### cos
#### tan
#### cot
#### sec
#### csc
#### asin
#### acos
#### atan
#### acot
#### asec
#### acsc
#### sinh
#### cosh
#### tanh
#### coth
#### sech
#### csch
#### asinh
#### acosh
#### atanh
#### acoth
#### acech
#### acsch
#### atan2
#### hypot.

### $FLOAT

Supports both fix and float format. Fix will apply the precision to just the decimal. Float will not restrict to the decimal, supporting large exponents with a specified precision. Each number maintains it's own precision, which is used in math operations with numbers of other precision. Some math operations may convert an input from one format to another. For example, passing in a float to a trig function will produce a fix format result. Floats also support specifying "extra" bits to apply to the calcuations to reduce error propagation (the default is 7 bits). 

The $FLOAT class inherits $INT class - see the the $INT class for additional functions supported.

Example:
<pre><code>> 5.13
5.13
</code></pre>

You can also create using hex format:
<pre><code>> -0x4.0x5
-4.3125
</code></pre>

Or binaary format:
<pre><code>> 0b101.11
5.625
</code></pre>

By default, all floats are "float" format. To change formats use the fix() and float() routines, where both the precision and the "extra" bits can also be specified. The parsing engine will set the precision to the system default, which is 128 bits. There are two ways to change this. First, change the system defualt using the setfloat and setfix routines. Second, pass in a $STR to the fix or float routines. For example:

<pre><code>> "3.5".float(300,6)
3.5
</code></pre>

To verify the float is being created properly, use the decode routine with FLOAT as the parameter. For example:

<pre><code>> "30.75".float(300,6).decode(FLOAT)
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"extra":6,"data":123}
> (30.75).decode(FLOAT).data
123

> (30.75).decode(FLOAT).data.hex()
7B
</code></pre>

Convert to other formats:
<pre><code>> (30.75).hex()
1E.C

> (30.75).bin()
11110.0011

> (30.75).int()
30

> (30.75).raw()
00048100077B

</code></pre>

Bit shifts:
<pre><code>> (30.75) >> 4
1.921875

> (30.75) << 4
492
</code></pre>

### $ID
Any identifier (starts with a letter and can follow with numbers and '-' but can not end with '-') will be initialized as an $ID. And an $ID can be used for many things, including associating a value. The '@' symbol is used to dereference an $ID to retrieve the data stored.

Example of using an $ID as a variable:
<pre><code>> x = "hi"

> @x
hi

> @x + " test"
hi test
</code></pre>

### $LIST
A $LIST is basically JSON, but extended to include all the various grapa data types. 

Examples:
* Create
  * x = {a:1, b:2}
* Access
  * {a:1, b:2}.a
  * {a:1, b:3}[1]
* Append
  * x += (c:3)
* Insert at
  * @x += (c:3) @x[0]
* Number of entries
  * @x[]
  * {a:1, b:2}[]
* Remove
  * @x -= @x[0]


### $ARRAY
Same as $LIST, but without the entry labels.

### $TIME
Uses UTC timezone.

Examples:
<pre><code>> $TIME().utc()
2020-04-23T19:32:41.192673

> $TIME().tz()
-25200000000

> $TIME().utc() + $TIME().tz()
2020-04-23T12:33:33.921638

> t1 = $TIME().utc()

> @t1.delta()
12.231424
</code></pre>

### $TABLE
A $TABLE is a higharchical database with columns, rows, with both row store and columns store. Uses the $file commands for creating, updated, and navigating.

### $RAW
A $RAW represents raw bytes. Most data types can be converted to and from $RAW, providing the abilty to make speicic tweaks to data. For example, this is how time addition/subtraction is performed - by converting the $TIME into $RAW and then into an $INT, and than back to a $TIME. There are several examples of using raw in the documentation for the other data types.

### $XML
An $XML represents the XML data type. Under the hood, it is represented as a $LIST and all the same command sused for $LIST should work on $XML. 

### $RULE
This datatype is basis of the grapa language. The syntax of the language is implemented as a set of global rule variables that are accessable and changeable - making the grapa language syntax dynamically mutable, either globally, or modified within a specific function by creating local variable rules that override the global rules. Rules variables can also be defined to support parsing of a domain specific language, or defining a data ETL task as a lanugae by defining the rules for the data and applying the data to the rules - in the same way a language would be defined.

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

Example of defining a custome rule, and applying the rule:
```
> x = rule $INT $INT {op(a:$1,b:$2){@a*@b}} | $INT {op(a:$1){@a}}
> (op()("4",@x))()
4

> (op()("4 3",@x))()
12

> op()("4",@x)
()[[op,()[[op,()[var,{a}]],{"a":4}]],{}]

> op()("4 3",@x)
()[[op,()[[op,()[mul,{()[var,{a}],()[var,{b}]}]],{"a":4,"b":3}]],{}]
```

To simplify creating rules that depend on matching on a predefined list of values, create a list and reference the list in the rules. If the rule matches, the value of the matched item is passed in as the token value.

```
> t = {x:"matched on x",y:5};
> r = rule $INT @t $INT {op(a:$2){@a}};

> (op()("1 x 2",@r))();
matched on x

> (op()("1 y 2",@r))();
5
```

If the lookup needs to be against something other than a list (maybe checking a database or even a web service), an operation can be added to a rule token that will run at compile time if the rule matches. Note that this will be called during the planning phase (or compile time) every time the rule is evaluated - so take caution on attaching a heavy workload. The result of the operation will be used as the result of the token matched. 

```
> x = rule $STR {()[lit,{$1}]};
> r = rule $INT <x{op(a:$1){@a.len()}}> $INT {op(a:$2){@a}};
> (op()("44 'x' 22",@r))();
1
```

Rules can also be embedded. Unfortunately, the current grammer requires an operation for a rule if the rule is to return any value - it doesn't make any assumptions. So as of the current release an embedded rule of (x|y) would match on x or y, but with no operation attached to x or y the result would not produce an output for the match. In some future version, this will be addressed to return a default. For now, an operation needs to be inserted if you want a value to be returned.

If the token handler returns an $ERR object, it will cause the rule to fail. So it not only has the ability to add additional logic to processing the token, is also can also serve as an additional component to the planning engine by validating the token against the intent of the rule - something that could not be done at a later phase and could only be done during the planning/compile phase. 

```
> r = rule $INT (x{()[lit,{"found x"}]}|y{()[lit,{"found y"}]}) $INT {op(a:$2){@a}};
> (op()("44 y 22",@r))();
found y
```

If there is a well known end sequence, and you want to collect all the tokens in between, you can insert an empty rule. All of the tokens, including any white space, will be passed on. The risk of using this in a rule is that if the end token does not exist, all tokens will be processed in the empty rule. 

```
> r = rule $INT <> $INT {op(a:$2){@a}};
> (op()("44 'x' 22",@r))();
[" ","x"," "]
```


### $ERR
If an operation results in an error, the $ERR data type is returned. Check using the type function: if (@result.type()==$ERR) something;

Under the hood, the $ERR type is a $LIST, and the same commands can be used to get more details on the error.

## System Class Types

### $SYSID
Same as $ID, but was initialized with '$' at the front of the identifier. Used for special system identifiers. 

### $SYSINT
Same as $INT, but was initialized with '$' at the front of the integer. Used for special system integers. 

### $SYSSTR
Same as $STR, but was initialized with '$' at the front of the string. Used for special system strings. 

## Defined Class Types

### $sys
A few general utility functions that are useful, but it wasn't clear if they should be added to the native language syntax, were added to $sys.

#### type (object)
<pre><code>> $sys().type(5)
$INT
</code></pre>
#### getenv (type)

getenv/putenv types:
Type | Description
------------ | -------------
$PATH |
$STATICLIB |
$ARGCIN |
$ARGV |
$LIB |
$BIN |
$NAME |
$WORK |
$HOME |
$TEMP |
$VERSION |
$LICENCE |
Any value not starting with '$' will be directed to the native OS getenv/putenv |

<pre><code>> $sys().getenv($VERSION)
{"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000}
</code></pre>
#### putenv (type, value)
See getenv.

### $obj
Several classes inherit $obj, such as $STR and $INT and $LIST. Functions that can be used accross the different data types are placed in the $obj class. Some of these functions may move to other classes. The $obj class is a general place to place functions as a starting point. For example, the setfloat and setfix functions may move to the $sys class. 

#### type
#### raw
#### uraw
#### bool
#### int
#### uint
#### float
#### fix
#### setfloat
#### setfix
#### str
#### base
#### hex
#### uhex
#### bin
#### ubin
#### time
#### bits
#### bytes
#### len
#### left
#### right
#### mid
#### rtrim
#### ltrim
#### trim
#### reverse
#### replace
#### split
#### join
#### upper
#### lower
#### setbit
#### clearbit
#### genbits
#### encode (type, value [,options])
encode/decode types:
Type | Description
------------ | -------------
RSA-KEY | 
AES256 |
ZIP-GRAPA |
BASE64 |
ESCAPE |
FLOAT |
XML-GRAPA | Only encode.
SHAKE256 | Only encode.
SHAKE128 | Only encode.
SHA3-256 | Only encode.
SHA3-384 | Only encode.
SHA3-512 | Only encode.

#### decode (type, value [,options])
See encode.

#### setfile
#### map
#### filter
#### reduce
#### sort
#### isint
#### iferr
#### exec
#### getname

### $file
Provides the ability to navigate either the file system or a database, querying data and updating data. This class/libraries will be enhanced over time to support navigating data types beyond the file system and the grapa database - such as JSON/XML and unstructured data where a mapping can be defined (maybe with a set of rules). With a few additional enhancements, this class/library will also enable extending the grapa syntaxt to include SQL with $file for the underlying data.

Each example below assumes the following command has been issued:
<pre><code> f = $file();</code></pre>
Which assignes f an instance of the $file class. The following are then operations that can be used from the $file class.

#### type ()
<pre><code>> @f.type()
$file
</code></pre>
#### table ()
The table function creates an in memory database. 

<pre><code>> t = @f.table()
> @t.mkrow("test","data for test")
> @t.getrow("test")
data for test
</code></pre>
#### pwd ()
Returns the current working directory, relative to the current home directory. 
<pre><code>> @f.pwd()
/

> @f.cd("lib")
> @f.pwd()
/lib
</code></pre>
#### cd (relativepath)
Changes the current working directory, relative to the current home directory.  Using ".." will result in moving back 1 level.
<pre><code>> @f.cd("lib")
> @f.pwd()
/lib
</code></pre>
#### phd ()
Returns the current home directory.
<pre><code>> @f.phd()
C:\Projects\Test
</code></pre>
#### chd (filesystempath)
Changes the current home directory.
<pre><code>> @f.chd("lib")
> @f.phd()
C:\Projects\Test\lib
</code></pre>
#### dir ([path])
Retrieves a list of files/directories in the current working directory.
<pre><code>> @f.chd("/Microsoft/AndroidNDK64/android-ndk-r16b/python-packages")
> @f.dir()
[
  {"$KEY":"adb","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"Android.bp","$TYPE":"FILE","$BYTES":784},
  {"$KEY":"fastboot","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"gdbrunner","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"NOTICE","$TYPE":"FILE","$BYTES":577},
  {"$KEY":"repo.prop","$TYPE":"FILE","$BYTES":4465}
]

> @f.chd("/Microsoft/AndroidNDK64/android-ndk-r16b")
> @f.dir("python-packages")
[
  {"$KEY":"adb","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"Android.bp","$TYPE":"FILE","$BYTES":784},
  {"$KEY":"fastboot","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"gdbrunner","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"NOTICE","$TYPE":"FILE","$BYTES":577},
  {"$KEY":"repo.prop","$TYPE":"FILE","$BYTES":4465}
]
</code></pre>

#### mkdir (name [,type]) 
Creates a directory at the current working directory withyin the file system.

type is one of:
- "", if in the file system will default to "DIR"
- "DIR", if in the file system, a new directory will be created. If in the database, will use "GROUP"
- "GROUP", create a database of GROUP type
- "ROW", create a ROW store database
- "COL", create a COL store datebase

<pre><code>> @f.mkdir("test")
> @f.cd("test")
> @f.dir()
[]

> @f.mkdir("testg","GROUP")
> @f.cd("testg")
> @f.dir()
[]
</code></pre>

#### rmdir (name)
Removes a directory.
<pre><code>> @f.rmdir("test")
</code></pre>
#### mkcol (name)
Creates a column within the current working directory.
<pre><code>> @f.mkcol("test")
</code></pre>
#### mkrow (name)
Adds a new within the current working directory with the specified name as the key.
<pre><code>> @f.mkcol("test")
</code></pre>
#### rmrow (name)
Removes a row the current working directory with the specified name. If the name is a number, the row with that index is removed.
<pre><code>> @f.rmrow("test")
</code></pre>
#### setrow (name, value [, column])
Updates the column in a row. By default the $VALUE column is updated. But an alternate column can be specified.

Note: this command will be updated to support setting multiple fields within a single call. And maybe even for multiple rows (not sure...maybe a setrows command).

<pre><code>> @f.setrow("test","value of test")
</code></pre>
#### getrow (name [, column])
Gets the column value in a row. By default the $VALUE column is retrieved. But an alternate column can be specified.

Note: this command will be updated to support getting multiple fields within a single call. And maybe even for multiple rows (not sure...maybe a getrows command).

<pre><code>> @f.getrow("test")
value of test
</code></pre>
#### debug ()
Used for debugging the database during development. Displayes the BTree structure of the data dictionary and fields and indexes for the current working directory when in a database (either in memory or on the file system).

### $net
Provides a socket library, cross functional with all platforms supported. 

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...esure thread safe by using lock/unlock (any variable can be used for lock/unlock).

#### type

#### mac

#### interfaces

#### connect

#### listen

#### onlisten

#### disconnect

#### host

#### send

#### recieve

#### nrecieve

#### onrecieve

### $thread
Provides a thread library, cross functional with all platforms supported.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...esure thread safe by using lock/unlock (any variable can be used for lock/unlock).

#### type

#### trylock

#### lock

#### unlock

#### wait

#### signal

#### waiting

#### start

#### stop

#### started

#### suspend

#### resume

#### suspended

## Custom Class Types
Create custome types using the class routine. The underlying structure is a $LIST where variables are stored, and the class can inherit other classes, including system types/classes (each system type is initiated as a class instance).

The following examle defines a class with a single method. The method setdiv2 updates data to the parameter divided by 2.
<pre><code>> myClass = class {data = 0; setdiv2 = op(p){data = @p/2;}; get = op(){@data;};};

> test = myClass();

> @test.setdiv2(5);

> @test.get();
2

> @test
{"data":2}
</code></pre>

## Operators
### Constants
#### true
#### false
#### null

### Assignment Operators
#### `=`
#### `+=`
#### `++=`
#### `-=`

### Math Operators
#### `+`
#### `-`
#### `**`
#### `*/`
#### `*`
#### `-+`
#### `/`
#### `%/`
#### `%`

### Bit Operators
#### `<<`
#### `>>`
#### `|`
#### `&`
#### `^`

### Compare Operators
#### `||`
#### `&&`
#### `==`
#### '<=>'
#### `!=`
####  `<=`
####  `>=`
#### `>`
#### `<`

### Modifier Operators
#### `-`
#### `!`
#### `~`

### Condition Operators
#### `?`

### Function Operators
#### op
#### class
#### obj
#### token
Used to define lexical rules. Currenlty only used to define special character handling like space and tab, where the default handling is to skip these characters so they do not need to be specived in the grammer. 

#### rule
#### const
#### `()[oplist]`
#### `()<opcode>`
#### literal
#### literal ([params])
#### `($OP)([params])`
Use to execute an $OP type. Use the op function to create the $OP. The op function is how functions are created in grapa.
<pre><code>> if = op(a){@a;};
> @f("test");
test
</code></pre>

### Command Operators
#### $INCLUDE
#### exit
#### console
#### echo
#### string

### Lexical Operators
There are several predefined lexical operators, most of which define how $ID, $INT, $FLOAT, $STR, etc, are processed and generate the corresponding tokens. There are also a few other lexical operators that will trigger special handling of the input stream. The following are two examples. Currently there is no way to define/change the lexical operators - this will come in some future version of grapa.

#### `$&`
  * Wrap the XML data in $& on either side to have grapa parse the input as XML. These characters are specail lexical triggers that modify the parsing engine token generation. The first instence turns the mode on and the second turns the mode off.
#### `$[`
  * Wrap input in these characters to have the entire code block parsed in a single instance. Otherwise a '\n' or '\r' will be used to trigger parsing/execution and an error would result if the line is not valid on it's own. Alternatively, put the code in a file, loand the file contents, and execute the contents of the string.

### Conditions and Loops

#### if

Syntax options:
* if (bool) statement;
* if (bool) statement; else statement;
* if (bool) statement; elseif statement; else statement;

Example:
<pre><code>> if (1==0) echo "1==0";  else echo "none";
none

> if (1==0) echo "1==0"; elseif (2==2) echo "2==2"; else echo "none";
2==2
</code></pre>

#### ?

Syntax options:
* bool ? statement for true;
* bool ? statement for true : statement for false;
* bool ? : statement for false;
* (>0)|(0)|(<0) ? statement for >0 : statement for 0 : statement for <0;

Example:
<pre><code>> 1?hi:by
hi

> 0?hi:by
by

> -55?hi:by:there
there
</code></pre>


#### switch

Syntax options:
* swtich (item) {case item1: statement; item2 statement; etc...;};
* swtich (item) {case item1: statement; item2 statement; etc...; default: statement;};

The following is a way to use a switch in place of if/ifelse/else sequence. The switch will compare until if arrives at a true.

<pre><code>> switch(true){case (1==0):echo "1==0\n"; case (2==2): echo "2==2\n"; default: echo "none\n";};
2==2
</code></pre>

#### while
Sytax:
* while(bool) statement;

Example:
<pre><code>> i = 0; while (@i<5) {@i+=1; echo @i.str()+":";};
1:2:3:4:5:
</code></pre>


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
<pre><code>> [1,2,3].map(op(n){@n*2});
[2,4,6]

> [1,2,3].map(op(n,p){@n*2+@p},5);
[7,9,11]
</code></pre>

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
<pre><code>> [1,2,3].reduce(op(s,n){@s*@n});
6

> [1,2,3].reduce(op(s,n){s+=@n*2},[]);
[2,4,6]

> [1,2,3].reduce(op(s,n,p){s+=@n*2+@p},[],5);
[7,9,11]
</code></pre>

#### filter
Sytax:
* list|array . filter ( op );
* list|array . filter ( op , param1, param2, etc);

Where op is:
* op (listvalue, param1, param2, etc) ( script )
* op (listvalue, param1, param2, et) { statement;}

Note: each item of the array will process in a separate thread. This is an easy way to add multi-threading to the processing, but beware and batch workoads in the array to keep the size of the array to the number of threads you want.

Example:
<pre><code>> [1,2,3,4,5,6,7,8,9].filter(op(n){@n%2!=0});
[1,3,5,7,9]

> [1,2,3,4,5,6,7,8,9].filter(op(n,p){@n%@p!=0},3);
[1,2,4,5,7,8]
</code></pre>

## Language Syntax
### Default Language Rules
To view a text version of the rules loaded into grapa on startup, run the following at the prompt:

<pre><code>> $sys().getenv($STATICLIB)."grapa.grc".str()
</code></pre>

The system will first check for a match on the "start" rule. Which is a global variable...if that global variable is of type $RULE, than it will be come the first rule for scripts. This is an easy way to provide an override on command processing. If "start" rule does not provide a match, than the system will evaluage using the "$start" rule. 

The default rules may be subject to change, and so are not listed here. Use the above command to retrieve/view.  To write the result to a file, use the following:

<pre><code>> $file().setrow("grapa.grc",$sys().getenv($STATICLIB)."grapa.grc".str())
</code></pre>

This will place the file in the current working directory. If you want to place in a different directly, refer to the $file class for commands to do that.

If you modify the file, you can try it out by placing it in "lib/grapa/" under the same directory that the above command would have written the file to, and then restart grapa. If grapa finds this file with this name in that location, it will use that file instead of the default. 

If you are familiar with YACC or BNF, following the grammer logic should be rather streat forward. Each rule is evaluated until a successful match, and all matches prduce an exectuaiton play where the code associated with each rule match is included in the execution plan. To optimize grammer resolution, rules carefully constructed to avoid re-evaluating rules more than neccessary by placing the most complex rules first and following with common rule patters. It is also important to avoid infinate recursive situations - mostly this is avoided by not referencing the same rule as the first token to evaluate. 

There are a few standard tokens that are defined - mostly the tokens provide special handling for either the lexical engine or the rules engine. One example is the space character, which when found a bit is set in the token skip the value for rules evaluation. This can be changed - but then would require including including the space token in the rules. 

### Operator

Commands | Results
------------ | -------------
f=op(){3%2};</br>f(); | 1
f=()[[op,()[mod,{3,2}]],{}];</br>f(); | 1
f=op(a,b){@a%@b};</br>f(); | ()[[op,()[mod,{()[var,{a}],()[var,{b}]}]],{a,b}]
f(842,5); | 2
f=op(a,b,c){d=@a%@b;@d*@c;};</br>f(); | ()[[op,()<()[assign,{d,()[mod,{()[var,{a}],()[var,{b}]}]}],()[mul,{()[var,{d}],()[var,{c}]}]>],{a,b,c}]
f(842,5,9); | 18

You can also define an operation by providing a script and a rule. If a rule is not specified, the "start" and "$start" rules are used (which is the default entry point for the grapa language).

Commands | Results
------------ | -------------
op()("4*2"); | ()[[op,8],{}]
op()("4*2",@$start); | ()[[op,8],{}]
op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){@a**@b}}); | ()[[op,()[[op,()[pow,{()[var,{a}],()[var,{b}]}]],{"a":4,"b":2}]],{}]
f = op()("4*2", rule $INT '*' $INT {op(a:$1,b:$3){@a**@b}});</br>f(); | 16

Note in the last example the rule to use was defined and passed in as a parameter to the planer, the operation result assigned to a variable, and then the variable executed as a function.

If the rules have already been defined, the following options to generate the execution plan - for either storing as a function or for debugging the rules.

Commands | Results | Description
------------ | ------------- | -------------
r = rule $INT `'*'` $INT {op(a:$1,b:$3){@a**@b}}; |  | Rule for "int * int", with associated code that applies the power operator
f = op()("4*2",@r);</br>f(); | 16 | Applying the rule to create a function, and running the function
f = @r.plan("4*2");</br>f(); | 16 | Another way to apply the rule
(@r.plan("4*2"))(); | 16 | Apply the rule and run the function in the same command

### Function Chaining
Any object that returns an object can be chained.

For example "4.4".float(300,4).pi() will convert the string 4.4 to a float with 300 bit percision and will return pi() to the power of the input, at the precision specified in the input. So, a 400 bit percision of pi to the power of 4.4. 

# Examples

## Crypt
### RSA test
The following will create an RSA key using hard coded public/private key values and verify the encode/decode functions work.

<pre><code>> e = 0xa932b948feed4fb2b692609bd22164fc9edb59fae7880cc1eaff7b3c9626b7e5b241c27a974833b2622ebe09beb451917663d47232488f23a117fc97720f1e7;
> d = (0x4adf2f7a89da93248509347d2ae506d683dd3a16357e859a980c4f77a4e2f7a01fae289f13a851df6e9db5adaa60bfd2b162bbbe31f7c8f828261a6839311929d2cef).uraw();
> d = (@d + (0x4f864dde65e556ce43c89bbbf9f1ac5511315847ce9cc8dc92470a747b8792d6a83b0092d2e5ebaf852c85cacf34278efa99160f2f8aa7ee7214de07b7).uraw()).uint();
> n = (0xe8e77781f36a7b3188d711c2190b560f205a52391b3479cdb99fa010745cbeba5f2adc08e1de6bf38398a0487c4a73610d94ec36f17f3f46ad75e17bc1adfec998395).uraw();
> n = (@n + (0x89f45f95ccc94cb2a5c500b477eb3323d8cfab0c8458c96f0147a45d27e45a4d11d54d77684f65d48f15fafcc1ba208e71e921b9bd9017c16a5231af7f).uraw()).uint();
> g = {e:@e,d:@d,n:@n};
> v = "this is a test of 95 chars to see if we can encode with RSA. It needs to be exactly 95...so need to make it so.".left(95);
> @v.encode("RSA-KEY",@g).decode("RSA-KEY",@g).str();
this is a test of 95 chars to see if we can encode with RSA. It needs to be exactly 95...so ne
</code></pre>

## Strings
### String Word Length
The following returns the length of each word in a string:
<pre><code>> "this is a test".split(" ").reduce(op(a,b){@a+=@b.len();},[])
[4,2,1,4]
</code></pre>

