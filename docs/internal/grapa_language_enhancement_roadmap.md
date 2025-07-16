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
- Unlimited precision math ($INT, $FLOAT, $TIME)
- Runtime-mutable grammar and executable BNF
- Advanced Unicode grep/regex
- Integrated data processing pipelines
- Native JSON/XML round-tripping via eval/.str()

## 5. Next Steps / TODO (for future tracking)
- [ ] Prioritize enhancements based on user feedback and target audience
- [ ] Design and prototype new language features (loops, exceptions, modules)
- [ ] Expand and document Python integration patterns
- [ ] Develop test cases and examples for new features
- [ ] Review and update documentation as features are added

*This document is for internal planning and is not linked from user-facing documentation.* 