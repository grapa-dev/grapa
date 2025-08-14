# $LIST
References:


A $LIST is basically JSON, but extended to include all the various grapa data types. 

Action | Example | Result
------------ | ------------- | -------------
Create | {a:1, b:2, c:3} | {"a":1,"b":2,"c":3}
Access | {a:1, b:2, c:3}.a</br>{a:1, b:2, c:3}[1]</br>{a:1, b:2, c:3}[-1] | 1</br>1</br>3
Assign | x = {a:1, b:2, c:3};</br>x.b = "x";</br>x["b"] = "by";</br>x[-2] = 1234; | </br></br>{"a":1,"b":"x","c":3}</br>{"a":1,"b":"by","c":3}</br>{"a":1,"b":1234,"c":3}
Append | x = {a:1, b:2};</br>x += (c:3);</br>x; | </br></br>{"a":1, "b":2, "c":3}
Append | x = {a:1, b:2};</br>x ++= {c:3,d:4};</br>x; | </br></br>{"a":1, "b":2, "c":3, "d":4}
Insert | x = {a:1, b:2};</br>x += (c:3) x[0];</br>x; | </br></br>{"c":3,"a":1,"b":2}
Count | {a:1, b:2, c:3}.len() | 3
Remove | x = {a:1, b:2, c:3};</br>x -= x[1];</br>x; | </br></br>{"a":1, "c":3}

### Advanced List Operations

#### Assignment Operations (`=`)
```grapa
/* Direct property assignment */
list = {a:1, b:2, c:3};
list.b = "x";             /* {"a":1,"b":"x","c":3} */
list["b"] = "by";         /* {"a":1,"b":"by","c":3} */

/* Assignment by index */
list[1] = 55;             /* {"a":1,"b":55,"c":3} */

/* Assignment by negative index */
list[-2] = 1234;          /* {"a":1,"b":1234,"c":3} */

/* Compound assignment on accessed elements */
list.b += "dee";          /* {"a":1,"b":"bydee","c":3} */
list[0] += 8;             /* {"a":9,"b":"bydee","c":3} */
```

#### Addition Operations (`+=`)
```grapa
/* Add single key-value pair */
list = {a:1, b:2};
list += (c:3);           /* {"a":1,"b":2,"c":3} */

/* Add multiple key-value pairs */
list += {d:4, e:5};      /* {"a":1,"b":2,"c":3,"d":4,"e":5} */

/* Insert at specific position */
list += (f:6) list[0];   /* {"f":6,"a":1,"b":2,"c":3,"d":4,"e":5} */
```

#### Concatenation Operations (`++=`)
```grapa
/* Concatenate two lists */
list1 = {a:1, b:2};
list2 = {c:3, d:4};
list1 ++= list2;         /* {"a":1,"b":2,"c":3,"d":4} */
```

#### Removal Operations (`-=`)
```grapa
list = {a:1, b:2, c:3, d:4};

/* Remove by key reference */
list -= list.c;          /* {"a":1,"b":2,"d":4} */

/* Remove by key string */
list -= list["b"];       /* {"a":1,"d":4} */

/* Remove by positive index */
list -= list[0];         /* {"d":4} */

/* Remove by negative index */
list -= list[-1];        /* {} */
```

#### Unsupported Operations
```grapa
list = {a:1, b:2, c:3};

/* These do NOT work: */
list -= 2;               /* No effect - value-based removal not supported */
list -= "b";             /* No effect - direct string removal not supported */
list -= list.b;          /* Error - trying to remove value, not key */
```
