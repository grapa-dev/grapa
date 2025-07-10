import sys
try:
    sys.stdout.reconfigure(encoding='utf-8')
except Exception:
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

import grapapy
xy = grapapy.grapa()

print('--- Verifying grep_python.md code examples ---')

def check(desc, expr, expected):
    print(f'\n{desc}')
    try:
        result = xy.eval(expr)
        print('Result:', result)
        print('Expected:', expected)
        if result == expected:
            print('✅ PASS')
        else:
            print('❌ FAIL')
    except Exception as e:
        print('Exception:', e)
        print('❌ FAIL')

# Basic usage
check('Simple pattern matching', '"Hello world".grep("world")', ['Hello world'])
check('Case-insensitive matching', '"Hello WORLD".grep("world", "i")', ['Hello WORLD'])
check('Match-only output', '"Hello world".grep(r"\\w+", "o")', ['Hello', 'world'])

# All parameters as variables
xy_vars = {
    "input": "data1\x00data2\x00data3",
    "pattern": "data\\d+",
    "options": "o",
    "delimiter": "\x00",
    "normalization": "NONE",
    "mode": "BINARY"
}
result = xy.eval('input.grep(pattern,options,delimiter,normalization,mode);', xy_vars)
print('\nAll parameters as variables:')
print('Result:', result)
print('Expected:', ['data1', 'data2', 'data3'])
print('✅ PASS' if result == ['data1', 'data2', 'data3'] else '❌ FAIL')

# Unicode examples
check('Unicode characters', '"Привет мир".grep("мир")', ['Привет мир'])
check('Unicode properties', '"Hello 世界 123 €".grep(r"\\p{L}+", "o")', ['Hello', '世界'])

# Emoji handling
check('Emoji handling', '"Hello 👋 world 🌍".grep(r"(?:\\p{So}(?:\\u200D\\p{So})*)+", "o")', ['👋', '🌍'])

# Unicode grapheme clusters
check('Unicode grapheme clusters', '"café mañana".grep(r"\\X", "o")', ['c', 'a', 'f', 'é', ' ', 'm', 'a', 'ñ', 'a', 'n', 'a'])

# Named groups with JSON output
check('Named groups with JSON output', '"John Doe (30)".grep(r"(?P<first>\\w+) (?P<last>\\w+) \\((?P<age>\\d+)\\)", "oj")', [{"match":"John Doe (30)","first":"John","last":"Doe","age":"30","offset":0,"line":1}])

# Unicode scripts
check('Unicode scripts', '"Hello 世界 こんにちは 안녕하세요".grep(r"\\p{sc=Han}+", "o")', ['世界'])

# Atomic groups
check('Atomic groups', '"aaa".grep(r"(?>a+)a", "o")', [])

# Lookaround assertions
check('Lookaround assertions', '"abc123def456".grep(r"(?<=\\d)(?=\\d)", "o")', ['', '', '', '', '', ''])

# Raw string literal (no escaping needed)
check('Raw string literal', '"file.txt".grep(r"^[a-zA-Z0-9_]+\\.txt$", "x")', ['file.txt'])

# Complex pattern
check('Complex pattern', '"user@domain.com".grep(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "x")', ['user@domain.com'])

print('\n--- End of grep_python.md example verification ---') 