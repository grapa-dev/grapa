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

### **❌ Runtime Flow Controls - INCOMPLETE**

#### **Current Status: Partially Implemented but Not Working**

The following runtime flow controls have C++ event classes but **lack proper plumbing** throughout the execution system:

1. **`break`** - Has event class but flag handling is incomplete
2. **`continue`** - Has event class but currently does nothing
3. **`return`** - Has event class but return value handling is incomplete
4. **`exit`** - Has event class but exit handling is incomplete

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
    result->mAbort = true;  // Sets abort flag
    return(result);
}
```

#### **Grammar Definition**
```grapa
| break {@<break,{}>}
```

#### **Issues Identified**
1. **Flag Recognition**: `mAbort` flag is set but not consistently checked throughout execution
2. **Loop Context**: While loops check for `mAbort` but other loop types may not
3. **Scope Cleanup**: Break may not trigger proper scope cleanup
4. **Memory Management**: Objects created before break may not be properly cleaned up

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
    return(result);  // Currently does nothing - no flag set
}
```

#### **Grammar Definition**
```grapa
| continue {@<continue,{}>}
```

#### **Issues Identified**
1. **No Flag Set**: Continue doesn't set any flag to signal loop continuation
2. **No Loop Recognition**: Loops don't check for continue signals
3. **Scope Handling**: Continue should skip remaining loop body but maintain scope
4. **Memory Management**: Objects created in current iteration may need cleanup

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
            // Return value processing is commented out
            // No return flag is set
        }
    }
    return(result);  // Returns NULL instead of return value
}
```

#### **Grammar Definition**
```grapa
| return '(' <$comp> ')' {@<return,{$3}>}
```

#### **Issues Identified**
1. **Return Value Handling**: Return value processing is commented out
2. **No Return Flag**: No flag to signal function return
3. **Function Context**: Functions don't check for return signals
4. **Scope Cleanup**: Function scope cleanup on return is incomplete
5. **Memory Management**: Objects created in function may not be cleaned up

### **4. Exit Statement**

#### **Current Implementation**
```cpp
class GrapaLibraryRuleExitEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleExitEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput);
};

// Implementation not shown in current code - likely incomplete
```

#### **Grammar Definition**
```grapa
| exit {@<exit,{}>}
```

#### **Issues Identified**
1. **Incomplete Implementation**: Exit handler implementation is missing
2. **No Exit Flag**: No flag to signal program exit
3. **Cleanup Handling**: No cleanup of global resources
4. **Exit Code**: No exit code handling

## Implementation Requirements for Phase 1

### **Critical Plumbing Requirements**

#### **1. Control Flow Flags**
Each runtime flow control needs a distinct flag in `GrapaRuleEvent`:

```cpp
class GrapaRuleEvent
{
public:
    bool mAbort;        // For break statements
    bool mContinue;     // For continue statements  
    bool mReturn;       // For return statements
    bool mExit;         // For exit statements
    GrapaRuleEvent* mReturnValue;  // For return values
};
```

#### **2. Flag Propagation System**
Control flow flags must be propagated through the execution stack:

```cpp
// In ProcessPlan and related execution methods
if (result && result->mAbort) {
    // Handle break - stop current loop
    return result;
}
if (result && result->mContinue) {
    // Handle continue - skip to next iteration
    return result;
}
if (result && result->mReturn) {
    // Handle return - exit current function
    return result;
}
if (result && result->mExit) {
    // Handle exit - terminate program
    return result;
}
```

#### **3. Loop Context Recognition**
All loop types must check for control flow flags:

```cpp
// In while loops, for loops, etc.
while (condition) {
    GrapaRuleEvent* result = ProcessPlan(namespace, body);
    if (result->mAbort) {
        // Break detected - exit loop
        break;
    }
    if (result->mContinue) {
        // Continue detected - skip to next iteration
        continue;
    }
    if (result->mReturn) {
        // Return detected - propagate up
        return result;
    }
}
```

#### **4. Function Context Recognition**
Functions must check for return signals:

```cpp
// In function execution
GrapaRuleEvent* result = ProcessPlan(namespace, functionBody);
if (result->mReturn) {
    // Return detected - exit function with return value
    return result->mReturnValue;
}
```

### **Memory Management Requirements**

#### **1. Scope Cleanup on Control Flow**
Control flow statements must trigger proper scope cleanup:

```cpp
// When break/continue/return/exit is detected
if (controlFlowFlag) {
    // Clean up current scope
    CleanupScope(currentScope);
    
    // Propagate control flow signal
    return controlFlowResult;
}
```

#### **2. Object Lifecycle Management**
Objects created before control flow must be properly cleaned up:

```cpp
// Before executing control flow statement
CleanupTemporaryObjects();
SetControlFlowFlag(flagType);
return controlFlowResult;
```

### **Implementation Strategy**

#### **Phase 1 Implementation Plan**

1. **Add Control Flow Flags** (Week 1)
   - Add `mContinue`, `mReturn`, `mExit` flags to `GrapaRuleEvent`
   - Add `mReturnValue` field for return values
   - Update flag initialization in constructors

2. **Implement Flag Propagation** (Week 2)
   - Update `ProcessPlan` to check and propagate control flow flags
   - Update all execution methods to handle flags
   - Implement flag clearing after handling

3. **Update Loop Handlers** (Week 3)
   - Update `GrapaLibraryRuleWhileEvent::Run` for continue support
   - Update `GrapaLibraryRuleForEvent::Run` for all control flow
   - Update any other loop handlers

4. **Update Function Handlers** (Week 4)
   - Update function execution to handle return signals
   - Implement return value processing
   - Add function scope cleanup on return

5. **Memory Management Integration** (Week 4)
   - Integrate scope cleanup with control flow
   - Ensure proper object lifecycle management
   - Test memory leak prevention

#### **Testing Strategy**

1. **Unit Tests**
   - Test each control flow statement in isolation
   - Test flag propagation through execution stack
   - Test memory cleanup on control flow

2. **Integration Tests**
   - Test control flow in loops
   - Test control flow in functions
   - Test nested control flow scenarios

3. **Memory Tests**
   - Test for memory leaks with control flow
   - Test object cleanup on scope exit
   - Test long-running programs with control flow

## Current Working Examples

### **While Loop with Break (Partially Working)**
```cpp
// In GrapaLibraryRuleWhileEvent::Run
while (true) {
    GrapaRuleEvent* rx1 = vScriptExec->ProcessPlan(pNameSpace, p1);
    GrapaRuleEvent* r1 = rx1 ? (rx1->mValue.mToken == GrapaTokenType::PTR ? rx1->vRulePointer : rx1) : NULL;
    bool isContinue = false;
    if (r1 && r1->mValue.mLength && r1->mValue.mBytes[0] && r1->mValue.mBytes[0] != '0' && !r1->IsNull()) {
        if (GrapaRuleEvent* p2 = pInput->Head(1)) {
            isContinue = true;
            if (GrapaRuleEvent* r2 = vScriptExec->ProcessPlan(pNameSpace, p2)) {
                bool isAbort = r2->mAbort;  // Break flag is checked here
                r2->CLEAR();
                delete r2;
                if (isAbort) break;  // Break is handled
            }
        }
    }
    if (rx1) { rx1->CLEAR(); delete rx1; }
    if (!isContinue) break;
}
```

### **Issues with Current Implementation**
1. **Break only works in while loops** - not in for loops
2. **Continue is not implemented** - no flag checking
3. **Return is not implemented** - no function context
4. **Exit is not implemented** - no program termination

## Conclusion

The runtime flow controls (`break`, `continue`, `return`, `exit`) are **partially implemented** but **not fully functional**. The C++ event classes exist and basic flag mechanisms are in place, but comprehensive plumbing throughout the execution system is missing.

**Key Requirements for Phase 1:**
1. **Complete flag propagation system** throughout execution stack
2. **Loop context recognition** for all loop types
3. **Function context recognition** for return statements
4. **Memory management integration** with control flow
5. **Comprehensive testing** of all control flow scenarios

This implementation will require careful attention to memory management and scope cleanup to ensure that control flow statements don't cause memory leaks or leave objects in an inconsistent state.

## Future: Exception Handling (try/catch)

### **Dependencies: Runtime Flow Controls Must Be Complete First**

Exception handling (`try/catch`) requires the **same flag propagation and context recognition system** as runtime flow controls, making it a natural extension once the basic control flow system is working.

### **Exception Handling Requirements**

#### **1. Exception Flow Flags**
```cpp
class GrapaRuleEvent
{
public:
    // Existing control flow flags
    bool mAbort;        // For break statements
    bool mContinue;     // For continue statements  
    bool mReturn;       // For return statements
    bool mExit;         // For exit statements
    GrapaRuleEvent* mReturnValue;  // For return values
    
    // Exception handling flags (future)
    bool mException;    // For exception signals
    GrapaRuleEvent* mExceptionValue;  // For exception objects
    GrapaRuleEvent* mExceptionHandler;  // For catch block
};
```

#### **2. Exception Propagation System**
```cpp
// In ProcessPlan and related execution methods
if (result && result->mException) {
    // Handle exception - look for catch block
    return HandleException(result->mExceptionValue);
}
```

#### **3. Try/Catch Context Recognition**
```cpp
// In try block execution
try {
    GrapaRuleEvent* result = ProcessPlan(namespace, tryBody);
    if (result->mException) {
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

#### **Phase 2 Implementation Plan (After Runtime Flow Controls)**

1. **Add Exception Flags** (Week 1)
   - Add `mException`, `mExceptionValue`, `mExceptionHandler` flags
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
exception->mException = true;
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

#### **Must Complete First:**
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
1. **Phase 1**: Complete runtime flow controls (break, continue, return, exit)
2. **Phase 2**: Implement exception handling (try/catch/throw)

This ensures that the foundational control flow system is solid before adding the complexity of exception handling.
