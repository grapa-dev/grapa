
# grapa
grapa - a grammar parser language / environment for processing data for ETL workflows, or experimenting with new language constructs. 

Grapa grammar is customizable at runtime. Default grammar here: [Grapa Grammar](lib/grapa/$grapa.grc)

Each rule in the grammar is loaded into global variables, which can be overridden, or replaced - which effectively changes the syntax of the language. Additionally, rules can be created to parse any data that a grammer can be used to define, where the output is what each rule resolves to. See [$RULE](docs/type/RULE.md) for more details.

The grammar includes parsing for JSON/XML/HTML. The underlying data structures are based on JSON, but add extentions for several native Grapa types such as $OP, as well as XML (JSON and XML can co-exist). 

The base grammar for the langauge is $OP and $CODE (see [Base types](docs/BASETYPES.md)) - and any compilation will translate to this as the executable. An $OP consists of a reference to a C++ function along with parameters. A $CODE is a sequence of $OP's. There are many C++ functions for base functionality such as variable handling, networking, and file IO. 

Grapa includes a form of a class library with inheritance, which is used to enxtend the language based on a default syntax for calling class library methods.

Integers are all big integers and floats are all big floats. Percision is unlimited, within available memory. 

The language is cross platform binary/execution compatible accross Windows, Linux, Mac (Intel and Apple). 

# Python
- [Python setup](docs/PYTHON.md)

# C++
- [Install from binary](docs/BINARY.md)
- [Build from source](docs/BUILD.md)
- [Run from OS shell](docs/RUN.md)

# Base Types
- [Base types](docs/BASETYPES.md)
- [Base libraries](docs/BASELIBS.md)
- [$OP](docs/type/OP.md)
- [$CODE](docs/type/CODE.md)

# System types
- [$BOOL](docs/type/BOOL.md)
- [$INT](docs/type/INT.md)
- [$FLOAT](docs/type/FLOAT.md)
- [$STR](docs/type/STR.md)
- [$ID](docs/type/ID.md)
- [$RAW](docs/type/RAW.md)
- [$TIME](docs/type/TIME.md)
- [$RULE](docs/type/RULE.md)
- [$ARRAY](docs/type/ARRAY.md)
- [$LIST](docs/type/LIST.md)
- [$VECTOR](docs/type/VECTOR.md)
- [$TABLE](docs/type/TABLE.md)
- [$XML](docs/type/XML.md)
- [$TAG](docs/type/TAG.md)
- [$WIDGET](docs/type/WIDGET.md)
- [$ERR](docs/type/ERR.md)
- [$SYSID](docs/type/SYSID.md)
- [$SYSINT](docs/type/SYSINT.md)
- [$SYSSTR](docs/type/SYSSTR.md)

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
