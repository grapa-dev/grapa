# $RAW
A $RAW represents raw bytes. Most data types can be converted to and from $RAW, providing the ability to make specific tweaks to data. For example, this is how time addition/subtraction is performed - by converting the $TIME into $RAW and then into an $INT, and than back to a $TIME. There are several examples of using raw in the documentation for the other data types.

When displayed, the value is printed in hex form, but the value in memory is in raw. If you convert to hex(), the result will be a text version of the hex of the raw data.

```
> "hi".raw();
0x6869

> "hi".raw().int();
26729

> (0x6869).raw().str();
hi
```
