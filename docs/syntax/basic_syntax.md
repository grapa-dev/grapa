# Basic Grapa Syntax Guide

This guide covers the essential syntax patterns for writing Grapa code, based on working examples and best practices.

## Output and Echo

### Correct Echo Syntax

Use the `.echo()` method on strings, not function calls:

```grapa
/* ✅ Correct */
"Hello World".echo();
("Result: " + value.str()).echo();

/* ❌ Incorrect */
echo("Hello World");
echo("Result: " + value);
```

### String Concatenation

Always wrap string concatenation expressions in parentheses:

```grapa
/* ✅ Correct */
("Name: " + name.str() + ", Age: " + age.str()).echo();
("File size: " + size + " bytes").echo();

/* ❌ Incorrect */
"Name: " + name.str() + ", Age: " + age.str().echo();
"File size: " + size + " bytes".echo();
```

## Loops

### While Loops

Use `while` loops with explicit increment:

```grapa
/* ✅ Correct */
i = 1;
while (i <= 10) {
    ("Processing item " + i.str()).echo();
    i = i + 1;
}

/* ❌ Incorrect */
for i in 1..10 {
    echo("Processing item " + i);
}
```

### Loop Examples

```grapa
/* Count from 1 to 5 */
i = 1;
while (i <= 5) {
    ("Count: " + i.str()).echo();
    i = i + 1;
}

/* Process array elements */
i = 0;
while (i < array.len()) {
    element = array.get(i);
    ("Element " + i.str() + ": " + element.str()).echo();
    i = i + 1;
}
```

## Variable Assignment

### Basic Assignment

```grapa
/* Simple variables */
name = "John Doe";
age = 30;
salary = 75000.50;

/* Object creation */
table = $file().table("ROW");
fs = $file();
```

### Global Variables

Use `$global` for variables that need to persist across function calls:

```grapa
/* Create global variable */
$global.my_table = $file().table("ROW");

/* Use global variable */
my_table.mkfield("name", "STR", "VAR");
```

## String Operations

### String Methods

```grapa
/* Convert to string */
text = value.str();

/* String concatenation */
full_name = first_name + " " + last_name;

/* String with variables */
message = ("Hello " + name + ", you are " + age.str() + " years old");
```

### String Literals

Always use double quotes for string literals:

```grapa
/* ✅ Correct */
name = "John Doe";
message = "Hello World";

/* ❌ Incorrect */
name = John Doe;
message = Hello World;
```

## Array and Object Access

### Array Access

Arrays (`$ARRAY`) support both bracket notation and `.get()` method:

```grapa
/* ✅ Both methods work for arrays */
element = ["a", "b", "c"];
value = element[1];        /* Returns "b" */
value = element.get(1);    /* Returns "b" */
index = element["b"];      /* Returns 1 */
```

### List/Object Access

Lists (`$LIST`) support bracket notation, `.get()` method, and `.getname()`:

```grapa
/* ✅ Multiple access methods for lists */
obj = {"a": 11, "b": 22, "c": 33};
value = obj["b"];          /* Returns 22 */
value = obj.get("b");      /* Returns 22 */
value = obj[1];            /* Returns 22 (numeric index) */
name = obj.getname(1);     /* Returns "b" (key name) */
```

### $file and $TABLE Access

For `$file` and `$TABLE` objects, always use `.get()` method:

```grapa
/* ✅ Correct: Use .get() for $file and $TABLE objects */
files = fs.ls();
file_info = files.get(0);

table_data = table.get("user1", "name");
table_records = table.ls();
record = table_records.get(0);

/* ❌ Avoid bracket notation for $file and $TABLE objects */
file_info = files[0];  /* May not work reliably */
table_data = table["user1"];  /* May not work reliably */
```

### Type Checking

```grapa
/* Check object types */
element = ["a", "b", "c"];
element.type();        /* Returns $ARRAY */

obj = {"a": 11, "b": 22, "c": 33};
obj.type();            /* Returns $LIST */

table = $file().table("ROW");
table.type();          /* Returns $TABLE */
```

## Function Calls

### Method Calls

```grapa
/* File operations */
fs.set("file.txt", "content");
content = fs.get("file.txt").str();

/* Table operations */
table.mkfield("name", "STR", "VAR");
table.set("key", "value", "field");
value = table.get("key", "field").str();
```

### System Functions

```grapa
/* Time functions */
current_time = $TIME().utc();

/* File functions */
file_obj = $file();
table_obj = $file().table("ROW");
```

## Comments

### Block Comments

Use block comments for documentation:

```grapa
/* This is a block comment
   It can span multiple lines
   and is useful for documentation */

/* Single line comment */
```

### Comment Examples

```grapa
/* Create a table for user data */
$global.users = $file().table("ROW");

/* Add fields to the table */
users.mkfield("name", "STR", "VAR");  /* Variable length string */
users.mkfield("age", "INT");          /* Integer field */
users.mkfield("salary", "FLOAT", "FIX", 8);  /* Fixed float */
```

## Control Structures

### If Statements

```grapa
if (condition) {
    ("Condition is true").echo();
} else {
    ("Condition is false").echo();
}

/* Example with comparison */
if (age >= 18) {
    ("User is an adult").echo();
} else {
    ("User is a minor").echo();
}
```

### Comparison Operators

```grapa
/* Equality */
if (name == "John") { }

/* Inequality */
if (age != 0) { }

/* Greater/Less than */
if (salary > 50000) { }
if (age < 18) { }

/* Greater/Less than or equal */
if (count >= 10) { }
if (value <= 100) { }
```

## Error Handling

### Type Checking

```grapa
/* Check if result is an error */
if (result.type() == $ERR) {
    ("Operation failed").echo();
} else {
    ("Operation succeeded").echo();
}

/* Check for specific error responses */
if (result == {"error":-1}) {
    ("Operation returned error -1").echo();
}
```

## Complete Example

```grapa
/* User Management System Example */

/* Create table in global namespace */
$global.users = $file().table("ROW");

/* Define table schema */
users.mkfield("name", "STR", "VAR");
users.mkfield("age", "INT");
users.mkfield("salary", "FLOAT", "FIX", 8);

/* Add user data */
users.set("user1", "John Doe", "name");
users.set("user1", 30, "age");
users.set("user1", 75000.50, "salary");

/* Retrieve and display user data */
name = users.get("user1", "name").str();
age = users.get("user1", "age").int();
salary = users.get("user1", "salary").float();

("User: " + name + ", Age: " + age.str() + ", Salary: " + salary.str()).echo();

/* Process multiple users */
i = 1;
while (i <= 5) {
    user_id = "user" + i.str();
    users.set(user_id, ("User " + i.str()), "name");
    users.set(user_id, (20 + i), "age");
    i = i + 1;
}

("Created " + i.str() + " users").echo();
```

## Common Patterns

### File Operations

```grapa
/* File system operations */
$global.fs = $file();

/* Create and write file */
fs.set("test.txt", "Hello World");
content = fs.get("test.txt").str();
("File content: " + content).echo();

/* Directory operations */
fs.mk("test_dir");
fs.cd("test_dir");
files = fs.ls();
fs.cd("..");
fs.rm("test_dir");
```

### Table Operations

```grapa
/* Table operations */
$global.table = $file().table("ROW");

/* Create fields */
table.mkfield("name", "STR", "VAR");
table.mkfield("age", "INT");
table.mkfield("value", "FLOAT", "FIX", 8);

/* Add data */
table.set("key1", "John", "name");
table.set("key1", 30, "age");
table.set("key1", 100.5, "value");

/* Retrieve data */
name = table.get("key1", "name").str();
age = table.get("key1", "age").int();
value = table.get("key1", "value").float();
```

This guide covers the essential syntax patterns for writing correct Grapa code. Follow these patterns to avoid common syntax errors and write maintainable code. 

/*
# Grapa Basic Syntax: Key Rules

## Statement and Block Termination
- Every statement must end with a semicolon (`;`), including after the closing brace of every block (such as `if`, `else`, `while`, and function blocks).
- Example:
  while (cond) {
      ...
  };
  if (cond) {
      ...
  } else {
      ...
  };

## Array Manipulation
- To append to an array, use the `+=` operator.
  - Example:
    arr = [];
    arr += "foo";
    arr += "bar";
    // arr is now ["foo", "bar"]
- Do not use `.push()` or `.append()`—these are not valid in Grapa.
*/ 