# $OBJ
Several classes inherit $OBJ, such as $STR and $INT and $LIST. Functions that can be used across the different data types are placed in the $OBJ class. Some of these functions may move to other classes. The $OBJ class is a general place to place functions as a starting point. For example, the setfloat and setfix functions may move to the $sys class.

## type()
All objects will return the type.

```
"test".type() will return $STR, for example.
```

If the item does not support type(), use $sys().type(value). For example, $sys().type("test") returns $STR.


## describe (object)
This is stubbed out...but not yet implemented. Type will provide just the type. Describe will provide more detail. If using describe on a table, details about the table will be returned, for example. Intention is to provide describe everywhere that type is available.


## isint
Checks if a string is an $INT.

```
"z55".isint() -> false
"55".isint() -> true
```

