# Grapa Send System Architecture

## Overview

The Grapa Send system is a complex multiprocessing architecture that handles all output from the Grapa language execution engine. It provides a unified interface for different types of output (console, editor, prompt, debug) while maintaining session context and supporting various execution environments.

## Core Architecture

### Base Classes

#### 1. GrapaSystemSend (Base Interface)
**Location:** `source/grapa/GrapaState.h:180-200`

```cpp
class GrapaSystemSend {
public:
    bool isActive;
    virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const char* sendbuf);
    virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void Send(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const char* sendbuf);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen) {};
    virtual void SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace);
    virtual GrapaRuleEvent* SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pMessage);
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen) {};
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf) {};
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue) {};
    virtual void SetActive(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, bool pActive);
};
```

**Purpose:** Provides the fundamental interface for all output operations.

**Design Pattern:** This is a **Template Method Pattern** where:
- `Send()` and `SendError()` are **template methods** that provide the framework
- `SendCommand()`, `SendPrompt()`, `SendEnd()` are **primitive operations** that derived classes override
- The base class provides default empty implementations (`{}`) for primitive operations
- Derived classes provide concrete implementations for their specific output destinations

#### 2. GrapaConsoleResponse (Response Base)
**Location:** `source/grapa/GrapaConsole.h:10-15`

```cpp
class GrapaConsoleResponse : public GrapaCritical, public GrapaSystemSend {
public:
    GrapaCHAR mPrompt;
    virtual void UpdatePrompt();
};
```

**Purpose:** Base class for all response handlers, inherits from both GrapaCritical (thread safety) and GrapaSystemSend.

### Response Implementation Classes

#### 1. GrapaConsole2Response (CLI Console)
**Location:** `source/grapa/GrapaSystem.h:20-28`

```cpp
class GrapaConsole2Response : public GrapaConsoleResponse {
public:
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaSystem.cpp:607-700`
- **SendCommand:** Outputs to `std::cout` (stdout) with Windows-specific `WriteConsoleA()` replacement
- **SendPrompt:** Outputs to `std::cout` (stdout) with Windows-specific `WriteConsoleA()` replacement
- **SendEnd:** Outputs to `std::cout` (stdout) with Windows-specific `WriteConsoleA()` replacement
- **SendError:** Outputs to `fprintf(stderr, ...)` (stderr) for all platforms

#### 2. GrapaEditorResponse (GUI Editor)
**Location:** `source/grapa/GrapaSystem.h:82-95`

```cpp
class GrapaEditorResponse : public GrapaConsoleResponse {
public:
    Fl_Text_Display* disp;
    GrapaNames mRuleVariables;
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaSystem.cpp:1154-1205`
- **SendCommand:** Outputs to FLTK text display widget (`disp->buffer()->append()`)
- **SendPrompt:** Outputs to FLTK text display widget (`disp->buffer()->append()`)
- **SendEnd:** Outputs to FLTK text display widget (`disp->buffer()->append()`)
- **SendError:** Outputs to `fprintf(stderr, ...)` (stderr) for debug/error messages

#### 3. GrapaPromptResponse (GUI Prompt)
**Location:** `source/grapa/GrapaSystem.h:105-112`

```cpp
class GrapaPromptResponse : public GrapaConsoleResponse {
public:
    My_Prompt_Editor* disp;
    GrapaNames mRuleVariables;
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaSystem.cpp:1206-1260`
- **SendCommand:** Outputs to FLTK prompt editor widget (`disp->buffer()->append()`)
- **SendPrompt:** Outputs to FLTK prompt editor widget (`disp->buffer()->append()`)
- **SendEnd:** Outputs to FLTK prompt editor widget (`disp->buffer()->append()`)
- **SendError:** Outputs to `fprintf(stderr, ...)` (stderr) for debug/error messages

#### 4. GrapaWidgetThread (Widget System)
**Location:** `source/grapa/GrapaWidget.h:49-56`

```cpp
class GrapaWidgetThread : public GrapaConsoleResponse {
public:
    virtual void SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace);
    virtual GrapaRuleEvent* SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pMessage);
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, void* sendbuf, u64 sendbuflen);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendError(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaWidget.cpp:3569-3720`
- **SendCommand:** Uses widget callback system (`echoCB`) for widget-specific output
- **SendPrompt:** Uses widget callback system (`promptCB`) for widget-specific output
- **SendEnd:** Uses widget callback system (`endCB`) for widget-specific output
- **SendError:** Outputs to `fprintf(stderr, ...)` (stderr) for debug/error messages

**Purpose:** Handles output for widget-based GUI applications with callback-driven architecture.

### Session Context Management

#### GrapaNames (Namespace with Response)
**Location:** `source/grapa/GrapaState.h:200-250`

```cpp
class GrapaNames {
private:
    GrapaConsoleResponse* vConsoleResponse;
public:
    virtual void SetResponse(GrapaConsoleResponse* pResponse) { vConsoleResponse = pResponse; }
    virtual GrapaConsoleResponse* GetResponse() { return vConsoleResponse; }
};
```

**Purpose:** Each execution namespace maintains a reference to its response handler, enabling session-specific output routing.

## Polymorphic Design Pattern

### Template Method Pattern Implementation

The Grapa Send system uses the **Template Method Pattern** to provide a unified interface across different output destinations:

```cpp
// Base class provides template methods
class GrapaSystemSend {
    virtual void Send(...) {
        // Template method - calls primitive operations
        SendCommand(...);  // Primitive operation (empty in base)
    }
    virtual void SendCommand(...) {};  // Primitive operation (empty in base)
};

// Derived classes override primitive operations
class GrapaConsole2Response : public GrapaSystemSend {
    virtual void SendCommand(...) {
        std::cout << ...;  // Concrete implementation for CLI
    }
};
```

### Polymorphic Echo Methods

The `GrapaScriptExec` Echo methods demonstrate perfect polymorphic design:

```cpp
void GrapaScriptExec::EchoValue(GrapaSystemSend* pSend, ...) {
    // This method doesn't know what type of response system is in use
    // It could be:
    // - GrapaConsole2Response (CLI output)
    // - GrapaEditorResponse (GUI output) 
    // - GrapaPromptResponse (GUI prompt)
    // - GrapaWidgetThread (Widget system)
    
    pSend->Send(this, vScriptState->vRuleVariables, "some output");
    // ^-- Virtual call gets resolved to the correct implementation
}
```

**Key Benefits:**
1. **Separation of Concerns:** Echo methods don't need to know about output destinations
2. **Extensibility:** New response types can be added without changing Echo methods
3. **Consistency:** All response types handle Send/SendError the same way
4. **Future-proof:** Adding Error versions of Echo methods would work automatically

### Send vs SendError Pattern

All response implementations follow the same pattern:

| Method | Purpose | Destination |
|--------|---------|-------------|
| `Send()` | Normal program output | stdout (CLI) / GUI widgets / Widget callbacks |
| `SendError()` | Debug/error output | stderr (all platforms) |

**Implementation Consistency:**
- **GrapaConsole2Response:** `Send()` → `std::cout`, `SendError()` → `fprintf(stderr, ...)`
- **GrapaEditorResponse:** `Send()` → GUI buffer, `SendError()` → `fprintf(stderr, ...)`
- **GrapaPromptResponse:** `Send()` → GUI buffer, `SendError()` → `fprintf(stderr, ...)`
- **GrapaWidgetThread:** `Send()` → Widget callbacks, `SendError()` → `fprintf(stderr, ...)`

## Output Flow Analysis

### Normal Program Output Flow

1. **Script Execution:** `GrapaScriptExec::EchoValue()` or `GrapaScriptExec::EchoList()`
2. **Send Calls:** Multiple `pSend->Send()` calls for different output components
3. **Response Routing:** `pNameSpace->GetResponse()->SendCommand()`
4. **Platform Output:** 
   - CLI: `std::cout` (stdout)
   - GUI: FLTK widgets
   - Widget: Widget-specific handlers

### Debug Output Flow

#### System Debug Output
1. **Debug Calls:** `GrapaDebug::DebugPrint()` methods
2. **Direct Output:** `fprintf(stderr, ...)` 
3. **Result:** Goes to stderr ✅

#### Session Debug Output
1. **Debug Calls:** `GrapaScriptExecStateDebug::DebugPrint()` methods
2. **Response Routing:** `pNameSpace->GetResponse()->Send()`
3. **Platform Output:** Same as normal program output (stdout for CLI)
4. **Result:** Goes to stdout ❌ (This is the issue!)

## Current Stream Separation Status

### Working Correctly ✅
- **System Debug Output:** `fprintf(stderr, ...)` → stderr
- **Error Messages:** CLI argument errors → stderr
- **Session Debug Output:** `pNameSpace->GetResponse()->SendError()` → stderr ✅ **RESOLVED**
- **Basic Stream Redirection:** `> stdout.txt 2> stderr.txt` works for all cases ✅ **RESOLVED**
- **Cross-Platform Compatibility:** Works on both Mac and Windows ✅ **RESOLVED**

### Implementation Details
- **Session Debug Output:** Now uses `SendError()` method instead of `Send()` method
- **Stream Separation:** Normal output → stdout, debug output → stderr
- **Session Context:** Maintained through polymorphic response system
- **All Response Types:** CLI, GUI, and Widget systems all support proper stream separation

## Send Method Usage Analysis

### Direct Send Calls (Response System)
**Total Instances:** 8
- `pNameSpace->GetResponse()->Send()` - 4 instances (session debug)
- `pNameSpace->GetResponse()->SendPrompt()` - 1 instance
- `pNameSpace->GetResponse()->SendStart()` - 1 instance  
- `pNameSpace->GetResponse()->SendEnd()` - 1 instance
- `pNameSpace->GetResponse()->SendM()` - 1 instance

### Indirect Send Calls (Echo System)
**Total Instances:** 100+ in `GrapaScriptExec::EchoValue()` and `EchoList()`
- `pSend->Send()` calls for token type output
- `pSend->Send()` calls for value formatting
- `pSend->Send()` calls for structural elements

### Console Send Calls
**Total Instances:** 20+ in `GrapaSystem.cpp`
- `mConsoleSend.Send()` for console output
- `vConsoleSend->Send()` for various console operations

## Critical Architecture Insights

### 1. Template Method Pattern Success
- **Base class** provides template methods (`Send()`, `SendError()`) with framework logic
- **Derived classes** override primitive operations (`SendCommand()`, `SendPrompt()`, `SendEnd()`) with concrete implementations
- **Polymorphic design** allows Echo methods to work with any response type without knowing the implementation details

### 2. Polymorphic Echo Methods
- **`GrapaScriptExec::EchoValue()`** and related methods use `GrapaSystemSend* pSend` parameter
- **Virtual method calls** automatically resolve to the correct derived class implementation
- **Future-proof design** - adding Error versions of Echo methods would work automatically with existing response types

### 3. Consistent Send/SendError Pattern
- **All response implementations** follow the same pattern: `Send()` for normal output, `SendError()` for debug/error output
- **Stream separation** is handled at the response level, not in the calling code
- **Cross-platform compatibility** achieved through consistent stderr usage in `SendError()` methods

### 4. Session Context Preservation
- **Debug output maintains session context** through the polymorphic response system
- **Multiprocessing safety** preserved through existing response system architecture
- **No bypass of response system** needed - the system already supports proper stream separation

### 5. Architectural Excellence
- **Separation of concerns:** Echo methods don't need to know about output destinations
- **Extensibility:** New response types can be added without changing Echo methods
- **Consistency:** All response types handle Send/SendError the same way
- **Maintainability:** Changes to output handling only require updates to response implementations

## Potential Solutions Analysis

### Option 1: Add SendError() Method
**Approach:** Add `SendError()` to base `GrapaSystemSend` class
**Impact:** 
- ✅ Maintains session context
- ✅ Minimal interface change
- ❌ Requires updates to all response implementations
- ❌ Changes all Send() callers to choose appropriate method

**Files to Modify:**
- `GrapaState.h` - Add SendError() to GrapaSystemSend
- `GrapaConsole.h` - Add SendError() to GrapaConsoleResponse  
- `GrapaSystem.h` - Add SendError() to all response classes
- `GrapaSystem.cpp` - Implement SendError() in all response classes
- `GrapaState.cpp` - Update session debug to use SendError()
- `GrapaWidget.h` - Add SendError() to GrapaWidgetThread

### Option 2: Add Stream Parameter to Send()
**Approach:** Modify `Send()` signature to include stream type
**Impact:**
- ✅ Maintains session context
- ❌ Massive refactoring across entire system
- ❌ Changes all 100+ Send() call sites
- ❌ Breaks existing interface

### Option 3: Debug Response System
**Approach:** Create separate debug response classes
**Impact:**
- ✅ Clean separation of concerns
- ✅ No interface changes
- ❌ Duplicates response system
- ❌ Complex session management

### Option 4: Response System Stream Awareness
**Approach:** Add stream type to response system context
**Impact:**
- ✅ Maintains existing interface
- ✅ Session context preserved
- ❌ Requires response system refactoring
- ❌ Complex implementation

## Recommended Approach

**Option 1: Add SendError() Method** appears to be the most practical solution because:

1. **Minimal Interface Change:** Only adds one method to base classes
2. **Session Context Preserved:** Uses existing response system
3. **Backward Compatible:** Existing Send() calls unchanged
4. **Clear Intent:** Explicit distinction between normal and error output
5. **Manageable Scope:** Limited number of files to modify

## Implementation Plan

### Phase 1: Interface Definition
1. Add `SendError()` to `GrapaSystemSend` base class
2. Add `SendError()` to `GrapaConsoleResponse` base class
3. Add `SendError()` to all response implementation classes

### Phase 2: Implementation
1. Implement `SendError()` in `GrapaConsole2Response` (stderr)
2. Implement `SendError()` in `GrapaEditorResponse` (error display)
3. Implement `SendError()` in `GrapaPromptResponse` (error display)
4. Implement `SendError()` in `GrapaWidgetThread` (error handling)

### Phase 3: Usage Update
1. Update session debug methods to use `SendError()`
2. Test stream separation on both Mac and Windows
3. Validate session context preservation

### Phase 4: Validation
1. Comprehensive testing across all response types
2. Performance impact assessment
3. Backward compatibility verification

## Conclusion

The session debug output stream separation issue has been **successfully resolved** through the implementation of the `SendError()` method across all response types. This solution demonstrates the architectural excellence of Grapa's polymorphic design:

### Key Achievements
1. **Stream Separation:** Normal output → stdout, debug output → stderr ✅
2. **Session Context Preservation:** Debug output maintains multiprocessing session isolation ✅
3. **Cross-Platform Compatibility:** Works consistently on Mac and Windows ✅
4. **Minimal Interface Change:** Only added `SendError()` methods to existing classes ✅
5. **Backward Compatibility:** All existing `Send()` calls remain unchanged ✅

### Architectural Validation
The successful resolution validates the **Template Method Pattern** and **polymorphic design** of the Grapa Send system:
- **Base class framework** provides the template methods
- **Derived class implementations** provide concrete stream handling
- **Echo methods** work with any response type through virtual method calls
- **Future extensibility** is built into the design

### Design Pattern Excellence
The Grapa Send system exemplifies excellent software architecture:
- **Separation of concerns** between output logic and destination handling
- **Extensibility** through polymorphic interfaces
- **Consistency** across all response implementations
- **Maintainability** through clear interface boundaries

This implementation serves as a model for how to add new functionality to existing polymorphic systems while maintaining architectural integrity and backward compatibility. 