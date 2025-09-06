---
tags:
  - maintainer
  - lowlevel
  - implementation
  - core-system
  - index
---

# Core System Implementation Documentation

This directory contains documentation for the core system components of Grapa, including namespace management, rule execution, and fundamental system behaviors.

## Core System Components

### Namespace and Scope Management
- **[NAMESPACE_STACK_ARCHITECTURE.md](NAMESPACE_STACK_ARCHITECTURE.md)** - Comprehensive documentation of the namespace stack system, including `$global`, `$parent`, `$this`, `$local` implementations and call stack tracing capabilities

### Rule Event System
- **[GRAPA_RULE_EVENT_MVAR_ANALYSIS.md](GRAPA_RULE_EVENT_MVAR_ANALYSIS.md)** - Analysis of the `mVar` flag in GrapaRuleEvent and its usage patterns
- **[GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md](GRAPA_RULE_EVENT_MVAR_IMPLEMENTATION.md)** - Implementation details for variable mode handling in rule events

### File System Integration
- **[AUTOMATIC_FILE_LOADING_MECHANISM.md](AUTOMATIC_FILE_LOADING_MECHANISM.md)** - Documentation of how Grapa automatically loads and manages files

### Property System
- **[PROPERTY_ASSIGNMENT_HACK_IMPLEMENTATION.md](PROPERTY_ASSIGNMENT_HACK_IMPLEMENTATION.md)** - Implementation details for property assignment mechanisms

## Key Concepts

### Namespace Stack Architecture
The namespace stack is implemented as a linked list of `GrapaNames` objects, where each object represents a namespace context with parent-child relationships. This enables:

- **Variable scope resolution** with hierarchical lookup
- **Call stack tracing** through `$parent` navigation
- **Thread isolation** with separate namespace contexts
- **Widget and network integration** with specialized contexts

### Rule Event System
The rule event system manages execution contexts and variable references through the `mVar` flag, enabling:

- **Reference vs. value semantics** for different data types
- **Scope-aware variable handling** for function parameters and returns
- **Memory management** with automatic cleanup

### File System Integration
Grapa's file system integration provides:

- **Automatic file loading** based on context and usage
- **Seamless database and file system navigation** with the `.cd()` command
- **Unified path system** for different storage backends

## Implementation Patterns

### Namespace Navigation
```cpp
// Navigate to root namespace
GrapaNames* par = pNameSpace->GetSkipThis();
while (par->GetSkipParrent()) par = par->GetSkipParrent();

// Access parent namespace
GrapaNames* parent = pNameSpace->GetParrent();

// Get current namespace queue
GrapaRuleQueue* queue = pNameSpace->GetNameQueue();
```

### Variable Resolution
```cpp
// Search through namespace hierarchy
GrapaNames* names = mNameSpace->GetSkipThis();
while (!parameter && names) {
    // Search current namespace
    GrapaRuleEvent* operation = names->GetNameQueue()->Tail();
    while (operation) {
        // Check for variable match
        operation = operation->Prev();
    }
    names = names->GetSkipParrent();  // Move to parent
}
```

### Scope Management
```cpp
// Create new namespace context
GrapaNames newNamespace;
newNamespace.SetParrent(parentNamespace);

// Add variables to namespace
GrapaRuleEvent* var = new GrapaRuleEvent(/* ... */);
newNamespace.GetNameQueue()->PushTail(var);

// Automatic cleanup on scope exit
// ~GrapaNames() handles cleanup
```

## Related Documentation

- **[CORE/EXECUTION_TREES.md](../CORE/EXECUTION_TREES.md)** - Execution tree model and compilation
- **[LANGUAGE_FEATURES/RULE_SYSTEM_ARCHITECTURE.md](../LANGUAGE_FEATURES/RULE_SYSTEM_ARCHITECTURE.md)** - Rule system implementation
- **[TYPE_SYSTEM/GRAPA_MEMORY_MANAGEMENT_SYSTEM.md](../TYPE_SYSTEM/GRAPA_MEMORY_MANAGEMENT_SYSTEM.md)** - Memory management patterns
