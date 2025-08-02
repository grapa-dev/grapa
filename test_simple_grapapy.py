#!/usr/bin/env python3
"""
Very simple GrapaPy test
"""

import grapapy

print("Testing GrapaPy...")

xy = grapapy.grapa()

# Test basic operations
print("Basic math:", xy.eval("2 + 2;"))
print("String:", xy.eval("'Hello World';"))
print("Array:", xy.eval("[1, 2, 3].len();"))

# Test factorial
print("Testing factorial...")
xy.eval("$global.fact = op(n=0){if(n<=1,1,n*fact(n-1));};")
result = xy.eval("fact(5);")
print("factorial(5) =", result)

# Test Python callback
def myfunc(x):
    print(f"Python function called with {x}")
    return x * 2

xy.eval("$this.myfunc = op(x=0){$py().eval('myfunc', {'x':x});};")
result = xy.eval("myfunc(10);")
print("Python callback result:", result)

print("Done!") 