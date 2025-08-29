# Grapa Language Enhancement Roadmap (Internal) - DEPRECATED

**Last Updated**: December 2024  
**Status**: DEPRECATED - Content consolidated into CURRENT_STATUS.md  
**Scope**: All language features, gaps, and enhancement priorities

> **⚠️ DEPRECATED (December 2024):** This document is **DEPRECATED** and no longer maintained.
>
> **📋 CURRENT STATUS:** All language enhancement information has been consolidated into **[CURRENT_STATUS.md](CURRENT_STATUS.md)**.
>
> **🔗 GO TO:** [CURRENT_STATUS.md](CURRENT_STATUS.md) for current language development priorities and status.

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
- **Complete Operator System**: All 26 operators fully functional with comprehensive type support

### **❌ Critical Gaps**
- **No `for`/`foreach` loops** (only `while`)
- **No native exception handling** (`try/catch`)
- **Limited string formatting/interpolation**
- **No module/import system** for code reuse
- **User-defined classes/objects** not fully documented or idiomatic

---

## 🚨 **IMMEDIATE PRIORITIES (Next 2-4 weeks)**

### **1. Missing Operator Documentation** (HIGH PRIORITY - Week 1) ✅ **COMPLETED**
**Status**: Completed December 2024  
**Estimated Effort**: 2-3 days  
**Source**: From operator audit results

#### Completed Documentation:
1. **`*/` (root)** - Mathematical root operation ✅
2. **`<=>` (cmp)** - Three-way comparison operator ✅  
3. **`++` (extend)** - Array extension ✅
4. **`--` (remove)** - Array removal ✅
5. **`.*` (dot)** - Dot product ✅

#### Files Updated:
- `docs/docs/syntax/operator.md` - Added comprehensive operator documentation ✅
- Type Support Matrix - Updated to include all 5 operators ✅
- Examples and Error Cases - Documented for all operators ✅

### **2. CLI Executable Enhancement** (HIGH PRIORITY - Week 2)
**Status**: Ready to start  
**Estimated Effort**: 3-5 days  
**Source**: User experience improvement priority

#### Tasks:
1. **Evaluate current main.cpp implementation** for Windows vs non-Windows parameter handling
2. **Add standard CLI options** (help, version, file input, pipe support) - ✅ COMPLETED
3. **Implement pipe support** for Linux/Mac command line workflows
4. **Add script execution modes** (file, stdin, interactive)
5. **Update build scripts** to ensure source/main.cpp is used consistently
6. **Update CLI Quickstart documentation** with new capabilities

#### Success Criteria:
- `grapa --help` shows comprehensive usage information
- `grapa script.grc` executes script files
- `echo "print('hello')" | grapa` works on Linux/Mac
- `grapa -i` starts interactive mode
- `grapa -v` shows version information
- Windows and non-Windows platforms handled appropriately

### **3. Unicode Language Binding** (MEDIUM PRIORITY - Week 3)
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
- **Bugs Found**: 5 critical bugs tracked and RESOLVED ✅
- **Documentation**: `docs/docs/syntax/operator.md` fully updated
- **Implementation**: All operators fully functional and tested ✅

### **✅ Operator System Status (December 2024)**
- **Status**: All critical operator bugs RESOLVED ✅
- **Testing**: Live testing confirms all operators working correctly
- **Documentation**: Implementation complete, missing docs identified
- **Reference**: `maintainers/INTERNAL_NOTES/OPERATOR_BUG_FIXES_STATUS.md`

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

- **No `.getfield()` or `.setfield()` methods on lists/arrays**: Must use `[]` for access and assignment
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
1. **Missing Operator Documentation** - Complete operator documentation
2. **Unicode Language Binding** - Complete Unicode implementation
3. **String Interpolation** - Improve string manipulation capabilities
4. **Loop Constructs** - Add `for` and `foreach` loops

### **Medium Priority (Next 6 months)**
1. **Exception Handling** - Add `try/catch` support
2. **Module System** - Add import/export capabilities
3. **Enhanced OOP Support** - Improve class/object documentation and features
4. **Standard Library** - Add math, stats, date/time, CSV utilities

### **Low Priority (Future)**
1. **Performance Optimizations** - Operator and runtime optimizations
2. **Advanced Type System** - Generics, type aliases, type checking
3. **Async/Concurrency** - Coroutines, async/await patterns
4. **Meta-programming** - Runtime code generation and manipulation

---

## 📊 **IMPLEMENTATION STATUS**

### **✅ Completed Features**
- **Operator System**: All 26 operators fully functional
- **Unicode Support**: Comprehensive Unicode grep/regex
- **Database Integration**: Native database operations
- **File System**: Complete file system operations
- **Network Operations**: HTTP, TCP, UDP support
- **Mathematical Functions**: Unlimited precision math
- **String Manipulation**: Advanced string operations
- **JSON/XML**: Native round-tripping support

### **🔄 In Progress**
- **Unicode Language Binding**: C++ implementation complete, language binding needed
- **Documentation**: Missing operator documentation

### **📋 Planned**
- **String Interpolation**: Template literal-style strings
- **Loop Constructs**: For and foreach loops
- **Exception Handling**: Try/catch blocks
- **Module System**: Import/export capabilities

### **🔮 Future**
- **Performance Optimizations**: Runtime and operator optimizations
- **Advanced Type System**: Generics and type checking
- **Async Programming**: Coroutines and async patterns
- **Meta-programming**: Runtime code generation

---

## 🎯 **SUCCESS METRICS**

### **Short-term Goals (3 months)**
- [ ] All 5 missing operators documented
- [ ] Unicode case folding available in Grapa language
- [ ] String interpolation implemented
- [ ] Loop constructs designed and implemented

### **Medium-term Goals (6 months)**
- [ ] Exception handling system implemented
- [ ] Module system designed and implemented
- [ ] Enhanced OOP documentation and features
- [ ] Standard library utilities added

### **Long-term Goals (12 months)**
- [ ] Performance optimizations implemented
- [ ] Advanced type system features added
- [ ] Async programming support added
- [ ] Meta-programming capabilities enhanced

---

**Last Updated**: December 2024  
**Current Focus**: Missing Operator Documentation  
**Status**: Operator System Complete, Language Enhancements in Progress 