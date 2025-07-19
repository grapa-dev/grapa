# Compare Operators
## `||`
If either of two conditions are true.

```
((1==1) && (2==2)) -> true
((1==1) && (2==3)) -> true
((1==2) && (2==3)) -> false
```

## `&&`
Two conditions must be true.

```
((1==1) && (2==2)) -> true
((1==1) && (2==3)) -> false
```

## `==`
Equal to.

## '<=>'
Returns -1 if a<b, 0 if a=b, and 1 if a>b.

```
> "a"<=>"b"
-1
```

## `!=`
Not equal to.

##  `<=`
Less than or equal to.

##  `>=`
Greater than or equal to.

## `>`
Greater than.

## `<`
Less than

# Modifier Operators
## `-`
Negate.

-(4+2) -> -6

## `!`
Not.

!true -> false

## `~`
Bitwise not.

(~0x6C).hex() -> FFFFFF93
