# $ERR
References:
- [Error Handling](syntax/basic_syntax.md#error-handling)
- [Object Methods](type/obj_methods.md#error-handling-with-iferr)

If an operation results in an error, the $ERR data type is returned. Check using the type function: `if (result.type() == $ERR) something;`

Under the hood, the $ERR type is a $LIST, and the same commands can be used to get more details on the error.

## Creating $ERR Types

There are three ways to create `$ERR` types:

### 1. **Direct Construction**
```grapa
x = $ERR();  /* Creates empty error object */
x.type();    /* $ERR */
x;           /* {} */
```

### 2. **Runtime Errors**
```grapa
x = 9 / 0;   /* Division by zero creates error */
x.type();    /* $ERR */
x;           /* {"error":-1} */
```

### 3. **Type Conversion with .err()**
```grapa
z = {"error": 43, "info": [3, 4]}.err();  /* Convert $LIST to $ERR */
z.type();    /* $ERR */
z;           /* {"error": 43, "info": [3, 4]} */
```

## Error Object Manipulation

Error objects can be modified like any other object:

```grapa
x = $ERR();
x ++= {error: 55, comment: 'test error'};
x;  /* {"error": 55, "comment": "test error"} */
x.type();  /* $ERR */
```

## iferr(new)
If value is $ERR type, the new is used. Otherwise the value is used.

```grapa
(10/2).iferr(55)  /* -> 5 */
(10/0).iferr(55)  /* -> 55 */
```

## Error Handling Patterns

```grapa
/* Check for errors */
result = some_operation();
if (result.type() == $ERR) {
    /* Handle error */
    result.error.echo();  /* Print error code */
} else {
    /* Use result */
}

/* Use iferr for simple fallback */
result = some_operation().iferr(default_value);
```
