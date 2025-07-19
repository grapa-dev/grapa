# Grapa & GrapaPy Development Backlog (Internal)

> **NOTE (July 2024):** This file tracks items **planned but not committed to work on in the near term** (next 2-12 months). For items committed to work on soon, see `TODO.md`.

**PURPOSE**: This file contains items we want to get to eventually but are not committing to work on in the next 2-4 weeks. Items move from here to `TODO.md` when we commit to working on them.

---

## 🎯 **MEDIUM-TERM ITEMS (Next 2-6 months)**

### 1. **Advanced Mathematical Features Documentation**
- **Description**: Document sophisticated mathematical capabilities discovered in operator audit
- **Scope**: Matrix operations, vector operations, advanced linear algebra, cryptographic features
- **Audit Findings**: Hidden mathematical sophistication in operators
- **Features to Document**:
  - **Matrix Operations**: Multiplication (`*`), inversion (`~`), vector operations (`*/`)
  - **Cryptographic Features**: Modular arithmetic with modpow/modinv in modulo (`%`)
  - **Root Operator (`*/`)**: nth root calculations with advanced mathematical support
  - **Bitwise NOT (`~`)**: Matrix inversion support beyond simple bitwise operations
- **Documentation Needs**:
  - Create dedicated mathematical operations documentation
  - Add to USE_CASES.md Section 3 (High-Precision Math & Scientific Computing)
  - Document matrix/vector/tuple operations
  - Explain operator mathematical relationships
- **Benefits**:
  - Showcases Grapa's advanced mathematical capabilities
  - Provides documentation for scientific computing users
  - Reveals hidden features that users may not discover
  - Demonstrates sophisticated implementation quality
- **Effort**: Medium (2-3 weeks)
- **Priority**: Medium (documentation enhancement)
- **Status**: 🟡 PLANNED

### 2. **Type Conversion Philosophy Documentation**
- **Description**: Document and clarify type conversion design philosophy across operators
- **Scope**: Inconsistent type conversion behavior, design decisions, user guidance
- **Audit Findings**: 48 type conversion gaps identified
- **Issues to Address**:
  - **Inconsistent Conversion**: Addition converts types, comparison operators don't
  - **Mathematical Purity**: Some operators intentionally don't convert for mathematical correctness
  - **User Expectations**: Users expect consistent behavior across operators
  - **Design Philosophy**: Balance between mathematical purity and user convenience
- **Documentation Needs**:
  - Create type conversion rules documentation
  - Explain design philosophy and trade-offs
  - Provide guidance on when to expect type conversion
  - Document intentional design decisions vs implementation gaps
- **Benefits**:
  - Clarifies operator behavior for users
  - Documents intentional design decisions
  - Provides guidance for future operator development
  - Reduces user confusion about type conversion
- **Effort**: Low (1-2 weeks)
- **Priority**: Low (documentation clarification)
- **Status**: 🟡 PLANNED

### 3. **Operator Performance and Best Practices Guide**
- **Description**: Create comprehensive guide for operator performance and best practices
- **Scope**: Performance characteristics, optimization tips, common pitfalls
- **Audit Findings**: Performance considerations in operator design
- **Topics to Cover**:
  - **Object ID Comparison**: Performance benefits of object ID vs content comparison
  - **Short-circuit Evaluation**: Logical operators and performance implications
  - **Type Conversion Costs**: Performance impact of type conversion
  - **Matrix Operations**: Performance characteristics of advanced mathematical features
  - **Error Handling**: Performance implications of error propagation
- **Documentation Needs**:
  - Create performance guide for operators
  - Document best practices for each operator category
  - Provide optimization tips and common pitfalls
  - Include performance benchmarks where relevant
- **Benefits**:
  - Helps users write efficient code
  - Documents performance characteristics
  - Provides optimization guidance
  - Improves overall code quality
- **Effort**: Medium (2-3 weeks)
- **Priority**: Low (performance documentation)
- **Status**: 🟡 PLANNED

### 4. **Language Feature Enhancements**
- **Description**: Add missing language features for modern programming
- **Scope**: `for`/`foreach` loops, exception handling, string interpolation, modules
- **Benefits**: Modern language features, improved developer experience
- **Potential Features**:
  - `for`/`foreach` loops (currently only `while`)
  - Exception handling (`try/catch`)
  - String interpolation/formatting
  - Module/import system
  - List comprehensions
- **Effort**: 2-4 weeks
- **Priority**: Low (enhancement)
- **Status**: 🟡 PLANNED
- **Reference**: `maintainers/DEVELOPMENT/grapa_language_enhancement_roadmap.md`

---

## 🔍 **INVESTIGATION ITEMS (No Timeline)**

### 5. **$KEY Field Datatype Investigation**
- **Description**: Investigate allowing any datatype for the $KEY field instead of hardcoded string type
- **Current Workaround**: Use .raw().int() or .raw().float() for numeric keys
- **Questions**:
  - Is this a design limitation or implementation constraint?
  - What would be the performance impact of supporting other types?
  - Are there use cases that would benefit from numeric keys?
- **Status**: Pending investigation
- **Priority**: Low

### 6. **String Splitting Functionality Investigation**
- **Description**: Investigate commented-out string splitting code in division operator
- **Source**: `GrapaLibraryRuleDivEvent::Run` (commented code)
- **Questions**:
  - Was this intentionally disabled or incomplete?
  - Should string splitting be a separate operator or method?
  - What would be the expected behavior?
- **Status**: Pending investigation
- **Priority**: Low

### 7. **Array/List Comparison Behavior Confirmation**
- **Description**: Confirm design decision on array/list comparison behavior
- **Current Behavior**: `[1,2,3] == [1,2,4]` returns TRUE (object ID comparison)
- **Expected Behavior**: Should return FALSE (content comparison)
- **Questions**:
  - Is object ID comparison intentional for performance?
  - Should we have both object ID and content comparison operators?
  - What do users expect from array comparison?
- **Status**: Pending design decision
- **Priority**: Low

---

## 📚 **LONG-TERM ENHANCEMENTS (6+ months)**

### 8. **Finite Field Educational Modules**
- **Description**: Comprehensive educational modules for teaching finite field arithmetic and cryptography
- **Scope**: Interactive learning tools, visualization, and practical applications
- **Documentation**: See `finite_field_educational_modules.md` for detailed specification
- **Benefits**: 
  - Enhances Section 7 (Education & Prototyping) in USE_CASES
  - Provides unique educational value for mathematics and cryptography
  - Expands target audience to educators and students
  - Demonstrates Grapa's capabilities for mathematical education
- **Prerequisites**: Working `modinv()`, `modpow()`, and basic arithmetic (✅ Already available)
- **Effort**: Medium (6-10 months across 4 phases)
- **Priority**: Low (educational enhancement, not critical functionality)
- **Status**: 🟡 PLANNED

### 9. **Operator Implementation Quirks Documentation**
- **Description**: Document and explain operator implementation quirks and edge cases
- **Scope**: Ternary operator forms, bitwise operator strictness, experimental features
- **Audit Findings**: Implementation quirks in several operators
- **Quirks to Document**:
  - **Ternary Operator**: Form 1 inverted logic, Form 3 unexpected behavior
  - **Bitwise Operators**: Strict INT/RAW only (except `~` with matrix support)
  - **String Splitting**: Commented-out functionality in division operator
  - **Experimental Features**: Disabled functionality that could be enabled
- **Documentation Needs**:
  - Document known implementation quirks
  - Explain why certain behaviors exist
  - Provide workarounds for unexpected behavior
  - Note experimental features for future consideration
- **Benefits**:
  - Helps users understand and work around quirks
  - Documents edge cases for future development
  - Provides context for implementation decisions
  - Reduces user frustration with unexpected behavior
- **Effort**: Low (1 week)
- **Priority**: Low (documentation enhancement)
- **Status**: 🟡 PLANNED

---

## 📋 **MOVED TO TODO.md**

### **Items Now in TODO.md (Committed to work on in next 2-4 weeks):**
- **Operator Bug Fixes** (Item #11) → TODO.md Priority #1
- **Cryptographic Features Stabilization** (Item #9) → TODO.md Priority #2
- **Optimization Implementation** (from TODO) → TODO.md Priority #3
- **Unicode Language Binding** (from TODO) → TODO.md Priority #4

### **Items Completed:**
- **Operator Documentation Update** (Item #10) → ✅ COMPLETED

---

## 🔄 **How Items Move Between Documents**

### **From Backlog to TODO:**
- When we commit to working on an item in the next 2-4 weeks
- When an item becomes critical for project success
- When dependencies are resolved and item is ready to start

### **From TODO to Backlog:**
- When we decide not to work on an item in the near term
- When an item is blocked by dependencies
- When priorities change and item is de-prioritized

### **From TODO to Completed:**
- When an item is finished and working
- When documentation is updated to reflect completion
- When tests are added to verify functionality

---

**Last Updated**: July 18, 2024  
**Current Focus**: Items committed to work on are in `TODO.md`  
**Status**: Clean separation established between committed work and planned work 