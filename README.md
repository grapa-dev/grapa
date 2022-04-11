
# grapa
grapa - a grammar parser language / environment for processing data for ETL workflows, or experimenting with new language constructs. 

The unique approach of grapa is that it includes native syntax for JSON/XML and BNF style grammars for defining data processing rules. The same grammar engine used to assist with parsing data is the same engine used to process the language. The syntax of the language can be customized (runtime-self-mutable) as the language rules are stored as global variables. If for example, you'd like to extend the langue to include some preferred syntax , that enhancement would function as native syntax. 

Underneath the syntax, there are several libraries that provide the lower level heavy lifting for strings, execution flow, files, networking, threads, classes with inheritance, encryption (RSA, AES, SHA, etc), math, database, etc. The grammar syntax you define can either invoke routines written in the existing syntax, or call lower level libraries. The native integer and float support uses unlimited precision (or limited by CPU/memory). 

Execution of all scripts (from code or compiled) run the same across all platforms, including the unlimited floating point routines. And will work the same on either big endian or little endian systems. 

# Setup
[Install from binary](docs/BINARY.md)

[Build from source](docs/BUILD.md)

[Run from OS shell](docs/RUN.md)

# Reference
[Base types](docs/BASETYPES.md)

[System types](docs/SYSTYPES.md)

[System libraries](docs/SYSLIBS.md)

[Creating a library](docs/CREATELIB.md)

[Operators](docs/OPERATORS.md)

[Syntax](docs/SYNTAX.md)

# System types
[$TIME](docs/TIME.md)

# System libraries
[File](docs/sys/file.md)

[Math](docs/sys/math.md)

[Net](docs/sys/net.md)

[Thread](docs/sys/thread.md)

[Sys](docs/sys/sys.md)

[Encode](docs/sys/encode.md)

[Bits](docs/sys/bits.md)

[Iterate](docs/sys/iterate.md)

[Output](docs/sys/output.md)

# Examples
[Examples](docs/EXAMPLES.md)

# Third-Party Libraries
* Miniz
* OpenSSL
* FLTK
