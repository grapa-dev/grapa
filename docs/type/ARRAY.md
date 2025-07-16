# $ARRAY
References:
- [$ARRAY object](../../lib/grapa/$ARRAY.grc)
- [$LIST object](../../lib/grapa/$LIST.grc)
- [$VECTOR object](../../lib/grapa/$VECTOR.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

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

> **Parallelism Note:**
> Array operations like `.map()` and `.filter()` are parallel by default and hardened for ETL/data processing workloads.
