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
/* Note: .left(), .right(), and .mid() also work with $LIST and $GOBJ types */

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

> **See Also:** [Object Methods Documentation](obj_methods.md) for comprehensive details on Unicode-aware string functions including `.len()`, `.bytes()`, `.raw()`, `.left()`, `.right()`, `.mid()`, `.reverse()`, `.lpad()`, and `.rpad()`. Note that `.left()`, `.right()`, and `.mid()` also support `$LIST` and `$GOBJ` types.

## String-Specific Methods

The following methods are available specifically on `$STR` objects:

### Case Conversion
```grapa
/* Basic case conversion */:
"Hello World".upper();     /* Returns: "HELLO WORLD" */
"Hello World".lower();     /* Returns: "hello world" */
"Hello World".casefold();  /* Returns: "hello world" */

/* Unicode case folding for international text */:
"İstanbul".casefold();     /* Returns: "istanbul" */
"ß".casefold();            /* Returns: "ss" */
```

### String Distance & Similarity Functions

#### **Unified Similarity Method (NEW)**
```grapa
/* Unified .similarity() method with multiple algorithms */
"hello".similarity("hallo", "levenshtein");        /* Returns: 1 (edit distance) */
"hello".similarity("hallo", "levenshtein_distance"); /* Same as above */
"hello".similarity("hallo", "jaro");               /* Returns: 0.88 (Jaro-Winkler similarity) */
"hello".similarity("hallo", "jaro_winkler_similarity"); /* Same as above */
"hello world".similarity("world hello", "cosine"); /* Returns: 1.0 (cosine similarity) */
"hello world".similarity("world hello", "cosine_similarity"); /* Same as above */
"hello world".similarity("hello there", "jaccard"); /* Returns: 0.333 (Jaccard similarity) */
"hello world".similarity("hello there", "jaccard_similarity"); /* Same as above */

/* Default method (cosine similarity) */
"hello world".similarity("world hello");           /* Returns: 1.0 (default cosine) */

/* Advanced options for Cosine and Jaccard */
"hello world".similarity("world hello", "cosine", {
  "method": "tfidf",                    /* Use TF-IDF instead of word frequency */
  "corpus": ["hello world", "goodbye"], /* Document corpus for TF-IDF */
  "case_sensitive": false               /* Case-insensitive comparison */
});

"hello world".similarity("hello there", "jaccard", {
  "jaccard_method": "char",             /* Use character-based instead of word-based */
  "jaccard_n": 3,                       /* Use 3-grams for character method */
  "case_sensitive": false               /* Case-insensitive comparison */
});
```


#### **Supported Similarity Methods for $STR**

The unified `.similarity()` method supports the following algorithms:

**Distance Methods (Lower = More Similar):**
- `"levenshtein"` / `"levenshtein_distance"` - Edit distance between strings
- `"damerau"` / `"damerau_levenshtein"` / `"damerau_levenshtein_distance"` - Edit distance with transposition support

**Similarity Methods (Higher = More Similar):**
- `"jaro"` / `"jaro_winkler"` / `"jaro_winkler_similarity"` - String similarity score (0-1 range)
- `"cosine"` / `"cosine_similarity"` - Vector-based similarity using word frequencies
- `"jaccard"` / `"jaccard_similarity"` - Set-based similarity using word overlap
- `"tfidf"` / `"cosine_tfidf"` - TF-IDF weighted cosine similarity

**Examples:**
```grapa
/* Distance methods (lower = more similar) */
"hello".similarity("hallo", "levenshtein");        /* Returns: 1 */
"hello".similarity("hlelo", "damerau");           /* Returns: 1 (transposition) */

/* Similarity methods (higher = more similar) */
"hello".similarity("hallo", "jaro");               /* Returns: 0.88 */
"hello world".similarity("world hello", "cosine"); /* Returns: 1.0 */
"hello world".similarity("hello there", "jaccard"); /* Returns: 0.333 */

/* Default method (cosine similarity) */
"hello world".similarity("world hello");           /* Returns: 1.0 */
```

**Use Cases:**
- **Case conversion**: Text normalization, user input processing
- **Levenshtein distance**: Spell checking, fuzzy matching, edit distance calculations
- **Damerau-Levenshtein distance**: Typo detection, DNA analysis, transposition handling
- **Jaro-Winkler similarity**: Record matching, duplicate detection, name matching
- **Cosine similarity**: Document similarity, word-based analysis
- **Jaccard similarity**: Set-based analysis, n-gram matching, document overlap

#### **Advanced Parameters**

The unified `.similarity()` method supports advanced parameters for fine-tuning Cosine and Jaccard algorithms:

**Cosine Similarity Parameters:**
- `"method"`: `"word_freq"` (default) or `"tfidf"` - Choose between word frequency or TF-IDF weighting
- `"corpus"`: Array of documents for TF-IDF calculation - Required for TF-IDF method
- `"case_sensitive"`: `true` (default) or `false` - Enable case-insensitive comparison

**Jaccard Similarity Parameters:**
- `"jaccard_method"`: `"word"` (default), `"char"`, or `"ngram"` - Choose similarity approach
- `"jaccard_n"`: Integer (default: 2) - N-gram size for character/ngram methods
- `"case_sensitive"`: `true` (default) or `false` - Enable case-insensitive comparison

**Examples:**
```grapa
/* TF-IDF with document corpus */
"machine learning".similarity("artificial intelligence", "cosine", {
  "method": "tfidf",
  "corpus": ["machine learning algorithms", "artificial intelligence systems", "data science methods"]
});

/* Character-based Jaccard with 3-grams */
"hello".similarity("hallo", "jaccard", {
  "jaccard_method": "char",
  "jaccard_n": 3
});

/* Case-insensitive comparison */
"Hello World".similarity("hello world", "cosine", {
  "case_sensitive": false
});
```

#### **Large String Optimization**

For very large strings (> 500 characters), consider using embedding-based similarity for better performance and semantic understanding:

```grapa
/* For large strings, use embedding-based similarity */
long_text1 = "This is a very long piece of text that contains multiple sentences and paragraphs...";
long_text2 = "This is another long piece of text with similar content but different wording...";

/* Convert to embeddings using $MODEL */
model = $MODEL();
model.load("text-embedding-3-small", "openai-embedding", {api_key: "your-api-key"});
embedding1 = model.gen(long_text1);
embedding2 = model.gen(long_text2);

/* Use vector similarity for better performance and semantic understanding */
similarity = embedding1."embedding".similarity(embedding2."embedding", "cosine");
```

**When to use embedding-based similarity:**
- **Long texts** (> 500 characters): Better performance than string algorithms
- **Semantic similarity**: When meaning matters more than exact character matching
- **Document comparison**: Comparing paragraphs, articles, or documents
- **Content recommendation**: Finding semantically similar content

**Performance comparison:**
- **String algorithms**: O(m×n) complexity, slow for long strings
- **Embedding algorithms**: O(1) complexity after embedding, but requires API calls

## Array Similarity

The `.similarity()` method supports **array similarity** - comparing a query against a list of items to find the best matches. This works with both **string arrays** and **object arrays** for metadata search.

### String Array Similarity

```grapa
/* Basic string array similarity */
strings = ["hello world", "hello there", "goodbye world"];
query = "hello";

/* Find best matches (returns structured object) */
result = strings.similarity(query, "cosine");
/* Returns: {
  "results": [
    {"index": 0, "similarity": 0.707, "item": "hello world"},
    {"index": 1, "similarity": 0.707, "item": "hello there"}
  ],
  "best_similarity": 0.707,
  "best_index": 0,
  "best_match": "hello world",
  "method": "cosine"
} */
```

### Object Array Similarity (Metadata Search)

The `.similarity()` method also supports **object similarity** for searching through collections of metadata objects:

```grapa
/* Metadata search with object similarity */
metadata = [
    {"name": "John", "age": 30, "city": "New York", "department": "Engineering"},
    {"name": "Jane", "age": 25, "city": "Boston", "department": "Marketing"},
    {"name": "Bob", "age": 30, "city": "Chicago", "department": "Engineering"}
];

/* Search for objects matching query criteria */
query = {"name": "John", "age": 30};
result = metadata.similarity(query, "cosine", {"include_scores": true, "include_items": true});
/* Returns: {
  "results": [
    {"index": 0, "similarity": 0.666667, "item": {"name": "John", "age": 30, "city": "New York", "department": "Engineering"}},
    {"index": 2, "similarity": 0.333333, "item": {"name": "Bob", "age": 30, "city": "Chicago", "department": "Engineering"}},
    {"index": 1, "similarity": 0.0, "item": {"name": "Jane", "age": 25, "city": "Boston", "department": "Marketing"}}
  ],
  "best_similarity": 0.666667,
  "best_index": 0,
  "best_match": {"name": "John", "age": 30, "city": "New York", "department": "Engineering"},
  "method": "cosine"
} */
```

**Object Similarity Features:**
- **Field Matching**: Compares fields that exist in both the query object and target objects
- **Partial Matches**: Query objects can contain a subset of fields from the target objects
- **Multiple Data Types**: Supports string, integer, float, and boolean field comparisons
- **Similarity Scoring**: Returns similarity as ratio of matching fields to total fields in query
- **Flexible Queries**: Can search by any combination of object fields

### Array Similarity Parameters

```grapa
/* Control result format and filtering */
result = strings.similarity(query, "cosine", {
  "top_n": 2,              /* Limit to top 2 results (default: 5) */
  "threshold": 0.5,        /* Minimum similarity score (default: 0.0) */
  "sort": "desc",          /* Sort order: "desc", "asc", "none" (default: "desc") */
  "include_scores": true,  /* Include similarity scores (default: true) */
  "include_items": true    /* Include actual items (default: true) */
});

/* Compact results without items */
compact = strings.similarity(query, "cosine", {
  "top_n": 3,
  "include_items": false
});
/* Returns: {
  "results": [
    {"index": 0, "similarity": 0.707},
    {"index": 1, "similarity": 0.707}
  ],
  "best_similarity": 0.707,
  "best_index": 0,
  "method": "cosine"
} */
```

### Array Similarity Use Cases

**String Array Similarity:**
- **Search and ranking**: Find best matches from a list of strings
- **Fuzzy matching**: Find closest matches even with typos
- **Content filtering**: Filter results by similarity threshold
- **Recommendation systems**: Rank items by similarity to user input
- **Data deduplication**: Find similar entries in datasets

**Object Array Similarity (Metadata Search):**
- **Database queries**: Search through structured data collections
- **User profiles**: Find users matching specific criteria
- **Product catalogs**: Search products by attributes
- **Document metadata**: Find documents with matching properties
- **Configuration management**: Search system configurations by parameters
- **API responses**: Filter and rank API results by metadata
- **Content management**: Find content items by tags, categories, or properties

**Performance:**
- **String algorithms**: O(m×n) complexity, slow for long strings
- **Object similarity**: O(n×f) complexity where n is number of objects and f is average number of fields
- **Embedding algorithms**: O(d) complexity where d is embedding dimension (~1536), consistent performance

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
- **$LIST**: Searches for subarrays using comprehensive comparison
- **$GOBJ**: Searches for sublists using comprehensive comparison

**Return Value:**
- Returns the 0-based index of the first occurrence
- Returns -1 if the term is not found
- Returns -1 if start position is beyond the string/data length

> **Note:** For RAW data, use `.uraw()` to create search terms from hex values to avoid padding issues.
> 
> **Array and List Search:** The `.find()` method supports searching for subarrays and sublists using the same comprehensive comparison logic as `switch` statements, ensuring consistent behavior across the language.
> 
> **Array-in-List Search:** When searching a `$GOBJ` with an `$LIST` term, the method searches for the array values within the list values and returns the position where the match is found. Use `.mid()` to extract the subset. For example: `{a:1,b:2,c:3}.find([2,3])` returns `1`, and `{a:1,b:2,c:3}.mid(1, 2)` returns `{b:2,c:3}`.

### Pattern Matching
```grapa
/* Basic pattern matching */
"hello world".match("hello");     /* true - pattern found */
"hello world".match("xyz");       /* false - pattern not found */
"hello world".match("a{");        /* false - invalid pattern handled gracefully */

/* Case insensitive matching */
"hello world".match("HELLO", "i"); /* true - case insensitive match */:
"Hello World".match("world", "i"); /* true - case insensitive match */:

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
