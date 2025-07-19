# Grapa Examples

This document provides practical examples of Grapa usage, organized from basic to advanced.

> **For Python Users:**
> See the [Python Integration Guide](python_integration.md) and [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users) for help translating Python code and using Grapa from Python.

> **See Also:**
> - [Getting Started](GETTING_STARTED.md)
> - [Python-to-Grapa Migration Guide](migrations/PYTHON_TO_GRAPA_MIGRATION.md)
> - [JS-to-Grapa Migration Guide](migrations/JS_TO_GRAPA_MIGRATION.md)
> - [Language Reference](FUNCTION_QUICK_REFERENCE.md)

## How to use this file
- Start with the basic examples and work your way down.
- Copy/paste examples into the Grapa REPL or a `.grc` file to experiment.
- For syntax rules, see the [Syntax Quick Reference](syntax/basic_syntax.md).

## Syntax Reminders
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`), not line comments (`// ...`).
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Table of Contents
- [Basic Examples](#basic-examples)
- [Data Processing](#data-processing)
- [Dynamic Code Execution](#dynamic-code-execution)
- [Pattern Matching (Grep)](#grep---pattern-matching)

## Basic Examples

### Hello World
```grapa
/* Hello World */
"Hello, Grapa!".echo();
```

### Basic Arithmetic
```grapa
/* Unlimited precision arithmetic */
result = 123456789012345678901234567890 * 987654321098765432109876543210;
result.echo();
```

### Working with Lists
```grapa
numbers = [1, 2, 3, 4, 5];
sum = numbers.reduce(op(a, b) { a + b; }, 0);
("Sum: " + sum).echo();
```

### Generating a Sequence (Range)
```grapa
/* Generate an array of numbers 0..9 */
seq = (10).range(0,1);
seq.echo();  /* Output: [0,1,2,3,4,5,6,7,8,9] */
```

### For-Loop Equivalent with range() and reduce()

You can use `.range()` and `.reduce()` together to replicate most for-loop behaviors, especially for accumulation or collecting values.

**Sum numbers 0..9:**
```grapa
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
sum.echo();  /* Output: 45 */
```

**Collect even numbers 0..9:**
```grapa
evens = (10).range(0,1).reduce(op(acc, x) {
    if (x % 2 == 0) { acc += x; };
}, []);
evens.echo();  /* Output: [0,2,4,6,8] */
```

> **Note:** This is the idiomatic Grapa alternative to a classic for loop for accumulation and transformation tasks.

### Parallel Sequence Processing with range() and map()/filter()

You can use `.range()` with `.map()` or `.filter()` to generate or filter sequences in parallel. This is useful for data generation, transformation, or extracting subsets.

**Example: Squares of 0..9**
```grapa
squares = (10).range(0,1).map(op(x) { x * x; });
squares.echo();  /* Output: [0,1,4,9,16,25,36,49,64,81] */
```

**Example: Even numbers 0..9**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
evens.echo();  /* Output: [0,2,4,6,8] */
```

> **Warning:** `.map()` and `.filter()` are parallel by default and will create one thread per item. For large arrays, always specify a thread count to avoid resource exhaustion:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

## Data Processing

> **Best Practices:**
> - Use `.range()` to generate a sequence array: e.g., `(10).range(0,1)` gives `[0,1,2,3,4,5,6,7,8,9]`.
> - Use functional chaining: `.map()`, `.reduce()`, `.filter()` for processing lists, arrays, and parsed data.
> - Always check for `$ERR` when accessing keys or attributes that may not exist (e.g., `if (item.type() != $ERR)`).
> - Use `[]` for list/array access, not `.get()` (which is for objects/tables).
> - Handle errors explicitly; Grapa does not support `try/catch`—check return values and use `if` statements for error handling.
> - See [docs/obj/transform.md](obj/transform.md) for advanced range/sequence usage.
> **Parallel ETL Advantage:**
> Grapa's parallel data processing (e.g., with `.map()`, `.filter()`, `.reduce()`) is a core, production-ready feature. Parallelism is well tested and hardened for real-world ETL workloads, making Grapa especially powerful for high-throughput data tasks.

### String Word Length
The following returns the length of each word in a string:
```grapa
/* Returns the length of each word in a string */
"this is a test".split(" ").reduce(op(a,b){a+=b.len();},[]);
// Output: [4,2,1,4]
```

### File Processing
```grapa
/* Read and process a file */
content = $file().read("data.txt");
lines = content.split("\n");
filtered = lines.filter(op(line) { line.len() > 0; });
result = filtered.map(op(line) { line.upper(); });
result.echo();
```

### JSON Processing
```grapa
/* Parse and process JSON data */
json_data = $file().read("data.json").json();
users = json_data.users;
active_users = users.filter(op(user) { user.active == true; });
names = active_users.map(op(user) { user.name; });
names.echo();
```

## Dynamic Code Execution

Grapa's most powerful feature is its ability to compile and execute code at runtime. This enables advanced meta-programming patterns:

### Basic Dynamic Execution
```grapa
/* Direct string execution */
op()("'Hello, World!'.echo();")();
/* Output: Hello, World! */

/* With parameters */
func = op("name"=0)("'Hello, ' + name + '!'.echo();");
func("Grapa");
/* Output: Hello, Grapa! */
```

### Dynamic Function Generation
```grapa
/* Generate functions from configuration */
operations = ["add", "sub", "mul", "div"];
funcs = {};
i = 0;
while (i < operations.len()) {
    op_name = operations.get(i);
    code = "a " + op_name + " b";
    funcs[op_name] = op("a"=0, "b"=0)(code);
    i += 1;
}

/* Execute generated functions */
funcs["add"](10, 5).echo();  /* 15 */
funcs["sub"](10, 5).echo();  /* 5 */
funcs["mul"](10, 5).echo();  /* 50 */
funcs["div"](10, 5).echo();  /* 2 */
```

### Template-Based Code Generation
```grapa
/* Create reusable templates */
template = "result = base * multiplier + offset; result";
process = op("base"=0, "multiplier"=1, "offset"=0)(template);
process(10, 2, 5).echo();  /* 25 */

/* Dynamic script evaluation */
user_input = "2 * (3 + 4)";
result = $sys().eval(user_input);
("Result: " + result).echo();  /* Result: 14 */
```

### Compiled Execution for Performance
```grapa
/* Compile once, execute many times */
compiled = $sys().compile("x = input * 2; y = x + offset; y");

/* Execute with different parameters */
result1 = $sys().eval(compiled, {"input": 10, "offset": 5});
result1.echo();  /* 25 */

result2 = $sys().eval(compiled, {"input": 20, "offset": 10});
result2.echo();  /* 50 */
```

### System-Level Evaluation
```grapa
/* Evaluate expressions with parameters */
result = $sys().eval("x + y", {"x": 5, "y": 3});
result.echo();  /* 8 */

/* Complex expressions */
result = $sys().eval("(a + b) * c", {"a": 2, "b": 3, "c": 4});
result.echo();  /* 20 */
```

## Grep - Pattern Matching

### Basic Pattern Matching
```grapa
"Hello world".grep("world")
["Hello world"]
```

### Match-Only Output
```grapa
"Hello world".grep("world", "o")
["world"]
```

### Case-Insensitive Matching
```grapa
"Hello WORLD".grep("world", "i")
["Hello WORLD"]

// Unicode case folding examples
"İstanbul ıstanbul".grep("istanbul", "i")
["İstanbul ıstanbul"]  // Turkish I characters

"Straße".grep("strasse", "i")
["Straße"]  // German sharp S

"γράμμα".grep("ΓΡΑΜΜΑ", "i")
["γράμμα"]  // Greek final sigma

"café résumé naïve".grep("CAFE RESUME NAIVE", "i")
["café résumé naïve"]  // Accented characters
```

### Context Lines

```grapa
input = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5\nLine 6\nLine 7";

// After context (2 lines after match)
input.grep("Line 2", "A2")
["Line 2\n", "Line 3\n", "Line 4\n"]

// Before context (2 lines before match)
input.grep("Line 5", "B2")
["Line 3\n", "Line 4\n", "Line 5\n"]

// Combined context (1 line before and after)
input.grep("Line 4", "A1B1")
["Line 3\n", "Line 4\n", "Line 5\n"]

// Context merging example - overlapping regions are merged
input2 = "a\nb\nc\nd\ne\nf";
input2.grep("c|d", "A1B1")
["b\n", "c\n", "d\n", "e\n"]  // Overlapping context merged into single block

// Context separators between non-overlapping blocks
input3 = "a\nb\nc\nd\ne\nf\ng\nh\ni\nj";
input3.grep("c|i", "A1B1")
["b\n", "c\n", "d\n", "--\n", "h\n", "i\n", "j\n"]  // -- separator between blocks
```

### Context Separators
```grapa
// Multiple non-overlapping context blocks are separated by -- lines
input = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5\nLine 6\nLine 7";
input.grep("Line 2|Line 6", "A1B1")
["Line 1\n", "Line 2\n", "Line 3\n", "--\n", "Line 5\n", "Line 6\n", "Line 7"]

// Context separators are not output in match-only mode
input.grep("Line 2|Line 6", "oA1B1")
["Line 2", "Line 6"]  // Only matches, no context or separators

// JSON output uses --- as separator
input.grep("Line 2|Line 6", "jA1B1")
["Line 1\n", "Line 2\n", "Line 3\n", "---", "Line 5\n", "Line 6\n", "Line 7"]
```

### Column Numbers
```grapa
// Get column positions of matches
"foo bar baz\nbar foo baz\nbaz bar foo".grep("foo", "oT")
["1:foo", "5:foo", "9:foo"]

// Multiple matches on same line
"foofoo bar".grep("foo", "oT")
["1:foo", "4:foo"]

// Column numbers with other options
"Hello world".grep("world", "oTL")
["7:\x1b[1;31mworld\x1b[0m"]
```

### Color Output
```grapa
// Add ANSI color codes around matches
"Hello world".grep("world", "oL")
["\x1b[1;31mworld\x1b[0m"]

// Color with case-insensitive matching
"Hello WORLD".grep("world", "oiL")
["\x1b[1;31mWORLD\x1b[0m"]

// Color with column numbers
"Hello world".grep("world", "oTL")
["7:\x1b[1;31mworld\x1b[0m"]
```

### Unicode Support
```grapa
"café résumé".grep("cafe", "d")
["café résumé"]
```

### Grapheme Cluster Patterns
```grapa
// Extract individual grapheme clusters
"e\u0301\n😀\u2764\ufe0f".grep("\\X", "o")
["é", "\n", "😀", "❤️"]

// Complex grapheme clusters with combining marks
"café résumé".grep("\\X", "o")
["c", "a", "f", "é", " ", "r", "é", "s", "u", "m", "é"]

// Emoji with modifiers
"😀\u2764\ufe0f".grep("\\X", "o")
["😀", "❤️"]
```

### Error Handling
```grapa
// Invalid patterns return empty arrays instead of crashing
"Hello world".grep("(", "o")
[]

"Hello world".grep(")", "o")
[]

"Hello world".grep("a{", "o")
[]

// Safe pattern testing
patterns = ["(", ")", "a{", "", "\\"];
for (i = 0; i < patterns.len(); i = i + 1) {
    result = "test".grep(patterns[i], "o");
    ("Pattern '" + patterns[i] + "' result: " + result.str() + "\n").echo();
}
```

### Unicode Edge Cases
```grapa
// Zero-length matches (now working correctly)
"abc".grep("^", "o")
[""]  // Empty string for each line

// Unicode boundary handling
"ÉÑÜ".grep(".", "o")
["É", "Ñ", "Ü"]

// Case-insensitive Unicode (may group characters due to Unicode complexity)
"ÉÑÜ".grep(".", "oi")
["ÉÑ", "Ü"]  // É and Ñ may be grouped together
```

### Word Boundaries
```grapa
// Match only standalone words
"hello world hello123 hello_test hello-world hello".grep("hello", "wo")
["hello", "hello"]

// Word boundaries with different characters
"hello_test hello test_hello _hello_ hello".grep("hello", "wo")
["hello"]

// Word boundaries with case-insensitive matching
"Hello WORLD hello123 HELLO_test".grep("HELLO", "woi")
["Hello", "HELLO"]

// Manual vs automatic word boundaries
"hello world hello123".grep("\\bhello\\b", "o")
["hello"]

"hello world hello123".grep("hello", "wo")
["hello"]
```

### Custom Delimiters
```grapa
"Line 1|Line 2|Line 3".grep("Line 2", "", "|")
["Line 2"]
```

### Null-Data Mode (Limited Support)
```grapa
// Note: The "z" option is implemented but limited by Grapa's string parser
// \x00 escape sequences are not converted to actual null bytes
"foo\x00bar\x00baz".grep("foo", "oz")
// Expected: ['foo'] (null-separated records)
// Current: ['foo\x00bar\x00baz'] (treats \x00 as literal characters)

// Workaround: Use custom delimiters
"foo|bar|baz".grep("foo", "", "|")
// Result: ['foo'] (works correctly)
```

## Grammer Updating

The standard grammer loaded by default includes suport for 2 user defined rules that assist in easily modifying the grammer.

custom_start - The engine will evaulate the $start rule before evaluating the predefined $start rule. Setting a variable "start" with a rule will cause the engine to call the user defined rule first. If this variable is set locally within a function, it will only apply to scripts parsed from within the scope of that function.

custom_function - The engine will evaluate the variable "function" if it is a $RULE as a part of the $function rule. To verify, check the $function variable. Used in contexts where a value is returned.

custom_command - Same as above. Used in contexts where a value is not returned.

Alternative, redefine any of the predefined rules for the default grammer. Take caution on changing the global variable, unless the change is inteded for the entire session. Restart the app to revert back. 

If any of the above is set as a local variable within some scope, such as a class instance or a function, the modification will only apply for exectution within that scope. You can, for example, have muliple threads with unique modifications that apply separately. This is bascially the result of having the rules implemented as variables.

## PCA - Principal Component Analysis

Center the data around the mean, calcualte covariance, and than the eigenvalues and eigenvectors. Identify the number of components based on the eigenvalues, and select that number of eigenvectors. Use the eigenvectors to calculate PCA. Use the result for the new feature (replacing the old features). Use the same method for generating the feature from new values (need to save the mean and eigenvectors for this).

```
X=[[3.4,9.1,1.4],[9.1,1.2,8.2],[1.4,8.2,5.2]];
M=X.mean(1);
R=(X-M).cov(1).eigh();

R.w;
[41.7879952005570829404677683153157,4.4920047994429170595322314913591,0.000000000000000000000000193325118422339381744140663363374750345332]

Y=R.v.left(1);
(Y.t() .* (X-M).t()).t();

[[4.3153778898305898473304814235512],[-7.4322629041586428646647322175959],[3.1168850143280530173342507940447]]

(Y.t() .* ([[3.4,9.1,1.4]]-M).t()).t();
[[4.3153778898305898473304814235512]]

(Y.t() .* ([[3.4,9.23,2.4]]-M).t()).t();
[[3.9393492470695862312625754586492]]

```

### Advanced Context Examples

```grapa
// Context merging - overlapping regions are automatically merged
input = "a\nb\nc\nd\ne\nf";
input.grep("c|d", "A1B1")
["b\n", "c\n", "d\n", "e\n"]  // Overlapping context merged into single block

// Context separators between non-overlapping blocks
input2 = "a\nb\nc\nd\ne\nf\ng\nh\ni\nj";
input2.grep("c|i", "A1B1")
["b\n", "c\n", "d\n", "--\n", "h\n", "i\n", "j\n"]  // -- separator between blocks

// Complex context with multiple options
log_content.grep("error", "A2B1io")  // 2 lines after, 1 before, match-only, case-insensitive
```

### Advanced Unicode Examples

```grapa
// Comprehensive Unicode "o" option testing
"éñü".grep(".", "o")
["é", "ñ", "ü"]  // Perfect Unicode character extraction

// Unicode with normalization and "o" option
"café résumé".grep("\\X", "oN")
["c", "a", "f", "é", " ", "r", "é", "s", "u", "m", "é"]  // Normalized grapheme clusters

// Complex Unicode scenarios with "o" option
"👨‍👩‍👧‍👦".grep("\\X", "o")
["👨‍👩‍👧‍👦"]  // Family emoji as single grapheme cluster

// Unicode properties with "o" option
"Hello 世界 123".grep("\\p{L}+", "o")
["Hello", "世界"]  // Unicode letters only

// Diacritic-insensitive with "o" option
"café résumé naïve".grep("cafe", "od")
["café"]  // Diacritic-insensitive matching
```

## Next Steps
- Learn about [Testing](TESTING.md) your Grapa code
- Check out the [Grep functionality](GREP.md) for advanced pattern matching
- Review the [Syntax Quick Reference](syntax/basic_syntax.md) for more syntax rules and tips

## Thread Safety and Parallelism
Grapa is fully thread safe by design. All variable and data structure updates are internally synchronized at the C++ level, so you will never encounter crashes or corruption from concurrent access. However, if your program logic allows multiple threads to read and write the same variable or data structure, you may see *logical* race conditions (unexpected values, overwrites, etc.). This is a design consideration, not a stability issue. Minimize shared mutable state between threads unless intentional.

**Only `$thread()` objects provide explicit locking and unlocking via `lock()`, `unlock()`, and `trylock()`.** To protect access to a shared resource, create a `$thread()` lock object and use it to guard access. Calling `.lock()` or `.unlock()` on a regular variable (like an array or scalar) will return an error.

**Canonical Example:**
```grapa
lock_obj = $thread();
lock_obj.lock();
// ... perform thread-safe operations on shared data ...
lock_obj.unlock();
```

See [Threading and Locking](sys/thread.md) and [Function Operators: static and const](operators/function.md) for details and best practices.
