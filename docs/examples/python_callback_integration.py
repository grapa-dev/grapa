#!/usr/bin/env python3
"""
Python Callback Integration Examples
Demonstrates bidirectional communication between Python and Grapa using $py().eval()

This example follows the pattern from grapapy_test.ipynb where Python functions
are defined in the global scope and then called from Grapa using $py().eval()
"""

import grapapy

# Define Python functions in global scope so they're accessible to $py().eval()
def python_multiply(x, y):
    """Simple Python function to multiply two numbers"""
    return x * y

def classify_number(n):
    """Classify a number into categories"""
    if n >= 100:
        return "large"
    elif n >= 10:
        return "medium"
    else:
        return "small"

def calculate_operation(a, b):
    """Perform different operations based on values"""
    if a > b:
        return a - b
    elif a < b:
        return a + b
    else:
        return a * b

def safe_divide(a, b):
    """Safe division with error handling"""
    try:
        if b == 0:
            return {"error": "Division by zero", "result": None}
        return {"error": None, "result": a / b}
    except Exception as e:
        return {"error": str(e), "result": None}

def python_square(x):
    """Square a number"""
    return x ** 2

# Create a global counter instance
class Counter:
    """A simple counter class to demonstrate stateful callbacks"""
    def __init__(self):
        self.count = 0
    
    def increment(self):
        self.count += 1
        return self.count
    
    def get_count(self):
        return self.count
    
    def reset(self):
        self.count = 0
        return 0

counter = Counter()

def main():
    print("=== Python Callback Integration Examples ===\n")
    
    # Create GrapaPy instance
    g = grapapy.grapa()
    
    # Test basic Grapa functionality first
    print("Testing basic Grapa functionality...")
    basic_result = g.eval("2 + 3")
    print(f"Basic test (2 + 3): {basic_result}")
    print()
    
    # Example 1: Basic callback registration
    print("Example 1: Basic Callback Registration")
    print("-" * 40)
    
    # Register the Python function as a Grapa callback
    g.eval("""
        $this.multiply = op(x=0, y=0) {
            $py().eval('python_multiply(x, y)', {'x': x, 'y': y});
        };
    """)
    
    # Use the callback from Grapa
    result = g.eval("multiply(5, 3);")
    print(f"Python multiply(5, 3) = {result}")
    print()
    
    # Example 2: Rule-based callbacks
    print("Example 2: Rule-Based Callbacks")
    print("-" * 40)
    
    # Create a rule that uses Python callbacks
    g.eval("""
        $this.classify_rule = rule 
              $INT $INT {op(a:$1,b:$2) {$py().eval('calculate_operation(v1,v2)', {"v1":a,"v2":b});}}
            | $INT      {op(a:$1)      {$py().eval('classify_number(v)',     {"v":a});}}
            ;
    """)
    
    # Test the rule with different inputs
    result1 = g.eval("$sys().eval('5', {}, 'classify_rule');")
    print(f"Classify 5: {result1}")
    
    result2 = g.eval("$sys().eval('8 5', {}, 'classify_rule');")
    print(f"Calculate 8,5: {result2}")
    print()
    
    # Example 3: Error handling
    print("Example 3: Error Handling")
    print("-" * 40)
    
    # Register error handling function
    g.eval("""
        $this.safe_div = op(a=0, b=1) {
            $py().eval('safe_divide(a, b)', {'a': a, 'b': b});
        };
    """)
    
    # Test normal division
    result = g.eval("safe_div(10, 2);")
    print(f"Safe divide 10/2: {result}")
    
    # Test division by zero
    result = g.eval("safe_div(10, 0);")
    print(f"Safe divide 10/0: {result}")
    print()
    
    # Example 4: Stateful Python functions
    print("Example 4: Stateful Python Functions")
    print("-" * 40)
    
    # Register counter methods
    g.eval("""
        $this.increment = op() {
            $py().eval('counter.increment()', {});
        };
        
        $this.get_count = op() {
            $py().eval('counter.get_count()', {});
        };
        
        $this.reset_count = op() {
            $py().eval('counter.reset()', {});
        };
    """)
    
    # Test stateful operations
    result = g.eval("increment();")
    print(f"Increment: {result}")
    
    result = g.eval("increment();")
    print(f"Increment: {result}")
    
    result = g.eval("get_count();")
    print(f"Get count: {result}")
    
    result = g.eval("reset_count();")
    print(f"Reset count: {result}")
    
    result = g.eval("get_count();")
    print(f"Get count after reset: {result}")
    print()
    
    # Example 5: Compiled code with callbacks
    print("Example 5: Compiled Code with Callbacks")
    print("-" * 40)
    
    # Compile Grapa code that uses Python callbacks
    compiled = g.compile("$py().eval('python_square(n)', {'n': $local.'n'});")
    
    # Use compiled code multiple times
    for i in range(1, 6):
        result = g.eval(compiled, {'n': i})
        print(f"Square of {i}: {result}")
    
    print("\n=== All Examples Completed Successfully! ===")

if __name__ == "__main__":
    main()