
# grapa
grapa - a high-level grammar parser language and environment for processing data in ETL workflows, or experimenting with new language constructs.

## High-Level Language

Grapa provides a **high-level language** defined in [`$grapa.grc`](lib/grapa/$grapa.grc) that most users interact with. This language includes:

- **Natural syntax** for data processing and manipulation
- **Built-in support** for JSON, XML, and HTML parsing
- **Rich data types** including lists, arrays, tables, and vectors
- **Object-oriented features** with inheritance and method chaining
- **Unlimited precision** integers and floats
- **Cross-platform compatibility** across Windows, Linux, and Mac

## Language Extensibility

The grammar is **customizable at runtime** - each rule is loaded into global variables that can be overridden or replaced, effectively changing the language syntax. Rules can be created to parse any data that a grammar can define, with the output being what each rule resolves to. See [$RULE](docs/type/RULE.md) for more details.

## Underlying Architecture

The base grammar compiles to $OP and $CODE operations (see [Base types](docs/BASETYPES.md)). An $OP consists of a reference to a C++ function with parameters, and a $CODE is a sequence of $OP's. The system provides many C++ functions for core functionality like variable handling, networking, and file I/O.

## Advanced Features

- **Class library with inheritance** for extending the language
- **Runtime-mutable BNF grammar** stored as a $LIST with C++ entry points
- **Unlimited precision arithmetic** - all integers are big integers, all floats are big floats
- **Cross-platform binary compatibility** across Windows, Linux, Mac (Intel and Apple Silicon) 

# Python
- [Python setup](docs/PYTHON.md)
- [Python integration guide](docs/python_integration.md) - Using Grapa with Python for data science, web development, and system administration

# C++
- [Install from binary](docs/BINARY.md)
- [Build from source](docs/BUILD.md)
- [Run from OS shell](docs/RUN.md)

# Language Reference

## Getting Started
- [Getting Started](docs/GETTING_STARTED.md) - Quick start guide for new users

## High-Level Language
- [Examples](docs/EXAMPLES.md) - Common usage patterns and examples
- [Testing](docs/TESTING.md) - How to test your Grapa code
- [Grep functionality](docs/GREP.md) - Advanced pattern matching with Unicode support

## System Functions (Advanced)
- [System functions](docs/SYSTEM_FUNCTIONS.md) - Complete reference of all available system functions
- [Function quick reference](docs/FUNCTION_QUICK_REFERENCE.md) - Concise function lookup

## Data Types
- [Base types](docs/BASETYPES.md) - Core $OP and $CODE types
- [System types](docs/SYSTYPES.md) - All available data types
- [$OP](docs/type/OP.md) - Operation type
- [$CODE](docs/type/CODE.md) - Code sequence type

# Libraries (Grapa classes)
- [$file](docs/sys/file.md)
- [$math](docs/sys/math.md)
- [$net](docs/sys/net.md)
- [$thread](docs/sys/thread.md)
- [$sys](docs/sys/sys.md)

# $OBJ Functions (inherited by several Grapa classes)
- [Encode functions](docs/obj/encode.md)
- [Bits functions](docs/obj/bits.md)
- [Iterator functions](docs/obj/iterate.md)
- [Format functions](docs/obj/format.md)
- [Transform functions](docs/obj/transform.md)
- [Type functions](docs/obj/type.md)
- [Vector functions](docs/obj/vector.md)
- [IO functions](docs/obj/io.md)
- [Document functions](docs/obj/document.md)

# Operators
- [Constants](docs/operators/constants.md)
- [Math operators](docs/operators/math.md)
- [Bit operators](docs/operators/bit.md)
- [Assignment operators](docs/operators/assignment.md)
- [Compare operators](docs/operators/compare.md)
- [Condition operators](docs/operators/condition.md)
- [Loop operators](docs/operators/loop.md)
- [Function operators](docs/operators/function.md)
- [Command operators](docs/operators/command.md)
- [Lexical operators](docs/operators/lexical.md)

# Syntax
- [Grammar](docs/syntax/grammar.md)
- [Precedence](docs/syntax/precedence.md)
- [Operator](docs/syntax/operator.md)
- [Chaining](docs/syntax/chaining.md)

# Examples
- [Examples](docs/EXAMPLES.md)

# Third-Party libraries used
* [Miniz zlib](https://github.com/richgel999/miniz/releases)
* [OpenSSL](https://www.openssl.org/)
* [FLTK](https://www.fltk.org/)
* [blst](https://github.com/supranational/blst)
