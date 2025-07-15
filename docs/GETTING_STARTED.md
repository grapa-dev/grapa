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
grapa -ccmd "'Hello, Grapa!'.echo();"

# Run from a file
grapa -cfile "my_script.grc"
```

**See also:** [Syntax Quick Reference](syntax/basic_syntax.md)

---

## Key Syntax Rules (Read This First!)
- Every statement and every block (including after closing braces) must end with a semicolon (`;`).
- Use block comments (`/* ... */`), not line comments (`// ...`).
- To append to arrays, use the `+=` operator (not `.push()` or `.append()`).
- See [Syntax Quick Reference](syntax/basic_syntax.md) for more.

---

## Installation

1. **Download** the appropriate binary for your platform from the releases
2. **Extract** the archive to a directory
3. **Add** the directory to your PATH (optional but recommended)

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
content = $file().read("data.txt");
lines = content.split("\n");
filtered = lines.filter(op(line) { line.len() > 0; });
result = filtered.map(op(line) { line.upper(); });
result.echo();

json_data = $file().read("data.json").json();
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
words = upper.split(" ");
words.echo();  /* ["HELLO", "WORLD"] */

text = "Hello world, goodbye world";
matches = text.grep("world", "o");
matches.echo();  /* ["world", "world"] */
```

---

## Next Steps
- Explore [Examples](EXAMPLES.md) for more complex use cases
- Learn about [Testing](TESTING.md) your Grapa code
- Dive into [System Functions](SYSTEM_FUNCTIONS.md) for advanced features
- Check out the [Grep functionality](GREP.md) for advanced pattern matching
- Review the [Syntax Quick Reference](syntax/basic_syntax.md) for more syntax rules and tips 