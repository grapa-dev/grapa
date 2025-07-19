# $ERR
References:
- [$ERR object](../lib/grapa/$ERR.grc)
- [$OBJ object](../lib/grapa/$OBJ.grc)

If an operation results in an error, the $ERR data type is returned. Check using the type function: if (@result.type()==$ERR) something;

Under the hood, the $ERR type is a $LIST, and the same commands can be used to get more details on the error.

```
> (1/0).type()==$ERR
true
```

# Part of $OBJ

## iferr(new)
If value is $ERR type, the new is used. Otherwise the value is used.

```
(10/2).iferr(55) -> 5
(10/0).iferr(55) -> 55
```
