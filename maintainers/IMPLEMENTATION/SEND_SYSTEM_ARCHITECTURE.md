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
    virtual void SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace);
    virtual GrapaRuleEvent* SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pMessage);
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
    virtual void SetActive(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, bool pActive);
};
```

**Purpose:** Provides the fundamental interface for all output operations.

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
**Location:** `source/grapa/GrapaSystem.h:20-26`

```cpp
class GrapaConsole2Response : public GrapaConsoleResponse {
public:
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaSystem.cpp:607-650`
- **SendCommand:** Outputs to `std::cout` (stdout)
- **SendPrompt:** Outputs to `std::cout` (stdout)  
- **SendEnd:** Outputs to `std::cout` (stdout)

#### 2. GrapaEditorResponse (GUI Editor)
**Location:** `source/grapa/GrapaSystem.h:82-89`

```cpp
class GrapaEditorResponse : public GrapaConsoleResponse {
public:
    Fl_Text_Display* disp;
    GrapaNames mRuleVariables;
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaSystem.cpp:1105-1129`
- **SendCommand:** Outputs to FLTK text display widget
- **SendPrompt:** Outputs to FLTK text display widget
- **SendEnd:** Outputs to FLTK text display widget

#### 3. GrapaPromptResponse (GUI Prompt)
**Location:** `source/grapa/GrapaSystem.h:94-101`

```cpp
class GrapaPromptResponse : public GrapaConsoleResponse {
public:
    My_Prompt_Editor* disp;
    GrapaNames mRuleVariables;
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Implementation:** `source/grapa/GrapaSystem.cpp:1130-1156`
- **SendCommand:** Outputs to FLTK prompt editor widget
- **SendPrompt:** Outputs to FLTK prompt editor widget
- **SendEnd:** Outputs to FLTK prompt editor widget

#### 4. GrapaWidgetThread (Widget System)
**Location:** `source/grapa/GrapaWidget.h:35-53`

```cpp
class GrapaWidgetThread : public GrapaConsoleResponse {
public:
    virtual void SendStart(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace);
    virtual GrapaRuleEvent* SendM(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pMessage);
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen);
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf);
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue);
};
```

**Purpose:** Handles output for widget-based GUI applications.

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
- **Basic Stream Redirection:** `> stdout.txt 2> stderr.txt` works for basic cases

### Not Working ❌
- **Session Debug Output:** Goes through response system → stdout instead of stderr
- **Mixed Output:** Session debug messages appear inline with normal program output

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

### 1. Response System is Universal
- **All output** goes through the response system
- **No direct stderr access** in response implementations
- **Session context** is maintained through `GrapaNames::GetResponse()`

### 2. Debug Output Split
- **System debug:** Direct `fprintf(stderr, ...)` ✅
- **Session debug:** Response system `Send()` → stdout ❌

### 3. Stream Separation Missing
- **No stream type parameter** in Send methods
- **No SendError() method** in base classes
- **All output treated as normal program output**

### 4. Session Context Critical
- **Debug output must maintain session context**
- **Cannot bypass response system** without losing session isolation
- **Multiprocessing safety** depends on response system

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

The session debug output stream separation issue requires a careful architectural solution that maintains Grapa's complex multiprocessing session management while adding proper stream awareness. The `SendError()` method approach provides the best balance of functionality, maintainability, and minimal disruption to the existing codebase. 