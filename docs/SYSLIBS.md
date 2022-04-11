# $OBJ
Several classes inherit $OBJ, such as $STR and $INT and $LIST. Functions that can be used across the different data types are placed in the $OBJ class. Some of these functions may move to other classes. The $OBJ class is a general place to place functions as a starting point. For example, the setfloat and setfix functions may move to the $sys class.

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
0x0FF

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

## encode (method, [,options])

encode/decode types:
Method | Options | Description
------------ | ------------- | -------------
"RSA" | {n,e,d [,p,q,dp,dq,qi]} | Data size must be exactly byte count of d  minus 32.
"AES256" | {key [,iv]} | key is 32 bits and iv is 16 bits
"ZIP-GRAPA" | |
"BASE64" | |
"ESCAPE" | |
"FLOAT" | |
"JSON" | | Only encode.
"XML" | | Only encode.
"SHAKE256" | | Only encode.
"SHAKE128" | | Only encode.
SHA3-256" | | Only encode.
SHA3-384" | | Only encode.
"SHA3-512" | | Only encode.
"PEM" | | Only decode.

Example of RSA using hard coded RSA key.

```
e = 65537;
d = (0xA4954A19EA13D5FF06C86E7719AB6B6D5F37EF2D0A453713F08CD3FF2847C7BE80F2A3D0159C2EA862997DEE93DC08CD18CDFBD96DCC077B978904D05B2FCA5988A63).uraw();
d = (d + (0x2CEB3B7286ACEF97682F418172B30509BD29A1C56E0244D79CFADD5A60C83D2F28A8B72643CB82AABA08871E094B36E6613B108166768B33F5C4860BD21).uraw()).uint();
n = (0xB47D8ABBD7A33B5AD29C8EFD291433D62695BA043F4130F8E677CD179315FD0DD39989539EC710EB3BCBCACBDED2230DE8575FAEE3ED2835B256ADB1AAEF2F34C430E).uraw();
n = (n + (0xAB9934FAF1977FA83E12B947806CE1F8D06692F02068FD18E4DE952895DDD2D9BDEE923E94257E84B63D21C92EC1F74B8AF4AF9A33608C7E8247F1C820F).uraw()).uint();
g = {"method":"RSA","n":n,"e":e, "d":d};
len = g.n.bytes()-42;
v = "this is a test of this is a test of 86 chars to see if we can encode with RSA. It needs to be exact size chars to see if we can encode with RSA. It needs to be exact size...so need to make it so.".left(len);
v.encode(g).decode(g).str();
```

```
v.encode("AES256",{key:"12345678123456781234567812345678",iv:"1234567812345678"}).decode("AES256",{key:"12345678123456781234567812345678",iv:"1234567812345678"}).str();
v.encode("SHA3-256");
v.encode("SHAKE128");
{1,2,3}.encode("ZIP-GRAPA").decode("ZIP-GRAPA");
v.encode("BASE64").decode("BASE64").str();
```

## decode (method [,options])
See encode.

## sign (method, [,options])

```
curve = "prime256v1";
pub = 0x25074E1882EF3F6230925ADFFEA56EC322F7EB4F23C3DECF12BFF5A24E11BA21C;
prv = 0x5A44899AE665F34833146FD9805222B9E7C22B8BE94B7F9C34A54AA9B9D89890;
g = {"method":"EC","curve":curve,"pub":pub,"prv":prv};
"test".sign(g).verify(g,"test");
```

## signadd (method, value, [,options])
Not implemented yet. Used for pairwise eliptic curves where sigatures can be added.

## verify (method, value, [,options])
See sign.

## verifyrecover (method, [,options])
Recovers signed data.

```
a = "RSA".genkeys();
b = "hello".sign(a);
b.verifyrecover(a).str();
```

## secret (method)
Diffie-Hellman key exchange. Node A generates the staring keys and sends "p" and "g" to node B. Node B then generates its keys using "p" and "g" from node A. Both nodes can then generate the shared secret, and they will be equal.  

```
a = "dh".genkeys()
b = "dh".genkeys({p:a.p,g:a.g})
a.secret(b)==b.secret(a)
```

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
