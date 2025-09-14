# Iteration Object Methods

**Note:** This page provides a quick reference for iteration methods available on objects. For comprehensive documentation with examples, troubleshooting, and best practices, see [Functional Array Methods](../operators/loop.md).

/*
Thread Safety:
Grapa is fully thread safe in all supported environments (command line, Grapa shell, and Python/GrapaPy). All built-in operations—including map, filter, reduce, $thread, and $net—are safe to use concurrently. Users do not need to take any special precautions for thread safety in these environments.

*Note: Only if Grapa is integrated directly into a non-thread-safe C++ host (not anticipated for normal users) would additional thread safety considerations arise.*
*/

## map($OP [,params])
See [map in the Looping section](../operators/loop.md#map) for full documentation, canonical examples, and troubleshooting.
Iterates through a $GOBJ/$ARRAY calling an $OP. Processes each item in parallel. Result of $OP placed in a $GOBJ/$ARRAY which is returned at the completion.

## filter($OP [,params])
See [filter in the Looping section](../operators/loop.md#filter) for full documentation, canonical examples, and troubleshooting.
Iterates through a $GOBJ/$ARRAY calling an $OP. Processes each item in parallel. If $OP of op is true, item is placed in a $GOBJ/$ARRAY which is returned at the completion. 

## reduce($OP [,start [,params]])
See [reduce in the Looping section](../operators/loop.md#reduce) for full documentation, canonical examples, and troubleshooting.
Iterates through a $GOBJ/$ARRAY calling an $OP. Processes each item in sequence as the intent is to combine results of each $OP. If "start" not provided, the first item of the list is used as the start. 

## sort([axis],[order],[op])
Sorts data structures across multiple types: `$ARRAY`, `$TUPLE`, `$GOBJ`, `$OBJ`, `$XML`, `$TAG`, `$EL`, `$OP`, `$CODE`.

- axis -> 0 = row, 1 = col
- order -> 0 = ascend, 1 = decend, or does selection if array if indexes passed in
- op -> function for the compare

**Examples:**
```
/* Lists and Objects - sort by key */
{z:1,m:2,p:3,b:4}.sort() -> {"b":4,"m":2,"p":3,"z":1}

/* Arrays and Tuples - sort by value */
[3,1,4,1,5,9].sort() -> [1,1,3,4,5,9]
(3,1,4,1,5,9).sort() -> (1,1,3,4,5,9)
```

Optionally pass in a compare routine.'''
```
> ["b","a","B","c","b","A"].sort()
["A","B","a","b","b","c"]

> ["b","a","B","c","b","A"].sort(0,0,op(a,b){a.upper()<=>b.upper();})
["a","A","B","b","b","c"]

> ["b","a","B","c","b","A"].sort(0,0,op(a,b){a.casefold()<=>b.casefold();})
["a","A","B","b","b","c"]

> ["b","a","B","c","b","A"].sort(0,0,op(a,b){$local.c=@a.upper()<=>@b.upper();if(@c==0)c=@a<=>@b;@c;})
[A,a,B,b,b,c]

> ["b","a","B","c","b","A"].sort(0,[1,2])
["a","B"]
```

## unique([op])
Remove duplicates across multiple types: `$ARRAY`, `$TUPLE`, `$GOBJ`, `$OBJ`, `$XML`, `$TAG`, `$EL`, `$OP`, `$CODE`.

**For Lists and Objects:** Removes duplicate keys, keeping the last value for each key.
**For Arrays and Tuples:** Removes duplicate values, keeping the last occurrence of each value.

```
/* Lists and Objects - remove duplicate keys */
{z:1,b:4,m:2,p:3,m:2,b:4}.unique() -> {"z":1,"b":4,"m":2,"p":3}

/* Arrays and Tuples - remove duplicate values */
[1,2,1,3,2,4].unique() -> [1,2,3,4]
(1,2,1,3,2,4).unique() -> (1,2,3,4)
```

Optionally pass in a compare routine.'''
```
> ["b","a","B","c","b","A"].unique()
["A","B","a","b","c"]

> ["b","a","B","c","b","A"].unique(op(a,b){a.upper()<=>b.upper();})
["A","b","c"]

> ["b","a","B","c","b","A"].unique(op(a,b){a.casefold()<=>b.casefold();})
["A","b","c"]
```

## getname([index])
Used in a map/reduce/filter to identify the name of the passed in item from the original list (if processing a $GOBJ). 

To use, do not de-reference the identify. The getname function will then see that it is an $ID and will attempt to locate it in the namespace. Since what is passed in is a pointer, the function is able to discover the item in the original list and discover the name.

```
> {a:1,b:2}.map(op(a){"a".getname()})
{"a":a,"b":b}
```

Or used to get the name of an item in a $GOBJ, using the index parameter.

```
> {a:1,b:2}.getname(0)
a
```

