#!/usr/bin/env python3
"""
Debug Reduce Operations
Test reduce step by step to understand the issue
"""

import grapapy

print("=== Debug Reduce Operations ===")

xy = grapapy.grapa()

# Test 1: Simple array
print("Test 1: Simple array [1, 2, 3]")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { acc + x; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 2: Check array creation
print("\nTest 2: Array creation")
try:
    result = xy.eval("[1, 2, 3];")
    print(f"   Array: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 3: Check array length
print("\nTest 3: Array length")
try:
    result = xy.eval("[1, 2, 3].len();")
    print(f"   Length: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 4: Manual sum
print("\nTest 4: Manual sum")
try:
    result = xy.eval("1 + 2 + 3;")
    print(f"   Manual sum: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 5: Different reduce syntax
print("\nTest 5: Different reduce syntax")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { return acc + x; }, 0);")
    print(f"   With return: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 6: Check if reduce exists
print("\nTest 6: Check reduce method")
try:
    result = xy.eval("[1, 2, 3].reduce;")
    print(f"   Reduce method: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 7: Try with explicit function
print("\nTest 7: Explicit function")
try:
    xy.eval("$global.add = op(a, b) { a + b; };")
    result = xy.eval("[1, 2, 3].reduce(add, 0);")
    print(f"   With explicit function: {result}")
except Exception as e:
    print(f"   Error: {e}")

print("\n=== Debug completed ===") 