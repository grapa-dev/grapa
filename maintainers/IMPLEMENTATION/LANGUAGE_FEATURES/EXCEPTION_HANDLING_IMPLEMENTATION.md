---
tags:
  - maintainer
  - implementation
  - exception-handling
  - try-catch-finally
  - throw
---

# Exception Handling Implementation: try/catch/finally/throw

## Overview

Grapa's exception handling system provides comprehensive error handling with try/catch/finally blocks and throw statements. The implementation leverages the existing switch/case infrastructure for consistency and code reuse.

## Architecture

### **Core Components**

1. **Throw Statement**: `throw` with optional variable binding
2. **Try Block**: Code that might throw exceptions
3. **Catch Blocks**: Exception handlers with pattern matching
4. **Finally Block**: Always-executed cleanup code
5. **Variable Binding**: Exception variables accessible in catch blocks

### **Implementation Strategy**

The exception handling system uses a **merged approach** with the switch statement:
- **Same Event Handler**: Both `switch` and `try` use `GrapaLibraryRuleSwitchEvent::Run`
- **Name-based Logic**: Handler checks `mName.Cmp("try")` to determine behavior
- **Unified Grammar**: Both use `<case_list>` for consistency

## Grammar Implementation

### **BNF Rules**

```grapa
// Throw statements
| throw '(' <$comp> ')' {@<throw,{$3}>}
| throw <$comp> {@<throw,{$2}>}

// Try/catch/finally statements
| try <$command> '{' <$case_list> '}' finally <$command> {@<try,{$2,$4,$7}>}
| try <$command> '{' <$case_list> '}' {@<try,{$2,$4,null}>}
```

### **Event Handler Registration**

```cpp
// Both switch and try use the same handler
{ "switch", &GrapaLibraryRuleEvent::HandleSwitch },
{ "try", &GrapaLibraryRuleEvent::HandleSwitch },
```

## C++ Implementation

### **Event Handler Structure**

```cpp
GrapaRuleEvent* GrapaLibraryRuleSwitchEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    // Check if this is a try statement
    if (mName.Cmp("try") == 0 && rx1 && rx1->mControlFlow != GrapaControlFlowType::THROW) {
        // Execute finally block if present
        // Return result
    }
    
    // Switch statement logic (default behavior)
    // Execute case matching logic
    // Execute finally block if present
}
```

### **Throw Event Handler**

```cpp
GrapaRuleEvent* GrapaLibraryRuleThrowEvent::Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = NULL;
    GrapaRuleEvent* p1 = pInput->Head(0);
    
    if (p1) {
        if (p1->mValue.mToken == GrapaTokenType::OP)
            result = vScriptExec->ProcessPlan(pNameSpace, p1);
        else
            result = vScriptExec->CopyItem(p1);
    } else {
        result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
    }
    
    result->mControlFlow = GrapaControlFlowType::THROW;
    return(result);
}
```

## Key Features

### **1. Variable Binding in Throw**

```grapa
throw (err:'error message');  // Creates variable 'err' accessible in catch
```

**Implementation Notes:**
- Variable binding requires enhancement to store variable name/value pairs
- Catch blocks need access to exception variables
- Scope management for exception variables

### **2. Flexible Catch Conditions**

```grapa
catch 'error'              // Simple constant matching
catch (err.left(1)=='e')   // Complex expression matching
catch (true)               // Catch-all condition
```

**Implementation Notes:**
- Uses existing case matching logic from switch statements
- Supports complex boolean expressions
- Consistent with switch/case syntax

### **3. Finally Block Execution**

```grapa
try {
    // Code that might throw
} {
    catch 'error': 'Handle error'.echo();
} finally: 'Always execute'.echo();
```

**Implementation Notes:**
- Always executes regardless of exception
- Preserves return values and control flow
- Executes after try/catch completion

### **4. Multiple Catch Blocks**

```grapa
try {
    throw 'network error';
} {
    catch 'network error': 'Network issue handled'.echo();
    catch 'file error': 'File issue handled'.echo();
    default: 'Any other error handled'.echo();
}
```

**Implementation Notes:**
- Sequential evaluation like switch/case
- First matching catch block executes
- Default catch handles unmatched exceptions

## Usage Patterns

### **Basic Exception Handling**

```grapa
try {
    result = divide(a, b);
} {
    catch 'division by zero': 'Cannot divide by zero'.echo();
    default: 'Other error occurred'.echo();
} finally: 'Cleanup resources'.echo();
```

### **Advanced Pattern Matching**

```grapa
try {
    throw (err:'network timeout');
} {
    catch (err.grep('network')): 'Network issue: '.interpolate({msg=err}).echo();
    catch (err.grep('timeout')): 'Timeout issue: '.interpolate({msg=err}).echo();
    default: 'Unknown error'.echo();
} finally: 'Close connections'.echo();
```

### **Resource Management**

```grapa
file = openFile('data.txt');
try {
    data = readFile(file);
    processData(data);
} {
    catch 'file not found': 'File missing'.echo();
    catch 'permission denied': 'Access denied'.echo();
} finally: closeFile(file);
```

## Integration Points

### **With Control Flow System**

- **Throw**: Sets `mControlFlow = GrapaControlFlowType::THROW`
- **Try/Catch**: Handles `THROW` control flow
- **Finally**: Preserves control flow flags

### **With Variable System**

- **Exception Variables**: Bound in throw, accessible in catch
- **Scope Management**: Local variables in catch blocks
- **Variable Persistence**: Exception variables available throughout catch

### **With String Interpolation**

- **Exception Variables**: Can be used in interpolate expressions
- **PTR Handling**: Exception variables may be PTR types requiring dereferencing

## Performance Characteristics

### **Exception Handling Overhead**

- **Minimal overhead** when no exceptions occur
- **Fast propagation** using existing control flow system
- **Efficient matching** using switch/case infrastructure
- **Memory management** with automatic cleanup

### **Optimization Strategies**

- **Exception objects** created only when thrown
- **Catch blocks** executed only when exceptions occur
- **Finally blocks** have minimal overhead
- **Variable binding** optimized for common cases

## Known Issues

### **1. PTR Dereferencing in Interpolate**

**Issue**: Exception variables (PTR types) not properly dereferenced in `interpolate()`
```grapa
// ❌ Doesn't work - PTR issue
'Error: ${err}'.interpolate().echo();

// ✅ Works - explicit parameter
'Error: ${msg}'.interpolate({msg=err}).echo();
```

**Status**: Under investigation

### **2. Variable Binding Enhancement**

**Issue**: Variable binding in throw statements needs enhancement for full functionality
```grapa
throw (err:'error message');  // Variable binding needs improvement
```

**Status**: Planned enhancement

## Future Enhancements

### **1. Enhanced Variable Binding**

- **Complete variable persistence** from throw to catch
- **Multiple exception variables** support
- **Type-safe exception handling**

### **2. Exception Types**

- **Built-in exception classes**
- **Exception inheritance hierarchy**
- **Type-based exception matching**

### **3. Performance Optimizations**

- **Exception object pooling**
- **Fast-path for common exceptions**
- **Compile-time exception analysis**

## Testing Strategy

### **Unit Tests**

- **Basic exception handling** scenarios
- **Variable binding** functionality
- **Finally block** execution
- **Multiple catch blocks** behavior

### **Integration Tests**

- **Control flow integration** with loops and functions
- **Variable scope** management
- **Memory management** and cleanup
- **Performance** under various conditions

### **Edge Cases**

- **Nested try/catch** blocks
- **Exception in finally** blocks
- **Control flow** with return/break/continue
- **Memory leaks** prevention

## Conclusion

The exception handling system provides a powerful and consistent approach to error handling in Grapa. By leveraging the existing switch/case infrastructure, it maintains consistency while adding comprehensive exception handling capabilities.

The merged approach with switch statements is elegant and functional, though some enhancements are planned for variable binding and PTR handling in string interpolation.
