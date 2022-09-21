# Python

References:
- [Grapa language](../README.md)

The following assumes you have Python and pip installed, and possibly Jupyter and Notebook installed. 

This will install natively (including dependant libraries which will be installed in the grapapy extention library and not alter the system). Platforms supported are:
- Windows 64bit
- Linux
- Amazon Linux (can be used in a Lambda and in Docker)
- Mac Intel
- Mac Apple M1 (would probably also work on M2)

Linux/Mac
```
pip3 install grapapy
```

Windows
```
pip install grapapy
```

Jupyter notebook example:
[grapapy_test_ipynb](../grapapy_test.ipynb)

## import and simple test
```
import grapapy
xy = grapapy.new()
xy.eval("x = 3.45; y = 4.32; x**y;")
```
'210.5612696743043090739172698565538965143180116634'

## Compile a script, and run the compiled script

```
x = xy.evalb("$sys().compile(\"10;\")")
xy.exec(x)
```
'10'

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
In the grapa script, need to include a second parameter to specify the return type of the Python function. Can support the following return types:
- "" - this is the default, any return value is ignored
- "int' - function returns an int, and can only be an int
- "str' - function returns a string, and can only be a string

```
def absolute_value(num):
    if num >= 0:
        return num
    else:
        return -num
xy.eval("""
    absolute_value = op(n=0){$py().eval("absolute_value("+n.str()+")","int");};
    absolute_value(-5);
""")
```
'5'

The first grapa script call defines a function that builds the Python script to send to Python.

The second call is an example of using this new grapa function.