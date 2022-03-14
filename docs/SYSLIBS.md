# $sys
A few general utility functions that are useful, but it wasn't clear if they should be added to the native language syntax, were added to $sys.

## type (object)
```
> $sys().type(5)
$INT
```

## getenv (type)

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
$PLATFORM | C++/G++ compile flags
Any value not starting with '$' will be directed to the native OS getenv/putenv |

```
> $sys().getenv($VERSION)
{"major":0,"minor":0,"micro":2,"releaselevel":"alpha","serial":63,"date":2020-04-24T16:30:37.000000}
```

Values for $PLATFORM 
* `__APPLE__`
* `_WIN32`
* `_WIN64`
* `__linux__`
* `__ANDROID__`
* `_MSC_VER`
* `_MSC_FULL_VER`
* `__GNUC__`
* `__GNUC_MINOR__`
* `__clang__`
* `__MINGW32__`
* `__MINGW64__`
* `__i386__`
* `__x86_64__`
* `__arm__`
* `__ARM_ARCH_5T__`
* `__ARM_ARCH_7A__`
* `__powerpc64__`
* `__aarch64__`

## putenv (type, value)
See getenv.

# $obj
Several classes inherit $obj, such as $STR and $INT and $LIST. Functions that can be used across the different data types are placed in the $obj class. Some of these functions may move to other classes. The $obj class is a general place to place functions as a starting point. For example, the setfloat and setfix functions may move to the $sys class. 

There is not much use in creating an $obj instance on it's own. 

## type()
All objects will return the type.

```
"test".type() will return $STR, for example.
```

If the item does not support type(), use $sys().type(value). For example, $sys().type("test") returns $STR.


## describe (object)
This is stubbed out...but not yet implemented. Type will provide just the type. Describe will provide more detail. If using describe on a table, details about the table will be returned, for example. Intention is to provide describe everywhere that type is available.

## raw()
Converts a value into it's raw bytes. Displays in hex form, but is stored in raw. Required in many cases as an intermediate form. For example, converting from a $STR to an $INT, you have two choices.

```
> "34".int();
34

> "34".raw();
3334

> "34".raw().int();
13108
```

## uraw()
Converts to an unsigned raw value. To avoid sign issues, a leading zero is added to raw/int. To remove, used uraw and uint.

```
> (0xFF).raw();
00FF

> (0xFF).uraw();
FF

> (0xFF).raw().int();
255

> (0xFF).raw().uint();
255

> (0xFF).uraw().int();
-1

> (0xFF).uraw().uint();
255
```

#### bool()
Converts to what the system sees as true/false.

```
> "1".bool();
true

> (0).bool();
false
```

## int()
Converts to $INT.

```
"44".int() -> 44
```

## uint()
Converts to unsigned $INT.

See $INT.

## float([bits [,extra]])
Converts to $FLOAT. Sets bit count for the entire number. Calculations are performed with "extra" bits and truncated for display.

```
"4.21".float() -> 4.21

"4.21".float(300,7) / "10412.42".float(300,7) -> 0.00040432483514879346011782083319727786624050893068085997299379010835137268761728

("4.21".float(300,7) / "10412.42".float(300,7)).float(50) -> 0.00040432483514879
```

## fix([bits [,extra]])
Converts to fixed float. Sets bit count after the decimal to bits. Calculations are performed with "extra" bits and truncated for display.

## setfloat([bits [,extra]])
Sets the default float type to float, and the default bits and extra.

## setfix([bits [,extra]])
Sets the default float type to fix, and the default bits and extra.

## str()
Converts to string. 

```
(44).str() -> "44"
```

## base(base)
Converts number to base. Base of power of 2 works well. Have not fully tested others. Performed as a series of mods and divisions. So can become expensive on very large numbers. Split large numbers into parts before using - but careful on the split. 

```
(15).base(8) -> 17
(15).base(7) -> 21
```

## hex()
Converts item to hex. The hex value is stored as an ascii representation of '0'-'F' characters.

## bin()
Converts to binary. 

```
(0xC).bin() -> 1100
```

## time()
Converts an $INT to $TIME.
See $TIME.

## bits()
Gets the bit count of the item.

```
"hi".bits() -> 16
```

## bytes()
Gets the byte count of the item.

```
"hi".bytes() -> 2
$TIME().utc().raw().bytes() -> 7
```

## len
Gets the length of the item.

```
"hi".len() -> 2
{1,2,3}.len() -> 3
```

## left(count)
Gets the left bytes of an item.

```
"testing".left(2) -> "te"
```

Use a negative number to truncate right.

```
"testing".left(-2) -> "testi"
```

## right(count)
Gets the right bytes of an item.

```
"testing".right(2) -> "ng"
```

Use a negative number to truncate left.

```
"testing".right(-2) -> "sting"
```

## mid(start,len)
Gets the middle bytes of an item.

```
"testing".mid(2,3) -> "sti"
```

## rtrim([str])
Trims right.

```
"  testing  ".rtrim() -> "  testing"
"bbbtestingbbb".rtrim(b) -> "bbbtesting"
```

## ltrim([str])
Trims left.

```
"  testing  ".ltrim() -> "testing  "
"bbbtestingbbb".ltrim(b) -> "testingbbb"
```

## trim([str])
Trims both left and right.

```
"  testing  ".trim() -> "testing"
"bbbtestingbbb".trim(b) -> "testing"
```

## lpad(n,[str])
Pads left to bring the total size up to n characters. Defaults to pad with a space, but will use str for padding if provided. 

Will left truncate input if length of input is less than n.

```
"test".lpad(7,"X") -> "XXXtest"
```

## rpad(n,[str])
Pads left to bring the total size up to n characters. Defaults to pad with a space, but will use str for padding if provided. 

Will right truncate input if length of input is less than n.

```
"test".rpad(7,"X") -> "testXXX"
```

## lrot([n])
For $LIST, $ARRAY, $XML.

Moves n (defaul=1) items from the start of the list to the end of the list, 1 at a time.

```
["a","b","c","d","e"].lrot(2) -> ["c","d","e","a","b"]
```

## rrot([n])
For $LIST, $ARRAY, $XML.

Moves n (defaul=1) items from the end of the list to the start of the list, 1 at a time.

```
["a","b","c","d","e"].rrot(2) -> ["d","e","a","b","c"]
```

## reverse()
Reverses the older of a list.

```
{z:1,m:2,p:3,b:4}.reverse() -> {"b":4,"p":3,"m":2,"z":1}
"testing".reverse() -> "gnitset"
```

## replace(old,new)
Replaces iteems.

```
"testing".replace("t","g") -> "gesging"
```

## split(item [,param])
Splits into an array.

```
"one\ntwo\nthree".split("\n") -> ["one","two","three"]
"this is a test".split(" ") -> ["this","is","a","test"]
"this is a test split into parts".split(3) -> ["this is a t","est split i","nto parts"]
"this is a test split into parts".split(3," ") -> ["this is a test ","split into ","parts"]
```

## join(item)
Joins what has been split.

```
["this is a test ","split into ","parts"].join("") -> "this is a test split into part"
```

## upper()
Converts to upper case.

```
"hi".upper() -> "HI"
```

## lower()
Converts to lower.

```
"HI".lower() -> "hi"
```

## setbit(bitpos)
Sets a bit, from low order.

```
(0b100110).setbit(3).bin() -> 101110
```

## clearbit(bitpos)
Clears a bit, from low order. 

```
(0b100110).clearbit(2).bin() -> 100010
```

## genbits
Generates sequence of bits.

```
(4).genbits() -> 15
(4).genbits().bin() -> 1111
```

## encode (type, [,options])

encode/decode types:
Type | Options | Description
------------ | ------------- | -------------
RSA-KEY | {n,e,d [,p,q,dp,dq,qi]} | Data size must be exactly byte count of d  minus 32.
AES256 | {key [,iv]} | key is 32 bits and iv is 16 bits
ZIP-GRAPA | |
BASE64 | |
ESCAPE | |
FLOAT | |
JSON | | Only encode.
XML | | Only encode.
SHAKE256 | | Only encode.
SHAKE128 | | Only encode.
SHA3-256 | | Only encode.
SHA3-384 | | Only encode.
SHA3-512 | | Only encode.
PEM | | Only decode.

Example of RSA using hard coded RSA key.

```
e = 65537;
d = (0xA4954A19EA13D5FF06C86E7719AB6B6D5F37EF2D0A453713F08CD3FF2847C7BE80F2A3D0159C2EA862997DEE93DC08CD18CDFBD96DCC077B978904D05B2FCA5988A63).uraw();
d = (d + (0x2CEB3B7286ACEF97682F418172B30509BD29A1C56E0244D79CFADD5A60C83D2F28A8B72643CB82AABA08871E094B36E6613B108166768B33F5C4860BD21).uraw()).uint();
n = (0xB47D8ABBD7A33B5AD29C8EFD291433D62695BA043F4130F8E677CD179315FD0DD39989539EC710EB3BCBCACBDED2230DE8575FAEE3ED2835B256ADB1AAEF2F34C430E).uraw();
n = (n + (0xAB9934FAF1977FA83E12B947806CE1F8D06692F02068FD18E4DE952895DDD2D9BDEE923E94257E84B63D21C92EC1F74B8AF4AF9A33608C7E8247F1C820F).uraw()).uint();
g = {"method":"RSA","n":n,"e":e, "d":d};
len = g.n.bytes()-42;
v = "this is a test of 95 chars to see if we can encode with RSA. It needs to be exact size...so need to make it so.".left(len);
v.encode(g).decode(g).str();
```

```
v.encode("AES256",{key:"12345678123456781234567812345678",iv:"1234567812345678"}).decode("AES256",{key:"12345678123456781234567812345678",iv:"1234567812345678"}).str();
v.encode("SHA3-256");
v.encode("SHAKE128");
{1,2,3}.encode("ZIP-GRAPA").decode("ZIP-GRAPA");
v.encode("BASE64").decode("BASE64").str();
```

## decode (type, value [,options])
See encode.

## setfile($file, name)
Updates file or table item with value. Separate from the $file class as this one supports chaining all the way to writing the result into a file. Requires having an existing $file instance -> or passing in $file() which creates a temporary instance.

"testing".setfile($file(),"test.txt");

## map($OP [,params])
See map in the Looping section. Iterates through a $LIST/$ARRAY calling an $OP. Processes each item in parallel, so be sure $OP is thread safe. If not, use reduce instead of map. Result of $OP placed in a $LIST/$ARRAY which is returned at the completion.

## filter($OP [,params])
See filter in the Looping section. Iterates through a $LIST/$ARRAY calling an $OP. Processes each item in parallel, so be sure $OP is thread safe. If not, use filter instead of map. If $OP of op is true, item is placed in a $LIST/$ARRAY which is returned at the completion. 

## reduce($OP [,start [,params]])
See reduce in the Looping section. Iterates through a $LIST/$ARRAY calling an $OP. Processes each item in sequence as the intent is to combine results of each $OP. If "start" not provided, the first item of the list is used as the start. 

## sort([axis],[order],[op])
Sorts a $LIST.

- axis -> 0 = row, 1 = col
- order -> 0 = ascend, 1 = decend, or does selection if array if indexes passed in
- op -> function for the compare

```
{z:1,m:2,p:3,b:4}.sort() -> {"b":4,"m":2,"p":3,"z":1}
```

Optionally pass in a compare routine.'''
```
> ["b","a","B","c","b","A"].sort()
["A","B","a","b","b","c"]

> ["b","a","B","c","b","A"].sort(0,0,op(a,b){a.upper()<=>b.upper();})
["a","A","B","b","b","c"]

> ["b","a","B","c","b","A"].sort(0,0,op(a,b){$local.c=a.upper()<=>b.upper();if(c==0)c=a<=>b;c;})
[A,a,B,b,b,c]

> ["b","a","B","c","b","A"].sort(0,[1,2])
["a","B"]
```

## unique([op])
Remove duplicates names.

```
{z:1,b:4,m:2,p:3,m:2,b:4}.unique() -> {"z":1,"b":4,"m":2,"p":3}
```

Optionally pass in a compare routine.'''
```
> ["b","a","B","c","b","A"].unique()
["A","B","a","b","c"]

> ["b","a","B","c","b","A"].unique(op(a,b){a.upper()<=>b.upper();})
["A","b","c"]
```

## isint
Checks if a string is an $INT.

```
"z55".isint() -> false
"55".isint() -> true
```

## iferr(new)
If value is $ERR type, the new is used. Otherwise the value is used.

```
(10/2).iferr(55) -> 5
(10/0).iferr(55) -> 55
```

## exec()
Executes string in the shell of the operating system. 

```
> "ver".exec()
Microsoft Windows [Version 10.0.18363.778]
```

## getname([index])
Used in a map/reduce/filter to identify the name of the passed in item from the original list (if processing a $LIST). 

To use, do not de-reference the identify. The getname function will then see that it is an $ID and will attempt to locate it in the namespace. Since what is passed in is a pointer, the function is able to discover the item in the original list and discover the name.

```
> {a:1,b:2}.map(op(a){"a".getname()})
{"a":a,"b":b}
```

Or used to get the name of an item in a $LIST, using the index parameter.

```
> {a:1,b:2}.getname(0)
a
```

## echo()
Outputs item to stdio.

## string()
Outputs escaped version of item to stdio 


# $math()

Inherits $obj().

Command | Example | Result
------------ | ------------- | -------------
$math().e(power) | $math().e()</br>$math().e(2.3) | 2.71828182845904523536028747135266</br>9.9741824548147207399576151569088
$math().pi(power) | $math().pi()</br>$math().pi(2.3) | 3.1415926535897932384626433832795</br>13.9137663852357115150632799555148
(val).log(base) | (100).log(10) | 2
(val).ln() | (100).ln() | 4.605170185988091368035982909368
(val).sin() | (0.5).sin()</br>(1).pi().sin() | 0.47942553860420300027328793521557</br>0
(val).cos() | (0.5).cos() | 0.87758256189037271611628158260382
(val).tan() | (0.5).tan() | 0.5463024898437905132551794657802
(val).cot() | (0.5).cot() | 1.830487721712451919268019438968
(val).sec() | (0.5).sec() | 1.1394939273245491223133277682049
(val).csc() | (0.5).csc() | 2.08582964293348818577250167545929
(val).asin() | (0.5).asin() | 0.52359877559829887307710723054658
(val).acos() | (0.5).acos() | 1.04719755119659774615421446109316
(val).atan() | (0.5).atan() | 0.46364760900080611621425623146121
(val).acot() | (0.5).acot() | 1.107148717794090503017065460178
(val).asec() | (1.5).asec() | 0.8410686705679302557765250318264
(val).acsc() | (1.5).acsc() | 0.72972765622696636345479665981332
(val).sinh() | (0.5).sinh() | 0.5210953054937473616224256264114
(val).cosh() | (0.5).cosh() | 1.1276259652063807852262251614026
(val).tanh() | (0.5).tanh() | -0.46211715726000975850231848364367
(val).coth() | (0.5).coth() | -2.163953413738652848770004010218
(val).sech() | (0.5).sech() | 0.8868188839700739086588977977834
(val).csch() | (0.5).csch() | 1.919034751334943719492202878727
(val).asinh() | (0.5).asinh() | 0.48121182505960344749775891342436
(val).acosh() | (2).acosh() | 1.3169578969248167086250463473079
(val).atanh() | (0.5).atanh() | 0.5493061443340548456976226184612
(val).acoth() | (0.5).acoth() | 0.5493061443340548456976226184612
(val).asech() | (0.5).asech() | 1.3169578969248167086250463473079
(val).acsch() | (0.5).acsch() | 1.4436354751788103424932767402731
$math().atan2(x,y) | `$math().atan2(10,10)*180/$math().pi()` | 45
$math().hypot(x,y) | $math().hypot(3,4) | 5

# $file()
Provides the ability to navigate either the file system or a database, querying data and updating data. This class/libraries will be enhanced over time to support navigating data types beyond the file system and the grapa database - such as JSON/XML and unstructured data where a mapping can be defined (maybe with a set of rules). With a few additional enhancements, this class/library will also enable extending the grapa syntax to include SQL with $file for the underlying data.

Each example below assumes the following command has been issued:
<pre><code> f = $file();</code></pre>
Which assignes f an instance of the $file class. The following are then operations that can be used from the $file class.

The name field for the commands can include a path relative to the "working directory" (see pwd()). If the "working directory" is a OS filesystem directory, than the path must reference a file within the OS filesystem. If the "working directory" is a grapa table, than the path and data item would be within the grapa table. What is not currently supported is referencing a grapa table item when the "working directory" is not within a grapa table.

## type()
```
> f.type()
$file
```
## table()
The table function creates an in memory database. 

```
> t = f.table()
> t.mkrow("test","data for test")
> t.getrow("test")
data for test
```
## pwd()
Returns the current working directory, relative to the current home directory. 
```
> f.pwd()
/

> f.cd("lib")
> f.pwd()
/lib
```
## cd([name])
Changes the current working directory, relative to the current home directory.  Using ".." will result in moving back 1 level.
```
> f.cd("lib")
> f.pwd()
/lib
```
## phd()
Returns the current home directory.
```
> f.phd()
C:\Projects\Test
```
## chd(filesystempath)
Changes the current home directory.
```
> f.chd("lib")
> f.phd()
C:\Projects\Test\lib
```
## ls([name])
Retrieves a list of files/directories in the current working directory.

```
> f.chd("/Microsoft/AndroidNDK64/android-ndk-r16b/python-packages");
> f.ls();
[
  {"$KEY":"adb","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"Android.bp","$TYPE":"FILE","$BYTES":784},
  {"$KEY":"fastboot","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"gdbrunner","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"NOTICE","$TYPE":"FILE","$BYTES":577},
  {"$KEY":"repo.prop","$TYPE":"FILE","$BYTES":4465}
]

> f.chd("/Microsoft/AndroidNDK64/android-ndk-r16b");
> f.ls("python-packages");
[
  {"$KEY":"adb","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"Android.bp","$TYPE":"FILE","$BYTES":784},
  {"$KEY":"fastboot","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"gdbrunner","$TYPE":"DIR","$BYTES":0},
  {"$KEY":"NOTICE","$TYPE":"FILE","$BYTES":577},
  {"$KEY":"repo.prop","$TYPE":"FILE","$BYTES":4465}
]
```

## mk(name [,type]) 
Creates a directory at the current working directory within the file system.

type is one of:
- "", if in the file system will default to "DIR"
- "DIR", if in the file system, a new directory will be created. If in the database, will use "GROUP"
- "GROUP", create a database of GROUP type
- "ROW", create a ROW store database
- "COL", create a COL store database

```
> f.mk("test")
> f.cd("test")
> f.ls()
[]

> f.mk("testg","GROUP")
> f.cd("testg")
> f.ls()
[]
```

## rm(name)
Removes a directory or file.
```
> f.rm("test")
```

## set(name, value [, field])
Updates the column in a row. By default the $VALUE column is updated. But an alternate column can be specified.

field defaults to $VALUE.

```
> f.set("test","value of test")
```

## get(name [, field])
Gets the column value in a row. By default the $VALUE column is retrieved. But an alternate column can be specified.

field defaults to $VALUE.

```
> f.set("test","value of test")
> f.get("test")
value of test
```

## mkfield(name [,fieldType[, storeType[, storeSize[, storeGrow]]]])
Creates a field within the current working directory.

```
> f.mkfield("test")
```

Default for all fields is fieldType=STR and storeType=VAR. 

fieldType | Description
------------ | -------------
BOOL | Fixed size for $BOOL
TIME | Stores an $INT. Size depeds on storeType and storeSize
INT | Stores an $INT. Size depeds on storeType and storeSize
FLOAT | Stores a $FLOAT. Size depends on storeType and storeSize
STR | Stores a $STR. Size depends on storeType and storeSize
TABLE | Stores a $TABE. Size depends on storeType and storeSize
RAW | Stores a $RAW. Size depends on storeType and storeSize

storeType | Description
------------ | -------------
FIX | Fixed field size, set storeSize and storeGrow. Data embedded in row/col.
VAR | Variable field size. Uses an extra reference for the data. Reference stored in row/col.
PAR | Partitioned field. Best for very large data requiring updates to portions of the data. Also used for COL store $TABLE types.

storeGrow is used by variable fields to determine how much to grow the field by when extra space is needed.

## rmfield(name)
Deletes a field within the current working directory.

## debug()
Used for debugging the database during development. Displays the BTree structure of the data dictionary and fields and indexes for the current working directory when in a database (either in memory or on the file system).

# $net
Provides a socket library, cross functional with all platforms supported. 

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...ensure thread safe by using lock/unlock (any variable can be used for lock/unlock).

## type()
Returns $net.

## mac()
```
> $net().mac()
{"name":"Ethernet","ip":"104.0.97.19","mac":FB05B43EFC09ABC8}
```

## interfaces()

```
> $net().interfaces()
{
  {"name":"Ethernet","mac":"FB05B43EFC09ABC8","family":"IPV6","address":"xxx:xxx:xxx:xxx::xxx"},      
  {"name":"Ethernet","mac":"FB05B43EFC09ABC8","family":"IPV6","address":"xxx:xxx:xxxx:xxx:xxxx:xxxx:xxxx:xxx"}
  {"name":"Ethernet","mac":"FB05B43EFC09ABC8","family":"IPV4","address":"104.0.97.19"},
}
```

## connect(url)

```
n2 = $net();
n2.connect('localhost:12345');
```
## bind(url)
Using bind/listen requires setting up at least 2 net objects. One for bind and the other for listen. See listen().
```
n1 = $net();
n1.bind(':12345');
```

## listen(net)
Pass in a bound net object. Listen is blocking, so a $thread may be needed to manage - or separate threads if more than one listener.

For those familiar with using sockets in C++, the $net class is a representation of sockets, and use bind/listen in the same way.

```
n1 = $net();
n1.bind(':12345');
n2 = $net();
n2.listen('@n1);
```

## onlisten(url,messageHandler [,connectHandler [,count:1]])

Accomplishes the same as bind/listen, but handled in a background thread and much easier to setup.

When a connection is initiated, a new thread and new network object is created, and that new network object binds to the connect. The connectHandler is then called to provide a way to initialize data structures, and than the messageHandler is called for incoming data. The connectHandler receives 1 parameter - an updateable variable. The messageHandler recieves 2 parameters - the message and a hasmore flag. If the hasmore is 0, the data can be processed. If the message length is zero, the remote connection terminated and messageHandler should cleanup as the thread will be closing. 

The count defines the number of listeners - but fixed to 1 for now. Once a connection is established, the listener automatically restarts. For reasonable workloads, the 1 should be sufficient. For now if more is needed, use Bind/Listen - this though with require the use of $thread. 

The following sets up a simple web service. Use postman to post messages to verify.

```
processPost = op(in)
{
	{processed:in};
}

postHandler = op(in) 
{
	$local.data = in.split("\r").join("");
	$local.len = data.len() - data.split("\n\n")[0].len() - 2;
	if (len<0) len=0;
	$local.body = data.right(len);
	$local.rstr = processPost(body).str();
	"HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: "+rstr.len().str()+"\r\n\r\n"+rstr;
};

postConnectHandler = op(netSession)
{
	netSession.data = "";
};

postMessageHandler = op(netSession,message,hasmore)
{
	netSession.data += message;
	if (hasmore==0)
	{
		netSession.send(postHandler(netSession.data));
		netSession.data = "";
	};
};

n=$net();
n.onlisten(':12345',postMessageHandler,postConnectHandler);
```

To very, try the following.
```
n2 = $net();
err = n2.connect('localhost:12345');
err = n2.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{try:55}');
n2.nreceive();
n2.receive();
n2.disconnect();
```

## disconnect()
Disconnects the session. Disconnecting a listener will disconnect all sessions the listener is a parent of.

## certificate([file])
Sets $net instance to SSL mode with certificate file.

Use certificate() to revert back to non-SSL.

See SSL_CTX_use_certificate_chain_file.

## private(file [,passOp [,param]])
Server in SSL mode requires a private key file. 

If the private key file is password protected, also provide an $OP for the SSL routines to request the password. If a param is added, the callback will include the param. 

See SSL_CTX_use_PrivateKey_file.

## trusted(file, path)

See SSL_CTX_load_verify_locations.

## verify()

Call after connecting with SSL.

Returns a list:
	result
	certificate

See the following for result codes:
https://www.openssl.org/docs/man1.0.2/man1/verify.html

Use decode("PEM") to decode the certificate.
```
> x = n2.verify();
> x.certificate.decode("PEM");
```

## chain()

Certificate chain.

Use decode(PEM) to decode.
```
> x = n2.chain();
> x[0].decode("PEM");
```

## host()

After running the sample in onlisten, try the following.
```
n.host();
{"url":":12345","host":"computer name","ip":"XX.XX.XX.XX","port":12345,"family":2,"connected":0,"bound":1}
```

## send(message)
See example in onlisten.

NOTE: If using SSL and the remote system breaks the connection, first send will return no error. The second send will return an error. This is not the case for read - where the first read after a broken connection will return an error and close the connection.

## receive()
Blocks until data is received. Use nreceive() first to verify data exists. Or use onreceive.

See example in onlisten.

## pending()
Number of bytes that can be received.

If an SSL connection, the byte count will be what is pending in the SSL encrypted buffer, which is different than the unencrypted data. 

See example in onlisten.

## onreceive(handler)
Handler will be called when data is received.

```
receiveHandler = op(netSession,message,hasmore)
{
	netSession.data += message;
	if (hasmore==0)
	{
		(netSession.data+"\n").echo();
		(@<prompt>)();
		netSession.data = "";
	};
};

n2 = $net();
n2.connect('localhost:12345');
n2.onreceive(receiveHandler);
n2.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{try:55}');
n2.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{try:400}');
n2.disconnect();
```

# $thread
Provides a thread library, cross functional with all platforms supported.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...ensure thread safe by using lock/unlock (any variable can be used for lock/unlock).

## type()
$thread

## trylock()
Attempts to lock.

## lock()
Applies a lock.

## unlock()
Releases the lock.

## wait()
Current thread waits for signal.

## signal()
Signals the wait to continue.

## waiting()
Indicates in waiting state.

## start(runOp, input, doneOp)
Starts the runOp in the background, passing paramList. When the thread exists, doneOp is called. All 3 inputs are copied,  as the originals are likely to go away after running the start command. So if an instance of object is passed in, the thread will end up using a copied instance and not the original instance.

The thread is run from the same namespace as where it is called. To use a shared object instance, access the variable from within the thread rather than passing in the variable. Or pass in the $ID for the variable and difference the variable from the thread (which essentially does the same thing as a variable lookup but allows a different variable name to be used). 

If accessing shared resources from within a thread, take care and use thread save logic, such as lock/unlock. 

The following is an example of creating a thread.

```
myRun = op(input) {"myRun:".echo();$sys().echo(@$local); input.c = input.a+input.b; "\n".echo(); @$local;};
myDone = op(input,result) {"myDone:".echo();$sys().echo(@$local); "\n".echo();};
t = $thread();
t.start(myRun,{a:1,b:2},myDone);
```

Output for above
```
myRun:{"input":{"a":1,"b":2}}
myDone:{"input":{"a":1,"b":2,"c":3},"result":{"input":{"a":1,"b":2,"c":3}}}
```

The input parameter is passed to both the run op and done op. The done op also receives any output from the run op.


## stop()
Stops the thread.

## started()
Indicates the running state of the thread.

## suspend()
Suspends the thread. If the thread is processing a queue and the queue is empty, put the thread in suspend mode. Than after pushing data onto the queue, call resume to have the thread resume processing.

## resume()
See suspend.

## suspended()
Indicates whether the thread is in a suspended state.
