# $STR
References:


**Full Unicode Support** - Grapa strings support complete Unicode including emoji, accented characters, and complex grapheme clusters. Can initialize with either double quotes or single quotes - which is easier than escaping a string that includes a quoted string - such as "this 'is' a test", or 'this "is" a test'. $STR inherits the $obj class - see $obj for functions supported.

**Important:** While string **content** supports full Unicode, **identifiers** (variable names, function names) are limited to ASCII characters only. See [$ID Documentation](id.md) for details on identifier limitations.

**Exception:** When using **quoted property access** (e.g., `obj.'property_name'`), property names can contain Unicode characters since they're treated as string literals. 

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

## Unicode Support

Grapa provides **full Unicode support** for strings, including:

### Unicode Characters and Emoji
```grapa
/* Accented characters */
text = "café résumé naïve";
text.len();  /* Returns: 15 (Unicode character count) */

/* Emoji and symbols */
emoji = "🚀🌟🎉";
emoji.len();  /* Returns: 3 (Unicode character count) */

/* Complex grapheme clusters */
family = "👨‍👩‍👧‍👦";
family.len();  /* Returns: 1 (single grapheme cluster) */

/* Mixed Unicode content */
mixed = "Hello 世界 🌍";
mixed.len();  /* Returns: 8 (Unicode character count) */
```

### Unicode-Aware String Functions

All string manipulation functions in Grapa are Unicode-aware:

```grapa
/* Unicode-aware length counting */
"héllo".len();      /* Returns: 5 (characters, not bytes) */
"🚀héllo".len();    /* Returns: 6 (characters, not bytes) */

/* Unicode-aware substring operations */
"héllo".left(3);    /* Returns: "hél" (3 Unicode characters) */
"🚀héllo".right(3); /* Returns: "llo" (3 Unicode characters) */
"héllo".mid(1, 3);  /* Returns: "éll" (3 Unicode characters from position 1) */
/* Note: .left(), .right(), and .mid() also work with $ARRAY and $GOBJ types */

/* Unicode-aware reversal */
"héllo".reverse();  /* Returns: "olléh" (Unicode characters reversed) */

/* Unicode-aware padding */
"héllo".lpad(10, "🚀"); /* Returns: "🚀🚀🚀🚀🚀héllo" */
"héllo".rpad(10, "é");  /* Returns: "hélloééééé" */
```

### Unicode vs Byte Operations

Grapa provides both Unicode character operations and byte-level operations:

```grapa
unicode_text = "héllo 🚀";

/* Unicode character operations */
unicode_text.len();    /* Returns: 7 (Unicode character count) */

/* Byte-level operations */
unicode_text.bytes();  /* Returns: 11 (byte count) */
unicode_text.raw();    /* Returns: 0x68C3A96C6C6F20F09F9A80 (hex bytes) */
```

> **See Also:** [Object Methods Documentation](obj_methods.md) for comprehensive details on Unicode-aware string functions including `.len()`, `.bytes()`, `.raw()`, `.left()`, `.right()`, `.mid()`, `.reverse()`, `.lpad()`, and `.rpad()`. Note that `.left()`, `.right()`, and `.mid()` also support `$ARRAY` and `$GOBJ` types.

## String-Specific Methods

The following methods are available specifically on `$STR` objects:

### Case Conversion
```grapa
/* Basic case conversion */
"Hello World".upper();     /* Returns: "HELLO WORLD" */
"Hello World".lower();     /* Returns: "hello world" */
"Hello World".casefold();  /* Returns: "hello world" */

/* Unicode case folding for international text */
"İstanbul".casefold();     /* Returns: "istanbul" */
"ß".casefold();            /* Returns: "ss" */
```

### String Similarity Functions
```grapa
/* Levenshtein distance - edit distance between strings */
"hello".levenshtein("hallo");     /* Returns: 1 (one character difference) */
"kitten".levenshtein("sitting");  /* Returns: 3 (three edit operations) */

/* Jaro-Winkler similarity - string similarity score */
"hello".jarowinkler("hallo");     /* Returns: 0.933... (high similarity) */
"hello".jarowinkler("world");     /* Returns: 0.0 (no similarity) */

/* Cosine similarity - vector-based similarity */
"hello world".cosinesimilarity("world hello");  /* Returns: 1.0 (identical words) */
"hello world".cosinesimilarity("goodbye world"); /* Returns: 0.5 (partial similarity) */
```

**Use Cases:**
- **Case conversion**: Text normalization, user input processing
- **Levenshtein distance**: Spell checking, fuzzy matching, edit distance calculations
- **Jaro-Winkler similarity**: Record matching, duplicate detection, name matching
- **Cosine similarity**: Document similarity, text analysis, recommendation systems

> **Note:** While string content supports full Unicode, **identifiers** (variable names, function names) are limited to ASCII characters only. This is a lexical limitation in the parser. See [$ID Documentation](id.md) for details.

## Common String Operations

### Basic Operations
Commands | Results
------------ | -------------
"this is a test".len(); | 14
"this is a test".split(""); | ["t","h","i","s"," ","i","s"," ","a"," ","t","e","s","t"]
"this is a test".raw(); | 0x7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996
"this is a test".find("test"); | 10
"this is a test".find("xyz"); | -1

### String Search with `.find()`

The `.find()` method searches for a substring or binary pattern within a string or RAW data, returning the 0-based index of the first occurrence or -1 if not found.

```grapa
/* Basic string search */
"hello world".find("world");      /* Returns: 6 */
"hello world".find("xyz");        /* Returns: -1 (not found) */
"hello world".find("o");          /* Returns: 4 (first occurrence) */

/* RAW data search */
0x10FE0021FA.uraw().find(0xFE0021.uraw());  /* Returns: 1 */
0x10FE0021FA.uraw().find(0x21.uraw());      /* Returns: 3 */

/* Search with null bytes */
$RAW("hello\0world").find("world".raw());   /* Returns: 6 */
$RAW("hello\0world").find("\0".raw());      /* Returns: 5 */

/* Optional start position */
"hello world".find("o", 5);       /* Returns: 7 (search from position 5) */
"hello world".find("o", 8);       /* Returns: -1 (not found after position 8) */

/* Optional start position and size limit */
"hello world".find("o", 1, 5);    /* Returns: 4 (search from position 1, limit 5 chars) */
```

**Parameters:**
- `term` (required): The substring or binary pattern to search for
- `start` (optional): Starting position for the search (default: 0)
- `size` (optional): Maximum number of characters/bytes to search (default: to end)

**Supported Data Types:**
- **$STR**: Searches for Unicode substrings
- **$RAW**: Performs binary search (handles null bytes and binary data)
- **$ARRAY**: Searches for subarrays using comprehensive comparison
- **$GOBJ**: Searches for sublists using comprehensive comparison

**Return Value:**
- Returns the 0-based index of the first occurrence
- Returns -1 if the term is not found
- Returns -1 if start position is beyond the string/data length

> **Note:** For RAW data, use `.uraw()` to create search terms from hex values to avoid padding issues.
> 
> **Array and List Search:** The `.find()` method supports searching for subarrays and sublists using the same comprehensive comparison logic as `switch` statements, ensuring consistent behavior across the language.
> 
> **Array-in-List Search:** When searching a `$GOBJ` with an `$ARRAY` term, the method searches for the array values within the list values and returns the position where the match is found. Use `.mid()` to extract the subset. For example: `{a:1,b:2,c:3}.find([2,3])` returns `1`, and `{a:1,b:2,c:3}.mid(1, 2)` returns `{b:2,c:3}`.

### Pattern Matching
```grapa
/* Basic pattern matching */
"hello world".match("hello");     /* true - pattern found */
"hello world".match("xyz");       /* false - pattern not found */
"hello world".match("a{");        /* false - invalid pattern handled gracefully */

/* Case insensitive matching */
"hello world".match("HELLO", "i"); /* true - case insensitive match */
"Hello World".match("world", "i"); /* true - case insensitive match */

/* With all grep parameters */
"hello world".match("hello", "i", "", "", "", 1); /* true - with all options */
```

> **See Also:** [Grep Method Documentation](../obj/transform.md#grep) for comprehensive regex search capabilities including advanced options, Unicode support, and complex pattern matching.

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
