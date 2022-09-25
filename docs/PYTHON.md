# Python

For details on the grapa language, see [Grapa language](../README.md)

The following assumes you have Python and pip installed, and possibly Jupyter and Notebook installed. 

This will install natively (including dependant libraries which will be installed in the grapapy extention library and not alter the system). Platforms supported are:
- Windows 64bit
- Linux
- Amazon Linux (can be used in a Lambda and in Docker)
- Mac Intel
- Mac Apple M1 (would probably also work on M2)

Linux/Mac
```
pip3 uninstall grapapy
pip3 install grapapy
```

Windows
```
pip uninstall grapapy
pip install grapapy
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

In grapa, the underlying structure for $ARRAY, $LIST, and $TUPLE are the same.

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
$REF
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
```
'210.5612696743043090739172698565538965143180116634'

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
xy.eval("@$ARG+' test';","hello")
```
'hello test'

## Call back to Python from grapa script
```
xy.eval("$py().eval('print(\"hi\")');")
```
hi

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
5

The first grapa script call defines a function that builds the Python script to send to Python.

The second call is an example of using this new grapa function.

## Convert XML to JSON
```
xy.eval("""
    (<test "v"=5.4>this is a test</test>).list();
""")
```
{'': {'test': [{'v': 5.4}, ['this is a test']]}}

