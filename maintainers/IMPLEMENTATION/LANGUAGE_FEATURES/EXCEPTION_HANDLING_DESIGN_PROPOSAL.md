---
tags:
  - maintainer
  - implementation
  - exception-handling
  - design-proposal
  - try-catch-throw
---

# Exception Handling Design Proposal: try/catch/throw

## Overview

This document proposes a comprehensive design for implementing exception handling (try/catch/throw) in Grapa, based on the successful control flow architecture that was implemented for break/continue/return/exit.

## Current Control Flow Architecture Analysis

### **Successfully Implemented Components**

#### **1. Core Data Structure**
```cpp
class GrapaControlFlowType {
public: enum { NONE = 0, EXIT, SYNTAX, BREAK, CONTINUE, RETURN, THROW };
};
```

**Key Insight**: `THROW` is already defined in the enum, indicating the foundation is in place.

#### **2. Control Flow Flag System**
```cpp
class GrapaRuleEvent {
    u8 mControlFlow; // Control flow type: 0 = none, 1 = exit, 2 = syntax, 3 = break, 4 = continue, 5 = return, 6 = throw
    // ... other fields
};
```

**Key Insight**: The `mControlFlow` field can carry exception information.

#### **3. Event Handler Pattern**
```cpp
GrapaRuleEvent* GrapaLibraryRuleBreakEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
    result->SetNull();
    result->mControlFlow = GrapaControlFlowType::BREAK;
    return(result);
}
```

**Key Insight**: Control flow events create result objects with appropriate flags.

#### **4. Flag Propagation System**
```cpp
// In loop handlers
u8 isControlFlowChange = r2->mControlFlow;
if (isControlFlowChange == GrapaControlFlowType::BREAK)
    isContinue = false;
if (isControlFlowChange == GrapaControlFlowType::CONTINUE) 
    ; // continue loop
if (isControlFlowChange == GrapaControlFlowType::RETURN || isControlFlowChange == GrapaControlFlowType::SYNTAX || isControlFlowChange == GrapaControlFlowType::EXIT)
{
    isContinue = false;
    result = r2;
    r2 = NULL;
}
```

**Key Insight**: Control flow flags are checked and propagated up the call stack.

#### **5. Grammar Integration**
```grapa
| break {@<break,{}>}
| continue {@<continue,{}>}
| return '(' <$comp> ')' {@<return,{$3}>}
| return <$comp> {@<return,{$2}>}
| exit '(' ')'{@<exit,{}>}
| exit {@<exit,{}>}
```

**Key Insight**: Grammar rules map to event handlers.

## Exception Handling Design Proposal

### **1. Extended Data Structure**

#### **Enhanced GrapaRuleEvent**
```cpp
class GrapaRuleEvent {
    u8 mControlFlow; // Control flow type (THROW = 6)
    GrapaRuleEvent* mExceptionValue; // Exception object/value
    GrapaRuleEvent* mExceptionType; // Exception type/class
    // ... existing fields
};
```

#### **Exception Information Structure**
```cpp
class GrapaExceptionInfo {
public:
    GrapaRuleEvent* exceptionValue;    // The thrown value
    GrapaRuleEvent* exceptionType;     // Exception type/class
    GrapaRuleEvent* exceptionMessage;  // Exception message
    GrapaRuleEvent* exceptionStack;    // Stack trace
    u64 throwLine;                     // Line number where thrown
    GrapaCHAR throwFile;               // File where thrown
};
```

### **2. Grammar Rules**

#### **Exception Handling Grammar**
```grapa
// Throw statement
| throw '(' <$comp> ')' {@<throw,{$3}>}
| throw <$comp> {@<throw,{$2}>}

// Try-catch statement
| try '{' <$command> '}' catch '(' <$ID> ')' '{' <$command> '}' {@<trycatch,{$3,$7,$9}>}
| try '{' <$command> '}' catch '{' <$command> '}' {@<trycatch,{$3,{},$6}>}

// Try-finally statement
| try '{' <$command> '}' finally '{' <$command> '}' {@<tryfinally,{$3,$6}>}

// Try-catch-finally statement
| try '{' <$command> '}' catch '(' <$ID> ')' '{' <$command> '}' finally '{' <$command> '}' {@<trycatchfinally,{$3,$7,$9,$12}>}
```

### **3. Event Handlers**

#### **Throw Event Handler**
```cpp
GrapaRuleEvent* GrapaLibraryRuleThrowEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
    result->SetNull();
    result->mControlFlow = GrapaControlFlowType::THROW;
    
    // Evaluate the exception value
    GrapaRuleEvent* p1 = pInput->Head(0);
    if (p1->mValue.mToken == GrapaTokenType::OP) {
        result->mExceptionValue = vScriptExec->ProcessPlan(pNameSpace, p1);
    } else {
        result->mExceptionValue = vScriptExec->CopyItem(p1);
    }
    
    // Set exception type based on value
    if (result->mExceptionValue) {
        result->mExceptionType = new GrapaRuleEvent(GrapaTokenType::STR, 0, "Exception", "");
        result->mExceptionType->mValue = result->mExceptionValue->mValue;
    }
    
    return(result);
}
```

#### **Try-Catch Event Handler**
```cpp
GrapaRuleEvent* GrapaLibraryRuleTryCatchEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = NULL;
    GrapaRuleEvent* tryBlock = pInput->Head(0);
    GrapaRuleEvent* catchVar = pInput->Head(1);
    GrapaRuleEvent* catchBlock = pInput->Head(2);
    
    // Execute try block
    if (tryBlock) {
        result = vScriptExec->ProcessPlan(pNameSpace, tryBlock);
        
        // Check for thrown exception
        if (result && result->mControlFlow == GrapaControlFlowType::THROW) {
            // Exception was thrown - execute catch block
            if (catchBlock) {
                // Create catch variable scope
                GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", "");
                
                // Bind exception to catch variable
                if (catchVar && result->mExceptionValue) {
                    GrapaRuleEvent* exceptionVar = new GrapaRuleEvent(0, catchVar->mValue, GrapaCHAR());
                    exceptionVar->vRulePointer = result->mExceptionValue;
                    pNameSpace->GetNameQueue()->PushTail(exceptionVar);
                }
                
                // Execute catch block
                GrapaRuleEvent* catchResult = vScriptExec->ProcessPlan(pNameSpace, catchBlock);
                
                // Clean up catch variable scope
                if (pNameSpace->GetNameQueue()->PopEvent(operation)) {
                    operation->CLEAR();
                    delete operation;
                }
                
                // Return catch block result (clear throw flag)
                if (catchResult) {
                    catchResult->mControlFlow = GrapaControlFlowType::NONE;
                    result = catchResult;
                } else {
                    result = new GrapaRuleEvent(GrapaTokenType::START, 0, "", "");
                    result->SetNull();
                }
            }
        }
    }
    
    return(result);
}
```

#### **Try-Finally Event Handler**
```cpp
GrapaRuleEvent* GrapaLibraryRuleTryFinallyEvent::Run(GrapaScriptExec *vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent *pOperation, GrapaRuleQueue* pInput)
{
    GrapaRuleEvent* result = NULL;
    GrapaRuleEvent* tryBlock = pInput->Head(0);
    GrapaRuleEvent* finallyBlock = pInput->Head(1);
    
    // Execute try block
    if (tryBlock) {
        result = vScriptExec->ProcessPlan(pNameSpace, tryBlock);
    }
    
    // Always execute finally block
    if (finallyBlock) {
        GrapaRuleEvent* finallyResult = vScriptExec->ProcessPlan(pNameSpace, finallyBlock);
        // Note: finally result is ignored, original result is preserved
    }
    
    return(result);
}
```

### **4. Control Flow Integration**

#### **Enhanced Flag Checking**
```cpp
// In all loop and function handlers
if (isControlFlowChange == GrapaControlFlowType::THROW) {
    // Propagate throw up the call stack
    result = r2;
    r2 = NULL;
    break; // Exit current context
}
```

#### **Exception Propagation**
```cpp
// In ProcessPlan function
if (result && result->mControlFlow == GrapaControlFlowType::THROW) {
    // Check if we're in a try block
    if (IsInTryBlock(pNameSpace)) {
        // Let try-catch handle it
        return result;
    } else {
        // Unhandled exception - propagate up
        return result;
    }
}
```

### **5. Built-in Exception Types**

#### **Exception Class Hierarchy**
```grapa
// Base exception class
class Exception {
    message = "";
    stack = "";
    
    func __init__(msg) {
        this.message = msg;
        this.stack = this.getStackTrace();
    }
    
    func getStackTrace() {
        // Return stack trace information
        return "Stack trace...";
    }
}

// Specific exception types
class TypeError extends Exception {}
class ValueError extends Exception {}
class RuntimeError extends Exception {}
class SyntaxError extends Exception {}
class FileError extends Exception {}
class NetworkError extends Exception {}
```

### **6. Usage Examples**

#### **Basic Exception Handling**
```grapa
// Throw an exception
func divide(a, b) {
    if (b == 0) {
        throw "Division by zero";
    };
    return a / b;
};

// Catch an exception
try {
    result = divide(10, 0);
} catch (error) {
    "Error: ".interpolate(error).echo();
};
```

#### **Exception with Types**
```grapa
// Throw typed exception
func processFile(filename) {
    if (!fileExists(filename)) {
        throw FileError("File not found: " + filename);
    };
    // Process file...
};

// Catch specific exception type
try {
    processFile("nonexistent.txt");
} catch (fileError) {
    if (fileError instanceof FileError) {
        "File error: ".interpolate(fileError.message).echo();
    } else {
        throw fileError; // Re-throw if not a file error
    };
};
```

#### **Finally Block**
```grapa
// Resource cleanup
file = openFile("data.txt");
try {
    data = readFile(file);
    processData(data);
} finally {
    closeFile(file); // Always executed
};
```

### **7. Implementation Phases**

#### **Phase 1: Core Infrastructure (Week 1)**
1. **Extend GrapaRuleEvent** with exception fields
2. **Implement ThrowEvent** handler
3. **Add throw grammar rules**
4. **Test basic throw functionality**

#### **Phase 2: Try-Catch Implementation (Week 2)**
1. **Implement TryCatchEvent** handler
2. **Add try-catch grammar rules**
3. **Implement exception propagation**
4. **Test try-catch functionality**

#### **Phase 3: Finally and Advanced Features (Week 3)**
1. **Implement TryFinallyEvent** handler
2. **Add finally grammar rules**
3. **Implement exception types**
4. **Test complete exception handling**

#### **Phase 4: Integration and Testing (Week 4)**
1. **Integrate with existing control flow**
2. **Add built-in exception types**
3. **Comprehensive testing**
4. **Documentation and examples**

### **8. Memory Management Considerations**

#### **Exception Object Lifecycle**
```cpp
// When throwing
GrapaRuleEvent* result = new GrapaRuleEvent();
result->mExceptionValue = vScriptExec->CopyItem(exceptionValue); // Deep copy
result->mExceptionType = vScriptExec->CopyItem(exceptionType);   // Deep copy

// When catching
GrapaRuleEvent* catchVar = new GrapaRuleEvent();
catchVar->vRulePointer = result->mExceptionValue; // Reference, don't copy

// When propagating
// Exception objects are preserved through the call stack
```

#### **Scope Management**
```cpp
// Try block creates new scope
GrapaRuleEvent* operation = vScriptExec->vScriptState->AddRuleOperation(pNameSpace->GetNameQueue(), "", "");

// Catch block creates new scope with exception variable
// Finally block executes in original scope

// All scopes are properly cleaned up on exit
```

### **9. Error Handling and Edge Cases**

#### **Unhandled Exceptions**
```cpp
// If exception reaches top level without being caught
if (result && result->mControlFlow == GrapaControlFlowType::THROW) {
    // Print error message
    "Unhandled exception: ".interpolate(result->mExceptionValue).echo();
    // Exit program or return error
    return result;
}
```

#### **Nested Try-Catch**
```cpp
// Inner catch can re-throw to outer catch
try {
    try {
        throw "Inner exception";
    } catch (innerError) {
        throw "Outer exception"; // Re-throw to outer catch
    };
} catch (outerError) {
    "Caught: ".interpolate(outerError).echo();
};
```

### **10. Performance Considerations**

#### **Exception Handling Overhead**
- **Minimal overhead** when no exceptions occur
- **Fast propagation** using existing control flow system
- **Efficient memory management** with reference counting
- **No stack unwinding** - uses Grapa's existing scope system

#### **Optimization Strategies**
- **Exception objects are created only when thrown**
- **Catch blocks are only executed when exceptions occur**
- **Finally blocks have minimal overhead**
- **Exception information is preserved efficiently**

## Conclusion

This design leverages the **proven control flow architecture** that successfully implements break/continue/return/exit. The key insights are:

1. **Reuse existing infrastructure** - `mControlFlow` flag system, event handlers, grammar rules
2. **Follow established patterns** - same event handler structure as break/continue/return
3. **Integrate seamlessly** - exception handling works with existing control flow
4. **Maintain consistency** - same memory management and scope handling patterns

The design provides **comprehensive exception handling** while maintaining **Grapa's performance characteristics** and **existing code compatibility**.
