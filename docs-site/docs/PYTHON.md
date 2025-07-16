# Python

/*
For a quick overview of what you can do with GrapaPy, see the new [Python Use Cases](PYTHON_USE_CASES.md) guide. This covers ETL, compiler/BNF learning, high-precision math, parallelism, web scraping, file/database integration, and education/prototyping, with quickstarts and migration tips for Python users.
*/

For details on the grapa language, see [Grapa language](../README.md)

**For comprehensive Python integration examples and use cases, see [Python Integration Guide](python_integration.md)**

The following assumes you have Python and pip installed, and possibly Jupyter and Notebook installed. 

This will install natively (including dependant libraries which will be installed in the grapapy extention library and not alter the system). 

Platforms supported are:
- win-amd64 - Windows Intel 64bit
- mac-amd64 - Mac Intel 64bit
- mac-arm64 - Mac Apple 64bit
- linux-amd64 - Ubuntu 22.04 (and compatible with C++23) Intel 64bit
- linux-arm64 - Ubuntu 24.04 (and compatible with C++23) Apple 64bit (maybe other ARM64 systems)

Windows
```
pip uninstall grapapy
pip install grapapy
```

Mac
```
pip3 uninstall grapapy
pip3 install grapapy
```

Linux
```
sudo apt update
sudo apt install -y python3-pip
sudo apt install -y x11-apps
sudo apt install -y libx11-dev
sudo apt install -y libxcursor-dev
sudo apt install -y libxft-dev
sudo apt install -y libxext-dev
sudo apt install -y libxinerama-dev
pip3 uninstall grapapy
pip3 install grapapy
```
If the pip3 install is being blocked, you will need to setup a virtual enviroment, or use "pip3 install --break-system-packages grapapy".

The following can be used to setup a virtual environment in Linux, if you would rather not use the override.
```
sudo apt install -y python3-venv
python3 -m venv ~/.venvs/grapa-env
source ~/.venvs/grapa-env/bin/activate
```

Amazon Linux
```
dnf update -y
dnf install -y libX*
dnf install -y python3-devel
python3 -m ensurepip --upgrade
python3 -m pip install --upgrade pip setuptools
pip3 install --user --upgrade packaging
dnf install -y perl
dnf install -y tar
pip3 uninstall grapapy
pip3 install grapapy
```

GrapaPy translates the following types between Python and Grapa:

Python | Grapa | Notes
------------ | ------------- | -------------
str | $STR
bytes | $RAW
int | $INT | Grapa $INT is unlimited precision
float | $FLOAT | Grapa $FLOAT is unlimited precision
list | $ARRAY | 
dict | $LIST | In grapa, a $LIST is a named version of $ARRAY.
tuple | $TUPLE | In grapa, a $TUPLE is the same as an $ARRAY. $TUPLE added for syntax transferability with Python.
bool | $BOOL
None | null

In grapa, the underlying structure for $ARRAY, $LIST, and $TUPLE are the same. With the difference bing first differnt brackets, and second that $LIST uses name/value pairs (like dict in Python). For lists that use more of a block of memory, use either $VECTOR or $TABLE. 

GrapaPy converts the following to str when passing back to Python. This will change in the future for some of these, such as $XML. 

Grapa | Notes
------------ | -------------
$ERR | Implemented as $LIST, this tye is returned on an error.
$XML, $EL, $TAG | Optionally convert to a $LIST before returning to Python
$TIME | Supports unlimited years, and as high granularity as the underlying OS can support.
$VECTOR | Work to translate between Python and Grapa not implemented.
$OP | This is the core grapa language that binds to internal libraries.
$CODE | This is the core grapa language that supports a list of $OP values.
$TABLE | This is a database type...no easy way to convert.
$WIDGET | Not yet supported in GrapayPy.This is based on the FLTK open source libraries. 
$CLASS
$OBJ
$RULE | The grapa language is implemented as global $RULE variables, that can be set, updated, etc. Code is evaluated against these $RULE variables.
$TOKEN | In addition to $RULE, there are $TOKEN definitions that control how raw data is converted to tokens before being applied to $RULE's.
$SYM, $SYSSYM
$SYSSTR
$SYSINT

For $XML, coverting to a $LIST and returning to Python will provide a "dict" view of the XML, which might be easier to parst through in Python. An example is provided below.

Jupyter notebook example:
[grapapy_test_ipynb](../grapapy_test.ipynb)

## import and simple test
```
import grapapy
xy = grapapy.grapa()
xy.eval("x = 3.45; y = 4.32; x**y;")
xy.eval("input.grep(pattern,options);",{"input":b"apple 123 pear 456\nbanana 789","pattern":b"\\d+","options":"o"})
```
'210.5612696743043090739172698565538965143180116634'

## Grep Examples

For a comprehensive guide to using Grapa grep from Python, see [Grapa Grep from Python (grep_python.md)](obj/grep_python.md).

### Method 1: Embedded Parameters
Pass parameters directly in the Grapa string:

```python
import grapapy
xy = grapapy.grapa()

# Basic grep with embedded string
xy.eval('"apple 123 pear 456\nbanana 789".grep(r"\\d+","o");')
# Result: ['123', '456', '789']

# With all parameters embedded
xy.eval('r"data1\\x00data2\\x00data3".grep(r"data\\d+", "o", r"\\x00", "NONE", "BINARY")')
# Result: ['data1', 'data2', 'data3']
```

### Method 2: Parameter Variables
Pass parameters as Python variables to be used in the Grapa string:

```python
import grapapy
xy = grapapy.grapa()

# Basic grep with parameter variables
xy.eval("input.grep(pattern,options);", {
    "input": "apple 123 pear 456\nbanana 789",
    "pattern": "\\d+",
    "options": "o"
})
# Result: ['123', '456', '789']

# With all parameters as variables
xy.eval("input.grep(pattern,options,delimiter,normalization,mode);", {
    "input": "data1\\x00data2\\x00data3",
    "pattern": "data\\d+",
    "options": "o",
    "delimiter": "\\x00",
    "normalization": "NONE",
    "mode": "BINARY"
})
# Result: ['data1', 'data2', 'data3']
```

### Unicode Examples

```python
import grapapy
xy = grapapy.grapa()

# Unicode characters
xy.eval('"Привет мир".grep("мир")')
# Result: ['Привет мир']

# Diacritic-insensitive matching
xy.eval('"café résumé naïve".grep("cafe", "d")')
# Result: ['café résumé naïve']

# Case-insensitive Unicode
xy.eval('"Café RÉSUMÉ Naïve".grep("café", "i")')
# Result: ['Café RÉSUMÉ Naïve']

# Unicode properties
xy.eval('"Hello 世界 123 €".grep(r"\\p{L}+", "o")')
# Result: ['Hello', '世界']

### Performance Examples

```python
import grapapy
xy = grapapy.grapa()

# Large input performance comparison
large_text = "..." # 50MB of text data

# Single-threaded (like Python re module)
result = xy.eval("input.grep(pattern, options, delimiter, normalization, mode, num_workers);", {
    "input": large_text,
    "pattern": "test",
    "options": "n",
    "num_workers": 1  # Single thread
})
# Takes ~9.6 seconds

# Parallel processing (massive speedup)
result = xy.eval("input.grep(pattern, options, delimiter, normalization, mode, num_workers);", {
    "input": large_text,
    "pattern": "test", 
    "options": "n",
    "num_workers": 16  # 16 threads
})
# Takes ~0.85 seconds (11.3x speedup!)
```

### All Grep Parameters

The grep function supports these parameters:

**Required:**
- `string`: Input text to search
- `pattern`: PCRE2 regular expression pattern

**Optional (with defaults):**
- `options`: String containing option flags (default: `""`)
- `delimiter`: Custom line delimiter (default: `"\n"`)
- `normalization`: Unicode normalization form (default: `"NONE"`)
- `mode`: Processing mode (default: `"UNICODE"`)
- `num_workers`: Number of worker threads for parallel processing: `0` for auto-detection, `1` for sequential, `2+` for parallel (default: `0`)

**🚀 Performance Note:** The `num_workers` parameter provides **massive performance improvements** over Python's single-threaded `re` module. Real-world tests show up to **11x speedup** on large inputs (50MB) when using 16 workers vs single-threaded processing.

For detailed grep documentation, see [Grep Documentation](obj/grep.md).

**Note:** For a comprehensive comparison of Grapa grep features with other tools like ripgrep, GNU grep, and Python's `re` module, including detailed feature matrices and use case recommendations, see the [Main Grep Documentation](obj/grep.md).

## Compile a script, and run the compiled script

```
x = xy.eval("$sys().compile(\"10;\")")
xy.eval(x)
```
10

## Pass in a single argument
Can only accept 1 argument.
Access from the grapa script using @$ARG

```
xy.eval("in1+' test';",{"in1":"hello"})
```
'hello test'

## Call back to Python from grapa script
```
xy.eval("$py().eval('print(\"hi\")');")
```
hi

## Call back to Python from grapa script
```
xy.eval("$py().eval('print(in1)',{'p1':in1});",{"in1":"hello"})
```
hello

## Call a Python function from a grapa script

```
def absolute_value(num):
    if num >= 0:
        return num
    else:
        return -num
xy.eval("""
    absolute_value = op(n=0){$py().eval("absolute_value("+n.str()+")");};
    absolute_value(-5);
""")
```

## Example: Running Python Integration Tests

See `test/test_python_examples.py` for a comprehensive set of Python-Grapa integration and callback tests, including argument passing and return values.

For advanced callback escaping and troubleshooting, see `test/test_python_callback.py`.

## Zero-Length Match and Empty String Output

> **Update (2024-06):** As of this release, zero-length matches and explicit empty strings in arrays are always output as `""` (empty string), never as `null`. This matches ripgrep’s behavior and ensures round-trip consistency and correct scripting semantics.

### Example: Zero-Length Match
```python
xy.eval('"a\nb\n".grep("^", "o")')
# Result: ["", "a", "", "b", ""]
```

### Example: Array Literal with Empty String
```python
xy.eval('[1, "", 2]')
# Result: [1, "", 2]
```

## Error Output

> **Note:** Invalid regex patterns always return `"$ERR"` (not a JSON object or other format).

```

# Grapa Python Integration Guide

This guide explains how to use Grapa's powerful features from Python through the GrapaPy extension.

## Installation

Install the GrapaPy extension:

```bash
pip install grapapy
```

## Basic Usage

```python
import grapapy

# Create a Grapa instance
xy = grapapy.grapa()

# Execute Grapa code
result = xy.eval("2 + 2;")
print(result)  # 4

# Complex calculations
result = xy.eval("x = 3.45; y = 4.32; x**y;")
print(result)  # 210.5612696743043
```

## Namespace Management

**Important**: GrapaPy maintains separate local and global namespaces. The local namespace is cleared between calls to `xy.eval()`, while the global namespace persists. This means:

- Variables created in the local namespace will not be available in subsequent calls
- Variables stored in the global namespace persist across calls
- For objects like `$file()` that need to be used across multiple calls, store them in the global namespace

**Key Insight**: You only need to use `$global.variable` the first time you create/initialize a variable. After that, you can use just `variable` directly. Grapa searches for variables in this order:
1. Current context (within `{ }` brackets)
2. Function scope
3. Local namespace
4. Call stack namespaces
5. Global namespace

### Example: File Operations with Global Namespace

```python
import grapapy

xy = grapapy.grapa()

# Store file object in global namespace (first time only)
xy.eval("$global.fs = $file();")

# Use the file object in subsequent calls (no $global needed)
xy.eval("fs.set('test.txt', 'Hello World');")
content = xy.eval("fs.get('test.txt');")
print(content)  # Hello World

# Get file information
info = xy.eval("fs.info('test.txt');")
print(info)
```

### Example: Table Operations with Global Namespace

```python
import grapapy

xy = grapapy.grapa()

# Create table in global namespace (first time only)
xy.eval("$global.table = $file().table('ROW');")

# Define fields (no $global needed)
xy.eval("table.mkfield('name', 'STR', 'VAR');")
xy.eval("table.mkfield('age', 'INT', 'FIX', 4);")

# Add data
xy.eval("table.set('user1', 'John Doe', 'name');")
xy.eval("table.set('user1', 30, 'age');")

# Retrieve data
name = xy.eval("table.get('user1', 'name');")
age = xy.eval("table.get('user1', 'age');")
print(f"Name: {name}, Age: {age}")
```

## File System Operations

### Basic File Operations

```python
import grapapy

xy = grapapy.grapa()

# Initialize file system in global namespace
xy.eval("$global.fs = $file();")

# Get current directory
pwd = xy.eval("$global.fs.pwd();")
print(f"Current directory: {pwd}")

# Get home directory
home = xy.eval("$global.fs.phd();")
print(f"Home directory: {home}")

# List directory contents
files = xy.eval("$global.fs.ls();")
print(f"Files: {files}")

# Change directory
xy.eval("$global.fs.cd('subdirectory');")

# Create directory
xy.eval("$global.fs.mk('new_directory');")
```

### File I/O Operations

```python
import grapapy

xy = grapapy.grapa()

# Initialize file system
xy.eval("$global.fs = $file();")

# Create and write to file
xy.eval("$global.fs.set('data.txt', 'Hello from Python!');")

# Read file content
content = xy.eval("$global.fs.get('data.txt');")
print(f"File content: {content}")

# Get file information
info = xy.eval("$global.fs.info('data.txt');")
print(f"File info: {info}")

# Delete file
xy.eval("$global.fs.rm('data.txt');")
```

### File Splitting

```python
import grapapy

xy = grapapy.grapa()

# Initialize file system
xy.eval("$global.fs = $file();")

# Create large file
xy.eval("""
    content = "";
    i = 1;
    while (i <= 100) {
        content = content + "Line " + i + ": Test data for splitting.\\n";
        i = i + 1;
    };
    $global.fs.set("large_file.txt", content);
""")

# Create output directory
xy.eval("$global.fs.mk('split_output');")

# Split file into 4 parts
result = xy.eval("$global.fs.split(4, 'large_file.txt', 'split_output', '\\\\n', '');")
print(f"Split result: {result}")

# List split files
split_files = xy.eval("$global.fs.ls('split_output');")
print(f"Split files: {split_files}")
```

## Database Operations

### Row Store Tables

```python
import grapapy

xy = grapapy.grapa()

# Create row store table in global namespace
xy.eval("$global.row_table = $file().table('ROW');")

# Define fields
xy.eval("$global.row_table.mkfield('name', 'STR', 'VAR');")
xy.eval("$global.row_table.mkfield('age', 'INT', 'FIX', 4);")
xy.eval("$global.row_table.mkfield('salary', 'FLOAT', 'FIX', 8);")

# Add records
xy.eval("$global.row_table.set('user1', 'John Doe', 'name');")
xy.eval("$global.row_table.set('user1', 30, 'age');")
xy.eval("$global.row_table.set('user1', 75000.50, 'salary');")

# Retrieve records
name = xy.eval("$global.row_table.get('user1', 'name');")
age = xy.eval("$global.row_table.get('user1', 'age');")
salary = xy.eval("$global.row_table.get('user1', 'salary');")

print(f"User: {name}, Age: {age}, Salary: {salary}")

# List all records
records = xy.eval("$global.row_table.ls();")
print(f"All records: {records}")
```

### Column Store Tables

```python
import grapapy

xy = grapapy.grapa()

# Create column store table
xy.eval("$global.col_table = $file().table('COL');")

# Define fields
xy.eval("$global.col_table.mkfield('name', 'STR', 'VAR');")
xy.eval("$global.col_table.mkfield('age', 'INT', 'FIX', 4);")
xy.eval("$global.col_table.mkfield('salary', 'FLOAT', 'FIX', 8);")

# Add records
xy.eval("$global.col_table.set('user1', 'Jane Smith', 'name');")
xy.eval("$global.col_table.set('user1', 25, 'age');")
xy.eval("$global.col_table.set('user1', 65000.75, 'salary');")

# Retrieve records
name = xy.eval("$global.col_table.get('user1', 'name');")
age = xy.eval("$global.col_table.get('user1', 'age');")
salary = xy.eval("$global.col_table.get('user1', 'salary');")

print(f"User: {name}, Age: {age}, Salary: {salary}")
```

### Table Management

```python
import grapapy

xy = grapapy.grapa()

# Initialize table
xy.eval("$global.table = $file().table('ROW');")
xy.eval("$global.table.mkfield('id', 'INT', 'FIX', 4);")
xy.eval("$global.table.mkfield('name', 'STR', 'VAR');")

# Add multiple records
xy.eval("""
    i = 1;
    while (i <= 10) {
        $global.table.set("record" + i, i, "id");
        $global.table.set("record" + i, "User " + i, "name");
        i = i + 1;
    }
""")

# Get debug information
debug_info = xy.eval("$global.table.debug();")
print(f"Table debug info: {debug_info}")

# Remove a field
xy.eval("$global.table.rmfield('name');")

# List remaining records
records = xy.eval("$global.table.ls();")
print(f"Records after field removal: {records}")
```

## Grep Operations

### Basic Grep

```python
import grapapy

xy = grapapy.grapa()

# Simple grep operation
result = xy.eval('"Hello World 123".grep("\\\\d+", "o");')
print(f"Grep result: {result}")

# Unicode grep
result = xy.eval('"Hello 世界 123 €".grep("\\\\p{L}+", "o");')
print(f"Unicode grep result: {result}")
```

### Advanced Grep with Parameters

```python
import grapapy

xy = grapapy.grapa()

# Grep with multiple parameters
result = xy.eval("input.grep(pattern, options, delimiter, normalization, mode, num_workers);", {
    "input": "apple 123 pear 456\nbanana 789",
    "pattern": "\\d+",
    "options": "o",
    "delimiter": "\\n",
    "normalization": "NONE",
    "mode": "UNICODE",
    "num_workers": 0
})
print(f"Advanced grep result: {result}")
```

## Error Handling

```python
import grapapy

xy = grapapy.grapa()

# Initialize file system
xy.eval("$global.fs = $file();")

# Handle file system errors
try:
    # Try to read non-existent file
    result = xy.eval("$global.fs.get('nonexistent.txt');")
    print(f"Result: {result}")
except Exception as e:
    print(f"Error: {e}")

# Handle table errors
try:
    # Try to access non-existent field
    result = xy.eval("$global.table.get('user1', 'nonexistent_field');")
    print(f"Result: {result}")
except Exception as e:
    print(f"Error: {e}")
```

## Performance Considerations

### Batch Operations

```python
import grapapy
import time

xy = grapapy.grapa()

# Initialize table
xy.eval("$global.table = $file().table('ROW');")
xy.eval("$global.table.mkfield('id', 'INT', 'FIX', 4);")
xy.eval("$global.table.mkfield('name', 'STR', 'VAR');")

# Batch insert
start_time = time.time()
xy.eval("""
    i = 1;
    while (i <= 1000) {
        $global.table.set("record" + i, i, "id");
        $global.table.set("record" + i, "User " + i, "name");
        i = i + 1;
    }
""")
insert_time = time.time() - start_time
print(f"Inserted 1000 records in {insert_time:.4f} seconds")

# Batch read
start_time = time.time()
records = xy.eval("$global.table.ls();")
read_time = time.time() - start_time
print(f"Read {len(records) if isinstance(records, list) else 'unknown'} records in {read_time:.4f} seconds")
```

## Best Practices

1. **Use Global Namespace**: Always store objects that need to persist across calls in the global namespace using `$global.variable`

2. **Initialize Once**: Create file system and table objects once and reuse them

3. **Error Handling**: Always wrap operations in try-catch blocks

4. **Batch Operations**: Use Grapa's built-in loops for batch operations instead of Python loops

5. **Resource Cleanup**: Clean up resources when done

```python
import grapapy

xy = grapapy.grapa()

# Initialize resources
xy.eval("$global.fs = $file();")
xy.eval("$global.table = $file().table('ROW');")

try:
    # Use resources
    xy.eval("$global.fs.set('data.txt', 'test');")
    xy.eval("$global.table.mkfield('name', 'STR', 'VAR');")
    
    # ... your operations ...
    
finally:
    # Cleanup (if needed)
    xy.eval("$global.fs.rm('data.txt');")
```

## Troubleshooting

### Common Issues

1. **Variables not persisting**: Use `$global.variable` instead of local variables
2. **File operations failing**: Ensure file system object is in global namespace
3. **Table operations failing**: Ensure table object is in global namespace
4. **Performance issues**: Use batch operations and avoid Python loops

### Debugging

```python
import grapapy

xy = grapapy.grapa()

# Check what's in global namespace
xy.eval("$global.debug_var = 'test';")
result = xy.eval("$global.debug_var;")
print(f"Global variable: {result}")

# Check file system object
xy.eval("$global.fs = $file();")
fs_type = xy.eval("$global.fs.type();")
print(f"File system type: {fs_type}")
```

This guide provides comprehensive coverage of using Grapa's features from Python. The key insight is using the global namespace to maintain object state across calls to the GrapaPy extension.

## Quick Links
- [GrapaPy Quickstart](GRAPAPY_INTEGRATION.md#quickstart-for-python-users)
- [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users)
- [Python Use Cases](PYTHON_USE_CASES.md)
