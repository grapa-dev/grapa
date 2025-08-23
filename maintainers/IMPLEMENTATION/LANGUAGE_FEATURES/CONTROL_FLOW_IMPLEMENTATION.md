---
tags:
  - maintainer
  - implementation
  - control-flow
  - runtime-flow-controls
  - phase-1
  - planning
---

# Control Flow Implementation

## Overview

This document describes the current state and implementation requirements for Grapa's control flow system, with particular focus on the **runtime flow controls** that need proper implementation for Phase 1 work.

## High-Level Architecture

### **Compiler Pipeline**
```
Bytes → Tokens → Rules → Compiled Execution Trees
```

1. **Lexical Analysis**: Converts source code bytes into tokens
2. **Parsing**: Converts tokens into grammar rules
3. **Compilation**: Converts rules into execution trees
4. **Execution**: Traverses execution trees calling appropriate handlers

### **Execution Engine**
```
Execution Plan → Tree Traversal → Handler Calls → Result
```

- **Execution Plan**: Contains the compiled execution tree
- **Tree Traversal**: Walks through the tree structure
- **Handler Calls**: Invokes appropriate C++ event handlers
- **Result**: Returns computed values or control flow signals

## Current Control Flow Implementation Status

### **✅ Working Control Flow Constructs**

#### **1. Conditional Statements**
- **`if`/`if-else`**: Fully implemented in `GrapaLibraryRuleIfEvent::Run`
- **`switch`**: Fully implemented in `GrapaLibraryRuleSwitchEvent::Run`
- **Ternary Operator**: Native C++ operator `condition ? value1 : value2`

#### **2. Loop Constructs**
- **`while`**: Fully implemented in `GrapaLibraryRuleWhileEvent::Run`
- **`for` loops**: ✅ **COMPLETED** - Consolidated implementation in `GrapaLibraryRuleForEvent::Run`
- **`do-while`**: ✅ **COMPLETED** - Integrated into for loop implementation

#### **3. Scope Management**
- **`{}` blocks**: Implemented in `GrapaLibraryRuleScopeEvent::Run`
- **Variable scoping**: Automatic scope-based cleanup
- **Memory management**: Deterministic cleanup on scope exit

### **✅ Runtime Flow Controls - COMPLETED**

#### **Current Status: Fully Implemented and Working**

The following runtime flow controls are now **fully functional** with proper control flow propagation:

1. **`break`** - ✅ **WORKING** - Properly terminates loops and propagates through execution stack
2. **`continue`** - ✅ **WORKING** - Properly skips to next iteration in loops
3. **`return`** - ✅ **WORKING** - Properly exits functions with return values and propagates through all contexts
4. **`exit`** - ✅ **WORKING** - Properly terminates program execution

#### **Key Implementation Areas Fixed**

**1. Function Calls (`GrapaLibraryRuleCallEvent::Run`)**
- ✅ **Fixed**: RETURN control flow properly terminated at function boundaries
- ✅ **Working**: Return values correctly propagated from function calls

**2. Switch Statements (`GrapaLibraryRuleSwitchEvent::Run`)**
- ✅ **Fixed**: RETURN control flow properly terminated at switch boundaries
- ✅ **Working**: Return values correctly propagated from switch cases

**3. Inline Code Blocks (`GrapaLibraryRuleSearchEvent::Run`)**
- ✅ **Fixed**: RETURN control flow properly terminated at inline code block boundaries
- ✅ **Working**: Return values correctly propagated from inline code blocks
- ✅ **Added**: Control flow detection during `.` notation iteration
- ✅ **Added**: Early termination for RETURN, THROW, SYNTAX, EXIT
- ✅ **Added**: Conditional cleanup only when no active control flow

**4. Reduce Operations (`GrapaLibraryRuleReduceEvent::Run`)**
- ✅ **Fixed**: Control flow properly terminated at reduce operation boundaries
- ✅ **Working**: Return values correctly propagated from reduce operations
- ✅ **Added**: Control flow detection during reduce iteration
- ✅ **Added**: Early termination for BREAK, RETURN, THROW, SYNTAX, EXIT
- ✅ **Added**: Proper accumulator value handling with control flow

**5. Functional Methods Control Flow**
- ✅ **Map Operations**: `.throw()`, `.return()`, `.break()` control flow implemented and working
- ✅ **Reduce Operations**: `.throw()`, `.return()`, `.break()` control flow implemented and working  
- ✅ **Filter Operations**: `.throw()`, `.return()`, `.break()` control flow implemented and working
- ✅ **Parallel Methods**: `.throw()` collects all results in array, `.return()`/`.break()` are thread-local
- ✅ **Sequential Methods**: `.reduce()` supports all control flow (sequential processing)
- ✅ **Thread Independence**: Each thread processes independently, control flow affects only that thread
- ✅ **Enhanced Error Collection**: `.map()` and `.filter()` collect all `.throw()` results in arrays

## Runtime Flow Control Implementation Analysis

### **1. Break Statement**

#### **Current Implementation**
```cpp
class GrapaLibraryRuleBreakEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleBreakEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};

GrapaRuleEvent* GrapaLibraryRuleBreakEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
    result->mControlFlow = GrapaControlFlowType::BREAK;  // Sets break flag
    return(result);
}
```

#### **Grammar Definition**
```grapa
| break {@<break,{}>}
```

#### **Status: ✅ Working**
- **Flag Recognition**: `mControlFlow` flag is properly set and checked throughout execution
- **Loop Context**: All loop types properly check for `BREAK` control flow
- **Scope Cleanup**: Break triggers proper scope cleanup
- **Memory Management**: Objects created before break are properly cleaned up

### **2. Continue Statement**

#### **Current Implementation**
```cpp
class GrapaLibraryRuleContinueEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleContinueEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};

GrapaRuleEvent* GrapaLibraryRuleContinueEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
    result->mControlFlow = GrapaControlFlowType::CONTINUE;  // Sets continue flag
    return(result);
}
```

#### **Grammar Definition**
```grapa
| continue {@<continue,{}>}
```

#### **Status: ✅ Working**
- **Flag Set**: Continue properly sets `CONTINUE` control flow flag
- **Loop Recognition**: All loops properly check for continue signals
- **Scope Handling**: Continue skips remaining loop body but maintains scope
- **Memory Management**: Objects created in current iteration are properly cleaned up

### **3. Return Statement**

#### **Current Implementation**
```cpp
class GrapaLibraryRuleReturnEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleReturnEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};

GrapaRuleEvent* GrapaLibraryRuleReturnEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = NULL;
    GrapaRuleEvent* p1 = pInput->Head(0);
    if (p1->mValue.mToken == GrapaTokenType::OP)
    {
        GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, p1);
        if (rx1)
        {
            result = rx1;  // Return value is properly processed
            result->mControlFlow = GrapaControlFlowType::RETURN;  // Sets return flag
        }
    }
    return(result);  // Returns the return value with control flow flag
}
```

#### **Grammar Definition**
```grapa
| return '(' <$comp> ')' {@<return,{$3}>}
```

#### **Status: ✅ Working**
- **Return Value Handling**: Return value processing is fully implemented
- **Return Flag**: Properly sets `RETURN` control flow flag
- **Function Context**: Functions properly check for return signals
- **Scope Cleanup**: Function scope cleanup on return is complete
- **Memory Management**: Objects created in function are properly cleaned up
- **Propagation**: RETURN control flow properly propagates through all execution contexts

### **4. Exit Statement**

#### **Current Implementation**
```cpp
class GrapaLibraryRuleExitEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleExitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};

GrapaRuleEvent* GrapaLibraryRuleExitEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
    result->mControlFlow = GrapaControlFlowType::EXIT;  // Sets exit flag
    return(result);
}
```

#### **Grammar Definition**
```grapa
| exit {@<exit,{}>}
```

#### **Status: ✅ Working**
- **Complete Implementation**: Exit handler implementation is complete
- **Exit Flag**: Properly sets `EXIT` control flow flag
- **Cleanup Handling**: Proper cleanup of global resources
- **Program Termination**: Correctly terminates program execution

## Control Flow Propagation System

### **✅ Complete Flag Propagation System**

The control flow system now has **complete flag propagation** throughout the execution stack:

#### **1. Control Flow Flags**
Each runtime flow control uses the `mControlFlow` flag in `GrapaRuleEvent`:

```cpp
class GrapaRuleEvent
{
public:
    u8 mControlFlow;  // Control flow type flag
    // Values: BREAK, CONTINUE, RETURN, THROW, SYNTAX, EXIT
};
```

#### **2. Flag Propagation System**
Control flow flags are properly propagated through the execution stack:

```cpp
// In ProcessPlan and related execution methods
if (result && result->mControlFlow == GrapaControlFlowType::BREAK) {
    // Handle break - stop current loop
    return result;
}
if (result && result->mControlFlow == GrapaControlFlowType::CONTINUE) {
    // Handle continue - skip to next iteration
    return result;
}
if (result && result->mControlFlow == GrapaControlFlowType::RETURN) {
    // Handle return - exit current function
    return result;
}
if (result && result->mControlFlow == GrapaControlFlowType::EXIT) {
    // Handle exit - terminate program
    return result;
}
```

#### **3. Context Recognition**
All execution contexts properly check for control flow flags:

**Loop Contexts:**
```cpp
// In while loops, for loops, etc.
while (condition) {
    GrapaRuleEvent* result = ProcessPlan(namespace, body);
    if (result->mControlFlow == GrapaControlFlowType::BREAK) {
        // Break detected - exit loop
        break;
    }
    if (result->mControlFlow == GrapaControlFlowType::CONTINUE) {
        // Continue detected - skip to next iteration
        continue;
    }
    if (result->mControlFlow == GrapaControlFlowType::RETURN) {
        // Return detected - propagate up
        return result;
    }
}
```

**Function Contexts:**
```cpp
// In function execution
GrapaRuleEvent* result = ProcessPlan(namespace, functionBody);
if (result->mControlFlow == GrapaControlFlowType::RETURN) {
    // Return detected - exit function with return value
    // Control flow flag is cleared at function boundary
    result->mControlFlow = 0;
    return result;
}
```

**Inline Code Block Contexts:**
```cpp
// In GrapaLibraryRuleSearchEvent::Run for inline code blocks
// During . notation iteration
u8 isControlFlowChange = e->mControlFlow;
if (isControlFlowChange == GrapaControlFlowType::BREAK)
    break;
if (isControlFlowChange == GrapaControlFlowType::CONTINUE)
    ;  // Continue to next iteration
if (isControlFlowChange == GrapaControlFlowType::RETURN || 
    isControlFlowChange == GrapaControlFlowType::THROW || 
    isControlFlowChange == GrapaControlFlowType::SYNTAX || 
    isControlFlowChange == GrapaControlFlowType::EXIT)
{
    break;  // Exit . notation processing
}

// After . notation completion
if (attrList && e && !e->mControlFlow)
    e = NULL;  // Only cleanup if no active control flow
```

### **✅ Memory Management Integration**

#### **1. Scope Cleanup on Control Flow**
Control flow statements properly trigger scope cleanup:

```cpp
// When control flow is detected
if (controlFlowFlag) {
    // Clean up current scope
    CleanupScope(currentScope);
    
    // Propagate control flow signal
    return controlFlowResult;
}
```

#### **2. Object Lifecycle Management**
Objects created before control flow are properly cleaned up:

```cpp
// Before executing control flow statement
CleanupTemporaryObjects();
SetControlFlowFlag(flagType);
return controlFlowResult;
```

## Verification Results

### **✅ Control Flow Propagation Tests**

All control flow statements now properly propagate through all execution contexts:

#### **1. Function Return Propagation**
```grapa
func test() { 
    "hi".{if (true) return 999; x=@$$; x.len()}.range(); 
    return 888; 
}; 
test()  // Returns: 999 ✅
```

#### **2. Switch Return Propagation**
```grapa
switch (1) { 
    case 1: "hi".{if (true) return 999; x=@$$; x.len()}.range(); 
    return 888; 
    default: return 777; 
}  // Returns: 999 ✅
```

#### **3. Loop Return Propagation**
```grapa
func test() { 
    while (true) { 
        if (true) return 999; 
    }; 
    return 888; 
}; 
test()  // Returns: 999 ✅
```

#### **4. Exception Propagation**
```grapa
func test() { 
    "hi".{if (true) throw "error"; x=@$$; x.len()}.range(); 
    return 888; 
}; 
test()  // Throws: error ✅
```

#### **5. Reduce Control Flow Propagation**
```grapa
/* Break - terminates reduce and returns current accumulator */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) break; acc + x; });  // Returns: 1 ✅
[1, 2, 3].reduce(op(acc, x) { if (x == 2) break; acc + x; }, 10);  // Returns: 11 ✅

/* Return - exits reduce and returns specified value */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) return 999; acc + x; });  // Returns: 999 ✅
[1, 2, 3].reduce(op(acc, x) { if (x == 2) return 999; acc + x; }, 10);  // Returns: 999 ✅

/* Throw - propagates exception */
[1, 2, 3].reduce(op(acc, x) { if (x == 2) throw "error"; acc + x; });  // Throws: error ✅

/* Control flow propagates through function calls */
func test() { 
    [1, 2, 3].reduce(op(acc, x) { if (x == 2) return 999; acc + x; }); 
    return 888; 
}; 
test();  // Returns: 999 ✅
```

#### **6. Nested Context Propagation**
```grapa
func test() { 
    switch (1) { 
        case 1: "hi".{if (true) return 999; x=@$$; x.len()}.range(); 
        return 888; 
        default: return 777; 
    }; 
}; 
test()  // Returns: 999 ✅
```

## Implementation Status Summary

### **✅ Phase 1 Complete: Runtime Flow Controls**

All runtime flow controls are now **fully implemented and working**:

1. **✅ Break Statement**: Properly terminates loops and propagates through execution stack
2. **✅ Continue Statement**: Properly skips to next iteration in loops
3. **✅ Return Statement**: Properly exits functions with return values and propagates through all contexts
4. **✅ Exit Statement**: Properly terminates program execution

### **✅ Key Implementation Areas Completed**

1. **✅ Control Flow Flags**: Complete `mControlFlow` flag system implemented
2. **✅ Flag Propagation**: Complete propagation through execution stack
3. **✅ Context Recognition**: All execution contexts properly handle control flow
4. **✅ Memory Management**: Proper scope cleanup and object lifecycle management
5. **✅ Function Boundaries**: RETURN control flow properly terminated at function boundaries
6. **✅ Switch Boundaries**: RETURN control flow properly terminated at switch boundaries
7. **✅ Inline Code Blocks**: RETURN control flow properly terminated at inline code block boundaries

### **✅ Testing Complete**

- **Unit Tests**: All control flow statements tested in isolation
- **Integration Tests**: Control flow tested in loops, functions, and nested contexts
- **Memory Tests**: No memory leaks with control flow
- **Propagation Tests**: Control flow properly propagates through all execution contexts

## Conclusion

The runtime flow controls (`break`, `continue`, `return`, `exit`) are now **fully implemented and functional**. The control flow system provides:

- **Complete flag propagation** throughout execution stack
- **Proper context recognition** for all execution contexts
- **Memory management integration** with control flow
- **Comprehensive testing** of all control flow scenarios

**Key Achievements:**
1. **Function Calls**: RETURN control flow properly terminated at function boundaries
2. **Switch Statements**: RETURN control flow properly terminated at switch boundaries  
3. **Inline Code Blocks**: RETURN control flow properly terminated at inline code block boundaries
4. **Control Flow Propagation**: All control flow statements properly propagate through all execution contexts

The control flow system is now **production-ready** and provides a solid foundation for exception handling and other advanced language features.

## Future: Exception Handling (try/catch)

### **Dependencies: Runtime Flow Controls Complete ✅**

Exception handling (`try/catch`) can now be implemented using the **complete flag propagation and context recognition system** that is working for runtime flow controls.

### **Exception Handling Requirements**

#### **1. Exception Flow Flags**
```cpp
class GrapaRuleEvent
{
public:
    // Existing control flow flags
    u8 mControlFlow;  // BREAK, CONTINUE, RETURN, THROW, SYNTAX, EXIT
    
    // Exception handling flags (future)
    GrapaRuleEvent* mExceptionValue;  // For exception objects
    GrapaRuleEvent* mExceptionHandler;  // For catch block
};
```

#### **2. Exception Propagation System**
```cpp
// In ProcessPlan and related execution methods
if (result && result->mControlFlow == GrapaControlFlowType::THROW) {
    // Handle exception - look for catch block
    return HandleException(result->mExceptionValue);
}
```

#### **3. Try/Catch Context Recognition**
```cpp
// In try block execution
try {
    GrapaRuleEvent* result = ProcessPlan(namespace, tryBody);
    if (result->mControlFlow == GrapaControlFlowType::THROW) {
        // Exception detected - execute catch block
        return ExecuteCatchBlock(result->mExceptionValue, catchBody);
    }
    return result;
} catch (exception) {
    // Catch block execution
    return ProcessPlan(namespace, catchBody);
}
```

### **Implementation Strategy for Exception Handling**

#### **Phase 2 Implementation Plan (Runtime Flow Controls Complete ✅)**

1. **Add Exception Flags** (Week 1)
   - Add `mExceptionValue`, `mExceptionHandler` fields
   - Update flag initialization in constructors

2. **Implement Exception Propagation** (Week 2)
   - Update `ProcessPlan` to check and propagate exception flags
   - Implement exception stack unwinding
   - Add exception context tracking

3. **Create Try/Catch Handlers** (Week 3)
   - Implement `GrapaLibraryRuleTryEvent::Run`
   - Implement `GrapaLibraryRuleCatchEvent::Run`
   - Add try/catch grammar rules

4. **Exception Object System** (Week 4)
   - Implement exception object creation
   - Add exception type hierarchy
   - Implement exception message handling

5. **Integration with Control Flow** (Week 4)
   - Ensure exceptions work with break/continue/return
   - Handle nested try/catch blocks
   - Test exception propagation through functions

### **Grammar Requirements for Exception Handling**

```grapa
| try <$command> catch '(' $ID ')' <$command> {@<try,{$2,$6,$7}>}
| throw '(' <$comp> ')' {@<throw,{$3}>}
```

### **Memory Management for Exception Handling**

#### **Exception-Safe Cleanup**
```cpp
// When exception is thrown
if (exceptionThrown) {
    // Clean up current scope
    CleanupScope(currentScope);
    
    // Unwind exception stack
    UnwindExceptionStack();
    
    // Execute catch block or propagate
    return exceptionResult;
}
```

#### **Exception Object Lifecycle**
```cpp
// Exception objects must be properly managed
GrapaRuleEvent* exception = CreateExceptionObject(message, type);
exception->mControlFlow = GrapaControlFlowType::THROW;
exception->mExceptionValue = exception;
return exception;
```

### **Testing Strategy for Exception Handling**

1. **Basic Exception Tests**
   - Test try/catch with simple exceptions
   - Test exception propagation
   - Test exception object creation

2. **Complex Exception Tests**
   - Test nested try/catch blocks
   - Test exceptions in loops and functions
   - Test exception with control flow statements

3. **Memory Tests**
   - Test for memory leaks with exceptions
   - Test object cleanup during exception unwinding
   - Test long-running programs with exceptions

### **Dependencies and Prerequisites**

#### **Completed ✅:**
1. ✅ **Runtime Flow Controls** - Flag propagation system
2. ✅ **Memory Management** - Scope cleanup and object lifecycle
3. ✅ **Context Recognition** - Loop and function context handling
4. ✅ **Flag Propagation** - Complete execution stack integration

#### **Exception Handling Adds:**
1. **Exception Stack** - Track exception context and handlers
2. **Exception Objects** - Create and manage exception instances
3. **Exception Unwinding** - Proper stack unwinding during exceptions
4. **Exception Types** - Exception hierarchy and type checking

### **Conclusion**

Exception handling (`try/catch`) is a **natural extension** of the runtime flow control system. It requires the same fundamental infrastructure:

- **Flag propagation system** for exception signals
- **Context recognition** for try/catch blocks
- **Memory management** for exception-safe cleanup
- **Stack unwinding** for proper exception handling

**Implementation Order:**
1. ✅ **Phase 1**: Complete runtime flow controls (break, continue, return, exit)
2. ✅ **Phase 1.5**: Functional methods control flow (`.throw()` in `.map()`/`.reduce()`)
3. **Phase 2**: Implement exception handling (try/catch/throw)

The foundational control flow system is now **solid and complete**, with functional methods supporting `.throw()` control flow. The system is ready for the addition of exception handling complexity.

### **Functional Methods Control Flow Architecture**

#### **Parallel vs Sequential Processing**

**Sequential Methods (`.reduce()`)**
- **Processing Model**: Sequential iteration like `for`/`while` loops
- **Control Flow Support**: Full support for all control flow (`break`, `continue`, `return`, `throw`, `exit`)
- **Behavior**: Early termination stops entire operation
- **Implementation**: Similar to loop control flow handling

**Parallel Methods (`.map()`, `.filter()`)**
- **Processing Model**: Multi-threaded parallel processing
- **Control Flow Support**: `.throw()`, `.return()`, `.break()` all supported and working
- **Throw Behavior**: Collects all `.throw()` results in array with `null` for non-throwing threads
- **Return Behavior**: Returns value for specific item only (thread-local)
- **Break Behavior**: Returns empty string for specific item only (thread-local)
- **Enhanced Error Collection**: Array length matches input, preserves all error information
- **Thread Independence**: Each thread processes independently, control flow affects only that thread

#### **Design Rationale**

The limitation to `.throw()` only in parallel methods is **architecturally sound**:

1. **Parallel Processing Reality**: One thread cannot stop other threads
2. **Multiple Control Flags**: Ambiguous which control flag "wins"
3. **Multiple Return Values**: No clear way to select the "correct" return value
4. **Error Handling**: `.throw()` provides natural error propagation mechanism

**Developer Guidance**: For `.return()` and `.break()` in parallel contexts, developers must implement custom logic within the function itself.
