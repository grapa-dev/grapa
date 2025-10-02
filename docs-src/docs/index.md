# Welcome to Grapa Documentation

## How to Use This Documentation
- Use the search bar (top right) to quickly find functions, guides, and examples.
- The navigation menu groups all reference material under 'Reference', guides under 'Guides', and support under 'Support'.
<!-- Advanced Topics link removed: content consolidated or planned/future -->
- For troubleshooting, see [Troubleshooting](troubleshooting.md).
- For a full API and type reference, see [Reference](api_reference.md).

---

## Getting Started
- [Quickstart Guide](getting_started.md)
- [Installation](installation.md) - Install Grapa on your platform
- [CLI Quickstart](cli_quickstart.md)
- [VSCode Extension](vscode_extension.md) - IDE support for Grapa development
- [API Reference](api_reference.md)
- [Migration Guides](migrations/python_to_grapa_migration.md)
- [Examples](examples.md)
- [Grapa in the Ecosystem](grapa_ecosystem_positioning.md) - When to use Grapa vs. other languages

## Advanced Topics
- [Machine Learning](machine_learning.md) - Linear regression, statistical analysis, and ML algorithms using vector operations
- [Debugging](debugging.md) - Comprehensive debugging system with CLI and script control
- [Grammar Design](syntax/grammar_design.md) - Create your own languages with Grapa
- [Multi-Syntax Programming](multi_syntax_programming.md) - JSON, XML, HTML (native), SQL/PROLOG/LISP (example implementations), and custom syntax support
- [Language Design with Executable BNF](language_design_with_executable_bnf.md) - Advanced language design and DSL creation
- [Isolated Rule Execution](isolated_rule_execution.md) - ETL and domain-specific processing without affecting main language

## Language Integration Examples
- [SQL Integration](sql_integration.md) - Example implementation of SQL syntax using Grapa's executable BNF
- [PROLOG Integration](prolog_integration.md) - Example implementation of PROLOG syntax using Grapa's executable BNF
- [LISP Integration](lisp_integration.md) - Example implementation of LISP syntax using Grapa's executable BNF

## Key Features
- **Dynamic Code Execution**: Compile and execute code at runtime with `op()(<string>)()` and `$sys().eval()`
- **Execution Trees**: Direct manipulation of `$OP` objects for advanced meta-programming
- **Multi-Syntax Support**: JSON, XML, HTML (native), SQL/PROLOG/LISP (example implementations), and custom syntax through executable BNF
- **Parallel Processing**: Built-in parallel data operations with `.map()`, `.filter()`, `.reduce()`
- **Comprehensive Vector Operations**: Complete matrix operations, linear algebra, statistical functions, and creative capabilities
- **Machine Learning**: Linear regression, statistical analysis, and ML algorithms using optimized vector operations
- **AI/ML Model Integration**: Native support for AI models via `$MODEL` type for text generation
- **Advanced Pattern Matching**: 100% ripgrep compatibility with binary data support and Unicode awareness
- **HTTP/HTTPS Networking**: Complete HTTP client with SSL/TLS, proxy support, cookie handling, and chunked encoding
- **Comprehensive Cryptography**: Production-ready cryptographic capabilities with OpenSSL 3.5.2 integration (RSA, EC, DH, AES, hash functions, key exchange)
- **Unlimited Precision**: Native support for arbitrary-precision arithmetic
- **Cross-Platform**: Runs on Windows, macOS (Apple Silicon), and Linux

## New Features
- **Searchable API Reference:** Find all built-in functions, types, and methods in one place.
- **Tag-based Filtering:** Use tags in the API Reference and throughout the docs to filter by topic, type, or use case.
- **Mobile-Friendly:** This site is fully responsive—try it on your phone or tablet!

## Recent Major Enhancements
- **Comprehensive Vector Operations:** Complete matrix operations, linear algebra, statistical functions, and creative capabilities
- **Advanced Sorting & Selection:** Custom comparison functions, order vectors, signed/unsigned comparison
- **Creative Function Application:** `vec * [op(x){...}]` for random generation, pattern creation, custom transformations
- **Enhanced Documentation:** Improved organization, navigation, and cross-references
- **Mixed Data Type Vectors:** Automatic type conversion and "best assumptions" for operations

## Need Help?
- [Troubleshooting](troubleshooting.md)
- [Updates](updates.md) - Keep Grapa up to date

---

*Grapa: High-performance scripting for data, ETL, and automation.*

---

> **Note:** After this round of improvements, we will review all documentation to determine where additional visuals, diagrams, or flowcharts would be most helpful for users.
