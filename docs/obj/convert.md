# Part of $OBJ

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

## bool()
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
