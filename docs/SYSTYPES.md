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
"this is a test".raw(); | 7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996

# $INT
Supports signed and unsigned. Unsigned is essentially an $INT that is not negative. Given this is unlimited precision, a separate $UINT type is not required (grapa originally had one, but it's been removed as it's essentially redundant and adds unnecessary complexity). The reason other languages have a $UINT type is to handle to bit overflow issue where the high order bit may become set during math operations. In grapa, when the high order bit is set, it automatically increases the precision and there is no overflow. Still, there is sometimes a need for specific $UINT type handling of raw bytes, and to support this there is a uint() function to ensure that a raw byte stream with a high order bit is not incorrectly interpreted as a negative number. 

The $INT class inherits $math class - see the the $math class for additional functions supported.

Examples:
Commands | Results
------------ | -------------
52322; | 52322
"52322".int(); | 52322
(52322).raw(); | 00CC62

Command | Example | Result
------------ | ------------- | -------------
(n).modinv(m) | (3504).modinv(385) | 79
(n).modpow(p,m) | (4).modpow(13,497) | 445
(bits).random() | (16).random() | 11942
(bits).genprime() | (16).genprime() | 60913</br>
(prime).isprime() | (60913).isprime() | 1
(bits).genrsa() | (2048).genrsa() | {"n":15393516245753936682160067234556253412750430802255743778814229572544054294097259619627956984788210632327430260756422996446381253153249781345283196778146943869627387098734177324722359738660665976616853578383297592405166139097576403800731299888096509467468294806244038979679705825508378722753372351633863988005907566854534544949211856629499511658312380872778697665853199690604789204003585821802286627943688070991768307690016267599491818375209115984992719308845944592197596445761556160073580204570454244818609933341847015039592523178376768260760784860436523239377646201172460077663895533645989009628971123289151850803431,"e":65537,"d":13641994957861797801239860002487559671827288722325001124151707486967036535104884854479023172810767559173858271582969126350089616295233948769916963987896524100095497851511070372764616226275409004408301506515432872528374038463573821394730822245458981489396196993250435386725015095984354429063214156627474867988597189451628997325925681141616557506588552488558841864362946322635349455724856402259625223168594174978986354594425814335237529438542251361036952837522411419140167302914778094265903749135384053575403257513464743052615490964725367349355084629511192897896262038309134665089785944182678712425681786027820252233473,"p":127525351491210739758506603062091339866602472144588924864153885241513959931528868361901295124271172489233580871065840798194041581977748077769443836206183903474877250523613161796934787318751453058205160524179069496822619466659130977085289078629456414992621730953731935128046011636107260954453798334287510785301,"q":120709459458458215149090987557869193974463585849659373482943650636124808813105265511854715553573984494307648288391119051840787006493359514518751056098674649187782133751617034710796002529153468083214633567324645517989231684251962352981897617821614879663479614766494007790854662782949676080820575455806904229131,"dp":108596095966751303464951020191529297749595150341293670869029161869534043209464800256486402789118358567704303139193333675731438220373165425213770710512091146513105947395705127587853175236464895306855239081647156408565750184091115563729693122804390391142762503361411501404719774530410457094574240982014002615573,"dq":79822185239035386547008639681140960502697756741268869920866257705851304825401772994686355229271851626017438416807212986079253357453182395291417569757608098425318554595535026783290006524687626833549838862638135518235764876819377518860025313352047631946165975628275894679952688949562431328724264142450213093093,"qi":85353290251593823983293134708457294766739743735581970939586643632920134544715934389683783433537242132792046190122480630405078881427501723330138575663576456847405896072681721913683535190255437598808737817039439311188397481130265056015078027907590498609730777468666008326788985545089258489739084708975591421831}

The genprime and genrsa commands are not threaded at this time. Once threaded, performance will increase.

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
> -0x4.0x5
-4.3125
```

Or binaary format:
```
> 0b101.11
5.625
```

By default, all floats are "float" format. To change formats use the fix() and float() routines, where both the precision and the "extra" bits can also be specified. The parsing engine will set the precision to the system default, which is 128 bits. There are two ways to change this. First, change the system default using the setfloat and setfix routines. Second, pass in a $STR to the fix or float routines. For example:

```
> "3.5".float(300,6)
3.5
```

To verify the float is being created properly, use the decode routine with FLOAT as the parameter. For example:

```
> "30.75".float(300,6).decode(FLOAT)
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"extra":6,"data":123}
> (30.75).decode(FLOAT).data
123

> (30.75).decode(FLOAT).data.hex()
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
00048100077B
```

Bit shifts:
```
> (30.75) >> 4
1.921875

> (30.75) << 4
492
```

# $ID
Any identifier (starts with a letter and can follow with numbers and '-' but can not end with '-') will be initialized as an $ID. And an $ID can be used for many things, including associating a value. The '@' symbol is used to de-reference an $ID to retrieve the data stored.

Example of using an $ID as a variable:
```
> x = "hi"

> @x
hi

> @x + " test"
hi test
```

# $LIST
A $LIST is basically JSON, but extended to include all the various grapa data types. 

Action | Example | Result
------------ | ------------- | -------------
Create | {a:1, b:2, c:3} | {a:1, b:2}
Access | {a:1, b:2, c:3}.a</br>{a:1, b:2, c:3}[1]</br>{a:1, b:2, c:3}[-1] | 1</br>1</br>3
Append | x = {a:1, b:2};</br>x += (c:3);</br>@x; | </br></br>{a:1, b:2, c:3}
Append | x = {a:1, b:2};</br>x ++= {c:3,d:4};</br>@x; | </br></br>{a:1, b:2, c:3, d:4}
Insert | x = {a:1, b:2};</br>x += (c:3) @x[0];</br>@x; | </br></br>{"c":3,"a":1,"b":2}
Count | {a:1, b:2, c:3}.len() | 3
Remove | x = {a:1, b:2, c:3};</br>x -= @x[1];</br>@x; | </br></br>{a:1, c:3}

# $ARRAY
Same as $LIST, but without the entry labels.

Supports an offset search if the contents of the array are $ID or $STR or $ARRAY values.
```
> months = [JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC];
> @months[MAR];
2
> @months[FFF];
ERR: {"err":-1}
```

If the elements are $ARRAY type, the first item of the array is used for the match.
```
> x = [[a,496],[b,964],[c,221]];
> @x.b;
1
> @x[@x.b];
[b,964]
> @x.g
ERR: {"err":-1}
```

There isn't currently built in support for searching for an $ARRAY or $LIST item within an $ARRAY. Currently, this would require an itterator to scan and compare, or the use of a hash of the contents as a key.

# $TIME
Uses UTC timezone. $TIME is intended to be a timestamp starting at Jan 1 1970, and a timer for tracking the passage of time. 

Examples:
```
> (0).time();
1970-01-01T00:00:00.000000

> $TIME().tz();
-25200000000000

> $TIME().tz()/1000000000
-25200

> $TIME().utc();
2020-07-10T19:05:58.6135794

> $TIME().utc() + $TIME().tz();
2020-07-10T12:06:15.9529207

> t1 = $TIME().utc();

> @t1.delta();
3.8281652

/* From an AWS docker container on Win10 */
> a = $TIME().utc(); (($TIME().utc()-@a)/1000000000.0).str()+" seconds";
0.000012 seconds
```

The following illustrate using various date ranges.
'''
> "1020-04-23T12:33:33.921638".time();
1020-04-23T12:33:33.921638

> "1020-04-23T12:33:33.921638".time() + 24*60*60*1000000000;
1020-04-24T12:33:33.921638

> "-1020-04-23T12:33:33.921638".time() + 1;
-1020-04-23T12:33:33.921638001

> "-1020-04-23T12:33:33.921638".time().int();
-94345471586078362000

> (-94345471586078362000).time();
-1020-04-23T12:33:33.921638
''''

And a side effect of using 1970 as the center. 
```
> a = $TIME().utc();

> @a;
2020-07-10T19:51:26.3316532

> (0-@a).time();
1919-06-24T04:08:33.6683468
```

# $TABLE
A $TABLE is a hierarchical database with columns, rows, with both row store and columns store. 

See $file commands for creating, updated, and navigating.
```
> f=$file().table()
> @f.ls()
[]

> @f.set("test","value")
> @f.ls()
[{"$KEY":"test","$TYPE":"ROW","$BYTES":5}]

> @f.get("test")
value

> @f.type()
$TABLE
```

# $RAW
A $RAW represents raw bytes. Most data types can be converted to and from $RAW, providing the ability to make specific tweaks to data. For example, this is how time addition/subtraction is performed - by converting the $TIME into $RAW and then into an $INT, and than back to a $TIME. There are several examples of using raw in the documentation for the other data types.

When displayed, the value is printed in hex form, but the value in memory is in raw. If you convert to hex(), the result will be a text version of the hex of the raw data.

```
> "hi".raw();
6869

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
> x = <test one=1>data</test><tt hi=dd />;

> @x.len();
2

> @x[0];
<test one=1>data</test>

> @x[1];
<tt hi=dd/>
```

Or specify a tag name.
```
> x = <test one=1>data</test><tt hi=dd />;
> @x.tt;
<tt hi=dd/>
```

Use $LIST to access the attributes.
```
> x = <test one=1>data</test><tt hi=dd />;

> @x.test.$LIST.len();
1

> @x.test.$LIST.one;
1
```

Index into the $TAG to access the contents.
```
> x = <test one=1>data</test><tt hi=dd />;

> @x.test.len();
1

> @x.test[0];
data
```

To convert $XML or $TAG to JSON format, use the list method. This produces the underlying representation of how the $XML and $TAG values are stored.
```
> x = <test one=1>data</test><tt hi=dd />;

> @x.list();
[{"test":[{"one":1},["data"]]},{"tt":[{"hi":dd}]}]

> @x.test.list();
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
> x = rule $INT $INT {op(a:$1,b:$2){@a*@b}} | $INT {op(a:$1){@a}}
> (op()("4",@x))()
4

> (op()("4 3",@x))()
12

> op()("4",@x)
@<[op,@<[op,@<var,{a}>],{"a":4}>]>

> op()("4 3",@x)
@<[op,@<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{"a":4,"b":3}>]>
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
> x = rule $STR {@<lit,{$1}>};
> r = rule $INT <x,op(a:$1){@a.len()}> $INT {op(a:$2){@a}};
> (op()("44 'x' 22",@r))();
1
```

Rules can also be embedded. Unfortunately, the current grammar requires an operation for a rule if the rule is to return any value - it doesn't make any assumptions. So as of the current release an embedded rule of (x|y) would match on x or y, but with no operation attached to x or y the result would not produce an output for the match. In some future version, this will be addressed to return a default. For now, an operation needs to be inserted if you want a value to be returned.

If the token handler returns an $ERR object, it will cause the rule to fail. So it not only has the ability to add additional logic to processing the token, is also can also serve as an additional component to the planning engine by validating the token against the intent of the rule - something that could not be done at a later phase and could only be done during the planning/compile phase. 

```
> r = rule $INT (x{@<lit,{"found x"}>}|y{@<lit,{"found y"}>}) $INT {op(a:$2){@a}};
> (op()("44 y 22",@r))();
found y
```

If there is a well known end sequence, and you want to collect all the tokens in between, you can insert an empty rule. All of the tokens, including any white space, will be passed on. The risk of using this in a rule is that if the end token does not exist, all tokens will be processed in the empty rule. 

```
> r = rule $INT <> $INT {op(a:$2){@a}};
> (op()("44 'x' 22",@r))();
[" ","x"," "]
```

The stop rule for a catchall can be either a token, a rule reference, or an embedded rule.

```
> e = rule $INT {@<lit,{$1}>} | $ID {@<lit,{$1}>};
> r = rule $INT <> <e> {op(a:$3){@a}};
> (op()("44 'x' 22",@r))();
22

> (op()("44 'x' hi",@r))();
hi

> r = rule $INT <> ($INT{@<lit,{$1}>}|$ID{@<lit,{$1}>}) {op(a:$3){@a}};
> (op()("44 'x' 22",@r))();
22

> (op()("44 'x' hi",@r))();
hi
```

A operation can also be associated with the empty rule above, which can process the tokens and either pass through the list, or pass through some transformed version. The following is an example f transforming.

```
> r = rule $INT <op(a:$1){@a.len().str()+" raw characters:"+@a.join("")}> $INT {op(a:$2){@a}};
> (op()("44 'x' 22",@r))();
3 raw characters: x
```

If the token handler returns an $ERR object, the corresponding rule option will fail and the next rule option will be evaluated. The following is an example of causing the first rule option to fail.

```
> r = rule $INT <op(a:$1){$ERR()}> $INT {op(a:$2){@a}} | $INT <> $INT {op(a:$2){@a}};
> (op()("44 'x' 22",@r))();
[" ","x"," "]
```

The catchall token can also be used to add support for comments in a grammar. The following is used in the grapa syntax. This rule is then inserted in places in the grammar where comments can be included. 

```
$starcomment = rule ('/' '*') <> ('*' '/');
```

Rules can include both right recursion and left recursion.  The following is an example of right recursion. Note that an internal function "prepend" is used to build the list. This is because grapa uses recursive decent, which results in the last match being processed first.

```
> r1 = rule $ID <r1> {@<prepend,{$2,$1}>} | $ID {@<createarray,{$1}>};
> (op()("a b c",@r1))();
[a,b,c]
```

But right recursion may not be desirable in some cases. One example is order of operations for additon and subtraction, where the evaluation needs to happen from left to right. Here is an example using right recursion. 

```
r1 = rule 
     $INT '+' <r1>             {@<add,{$1,$3}>} 
  | $INT '-' <r1>              {@<sub,{$1,$3}>} 
  | $INT                            {@<lit,{$1}>}
  ;

> op()("5-3+2",@r1);
@<[op,@<sub,{5,@<add,{3,2}>}>],{}>

> (op()("5-3+2",@r1))();
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

> op()("5-3+2",@r2);
@<[op,@<add,{@<sub,{5,3}>,2}>],{}>

> (op()("5-3+2",@r2))();
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
1
```


# $SYSID
Same as $ID, but was initialized with '$' at the front of the identifier. Used for special system identifiers. 

# $SYSINT
Same as $INT, but was initialized with '$' at the front of the integer. Used for special system integers. 

# $SYSSTR
Same as $STR, but was initialized with '$' at the front of the string. Used for special system strings. 
