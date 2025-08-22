# $ID
References:


Any identifier (starts with a letter and can follow with letters or numbers) will be initialized as an $ID. And an $ID can be used for many things, including associating a value. Optionally, the '@' symbol is used to de-reference an $ID to retrieve the data stored - which is useful when being used in a location that requires an $ID (such as a label). When used in a location that requires a value, an $ID will be automatically dereferenced.

**Important:** Identifiers (variable names, function names) are **not Unicode-aware** and are limited to ASCII characters. This is a lexical limitation - the lexer uses a specific array of allowed bytes for identifiers. While string content supports full Unicode, identifiers must use only ASCII letters, numbers, and underscores.

**Exception:** When using **quoted property access** (e.g., `obj.'property_name'`), the property name can contain Unicode characters since it's treated as a string literal rather than an identifier. 

Example of using an $ID as a variable:
```grapa
> x = "hi"

> x
hi

> x + " test"
hi test

> {x:"test"}
{"x":"test"}

> {@x:"test"}
{"hi":"test"}
```

### Valid Identifiers (ASCII Only)

```grapa
/* Valid variable names */
name = "John";
age_123 = 30;
user_name = "alice";
test123 = "works";

/* Valid function names */
function_name = op(x) { x.echo(); };
calculate_sum = op(a, b) { a + b; };
```

### Invalid Identifiers (Unicode Not Supported)

```grapa
/* These will cause parsing errors */
héllo = "test";     /* Error: Unicode character in identifier */
café = "test";      /* Error: Unicode character in identifier */
user_世界 = "test";  /* Error: Unicode character in identifier */
🚀variable = "test"; /* Error: Unicode character in identifier */
```

**Note:** While identifiers must be ASCII-only, the string content they store can contain full Unicode:

```grapa
/* Valid: ASCII identifier with Unicode content */
message = "Hello 世界! 🚀";
message.echo();  /* Output: Hello 世界! 🚀 */

/* Valid: ASCII identifier with Unicode function name as string */
function_name = "café_function";
function_name.echo();  /* Output: café_function */
```

### Unicode Property Access (Exception)

When using quoted property access, you can use Unicode characters in property names:

```grapa
/* Object with Unicode property names */
data = {
    "héllo": 5,
    "🚀test": 10,
    "世界": "hello",
    "👨‍👩‍👧‍👦": "family"
};

/* Access Unicode properties using quoted syntax */
data.'héllo'.echo();      /* Output: 5 */
data.'🚀test'.echo();     /* Output: 10 */
data.'世界'.echo();        /* Output: hello */
data.'👨‍👩‍👧‍👦'.echo();   /* Output: family */

/* Functions with Unicode names via quoted access */
functions = {
    "🚀function": op(x, y) { x + y },
    "世界_function": op(x) { x * 2 }
};

functions.'🚀function'(3, 4).echo();     /* Output: 7 */
functions.'世界_function'(5).echo();     /* Output: 10 */
```

**Key Point:** This works because quoted property access treats the property name as a string literal, bypassing the identifier lexer restrictions.

---

## See also
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Use Cases](../use_cases/index.md)
- [Language Reference](../syntax/basic_syntax.md)
