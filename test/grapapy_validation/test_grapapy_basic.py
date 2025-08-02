#!/usr/bin/env python3
"""
Basic GrapaPy operations that work
"""

import grapapy

print("=== Basic GrapaPy Test ===")

xy = grapapy.grapa()

# 1. Simple math
print("1. Math operations:")
print("   2 + 2 =", xy.eval("2 + 2;"))
print("   10 * 5 =", xy.eval("10 * 5;"))
print("   100 / 4 =", xy.eval("100 / 4;"))

# 2. String operations
print("\n2. String operations:")
print("   'Hello' + ' World' =", xy.eval("'Hello' + ' World';"))
print("   'Test'.len() =", xy.eval("'Test'.len();"))

# 3. Array operations
print("\n3. Array operations:")
print("   [1,2,3].len() =", xy.eval("[1,2,3].len();"))
print("   [1,2,3][0] =", xy.eval("[1,2,3][0];"))

# 4. Simple function
print("\n4. Simple function:")
xy.eval("$global.square = op(x){x*x;};")
print("   square(5) =", xy.eval("square(5);"))

# 5. File operations
print("\n5. File operations:")
xy.eval("$global.fs = $file();")
xy.eval("fs.set('test.txt', 'Hello from Python!');")
content = xy.eval("fs.get('test.txt');")
print("   File content:", content)

# 6. Grep operations
print("\n6. Grep operations:")
text = "apple\nbanana\ncherry\napple pie\n"
result = xy.eval("text.grep('apple');", {"text": text})
print("   grep('apple') =", result)

print("\n=== Test completed successfully! ===") 