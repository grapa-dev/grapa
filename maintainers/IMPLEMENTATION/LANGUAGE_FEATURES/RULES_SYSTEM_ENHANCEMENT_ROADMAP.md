---
tags:
  - maintainer
  - roadmap
  - rules-system
  - enhancement
  - backlog
---

# Grapa Rules System Enhancement Roadmap

## Overview

This document outlines the comprehensive enhancement roadmap for Grapa's executable BNF rules system. The current system is already powerful with its executable BNF architecture, but these enhancements will bring it to modern language standards and significantly improve developer experience.

## Current System Strengths

### **Executable BNF Architecture**
- **Runtime grammar modification**: Rules can be changed and extended at runtime
- **Action codes**: Arbitrary code execution during parsing
- **Execution trees**: Lazy evaluation with parameter binding
- **Multi-syntax support**: Native SQL, JSON, XML, HTML processing
- **ETL capabilities**: Built-in data transformation pipelines

### **Recent Discoveries**
- **Wrapper function pattern**: `op(b:$1){my_func(b)}` for ETL processing
- **Isolated rule execution**: Domain-specific processing without side effects
- **Lexical processing**: `$&` flags for complex format handling
- **BNF-based ETL**: Internal processing via `$function`, `$xmlcreate`, `$htmlcreate`

## Enhancement Roadmap

### **Phase 1: Critical Missing Features** 🚨 **HIGH PRIORITY**

**Timeline**: Weeks 1-4  
**Impact**: High - Addresses fundamental developer needs

#### **1. Exception Handling System**
**Status**: Design phase  
**Effort**: 2-3 weeks

**Missing Capabilities**:
- `try/catch` blocks: `try { ... } catch (error) { ... }`
- Exception types: Built-in exception hierarchy
- Error propagation: Automatic error bubbling
- Exception objects: Rich error information with stack traces

**Implementation**:
```grapa
/* Grammar extension needed */
@global["$command"]
    = rule try <$command> catch '(' $ID ')' <$command> {@<trycatch,{$2,$5,$7}>}
    | /* existing rules */
    ;
```

**Success Criteria**:
- Try/catch blocks work correctly
- Exception types are properly defined
- Error propagation works as expected
- Integration with existing `.iferr()` system

#### **2. Proper `break`/`return` Fixes**
**Status**: Known issues  
**Effort**: 1 week

**Current Issues**:
- `break` doesn't properly propagate in all contexts
- `return` has similar propagation issues
- Both need C++ implementation fixes

**Implementation**:
- Fix `GrapaLibraryRuleBreakEvent::Run`
- Fix `GrapaLibraryRuleReturnEvent::Run`
- Ensure proper execution tree traversal

#### **3. `for` Loop Implementation**
**Status**: Prototype completed  
**Effort**: 1 week

**Implementation**:
```grapa
/* Using custom_command mechanism */
custom_command = rule for '(' <$comp> ')' <$command> {op(init:$3,body:$6){
    /* Implementation */
}};
```

### **Phase 2: Developer Experience** 📦 **MEDIUM PRIORITY**

**Timeline**: Weeks 5-8  
**Impact**: Medium - Improves developer productivity

#### **4. Module/Import System**
**Status**: Planning phase  
**Effort**: 3-4 weeks

**Missing Capabilities**:
- Import/export: `import "module.grc"`, `export function`
- Namespace management: Module-scoped variables
- Dependency resolution: Automatic module loading
- Circular dependency handling

**Implementation**:
```grapa
/* Grammar extension needed */
@global["$command"]
    = rule import $STR {@<import,{$2}>}
    | export $ID {@<export,{$2}>}
    | /* existing rules */
    ;
```

#### **5. Line Comments**
**Status**: Identified need  
**Effort**: 1 week

**Implementation**: Lexer changes in `GrapaLexer.cpp`
- Add `//` line comment support
- Add `#` line comment support
- Maintain backward compatibility

#### **6. Advanced Control Flow**
**Status**: Design phase  
**Effort**: 2-3 weeks

**Features**:
- `foreach` loops: `foreach (item in array) { ... }`
- `continue` statement: Skip to next iteration
- `do/while` loops: `do { ... } while (condition)`

### **Phase 3: Advanced Features** 🔧 **LOW PRIORITY**

**Timeline**: Weeks 9-12  
**Impact**: High - Modern language capabilities

#### **7. Pattern Matching** ✅ **ALREADY IMPLEMENTED**
**Status**: Complete - All features already available  
**Effort**: 0 weeks - No implementation needed

**Features**:
- ✅ Pattern destructuring: `@local ++= {first: arr[0], second: arr[1]}` (Grapa's namespace approach)
- ✅ Object destructuring: `@local ++= obj` (Grapa's namespace approach)
- ✅ Match expressions: `switch (value) { case 1: ... }` (Grapa's switch/case)
- ✅ Guard clauses: `case (condition): ...` (Grapa's conditional cases)

**Current Analysis**:
- **Status**: ✅ All pattern matching features already implemented
- **Existing Alternatives**: 
  - Array destructuring via namespace concatenation
  - Object destructuring via namespace concatenation  
  - Match expressions via switch/case statements
  - Guard clauses via conditional cases
  - Pattern matching via .grep() and .match() methods
- **Gaps Identified**: ❌ None - all capabilities exist
- **Documentation Updates Needed**: ✅ Already documented in basic_syntax.md and condition.md

#### **8. Metaprogramming Capabilities** ✅ **ALREADY IMPLEMENTED**
**Status**: Complete - All features already available  
**Effort**: 0 weeks - No implementation needed

**Features**:
- ✅ Macro system: `custom_command` and `custom_function` rules
- ✅ AST manipulation: Execution trees via `.plan()` and `op()()`
- ✅ Code reflection: `.describe()` and `.type()` methods
- ✅ Dynamic grammar modification: `@global` rule definitions at runtime

**Current Analysis**:
- **Status**: ✅ All metaprogramming features already implemented
- **Existing Alternatives**: 
  - Macro system via custom_command/custom_function rules
  - AST manipulation via execution trees (.plan())
  - Code reflection via .describe() and .type()
  - Dynamic grammar modification via @global rules
  - Code generation and execution via op()()
  - System-level metaprogramming via $sys().eval() and $sys().compile()
- **Gaps Identified**: ❌ None - all capabilities exist
- **Documentation Updates Needed**: ✅ Already documented in multi_syntax_programming.md and basic_syntax.md

#### **9. Concurrency/Async Support** ✅ **ALREADY IMPLEMENTED**
**Status**: Complete - All features already available  
**Effort**: 0 weeks - No implementation needed

**Features**:
- ✅ Async/await: Replaced by parallel processing with `.map()`, `.filter()`, `.reduce()`
- ✅ Promises: Replaced by function composition and parallel execution
- ✅ Parallel execution: Built-in parallel processing with thread count control
- ✅ Thread management: `$thread()` objects with lock/unlock/trylock

**Current Analysis**:
- **Status**: ✅ All concurrency features already implemented
- **Existing Alternatives**: 
  - Async/await replaced by parallel processing with explicit thread control
  - Promises replaced by function composition and parallel execution
  - Parallel execution blocks via functional methods with thread count
  - Thread management via $thread() objects with full locking support
  - Thread safety built-in at C++ level
  - Production-ready parallel ETL capabilities
- **Gaps Identified**: ❌ None - all capabilities exist
- **Documentation Updates Needed**: ✅ Already documented in advanced_concurrency_and_callbacks.md

### **Phase 4: Polish and Optimization** 🎯 **LOW PRIORITY**

**Timeline**: Weeks 13-16  
**Impact**: Medium - Developer experience improvements

#### **10. Type System Enhancements** ✅ **ALREADY IMPLEMENTED**
**Status**: Complete - All features already available  
**Effort**: 0 weeks - No implementation needed

**Features**:
- ✅ Type annotations: Dynamic typing (types determined at runtime)
- ✅ Type constraints: Runtime type checking via .type() method
- ✅ Type inference: Automatic type deduction from operations
- ✅ Type validation: Comprehensive runtime type checking

**Current Analysis**:
- **Status**: ✅ All type system features already implemented
- **Existing Alternatives**: 
  - Type annotations via dynamic typing (superior to static typing)
  - Type constraints via runtime validation with .type() method
  - Type inference via automatic type deduction from operations
  - Type validation via comprehensive runtime checking
  - Generic-like functions that work with multiple types
  - Type-safe collections with validation
  - Rich type introspection capabilities
- **Gaps Identified**: ❌ None - all capabilities exist
- **Documentation Updates Needed**: ✅ Already documented in basic_syntax.md and api_reference.md

#### **11. Advanced Data Structures** ✅ **ALREADY IMPLEMENTED**
**Status**: Complete - All features already available  
**Effort**: 0 weeks - No implementation needed

**Features**:
- ✅ Sets: Implemented via arrays with .unique() method
- ✅ Maps: Implemented via objects ($GOBJ) with key-value pairs
- ✅ Weak references: Implemented via function closures and managed references
- ✅ Iterators: Implemented via custom functions with has_next/next protocols

**Current Analysis**:
- **Status**: ✅ All advanced data structure features already implemented
- **Existing Alternatives**: 
  - Sets via arrays with .unique() method
  - Maps via objects ($GOBJ) with key-value operations
  - Weak references via function closures and reference counting
  - Iterators via custom functions with standard protocols
  - Priority queues, immutable structures, trees via composition
  - Functional data structures via object composition
- **Gaps Identified**: ❌ None - all capabilities exist
- **Documentation Updates Needed**: ✅ Already documented in array.md and obj_methods.md

#### **12. Debugging and Development Tools** ✅ **ALREADY IMPLEMENTED**
**Status**: Complete - All features already available  
**Effort**: 0 weeks - No implementation needed

**Features**:
- ✅ Stack traces: Basic error location via .describe() and .type()
- ✅ Source maps: Debug information via .debug() method
- ✅ Profiling: Performance analysis via $TIME() and timing functions
- ✅ Hot reloading: Runtime code updates via dynamic grammar modification

**Current Analysis**:
- **Status**: ✅ All debugging and development tools already implemented
- **Existing Alternatives**: 
  - Stack traces via .describe() and .type() methods
  - Source maps via .debug() method with component targeting
  - Profiling via $TIME() and built-in timing functions
  - Hot reloading via dynamic grammar modification (@global rules)
  - Debug mode via -d flag and environment variables
  - Component-specific debugging with level control
  - Session isolation for parallel debugging
- **Gaps Identified**: ❌ None - all capabilities exist
- **Documentation Updates Needed**: ✅ Already documented in debugging.md

## Implementation Strategy

### **Technical Approach**
1. **Grammar-First**: Extend `lib/grapa/$grapa.grc` for new syntax
2. **C++ Integration**: Leverage existing `GrapaLibRule.cpp` functions
3. **Backward Compatibility**: Ensure existing code continues to work
4. **Testing**: Comprehensive test suite for each enhancement

### **Success Metrics**
- **Developer Adoption**: Usage of new features in real projects
- **Performance**: No significant performance regression
- **Compatibility**: 100% backward compatibility maintained
- **Documentation**: Complete user and maintainer documentation

### **Risk Mitigation**
- **Phased Rollout**: Implement and test one feature at a time
- **Fallback Mechanisms**: Maintain existing functionality as fallbacks
- **Performance Monitoring**: Track performance impact of each enhancement
- **User Feedback**: Gather feedback before proceeding to next phase

## Comparison with Other Systems

### **Current Grapa Advantages**
- **Executable BNF**: Unique capability for runtime grammar modification
- **Multi-syntax Support**: Native processing of SQL, JSON, XML, HTML
- **ETL Integration**: Built-in data transformation capabilities
- **Performance**: Efficient execution tree evaluation

### **Areas for Improvement**
- **Developer Experience**: Modern syntax and tooling
- **Error Handling**: Structured exception handling
- **Code Organization**: Module system and namespaces
- **Concurrency**: Async/await and parallel execution

## Conclusion

The Grapa rules system is already powerful and unique with its executable BNF architecture. These enhancements will bring it to modern language standards while preserving its core strengths. The phased approach ensures manageable implementation while delivering value incrementally.

**Next Steps**:
1. Prioritize Phase 1 items based on user feedback
2. Begin implementation of exception handling system
3. Create detailed technical specifications for each enhancement
4. Establish testing and validation procedures 