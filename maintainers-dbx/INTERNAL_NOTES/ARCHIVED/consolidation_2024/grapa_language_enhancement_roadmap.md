# Grapa Language Enhancement Roadmap (Internal)

## 1. General Language Feature Gaps
- No `for`/`foreach` loops (only `while`)
- No native exception handling (`try/catch`)
    - **Note:** Grapa does not support `try`/`catch` or any structured exception handling. All error handling must be done via explicit checks (e.g., checking return values, using `if` statements). There is no built-in way to catch or recover from runtime exceptions.
- Limited string formatting/interpolation
- No module/import system for code reuse
- User-defined classes/objects not fully documented or idiomatic
- No list comprehensions/generator expressions
- No built-in date/time parsing/formatting utilities
- No built-in CSV/TSV/Excel parsing
- No plotting/visualization
- No package manager/ecosystem
- No built-in documentation/help system
- No native Unicode support outside grep
- No direct async/concurrent programming support
- **No `.get()` or `.set()` methods on lists/arrays:** Must use `[]` for access and assignment. This is a common Python/JavaScript habit that does not work in Grapa.
- **No `.match()` method on strings:** Use `.grep()` for regex/pattern matching. `.match()` is not implemented.
- **No `//` line comments:** Only block comments (`/* ... */`) are supported. Using `//` will cause syntax errors.
- **End-of-line comments inside `{}` code blocks do not work:** Block comments at the end of a line within a code block can cause parsing errors.
- **No `.append()` or `.push()` on arrays/lists:** Use `+=` for appending.
- **No attribute-style access for list/dict keys:** Must use `[]` or explicit key lookup, not dot notation for dynamic keys.
- **No automatic truthy/falsy conversion:** Explicit boolean checks are required; non-empty strings/lists are not automatically `true`.
- **No implicit variable declaration:** All variables must be explicitly declared/assigned before use.
- **No built-in `len()` function:** Use `.len()` method on arrays/lists/strings.
- **No built-in `map()`/`filter()`/`reduce()` as global functions:** These are methods on arrays/lists, not standalone functions.
- **No nested block comments:** Grapa does not support nested comments (e.g., `/* ... /* ... */ ... */`). Attempting to nest block comments will break parsing. This is a common expectation from other languages and a source of confusion/errors.

*These gaps are common stumbling blocks for new users coming from Python, JavaScript, or similar languages, and are frequent sources of incorrect code generation or user confusion.*

## 2. Most Useful Additions for Usability
- Add `for`/`foreach` loops
- Add exception handling (`try/catch`)
- Add string interpolation/formatting
- Add module/import system
- Expand and document OOP support
- Expand Unicode support to all string operations
- Add standard library for math, stats, date/time, CSV, HTTP, JSON
- Add high-level HTTP client abstraction
- Add DataFrame-like table API
- Add plotting/visualization utilities

## 3. Python Integration Opportunities
- Seamless data exchange (arrays, dicts, DataFrames)
- Better error propagation (map Grapa errors to Python exceptions)
- High-level file/table/dataframe API (Pandas-like)
- Vectorized operations (NumPy-like)
- Direct function callbacks from Grapa to Python
- Improved documentation/examples for integration

## 4. Unique Grapa Strengths to Highlight
- **Superior Dynamic Code Execution**: `op()(<string>)()` and `$sys().eval()` provide human-readable execution trees that are directly executable
- **Execution Tree Architecture**: $OP objects are both compiled code and inspectable data structures
- **Advanced Meta-Programming**: Direct manipulation of execution trees enables sophisticated code generation
- **Unlimited precision math ($INT, $FLOAT, $TIME)**
- **Runtime-mutable grammar and executable BNF**
- **Advanced Unicode grep/regex**
- **Integrated data processing pipelines**
- **Native JSON/XML round-tripping via eval/.str()**

## 5. Unicode Enhancement Progress - UPDATED

### ✅ Completed
- **Unicode Case Folding C++ Implementation**: Lookup table-based case folding implemented in `source/grep/grapa_grep_unicode.hpp`
- **Turkish I Support**: İ (U+0130) → i (U+0069), ı (U+0131) → i (U+0069)
- **German Sharp S Support**: ß (U+00DF) → s (U+0073)
- **Greek Final Sigma Support**: ς (U+03C2) → σ (U+03C3)
- **Common Accented Characters**: À→à, Á→á, etc.
- **Composed Form Output**: Produces composed forms instead of decomposed forms, avoiding regex matching issues

### 🔄 In Progress
- **Grapa Language Binding**: Need to add `case_fold()` method to `lib/grapa/$OBJ.grc`
- **Integration Testing**: Verify end-to-end functionality from Grapa scripts

### 📋 Next Steps
- Add `case_fold = @<"case_fold",{@<this>}>;` to `lib/grapa/$OBJ.grc`
- Connect to C++ `UnicodeString::case_fold()` implementation
- Test Turkish I case folding from Grapa scripts
- Make existing `upper()` and `lower()` methods Unicode-aware
- Add Unicode normalization methods to Grapa language

## 6. Next Steps / TODO (for future tracking)
- [ ] Prioritize enhancements based on user feedback and target audience
- [ ] Design and prototype new language features (loops, exceptions, modules)
- [ ] Expand and document Python integration patterns
- [ ] Develop test cases and examples for new features
- [ ] Review and update documentation as features are added
- [ ] **Implement Grapa language binding for Unicode case folding** (High Priority)
- [ ] **Make all string operations Unicode-aware** (Medium Priority)

*This document is for internal planning and is not linked from user-facing documentation.* 

## 7. Language Gaps Identified from Migration Guides (2024)

- **Meta-programming (alias, undef, defined?)**: Needed for dynamic method aliasing, method removal, and runtime introspection (Ruby, Kotlin, Swift).
- **File/line macros (__FILE__, __LINE__, #file, #line)**: Useful for debugging, error reporting, and meta-programming (Ruby, Swift).
- **Block/yield support (yield, block_given?)**: Enables Ruby-style block passing and advanced functional idioms.
- **Ensure/finally/defer (ensure, finally, defer)**: Needed for resource cleanup and exception-safe code (Ruby, Kotlin, Swift).
- **Protocol/interface/extension (protocol, interface, extension)**: Needed for advanced type abstraction and code reuse (Swift, Kotlin).
- **Annotation/meta-programming (@file:JvmName, @objc, @escaping, @Annotation)**: Needed for advanced code generation and interop (Kotlin, Swift).
- **Coroutine/async (coroutineScope)**: Needed for async/concurrent programming (Kotlin).
- **Advanced type features (typealias, Any, generics)**: Needed for type abstraction and generic programming (Swift, Kotlin).
- **Type checking/casting (is, as, as?)**: Needed for safe type operations (Kotlin, Swift).
- **Guard statement (guard)**: Needed for early exit and error handling (Swift). 