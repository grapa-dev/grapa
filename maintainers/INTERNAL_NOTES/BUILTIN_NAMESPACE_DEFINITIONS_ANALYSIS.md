# Built-in Namespace Definitions Analysis

**Date**: December 2024  
**Investigation**: Built-in Namespace Definitions and Class Registration  
**Status**: Analysis Complete

## Overview

This document analyzes how Grapa defines and registers built-in classes and functions in the global namespace, examining the patterns used for system objects like `$file`, `$sys`, `$net`, `$thread`, and `$widget`.

## Current Built-in Namespace System

### **Namespace Hierarchy**

Grapa uses a dynamic namespace system with the following hierarchy:

1. **Current Context** (within `{ }` brackets)
2. **Function Scope** (local variables)
3. **Local Namespace** (`$local`)
4. **Call Stack Namespaces** (parent functions)
5. **Global Namespace** (`$global`)

### **Built-in Namespace Keywords**

**Core Namespace Keywords**:
- `$global` - Global namespace for persistent variables
- `$local` - Local namespace (automatic for each function)
- `$this` - Current object context
- `$parent` - Parent object context
- `$root` - Root namespace reference

## Built-in Class Registration System

### **Library Function Registration Pattern**

All built-in classes are registered through a centralized library function lookup system in `GrapaLibRule.cpp`:

```cpp
// From GrapaLibRule.cpp - Library function registration
GrapaLibraryEvent* GrapaLibraryRuleEvent::Handle(GrapaCHAR& pName)
{
    GrapaLibraryEvent* lib = NULL;
    
    // File system functions
    if (pName.Cmp("file_table") == 0) lib = new GrapaLibraryRuleTableEvent(pName);
    else if (pName.Cmp("file_pwd") == 0) lib = new GrapaLibraryRulePwdEvent(pName);
    else if (pName.Cmp("file_cd") == 0) lib = new GrapaLibraryRuleCdEvent(pName);
    else if (pName.Cmp("file_ls") == 0) lib = new GrapaLibraryRuleLsEvent(pName);
    else if (pName.Cmp("file_mk") == 0) lib = new GrapaLibraryRuleMkEvent(pName);
    else if (pName.Cmp("file_rm") == 0) lib = new GrapaLibraryRuleRmEvent(pName);
    else if (pName.Cmp("file_get") == 0) lib = new GrapaLibraryRuleGetEvent(pName);
    else if (pName.Cmp("file_set") == 0) lib = new GrapaLibraryRuleSetEvent(pName);
    
    // Network functions
    else if (pName.Cmp("net_connect") == 0) lib = new GrapaLibraryRuleConnectEvent(pName);
    else if (pName.Cmp("net_send") == 0) lib = new GrapaLibraryRuleSendEvent(pName);
    else if (pName.Cmp("net_receive") == 0) lib = new GrapaLibraryRuleReceiveEvent(pName);
    
    // Thread functions
    else if (pName.Cmp("thread_start") == 0) lib = new GrapaLibraryRuleStartEvent(pName);
    else if (pName.Cmp("thread_stop") == 0) lib = new GrapaLibraryRuleStopEvent(pName);
    
    // Widget functions
    else if (pName.Cmp("widget_new") == 0) lib = new GrapaLibraryRuleWidgetNewEvent(pName);
    else if (pName.Cmp("widget_show") == 0) lib = new GrapaLibraryRuleWidgetShowEvent(pName);
    
    return lib;
}
```

### **Class Definition Pattern**

Each built-in class follows a consistent pattern:

```cpp
// Example: File system class
class GrapaLibraryRuleGetEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleGetEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};

// Example: Network class
class GrapaLibraryRuleConnectEvent : public GrapaLibraryEvent
{
public:
    GrapaLibraryRuleConnectEvent(GrapaCHAR& pName) { mName.FROM(pName); };
    virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput);
};
```

## Built-in Class Categories

### **1. File System Classes (`$file`)**

**Core Functions**:
- `file_get` - Read file content
- `file_set` - Write file content
- `file_ls` - List directory contents
- `file_mk` - Create directory
- `file_rm` - Remove file/directory
- `file_cd` - Change directory
- `file_pwd` - Get current directory
- `file_table` - Create database table
- `file_mkfield` - Create table field
- `file_rmfield` - Remove table field
- `file_info` - Get file information
- `file_debug` - Debug file operations

**Class Structure**:
```cpp
// File system operations are implemented as individual library events
// Each function is a separate class inheriting from GrapaLibraryEvent
```

### **2. System Classes (`$sys`)**

**Core Functions**:
- `sys_eval` - Evaluate script
- `sys_compile` - Compile script
- `sys_getenv` - Get environment variable
- `sys_setenv` - Set environment variable
- `sys_version` - Get system version

**Class Structure**:
```cpp
// System functions are implemented as individual library events
// Each function is a separate class inheriting from GrapaLibraryEvent
```

### **3. Network Classes (`$net`)**

**Core Functions**:
- `net_connect` - Connect to network
- `net_bind` - Bind to port
- `net_listen` - Listen for connections
- `net_send` - Send data
- `net_receive` - Receive data
- `net_disconnect` - Disconnect
- `net_host` - Get host information
- `net_certificate` - Handle certificates

**Class Structure**:
```cpp
// Network operations are implemented as individual library events
// Each function is a separate class inheriting from GrapaLibraryEvent
```

### **4. Thread Classes (`$thread`)**

**Core Functions**:
- `thread_start` - Start thread
- `thread_stop` - Stop thread
- `thread_suspend` - Suspend thread
- `thread_resume` - Resume thread
- `thread_lock` - Lock thread
- `thread_unlock` - Unlock thread
- `thread_wait` - Wait for thread
- `thread_signal` - Signal thread

**Class Structure**:
```cpp
// Thread operations are implemented as individual library events
// Each function is a separate class inheriting from GrapaLibraryEvent
```

### **5. Widget Classes (`$widget`)**

**Core Functions**:
- `widget_new` - Create widget
- `widget_show` - Show widget
- `widget_hide` - Hide widget
- `widget_resize` - Resize widget
- `widget_get` - Get widget property
- `widget_set` - Set widget property
- `widget_callback` - Set widget callback

**Class Structure**:
```cpp
// Widget operations are implemented as individual library events
// Each function is a separate class inheriting from GrapaLibraryEvent
```

## Dynamic Class Registration

### **Python Integration Example**

The Python integration demonstrates dynamic class registration:

```cpp
// From mainpy.cpp - Dynamic class registration
GrapaCHAR runStr("$global[\"$py\"] = class {eval = op(script,locals={},import=\"\",attr=\"\"){@<\"py\",\"eval\",{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};exec = op(script,locals={},import=\"\",attr=\"\"){@<\"py\",\"exec\",{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};};");
grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
```

**Pattern Analysis**:
1. **Global Registration**: `$global["$py"] = class { ... }`
2. **Method Definition**: `eval = op(script,locals={},import="",attr=""){ ... }`
3. **Library Call**: `@<"py","eval",{...}>()` - Calls Python library
4. **Parameter Passing**: Variables passed as `@<var,{script}>` format

### **Grammar Definition**

The grammar system supports dynamic class definitions:

```grapa
// From lib/grapa/$grapa.grc - Grammar support for built-ins
@global["$lit"]
    = rule '@' this {@<this,{}>}
    | '@' parent {@<parent,{}>}
    | '@' global {@<global,{}>}
    | '@' local {@<local,{}>}
    | '@' '<' <$op_list> '>'
    | '@' '[' <$code_list> ']'
    | '@' <$litname> {@<var,{$2}>}
    | this {@<this,{}>}
    | global {@<global,{}>}
    | local {@<local,{}>}
    | $this {@<this,{}>}
    | $parent {@<parent,{}>}
    | $global {@<global,{}>}
    | $local {@<local,{}>}
    | $root {@<var,{$1}>}
    | $self {@<var,{$1}>}
    | $ID {@<var,{$1}>}
    | $SYSID {@<lit,{$1}>}
    | $STR {@<lit,{$1}>}
    | $SYSSTR {@<lit,{$1}>}
    | $INT {@<lit,{$1}>}
    | $RAW {@<lit,{$1}>}
    | $SYSINT {@<lit,{$1}>}
    | $FLOAT {@<lit,{$1}>}
    ;
```

## Namespace Resolution Process

### **Variable Lookup Process**

1. **Current Context**: Variables within current `{ }` block
2. **Function Scope**: Local variables in current function
3. **Local Namespace**: `$local` variables
4. **Call Stack**: Variables from parent functions
5. **Global Namespace**: `$global` variables
6. **Built-in Functions**: Library function lookup

### **Library Function Resolution**

```cpp
// From GrapaState.cpp - Library function resolution
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

## Implications for GrapaDBX

### **Current State Analysis**

**Built-in Class System**:
- ✅ **Well-Defined**: Clear patterns for class registration
- ✅ **Extensible**: Easy to add new built-in classes
- ✅ **Consistent**: All classes follow same inheritance pattern
- ✅ **Dynamic**: Support for runtime class registration

**Namespace Management**:
- ✅ **Hierarchical**: Clear namespace resolution order
- ✅ **Dynamic**: Automatic namespace creation
- ✅ **Flexible**: Support for global and local scoping

### **GrapaDBX Integration Opportunities**

**1. Formula Field Integration**:
```cpp
// Potential formula field registration
if (pName.Cmp("dbx_formula") == 0) lib = new GrapaLibraryRuleDBXFormulaEvent(pName);
if (pName.Cmp("dbx_execute") == 0) lib = new GrapaLibraryRuleDBXExecuteEvent(pName);
```

**2. Database Class Registration**:
```cpp
// Potential database class registration
if (pName.Cmp("dbx_create") == 0) lib = new GrapaLibraryRuleDBXCreateEvent(pName);
if (pName.Cmp("dbx_open") == 0) lib = new GrapaLibraryRuleDBXOpenEvent(pName);
if (pName.Cmp("dbx_query") == 0) lib = new GrapaLibraryRuleDBXQueryEvent(pName);
```

**3. Dynamic Formula Registration**:
```grapa
// Potential dynamic formula registration
$global["$dbx"] = class {
    formula = op(name, expression) { @<"dbx","formula",{@<var,{name}>,@<var,{expression}>}>(); };
    execute = op(formula, params) { @<"dbx","execute",{@<var,{formula}>,@<var,{params}>}>(); };
};
```

## Recommendations

### **Immediate Actions (Not Required for Current Work)**

1. **Document Current Patterns**: The current built-in class system is well-designed and documented
2. **Maintain Consistency**: Continue using the established patterns for new classes
3. **Leverage Existing Infrastructure**: Use the library function registration system for new features

### **Future Enhancements**

**1. GrapaDBX Class Registration**:
- Register GrapaDBX functions as built-in classes
- Follow existing naming patterns (`dbx_*`)
- Use established inheritance hierarchy

**2. Formula Class Integration**:
- Create formula-specific library events
- Integrate with existing namespace system
- Maintain backward compatibility

**3. Dynamic Class Loading**:
- Extend current dynamic registration system
- Support runtime class definition
- Maintain type safety and validation

## Risk Assessment

### **Low Risk**
- **Current System**: Well-tested and stable
- **Pattern Consistency**: Clear and consistent patterns
- **Backward Compatibility**: Existing code continues to work

### **Medium Risk**
- **Dynamic Registration**: Runtime class creation complexity
- **Namespace Pollution**: Potential for naming conflicts
- **Performance**: Dynamic lookup overhead

### **High Risk**
- **Breaking Changes**: Modifying core namespace system
- **Complexity**: Over-engineering the class system
- **Maintenance**: Complex dynamic class management

## Conclusion

The current Grapa built-in namespace system is well-designed and provides a solid foundation for extending functionality. The library function registration pattern is consistent, extensible, and supports both static and dynamic class registration.

**Recommendation**: Continue using the existing built-in class registration patterns for GrapaDBX integration. The current system provides all necessary infrastructure for adding new database and formula functionality.

**Priority**: Low - this investigation provides valuable context for future development but doesn't impact current GrapaDBX work.

---

*This analysis provides the foundation for understanding Grapa's built-in namespace system and its implications for future GrapaDBX integration.* 