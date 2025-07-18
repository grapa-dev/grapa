# Condition Operators

## if

Syntax options:
* if (bool) statement;
* if (bool) statement; else statement;
* if (bool) statement; elseif statement; else statement;

Example:
```
> if (1==0) "1==0\n".echo();  else "none\n".echo();
none

> if (1==0) "1==0\n".echo(); elseif (2==2) "2==2\n".echo(); else "none\n".echo();
2==2
```

## switch

Syntax options:
* swtich (item) {case item1: statement; item2 statement; etc...;};
* swtich (item) {case item1: statement; item2 statement; etc...; default: statement;};

The following is a way to use a switch in place of if/ifelse/else sequence. The switch will compare until if arrives at a true.

```
> switch(true){case (1==0):"1==0\n".echo(); case (2==2): "2==2\n".echo(); default: "none\n".echo();};
2==2
```

## `?`

Syntax options:
* bool ? statement for true;
* bool ? statement for true : statement for false;
* bool ? : statement for false;
* (<0)|(0)|(>0) ? statement for -1 : statement for 0 : statement for 1;

Example:
```
> 1?hi:by
hi

> 0?hi:by
by

> -55?hi:by:there
hi

> ("a"<=>"b")?a:e:b
a
```
