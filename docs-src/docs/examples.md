# Examples

Welcome to the Grapa Examples hub! Here you'll find practical, ready-to-run scripts and code snippets demonstrating Grapa's core features, from basic syntax to advanced data processing and Python integration.

[See the Examples Overview](examples/README.md)

## Example Scripts

### **Core Language Examples**
| Example | Description |
|---------|-------------|
| [Basic Example](examples/basic_example.grc) | Demonstrates basic Grapa syntax, variable assignment, arrays, strings, and math operations. |
| [Advanced Example](examples/advanced_example.grc) | Shows more complex operations: string processing, array manipulation, and returning multiple results. |
| [Performance Example](examples/performance_example.grc) | Illustrates performance-related operations, including loops and array creation. |
| [Vector Operations](examples/vector_operations.grc) | Comprehensive examples of vector operations including matrix operations, linear algebra, statistical functions, and multi-dimensional data processing. |
| [Array Comprehension Alternatives](examples/array_comprehension_alternatives.grc) | Demonstrates how to accomplish array comprehension patterns using Grapa's functional methods with threading support. |
| [String Interpolation](examples/string_interpolation_example.grc) | Shows advanced string interpolation techniques including variable substitution, expression evaluation, and dynamic script execution. |
| [Database Examples](examples/database_examples.grc) | Demonstrates database operations including table creation, data manipulation, queries, and advanced database features. |
| [Advanced Language Features Demo](examples/advanced_language_features_demo.grc) | Comprehensive demonstration of all advanced language features already implemented in Grapa, including pattern matching, metaprogramming, concurrency, type system, data structures, and debugging tools. |


### **Language Extension Examples**
| Example | Description |
|---------|-------------|
| [SQL Syntax Example](examples/sql_syntax_example.grc) | **Example Implementation** - Demonstrates how SQL syntax could be added to Grapa using custom_command and custom_function as variables. Shows the pattern for extending Grapa's grammar. |
| [Language Design Example](examples/language_design_example.grc) | Demonstrates executable BNF system for creating custom languages, DSLs, and extending Grapa. |
| [Isolated Rule Execution Example](examples/isolated_rule_execution_demo.grc) | Demonstrates executing custom rules in isolation for ETL and domain-specific processing. |
| [Wrapper Function Example](examples/wrapper_function_example.grc) | Demonstrates the wrapper function pattern for ETL processing using rule tokens with custom functions. |

### **Integration Examples**
| Example | Description |
|---------|-------------|
| [Python Integration Example](examples/python_integration_example.py) | Demonstrates how to use Grapa from Python, including math, strings, arrays, file, and grep operations. |

> **How to use these examples:**
> - Click any example to view the source.
> - Copy/paste `.grc` scripts into the Grapa REPL or save and run them with the CLI.
> - For Python, run the `.py` script with Python 3 and the `grapapy` module installed.
> 
> **Note about SQL Syntax Example:** The SQL syntax example demonstrates how Grapa's executable BNF can be used to add custom syntax to the language. This is an example implementation showing the pattern - SQL is not natively supported in Grapa, but this example shows how it could be added.

## See Also
- [Getting Started](getting_started.md)
- [API Reference](api_reference.md)
- [Python Integration Guide](python_integration.md)
- [Migration Guides](migrations/python_to_grapa_migration.md)
- [Language Reference](function_quick_reference.md)
- [Use Cases](use_cases/index.md)
- [Testing](testing.md)

---

## Table of Contents
- [Basic Examples](#basic-examples)
- [Data Processing](#data-processing)
- [Dynamic Code Execution](#dynamic-code-execution)
- [Pattern Matching (Grep)](#grep-pattern-matching)

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

### Array and List Operations
```grapa
/* Array operations */
arr = [1, 2, 3, 4, 5];
arr += 6;                 /* Add element: [1,2,3,4,5,6] */
arr += 10 arr[0];         /* Insert at position: [10,1,2,3,4,5,6] */
arr -= arr[2];            /* Remove by index: [10,1,4,5,6] */

/* Array assignment operations */
arr[0] = 100;             /* Direct assignment: [100,1,4,5,6] */
arr[-1] = 999;            /* Negative index assignment: [100,1,4,5,999] */
arr[1] += 50;             /* Compound assignment: [100,51,4,5,999] */
arr.echo();

/* List operations */
list = {a:1, b:2, c:3};
list += (d:4);            /* Add key-value: {"a":1,"b":2,"c":3,"d":4} */
list += (e:5) list[0];    /* Insert at position: {"e":5,"a":1,"b":2,"c":3,"d":4} */
list -= list.b;           /* Remove by key: {"e":5,"a":1,"c":3,"d":4} */

/* List assignment operations */
list.a = 10;              /* Dot notation assignment: {"e":5,"a":10,"c":3,"d":4} */
list["c"] = 30;           /* Bracket notation assignment: {"e":5,"a":10,"c":30,"d":4} */
list[-2] = 300;           /* Negative index assignment: {"e":5,"a":10,"c":300,"d":4} */
list.a += 5;              /* Compound assignment: {"e":5,"a":15,"c":300,"d":4} */
list.echo();

/* Advanced: Recursive keys function using .getname() */
keys = op(lst){lst.reduce(op(acc,x){if(x.type()==$LIST){acc += keys(x);}else{acc += 'x'.getname();}},[]);};

obj = {name:"Alice", age:30, test:{a:1,b:2}, city:"NYC"};
keys(obj).echo();  /* Outputs: [name,age,[a,b],city] */

nested = {level1:{level2:{level3:"value"}}};
keys(nested).echo();  /* Outputs: [level1,[level2,[level3]]] */

/* Concatenation */
arr1 = [1, 2, 3];
arr2 = [4, 5, 6];
arr1 ++= arr2;            /* Concatenate: [1,2,3,4,5,6] */
arr1 ++= arr2 arr1[0];    /* Insert at beginning: [4,5,6,1,2,3] */
arr1.echo();

/* Search and filtering */
data = [{id:1, name:"Alice"}, {id:2, name:"Bob"}, {id:3, name:"Alice"}];
alice_users = data.filter(op(item){item.name == "Alice";});  /* Find all Alice users */
alice_users.echo();

/* List comprehension */
squares = [x*x for x in 5];  /* [0, 1, 4, 9, 16] */
evens = [x for x in 10 if x % 2 == 0];  /* [0, 2, 4, 6, 8] */
doubled = [x*2 for x in [1, 2, 3]];  /* [2, 4, 6] */
chars = [x for x in "hello"];  /* ["h", "e", "l", "l", "o"] */
```

### Truthy and Falsy Examples
```grapa
/* Natural boolean behavior in conditions */
user_input = "hello";
if (user_input) {
    ("Processing: " + user_input).echo();  /* Output: Processing: hello */
} else {
    ("No input provided").echo();
}

/* Check if array has items */
items = [1, 2, 3];
if (items) {
    ("Processing " + items.len() + " items").echo();  /* Output: Processing 3 items */
} else {
    ("No items to process").echo();
}

/* Check if configuration exists */
config = null;
if (config) {
    ("Using configuration").echo();
} else {
    ("Using default settings").echo();  /* Output: Using default settings */
}

/* Ternary operator with truthy/falsy */
status = user_input ? "active" : "inactive";  /* "active" */
message = items ? "Items found" : "No items";  /* "Items found" */

/* Empty values are falsy */
empty_string = "";
empty_array = [];
empty_list = {};

if (!empty_string) { ("Empty string is falsy").echo(); }
if (!empty_array) { ("Empty array is falsy").echo(); }
if (!empty_list) { ("Empty list is falsy").echo(); }

/* Zero is falsy */
count = 0;
if (!count) { ("Zero is falsy").echo(); }

/* Non-zero numbers are truthy */
count = 5;
if (count) { ("Non-zero is truthy").echo(); }

### Vector Operations
```grapa
/* Vector creation and operations */
vec = [1, 2, 3, 4, 5];
vec += 6;                 /* Add element */
vec ++= [7, 8];           /* Extend vector */
vec ++= [9, 10] vec[0];   /* Insert at beginning */

/* Mathematical operations */
vec1 = [1, 2, 3];
vec2 = [4, 5, 6];
sum = vec1 + vec2;        /* Element-wise addition: [5,7,9] */
scaled = vec1 * 2;        /* Scale by scalar: [2,4,6] */
sum.echo();
scaled.echo();
```

### Widget Operations
```grapa
/* Widget creation and manipulation */
form = {name:"login_form", type:"form"};
form += ("username", "text", {placeholder:"Enter username"});
form += ("password", "password", {placeholder:"Enter password"});
form += ("submit", "button", {label:"Login"});

/* Widget styling */
form.children[2].style.background = "#007bff";
form.children[2].style.color = "white";
```

### XML Operations
```grapa
/* XML creation and basic access */
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;

/* Array-like element access */
xml[0]                                    /* Root element */
xml[0][0]                                 /* First child element */
xml[0][0][0]                              /* Element content: "Hello" */

/* Dot notation access */
xml.root                                  /* Root element */
xml.root.item                             /* First item element */
xml.root.item[0]                          /* Item content: "Hello" */

/* Attribute access */
xml[0][0].attr                            /* All attributes: {"id":"1"} */
xml[0][0].attr.id                         /* Specific attribute: "1" */
xml.root.item.attr.id                     /* Via dot notation: "1" */

/* Advanced element finding with .findall() */
xml.findall({name:"item"})                /* All item elements */
xml.findall({name:"item", attr:{id:"2"}}) /* Items with id="2" */
xml.findall({value:"Hello"})              /* Elements with content "Hello" */

/* Complex queries with logical operators */
xml.findall({and:[{name:"item"}, {attr:{id:"1"}}]}) /* AND logic */
xml.findall({or:[{name:"item"}, {name:"title"}]})   /* OR logic */

/* Working with results */
results = xml.findall({name:"item"});
results.len()                              /* Number of matches */
results[0].attr.id                         /* First match's id attribute */

/* HTML element finding */
html = <html><body><h1>Title</h1><p>Content</p></body></html>;
html.findall({name:"p"})                  /* All paragraph elements */

/* XML to LIST conversion */
list = xml.list();                        /* Convert to LIST structure */
list.root[1][0].item[1][0]                /* Access converted data: "Hello" */
```

### LIST and ARRAY Operations with .findall()
```grapa
/* LIST findall capabilities */
data = {name:"Alice", age:30, city:"New York"};

/* Find by property existence */
data.findall({has:{name:"age"}})          /* Returns: {"data":{"name":"Alice","age":30}} */

/* Find by property value */
data.findall({has:{name:"name", value:"Alice"}}) /* Returns: {"data":{"name":"Alice","age":30}} */

/* Complex nested structures */
nested = {user:{name:"Alice", age:30}, admin:{name:"Bob", age:25}};

/* Find nested objects */
nested.findall({has:{name:"user"}})       /* Returns: {"nested":{"user":{"name":"Alice","age":30},"admin":{"name":"Bob","age":25}}} */

/* Find with nested criteria */
nested.findall({has:{name:"user", has:{name:"name", value:"Alice"}}}) /* Returns: {"nested":{"user":{"name":"Alice","age":30},"admin":{"name":"Bob","age":25}}} */

/* ARRAY findall capabilities */
arr = [{name:"Alice"}, {name:"Bob"}, {name:"Charlie"}];

/* Find objects with specific properties */
arr.findall({has:{name:"name"}})          /* Returns: [{"name":"Alice"},{"name":"Bob"},{"name":"Charlie"}] */

/* Find objects with specific property values */
arr.findall({has:{name:"name", value:"Bob"}}) /* Returns: [{"name":"Bob"}] */

/* Complex array queries */
complex_arr = [{user:{name:"Alice", role:"admin"}}, {user:{name:"Bob", role:"user"}}];

/* Find with nested criteria */
complex_arr.findall({has:{name:"user", has:{name:"role", value:"admin"}}}) /* Returns: [{"user":{"name":"Alice","role":"admin"}}] */

/* Logical operations with arrays */
complex_arr.findall({or:[{has:{name:"user", has:{name:"role", value:"admin"}}}, {has:{name:"user", has:{name:"role", value:"user"}}}]}) /* Returns: [{"user":{"name":"Alice","role":"admin"}},{"user":{"name":"Bob","role":"user"}}] */
```

### String Interpolation
```grapa
/* Basic variable interpolation */
name = "Alice";
age = 25;
"Hello ${name}, you are ${age} years old!".interpolate().echo();

/* Expression evaluation */
"Sum: ${1 + 2}, Product: ${3 * 4}".interpolate().echo();

/* Script execution with op() function */
x = 10;
"Value: ${op()(\"x\")()}".interpolate().echo();

/* Template with parameters */
"Hello ${name}".interpolate({name:"Alice"}).echo();
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
casefolded = filtered.map(op(line) { line.casefold(); });
result.echo();
casefolded.echo();
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

### Parameter Matching Examples
```grapa
/* Function with default parameters */
calculator = op(operation="add", x=0, y=0) {
    if (operation == "add") { x + y; }
    else if (operation == "multiply") { x * y; }
    else if (operation == "power") { x**y; }
    else { 0; }
};

/* Traditional named parameters */
calculator(operation="multiply", x=5, y=3).echo();  /* 15 */

/* JSON-style named parameters */
calculator(operation:"power", x:2, y:8).echo();     /* 256 */

/* Mixed named and positional */
calculator(5, operation:"add", y:3).echo();         /* 8 */

/* Positional parameters */
calculator("multiply", 4, 6).echo();                /* 24 */
```

### Advanced String Interpolation
```grapa
/* Variable script references */
script = "xx*yy;";
"${op(yy=8)(script)()}".interpolate({xx:4}).echo();  /* 32 */

/* Multi-level parameter passing */
calc = "x * y + z;";
"${op(y=5,z=10)(calc)()}".interpolate({x:3}).echo();  /* 25 */

/* Dynamic script construction */
operation = "multiply";
script = "x " + operation + " y;";
"${op(y=5)(script)()}".interpolate({x:3}).echo();  /* 15 */
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

### String Templates and Dynamic Construction

For string-specific templates and dynamic construction patterns, see [String Templates and Dynamic Construction](type/str.md#string-templates-and-dynamic-construction) in the String Type documentation.

**Quick Examples:**
```grapa
/* Parameterized string templates */
greeting = op("name"=0, "time"=0){
    "Good " + time + ", " + name + "!"
};
greeting("Alice", "morning").echo();  /* Good morning, Alice! */

/* Dynamic string construction */
template = "name + \"! You are \" + age.str() + \" years old.\"";
name = "Alice";
age = 25;
result = op()(template)();
result.echo();  /* Alice! You are 25 years old. */
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
// Basic custom delimiter
"Line 1|Line 2|Line 3".grep("Line 2", "", "|")
["Line 2"]

// Multi-character delimiter
"Line 1|||Line 2|||Line 3".grep("Line", "o", "|||")
["Line 1", "Line 2", "Line 3"]

// Lookaround assertions with custom delimiter
"word123|text456|word789".grep("\\w+(?=\\d)", "o", "|")
["word", "text", "word"]

// Unicode script properties with custom delimiter
"Hello 世界|Goodbye 世界|Test 123".grep("\\p{sc=Latin}+", "o", "|")
["Hello", "Goodbye", "Test"]

// Grapheme clusters with custom delimiter
"Hello 👋 world 🌍|||Goodbye 👋 universe 🌌".grep("\\X", "o", "|||")
["H", "e", "l", "l", "o", " ", "👋", " ", "w", "o", "r", "l", "d", " ", "🌍", "G", "o", "o", "d", "b", "y", "e", " ", "👋", " ", "u", "n", "i", "v", "e", "r", "s", "e", " ", "🌌"]

// Word boundaries with custom delimiter
"line1|line2|line3".grep("line", "wo", "|")
["line1", "line2", "line3"]

// Multiline patterns with custom delimiter
"start|middle|end".grep("start.*end", "s", "|")
["start|middle|end"]
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

## Grammar Extension

Grapa supports dynamic language syntax extension through custom commands and functions. This powerful feature allows you to create domain-specific languages and extend the grammar at runtime.

For comprehensive documentation on syntax extension, including custom commands, custom functions, scoping, and dynamic compilation, see [Language Syntax Extension](type/rule.md#language-syntax-extension).

### Quick Example

```grapa
// Define a custom function
custom_function = rule select $INT {op(p:$2){p*5}};

// Use it directly
select 4;        // Returns 20
x = select 8;    // x = 40
```

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
- Learn about [Testing](testing.md) your Grapa code
- Check out the [Grep functionality](grep.md) for advanced pattern matching
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
