import sys
try:
    sys.stdout.reconfigure(encoding='utf-8')
except Exception:
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

import grapapy
xy = grapapy.grapa()

print('--- Grapa Python Callback Escaping Test ---')

def try_callback(desc, expr):
    print(f'\n{desc}')
    try:
        result = xy.eval(expr)
        print('Result:', result)
    except Exception as e:
        print('Exception:', e)

# 1. Raw string in Python (works)
try_callback('Raw string in Python', r'$py().eval("print(\"hi\")")')

# 2. Triple quotes in Python (works)
try_callback('Triple quotes in Python', '''$py().eval("print(\\"hi\\")")''')

# 3. Print a simple value (works)
try_callback('Print a simple value', '$py().eval("print(123)")')

# 4. Print a simple return (works)
try_callback('Print a simple value', '$py().eval("123")')

print('\n--- End of callback test ---')
