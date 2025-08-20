# From $OBJ

## len()
Gets the length of the item.

```
"hi".len() -> 2
{1,2,3}.len() -> 3
```

## left(count)
Gets the left bytes of an item.

```
"testing".left(2) -> "te"
```

Use a negative number to truncate right.

```
"testing".left(-2) -> "testi"
```

## right(count)
Gets the right bytes of an item.

```
"testing".right(2) -> "ng"
```

Use a negative number to truncate left.

```
"testing".right(-2) -> "sting"
```

## mid(start,len)
Gets the middle bytes of an item.

```
"testing".mid(2,3) -> "sti"
```

## midtrim(items, offset, blocksize)

```
items = array of [label, offset, len, ltrim, rtrim, op]

"this is a test to see".midtrim([["a",2,1," "," "],["b",10,5," "," ",op(a){a.len();}]],1,13);
{"a":"s","b":3}
```

## rtrim([chars])
Trims characters from the right side of a string.

### Parameters:
- `chars` (optional) - Character(s) to trim. Can be:
  - **Single character**: `"x"` - trims that specific character
  - **String**: `"xyz"` - trims that specific string pattern
  - **Array**: `[" ", "\t", "\n", "\r"]` - trims any of the characters in the array
  - **Omitted**: defaults to space `" "`

### Examples:
```grapa
/* Default: trim spaces */
"  testing  ".rtrim() -> "  testing"

/* Single character */
"bbbtestingbbb".rtrim("b") -> "bbbtesting"

/* Multiple whitespace characters */
"  \t\n\rhello world  ".rtrim([" ", "\t", "\n", "\r"]) -> "  \t\n\rhello world"

/* String pattern */
"helloworldworld".rtrim("world") -> "hello"
```

## ltrim([chars])
Trims characters from the left side of a string.

### Parameters:
- `chars` (optional) - Character(s) to trim. Can be:
  - **Single character**: `"x"` - trims that specific character
  - **String**: `"xyz"` - trims that specific string pattern
  - **Array**: `[" ", "\t", "\n", "\r"]` - trims any of the characters in the array
  - **Omitted**: defaults to space `" "`

### Examples:
```grapa
/* Default: trim spaces */
"  testing  ".ltrim() -> "testing  "

/* Single character */
"bbbtestingbbb".ltrim("b") -> "testingbbb"

/* Multiple whitespace characters */
"  \t\n\rhello world  ".ltrim([" ", "\t", "\n", "\r"]) -> "hello world  "

/* String pattern */
"worldworldhello".ltrim("world") -> "hello"
```

## trim([chars])
Trims characters from both left and right sides of a string.

### Parameters:
- `chars` (optional) - Character(s) to trim. Can be:
  - **Single character**: `"x"` - trims that specific character
  - **String**: `"xyz"` - trims that specific string pattern
  - **Array**: `[" ", "\t", "\n", "\r"]` - trims any of the characters in the array
  - **Omitted**: defaults to space `" "`

### Examples:
```grapa
/* Default: trim spaces */
"  testing  ".trim() -> "testing"

/* Single character */
"bbbtestingbbb".trim("b") -> "testing"

/* Multiple whitespace characters */
"  \t\n\rhello world  ".trim([" ", "\t", "\n", "\r"]) -> "hello world"

/* String pattern */
"worldworldhelloworldworld".trim("world") -> "hello"

/* Common whitespace trimming */
whitespace = [" ", "\t", "\n", "\r"];
"  \t\n\r  hello world  \t\n\r  ".trim(whitespace) -> "hello world"
```

## lpad(n,[str])
Pads left to bring the total size up to n characters. Defaults to pad with a space, but will use str for padding if provided. 

Will left truncate input if length of input is less than n.

```
"test".lpad(7,"X") -> "XXXtest"
```

## rpad(n,[str])
Pads left to bring the total size up to n characters. Defaults to pad with a space, but will use str for padding if provided. 

Will right truncate input if length of input is less than n.

```
"test".rpad(7,"X") -> "testXXX"
```

## lrot([n])
For $LIST, $ARRAY, $XML.

Moves n (defaul=1) items from the start of the list to the end of the list, 1 at a time.

```
["a","b","c","d","e"].lrot(2) -> ["c","d","e","a","b"]
```

## rrot([n])
For $LIST, $ARRAY, $XML.

Moves n (defaul=1) items from the end of the list to the start of the list, 1 at a time.

```
["a","b","c","d","e"].rrot(2) -> ["d","e","a","b","c"]
```

## reverse()
Reverses the older of a list.

```
{z:1,m:2,p:3,b:4}.reverse() -> {"b":4,"p":3,"m":2,"z":1}
"testing".reverse() -> "gnitset"
```

## replace(old,new)
Replaces iteems.

```
"testing".replace("t","g") -> "gesging"
```

## interpolate([params])
Performs string interpolation with embedded expressions and script execution. Supports variable substitution, expression evaluation, and dynamic script execution using the powerful `op()` function infrastructure.

### Parameters:
- `params` (optional) - `$LIST` type providing additional variables for interpolation context

### Template Syntax:
- `${code}` - Direct code execution within current scope
- `${op()("script")()}` - **PRIMARY APPROACH** - Script execution using existing `op()` function infrastructure
- `${op(params)("script")()}` - Script execution with parameters using `op()` function
- `${op(params)(script_var)()}` - **ADVANCED** - Dynamic script execution with variable references

### What Can Be Used Inside `${}`:
- **Variables**: `${name}`, `${age}`, `${user.id}`
- **Expressions**: `${x + y}`, `${price * 1.08}`, `${count > 0}`
- **Function calls**: `${get_data()}`, `${calculate(a, b)}`
- **Method calls**: `${text.upper()}`, `${array.len()}`
- **Object literals**: `${{name:"John", age:30}}`
- **Array literals**: `${[1, 2, 3]}`
- **Array operations**: `${data.filter(op(x){x > 0})}`
- **Nested expressions**: `${users.map(op(u){u.name}).join(", ")}`
- **Conditional expressions**: `${status == "ok" ? "Success" : "Failed"}`



### Examples:

```grapa
/* Basic variable interpolation */
name = "Alice";
age = 25;
"Hello ${name}, you are ${age} years old!".interpolate();
/* → "Hello Alice, you are 25 years old!" */

/* Expression evaluation */
"Sum: ${1 + 2}, Product: ${3 * 4}".interpolate();
/* → "Sum: 3, Product: 12" */

/* Complex expressions with variables */
x = 10;
y = 5;
"Sum: ${x + y}, Product: ${x * y}".interpolate();
/* → "Sum: 15, Product: 50" */

/* Script execution with op() function */
x = 10;
"Value: ${op()(\"x\")()}".interpolate();
/* → "Value: 10" */

/* Parameterized script execution */
x = 10;
"Value: ${op(v=x)(\"v + 2\")()}".interpolate();
/* → "Value: 12" */

/* Variable script references - ADVANCED FEATURE */
script = "xx*yy;";
"${op(yy=8)(script)()}".interpolate({xx:4});
/* → "32" */

/* Multi-level parameter passing - ADVANCED FEATURE */
calc = "x * y + z;";
"${op(y=5,z=10)(calc)()}".interpolate({x:3});
/* → "25" */

/* Template string interpolation with parameters */
"Hello ${name}".interpolate({name:"Alice"});
/* → "Hello Alice" */

/* Method chaining */
"Hello ${name}".interpolate({name:"Alice"}).upper();
/* → "HELLO ALICE" */

/* Direct object syntax */
"Object: ${{a:1,b:2}}".interpolate();
/* → "Object: {"a":1,"b":2}" */

/* Nested objects */
"Nested: ${{a:{b:1},c:2}}".interpolate();
/* → "Nested: {"a":{"b":1},"c":2}" */

/* Function calls inside ${} */
get_user = op() { {name:"John", age:30}; };
"User: ${get_user()}".interpolate();
/* → "User: {"name":"John","age":30}" */

/* Function calls with parameters */
multiply = op(x, y) { x * y; };
"Result: ${multiply(5, 3)}".interpolate();
/* → "Result: 15" */

/* Array operations */
"Numbers: ${[1,2,3,4].filter(op(x){x > 2})}".interpolate();
/* → "Numbers: [3,4]" */

/* String methods */
"Uppercase: ${'hello world'.upper()}".interpolate();
/* → "Uppercase: HELLO WORLD" */
```

### Advanced Features:

#### Multi-Level Parameter Passing
Parameters can be passed at both op-level and interpolation-level:
- **Op-level**: `op(yy=8)` - Parameters passed to script execution
- **Interpolation-level**: `{xx:4}` - Parameters available in interpolation context

#### Variable Script References
Scripts stored in variables enable dynamic execution:
```grapa
script = "xx*yy;";
"${op(yy=8)(script)()}".interpolate({xx:4});
/* → "32" */
```

#### Template String Interpolation
Classic template functionality with parameter substitution:
```grapa
template = "Hello ${name}, welcome to ${city}!";
template.interpolate({name:"Alice", city:"New York"});
/* → "Hello Alice, welcome to New York!" */
```

#### Dynamic Script Construction
Programmatic script building and execution:
```grapa
operation = "multiply";
script = "x " + operation + " y;";
"${op(y=5)(script)()}".interpolate({x:3});
/* → "15" */
```

### Enterprise Use Cases:

#### Template Engines
```grapa
email_template = "Dear ${name},\n\nYour order #${order_id} has been shipped.\n\nBest regards,\n${company}";
email_template.interpolate({name:"John", order_id:"12345", company:"GrapaCorp"});
```

#### Configuration Systems
```grapa
config_template = "database.host=${db_host}\ndatabase.port=${db_port}";
config_template.interpolate({db_host:"localhost", db_port:"5432"});
```

#### Code Generation
```grapa
class_template = "class ${class_name} {\n  private ${field_type} ${field_name};\n}";
class_template.interpolate({class_name:"User", field_type:"String", field_name:"name"});
```

### Performance Considerations:
- **Variable interpolation**: Fastest for simple variable substitution
- **Expression evaluation**: Good for mathematical and logical expressions
- **Script execution**: More overhead, use for complex logic
- **Dynamic script references**: Highest overhead, use for programmatic script construction

> **Note**: The interpolate method is enterprise-ready and supports complex template systems, configuration management, and code generation workflows.

## grep(pattern, options, delimiter, normalization, mode, num_workers) 
Extracts matches from a string using PCRE2-powered regular expressions with full Unicode support. Returns an array of results or JSON format with named groups.

> **For comprehensive Unicode, advanced regex, diacritic-insensitive, and output option documentation, see [Unicode Grep Documentation](../grep.md).**

### Parameters:

- `pattern` — PCRE2 regular expression string with Unicode support, named groups, and advanced features.
- `options` — Combination of the following flags:

  **Matching Options:**
  - `a` – All mode: treat the entire input as one block (no line splitting).
  - `i` – Case-insensitive match with Unicode case folding.
  - `d` – Diacritic-insensitive match (strip accents/diacritics from both input and pattern, robust Unicode-aware).
  - `v` – Invert match (select non-matching lines or spans).
  - `x` – Match entire line exactly (equivalent to anchoring with `^` and `$`).
  - `N` – Normalize input and pattern to NFC Unicode form.

  **Output Options:**
  - `o` – Output only matched substrings.
  - `n` – Prefix matches with line number.
  - `l` – Return only matching line numbers.
  - `b` – Prefix results with byte offset.
  - `j` – JSON output format with named groups, offsets, and line numbers.

  **Processing Options:**
  - `c` – Return count of matches (or count of deduplicated matches if `d` is also set).
  - `d` – Deduplicate results (line-level by default, or substring-level when combined with `o`, `g`, or `b`).
  - `g` – Group matches per line.

  **Parallel Processing:**
  - `num_workers` – Number of worker threads: `0` for auto-detection, `1` for sequential, `2+` for parallel processing.

  **Unicode Support:**
  - Unicode categories: `\p{L}`, `\p{N}`, `\p{Z}`, `\p{P}`, `\p{S}`, `\p{C}`, `\p{M}`
  - Unicode scripts: `\p{sc=Latin}`, `\p{sc=Han}`, etc.
  - Unicode script extensions: `\p{scx:Han}`, etc.
  - Unicode general categories: `\p{Lu}`, `\p{Ll}`, etc.
  - Named groups: `(?P<name>...)`
  - Atomic groups: `(?>...)`
  - Lookaround assertions: `(?=...)`, `(?<=...)`, `(?!...)`, `(?<!...)`
  - Unicode grapheme clusters: `\X`
  - Advanced Unicode properties: `\p{Emoji}`, `\p{So}`, etc.
  - Possessive quantifiers: `*+`, `++`, `?+`, `{n,m}+`
  - Conditional patterns: `?(condition)...`
  - Context lines: `A`, `B`, `C` options

  **Not Supported:**
  - Unicode blocks: `\p{In_Basic_Latin}`, etc.
  - Unicode age properties: `\p{Age=...}`
  - Unicode bidirectional classes: `\p{Bidi_Class:...}`

### Examples:

```grapa
/* Basic pattern matching */
"apple 123 pear 456\nbanana 789".grep("\\d+", "o");
/* → ["123", "456", "789"] */

/* With line numbers */
"apple 123 pear 456\nbanana 789".grep("\\d+", "on");
/* → ["1:123", "1:456", "2:789"] */

/* Unicode support */
"Hello 世界 123 €".grep("\\p{L}+", "o");
/* → ["Hello", "世界"] */

/* Named groups with JSON output */
"John Doe".grep("(?P<first>\\w+) (?P<last>\\w+)", "oj");
/* → [{"match":"John Doe","first":"John","last":"Doe","offset":0,"line":1}] */

/* Date parsing with JSON output */
"2023-04-27\n2022-12-31".grep("(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})", "oj");
/* → [
    {"match":"2023-04-27","year":"2023","month":"04","day":"27","offset":0,"line":1},
    {"match":"2022-12-31","year":"2022","month":"12","day":"31","offset":11,"line":2}
  ] */

/* Raw string literals for better readability */
"file.txt".grep("^[a-zA-Z0-9_]+\\.txt$", "x");
/* → ["file.txt"] - No need to escape backslashes */

"user@domain.com".grep("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "x");
/* → ["user@domain.com"] - Much cleaner than escaped version */

/* Raw strings preserve literal escape sequences */
"\\x45".grep("\\x45", "o");
/* → ["\\x45"] - Literal string, not character "E" */

/* Context lines */
"Line 1\nLine 2\nLine 3\nLine 4".grep("Line 2", "A1B1");
/* → ["Line 1", "Line 2", "Line 3"] */

/* Unicode normalization (NFC) */
"café".grep("cafe", "o", "", "NFC");
/* → ["café"] */

/* Binary mode for raw byte processing */
"\\x48\\x65\\x6c\\x6c\\x6f".grep("Hello", "o", "", "NONE", "BINARY");
/* → ["Hello"] */

/* Custom delimiter examples */
"apple|||pear|||banana".grep("\\w+", "o", "|||");
/* → ["apple", "pear", "banana"] */

"section1###section2###section3".grep("section\\d+", "o", "###");
/* → ["section1", "section2", "section3"] */
```

> **📖 For comprehensive Unicode grep documentation including advanced features, named groups, JSON output, and Unicode properties, see [Unicode Grep Documentation](../grep.md).**

> **💡 Tip**: Use raw string literals (prefix with `r`) for better regex pattern readability. For example, `r"\w+"` instead of `"\\w+"`. Raw strings suppress all escape sequences except for escaping the quote character used to enclose the string.

## match(pattern, options, delimiter, normalization, mode, num_workers)
Tests if a pattern matches in a string, returning a boolean value.

**Returns**: `$BOOL` - `true` if pattern is found, `false` if not found or invalid pattern

### Parameters:
- `pattern` - Regex pattern to search for
- `options` (optional) - Search options (same as grep)
- `delimiter` (optional) - Line delimiter for multiline matching
- `normalization` (optional) - Unicode normalization form
- `mode` (optional) - Processing mode
- `num_workers` (optional) - Number of parallel workers

### Examples:
```grapa
/* Basic pattern matching */
"hello world".match("hello");     /* true - pattern found */
"hello world".match("xyz");       /* false - pattern not found */
"hello world".match("a{");        /* false - invalid pattern handled gracefully */

/* Case insensitive matching */
"hello world".match("HELLO", "i"); /* true - case insensitive match */
"Hello World".match("world", "i"); /* true - case insensitive match */

/* With regex patterns */
"hello world".match(r"\w+");      /* true - word characters found */
"hello world".match(r"\d+");      /* false - no digits found */

/* With all parameters */
"hello world".match("hello", "i", "", "", "", 1); /* true - with all options */
```

### When to Use match() vs grep():
- **Use `.match()`** when you only need to know if a pattern exists (boolean result)
- **Use `.grep()`** when you need the actual matches or match details

### Error Handling:
- Invalid regex patterns return `false` (graceful degradation)
- All other errors from grep are propagated as `$ERR`

---

### Diacritic-Insensitive Matching (`d` option)

The `d` option enables **diacritic-insensitive matching**. When enabled, both the input and the pattern are:
1. **Unicode normalized** (NFC by default, or as specified)
2. **Case folded** (Unicode-aware, not just ASCII)
3. **Diacritics/accents are stripped** (works for Latin, Greek, Cyrillic, Turkish, Vietnamese, and more)

This allows matches like:
- `"café".grep("cafe", "d")` → `["café"]`
- `"CAFÉ".grep("cafe", "di")` → `["CAFÉ"]`
- `"mañana".grep("manana", "d")` → `["mañana"]`
- `"İstanbul".grep("istanbul", "di")` → `["İstanbul"]`
- `"καφές".grep("καφες", "d")` → `["καφές"]`
- `"кофе".grep("кофе", "di")` → `["кофе"]`

#### Special Capabilities
- Handles both precomposed (NFC) and decomposed (NFD) Unicode forms
- Supports diacritic-insensitive matching for Latin, Greek, Cyrillic, Turkish, Vietnamese, and more
- Works with case-insensitive (`i`) and normalization (`N`, or normalization parameter) options
- Robust for international text, including combining marks

#### Limitations
- Only covers scripts and diacritics explicitly mapped (Latin, Greek, Cyrillic, Turkish, Vietnamese, etc.)
- Does **not** transliterate between scripts (e.g., Greek to Latin)
- Does **not** remove all possible Unicode marks outside supported ranges (e.g., rare/archaic scripts)
- For full Unicode normalization, use with the normalization parameter (e.g., `"NFC"`, `"NFD"`)
- Does **not** perform locale-specific collation (e.g., German ß vs ss)

#### Example
```grapa
input = "café\nCAFÉ\ncafe\u0301\nCafe\nCAFÉ\nmañana\nmañana\nİstanbul\nistanbul\nISTANBUL\nstraße\nSTRASSE\nStraße\nкофе\nКофе\nκαφές\nΚαφές\n";
result = input.grep(r"cafe", "di");
/* Result: ["café", "CAFÉ", "café", "Cafe", "CAFÉ"] */
```

## split(sep, max, axis)
Splits into an array.

```
"one\ntwo\nthree".split("\n") -> ["one","two","three"]
"this is a test".split(" ") -> ["this","is","a","test"]
"this is a test split into parts".split(3) -> ["this is a t","est split i","nto parts"]
"this is a test split into parts".split(" ", 3) -> ["this is a test ","split into ","parts"]
```

---

## String Distance Functions

Grapa provides three string distance algorithms for fuzzy matching, spell checking, and similarity analysis. Each algorithm is optimized for different use cases:

> **📋 Current Implementation Status**: 
> - **Levenshtein**: ✅ **Fully working** - Correctly calculates edit distance for all string comparisons
> - **Jaro-Winkler**: ✅ **Fully working** - Correctly calculates similarity scores for all string comparisons
> - **Cosine Similarity**: ✅ **Fully working** - Uses word-based frequency analysis for document similarity
> 
> All three algorithms are now functional and provide accurate results for their respective use cases.

### levenshtein(other, options)
Calculates the **edit distance** between two strings - the minimum number of single-character edits (insertions, deletions, substitutions) needed to transform one string into another.

**Returns**: `$INT` - Distance value (0 = identical, higher = more different)

**Best for**: Spell checking, fuzzy matching, finding similar words

**Examples**:
```grapa
"kitten".levenshtein("sitting")  /* Returns: 3 (k→s, e→i, add g) */
"hello".levenshtein("world")     /* Returns: 4 (h→w, e→o, l→r, o→l) */
"hello".levenshtein("hello")     /* Returns: 0 (identical) */
"book".levenshtein("back")       /* Returns: 2 (o→a, o→c) */
```

**When to use Levenshtein**:
- ✅ **Spell checking** - Find closest matches to misspelled words
- ✅ **Fuzzy search** - Find items with similar names
- ✅ **Data cleaning** - Identify potential duplicates with typos
- ✅ **Short strings** - Words, names, codes (typically < 50 characters)

### jarowinkler(other, options)
Calculates **Jaro-Winkler similarity** - optimized for short strings like names, with bonus for matching prefixes.

**Returns**: `$FLOAT` - Similarity score (0.0 = completely different, 1.0 = identical)

**Best for**: Name matching, short string similarity, person names

**Examples**:
```grapa
"martha".jarowinkler("marhta")   /* Returns: 0.0 (low similarity) */
"hello".jarowinkler("world")     /* Returns: 0.0 (low similarity) */
"hello".jarowinkler("hello")     /* Returns: 1.0 (identical) */
"abc".jarowinkler("abc")         /* Returns: 1.0 (identical) */
```

**When to use Jaro-Winkler**:
- ✅ **Name matching** - Person names, company names
- ✅ **Short strings** - Typically 2-10 characters
- ✅ **Prefix importance** - When matching prefixes is valuable
- ✅ **Exact matches** - High similarity for identical strings

### cosinesimilarity(other, options)
Calculates **cosine similarity** using vector space model - treats strings as word vectors and measures the angle between them.

**Returns**: `$FLOAT` - Similarity score (0.0 = completely different, 1.0 = identical)

**Best for**: Document similarity, longer texts, semantic similarity

**Examples**:
```grapa
"hello world".cosinesimilarity("hello there")  /* Returns: 0.0 (low similarity) */
"hello world".cosinesimilarity("hello world")  /* Returns: 1.0 (identical) */
"hello world".cosinesimilarity("world hello")  /* Returns: 0.0 (low similarity) */
"abc".cosinesimilarity("abc")                  /* Returns: 1.0 (identical) */
```

**When to use Cosine Similarity**:
- ✅ **Document similarity** - Comparing longer texts
- ✅ **Word-based analysis** - When word order doesn't matter much
- ✅ **Longer strings** - Sentences, paragraphs, documents
- ⚠️ **Uses word frequency** - Current implementation uses simple word frequency (not TF-IDF)
- 📋 **Future enhancement** - TF-IDF implementation planned for better accuracy

### Algorithm Selection Guide

| Use Case | Recommended Algorithm | Reason |
|----------|----------------------|--------|
| **Spell checking** | Levenshtein | Measures actual edit distance |
| **Fuzzy search** | Levenshtein | Most intuitive distance metric |
| **Short codes/IDs** | Levenshtein | Precise edit distance |
| **Name matching** | Jaro-Winkler | Optimized for short strings with prefix bonus |
| **Document similarity** | Cosine Similarity | Word-based analysis for longer texts |
| **Exact string matching** | Any algorithm | All return high similarity for identical strings |

### Performance Characteristics

| Algorithm | Time Complexity | Space Complexity | Best For String Length |
|-----------|----------------|------------------|----------------------|
| **Levenshtein** | O(m×n) | O(m×n) | < 100 characters |
| **Jaro-Winkler** | O(n²) | O(n) | < 20 characters |
| **Cosine Similarity** | O(n) | O(n) | Any length |

### Implementation Notes

#### Current Status ✅ **FULLY IMPLEMENTED**
- **Cosine Similarity**: Now supports both word frequency and TF-IDF approaches
  - **Auto-selection**: Automatically chooses TF-IDF when corpus is provided, word frequency otherwise
  - **Manual selection**: Can force specific method using `{method: "word_freq"}` or `{method: "tfidf"}`
  - **TF-IDF**: Uses corpus-based IDF calculation with smoothing to avoid log(0)
  - **PTR Handling**: Properly handles Grapa's internal PTR types for corpus data
- **Case Sensitivity**: Supports case-insensitive comparison via `{case_sensitive: false}`
- **Options Parameter**: Fully implemented with auto-selection logic and robust parsing

#### Auto-Selection Logic
```grapa
// No options → word frequency (backward compatible)
"hello world".cosinesimilarity("hello there")

// With corpus → automatically uses TF-IDF
"hello world".cosinesimilarity("hello there", {corpus: my_documents})

// Force specific method
"hello world".cosinesimilarity("hello there", {method: "word_freq"})
"hello world".cosinesimilarity("hello there", {method: "tfidf", corpus: my_documents})

// Case-insensitive comparison
"Hello World".cosinesimilarity("hello world", {case_sensitive: false})
```

#### When to Use Each Implementation
- **Word Frequency**: Use for small, focused datasets where common words are meaningful
- **TF-IDF**: Use for general-purpose text analysis with larger corpora to reduce noise from high-frequency words
- **Rule of Thumb**: For serious text analysis beyond toy data, TF-IDF is almost always better, but requires a meaningful corpus

#### Future Enhancements (Optional)
- **Additional algorithms**: Hamming distance, N-gram similarity, weighted edit distance
- **Performance optimizations**: Caching for large corpora, parallel processing
- **Advanced options**: Custom word weighting, stop word filtering, stemming

### Options Parameter (Future Enhancement)

All three functions will support an optional `options` parameter for advanced configuration:

```grapa
/* Case-insensitive comparison (planned) */
"Hello".levenshtein("hello", {case_sensitive: false})  /* Returns: 0 */
"Hello".jarowinkler("hello", {case_sensitive: false})  /* Returns: 1.0 */
"Hello".cosinesimilarity("hello", {case_sensitive: false})  /* Returns: 1.0 */
```

### Real-World Examples

```grapa
/* Spell checking */
words = ["hello", "world", "grapa", "programming"];
user_input = "helo";
closest = words.map(word => ({word: word, distance: word.levenshtein(user_input)}))
               .sort((a, b) => a.distance - b.distance)[0];
/* Result: {word: "hello", distance: 1} */

/* Name matching with Jaro-Winkler */
names = ["John Smith", "Jane Doe", "Bob Johnson"];
search_name = "Jon Smith";
matches = names.map(name => ({name: name, similarity: name.jarowinkler(search_name)}))
              .filter(match => match.similarity > 0.8);
/* Result: [{name: "John Smith", similarity: 0.961}] */

/* Document similarity with Cosine Similarity */
documents = ["the quick brown fox", "a quick brown fox", "hello world"];
query = "quick brown fox";
similar = documents.map(doc => ({doc: doc, similarity: doc.cosinesimilarity(query)}))
                  .filter(match => match.similarity > 0.7);
/* Result: [{doc: "the quick brown fox", similarity: 0.999}, {doc: "a quick brown fox", similarity: 0.999}] */
```

## join(item)
Joins what has been split.

```
["this is a test ","split into ","parts"].join("") -> "this is a test split into part"
```

## upper()
Converts to upper case.

```
"hi".upper() -> "HI"
```

## lower()
Converts to lower.

```
"HI".lower() -> "hi"
```

## casefold()
Converts to Unicode case folded form for case-insensitive comparisons.

```
"İstanbul".casefold() -> "istanbul"
"ß".casefold() -> "ss"
"HELLO".casefold() -> "hello"
```

**Note**: `casefold()` is more aggressive than `lower()` and handles Unicode-specific case conversions like Turkish İ → i and German ß → ss.

## data = (stop).range(start,step)

```
(9).range(1,2);
[1,3,5,7]
```

## sort(axis,order,kind)

## argsort(axis,order,kind)

## unique(op)

## group(op1,op2,op3)

## raw()
Converts a value into it's raw bytes. Displays in hex form, but is stored in raw. Required in many cases as an intermediate form. For example, converting from a $STR to an $INT, you have two choices.

```
> "34".int();
34

> "34".raw();
3334

> "34".raw().int();
13108
```

## uraw()
Converts to an unsigned raw value. To avoid sign issues, a leading zero is added to raw/int. To remove, used uraw and uint.

```
> (0xFF).raw();
0x0FF

> (0xFF).uraw();
FF

> (0xFF).raw().int();
255

> (0xFF).raw().uint();
255

> (0xFF).uraw().int();
-1

> (0xFF).uraw().uint();
255
```

## bool()
Converts to what the system sees as true/false.

```
> "1".bool();
true

> (0).bool();
false
```

## int()
Converts to $INT.

```
"44".int() -> 44
```

## uint()
Converts to unsigned $INT.

See $INT.

## float([bits [,extra]])
Converts to $FLOAT. Sets bit count for the entire number. Calculations are performed with "extra" bits and truncated for display.

```
"4.21".float() -> 4.21

"4.21".float(300,7) / "10412.42".float(300,7) -> 0.00040432483514879346011782083319727786624050893068085997299379010835137268761728

("4.21".float(300,7) / "10412.42".float(300,7)).float(50) -> 0.00040432483514879
```

## fix([bits [,extra]])
Converts to fixed float. Sets bit count after the decimal to bits. Calculations are performed with "extra" bits and truncated for display.

## setfloat([bits [,extra]])
Sets the default float type to float, and the default bits and extra.

## setfix([bits [,extra]])
Sets the default float type to fix, and the default bits and extra.

## str()
Converts to string. 

```
(44).str() -> "44"
```

## base(base)
Converts number to base. Base of power of 2 works well. Have not fully tested others. Performed as a series of mods and divisions. So can become expensive on very large numbers. Split large numbers into parts before using - but careful on the split. 

```
(15).base(8) -> 17
(15).base(7) -> 21
```

## hex()
Converts item to hex. The hex value is stored as an ascii representation of '0'-'F' characters.

## bin()
Converts to binary. 

```
(0xC).bin() -> 1100
```

## setconst(truefalse)
Sets a bit to lock the variable from being modified. If set as const, the variable will not be locked when accessed. Useful for global variables accessed by multiple threads where if not a const will cause threads to block on access which is not needed if the variable doesn't change.

> **Performance & Parallelism:**
> All array/vector transformation methods (e.g., `.map()`, `.filter()`, `.reduce()`) are parallel by default, robust, and production-ready for ETL workloads. Grapa's parallelism is well tested for high-throughput data processing.

## Object Inspection and Reflection

### describe(options)

Provides comprehensive object description and reflection capabilities with configurable options.

**Parameters:**
- `options` (optional): Object with configuration options
  - `properties` (boolean): Include property names and types (default: true)
  - `methods` (boolean): Include method names and signatures (default: true)
  - `structure` (boolean): Include internal structure details (default: false)
  - `values` (boolean): Include actual values for small objects (default: false)
  - `format` (string): Output format - "text", "json", or "xml" (default: "text")

**Returns:** String description of the object

**Examples:**

```grapa
/* Basic descriptions */
'hello'.describe();                    /* "String with length 5" */
(123).describe();                      /* "Integer: 123" */
[1, 2, 3].describe();                  /* "Array with 3 elements" */
{name: 'Alice', age: 30}.describe();   /* "List with 2 properties (keys: name, age)" */

/* With structure details */
(2.3).describe({structure: true});
/* Returns: "Float: 2.3... (sign:false, trunc:false, fix:false, exp:1, max:184, extra:10, data:3689348814741910323)" */

/* With values */
'world'.describe({values: true});      /* "String with length 5: \"world\"" */

/* JSON format */
{name: 'Bob'}.describe({format: 'json'});
/* Returns: {"type":"list","length":1,"properties":1,"keys":["name"]} */

/* Array with type information */
[1, 'hello', 3.14].describe({structure: true});
/* Returns: "Array with 3 elements (types: integer, string, float)" */
```

**Float Structure Inspection:**
When `structure: true` is used with float values, the method reveals the 7 internal components of Grapa's `$FLOAT` type:

1. **sign** - Boolean indicating if the number is signed
2. **trunc** - Boolean indicating if truncation occurred  
3. **fix** - Boolean indicating if it's a fixed-point number
4. **exp** - Exponent value
5. **max** - Maximum bits
6. **extra** - Extra precision bits
7. **data** - The actual numeric data

This provides complete introspection into Grapa's high-precision floating-point implementation.

**Complementary Methods:**
For programmatic access to float components, Grapa also provides:
- **`.decode("FLOAT")`** - Extract components into an object for manipulation
- **`.encode("FLOAT")`** - Create a float from component values

```grapa
/* Extract components for manipulation */
components = (2.3).decode("FLOAT");
/* Returns: {"sign":false,"trunc":false,"fix":false,"exp":1,"max":184,"extra":10,"data":3689348814741910323} */

/* Modify components and create new float */
components.max = 300;
components.extra = 20;
new_float = components.encode("FLOAT");

/* .describe() provides inspection, .decode()/.encode() provide manipulation */
```
