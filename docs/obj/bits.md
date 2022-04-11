# Part of $OBJ

## bits()
Gets the bit count of the item.

```
"hi".bits() -> 16
```

## bytes()
Gets the byte count of the item.

```
"hi".bytes() -> 2
$TIME().utc().raw().bytes() -> 7
```

## setbit(bitpos)
Sets a bit, from low order.

```
(0b100110).setbit(3).bin() -> 101110
```

## clearbit(bitpos)
Clears a bit, from low order. 

```
(0b100110).clearbit(2).bin() -> 100010
```

## genbits
Generates sequence of bits.

```
(4).genbits() -> 15
(4).genbits().bin() -> 1111
```

