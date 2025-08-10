# Component-Specific Debug Levels

## Overview

The Grapa debug system now supports component-specific debug levels, allowing different verbosity levels for different software components. This enables fine-grained control over debug output, making it possible to have high verbosity for specific components while keeping others quiet.

## Problem Statement

The original question was: "Is it conceivable that someone would want debug level 1 for everything but then debug level 2 for just one of the components? Or debug level 0 (nothing) for everything, and debug level 3 for just one of the components?"

**Answer: Yes, absolutely!** This is a very common debugging scenario:

1. **Focused Debugging**: When investigating a specific component issue, you want detailed output from that component but minimal noise from others
2. **Performance Analysis**: When profiling specific components, you need high verbosity for the target component
3. **Production Debugging**: When debugging in production, you want to enable debug for specific components without overwhelming the logs
4. **Component Isolation**: When testing individual components, you want to isolate their debug output

## Solution Design

### Component Level Format

The component string format has been extended to support level specifications:

#### Basic Format
```
"component:level[,component:level,...]"
```

#### Examples
- `"grep:2,vector:1,database:3"` - Specific levels per component
- `"grep:3,*:1"` - grep at level 3, everything else at level 1
- `"grep:3,*:0"` - grep at level 3, everything else disabled
- `"grep:2,vector,database:3"` - Mixed format (vector uses default level)

### Implementation Details

#### C++ Methods Added

**GrapaDebug** (System-level):
```cpp
int GetComponentDebugLevel(const char* component);
bool ShouldDebug(const char* component, int minLevel);
void DebugPrint(const char* component, const char* pStr, int level = 0, bool flush = true);
void DebugPrint(const char* component, const GrapaCHAR& pValue, int level = 0, bool flush = true);
```

**GrapaScriptExecStateDebug** (Session-level):
```cpp
int GetComponentDebugLevel(const char* component);
bool ShouldDebug(const char* component, int minLevel);
void DebugPrint(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const char* component, const char* pStr, int level = 0);
void DebugPrint(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const char* component, const GrapaCHAR& pValue, int level = 0);
```

#### Parsing Logic

The `GetComponentDebugLevel` method implements sophisticated parsing:

1. **Exact Component Match**: Look for `"component:"` pattern
2. **Wildcard Level**: Look for `"*:"` pattern for default level
3. **Fallback**: Use simple component matching with default level
4. **Boundary Checking**: Ensure complete component name matches

#### Environment Variables

**System-level**:
- `GRAPA_DEBUG_COMPONENTS`: Component list with optional levels

**Session-level**:
- `GRAPA_SESSION_DEBUG_COMPONENTS`: Session-specific component list with optional levels

## Usage Scenarios

### Scenario 1: Focused Component Debugging
```grapa
/* Enable high verbosity for grep, minimal for others */
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "grep:3,*:1");
```

**Result**:
- grep: Debug levels 3 and above
- All other components: Debug levels 1 and above

### Scenario 2: Single Component Debugging
```grapa
/* Only debug database component */
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "database:3,*:0");
```

**Result**:
- database: Debug levels 3 and above
- All other components: Disabled

### Scenario 3: Mixed Verbosity Levels
```grapa
/* Different levels for different components */
$sys().putenv("GRAPA_DEBUG_COMPONENTS", "grep:2,vector:1,database:3,network:0");
```

**Result**:
- grep: Debug levels 2 and above
- vector: Debug levels 1 and above
- database: Debug levels 3 and above
- network: Disabled
- Other components: Use default level

### Scenario 4: Session-Specific Override
```grapa
/* Session-specific component levels */
$sys().putenv("GRAPA_SESSION_DEBUG", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "vector:3,database:1,*:0");
```

**Result**:
- Session overrides system debug for these components
- vector: Debug levels 3 and above
- database: Debug levels 1 and above
- All other components: Disabled

## C++ Usage Examples

### System-Level Debug
```cpp
// Check if component should debug at level 2
if (gSystem->mDebug.ShouldDebug("grep", 2)) {
    gSystem->mDebug.DebugPrint("grep", "Detailed grep operation", 2);
}

// Get component's debug level
int grepLevel = gSystem->mDebug.GetComponentDebugLevel("grep");
if (grepLevel >= 3) {
    gSystem->mDebug.DebugPrint("grep", "Very detailed grep info", 3);
}
```

### Session-Level Debug
```cpp
// Check if component should debug at level 1
if (vScriptExec->vScriptState->mDebug.ShouldDebug("vector", 1)) {
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "vector", "Vector operation", 1);
}

// Get component's debug level
int vectorLevel = vScriptExec->vScriptState->mDebug.GetComponentDebugLevel("vector");
if (vectorLevel >= 2) {
    vScriptExec->vScriptState->mDebug.DebugPrint(vScriptExec, pNameSpace, "vector", "Detailed vector info", 2);
}
```

## Benefits

1. **Granular Control**: Fine-grained control over debug verbosity per component
2. **Performance**: Reduce debug output noise by disabling unnecessary components
3. **Focus**: Concentrate debug output on specific components under investigation
4. **Flexibility**: Support for both system-wide and session-specific component levels
5. **Backward Compatibility**: Existing component lists without levels continue to work
6. **Wildcard Support**: Easy configuration for "everything except X" scenarios

## Implementation Notes

- **Thread Safety**: All debug operations use appropriate locks
- **Memory Safety**: String parsing includes boundary checks
- **Performance**: Efficient string search with early termination
- **Error Handling**: Graceful fallback for malformed component strings
- **Default Behavior**: Components without level specification use the default debug level

## Future Enhancements

1. **Component Aliases**: Support for component name aliases
2. **Hierarchical Components**: Support for nested component names (e.g., "database.btree")
3. **Dynamic Updates**: Runtime component level changes without restart
4. **Debug Profiles**: Predefined debug configurations for common scenarios
5. **Log Rotation**: Component-specific log file rotation
6. **Performance Metrics**: Component-specific debug performance impact tracking 