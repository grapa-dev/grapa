# Library Specification in $OP Syntax Analysis

**Date**: December 2024  
**Investigation**: Library Specification in $OP Syntax and Dynamic Loading  
**Status**: Analysis Complete

## Overview

This document analyzes the current library specification mechanisms in Grapa's $OP syntax, examining the three possible ways to specify libraries and their implications for future dynamic loading and version compatibility.

## Current Library Specification Mechanisms

### 1. **Single Parameter (Default Library)**

**Syntax**: `@<[op,function_name],{}>`
**Example**: `@<[op,file_get],{}>`

**Implementation**: 
```cpp
case 1:
    libName = &gSystem->mLib;  // Default library
    libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
    break;
```

**Usage**: 
- Function name is looked up in the default library (`gSystem->mLib`)
- Current pattern: `file_get`, `net_connect`, `widget_new`
- Simple and straightforward

### 2. **Two Parameters (Library + Function)**

**Syntax**: `@<[op,library_name,function_name],{}>`
**Example**: `@<[op,file,get],{}>`

**Implementation**:
```cpp
case 2:
    inputItem = (GrapaRuleEvent*)pOperation->vQueue->Tail();
    if (inputItem && inputItem->mValue.mToken == GrapaTokenType::LIST && !inputItem->mNull)
    {
        libName = &gSystem->mLib;  // Default library
        libParam = (GrapaRuleEvent*)pOperation->vQueue->Head();
    }
    else
    {
        libName = (GrapaRuleEvent*)pOperation->vQueue->Head();  // Explicit library
        libParam = inputItem;
        inputItem = NULL;
        if (libName==NULL || libName->mValue.StrCmp("$SYS") == 0 || 
            (libName->mValue.mToken == GrapaTokenType::SYSID && libName->mValue.StrCmp("SYS") == 0)) 
            libName = &gSystem->mLib;
    }
    break;
```

**Usage**:
- Allows explicit library specification
- Falls back to default library if library name is `$SYS` or `SYS`
- Enables future dynamic library loading

### 3. **Three Parameters (Library + Function + Input)**

**Syntax**: `@<[op,library_name,function_name,input_data],{}>`
**Example**: `@<[op,file,get,filename],{}>`

**Implementation**:
```cpp
case 3:
    libName = (GrapaRuleEvent*)pOperation->vQueue->Head();
    libParam = libName ? libName->Next() : NULL;
    inputItem = libParam ? libParam->Next() : NULL;
    if (libName == NULL || libName->mValue.StrCmp("$SYS") == 0 || 
        (libName->mValue.mToken == GrapaTokenType::SYSID && libName->mValue.StrCmp("SYS") == 0)) 
        libName = &gSystem->mLib;
    break;
```

**Usage**:
- Full parameter specification
- Includes input data as third parameter
- Most flexible for complex operations

## Current Library Loading Mechanism

### **LoadLib Function**
```cpp
void GrapaScriptExec::LoadLib(GrapaRuleEvent *libName)
{
    if (libName && libName->vLibraryEvent == NULL)
    {
        s64 idx;
        GrapaCHAR s;
        if (libName->mValue.mToken == GrapaTokenType::SYSID)
        {
            s.FROM("$");
            s.Append(libName->mValue);
        }
        else
            s.FROM(libName->mValue);
        libName->vLibraryEvent = gSystem->mLibraryQueue.Search(s,idx);
        if (libName->vLibraryEvent == NULL)
        {
            if (s.StrCmp("$SYS") == 0) libName->vLibraryEvent = new GrapaLibraryRuleEvent(s);
            if (libName->vLibraryEvent)
                gSystem->mLibraryQueue.PushTail(libName->vLibraryEvent);
        }
    }
}
```

### **Library Function Registration**
```cpp
// From GrapaLibRule.cpp
if (pName.Cmp("file_get") == 0) lib = new GrapaLibraryRuleGetEvent(pName);
if (pName.Cmp("net_connect") == 0) lib = new GrapaLibraryRuleConnectEvent(pName);
if (pName.Cmp("widget_new") == 0) lib = new GrapaLibraryRuleWidgetNewEvent(pName);
```

## Current Usage Patterns

### **Pattern 1: Single Function Names (Most Common)**
```grapa
/* Current usage */
@<[op,file_get],{}>  /* Single parameter */
@<[op,net_connect],{}>  /* Single parameter */
@<[op,widget_new],{}>  /* Single parameter */
```

### **Pattern 2: Library + Function (Not Currently Used)**
```grapa
/* Potential future usage */
@<[op,file,get],{}>  /* Two parameters */
@<[op,net,connect],{}>  /* Two parameters */
@<[op,widget,new],{}>  /* Two parameters */
```

### **Pattern 3: Full Parameter Specification (Not Currently Used)**
```grapa
/* Potential future usage */
@<[op,file,get,filename],{}>  /* Three parameters */
@<[op,net,connect,host],{}>  /* Three parameters */
```

## Implications for Dynamic Library Loading

### **Current Limitations**
1. **Static Registration**: All library functions are statically registered in `GrapaLibRule.cpp`
2. **No Dynamic Loading**: No mechanism to load libraries at runtime
3. **Fixed Function Names**: Function names are hardcoded in the lookup table

### **Future Dynamic Loading Potential**

#### **Option 1: Library Name Resolution**
```cpp
// Enhanced LoadLib function
void GrapaScriptExec::LoadLib(GrapaRuleEvent *libName)
{
    if (libName && libName->vLibraryEvent == NULL)
    {
        GrapaCHAR s;
        if (libName->mValue.mToken == GrapaTokenType::SYSID)
        {
            s.FROM("$");
            s.Append(libName->mValue);
        }
        else
            s.FROM(libName->mValue);
        
        // Try to find existing library
        s64 idx;
        libName->vLibraryEvent = gSystem->mLibraryQueue.Search(s,idx);
        
        // If not found, try dynamic loading
        if (libName->vLibraryEvent == NULL)
        {
            libName->vLibraryEvent = LoadDynamicLibrary(s);
            if (libName->vLibraryEvent)
                gSystem->mLibraryQueue.PushTail(libName->vLibraryEvent);
        }
    }
}
```

#### **Option 2: Function Name Resolution**
```cpp
// Enhanced function lookup
GrapaLibraryEvent* LoadDynamicFunction(const GrapaCHAR& libraryName, const GrapaCHAR& functionName)
{
    // Load library if not already loaded
    GrapaLibraryEvent* library = LoadLibrary(libraryName);
    if (!library) return NULL;
    
    // Look up function in library
    return library->GetFunction(functionName);
}
```

## Version Compatibility Considerations

### **Current State**
- **No Version Information**: Library functions don't include version data
- **Static Registration**: All functions registered at compile time
- **No Compatibility Checking**: No mechanism to check function compatibility

### **Future Enhancement Path**

#### **Phase 1: Library Version Embedding**
```cpp
// Enhanced library registration
struct LibraryInfo {
    GrapaCHAR name;
    GrapaCHAR version;
    GrapaCHAR functions;  // JSON or similar format
};

// Version-aware library loading
GrapaLibraryEvent* LoadVersionedLibrary(const GrapaCHAR& libraryName, const GrapaCHAR& requiredVersion)
{
    LibraryInfo info = GetLibraryInfo(libraryName);
    if (!IsVersionCompatible(info.version, requiredVersion))
    {
        // Handle version mismatch
        return LoadCompatibleVersion(libraryName, requiredVersion);
    }
    return LoadLibrary(libraryName);
}
```

#### **Phase 2: Function-Level Versioning**
```cpp
// Function-level version information
struct FunctionInfo {
    GrapaCHAR name;
    GrapaCHAR version;
    GrapaCHAR signature;
    GrapaCHAR implementation;
};

// Version-aware function lookup
GrapaLibraryEvent* LoadVersionedFunction(const GrapaCHAR& libraryName, const GrapaCHAR& functionName, const GrapaCHAR& requiredVersion)
{
    FunctionInfo info = GetFunctionInfo(libraryName, functionName);
    if (!IsVersionCompatible(info.version, requiredVersion))
    {
        // Handle function version mismatch
        return LoadCompatibleFunction(libraryName, functionName, requiredVersion);
    }
    return LoadFunction(libraryName, functionName);
}
```

## Recommendations for GrapaDB2

### **Immediate Actions (Not Required for Current Work)**

1. **Document Current Patterns**: The current single-parameter pattern is sufficient for GrapaDB2
2. **Maintain Backward Compatibility**: Keep existing function naming patterns
3. **Prepare for Future**: The infrastructure exists for dynamic loading but isn't needed now

### **Future Considerations**

#### **Dynamic Library Loading**
- **Library Name Resolution**: Use Pattern 2 (`@<[op,library,function],{}>`) for dynamic libraries
- **Function Name Resolution**: Use Pattern 3 (`@<[op,library,function,input],{}>`) for complex operations
- **Version Compatibility**: Embed version information in library metadata

#### **Backward Compatibility**
- **Legacy Support**: Maintain support for Pattern 1 (`@<[op,function],{}>`) for existing code
- **Migration Path**: Provide tools to convert between patterns
- **Documentation**: Clear documentation of when to use each pattern

## Implementation Strategy

### **Current GrapaDB2 Approach**
- **Use Existing Pattern**: Continue using single-parameter pattern for formula functions
- **No Changes Required**: Current implementation is sufficient
- **Future-Ready**: Infrastructure exists for future enhancements

### **Future Enhancement Path**

#### **Phase 1: Library Infrastructure**
```cpp
// Add to GrapaSystem
GrapaHashMap<GrapaCHAR, GrapaLibraryEvent*> mDynamicLibraries;
GrapaHashMap<GrapaCHAR, GrapaCHAR> mLibraryVersions;

// Enhanced library loading
GrapaLibraryEvent* LoadDynamicLibrary(const GrapaCHAR& libraryName);
GrapaLibraryEvent* GetLibraryFunction(const GrapaCHAR& libraryName, const GrapaCHAR& functionName);
```

#### **Phase 2: Version Management**
```cpp
// Version compatibility checking
bool IsLibraryCompatible(const GrapaCHAR& libraryName, const GrapaCHAR& requiredVersion);
bool IsFunctionCompatible(const GrapaCHAR& libraryName, const GrapaCHAR& functionName, const GrapaCHAR& requiredVersion);

// Version-aware loading
GrapaLibraryEvent* LoadCompatibleLibrary(const GrapaCHAR& libraryName, const GrapaCHAR& requiredVersion);
```

#### **Phase 3: Dynamic Loading**
```cpp
// Runtime library loading
GrapaError LoadLibraryFromFile(const GrapaCHAR& libraryPath);
GrapaError UnloadLibrary(const GrapaCHAR& libraryName);
GrapaError ReloadLibrary(const GrapaCHAR& libraryName);
```

## Risk Assessment

### **Low Risk**
- **Current Implementation**: Single-parameter pattern is stable and well-tested
- **Backward Compatibility**: Existing code will continue to work
- **No Breaking Changes**: Current approach doesn't require changes

### **Medium Risk**
- **Dynamic Loading**: Adds complexity to library management
- **Version Compatibility**: Requires careful version management
- **Performance**: Dynamic loading may have performance implications

### **High Risk**
- **Breaking Changes**: Future changes to library specification could break existing code
- **Complexity**: Dynamic loading adds significant complexity
- **Maintenance**: More complex library management system

## Conclusion

The current Grapa system has a well-designed infrastructure for library specification with three distinct patterns, but only the single-parameter pattern is currently used. The infrastructure exists for future dynamic loading and version compatibility, but these features are not required for the current GrapaDB2 implementation.

**Recommendation**: Continue using the current single-parameter pattern for GrapaDB2 formula functions. The existing infrastructure provides a clear path for future enhancements without requiring immediate changes.

**Priority**: Low - this investigation provides valuable context for future development but doesn't impact current GrapaDB2 work.

---

*This analysis provides the foundation for understanding Grapa's library specification mechanisms and their implications for future dynamic loading and version compatibility features.* 