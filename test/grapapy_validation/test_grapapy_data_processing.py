#!/usr/bin/env python3
"""
Data Processing Example with GrapaPy
Based on the documentation examples
"""

import grapapy

print("=== GrapaPy Data Processing Example ===")

xy = grapapy.grapa()

# 1. Create a simple dataset
print("1. Creating dataset...")
xy.eval("$global.data = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100];")

# 2. Basic statistics
print("\n2. Basic statistics:")
total = xy.eval("data.reduce(op(sum, x){sum + x;}, 0);")
count = xy.eval("data.len();")
average = xy.eval("total / count;")

print(f"   Total: {total}")
print(f"   Count: {count}")
print(f"   Average: {average}")

# 3. Filter operations
print("\n3. Filter operations:")
evens = xy.eval("data.filter(op(x){x % 2 == 0;});")
odds = xy.eval("data.filter(op(x){x % 2 != 0;});")
large = xy.eval("data.filter(op(x){x > 50;});")

print(f"   Even numbers: {evens}")
print(f"   Odd numbers: {odds}")
print(f"   Numbers > 50: {large}")

# 4. Transform operations
print("\n4. Transform operations:")
doubled = xy.eval("data.map(op(x){x * 2;});")
squared = xy.eval("data.map(op(x){x * x;});")

print(f"   Doubled: {doubled}")
print(f"   Squared: {squared}")

# 5. String processing
print("\n5. String processing:")
text = "apple\nbanana\ncherry\napple pie\norange\npear\napple cider\n"

# Count lines containing 'apple'
apple_lines = xy.eval("text.grep('apple').len();", {"text": text})
print(f"   Lines containing 'apple': {apple_lines}")

# Get all lines with context
with_context = xy.eval("text.grep('apple', 'A1B1');", {"text": text})
print(f"   Lines with context: {with_context}")

# 6. File operations with data
print("\n6. File operations:")
xy.eval("$global.fs = $file();")

# Create a data file
data_content = "name,age,city\nAlice,25,New York\nBob,30,Los Angeles\nCharlie,35,Chicago\n"
xy.eval("fs.set('people.csv', data_content);")

# Read and process the file
file_content = xy.eval("fs.get('people.csv');")
print(f"   File content: {file_content}")

# 7. High precision math
print("\n7. High precision math:")
big_num = xy.eval("12345678901234567890 * 98765432109876543210;")
print(f"   Big calculation: {big_num}")

print("\n=== Data processing example completed! ===") 