# Python

For details on the grapa language, see [Grapa language](../README.md)

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
