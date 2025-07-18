# $STR
References:


ASCII only - Unicode will eventually be added. Can initialize with either double quotes or single quotes - which is easier than escaping a string that includes a quoted string - such as "this 'is' a test", or 'this "is" a test'. $STR inherits the $obj class - see $obj for functions supported. 

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

Examples:
Commands | Results
------------ | -------------
"this is a test".len(); | 14
"this is a test".split(""); | ["t","h","i","s"," ","i","s"," ","a"," ","t","e","s","t"]
"this is a test".raw(); | 0x7468697320697320612074657374
"this is a test".raw().int(); | 2361031878030638688519054699098996
