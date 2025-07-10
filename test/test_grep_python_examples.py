import sys
try:
    sys.stdout.reconfigure(encoding='utf-8')
except Exception:
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
import grapapy
xy = grapapy.grapa()

print('--- Basic grep test ---')
result = xy.eval('"Hello world".grep("world")')
print('Basic grep test:', result)

print('--- Case-insensitive ---')
result = xy.eval('"Hello WORLD".grep("world", "i")')
print('Case-insensitive:', result)

print('--- Match-only ---')
result = xy.eval(r'"Hello world".grep(r"\w+", "o")')
print('Match-only:', result)

print('--- Parameter variables ---')
result = xy.eval("input.grep(pattern,options);", {
    "input": "apple 123 pear 456\nbanana 789",
    "pattern": r"\d+",
    "options": "o"
})
print('Parameter variables:', result)

print('--- All parameters as variables ---')
result = xy.eval("input.grep(pattern,options,delimiter,normalization,mode);", {
    "input": "data1\x00data2\x00data3",
    "pattern": r"data\d+",
    "options": "o",
    "delimiter": "\x00",
    "normalization": "NONE",
    "mode": "BINARY"
})
print('All parameters:', result)

print('--- Unicode characters ---')
result = xy.eval('"Привет мир".grep("мир")')
print('Unicode characters:', result)

print('--- Diacritic-insensitive ---')
result = xy.eval('"café résumé naïve".grep("cafe", "d")')
print('Diacritic-insensitive:', result)

print('--- Case-insensitive Unicode ---')
result = xy.eval('"Café RÉSUMÉ Naïve".grep("café", "i")')
print('Case-insensitive Unicode:', result)

print('--- Unicode properties ---')
result = xy.eval(r'"Hello 世界 123 €".grep(r"\p{L}+", "o")')
print('Unicode properties:', result) 