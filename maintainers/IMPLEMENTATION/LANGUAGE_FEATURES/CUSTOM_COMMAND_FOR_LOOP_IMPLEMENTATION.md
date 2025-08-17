---
tags:
  - maintainer
  - implementation
  - for-loop
  - grammar-extension
  - syntax-improvements
  - completed
---

# Native "for" Loop Implementation

## Overview

This document describes the **completed native implementation** of for loops in Grapa. The implementation uses a consolidated smart handler that supports all loop variations through a single C++ class.

## Status: ✅ COMPLETED

**Implementation Date:** January 2025  
**Status:** Production-ready with comprehensive testing  
**Grammar:** All variations route to single `@<for,{...}>` handler  
**C++ Class:** `GrapaLibraryRuleForEvent` with smart parameter detection

## Architecture

### 1. **Consolidated Grammar Rules**

All for loop variations now route to a single handler in `lib/grapa/$grapa.grc`:

```grapa
| foreach $ID in <$comp> <$command> {@<for,{$2,$4,$5}>}
| do <$command> while '(' <$comp> ')' {@<for,{$2,$5}>}
| for '(' <$comp> ';' <$comp> ';' <$comp> ')' <$command> {@<for,{$3,$5,$7,$9}>}
| for $ID from <$comp> to <$comp> step <$comp> <$command> {@<for,{$2,$4,$6,$8,$9}>}
| for $ID from <$comp> to <$comp> <$command> {@<for,{$2,$4,$6,$7}>}
| for $ID in <$comp> <$command> {@<for,{$2,$4,$5}>}
```

### 2. **Smart C++ Implementation**

The `GrapaLibraryRuleForEvent::Run` method intelligently determines loop type based on parameter count:

- **2 parameters**: `do-while` loop
- **3 parameters**: `for-in` loop (smart detection of numeric vs collection)
- **4 parameters**: `for-from` or `complex-for` (detected by first parameter type)
- **5 parameters**: `for-from-step` loop

## Implementation Details

### 3. **C++ Class Structure**

```cpp
class GrapaLibraryRuleForEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleForEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
    
private:
    GrapaRuleEvent* HandleDoWhile(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput);
    GrapaRuleEvent* HandleForIn(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput);
    GrapaRuleEvent* HandleForFromOrComplex(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput);
    GrapaRuleEvent* HandleForFrom(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput);
    GrapaRuleEvent* HandleComplexFor(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput);
    GrapaRuleEvent* HandleForFromStep(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleQueue* pInput);
};
```

### 4. **Key Implementation Features**

#### **Delayed Evaluation**
- Uses `ProcessPlan` instead of `GrapaLibraryParam` for re-evaluation on each iteration
- Ensures expressions are evaluated fresh each time through the loop

#### **PTR Token Handling**
- Properly dereferences `$PTR` tokens returned by `ProcessPlan`
- Pattern: `result ? (result->mValue.mToken == GrapaTokenType::PTR ? result->vRulePointer : result) : NULL`

#### **Native Variable Assignment**
- Uses `@` syntax with string expressions: `"@" + varName + " = " + value.ToString()`
- Executes via `ProcessPlan` to ensure proper namespace integration

#### **Smart Type Detection**
- `for-in` loops automatically detect numeric ranges vs collections
- `for-from` vs `complex-for` detected by first parameter type (`$ID` vs expression)

### 5. **Supported Loop Types**

#### **Numeric Range Loops**
```grapa
/* Basic range */
for i from 1 to 5 {
    (i).echo();
}

/* With step */
for i from 0 to 10 step 2 {
    (i).echo();
}
```

#### **Collection Loops**
```grapa
/* Array iteration */
for item in [1, 2, 3, 4, 5] {
    (item).echo();
}

/* String iteration */
for char in "Hello" {
    (char).echo();
}

/* List iteration */
for value in {a:1, b:2, c:3} {
    (value).echo();
}
```

#### **C-Style Loops**
```grapa
/* Traditional for loop */
for (i = 1; i <= 10; i = i + 1) {
    (i).echo();
}
```

#### **Do-While Loops**
```grapa
/* Execute at least once */
do {
    ("Executing...").echo();
} while (condition);
```

## Testing

All loop variations have been tested and are working correctly:

```bash
./grapa -c "for i from 1 to 3 (i).echo();"  # ✅ Works
./grapa -c "for i from 1 to 5 step 2 (i).echo();"  # ✅ Works  
./grapa -c "for i in 3 (i).echo();"  # ✅ Works
```

## Legacy Information

The following section documents the previous custom_command approach for reference:

### 2. **Rule Definition Pattern**

Custom commands follow this pattern:

```grapa
custom_command = rule <pattern> {op(param1:$2, param2:$4, ...){
    /* Implementation code here */
}};
```

Where:
- `<pattern>` defines the syntax to match
- `op(...)` creates an executable operation
- Parameters are captured using `$n` where n is the position
- The implementation is written in Grapa code within the `op()` block

## Implementation Examples

### 1. **Improved For Loop (Grapa-style)**

```grapa
custom_command = rule for $ID from <$comp> to <$comp> <$command> {
    op(var:$2, start:$4, end:$6, body:$8){
        /* Validate inputs */
        start_val = start();
        end_val = end();
        
        if (start_val.type() != "$INT" || end_val.type() != "$INT") {
            ("Error: Start and end must be integers").echo();
            return;
        };
        
        /* Set initial value using indirect variable assignment */
        @@var = start_val;
        
        /* Loop from start to end (inclusive) */
        while (@@var <= end_val) {
            /* Execute the loop body */
            body();
            
            /* Increment using indirect variable assignment */
            @@var += 1;
        };
    }
};
```

**Key Improvements:**
1. **Direct Evaluation**: `start()` and `end()` evaluate $OP parameters directly
2. **Indirect Assignment**: `@@var` syntax eliminates complex string concatenation
3. **Cleaner Syntax**: Much more readable and maintainable
4. **Better Performance**: No string operations for variable assignment

### 2. **For Loop with Configurable Step**

```grapa
custom_command = rule for $ID from <$comp> to <$comp> step <$comp> <$command> {
    op(var:$2, start:$4, end:$6, step:$8, body:$10){
        /* Validate inputs */
        start_val = start();
        end_val = end();
        step_val = step();
        
        if (start_val.type() != "$INT" || end_val.type() != "$INT" || step_val.type() != "$INT") {
            ("Error: Start, end, and step must be integers").echo();
            return;
        };
        
        if (step_val == 0) {
            ("Error: Step cannot be zero").echo();
            return;
        };
        
        /* Set initial value */
        @@var = start_val;
        
        /* Loop with configurable step */
        if (step_val > 0) {
            /* Forward iteration */
            while (@@var <= end_val) {
                body();
                @@var += step_val;
            };
        } else {
            /* Backward iteration */
            while (@@var >= end_val) {
                body();
                @@var += step_val;
            };
        };
    }
};
```
}};
```

**Usage:**
```grapa
for (i=0; i<5; i++) {
    ("Iteration " + i).echo();
};
```

### 2. **Range-Based For Loop (Python-style)**

```grapa
custom_command = rule for $ID from <$comp> to <$comp> <$command> {op(var:$2,start:$4,end:$6,body:$8){
    /* Set initial value */
    op()(var + " = " + start)();
    
    /* Loop from start to end */
    current = start;
    while (current <= end) {
        /* Execute the loop body */
        op()(body)();
        
        /* Increment the variable */
        current += 1;
        op()(var + " = " + current)();
    };
}};
```

**Usage:**
```grapa
for i from 1 to 5 {
    ("Iteration " + i).echo();
};
```

### 3. **Collection Iteration (JavaScript-style)**

```grapa
custom_command = rule for $ID in <$comp> <$command> {op(var:$2,items:$4,body:$6){
    /* Handle different collection types */
    if (items.type() == "$LIST" || items.type() == "$ARRAY") {
        i = 1;
        while (i <= items.len()) {
            /* Set the loop variable */
            op()(var + " = " + items[i])();
            
            /* Execute loop body */
            op()(body)();
            
            i += 1;
        };
    } else if (items.type() == "$STR") {
        /* String iteration */
        i = 1;
        while (i <= items.len()) {
            op()(var + " = \"" + items[i] + "\"")();
            op()(body)();
            i += 1;
        };
    } else {
        ("Error: Unsupported collection type").echo();
    };
}};
```

**Usage:**
```grapa
my_list = [10, 20, 30, 40, 50];
for item in my_list {
    ("Processing: " + item).echo();
};

for char in "Hello" {
    ("Character: " + char).echo();
};
```

## Key Implementation Details

### 1. **op()()() Pattern**

The `op()()()` pattern is crucial for dynamic code execution:

- `op()` - Creates an $OP object
- `op()(script)` - Compiles the script string into an executable operation
- `op()(script)()` - Executes the compiled operation

This allows the custom command to:
- Execute arbitrary Grapa code at runtime
- Set variables dynamically
- Evaluate expressions
- Execute loop bodies

### 2. **Parameter Capture**

Parameters are captured using positional references:

```grapa
custom_command = rule for $ID in <$comp> <$command> {op(var:$2,items:$4,body:$6){
    /* $2 = variable name (e.g., "i")
       $4 = items to iterate over
       $6 = loop body code */
}};
```

### 3. **Safety Considerations**

- **Infinite Loop Protection**: Always include a maximum iteration limit
- **Error Handling**: Validate input parameters and provide meaningful error messages
- **Variable Scope**: Be careful about variable name conflicts
- **Performance**: Consider the overhead of dynamic code compilation

### 4. **Integration with Existing Syntax**

Custom commands integrate seamlessly with existing Grapa syntax:

```grapa
/* Mix custom for loops with regular Grapa code */
result = 0;
for (i=1; i<=5; i++) {
    result += i * i;
};

/* Use with conditionals */
for (i=1; i<=10; i++) {
    if (i % 2 == 0) {
        ("Even: " + i).echo();
    };
};
```

## Testing and Validation

### 1. **Test Script Structure**

```grapa
/* Define the custom command */
custom_command = rule for $ID from <$comp> to <$comp> <$command> {op(var:$2,start:$4,end:$6,body:$8){
    /* Implementation */
}};

/* Test the command */
for i from 1 to 3 {
    ("Test iteration " + i).echo();
};
```

### 2. **Running Tests**

```bash
# Run the test script
./grapa test/use_cases/simple_for_loop_demo.grc

# Or execute via op()()() pattern
./grapa -c "op()($file().get('test/use_cases/simple_for_loop_demo.grc'))()"
```

## Advantages of This Approach

### 1. **No Core Language Changes**
- Extends grammar without modifying the core language
- Maintains backward compatibility
- Can be enabled/disabled per script

### 2. **Natural Syntax**
- Provides familiar syntax for developers from other languages
- Reduces learning curve
- Improves code readability

### 3. **Flexible Implementation**
- Can implement any loop pattern
- Supports different iteration styles
- Can add custom logic and error handling

### 4. **Runtime Extensibility**
- Commands can be defined at runtime
- Supports dynamic grammar modification
- Enables domain-specific language features

## Limitations and Considerations

### 1. **Performance Overhead**
- Dynamic code compilation has runtime cost
- Each loop iteration requires op()()() calls
- Not suitable for high-performance loops

### 2. **Debugging Complexity**
- Error messages may be less clear
- Stack traces can be complex
- Harder to debug than native syntax

### 3. **Scope and Variable Management**
- Need to be careful about variable scoping
- Dynamic variable assignment can be tricky
- Potential for variable name conflicts

## Future Enhancements

### 1. **Native Implementation**
For better performance, consider implementing for loops natively in the C++ backend:

```cpp
// In GrapaLibRule.cpp
else if (pName.Cmp("for") == 0) lib = new GrapaLibraryRuleForEvent(pName);
```

### 2. **Optimization**
- Compile-time optimization of common patterns
- Static analysis for loop bounds
- Dead code elimination

### 3. **Enhanced Syntax**
- Support for `break` and `continue` statements
- Nested loop optimization
- Parallel loop execution

## Conclusion

The `custom_command` mechanism provides a powerful way to extend Grapa's grammar with new syntax constructs like for loops. While this approach has some performance overhead, it offers significant benefits in terms of developer experience and language adoption.

For production use, consider implementing the most common patterns natively in the C++ backend for better performance, while keeping the `custom_command` approach available for specialized use cases and rapid prototyping.

## References

- [Grapa Rule System Architecture](RULE_SYSTEM_ARCHITECTURE.md)
- [Custom Command Examples](../test/use_cases/simple_for_loop_demo.grc)
- [Grapa Grammar Definition](lib/grapa/$grapa.grc)
- [Function Operators Documentation](../../docs-src/docs/operators/function.md) 