import sys
try:
    sys.stdout.reconfigure(encoding='utf-8')
except Exception:
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
import grapapy
xy = grapapy.grapa()

print('--- Math test ---')
result = xy.eval('x = 3.45; y = 4.32; x**y;')
print('Math:', result)

print('--- Grep test (bytes) ---')
result = xy.eval('input.grep(pattern,options);', {"input":b"apple 123 pear 456\nbanana 789", "pattern":br"\d+", "options":"o"})
print('Grep (bytes):', result)

print('--- Grep test (string) ---')
result = xy.eval('input.grep(pattern,options);', {"input":"apple 123 pear 456\nbanana 789", "pattern":r"\d+", "options":"o"})
print('Grep (string):', result)

print('--- Compile and run script ---')
x = xy.eval('$sys().compile("10;")')
result = xy.eval(x)
print('Compile/run:', result)

print('--- Single argument ---')
result = xy.eval("in1+' test';",{"in1":"hello"})
print('Single argument:', result)

print('--- Python callback ---')
xy.eval('$py().eval(\'print("hi")\');')

print('--- Python callback ---')
xy.eval(r'$py().eval("print(\"hi\")")')

print('--- Python callback with argument ---')
# Use single quotes for Grapa, double quotes for Python, and pass variables as shown
xy.eval(r"$py().eval('print(p1)',{'p1':in1});", {"in1": "hello"})

print('--- Python callback returning result ---')
result = xy.eval('$py().eval(\'10*20\');')
print('Callback result:', result)

print('--- Call Python function from Grapa ---')
def absolute_value(num):
    if num >= 0:
        return num
    else:
        return -num
result = xy.eval('''
    absolute_value = op(n=0){$py().eval("absolute_value("+n.str()+")");};
    absolute_value(-5);
''')
print('absolute_value(-5) result:', result)

print('--- Call Python function with object ---')
def absolute_value(num):
    if num >= 0:
        return num
    else:
        return -num
xy.eval('''
    absolute_value = op(n=0){$local.locals={"g":n};$py().eval("absolute_value(g)",locals);};
    absolute_value(-5);
''')

print('--- XML to JSON ---')
result = xy.eval('(<test "v"=5.4>this is a test</test>).list();')
print('XML to JSON:', result) 

print('--- End of test ---') 