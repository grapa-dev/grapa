# $FLOAT
References:
- [GrapaFloat.cpp source](../../source/grapa/GrapaFloat.cpp)
- [$FLOAT object](../../lib/grapa/$FLOAT.grc)
- [$math object](../../lib/grapa/$math.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

Supports both fix and float format. Fix will apply the precision to just the decimal. Float will not restrict to the decimal, supporting large exponents with a specified precision. Each number maintains it's own precision, which is used in math operations with numbers of other precision. Some math operations may convert an input from one format to another. For example, passing in a float to a trig function will produce a fix format result. Floats also support specifying "extra" bits to apply to the calculations to reduce error propagation (the default is 7 bits). 

The $FLOAT class inherits $INT class - see the the $INT class for additional functions supported.

Example:
```
> 5.13
5.13
```

You can also create using hex format:
```
> -0x4.5
-4.3125
```

Or binaary format:
```
> 0b101.11
5.75
```

By default, all floats are "float" format. To change formats use the fix() and float() routines, where both the precision and the "extra" bits can also be specified. The parsing engine will set the precision to the system default, which is 128 bits. There are two ways to change this. First, change the system default using the setfloat and setfix routines. Second, pass in a $STR to the fix or float routines. For example:

```
>"3.5".float()/0.511
6.849315068493150684931506849315

>"3.5".float(300,6)/0.511
6.849315068493150684931506849315068493150684931
```

To verify the float is being created properly, use the decode routine with FLOAT as the parameter. For example:

```
> "30.75".float(300,6).decode("FLOAT")
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"extra":6,"data":123}
> (30.75).decode("FLOAT").data
123

> (30.75).decode("FLOAT").data.hex()
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
0x00048100077B
```

Bit shifts:
```
> (30.75) >> 4
1.921875

> (30.75) << 4
492.0
```
