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
Python | Grapa
------------ | ------------- | -------------
str | $STR
bytes | $RAW
int | $INT | grapa $INT is unlimited precision
float | $FLOAT | grapa $FLOAT is unlimited precision
list | $ARRAY
dict | $LIST
tuple | $TUPLE
bool | $BOOL
None | null

If there the automatic translation is not designed, convert the data to a string before passing into grapa, or before returning from grapa. For example, if using a $INT larger than what Python can support, convert to a string before returning.

There are also several data types in grapa that are not built into Python (but can be added as a library) - for example datetime. A $DATE value in grapa could be converted to a string before returning to Pything, and then using the datetime module to use the date.

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