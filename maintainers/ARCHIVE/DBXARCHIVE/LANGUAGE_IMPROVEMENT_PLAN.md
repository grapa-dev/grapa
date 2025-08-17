# Grapa Language Improvement Plan

This document outlines a comprehensive plan to make the Grapa language more programmer friendly and increase adoption. The goal is to audit the current language, summarize migration findings, identify pain points, and propose concrete improvements for syntax, usability, and onboarding.

---

## 📊 Current Language Audit

### Comment Handling
- **Current**: Only block comments (`/* ... */`) are supported
- **Missing**: Line comments (`//`, `#`) are not supported
- **Restrictions**: Comments not allowed inside class definitions, parameter lists, or array/object literals
- **BNF Reference**: `$starcomment` rule: `('/*') <> ('*/')`
- **Supported Locations**: Between commands, after commands, before commands, standalone lines
- **Unsupported Locations**: Inside class definitions, parameter lists, array/object literals
- **Reference**: See `maintainers/DEVELOPMENT/COMMENT_HANDLING_GUIDE.md` for full details

### Loop Syntax
- **Current**: No `for`/`foreach`/`range` loops
- **Alternative**: Use `while`, `.range()`, `.map()`, `.filter()`, `.reduce()`
- **Example**: `i = 0; while (i < arr.len()) { ...; i += 1; }` or `(n).range(0,1).map(op(i) { ... })`

### Error Handling
- **Current**: No `try/catch` mechanism
- **Alternative**: Use `.iferr()` for fallback or check for `$ERR` type
- **Example**: `result = some_operation().iferr(0);`

### Module/Import System
- **Current**: No formal module/import system
- **Alternative**: Use `$file()` for file operations and `$sys().eval()` for dynamic code execution

### Syntax Ergonomics
- **String Interpolation**: No string interpolation (use concatenation: `"Hello " + name`)
- **Property Access**: No attribute-style access for dict/list keys (use `[]` or dot notation for $LIST/$OBJ)
- **Semicolons**: All statements and blocks must end with a semicolon (`;`)
- **Boolean Logic**: No implicit truthy/falsy; use explicit boolean checks
- **Regular Expressions**: No regex as first-class objects (use `.grep()` for pattern matching)

---

## 🌍 Migration Analysis

### Python Migration
- **Loops**: No `for`/`foreach` loops; use `while` or `.range()`+functional methods
- **Error Handling**: No `try/catch`; use `.iferr()`
- **Comments**: Only block comments, always on their own line
- **Property Access**: `.get()` only for `$file`/`$TABLE`
- **String Operations**: No string interpolation; use concatenation
- **Assessment**: Many Python features available via different mechanisms

### JavaScript Migration
- **Loops**: No `for`/`foreach` loops; use `while` or `.range()`+functional methods
- **Error Handling**: No `try/catch`; use `.iferr()`
- **Comments**: Only block comments, always on their own line
- **Property Access**: `.get()` only for `$file`/`$TABLE`
- **String Operations**: No template literals; use concatenation
- **Advanced Features**: No destructuring, spread, or optional chaining

### Go Migration
- **Loops**: No `for`/`range` loops; use `while` or `.range()`+functional methods
- **Comments**: Only block comments, always on their own line
- **Property Access**: `.get()` only for `$file`/`$TABLE`
- **Type System**: No static typing; dynamic typing with runtime checks
- **Modules**: No package/import system; use file system and dynamic eval

---

## 🚨 Known Pain Points & User Feedback

### High Priority Issues
1. **Lack of line comments** (`//`, `#`)
2. **No `for`/`foreach`/`range` loop syntax** - functional style unfamiliar to many
3. **No string interpolation or template literals**
4. **No module/import system** for code reuse
5. **Error handling is functional**, not exception-based

### Medium Priority Issues
6. **Some migration idioms require more verbose code**
7. **Onboarding could be improved** with more real-world examples
8. **IDE/tooling support** needs improvement

---

## 🎯 Proposed Improvements

### Phase 1: High Impact, Low Effort
1. **Line Comments**: Add support for `//` and `#` line comments
2. **String Interpolation**: Add template literal support
3. **Loop Syntax**: Add `for` loop as syntactic sugar for `.range()` patterns
4. **Error Handling**: Improve `.iferr()` ergonomics

### Phase 2: Medium Impact, Medium Effort
5. **Module System**: Design basic import/include system
6. **Property Access**: Improve attribute-style access for objects
7. **Boolean Logic**: Add implicit truthy/falsy support
8. **IDE Integration**: Improve error messages and syntax highlighting

### Phase 3: Advanced Features
9. **Regular Expressions**: Add first-class regex support
10. **Advanced Loops**: Add `foreach` and `range` constructs
11. **Exception Handling**: Add `try/catch`-like constructs
12. **Type System**: Consider optional static typing

## 🚨 **IMMEDIATE PRIORITIES**

### **1. Loop Constructs** (HIGH IMPACT)
**Status**: Design phase  
**Estimated Effort**: 2-3 weeks  
**Priority**: High (frequently requested)

#### **Proposed Features**
- **`for` loop**: `for (i = 0; i < 10; i++) { ... }`
- **`foreach` loop**: `foreach (item in array) { ... }`
- **Range-based loops**: `for (i in 1..10) { ... }`

#### **Implementation Strategy**
1. **Extend BNF grammar** for loop syntax
2. **Add loop control structures** to execution trees
3. **Implement break/continue support**
4. **Create comprehensive test suite**

### **2. Exception Handling** (HIGH IMPACT)
**Status**: Design phase  
**Estimated Effort**: 2-3 weeks  
**Priority**: High (error handling improvement)

#### **Proposed Features**
- **`try/catch` blocks**: `try { ... } catch (error) { ... }`
- **Exception types**: Built-in exception hierarchy
- **Error propagation**: Automatic error bubbling

#### **Implementation Strategy**
1. **Design exception object structure**
2. **Extend execution tree** for exception handling
3. **Integrate with existing error system**
4. **Create exception hierarchy**

### **3. Module System** (MEDIUM IMPACT)
**Status**: Planning phase  
**Estimated Effort**: 3-4 weeks  
**Priority**: Medium (code organization)

#### **Proposed Features**
- **Import/export**: `import "module.grc"`, `export function`
- **Namespace management**: Module-scoped variables
- **Dependency resolution**: Automatic module loading

#### **Implementation Strategy**
1. **Design module file format**
2. **Implement module loader** in GrapaLibRule.cpp
3. **Add namespace support** to execution context
4. **Create module dependency resolver**

---

## 📚 Adoption Strategy

### Immediate Actions
1. **Prioritize improvements** that lower the learning curve for new users
2. **Maintain migration guides** and keep them up to date
3. **Solicit user feedback** via issues/PRs
4. **Document new features** in canonical syntax guide
5. **Provide clear upgrade paths** for breaking changes

### Documentation Standards
- **Canonical Reference**: `docs-src/docs/syntax/basic_syntax.md` is the authoritative source
- **Empirical Testing**: All code samples must be tested as `.grc` scripts
- **Cross-Platform**: Ensure all examples work on Windows, Mac, and Linux
- **Migration Guides**: Keep all language migration guides updated

### Maintenance Policy
Any change to the Grapa language requires:
1. **Immediate update** of `docs-src/docs/syntax/basic_syntax.md`
2. **Full scan** of all `.md` files in `docs-src` for code samples
3. **Empirical testing** of all `.grc` code samples
4. **Migration guide updates** if new features address pain points
5. **Cross-linking** all relevant documentation

---

## 🔄 Implementation Roadmap

### Short Term (Next 3 months)
- [ ] **Line Comments**: Implement `//` and `#` support
- [ ] **String Interpolation**: Add template literal syntax
- [ ] **Basic For Loops**: Add `for` loop construct
- [ ] **Error Handling**: Improve `.iferr()` usability

### Medium Term (3-6 months)
- [ ] **Module System**: Design and implement import/include
- [ ] **Property Access**: Improve object property access
- [ ] **Boolean Logic**: Add implicit truthy/falsy
- [ ] **IDE Support**: Improve error messages and highlighting

### Long Term (6+ months)
- [ ] **Regular Expressions**: First-class regex support
- [ ] **Advanced Loops**: `foreach` and `range` constructs
- [ ] **Exception Handling**: `try/catch`-like constructs
- [ ] **Type System**: Optional static typing

---

## 📋 Success Metrics

### User Experience
- **Reduced learning curve** for Python/JS/Go developers
- **Improved onboarding** completion rates
- **Better error messages** and debugging experience
- **More intuitive syntax** for common operations

### Technical Metrics
- **Backward compatibility** maintained
- **Cross-platform support** preserved
- **Performance impact** minimized
- **Documentation coverage** complete

---

## 🔗 Related Documentation

- **Canonical Syntax**: `docs-src/docs/syntax/basic_syntax.md`
- **Migration Guides**: `docs-src/docs/migrations/`
- **Comment Handling**: `maintainers/DEVELOPMENT/COMMENT_HANDLING_GUIDE.md`
- **Onboarding**: `maintainers/DEVELOPMENT/ONBOARD.md`
- **Current Status**: `maintainers/DEVELOPMENT/CURRENT_STATUS.md`

---

*Last Updated: December 2024*
*This plan was developed based on analysis during the GrapaDBX investigation and user feedback.* 