# Part of $OBJ


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

