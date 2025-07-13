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

## rtrim([str])
Trims right.

```
"  testing  ".rtrim() -> "  testing"
"bbbtestingbbb".rtrim(b) -> "bbbtesting"
```

## ltrim([str])
Trims left.

```
"  testing  ".ltrim() -> "testing  "
"bbbtestingbbb".ltrim(b) -> "testingbbb"
```

## trim([str])
Trims both left and right.

```
"  testing  ".trim() -> "testing"
"bbbtestingbbb".trim(b) -> "testing"
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

## grep(pattern, options, delimiter, normalization, mode, num_workers) 
Extracts matches from a string using PCRE2-powered regular expressions with full Unicode support. Returns an array of results or JSON format with named groups.

> **For comprehensive Unicode, advanced regex, diacritic-insensitive, and output option documentation, see [Unicode Grep Documentation](grep.md).**

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
"file.txt".grep(r"^[a-zA-Z0-9_]+\.txt$", "x");
/* → ["file.txt"] - No need to escape backslashes */

"user@domain.com".grep(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$", "x");
/* → ["user@domain.com"] - Much cleaner than escaped version */

/* Raw strings preserve literal escape sequences */
"\\x45".grep(r"\x45", "o");
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

> **📖 For comprehensive Unicode grep documentation including advanced features, named groups, JSON output, and Unicode properties, see [Unicode Grep Documentation](grep.md).**

> **💡 Tip**: Use raw string literals (prefix with `r`) for better regex pattern readability. For example, `r"\w+"` instead of `"\\w+"`. Raw strings suppress all escape sequences except for escaping the quote character used to enclose the string.

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
// Result: ["café", "CAFÉ", "café", "Cafe", "CAFÉ"]
```

## split(sep, max, axis)
Splits into an array.

```
"one\ntwo\nthree".split("\n") -> ["one","two","three"]
"this is a test".split(" ") -> ["this","is","a","test"]
"this is a test split into parts".split(3) -> ["this is a t","est split i","nto parts"]
"this is a test split into parts".split(" ", 3) -> ["this is a test ","split into ","parts"]
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
