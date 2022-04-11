# $BOOL
The following are equivalent in comparisons:
- True: true, 1
- False: false, null

Examples:
Commands | Results
------------ | -------------
1==true; | true
null==false; | true
1==false; | false
"55"==(55).str(); | true
"5.54"==(5.54).str(); | true

# $STR
ASCII only - Unicode will eventually be added. Can initialize with either double quotes or single quotes - which is easier than escaping a string that includes a quoted string - such as "this 'is' a test", or 'this "is" a test'. $STR inherits the $obj class - see $obj for functions supported. 

Examples:
Commands | Results
------------ | -------------
"this is a test".len(); | 14
"this is a test".split(""); | ["t","h","i","s"," ","i","s"," ","a"," ","t","e","s","t"]
"this is a test".raw(); | 0x7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996

# $INT
Supports signed and unsigned. Unsigned is essentially an $INT that is not negative. Given this is unlimited precision, a separate $UINT type is not required (grapa originally had one, but it's been removed as it's essentially redundant and adds unnecessary complexity). The reason other languages have a $UINT type is to handle to bit overflow issue where the high order bit may become set during math operations. In grapa, when the high order bit is set, it automatically increases the precision and there is no overflow. Still, there is sometimes a need for specific $UINT type handling of raw bytes, and to support this there is a uint() function to ensure that a raw byte stream with a high order bit is not incorrectly interpreted as a negative number. 

The $INT class inherits $math class - see the the $math class for additional functions supported.

Examples:
Commands | Results
------------ | -------------
52322; | 52322
"52322".int(); | 52322
(52322).raw(); | 0x00CC62

Command | Example | Result
------------ | ------------- | -------------
(n).modinv(m) | (3504).modinv(385) | 79
(n).modpow(p,m) | (4).modpow(13,497) | 445
(bits).random() | (16).random() | 11942
(bits).genprime() | (16).genprime() | 60913</br>
(prime).isprime() | (60913).isprime() | 1
[method].genkeys(bits) | "rsa".genkeys(2048)| {"method":"RSA","n":133340969878490876761315268119495128494223592724998328527037443092680058163238200550226099759810589020622773180707498143917954376840030816382652791849882910685566880060504667253115225255092587725908463639052005806166349797227866973060510752508533977442800215647215704344710830305717250386898264812566696319357,"e":65537,"d":21462897161118150952211952994378047675138695388193041604463402157326120108701951227616996908101406893488405546230120358273807173372072036895503369107899577024301153717464245508429552994282320868505600503184606269169145300796609156095355884623057408882085310653464552867426166711714262346885341708845892087245,"p":13018924438699683584351960540369401752216593462087352663934874988571387642612039737870891924873164443636284470531329392240434226360455419391927666947798619,"q":10242088008601179955032546512112320803965163751179416704553769484689836112666734931701358640253590592335899278834853169695399444476865991657075568700106503,"dmp1":1548873977273012693702675379301161564643373654941408497805822974592842355454873946567272599268139572562554129983563105898937480551939681477621191375741731,"dmq1":5424929445085566317943219663319269909639481961253812229201443461281997665120471942324934960085492015682073815801673223974798982500350171176466182710328471,"iqmp":10773193752676840425045540382104355834555937318335526551444565657080681982821255440896494264459939256156471093125837581716788587780960159457805940209113560}
[method].genkeys(bits) | "dh".genkeys(2048)| {"method":"DH","p":167078505354342589623427531492070898897587317696780796495676967395654292799803550840398173836912469408479735924893415233091677021366974572094690601960343065472632718027733503897827167893707501104206432738261416896424762983822126266525382323422153324376521434066353958039942900708734698139361179368319160431579,"g":2,"pub":14955576220841616887417379990591146892005211708711628337065388059470382074287246234855039345355319994984538334678860682678775812791815222790987279094705284827152039498898778569530087184692169346653614591096732468758433738942280543650288875155511119797833901004686162214179337541808548760952697675904359431835,"prv":80473203669684850361358238923455160620969904261951719764579723274723983598603094098470687597414850420649402185596369251972214494397863209489964591507277182287056638504511782608178939474196777068400324267938853688096295413165184178155627262041570190419735976104264552360432339859596697998475206199750365187654}
[method].genkeys(bits) | "ec".genkeys(256)| {"method":"EC","curve":"prime256v1","pub":328583515414440344309737813659797994379375465829367947313294493668935200753665,"prv":79112223757318388912284493496056150146052605496312695677413829416913194654396}
[method].genkeys(bits) | "bc".genkeys(256)| {"method":"EC","curve":"prime256v1","pub":328583515414440344309737813659797994379375465829367947313294493668935200753665,"prv":79112223757318388912284493496056150146052605496312695677413829416913194654396}


# $FLOAT

Supports both fix and float format. Fix will apply the precision to just the decimal. Float will not restrict to the decimal, supporting large exponents with a specified precision. Each number maintains it's own precision, which is used in math operations with numbers of other precision. Some math operations may convert an input from one format to another. For example, passing in a float to a trig function will produce a fix format result. Floats also support specifying "extra" bits to apply to the calculations to reduce error propagation (the default is 7 bits). 

The $FLOAT class inherits $INT class - see the the $INT class for additional functions supported.

Example:
```
> 5.13
5.13
```

You can also create using hex format:
```
> -0x4.5
-4.3125
```

Or binaary format:
```
> 0b101.11
5.75
```

By default, all floats are "float" format. To change formats use the fix() and float() routines, where both the precision and the "extra" bits can also be specified. The parsing engine will set the precision to the system default, which is 128 bits. There are two ways to change this. First, change the system default using the setfloat and setfix routines. Second, pass in a $STR to the fix or float routines. For example:

```
> "3.5".float(300,6)
3.5
```

To verify the float is being created properly, use the decode routine with FLOAT as the parameter. For example:

```
> "30.75".float(300,6).decode("FLOAT")
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"extra":6,"data":123}
> (30.75).decode("FLOAT").data
123

> (30.75).decode("FLOAT").data.hex()
7B
```

Convert to other formats:
```
> (30.75).hex()
1E.C

> (30.75).bin()
11110.0011

> (30.75).int()
30

> (30.75).raw()
0x00048100077B
```

Bit shifts:
```
> (30.75) >> 4
1.921875

> (30.75) << 4
492.0
```

# $ID
Any identifier (starts with a letter and can follow with letters or numbers will be initialized as an $ID. And an $ID can be used for many things, including associating a value. Optionally, the '@' symbol is used to de-reference an $ID to retrieve the data stored - which is useful when being used in a location that requires and $ID (such as a label). When used in a location that requires a value, an $ID will be automatically dereferenced. 

Example of using an $ID as a variable:
```
> x = "hi"

> x
hi

> x + " test"
hi test

> {x:"test"}
{"x":"test"}

> {@x:"test"}
{"hi":"test"}
```

# $LIST
A $LIST is basically JSON, but extended to include all the various grapa data types. 

Action | Example | Result
------------ | ------------- | -------------
Create | {a:1, b:2, c:3} | {"a":1,"b":2,"c":3}
Access | {a:1, b:2, c:3}.a</br>{a:1, b:2, c:3}[1]</br>{a:1, b:2, c:3}[-1] | 1</br>1</br>3
Append | x = {a:1, b:2};</br>x += (c:3);</br>x; | </br></br>{"a":1, "b":2, "c":3}
Append | x = {a:1, b:2};</br>x ++= {c:3,d:4};</br>x; | </br></br>{"a":1, "b":2, "c":3, "d":4}
Insert | x = {a:1, b:2};</br>x += (c:3) x[0];</br>x; | </br></br>{"c":3,"a":1,"b":2}
Count | {a:1, b:2, c:3}.len() | 3
Remove | x = {a:1, b:2, c:3};</br>x -= x[1];</br>x; | </br></br>{"a":1, "c":3}

# $ARRAY
Same as $LIST, but without the entry labels.

Supports an offset search if the contents of the array are $ID or $STR or $ARRAY values.
```
> months = ["JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"];
> months["MAR"];
2
> months["FFF"];
{"err":-1}
> months["FFF"].iferr(-1);
-1
```

If the elements are $ARRAY type, the first item of the array is used for the match.
```
> x = [["a",496],["b",964],["c",221]];
> x."b";
1
> x[x."b"];
["b",964]
> x.g
{"err":-1}
```

There isn't currently built in support for searching for an $ARRAY or $LIST item within an $ARRAY. Currently, this would require an itterator to scan and compare, or the use of a hash of the contents as a key.


# $TABLE
A $TABLE is a hierarchical database with columns, rows, with both row store and columns store. 

See $file commands for creating, updated, and navigating.
```
> f=$file().table()
> f.ls()
[]

> f.set("test","value")
> f.ls()
[{"$KEY":"test","$TYPE":"ROW","$BYTES":5}]

> f.get("test")
value

> f.type()
$TABLE
```

# $RAW
A $RAW represents raw bytes. Most data types can be converted to and from $RAW, providing the ability to make specific tweaks to data. For example, this is how time addition/subtraction is performed - by converting the $TIME into $RAW and then into an $INT, and than back to a $TIME. There are several examples of using raw in the documentation for the other data types.

When displayed, the value is printed in hex form, but the value in memory is in raw. If you convert to hex(), the result will be a text version of the hex of the raw data.

```
> "hi".raw();
0x6869

> "hi".raw().int();
26729

> (0x6869).raw().str();
hi
```

# $XML
$XML is an array of $TAG or $STR values. The $XML class also inherits the $ARRAY class, and any operation that works on an $ARRAY will work on an $XML. 

For consistency, entering a single $TAG will always result in the creation of $XML, where the $XML includes at least 1 $TAG. 

To reference items in $XML, use an index.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.len();
2

> x[0];
<test one=1>data</test>

> x[1];
<tt hi="dd"/>
```

Or specify a tag name.
```
> x = <test one=1>data</test><tt hi="dd" />;
> x.tt;
<tt hi="dd"/>
```

Use $LIST to access the attributes.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.test.$LIST.len();
1

> x.test.$LIST.one;
1
```

Index into the $TAG to access the contents.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.test.len();
1

> x.test[0];
data
```

To convert $XML or $TAG to JSON format, use the list method. This produces the underlying representation of how the $XML and $TAG values are stored.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.list();
[{"test":[{"one":1},["data"]]},{"tt":[{"hi":dd}]}]

> x.test.list();
{"test":[{"one":1},["data"]]}
```

# $TAG

A $TAG includes a tag name, a list of attributes, and an $XML (which is a list of $TAG or $STR values). 

See $XML.

# $VECTOR

A $VECTOR is a multi-dimentional set of values. Most often either 1 or 2 dimentions. 

The syntax for a $VECTOR is the same as an $ARRAY, but uses a '#' before and after the array, and requires all values for each dimention to be specified (for a 2 dimentional vector, all rows much have the same number of columns).

# $RULE
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
	○ https://medium.com/@gvanrossum_83706/left-recursive-peg-grammars-65dab3c580e1
A simular approach is used for Grapa - but with a few improvements on the appropach to support more complex scenarios than what's required to support the Python syntax alone.


# $ERR
If an operation results in an error, the $ERR data type is returned. Check using the type function: if (@result.type()==$ERR) something;

Under the hood, the $ERR type is a $LIST, and the same commands can be used to get more details on the error.

```
> (1/0).type()==$ERR
true
```


# $SYSID
Same as $ID, but was initialized with '$' at the front of the identifier. Used for special system identifiers. 

# $SYSINT
Same as $INT, but was initialized with '$' at the front of the integer. Used for special system integers. 

# $SYSSTR
Same as $STR, but was initialized with '$' at the front of the string. Used for special system strings. 
