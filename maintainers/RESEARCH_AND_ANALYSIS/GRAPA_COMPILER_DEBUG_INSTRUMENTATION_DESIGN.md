# Grapa Compiler Debug Instrumentation Design

## Overview

This document outlines the strategic design for instrumenting the Grapa compiler with session debug output to improve error messages and debugging capabilities. The compiler consists of two main phases:

1. **Lexical Analysis (Lexer)**: Converts raw bytes into Grapa tokens
2. **Syntactic Analysis (Parser)**: Processes tokens into execution trees ($OP/$CODE objects)

## Current Compiler Architecture

### 1. **Lexical Analysis Phase**

**Entry Point**: `GrapaScriptExec::Plan()` in `source/grapa/GrapaState.cpp:6146`

**Process Flow**:
```
Raw Input → GrapaItemState → Token Queue → PlanRule()
```

**Key Components**:
- **`GrapaItemState`**: State machine for token processing
- **Token Types**: Defined in `GrapaTokenType` enum (STR, INT, FLOAT, ID, etc.)
- **State Machine**: START → ESCAPE → BLOCK states
- **Token Queue**: `tokenQueue` holds processed tokens

**Current Error Handling**:
- When lexer fails, user sees remaining tokens that couldn't be processed
- No context about where in the input the failure occurred
- No explanation of why the failure happened

### 2. **Syntactic Analysis Phase**

**Entry Point**: `GrapaScriptExec::PlanRule()` in `source/grapa/GrapaState.cpp:3270`

**Process Flow**:
```
Token Stream → BNF Rule Matching → Execution Tree ($OP/$CODE)
```

**Key Components**:
- **BNF Grammar**: Defined in `lib/grapa/$grapa.grc`
- **Rule Matching**: Recursive descent with left recursion handling
- **Operation Building**: Creates `$OP` and `$CODE` nodes
- **Left Recursion**: Two-phase parsing (Phase 1: non-left-recursive, Phase 2: left-recursive)

**Current Error Handling**:
- When parser fails, user sees list of remaining tokens
- No context about which rule was being matched
- No explanation of why the rule failed to match

## Strategic Debug Instrumentation Design

### **Component Name**: `compiler`

**Debug Levels**:
- **Level 1**: Basic compilation progress and errors
- **Level 2**: Token processing details
- **Level 3**: Rule matching decisions
- **Level 4**: Detailed state machine transitions
- **Level 5**: Full token stream and rule alternatives

### **1. Lexical Analysis Instrumentation**

#### **Entry Points for Debug Output**

**A. Token Creation (`GrapaItemState::AddToken()`)**
```cpp
// Location: source/grapa/GrapaState.cpp:330
GrapaRuleEvent* GrapaItemState::AddToken(u8 pToken, const char* pName, bool pSkip, const GrapaBYTE& pValue)
{
    // ADD DEBUG: Token creation
    if (vScriptExec && vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 2)) {
        char debugMsg[256];
        snprintf(debugMsg, sizeof(debugMsg), "LEX: Created token type=%d name='%s' value='%.*s' skip=%d", 
                 pToken, pName ? pName : "NULL", (int)pValue.mLength, (char*)pValue.mBytes, pSkip);
        vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 2);
    }
    
    // ... existing code ...
}
```

**B. State Machine Transitions (`GrapaScriptExec::Plan()`)**
```cpp
// Location: source/grapa/GrapaState.cpp:6168 (enum states)
// Location: source/grapa/GrapaState.cpp:6256 (state transitions)

// ADD DEBUG: State transitions
if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 4)) {
    char debugMsg[256];
    snprintf(debugMsg, sizeof(debugMsg), "LEX: State transition %d -> %d at position %llu", 
             oldState, newState, (unsigned long long)currentPosition);
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 4);
}
```

**C. Lexical Errors**
```cpp
// Location: Various error conditions in Plan()
// ADD DEBUG: Lexical error with context
if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 1)) {
    char debugMsg[512];
    snprintf(debugMsg, sizeof(debugMsg), 
             "LEX ERROR: Failed to tokenize at position %llu, character '%c' (0x%02x). "
             "Expected: %s. Context: '%.20s...'", 
             position, currentChar, currentChar, expectedToken, context);
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 1);
}
```

### **2. Syntactic Analysis Instrumentation**

#### **Entry Points for Debug Output**

**A. Rule Matching Attempts (`GrapaScriptExec::PlanRule()`)**
```cpp
// Location: source/grapa/GrapaState.cpp:3270
GrapaRuleEvent* GrapaScriptExec::PlanRule(GrapaNames* pNameSpace, GrapaRuleEvent* pTokenEvent, 
                                         GrapaRuleEvent* pRuleEvent, GrapaRuleQueue* pOperationQueue, 
                                         bool& pMatched, GrapaRuleEvent* pLastNext, bool& pAddLastNext, 
                                         s8 pLeftRecursion, GrapaKeyValue& pCache, bool pTouched)
{
    // ADD DEBUG: Rule matching attempt
    if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 3)) {
        char debugMsg[256];
        snprintf(debugMsg, sizeof(debugMsg), 
                 "PARSE: Attempting rule '%s' with token type=%d value='%.*s' leftRecursion=%d", 
                 pRuleEvent->mName.mBytes ? (char*)pRuleEvent->mName.mBytes : "NULL",
                 pTokenEvent ? pTokenEvent->mValue.mToken : -1,
                 pTokenEvent ? (int)pTokenEvent->mValue.mLength : 0,
                 pTokenEvent ? (char*)pTokenEvent->mValue.mBytes : "NULL",
                 pLeftRecursion);
        vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 3);
    }
    
    // ... existing code ...
    
    // ADD DEBUG: Rule match result
    if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 3)) {
        char debugMsg[256];
        snprintf(debugMsg, sizeof(debugMsg), 
                 "PARSE: Rule '%s' %s", 
                 pRuleEvent->mName.mBytes ? (char*)pRuleEvent->mName.mBytes : "NULL",
                 pMatched ? "MATCHED" : "FAILED");
        vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 3);
    }
}
```

**B. Left Recursion Handling**
```cpp
// Location: source/grapa/GrapaState.cpp:3559-3568
// ADD DEBUG: Left recursion phases
if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 3)) {
    char debugMsg[256];
    snprintf(debugMsg, sizeof(debugMsg), 
             "PARSE: Left recursion phase %d for rule '%s'", 
             pLeftRecursion,
             pRuleEvent->mName.mBytes ? (char*)pRuleEvent->mName.mBytes : "NULL");
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 3);
}
```

**C. Operation Tree Building**
```cpp
// Location: source/grapa/GrapaState.cpp:2777 (operation creation)
// ADD DEBUG: Operation tree building
if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 2)) {
    char debugMsg[256];
    snprintf(debugMsg, sizeof(debugMsg), 
             "PARSE: Created operation '%s' with %d parameters", 
             operationName, parameterCount);
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", debugMsg, 2);
}
```

### **3. Error Context Enhancement**

#### **A. Token Stream Context**
```cpp
// ADD DEBUG: Token stream context for errors
if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 1)) {
    // Show last 5 tokens for context
    GrapaCHAR contextMsg = "PARSE ERROR: Token stream context: ";
    GrapaRuleEvent* token = pTokenEvent;
    int tokenCount = 0;
    
    while (token && tokenCount < 5) {
        contextMsg.Append(" [");
        contextMsg.Append(token->mValue.mToken);
        contextMsg.Append(":'");
        contextMsg.Append((char*)token->mValue.mBytes, token->mValue.mLength);
        contextMsg.Append("']");
        token = token->Next();
        tokenCount++;
    }
    
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", contextMsg, 1);
}
```

#### **B. Rule Stack Context**
```cpp
// ADD DEBUG: Rule stack for debugging
if (vScriptExec->vScriptState->mDebug.ShouldDebug("compiler", 3)) {
    // Track rule call stack
    static std::vector<GrapaCHAR> ruleStack;
    
    // Push current rule
    ruleStack.push_back(pRuleEvent->mName);
    
    // Show current rule stack
    GrapaCHAR stackMsg = "PARSE: Rule stack: ";
    for (size_t i = 0; i < ruleStack.size(); i++) {
        if (i > 0) stackMsg.Append(" -> ");
        stackMsg.Append(ruleStack[i]);
    }
    
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "compiler", stackMsg, 3);
    
    // Pop rule when exiting
    if (!pMatched) {
        ruleStack.pop_back();
    }
}
```

## Implementation Status

### **Phase 1: Basic Error Context** ✅ **COMPLETED**
- **Parser Error Context**: Enhanced error messages with rule matching context
- **Token Stream Context**: Shows remaining tokens when parsing fails
- **Rule Stack Information**: Displays current rule stack during errors
- **Error Suggestions**: Provides helpful suggestions for common syntax issues

### **Phase 2: Detailed Compilation Flow** ✅ **COMPLETED**
- **Lexer Token Creation**: Debug output for token creation process
- **Parser Rule Matching**: Detailed rule matching attempts and results
- **Component Separation**: Separate `lexer` and `parser` debug components
- **Session Context Integration**: Lexer receives session context via `vScriptExec`

### **Phase 3: Advanced Debugging** 🔄 **IN PROGRESS**
- **State Machine Transitions**: Detailed lexer state transitions (Level 4-5)
- **Full Token Stream Logging**: Complete token stream with context (Level 4-5)
- **Memory Allocation Tracking**: Compilation memory usage (Level 5)
- **Performance Metrics**: Compilation timing and optimization data (Level 5)

## Architectural Solutions Implemented

### **Session Context Passing**
The challenge of passing session context to the lexer was solved through:

#### **GrapaItemState Enhancement**
```cpp
class GrapaItemState {
    GrapaScriptExec* vScriptExec;  // Session context
    // ... existing members
};
```

#### **Context Passing Pattern**
```cpp
// During lexer creation in GrapaScriptExec::Plan()
itemState.SetParams(&vScriptState->mItemParams, 
                   vScriptState->GetNameSpace(), 
                   this);  // Pass vScriptExec context
```

#### **Debug Output Integration**
```cpp
// Lexer debug output uses session context
if (vScriptExec && vScriptExec->vScriptState->mDebug.ShouldDebug("lexer", 2)) {
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, mNameSpace, "lexer", debugMsg, 2);
}
```

### **Component Separation**
The compiler debug component was separated into granular components:

#### **Lexer Component** (`lexer`)
- **Scope**: Tokenization process only
- **Debug Levels**: 1-5
- **Output**: Token creation, state transitions, error context

#### **Parser Component** (`parser`)
- **Scope**: Grammar parsing only
- **Debug Levels**: 1-5
- **Output**: Rule matching, execution tree building, error context

#### **Combined Component** (`compiler`)
- **Scope**: Both lexer and parser (shorthand)
- **Debug Levels**: 1-5
- **Implementation**: Returns higher level of lexer/parser components

### **Async Pipeline Architecture**
The discovery that lexer and parser operate as separate, parallel queues was documented:

#### **Multi-Stage Pipeline**
```
Raw Input → Lexer Queue → Token Queue → Parser Queue → Execution Tree
```

#### **Key Characteristics**
- **Parallel Processing**: Lexer and parser work simultaneously
- **Queue-Based**: Each stage waits for items in its queue
- **Session Context**: Lexer receives session context at creation time
- **Isolation**: Each session has independent pipeline instances

## Error Message Enhancement

### **Current Error Output**
```
Syntax Error: Remaining tokens: [ID:'x'] [SYM:'='] [ID:'y']
```

### **Enhanced Error Output**
```
[DEBUG-SESSION-123-compiler] PARSE ERROR: Failed to match rule '$statement' at token position 3
[DEBUG-SESSION-123-compiler] PARSE ERROR: Token stream context: [ID:'x'] [SYM:'='] [ID:'y'] [SYM:';']
[DEBUG-SESSION-123-compiler] PARSE ERROR: Expected one of: $expression, $assignment, $declaration
[DEBUG-SESSION-123-compiler] PARSE ERROR: Rule stack: $start -> $statement_list -> $statement
[DEBUG-SESSION-123-compiler] PARSE ERROR: Suggestion: Check for missing semicolon or invalid expression
```

## Performance Considerations

### **Debug Output Overhead**
- **Level 1**: Minimal overhead, only on errors
- **Level 2-3**: Moderate overhead, string formatting
- **Level 4-5**: High overhead, extensive logging

### **Optimization Strategies**
1. **Conditional Compilation**: Use `#ifdef GRAPA_DEBUG_COMPILER`
2. **Lazy Evaluation**: Only format debug messages when debug is enabled
3. **Buffer Reuse**: Reuse debug message buffers to reduce allocation
4. **Level Filtering**: Early return if debug level not enabled

## Testing Strategy

### **Test Cases**
1. **Valid Code**: Ensure debug output doesn't interfere with normal compilation
2. **Lexical Errors**: Test debug output for invalid tokens
3. **Syntax Errors**: Test debug output for invalid syntax
4. **Complex Errors**: Test debug output for nested rule failures
5. **Performance**: Measure compilation time with debug enabled

### **Test Scripts**
```grapa
// Test lexical error debugging
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "compiler:3");

// This should produce detailed debug output
x = 5; y = 10; z = x + y;  // Valid code
x = 5; y = 10; z = x +     // Invalid code - missing operand
```

## Integration with Existing Debug System

### **Component Registration**
- **Component Name**: `compiler`
- **Debug Levels**: 1-5
- **Session Support**: Full session isolation
- **Output Format**: Session-specific with component prefix

### **Environment Variable Support**
```grapa
// Enable compiler debugging
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "compiler:3");

// Session-specific compiler debugging
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "compiler:2");
```

## Risk Assessment

### **Low Risk**
- **Error Context Enhancement**: Only adds debug output, doesn't change core logic
- **Session Integration**: Uses existing debug infrastructure
- **Backward Compatibility**: Debug output is opt-in

### **Medium Risk**
- **Performance Impact**: Debug output adds overhead
- **Memory Usage**: Debug message formatting uses additional memory
- **Complexity**: Adds complexity to core compilation code

### **Mitigation Strategies**
1. **Conditional Compilation**: Debug code only compiled when needed
2. **Performance Testing**: Measure and optimize debug overhead
3. **Incremental Implementation**: Implement in phases to manage risk
4. **Extensive Testing**: Test with various error conditions

## Conclusion

This design provides a strategic approach to instrumenting the Grapa compiler with session debug output. The phased implementation allows for immediate user benefit while building toward comprehensive debugging capabilities.

**Key Benefits**:
1. **Better Error Messages**: Users get context about compilation failures
2. **Developer Debugging**: Detailed compilation flow for debugging
3. **Session Isolation**: Debug output isolated between parallel sessions
4. **Performance Conscious**: Minimal overhead when debug not enabled

**Next Steps**:
1. Implement Phase 1 (Basic Error Context)
2. Test with real error conditions
3. Measure performance impact
4. Proceed with Phase 2 based on results 