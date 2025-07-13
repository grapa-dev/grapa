# Getting Started with Grapa

## Quick Start

Grapa is a high-level language designed for data processing and ETL workflows. Here's how to get started:

### Installation

1. **Download** the appropriate binary for your platform from the releases
2. **Extract** the archive to a directory
3. **Add** the directory to your PATH (optional but recommended)

### Your First Grapa Program

```grapa
// Simple hello world
"Hello, Grapa!".echo()

// Basic arithmetic with unlimited precision
result = 123456789012345678901234567890 * 987654321098765432109876543210
result.echo()

// Working with lists
numbers = [1, 2, 3, 4, 5]
sum = numbers.reduce(op(a, b) { a + b }, 0)
("Sum: " + sum).echo()
```

### Running Grapa Code

```bash
# Interactive mode
grapa

# Run a command directly
grapa -ccmd "'Hello World'.echo()"

# Run from a file
grapa -cfile "my_script.grc"
```

## Core Concepts

### Data Types

Grapa provides rich data types for modern data processing:

```grapa
// Strings
text = "Hello, world!"

// Numbers (unlimited precision)
big_number = 123456789012345678901234567890

// Lists
my_list = [1, 2, 3, "hello", true]

// Arrays (fixed-size)
my_array = {1, 2, 3, 4, 5}

// Tables (key-value pairs)
my_table = {"name": "John", "age": 30, "city": "New York"}

// Vectors (mathematical vectors)
my_vector = <1, 2, 3, 4, 5>
```

### JSON and XML Support

Grapa has built-in support for JSON and XML:

```grapa
// Parse JSON
json_text = '{"name": "John", "age": 30, "hobbies": ["reading", "coding"]}'
data = json_text.json()
data.name.echo()  // "John"
data.hobbies[0].echo()  // "reading"

// Parse XML
xml_text = '<person><name>John</name><age>30</age></person>'
xml_data = xml_text.xml()
xml_data.findall("name")[0].echo()  // "John"
```

### Pattern Matching with Grep

Grapa includes powerful pattern matching capabilities:

```grapa
// Basic pattern matching
text = "Hello world\nGoodbye world\nHello again"
matches = text.grep("Hello")
matches.echo()  // ["Hello world", "Hello again"]

// Match-only output
matches = text.grep("world", "o")
matches.echo()  // ["world", "world"]

// Case-insensitive matching
matches = text.grep("hello", "i")
matches.echo()  // ["Hello world", "Hello again"]
```

## Common Patterns

### Data Processing

```grapa
// Read and process a file
content = $file().read("data.txt")
lines = content.split("\n")
filtered = lines.filter(op(line) { line.len() > 0 })
result = filtered.map(op(line) { line.upper() })
result.echo()

// Process JSON data
json_data = $file().read("data.json").json()
users = json_data.users
active_users = users.filter(op(user) { user.active == true })
names = active_users.map(op(user) { user.name })
names.echo()
```

### Mathematical Operations

```grapa
// Unlimited precision arithmetic
a = 123456789012345678901234567890
b = 987654321098765432109876543210
product = a * b
product.echo()

// Vector operations
v1 = <1, 2, 3>
v2 = <4, 5, 6>
dot_product = v1.dot(v2)
dot_product.echo()  // 32
```

### String Processing

```grapa
// String manipulation
text = "  hello world  "
trimmed = text.trim()
upper = trimmed.upper()
words = upper.split(" ")
words.echo()  // ["HELLO", "WORLD"]

// Pattern matching
text = "Hello world, goodbye world"
matches = text.grep("world", "o")
matches.echo()  // ["world", "world"]
```

## Next Steps

- Explore [Examples](EXAMPLES.md) for more complex use cases
- Learn about [Testing](TESTING.md) your Grapa code
- Dive into [System Functions](SYSTEM_FUNCTIONS.md) for advanced features
- Check out the [Grep functionality](GREP.md) for advanced pattern matching

## Getting Help

- Use the interactive mode to experiment: `grapa`
- Check the [Function Quick Reference](FUNCTION_QUICK_REFERENCE.md) for common functions
- Review [Examples](EXAMPLES.md) for practical patterns
- Explore the [type documentation](type/) for detailed type information 