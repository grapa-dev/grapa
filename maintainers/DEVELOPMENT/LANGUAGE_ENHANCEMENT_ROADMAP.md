# Grapa Language Enhancement Roadmap (Internal)

**Last Updated**: July 19, 2024  
**Status**: Active planning document for language development  
**Scope**: All language features, gaps, and enhancement priorities

> **NOTE**: This is the single source of truth for language development planning. All other language planning files are deprecated.

---

## 🎯 **Current Language State**

### **✅ Strengths**
- **Superior Dynamic Code Execution**: `op()(<string>)()` and `$sys().eval()` provide human-readable execution trees
- **Execution Tree Architecture**: $OP objects are both compiled code and inspectable data structures
- **Advanced Meta-Programming**: Direct manipulation of execution trees enables sophisticated code generation
- **Unlimited precision math ($INT, $FLOAT, $TIME)**
- **Runtime-mutable grammar and executable BNF**
- **Advanced Unicode grep/regex**
- **Integrated data processing pipelines**
- **Native JSON/XML round-tripping via eval/.str()**

### **❌ Critical Gaps**
- **No `for`/`foreach` loops** (only `while`)
- **No native exception handling** (`try/catch`)
- **Limited string formatting/interpolation**
- **No module/import system** for code reuse
- **User-defined classes/objects** not fully documented or idiomatic

---

## 🚨 **IMMEDIATE PRIORITIES (Next 2-4 weeks)**

### **1. Operator Bug Fixes** (HIGH PRIORITY - Week 1)
**Status**: Ready to start  
**Estimated Effort**: 1-2 weeks  
**Source**: From operator audit results

#### Critical Bugs to Fix:
1. **STR > INT behavior**: `"hello" > 5` returns TRUE (should be FALSE)
2. **Logical NOT FLOAT**: `!5.0` returns TRUE (should be FALSE)  
3. **Logical NOT complex**: `!(5 && 3)` returns FALSE (should be TRUE)
4. **STR <=> INT**: Returns numbers instead of errors

#### Design Decisions to Confirm:
- Array/List object ID comparison vs content comparison
- Type conversion philosophy across operators

#### Files to Update:
- `source/grapa/GrapaLibRule.cpp` - Fix operator implementations
- `test/operators/` - Add test cases for fixes
- `docs/docs/syntax/operator.md` - Update with fixes

### **2. Unicode Language Binding** (MEDIUM PRIORITY - Week 2)
**Status**: Ready to start  
**Estimated Effort**: 1-2 days  
**Source**: From Unicode enhancement progress

#### Tasks:
1. **Add `case_fold()` method** to `lib/grapa/$OBJ.grc`
2. **Connect to C++ implementation** in `source/grep/grapa_grep_unicode.hpp`
3. **Test Turkish I case folding** from Grapa scripts
4. **Make `upper()` and `lower()` Unicode-aware**

#### Success Criteria:
- `"İstanbul".case_fold()` returns `"istanbul"`
- `"Straße".case_fold()` returns `"strasse"`
- Integration with grep `i` option works correctly

### **3. String Interpolation** (MEDIUM PRIORITY - Week 3)
**Status**: Planning phase  
**Estimated Effort**: 1 week  
**Source**: From language gaps analysis

#### Design Goals:
- Template literal-style interpolation: `"Hello ${name}!"`
- Expression evaluation within strings
- Backward compatibility with existing string operations

#### Implementation Plan:
- Extend string literal parsing in BNF grammar
- Add interpolation evaluation in GrapaLibRule.cpp
- Create comprehensive test suite

---

## 📋 **MEDIUM-TERM ENHANCEMENTS (Next 2-6 months)**

### **4. Loop Constructs** (HIGH IMPACT)
**Status**: Design phase  
**Estimated Effort**: 2-3 weeks  
**Priority**: High (frequently requested)

#### Proposed Features:
- **`for` loop**: `for (i = 0; i < 10; i++) { ... }`
- **`foreach` loop**: `foreach (item in array) { ... }`
- **Range-based loops**: `for (i in 1..10) { ... }`

#### Implementation Strategy:
- Extend BNF grammar for loop syntax
- Add loop control structures to execution trees
- Implement break/continue support

### **5. Exception Handling** (HIGH IMPACT)
**Status**: Design phase  
**Estimated Effort**: 2-3 weeks  
**Priority**: High (error handling improvement)

#### Proposed Features:
- **`try/catch` blocks**: `try { ... } catch (error) { ... }`
- **Exception types**: Built-in exception hierarchy
- **Error propagation**: Automatic error bubbling

#### Implementation Strategy:
- Design exception object structure
- Extend execution tree for exception handling
- Integrate with existing error system

### **6. Module System** (MEDIUM IMPACT)
**Status**: Planning phase  
**Estimated Effort**: 3-4 weeks  
**Priority**: Medium (code organization)

#### Proposed Features:
- **Import/export**: `import "module.grc"`, `export function`
- **Namespace management**: Module-scoped variables
- **Dependency resolution**: Automatic module loading

#### Implementation Strategy:
- Design module file format
- Implement module loader in GrapaLibRule.cpp
- Add namespace support to execution context

---

## 🔍 **OPERATOR AUDIT RESULTS**

### **✅ Completed Audit (July 2024)**
- **Scope**: All 26 operators audited
- **Gaps Identified**: 243 gaps across type combinations
- **Bugs Found**: 5 critical bugs tracked
- **Documentation**: `docs/docs/syntax/operator.md` fully updated

### **Missing Operator Documentation**
Document the 5 operators currently missing from user documentation:

| Operator | Name | Function | Status |
|----------|------|----------|--------|
| `*/` | root | Mathematical root | ❌ Missing |
| `<=>` | cmp | Three-way comparison | ❌ Missing |
| `++` | extend | Array extension | ❌ Missing |
| `--` | remove | Array removal | ❌ Missing |
| `.*` | dot | Dot product | ❌ Missing |

### **Type Support Gaps**
- **Arithmetic operators**: Missing type combinations for STR, ARRAY, WIDGET
- **Assignment operators**: Array assignment behavior (`+=` vs `++=`) not documented
- **Comparison operators**: Type conversion rules unclear
- **Bitwise operators**: Limited to INT/RAW only (except `~` with matrix support)

---

## 🌐 **UNICODE ENHANCEMENT PROGRESS**

### ✅ **Completed**
- **Unicode Case Folding C++ Implementation**: Lookup table-based case folding implemented in `source/grep/grapa_grep_unicode.hpp`
- **Turkish I Support**: İ (U+0130) → i (U+0069), ı (U+0131) → i (U+0069)
- **German Sharp S Support**: ß (U+00DF) → s (U+0073)
- **Greek Final Sigma Support**: ς (U+03C2) → σ (U+03C3)
- **Common Accented Characters**: À→à, Á→á, etc.
- **Composed Form Output**: Produces composed forms instead of decomposed forms

### 🔄 **In Progress**
- **Grapa Language Binding**: Need to add `case_fold()` method to `lib/grapa/$OBJ.grc`
- **Integration Testing**: Verify end-to-end functionality from Grapa scripts

### 📋 **Next Steps**
- Add `case_fold = @<"case_fold",{@<this>}>;` to `lib/grapa/$OBJ.grc`
- Connect to C++ `UnicodeString::case_fold()` implementation
- Test Turkish I case folding from Grapa scripts
- Make existing `upper()` and `lower()` methods Unicode-aware
- Add Unicode normalization methods to Grapa language

---

## 🚫 **LANGUAGE GAPS FROM MIGRATION GUIDES**

### **Core Gaps (True Language Gaps)**
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Meta-programming (alias, undef, defined?)**: Needed for dynamic method aliasing, method removal, and runtime introspection (Ruby, Kotlin, Swift)
- **File/line macros (__FILE__, __LINE__, #file, #line)**: Useful for debugging, error reporting, and meta-programming (Ruby, Swift)
- **Block/yield support (yield, block_given?)**: Enables Ruby-style block passing and advanced functional idioms
- **Ensure/finally/defer (ensure, finally, defer)**: Needed for resource cleanup and exception-safe code (Ruby, Kotlin, Swift)
- **Protocol/interface/extension (protocol, interface, extension)**: Needed for advanced type abstraction and code reuse (Swift, Kotlin)
- **Annotation/meta-programming (@file:JvmName, @objc, @escaping, @Annotation)**: Needed for advanced code generation and interop (Kotlin, Swift)
- **Coroutine/async (coroutineScope)**: Needed for async/concurrent programming (Kotlin)
- **Advanced type features (typealias, Any, generics)**: Needed for type abstraction and generic programming (Swift, Kotlin)
- **Type checking/casting (is, as, as?)**: Needed for safe type operations (Kotlin, Swift)
- **Guard statement (guard)**: Needed for early exit and error handling (Swift)

### **Common Stumbling Blocks**
*These gaps are common stumbling blocks for new users coming from Python, JavaScript, or similar languages:*

- **No `.get()` or `.set()` methods on lists/arrays**: Must use `[]` for access and assignment
- **No `.match()` method on strings**: Use `.grep()` for regex/pattern matching
- **No `//` line comments**: Only block comments (`/* ... */`) are supported
- **No `.append()` or `.push()` on arrays/lists**: Use `+=` for appending
- **No attribute-style access for list/dict keys**: Must use `[]` or explicit key lookup
- **No automatic truthy/falsy conversion**: Explicit boolean checks are required
- **No implicit variable declaration**: All variables must be explicitly declared/assigned
- **No built-in `len()` function**: Use `.len()` method on arrays/lists/strings
- **No built-in `map()`/`filter()`/`reduce()` as global functions**: These are methods on arrays/lists
- **No nested block comments**: Grapa does not support nested comments

---

## 🎯 **ENHANCEMENT PRIORITIZATION**

### **High Priority (Next 3 months)**
1. **Operator Bug Fixes** - Fix critical operator behavior issues
2. **Unicode Language Binding** - Complete Unicode implementation
3. **String Interpolation** - Improve string manipulation capabilities
4. **Loop Constructs** - Add `for` and `foreach` loops

### **Medium Priority (Next 6 months)**
1. **Exception Handling** - Add `try/catch` support
2. **Module System** - Add import/export capabilities
3. **Enhanced OOP Support** - Improve class/object documentation and features
4. **Standard Library** - Add math, stats, date/time, CSV utilities

### **Low Priority (Future)**
1. **Advanced Type Features** - Generics, type aliases, type checking
2. **Async/Concurrent Programming** - Coroutines, async/await
3. **Advanced Meta-programming** - Annotations, reflection
4. **Package Manager** - Dependency management ecosystem

---

## 📊 **IMPLEMENTATION ROADMAP**

### **Phase 1: Bug Fixes & Stabilization (Weeks 1-4)**
- [ ] Fix operator bugs (STR > INT, Logical NOT, etc.)
- [ ] Complete Unicode language binding
- [ ] Stabilize cryptographic features
- [ ] Implement performance optimizations

### **Phase 2: Core Language Features (Months 2-4)**
- [ ] Add string interpolation
- [ ] Implement loop constructs
- [ ] Design exception handling system
- [ ] Enhance OOP documentation

### **Phase 3: Advanced Features (Months 5-8)**
- [ ] Implement module system
- [ ] Add standard library components
- [ ] Enhance type system
- [ ] Improve meta-programming capabilities

### **Phase 4: Ecosystem Development (Months 9-12)**
- [ ] Develop package manager
- [ ] Create development tools
- [ ] Build community resources
- [ ] Establish best practices

---

## 🔧 **IMPLEMENTATION GUIDELINES**

### **Design Principles**
- **Backward Compatibility**: New features must not break existing code
- **Consistency**: Follow existing Grapa patterns and conventions
- **Performance**: Maintain Grapa's performance characteristics
- **Simplicity**: Prefer simple, clear implementations over complex features

### **Development Process**
1. **Design Phase**: Document feature specification and design decisions
2. **Implementation Phase**: Implement in C++ backend and Grapa language layer
3. **Testing Phase**: Create comprehensive test suite
4. **Documentation Phase**: Update user and maintainer documentation
5. **Integration Phase**: Ensure compatibility with existing features

### **Quality Standards**
- **Test Coverage**: All new features must have comprehensive tests
- **Documentation**: User-facing documentation must be clear and complete
- **Performance**: New features must not significantly impact performance
- **Error Handling**: Robust error handling for all new features

---

## 📞 **COORDINATION**

This document should be updated whenever:
- New language gaps are identified
- Enhancement priorities change
- Implementation progress is made
- Design decisions are finalized

**Maintainers**: Update this file to reflect current language development status and planning.

---

**Last Updated**: July 19, 2024  
**Current Focus**: Operator Bug Fixes  
**Next Milestone**: Unicode Language Binding 