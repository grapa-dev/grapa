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

## time()
Converts an $INT to $TIME.
See $TIME.

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
