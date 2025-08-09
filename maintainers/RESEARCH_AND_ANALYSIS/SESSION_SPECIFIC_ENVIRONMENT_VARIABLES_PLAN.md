# Session-Specific Environment Variables Implementation Plan

## Overview

This document outlines the implementation plan for extending Grapa's `$sys().getenv()` and `$sys().putenv()` functions to support session-specific environment variables. This will allow multiple Grapa sessions to have isolated debug configurations and other session-specific settings without affecting the global OS environment or other sessions.

## Current Architecture Analysis

### Existing Implementation
Based on the current C++ code in `source/grapa/GrapaLibRule.cpp`:

```cpp
// Current getenv implementation (lines ~3360-3370)
else if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
{
    err = 0;
    result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR(getenv((char*)r1.vVal->mValue.mBytes)));
}

// Current putenv implementation (lines ~3400-3410)
else if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
{
    err = 0;
#ifdef WIN32
    _putenv_s((char*)r1.vVal->mValue.mBytes, r2.vVal && r2.vVal->mValue.mBytes ? (char*)r2.vVal->mValue.mBytes : "");
#else
    setenv((char*)r1.vVal->mValue.mBytes, r2.vVal && r2.vVal->mValue.mBytes ? (char*)r2.vVal->mValue.mBytes : "", 1);
#endif
}
```

### Session Architecture
Grapa's C++ architecture already supports session isolation through:

1. **`GrapaScriptState` class**: Each session has its own execution state (`vScriptExec->vScriptState`)
2. **`GrapaScriptExec`**: Executes scripts within a specific script state context
3. **`GrapaNames` class**: Each session has its own namespace (`pNameSpace`)

### Performance Concern Identified
**Issue**: Using `pNameSpace->Search()` for every debug variable access would require recursive namespace stack searches, which is inefficient for frequently accessed debug flags.

**Solution**: Implement session-specific storage directly in `GrapaScriptState` for efficient access.

## Efficient Implementation Strategy

### Recommended Approach: Direct Session Variables in GrapaScriptState

**Add session-specific storage directly to the GrapaScriptState class:**

```cpp
// In GrapaState.h - Add to GrapaScriptState class
class GrapaScriptState : public GrapaScriptExecState
{
public:
    GrapaItemParams mItemParams;
    GrapaRuleQueue mRuleStartQueue;
    GrapaNames* vRuleVariables;
    GrapaLocalDatabase mPath;
    GrapaRuleEvent mNet;
    GrapaConsoleSend* vConsoleSend;
    
    // NEW: Direct session variables for efficient access
    std::map<GrapaCHAR, GrapaCHAR> mSessionVars;  // Session-specific environment variables
    GrapaCritical mSessionVarsLock;  // Thread safety for session variables

public:
    GrapaScriptState();
    virtual ~GrapaScriptState();
    
    // NEW: Efficient session variable methods
    GrapaCHAR GetSessionVar(const GrapaCHAR& name);
    void SetSessionVar(const GrapaCHAR& name, const GrapaCHAR& value);
    bool HasSessionVar(const GrapaCHAR& name);
    void ClearSessionVars();
    
    virtual void CLEAR();
    virtual void LoadSystem();
    virtual void ClearQueue();
    // ... existing methods
};
```

## Recommended Implementation

### Phase 1: Extend GrapaScriptState Class

**Modify `source/grapa/GrapaState.h`:**

```cpp
class GrapaScriptState : public GrapaScriptExecState
{
public:
    GrapaItemParams mItemParams;
    GrapaRuleQueue mRuleStartQueue;
    GrapaNames* vRuleVariables;
    GrapaLocalDatabase mPath;
    GrapaRuleEvent mNet;
    GrapaConsoleSend* vConsoleSend;
    
    // NEW: Direct session variables for efficient access
    std::map<GrapaCHAR, GrapaCHAR> mSessionVars;
    GrapaCritical mSessionVarsLock;

public:
    GrapaScriptState();
    virtual ~GrapaScriptState();
    
    // NEW: Efficient session variable methods
    GrapaCHAR GetSessionVar(const GrapaCHAR& name) {
        mSessionVarsLock.WaitCritical();
        auto it = mSessionVars.find(name);
        GrapaCHAR result = (it != mSessionVars.end()) ? it->second : GrapaCHAR();
        mSessionVarsLock.LeaveCritical();
        return result;
    }
    
    void SetSessionVar(const GrapaCHAR& name, const GrapaCHAR& value) {
        mSessionVarsLock.WaitCritical();
        mSessionVars[name] = value;
        mSessionVarsLock.LeaveCritical();
    }
    
    bool HasSessionVar(const GrapaCHAR& name) {
        mSessionVarsLock.WaitCritical();
        bool result = (mSessionVars.find(name) != mSessionVars.end());
        mSessionVarsLock.LeaveCritical();
        return result;
    }
    
    void ClearSessionVars() {
        mSessionVarsLock.WaitCritical();
        mSessionVars.clear();
        mSessionVarsLock.LeaveCritical();
    }
    
    virtual void CLEAR() {
        if (GetQueue()) GetQueue()->CLEAR();
        mItemParams.mMapQueue.CLEAR();
        mItemState.CLEAR();
        mItemQueue.CLEAR();
        mRuleStartQueue.CLEAR();
        ClearSessionVars();  // NEW: Clear session variables
    }
    
    // ... existing methods
};
```

### Phase 2: Modify getenv() Implementation

**Modify `GrapaLibraryRuleGetEnvEvent::Run()` in `source/grapa/GrapaLibRule.cpp`:**

```cpp
else if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
{
    // NEW: Check session variables first (efficient direct access)
    GrapaScriptState* scriptState = vScriptExec ? vScriptExec->vScriptState : NULL;
    if (scriptState && scriptState->HasSessionVar(r1.vVal->mValue)) {
        GrapaCHAR sessionValue = scriptState->GetSessionVar(r1.vVal->mValue);
        result = new GrapaRuleEvent(0, GrapaCHAR(), sessionValue);
        err = 0;
    } else {
        // Fall back to OS environment variable
        result = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR(getenv((char*)r1.vVal->mValue.mBytes)));
        err = 0;
    }
}
```

### Phase 3: Modify putenv() Implementation

**Modify `GrapaLibraryRulePutEnvEvent::Run()` in `source/grapa/GrapaLibRule.cpp`:**

```cpp
else if (r1.vVal->mValue.mLength && r1.vVal->mValue.mBytes[0] != '$')
{
    // NEW: Store in session variables (efficient direct access)
    GrapaScriptState* scriptState = vScriptExec ? vScriptExec->vScriptState : NULL;
    if (scriptState) {
        GrapaCHAR value = (r2.vVal && r2.vVal->mValue.mBytes) ? r2.vVal->mValue : GrapaCHAR();
        scriptState->SetSessionVar(r1.vVal->mValue, value);
        err = 0;
    } else {
        // Fall back to OS environment variable
        err = 0;
#ifdef WIN32
        _putenv_s((char*)r1.vVal->mValue.mBytes, r2.vVal && r2.vVal->mValue.mBytes ? (char*)r2.vVal->mValue.mBytes : "");
#else
        setenv((char*)r1.vVal->mValue.mBytes, r2.vVal && r2.vVal->mValue.mBytes ? (char*)r2.vVal->mValue.mBytes : "", 1);
#endif
    }
}
```

## Use Cases

### 1. Session-Specific Debug Configuration
```grapa
/* Set session-specific debug flags */
$sys().putenv("DEBUG_MODE", "true");
$sys().putenv("LOG_LEVEL", "verbose");
$sys().putenv("PERF_TRACKING", "enabled");

/* These are isolated to this session only */
if ($sys().getenv("DEBUG_MODE") == "true") {
    "Session debug mode enabled".echo();
}
```

### 2. Parallel Session Isolation
```grapa
/* Session A */
$sys().putenv("SESSION_ID", "A");
$sys().putenv("DEBUG_LEVEL", "info");

/* Session B (running in parallel) */
$sys().putenv("SESSION_ID", "B");
$sys().putenv("DEBUG_LEVEL", "error");

/* Each session sees only its own variables */
"Session: " + $sys().getenv("SESSION_ID").echo();
"Debug Level: " + $sys().getenv("DEBUG_LEVEL").echo();
```

### 3. Performance Profiling Per Session
```grapa
/* Session-specific performance tracking */
$sys().putenv("PERF_START_TIME", $sys().utc().str());
$sys().putenv("PERF_OPERATION_COUNT", "0");

/* Track operations */
operation_count = $sys().getenv("PERF_OPERATION_COUNT").int() + 1;
$sys().putenv("PERF_OPERATION_COUNT", operation_count.str());
```

## Technical Considerations

### 1. Performance Benefits
- **Direct Access**: O(1) hash table lookup instead of O(n) recursive namespace search
- **Minimal Overhead**: Only affects environment variable lookups
- **Thread Safe**: Uses GrapaCritical for thread safety
- **Memory Efficient**: Session variables are cleaned up when session ends

### 2. Variable Lifetime
- **Session Variables**: Live only for the duration of the Grapa session
- **System Variables**: Persist across sessions (existing behavior)
- **OS Variables**: Fallback when session variable not found

### 3. Backward Compatibility
- **100% Compatible**: Existing scripts continue to work unchanged
- **Progressive Enhancement**: New session-specific features are optional
- **Fallback Behavior**: OS environment variables still accessible

## Testing Strategy

### 1. Unit Tests
- Test session variable isolation between parallel sessions
- Verify system variables remain global
- Test fallback to OS environment variables

### 2. Integration Tests
- Test with existing Grapa scripts
- Verify no regression in current functionality
- Test performance impact on environment variable access

### 3. Edge Cases
- Test with very long variable names/values
- Test with special characters in variable names
- Test memory cleanup when sessions end

## Implementation Steps

### Step 1: Extend GrapaScriptState Class
1. Add `mSessionVars` map and `mSessionVarsLock` to `GrapaScriptState`
2. Implement `GetSessionVar`, `SetSessionVar`, `HasSessionVar`, `ClearSessionVars` methods
3. Update `CLEAR()` method to clear session variables

### Step 2: Modify getenv() and putenv()
1. Update `GrapaLibraryRuleGetEnvEvent::Run()` to check session variables first
2. Update `GrapaLibraryRulePutEnvEvent::Run()` to store in session variables
3. Ensure proper fallback to OS environment variables

### Step 3: Compilation and Testing
1. Compile with `python3 build.py --exe-only`
2. Run existing test suites to verify no regressions
3. Create new test scripts for session isolation

### Step 4: Documentation Updates
1. Update `docs-src/docs/sys/sys.md` with session-specific examples
2. Update `docs-src/docs/cli_quickstart.md` with session usage patterns
3. Add examples to relevant migration guides

## Benefits

### 1. **Performance**: O(1) direct access instead of O(n) recursive search
### 2. **Debug Isolation**: Each session can have independent debug configurations
### 3. **Parallel Development**: Multiple developers can run different debug levels simultaneously
### 4. **Performance Tracking**: Session-specific metrics without interference
### 5. **Clean Architecture**: Leverages existing GrapaScriptState execution state management
### 6. **Zero Breaking Changes**: Completely backward compatible

## Risks and Mitigation

### 1. **Memory Leaks**
- **Risk**: Session variables not properly cleaned up
- **Mitigation**: Clear session variables in `CLEAR()` method

### 2. **Thread Safety**
- **Risk**: Race conditions in multi-threaded environments
- **Mitigation**: Use `GrapaCritical` for thread-safe access

### 3. **Performance Degradation**
- **Risk**: Slower environment variable lookups
- **Mitigation**: Direct hash table access is faster than recursive search

## Future Enhancements

### 1. **Variable Scoping**: Support for function-local environment variables
### 2. **Variable Export**: Ability to export session variables to child processes
### 3. **Variable Persistence**: Save/restore session configurations
### 4. **Variable Validation**: Type checking and validation for session variables

## Conclusion

This implementation provides a clean, efficient way to add session-specific environment variables to Grapa while maintaining full backward compatibility. It leverages the existing `GrapaScriptState` execution state management and provides immediate benefits for debugging and development workflows.

The direct session variable storage approach in `GrapaScriptState` eliminates the performance concern about recursive namespace searches while providing the session isolation functionality needed for effective debugging and development. 