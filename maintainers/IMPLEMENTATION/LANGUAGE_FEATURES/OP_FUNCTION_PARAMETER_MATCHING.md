# OP Function Parameter Matching Implementation

## Overview

This document describes the internal implementation of parameter matching in Grapa's `op` function, including how both `=` and `:` syntax are supported for named parameters.

## Key Components

### 1. Parameter Definition Storage

**Location**: `source/grapa/GrapaState.cpp` - `AddRuleOperation` function (line 2588)

**Implementation**:
```cpp
GrapaRuleEvent* GrapaScriptState::AddRuleOperation(GrapaRuleQueue* pRuleEvent, const char* pName, const char* pValue, s64 a, s64 b, s64 c, s64 d)
{
    // Creates operation with parameter namespace
    GrapaRuleEvent* operation = new GrapaRuleEvent(GrapaTokenType::RULEOP, 
        (pName&&*pName) ? pRuleEvent->HashId(pName, 0) : 0, pName, pValue);
    
    // Parameter namespace is stored in operation->vQueue
    operation->vQueue = new GrapaRuleQueue();
    
    // Default values are stored as GrapaRuleEvent objects
    // Example: op(a=1,b=2) creates namespace {"a":1, "b":2}
}
```

### 2. Parameter Matching Logic

**Location**: `source/grapa/GrapaLibRule.cpp` - `GrapaLibraryRuleOpEvent::Run` function (line 8815)

**Core Algorithm**:
```cpp
GrapaRuleEvent* GrapaLibraryRuleOpEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    // Create local namespace for function execution
    GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", "");
    
    // Process input parameters
    GrapaRuleEvent *e = pInput ? pInput->Head() : NULL;
    while (e)
    {
        // Create parameter binding using mName field
        GrapaRuleEvent *v = new GrapaRuleEvent(GrapaTokenType::PTR, 0, (char*)e->mName.mBytes);
        GrapaRuleEvent *eH = e;
        while (eH && eH->mValue.mToken == GrapaTokenType::PTR) eH = eH->vRulePointer;
        v->vRulePointer = eH;
        operation->vQueue->PushTail(v);
        e = e->Next();
    }
    
    // Execute function with bound parameters
    result = vScriptExec->ProcessPlan(pNameSpace, code);
}
```

### 3. Parameter Name Resolution

**Key Insight**: The parameter matching is **name-based**, not syntax-based. Both `=` and `:` syntax produce the same `GrapaRuleEvent` structure with the parameter name stored in `mName.mBytes`.

**Example**:
```grapa
f = op(a=1, b=2) { a + b; };

// These produce identical parameter structures:
f(a=3, b=4);     /* Traditional syntax */
f(a:3, b:4);     /* JSON-style syntax */
```

**C++ Structure**:
```cpp
// Both syntaxes create GrapaRuleEvent with:
e->mName.mBytes = "a" or "b"  // Parameter name
e->mValue = 3 or 4           // Parameter value
```

## Parameter Matching Rules

### 1. Named Parameter Resolution

**Process**:
1. Input parameters are processed in order
2. Each parameter's `mName` field is used to match against function definition
3. Parameter values are bound to the local namespace
4. Default values are used for unmatched parameters

**Example**:
```grapa
f = op(a=1, b=2, c=3) { a + b + c; };

f(10, b:20);  /* Results in: a=10, b=20, c=3 */
```

### 2. Positional Parameter Resolution

**Process**:
1. Parameters without names are assigned in definition order
2. Named parameters can be interspersed with positional parameters
3. Unassigned parameters use default values

**Example**:
```grapa
f = op(a=1, b=2, c=3) { a + b + c; };

f(10, 20, 30);     /* a=10, b=20, c=30 */
f(10, b:20, 30);   /* a=10, b=20, c=30 */
f(b:20, 10, 30);   /* a=10, b=20, c=30 */
```

### 3. Mixed Parameter Resolution

**Algorithm**:
1. Process named parameters first (by name matching)
2. Fill remaining slots with positional parameters (in order)
3. Apply default values to unassigned parameters

## Why Both `=` and `:` Work

### Design Rationale

1. **Traditional Programming Syntax**: `=` is the standard assignment operator
2. **JSON Compatibility**: `:` is the standard JSON property separator
3. **Unified Parameter Model**: Both syntaxes produce identical internal structures

### Implementation Details

**Lexer Processing**:
- Both `=` and `:` are recognized as parameter separators
- The lexer produces identical token structures for both
- Parameter names and values are stored identically

**Parser Processing**:
- Parameter definitions are stored with default values
- Parameter calls are processed by name, not syntax
- The `mName` field is the key for parameter matching

## Performance Characteristics

### Parameter Binding Performance

1. **Name-based Lookup**: O(n) where n is number of parameters
2. **Namespace Creation**: O(1) - single operation creation
3. **Parameter Copying**: O(n) - each parameter copied to local namespace
4. **Memory Management**: Automatic cleanup via `PopEvent`

### Optimization Opportunities

1. **Parameter Name Hashing**: Could use hash-based lookup for large parameter sets
2. **Default Value Caching**: Default values could be pre-computed
3. **Parameter Order Optimization**: Frequently used parameter orders could be cached

## Error Handling

### Parameter Mismatch Scenarios

1. **Missing Required Parameters**: Uses default values or returns error
2. **Extra Parameters**: Ignored (no error)
3. **Invalid Parameter Names**: Returns error
4. **Type Mismatches**: Handled by Grapa's type system

### Error Recovery

```cpp
// In GrapaLibraryRuleOpEvent::Run
if (result && result->mValue.mToken == GrapaTokenType::ERR) {
    // Parameter binding error - return error to caller
    return result;
}
```

## Integration with Other Systems

### 1. JSON Integration

The `:` syntax enables seamless JSON integration:
```grapa
config = {"operation": "add", "x": 5, "y": 3};
calculator(operation:config.operation, x:config.x, y:config.y);
```

### 2. Dynamic Parameter Construction

Parameters can be built programmatically:
```grapa
params = {"a": 10, "b": 20};
f(a:params.a, b:params.b);
```

### 3. Template Systems

Parameter matching supports template systems:
```grapa
template = op(params) {
    "Result: ${params.x + params.y}".interpolate();
};
template(x:5, y:3);
```

## Future Enhancements

### Potential Improvements

1. **Parameter Validation**: Runtime type checking for parameters
2. **Parameter Documentation**: Built-in parameter documentation support
3. **Parameter Aliases**: Support for parameter aliases/alternatives
4. **Parameter Groups**: Support for grouped parameter sets

### Backward Compatibility

All current parameter matching behavior will be preserved:
- Both `=` and `:` syntax continue to work
- Positional and named parameter mixing continues to work
- Default values continue to work as expected

## Testing Considerations

### Test Cases

1. **Basic Parameter Matching**:
   ```grapa
   f = op(a=1, b=2) { a + b; };
   assert(f(3, 4) == 7);
   assert(f(a:3, b:4) == 7);
   ```

2. **Mixed Parameter Usage**:
   ```grapa
   f = op(a=1, b=2, c=3) { a + b + c; };
   assert(f(10, b:20) == 33);
   assert(f(b:20, 10) == 33);
   ```

3. **JSON Integration**:
   ```grapa
   config = {"x": 5, "y": 3};
   f = op(x=0, y=0) { x * y; };
   assert(f(x:config.x, y:config.y) == 15);
   ```

## Conclusion

The parameter matching system in Grapa's `op` function is designed for flexibility and JSON compatibility. The dual syntax support (`=` and `:`) provides both traditional programming language feel and seamless JSON integration, while maintaining a unified internal parameter model.

The implementation is efficient and extensible, supporting complex parameter scenarios while maintaining backward compatibility and clear error handling. 