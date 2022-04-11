
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
[System libs](docs/SYSLIBS.md)

[Creating a library](docs/CREATELIB.md)

[Operators](docs/OPERATORS.md)

[Syntax](docs/SYNTAX.md)

# Base types
[$OP](docs/type/OP.md)

[$CODE](docs/type/CODE.md)

# System types

[$BOOL](docs/type/BOOL.md)

[$INT](docs/type/INT.md)

[$FLOAT](docs/type/FLOAT.md)

[$STR](docs/type/STR.md)

[$ID](docs/type/ID.md)

[$RAW](docs/type/RAW.md)

[$TIME](docs/type/TIME.md)

[$RULE](docs/type/RULE.md)

[$ARRAY](docs/type/ARRAY.md)

[$LIST](docs/type/LIST.md)

[$TABLE](docs/type/TABLE.md)

[$VECTOR](docs/type/VECTOR.md)

[$XML](docs/type/XML.md)

[$TAG](docs/type/TAG.md)

[$ERR](docs/type/ERR.md)

[$SYSID](docs/type/SYSID.md)

[$SYSINT](docs/type/SYSINT.md)

[$SYSSTR](docs/type/SYSSTR.md)


# System libraries
[$file](docs/sys/file.md)

[$math](docs/sys/math.md)

[$net](docs/sys/net.md)

[$thread](docs/sys/thread.md)

[$sys](docs/sys/sys.md)

# Object inherited functions

[Encode functions](docs/sys/encode.md)

[Bits functions](docs/sys/bits.md)

[Iterator functions](docs/sys/iterate.md)

[Output functions](docs/sys/output.md)

[Sets funcitons - string, arry, etc.](docs/sys/sets.md)

[Type functions](docs/sys/type.md)

# Examples
[Examples](docs/EXAMPLES.md)

# Third-Party Libraries
* Miniz
* OpenSSL
* FLTK
