# $STR
References:


ASCII only - Unicode will eventually be added. Can initialize with either double quotes or single quotes - which is easier than escaping a string that includes a quoted string - such as "this 'is' a test", or 'this "is" a test'. $STR inherits the $obj class - see $obj for functions supported. 

## String Literals

Grapa supports three types of string literals:

### Double and Single Quotes
```grapa
/* Double quotes */
name = "John Doe";
message = "Hello World";

/* Single quotes */
name = 'John Doe';
message = 'Hello World';

/* Mixed quotes to avoid escaping */
text = "This 'is' a test";  /* No need to escape single quotes */
text = 'This "is" a test';  /* No need to escape double quotes */
```

### Raw Strings
Raw strings (prefixed with `r`) suppress escape sequence processing, making them ideal for regex patterns and paths:

```grapa
/* Regular string - requires double escaping for regex */
pattern = "\\w+\\s+\\d+";

/* Raw string - no escaping needed */
pattern = r"\w+\s+\d+";

/* File paths with raw strings */
path = r"C:\Users\Name\Documents\file.txt";

/* Complex regex patterns */
email_pattern = r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$";
```

**Important:** In raw strings, **only the enclosing quote character** (whether `"` or `'`) needs to be escaped. This is the only escape sequence that the string parser recognizes in raw strings:

```grapa
/* Escape the enclosing quote character in raw strings */
text = r"This \"is\" a test";  /* Escape double quotes when using double quotes */
text = r'This \'is\' a test';  /* Escape single quotes when using single quotes */

/* All other backslashes are treated literally */
path = r"C:\Users\Name\Documents\file.txt";  /* No escaping needed for backslashes */
pattern = r"\w+\s+\d+";  /* No escaping needed for regex */
```

## Common String Operations

### Basic Operations
Commands | Results
------------ | -------------
"this is a test".len(); | 14
"this is a test".split(""); | ["t","h","i","s"," ","i","s"," ","a"," ","t","e","s","t"]
"this is a test".raw(); | 0x7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996

### Trimming Operations
```grapa
/* Basic trimming */
"  hello world  ".trim() -> "hello world"

/* Trim specific characters */
"bbbtestingbbb".trim("b") -> "testing"

/* Trim multiple whitespace characters */
whitespace = [" ", "\t", "\n", "\r"];
"  \t\n\r  hello world  \t\n\r  ".trim(whitespace) -> "hello world"

/* Left and right trimming */
"  hello world  ".ltrim() -> "hello world  "
"  hello world  ".rtrim() -> "  hello world"
```

> **See Also:** [String Transform Functions](../obj/transform.md#trimchars) for comprehensive trimming documentation including array-based multi-character trimming.

## String Templates and Dynamic Construction

Grapa provides several powerful approaches for constructing strings dynamically, from simple concatenation to advanced template patterns.

### String Concatenation (Simple Cases)

For basic string construction, use concatenation with parentheses:

```grapa
/* Basic concatenation */
name = "Alice";
age = 25;
message = "Hello, " + name + "! You are " + age.str() + " years old.";
message.echo();  /* Output: Hello, Alice! You are 25 years old. */

/* With parentheses for clarity */
message = ("Hello, " + name + "! You are " + age.str() + " years old.");
```

### Parameterized Templates (Reusable)

For reusable templates with parameters, use `op()` functions:

```grapa
/* Create a parameterized template */
template = op("name"=0, "age"=0){
    name + "! You are " + age.str() + " years old."
};

/* Use the template multiple times */
result1 = template("Alice", 25);
result1.echo();  /* Output: Alice! You are 25 years old. */

result2 = template("Bob", 30);
result2.echo();  /* Output: Bob! You are 30 years old. */
```

### String Interpolation (Recommended)

For modern string construction with embedded expressions, use the `interpolate()` method:

```grapa
/* Basic string interpolation */
name = "Alice";
age = 25;
result = "Hello ${name}, you are ${age} years old!".interpolate();
result.echo();  /* Output: Hello Alice, you are 25 years old! */

/* Expression evaluation */
result = "Sum: ${1 + 2}, Product: ${3 * 4}".interpolate();
result.echo();  /* Output: Sum: 3, Product: 12 */

/* Script execution with op() function */
x = 10;
result = "Value: ${op()(\"x\")()}".interpolate();
result.echo();  /* Output: Value: 10 */

/* Template with parameters */
result = "Hello ${name}".interpolate({name:"Alice"});
result.echo();  /* Output: Hello Alice */
```

> **See Also:** [String Interpolation Documentation](../obj/transform.md#interpolateparams) for comprehensive documentation including advanced features like multi-level parameter passing and dynamic script execution.

### Dynamic Code Execution (Complex Cases)

For complex string construction with arbitrary expressions:

```grapa
/* Dynamic template as string */
template = "name + \"! You are \" + age.str() + \" years old.\"";
name = "Alice";
age = 25;
result = op()(template)();
result.echo();  /* Output: Alice! You are 25 years old. */

/* System-level evaluation with context */
result = $sys().eval("name + \"! You are \" + age.str() + \" years old.\"", {"name": name, "age": age});
result.echo();  /* Output: Alice! You are 25 years old. */
```

### Template Patterns for Common Use Cases

#### User Greeting Templates
```grapa
/* Simple greeting template */
greeting = op("name"=0, "time"=0){
    "Good " + time + ", " + name + "!"
};

greeting("Alice", "morning").echo();  /* Good morning, Alice! */
greeting("Bob", "evening").echo();    /* Good evening, Bob! */
```

#### Data Formatting Templates
```grapa
/* Data formatting template */
format_record = op("name"=0, "age"=0, "city"=0){
    "Name: " + name + ", Age: " + age.str() + ", City: " + city
};

record = format_record("Alice", 25, "New York");
record.echo();  /* Output: Name: Alice, Age: 25, City: New York */
```

#### Dynamic SQL-like Queries
```grapa
/* Dynamic query template */
build_query = op("table"=0, "conditions"=0){
    "SELECT * FROM " + table + " WHERE " + conditions
};

query = build_query("users", "age > 18");
query.echo();  /* Output: SELECT * FROM users WHERE age > 18 */
```

### When to Use Each Approach

| Use Case | Recommended Approach | Example |
|----------|---------------------|---------|
| **Simple concatenation** | String concatenation | `"Hello, " + name` |
| **Variable substitution** | String interpolation | `"Hello ${name}".interpolate()` |
| **Expression evaluation** | String interpolation | `"Sum: ${x + y}".interpolate()` |
| **Script execution** | String interpolation | `"${op()(\"script\")()}".interpolate()` |
| **Reusable templates** | `op()` functions | `template = op("name"=0){...}` |
| **Complex expressions** | Dynamic execution | `op()(template)()` |
| **System integration** | `$sys().eval()` | `$sys().eval(script, params)` |

### Performance Considerations

- **String concatenation**: Fastest for simple cases
- **String interpolation**: Good for variable substitution and expressions
- **`op()` templates**: Good for reusable templates (compiled once)
- **Dynamic execution**: More overhead, use for complex cases only
- **`$sys().eval()`**: Highest overhead, use for system integration

> **Note**: Grapa's template approaches are more powerful than traditional string interpolation because they can execute arbitrary expressions and complex logic, not just variable substitution.

---

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
