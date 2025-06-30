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
5

Same as above, but with passing in an object to Python.
```
def absolute_value(num):
    if num >= 0:
        return num
    else:
        return -num
xy.eval("""
    absolute_value = op(n=0){$local.locals={"g":n};$py().eval("absolute_value(g)",locals);};
    absolute_value(-5);
""")
```

## Convert XML to JSON
```
xy.eval("""
    (<test "v"=5.4>this is a test</test>).list();
""")
```
{'': {'test': [{'v': 5.4}, ['this is a test']]}}

## Domain specific language in Python using Grapa
The following is an illustration on how to use the $RULE type in Grapa to create a domain specific language in Python that can be executed.

```
>>> import grapapy
>>> xy = grapapy.grapa()
>>> def func1(num):
...     if num >= 10:
...         print("found higher")
...         return "higher"
...     else:
...         print("found lower")
...         return "lower"
...
>>> def func2(num1,num2):
...     if num1 >= 10:
...         print("div")
...         return num1/num2
...     else:
...         print("mul")
...         return num1*num2
...
>>> xy.eval("""
...     $this.testrule = rule
...           $INT $INT {op(a:$1,b:$2) {$py().eval('func2(v1,v2)', {"v1":a,"v2":b} );}}
...         | $INT      {op(a:$1)      {$py().eval('func1(v)',     {"v":a}         );}}
...         ;
... """)
>>> xy.eval("$sys().eval(s,{},'testrule');",{"s":"4"})
found lower
'lower'
>>> xy.eval("$sys().eval(s,{},'testrule');",{"s":"4 6"})
mul
24
>>> cm = xy.compile("4 8",'testrule')
>>> xy.eval(cm)
mul
32
>>>
```
