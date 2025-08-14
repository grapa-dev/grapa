---
tags:
  - user
  - highlevel
---
# $ARRAY
References:
- [$LIST object](list.md)
- [$VECTOR object](vector.md)

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

### Array Operations

#### Assignment Operations (`=`)
```grapa
/* Direct element assignment */
arr = [3, 6, "hi"];
arr[1] = "by";            /* [3,"by","hi"] */

/* Assignment by negative index */
arr[-2] = "good";         /* [3,"good","hi"] */

/* Compound assignment on accessed elements */
arr[-2] += "dee";         /* [3,"gooddee","hi"] */
arr[0] += 8;              /* [11,"gooddee","hi"] */
```

#### Addition Operations (`+=`)
```grapa
/* Add single element */
arr = [1, 2, 3, 4, 5];
arr += 6;                 /* [1,2,3,4,5,6] */

/* Add multiple elements */
arr += [7, 8, 9];         /* [1,2,3,4,5,6,7,8,9] */

/* Insert at specific position */
arr += 10 arr[0];         /* [10,1,2,3,4,5,6,7,8,9] */
arr += 20 arr[2];         /* [10,1,20,2,3,4,5,6,7,8,9] */
```

#### Concatenation Operations (`++=`)
```grapa
/* Concatenate two arrays */
arr1 = [1, 2, 3];
arr2 = [4, 5, 6];
arr1 ++= arr2;            /* [1,2,3,4,5,6] */
```

#### Removal Operations (`-=`)
```grapa
arr = [1, 2, 3, 4, 5];

/* Remove by index */
arr -= arr[2];            /* [1,2,4,5] */

/* Remove by negative index */
arr -= arr[-1];           /* [1,2,4] */

/* Remove first element */
arr -= arr[0];            /* [2,4] */
```

#### Unsupported Operations
```grapa
arr = [1, 2, 3, 4, 5];

/* These do NOT work: */
arr -= 3;                 /* No effect - value-based removal not supported */
arr -= "2";               /* No effect - direct string removal not supported */
```

> **Parallelism Note:**
> Array operations like `.map()` and `.filter()` are parallel by default and hardened for ETL/data processing workloads.
