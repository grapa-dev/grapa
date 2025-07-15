# Grapa Examples

This document provides practical examples of Grapa usage, organized from basic to advanced.

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
- [Pattern Matching (Grep)](#grep---pattern-matching)
- [Cryptography](#crypt)
- [Advanced Patterns](#advanced-patterns)

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

## Data Processing

### String Word Length
The following returns the length of each word in a string:
```grapa
"this is a test".split(" ").reduce(op(a,b){a+=b.len();},[])
[4,2,1,4]
```

### File Processing
```grapa
// Read and process a file
content = $file().read("data.txt")
lines = content.split("\n")
filtered = lines.filter(op(line) { line.len() > 0 })
result = filtered.map(op(line) { line.upper() })
result.echo()
```

### JSON Processing
```grapa
// Parse and process JSON data
json_data = $file().read("data.json").json()
users = json_data.users
active_users = users.filter(op(user) { user.active == true })
names = active_users.map(op(user) { user.name })
names.echo()
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
