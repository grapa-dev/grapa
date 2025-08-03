#!/usr/bin/env python3
"""
Test Different Reduce Syntax Patterns
"""

import grapapy

print("=== Testing Different Reduce Syntax ===")

xy = grapapy.grapa()

# Test 1: Using += (compound assignment)
print("Test 1: Using += (compound assignment)")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { acc += x; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 2: Using + (addition)
print("\nTest 2: Using + (addition)")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { acc + x; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 3: Using return statement
print("\nTest 3: Using return statement")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { return acc + x; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 4: Using explicit return with +=
print("\nTest 4: Using explicit return with +=")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { acc += x; return acc; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 5: Using explicit return with +
print("\nTest 5: Using explicit return with +")
try:
    result = xy.eval("[1, 2, 3].reduce(op(acc, x) { return acc + x; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 6: Check if reduce is implemented at all
print("\nTest 6: Check if reduce exists")
try:
    result = xy.eval("[1, 2, 3].reduce;")
    print(f"   Reduce method exists: {result}")
except Exception as e:
    print(f"   Error: {e}")

# Test 7: Try with range
print("\nTest 7: Try with range")
try:
    result = xy.eval("(3).range(0,1).reduce(op(acc, x) { acc += x; }, 0);")
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Error: {e}")

print("\n=== Testing completed ===") 