
# Grapa

/*
Project Philosophy:
Grapa is designed to be inviting and usable for the broadest audience—Python users, educators, researchers, data engineers, and anyone interested in data processing, language experimentation, or high-performance computing. Our mission is to make advanced grammar-based programming, unlimited precision, and parallelism accessible and productive for all.
*/

---

## Why Grapa?
- **Unlimited Precision:** $INT, $FLOAT, and $TIME types support arbitrary precision for scientific, cryptographic, and financial applications.
- **Executable BNF & Mutable Grammar:** Define, modify, and experiment with grammars at runtime—ideal for language design, teaching, and research.
- **Parallelism & Concurrency:** True parallel processing with $thread, $net, and map/reduce/filter, overcoming Python's GIL and enabling high-performance ETL and data workflows.
- **Unified File/Database API:** Seamlessly navigate and process files and databases with a single, consistent interface.
- **Python Integration:** Use GrapaPy to bring all these features to Python, with idiomatic APIs and migration tips for Python users.
- **Education & Prototyping:** Rapidly prototype new languages, teach compiler concepts, and experiment with meta-programming.

---

## 🚀 Quickstart

1. **Install Grapa**  
   See [Getting Started](docs/GETTING_STARTED.md) for platform-specific instructions.
2. **Run Your First Script**
   ```sh
   ./grapa -cfile "examples/hello_world.grc"
   ```
3. **Explore Use Cases & Examples**  
   See [Use Cases](docs/USE_CASES.md) and [Examples](docs/EXAMPLES.md) for real-world workflows in both Grapa and Python.

---

## 📖 Table of Contents

- [Getting Started](docs/GETTING_STARTED.md)
- [Why Grapa? (Use Cases)](docs/USE_CASES.md)
- [Python Quickstart & Use Cases](docs/PYTHON_USE_CASES.md)
- [Syntax Quick Reference](docs/syntax/basic_syntax.md)
- [Examples](docs/EXAMPLES.md)
- [Grep (Unicode-aware)](docs/GREP.md)
- [Testing](docs/TESTING.md)
- [Writing & Running Grapa Scripts](docs/grc_scripts.md)
- [Project & File Navigation](docs/directory_navigation.md)
- [Troubleshooting](docs/TROUBLESHOOTING.md)

### System Features
- [$file (File I/O)](docs/sys/file.md)
- [$math (Math)](docs/sys/math.md)
- [$net (Networking)](docs/sys/net.md)
- [$thread (Threading)](docs/sys/thread.md)
- [$sys (System)](docs/sys/sys.md)

### Data Types
- [Base Types](docs/BASETYPES.md)
- [System Types](docs/SYSTYPES.md)
- [$OP](docs/type/OP.md)
- [$CODE](docs/type/CODE.md)
- [All Type Docs](docs/type/)

### Operators
- [Operator Quick Reference](docs/syntax/operator.md)
- [Operator Precedence Table](docs/syntax/precedence.md)
- [Conditionals (if, switch, ?)](docs/operators/condition.md)
- [Assignment](docs/operators/assignment.md)
- [Math](docs/operators/math.md)
- [Bitwise](docs/operators/bit.md)
- [Compare](docs/operators/compare.md)
- [Loop](docs/operators/loop.md)
- [Function](docs/operators/function.md)
- [Command](docs/operators/command.md)
- [Lexical](docs/operators/lexical.md)
- [Constants](docs/operators/constants.md)

### Database Features
- [Database Quick Reference](docs/database/quick_reference.md)
- [Column Store Guide](docs/database/column_store.md)
- [Database Troubleshooting](docs/database/troubleshooting.md)

### Advanced/Reference
- [System Functions](docs/SYSTEM_FUNCTIONS.md)
- [Function Quick Reference](docs/FUNCTION_QUICK_REFERENCE.md)
- [GRZ Format Specification](docs/GRZ_FORMAT.md)
- [Binary Grep Guide](docs/BINARY_GREP.md)
- [System Include Reference](docs/INCLUDE_SYSTEM.md)
- [Implementation Progress](docs/IMPLEMENTATION_PROGRESS.md)
- [Unicode Grep Status](docs/UNICODE_GREP_STATUS.md)
- [Ripgrep Parity Results](docs/RIPGREP_PARITY_RESULTS.md)
- [Production Readiness Assessment](docs/PRODUCTION_READINESS_ASSESSMENT.md)

### Build & Install
- [Build from Source](docs/BUILD.md)
- [Install from Binary](docs/BINARY.md)
- [Dependencies](docs/DEPENDENCIES.md)
- [AWS/Docker Setup](docs/SETUPAWSDOCKER.md)

### Python & C++ Integration
- [Python Quickstart & Use Cases](docs/PYTHON_USE_CASES.md)
- [Python Setup](docs/PYTHON.md)
- [Python Integration Guide](docs/python_integration.md)
- [GrapaPy Integration](docs/GRAPAPY_INTEGRATION.md)

---

## 🧩 Third-Party Libraries Used

- **Cryptography:** [OpenSSL](https://www.openssl.org/), [blst](https://github.com/supranational/blst), [tiny-AES-c](https://github.com/kokke/tiny-AES-c)
- **Unicode/Text Processing:** [utf8proc](https://github.com/JuliaStrings/utf8proc) (used in grep only)
- **Regular Expressions:** [PCRE2](https://www.pcre.org/)
- **GUI/Graphics:** [FLTK](https://www.fltk.org/)
- **Python Bindings:** [pybind11](https://github.com/pybind/pybind11)
- **Compression:** [miniz](https://github.com/richgel999/miniz)
- **Other:** X11, libX11, libXft, libXext, libXinerama, libXcursor, libXrender, libfontconfig, libfreetype, libpng, libbrotlidec, pthread, libc, libdl, libm, libstdc++

---

## ❓ Getting Help
- [Troubleshooting Guide](docs/TROUBLESHOOTING.md)
- [Directory Navigation](docs/directory_navigation.md)

---

## Contributing
Interested in contributing? See [Implementation Progress](docs/IMPLEMENTATION_PROGRESS.md) and [Production Readiness Assessment](docs/PRODUCTION_READINESS_ASSESSMENT.md).

---

## License
See LICENSE file for details.
