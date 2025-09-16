# $py() - Python Integration System Object

## Overview

The `$py()` system object provides bidirectional communication between Grapa and Python, allowing Grapa code to call back into Python functions. This object is **only available in the Python extension** (`grapapy`), not in the standalone CLI.

## Availability

- ✅ **Python Extension** (`grapapy`): Fully supported
- ❌ **Standalone CLI**: Not available

## Methods

### `.eval(script, locals={}, import='', attr='')`

Executes Python code and returns the result converted to a Grapa object.

**Parameters:**
- `script` (`$STR`): Python code to execute
- `locals` (`$GOBJ`, optional): Dictionary of local variables for the Python execution context
- `import` (`$STR`, optional): Python module to import (default: `"__main__"`)
- `attr` (`$STR`, optional): Attribute to access from the imported module (default: `"__dict__"`)

**Returns:** Grapa object (converted from Python result)

**Example:**
```grapa
/* Basic Python evaluation */
result = $py().eval("2 + 3 * 4");  /* Returns: 14 */

/* With local variables - NEW WAY (correct) */
locals = {"x": 10, "y": 5};
result = $py().eval("x + y", locals);  /* Returns: 15 */

/* When accessing parameters in Grapa scripts called from Python */
result = $py().eval("x + y", {"x": $local.'x', "y": $local.'y'});  /* Returns: 15 */

/* Call Python function */
result = $py().eval("abs(-42)");  /* Returns: 42 */
```

### `.exec(script, locals={}, import='', attr='')`

Executes Python code without returning a value (for side effects).

**Parameters:**
- `script` (`$STR`): Python code to execute
- `locals` (`$GOBJ`, optional): Dictionary of local variables
- `import` (`$STR`, optional): Python module to import (default: `"__main__"`)
- `attr` (`$STR`, optional): Attribute to access from the imported module (default: `"__dict__"`)

**Returns:** `null` (no return value)

**Example:**
```grapa
/* Execute Python code for side effects */
$py().exec("print('Hello from Python!')");

/* Define Python variables */
locals = {"counter": 0};
$py().exec("counter += 1", locals);
```

## Type Conversion

The `$py()` object automatically handles bidirectional type conversion:

### Grapa → Python

| Grapa Type | Python Type | Notes |
|------------|-------------|-------|
| `$INT` | `int` | Direct conversion |
| `$FLOAT` | `float` | Direct conversion |
| `$STR` | `str` | UTF-8 string |
| `$BOOL` | `bool` | True/false values |
| `$ARRAY` | `list` | Element-by-element conversion |
| `$GOBJ` | `dict` | Key-value pairs |
| `$TUPLE` | `tuple` | Element-by-element conversion |
| `$RAW` | `bytes` | Binary data |
| `null` | `None` | Null values |

### Python → Grapa

| Python Type | Grapa Type | Notes |
|-------------|------------|-------|
| `int` | `$INT` | Direct conversion |
| `float` | `$FLOAT` | Direct conversion |
| `str` | `$STR` | UTF-8 string |
| `bool` | `$BOOL` | True/false values |
| `list` | `$ARRAY` | Element-by-element conversion |
| `dict` | `$GOBJ` | Key-value pairs |
| `tuple` | `$TUPLE` | Element-by-element conversion |
| `bytes` | `$RAW` | Binary data |
| `None` | `null` | Null values |

## Parameter Access Changes

**Important**: When Grapa scripts are called from Python (via `g.eval()`), parameters are now accessed as **string labels** rather than ID labels. This means you must use quoted syntax:

```grapa
/* OLD WAY (broken in Python-called scripts) */
$this.multiply = op(x=0, y=0) {
    $py().eval("x * y", {"x": x, "y": y});
};

/* NEW WAY (correct for Python-called scripts) */
$this.multiply = op(x=0, y=0) {
    $py().eval("x * y", {"x": $local.'x', "y": $local.'y'});
};
```

**Key Points:**
- Use `$local.'param_name'` instead of `param_name` when accessing parameters
- This applies to scripts called from Python via `g.eval()`
- Scripts run directly in Grapa CLI still use the old syntax
- This change aligns with the broader `$GOBJ` string label vs ID label distinction

## Common Usage Patterns

### 1. Simple Function Calls

```grapa
/* Call Python built-in functions */
result = $py().eval("len('hello')");  /* Returns: 5 */
result = $py().eval("max([1, 5, 3, 9, 2])");  /* Returns: 9 */
result = $py().eval("sorted([3, 1, 4, 1, 5])");  /* Returns: [1, 1, 3, 4, 5] */
```

### 2. Custom Python Functions

```grapa
/* Define a Grapa function that calls Python */
$this.process_data = op(data, multiplier=2) {
    $local.locals = {"data": data, "mult": multiplier};
    $py().eval("data * mult", $local.locals);
};

/* Use the function */
result = process_data(5, 3);  /* Returns: 15 */
result = process_data([1, 2, 3], 2);  /* Returns: [2, 4, 6] */
```

### 3. Complex Data Processing

```grapa
/* Process complex data structures */
data = {"users": [{"name": "Alice", "age": 30}, {"name": "Bob", "age": 25}]};
locals = {"data": data};

/* Python code to filter users by age */
script = "[user for user in data['users'] if user['age'] >= 30]";
result = $py().eval(script, locals);  /* Returns: [{"name": "Alice", "age": 30}] */
```

### 4. Error Handling

```grapa
/* Safe Python execution with error handling */
$this.safe_python = op(script, locals={}) {
    $local.result = $py().eval(script, locals);
    if ($local.result.type() == $ERR) {
        "Python error occurred".echo();
        null;
    } else {
        $local.result;
    };
};

/* Test error handling */
result = safe_python("1 / 0");  /* Returns: null, prints error message */
result = safe_python("2 + 2");  /* Returns: 4 */
```

## Advanced Examples

### Rule-Based Python Integration

```grapa
/* Create a rule that uses Python for different operations */
$this.python_rule = rule 
      $INT $INT {op(a:$1,b:$2) {$py().eval("a + b", {"a":a,"b":b});}}
    | $STR      {op(s:$1)      {$py().eval("s.upper()", {"s":s});}}
    | $ARRAY    {op(arr:$1)    {$py().eval("sum(arr)", {"arr":arr});}}
    ;

/* Use the rule */
result = $sys().eval("5 3", {}, "python_rule");     /* Returns: 8 */
result = $sys().eval("hello", {}, "python_rule");   /* Returns: "HELLO" */
result = $sys().eval("[1,2,3,4]", {}, "python_rule"); /* Returns: 10 */
```

### Compiled Code with Python

```grapa
/* Compile Grapa code that uses Python */
compiled = $sys().compile("$py().eval('x * 2', {'x': input})", "", "");

/* Use compiled code multiple times */
result1 = $sys().eval(compiled, {"input": 5});   /* Returns: 10 */
result2 = $sys().eval(compiled, {"input": 7});   /* Returns: 14 */
```

## Performance Considerations

1. **Overhead**: Python callbacks have overhead due to type conversion and GIL management
2. **Use Cases**: Best for complex operations that benefit from Python's ecosystem
3. **Caching**: Consider caching Python function results when possible
4. **Batch Operations**: Group multiple Python operations when possible

## Error Handling

Python exceptions are automatically converted to Grapa error objects:

```grapa
/* Python exceptions become Grapa errors */
result = $py().eval("1 / 0");  /* Returns: $ERR object */
if (result.type() == $ERR) {
    "Python error occurred".echo();
}
```

## Thread Safety

The `$py()` object automatically manages Python's Global Interpreter Lock (GIL), ensuring thread-safe execution even in multi-threaded Grapa applications.

## Limitations

1. **Python Extension Only**: Not available in standalone CLI
2. **Type Conversion**: Complex objects may have conversion overhead
3. **Error Propagation**: Python exceptions become Grapa error objects
4. **Performance**: Callbacks have overhead compared to native Grapa operations

## See Also

- [Python Integration Guide](../python_integration.md) - Comprehensive Python integration documentation
- [Python Use Cases](../python_use_cases.md) - Real-world examples and patterns
- [System Objects](../sys/) - Other system objects and their capabilities
