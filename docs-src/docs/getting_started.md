---
tags:
  - user
  - highlevel
---
# Getting Started with Grapa

## Who is this for?
Anyone new to Grapa, or looking for a quick refresher on installation, basic syntax, and first steps.

## Quick Start

1. **Install Grapa** (see below for platform-specific instructions)
2. **Write your first script:**

```grapa
"Hello, Grapa!".echo();
```

3. **Run it:**
```bash
# Interactive mode
grapa

# Run a command directly
grapa -c "'Hello, Grapa!'.echo();"

# Run from a file
grapa -f "my_script.grc"
```

**See also:** [Syntax Quick Reference](syntax/basic_syntax.md)

---

## Key Syntax Rules (Read This First!)
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`) or line comments (`// ...`). `#` comments are not supported.
- Comments can be used anywhere in your code, including at the end of lines.
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- Grapa uses dynamic typing by design - types are determined at runtime for maximum flexibility.
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Installation

### Universal Installer (Recommended)
```bash
# Download and run the universal installer
curl -O https://github.com/grapa-dev/grapa/releases/download/v0.1.53/install-grapa-0.1.53.py
python3 install-grapa-0.1.53.py
```

### Manual Installation
1. **Download** the platform-specific package from [GitHub Releases](https://github.com/grapa-dev/grapa/releases)
2. **Extract** the archive and run the automated install script
3. **Verify** installation with `grapa --version`

For detailed installation instructions, see the [Installation Guide](installation.md).

> **Note:** Grapa requires macOS 15.0+ (Apple Silicon only). Intel Macs are no longer supported due to Apple's discontinuation of Intel Mac support in macOS 15.0.

---

## Your First Grapa Program

```grapa
/* Simple hello world */
"Hello, Grapa!".echo();

/* Basic arithmetic with unlimited precision */
result = 123456789012345678901234567890 * 987654321098765432109876543210;
result.echo();

/* Working with lists */
numbers = [1, 2, 3, 4, 5];
sum = numbers.reduce(op(a, b) { a + b; }, 0);
("Sum: " + sum).echo();
```

---

## Core Concepts

### Everything is Data

Grapa's most fundamental concept is that **everything is data**. Unlike traditional programming languages that distinguish between "data" and "code," Grapa treats functions, classes, primitives, and everything else as different types of data that can be manipulated, stored, and passed around.

```grapa
/* Functions are data ($OP type) */
my_function = op(x) { x * 2; };
my_function.type().echo();  /* $OP */

/* Numbers are data ($INT type) */
my_number = 42;
my_number.type().echo();    /* $INT */

/* Strings are data ($STR type) */
my_string = "hello";
my_string.type().echo();    /* $STR */

/* Classes are data ($CLASS type) */
MyClass = class { name = ""; };
MyClass.type().echo();      /* $CLASS */

/* You can store functions in variables */
functions = [op(x) { x + 1; }, op(x) { x * 2; }];
result1 = functions[0](5);  /* 6 */
result2 = functions[1](5);  /* 10 */

/* You can pass functions as parameters */
apply_func = op(func, value) { func(value); };
result = apply_func(op(x) { x * 3; }, 7);  /* 21 */
```

This unified data model enables powerful metaprogramming, code generation, and flexible data structures that would be complex in other languages.

### Dynamic Code Execution

Grapa's most powerful feature is its ability to compile and execute code at runtime:

```grapa
/* Direct string execution */
op()("'Hello, World!'.echo();")();
/* Output: Hello, World! */

/* With parameters */
func = op("name"=0)("'Hello, ' + name + '!'.echo();");
func("Grapa");
/* Output: Hello, Grapa! */

/* System-level evaluation */
result = $sys().eval("x + y", {"x": 5, "y": 3});
result.echo();  /* 8 */

/* Compiled execution for performance */
compiled = $sys().compile("result = input * 2 + offset; result");
result = $sys().eval(compiled, {"input": 10, "offset": 5});
result.echo();  /* 25 */
```

### Language Syntax Extension

Grapa's dynamic capabilities extend beyond code execution - you can actually extend the language syntax itself at runtime:

```grapa
/* Define a custom function that becomes part of the language */
$custom_function = rule select $INT {op(p:$2){p*5}};

/* Use it as if it were built into the language */
select 4;        // Returns 20
x = select 8;    // x = 40

/* Define custom commands for domain-specific languages */
$custom_command = rule reset_data {op(){clear_database()}};
reset_data;      // Executes clear_database()
```

This feature enables you to create domain-specific languages, add custom syntax for your applications, and adapt Grapa to specific use cases without modifying the core implementation.

**See also:** [Language Syntax Extension](type/rule.md#language-syntax-extension) for comprehensive documentation.

### Data Types

Grapa provides rich data types for modern data processing:

```grapa
text = "Hello, world!";
big_number = 123456789012345678901234567890;
my_list = [1, 2, 3, "hello", true];
my_array = {1, 2, 3, 4, 5};
my_table = {"name": "John", "age": 30, "city": "New York"};
my_vector = <1, 2, 3, 4, 5>;
```

### Namespace System

Grapa automatically manages namespaces for variable scoping:

```grapa
/* Global variables persist across function calls */
$global.config = {"debug": true, "timeout": 30};

/* Function creates its own local namespace */
func = op(x) {
    local_var = x * 2;  /* Local to this function */
    config.echo();      /* Access global variable */
    local_var.echo();   /* Access local variable */
};

func(5);
```

### JSON and XML Support

```grapa
json_text = '{"name": "John", "age": 30, "hobbies": ["reading", "coding"]}';
data = json_text.json();
data.name.echo();  /* "John" */
data.hobbies[0].echo();  /* "reading" */

xml_text = '<person><name>John</name><age>30</age></person>';
xml_data = xml_text.xml();
xml_data.findall("name")[0].echo();  /* "John" */
```

### Pattern Matching with Grep

```grapa
text = "Hello world\nGoodbye world\nHello again";
matches = text.grep("Hello");
matches.echo();  /* ["Hello world", "Hello again"] */

matches = text.grep("world", "o");
matches.echo();  /* ["world", "world"] */

matches = text.grep("hello", "i");
matches.echo();  /* ["Hello world", "Hello again"] */
```

---

## Common Patterns

### Data Processing

```grapa
/* Data processing example */
content = $file().getfield("data.txt");
lines = content.split("\n");
filtered = lines.filter(op(line) { line.len() > 0; });
result = filtered.map(op(line) { line.upper(); });
casefolded = filtered.map(op(line) { line.casefold(); });
result.echo();

json_data = $file().getfield("data.json").json();
users = json_data.users;
active_users = users.filter(op(user) { user.active == true; });
names = active_users.map(op(user) { user.name; });
names.echo();
```

### Mathematical Operations

```grapa
a = 123456789012345678901234567890;
b = 987654321098765432109876543210;
product = a * b;
product.echo();

v1 = <1, 2, 3>;
v2 = <4, 5, 6>;
dot_product = v1.dot(v2);
dot_product.echo();  /* 32 */
```

### String Processing

```grapa
text = "  hello world  ";
trimmed = text.trim();
upper = trimmed.upper();
casefolded = trimmed.casefold();
words = upper.split(" ");
words.echo();  /* ["HELLO", "WORLD"] */

text = "Hello world, goodbye world";
matches = text.grep("world", "o");
matches.echo();  /* ["world", "world"] */
```

---

## Automatic File Loading

**Important**: This is **runtime loading** - files are loaded when the function or class is first called, not at compile time. This differs from the `include` command which loads files at compile time.

Grapa includes a powerful automatic file loading mechanism that makes it easy to organize and reuse code:

### Creating Reusable Functions

```grapa
/* File: lib/grapa/utils.grc */
$global["utils"] = class {
    format_date = op(date) {
        return date.format("YYYY-MM-DD");
    };
    
    validate_email = op(email) {
        return email.grep(r"^[^@]+@[^@]+\.[^@]+$", "x").len() > 0;
    };
};
```

### Using Functions Automatically

```grapa
/* Grapa automatically loads utils.grc when you call utils() */
formatted = utils().format_date($TIME());
is_valid = utils().validate_email("user@example.com");
```

### Search Paths

Grapa automatically searches for `.grc` and `.grz` files in this order:

1. **Current working directory** - Where you run the `grapa` command
2. **Library directory** - Determined by this priority:
   - `{current_working_directory}/lib/grapa` (development/local)
   - `/usr/lib/grapa` (system-wide, package manager)
   - `/usr/local/lib/grapa` (system-wide, user-installed)
   - `{binary_directory}/lib/grapa` (fallback, relative to grapa executable)
3. **Static library** - Built-in compressed library files

> **Tip:** Place your utility functions in `lib/grapa/` in your project directory for automatic discovery. No import statements needed!

---

## Integration Options

### C++ Integration

Grapa can be embedded in C++ applications using the universal development kit:

```cpp
#include "grapa/GrapaLink.h"

int main() {
    // Initialize Grapa runtime
    GrapaLink grapa;
    
    // Execute Grapa code
    GrapaValue result = grapa.eval("2 + 2");
    std::cout << "Result: " << result << std::endl;
    
    return 0;
}
```

**Build with CMake:**
```bash
# After extracting the platform-specific package
cd grapa-0.1.53
mkdir build && cd build
cmake ..
cmake --build .
./grapa_example
```

The universal development kit includes:
- Complete C++ API headers
- Sample application (`main.cpp`)
- CMake build system
- All platform libraries

**See also:** [C++ Integration Guide](cpp_integration.md) for comprehensive C++ embedding documentation and [Installation Guide](installation.md) for the complete development workflow.

### Python Integration

Grapa can be used from Python via GrapaPy:

```python
import grapapy

# Create Grapa instance
g = grapapy.grapa()

# Execute Grapa code
result = g.eval('2 + 2')
print(f'Result: {result}')
```

**Install GrapaPy:**
```bash
pip install grapapy
```

## Next Steps
- Explore [Examples](examples.md) for more complex use cases
- Learn about [Testing](testing.md) your Grapa code
- Dive into advanced system functions for advanced features
- Check out the [Grep functionality](grep.md) for advanced pattern matching
- Review the [Syntax Quick Reference](syntax/basic_syntax.md) for more syntax rules and tips
- Learn about [Command Line Arguments](sys/sys.md#command-line-arguments) for CLI script development 

> **Tip:** Use `.range()` to generate arrays of numbers: `(n).range(start, step)`. 

> **Comment Style:**
> - Block comments (`/* ... */`) and line comments (`// ...`) are both supported in Grapa.
> - Comments can be used anywhere in your code, including at the end of lines.
> - `#` comments are not supported and will cause syntax errors.

> **Did you know?**
> Grapa's parallel ETL/data processing is a core, production-ready feature. Parallelism is well tested and ideal for real-world, high-throughput data tasks. 

> **For Newcomers/Non-Python Users:**
> - [JS-to-Grapa Migration Guide](migrations/js_to_grapa_migration.md)
> - [Language Reference](function_quick_reference.md)
> - [Examples](examples.md)

> **For Python Users:**
> See the [Python Integration Guide](python_integration.md), [Migration Tips for Python Users](python_use_cases.md#migration-tips-for-python-users), and [GrapaPy Namespace Solution](grapapy_namespace_solution.md) for essential guidance on using Grapa from Python.

> **For C++ Developers:**
> See the [C++ Integration Guide](cpp_integration.md) for comprehensive documentation on embedding Grapa in C++ applications, including performance optimization, error handling, and advanced use cases. The [Installation Guide](installation.md) provides the complete development workflow with CMake build system. 