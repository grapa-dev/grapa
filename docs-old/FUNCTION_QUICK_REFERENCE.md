# Grapa System Functions Quick Reference

> **For Python Users:**
> - [GrapaPy Quickstart](GRAPAPY_INTEGRATION.md#quickstart-for-python-users)
> - [Migration Tips for Python Users](PYTHON_USE_CASES.md#migration-tips-for-python-users)
> - [Python Use Cases](PYTHON_USE_CASES.md)
>
> **For Newcomers/Non-Python Users:**
> - [Getting Started](GETTING_STARTED.md)
> - [Examples](EXAMPLES.md)
> - [JS-to-Grapa Migration Guide](JS_TO_GRAPA_MIGRATION.md)
> - [Language Reference](FUNCTION_QUICK_REFERENCE.md)

---

## Table of Contents
- [Core Language](#core-language) <!-- tags: grammar, rules, system -->
- [Variables & Scope](#variables--scope) <!-- tags: variables, scope, assignment -->
- [Data Structures](#data-structures) <!-- tags: array, tuple, list, xml, tag, code -->
- [Search & Analysis](#search--analysis) <!-- tags: search, sort, group, unique -->
- [Control Flow](#control-flow) <!-- tags: flow, if, while, switch, plan -->
- [Functional Programming](#functional-programming) <!-- tags: map, reduce, filter, range, parallel -->
- [Type & I/O](#type--io) <!-- tags: type, io, error, message, prompt -->
- [Cryptography](#cryptography) <!-- tags: crypto, prime, random, sign, verify -->
- [Math - Basic](#math---basic) <!-- tags: math, arithmetic, pow, mod, abs -->
- [Math - Trigonometry](#math---trigonometry) <!-- tags: trig, sin, cos, tan -->
- [Math - Hyperbolic](#math---hyperbolic) <!-- tags: hyperbolic, sinh, cosh, tanh -->
- [Math - Special](#math---special) <!-- tags: atan2, hypot, special -->
- [Bit Operations](#bit-operations) <!-- tags: bit, shift, and, or, xor -->
- [Matrix Operations](#matrix-operations) <!-- tags: matrix, linear algebra, transpose, solve -->
- [Type Conversion](#type-conversion) <!-- tags: convert, int, float, str, array, xml -->
- [String Operations](#string-operations) <!-- tags: string, trim, pad, grep, split, join -->
- [Array/Matrix](#arraymatrix) <!-- tags: array, matrix, shape, sum, mean -->
- [Comparison](#comparison) <!-- tags: compare, eq, neq, gt, lt -->
- [Time](#time) <!-- tags: time, utc, tz -->
- [File System](#file-system) <!-- tags: file, directory, table, metadata -->
- [Networking](#networking) <!-- tags: network, net, connect, send, receive -->
- [HTTP](#http) <!-- tags: http, read, send, message -->
- [Threading](#threading) <!-- tags: thread, lock, wait, start, stop -->
- [GUI Widgets](#gui-widgets) <!-- tags: gui, widget, show, hide, event -->
- [OOP](#oop) <!-- tags: class, object, oop -->
- [Usage](#usage)
- [See Also](#see-also)

---

## Core Language <!-- tags: grammar, rules, system -->
System-level grammar and rule management functions.
`rule`, `ruleval`, `ruleref`, `token`, `grammar`, `reset`, `shell`, `eval`, `exec`, `post`, `sleep`, `exit`, `getenv`, `putenv`, `include`

[Back to Top](#grapa-system-functions-quick-reference)

## Variables & Scope <!-- tags: variables, scope, assignment -->
Variable declaration, assignment, and scope management.
`name`, `getname`, `lit`, `ref`, `setnull`, `var`, `assign`, `assignappend`, `assignextend`, `global`, `parent`, `this`, `local`, `static`, `const`, `setconst`

[Back to Top](#grapa-system-functions-quick-reference)

## Data Structures <!-- tags: array, tuple, list, xml, tag, code -->
Create and manipulate arrays, tuples, lists, XML, tags, and code blocks.
`createarray`, `createtuple`, `createlist`, `createxml`, `createel`, `createtag`, `createop`, `createcode`, `createrule`, `extend`, `remove`, `prepend`, `pop`, `call`

[Back to Top](#grapa-system-functions-quick-reference)

## Search & Analysis <!-- tags: search, sort, group, unique -->
Search, sort, group, and analyze data.
`search`, `findall`, `sort`, `argsort`, `unique`, `group`

[Back to Top](#grapa-system-functions-quick-reference)

## Control Flow <!-- tags: flow, if, while, switch, plan -->
Control program execution and logic flow.
`return`, `break`, `if`, `while`, `scope`, `switch`, `case`, `plan`, `wrap`, `op`

[Back to Top](#grapa-system-functions-quick-reference)

## Functional Programming <!-- tags: map, reduce, filter, range, parallel -->
Parallel and sequential data processing functions.
`map`, `reduce`, `filter`, `range`

> **Note:** `map` and `filter` are parallel by default and robust for ETL workloads; `reduce` is sequential. All are production-ready for high-throughput data processing. See [Examples](EXAMPLES.md#data-processing) and [Migration Guides](PYTHON_TO_GRAPA_MIGRATION.md#migrating-python-for-loops) for best practices and caveats.

[Back to Top](#grapa-system-functions-quick-reference)

## Type & I/O <!-- tags: type, io, error, message, prompt -->
Type checking, error handling, and input/output.
`isint`, `iferr`, `message`, `string`, `echo`, `console`, `prompt`

[Back to Top](#grapa-system-functions-quick-reference)

## Cryptography <!-- tags: crypto, prime, random, sign, verify -->
Cryptographic operations, prime generation, and random number utilities.
`genprime`, `staticprime`, `isprime`, `isaks`, `random`, `setbit`, `clearbit`, `genbits`, `genkeys`, `encode`, `encoderaw`, `decode`, `sign`, `signadd`, `verify`, `verifyrecover`, `secret`

[Back to Top](#grapa-system-functions-quick-reference)

## Math - Basic <!-- tags: math, arithmetic, pow, mod, abs -->
Basic arithmetic and mathematical constants.
`setfloat`, `setfix`, `root`, `pow`, `mod`, `modpow`, `modinv`, `abs`, `gcd`, `e`, `pi`, `ln`, `log`, `add`, `sub`, `mul`, `div`

[Back to Top](#grapa-system-functions-quick-reference)

## Math - Trigonometry <!-- tags: trig, sin, cos, tan -->
Trigonometric functions.
`sin`, `cos`, `tan`, `cot`, `sec`, `csc`, `asin`, `acos`, `atan`, `acot`, `asec`, `acsc`

[Back to Top](#grapa-system-functions-quick-reference)

## Math - Hyperbolic <!-- tags: hyperbolic, sinh, cosh, tanh -->
Hyperbolic and inverse hyperbolic functions.
`sinh`, `cosh`, `tanh`, `coth`, `sech`, `csch`, `asinh`, `acosh`, `atanh`, `acoth`, `asech`, `acsch`

[Back to Top](#grapa-system-functions-quick-reference)

## Math - Special <!-- tags: atan2, hypot, special -->
Special mathematical functions.
`atan2`, `hypot`

[Back to Top](#grapa-system-functions-quick-reference)

## Bit Operations <!-- tags: bit, shift, and, or, xor -->
Bitwise and logical operations.
`bsl`, `bsr`, `bor`, `band`, `or`, `and`, `xor`, `inv`

[Back to Top](#grapa-system-functions-quick-reference)

## Matrix Operations <!-- tags: matrix, linear algebra, transpose, solve -->
Matrix and linear algebra operations.
`t`, `rref`, `det`, `rank`, `solve`, `cov`, `neg`

[Back to Top](#grapa-system-functions-quick-reference)

## Type Conversion <!-- tags: convert, int, float, str, array, xml -->
Convert between types and representations.
`bits`, `bytes`, `len`, `bool`, `not`, `int`, `raw`, `base`, `str`, `list`, `array`, `vector`, `xml`, `float`, `fix`, `time`, `type`, `describe`

[Back to Top](#grapa-system-functions-quick-reference)

## String Operations <!-- tags: string, trim, pad, grep, split, join -->
String manipulation and analysis.
`left`, `right`, `mid`, `midtrim`, `rtrim`, `ltrim`, `trim`, `rrot`, `lrot`, `rpad`, `lpad`, `reverse`, `replace`, `grep`, `split`, `join`, `lower`, `upper`

[Back to Top](#grapa-system-functions-quick-reference)

## Array/Matrix <!-- tags: array, matrix, shape, sum, mean -->
Array and matrix shape and statistics.
`shape`, `reshape`, `dot`, `identity`, `diagonal`, `triu`, `tril`, `eigh`, `sum`, `mean`

[Back to Top](#grapa-system-functions-quick-reference)

## Comparison <!-- tags: compare, eq, neq, gt, lt -->
Comparison and ordering operations.
`eq`, `neq`, `gteq`, `gt`, `lteq`, `lt`, `cmp`

[Back to Top](#grapa-system-functions-quick-reference)

## Time <!-- tags: time, utc, tz -->
Time and timezone utilities.
`utc`, `tz`

[Back to Top](#grapa-system-functions-quick-reference)

## File System <!-- tags: file, directory, table, metadata -->
File and directory navigation, file operations, and file-based database utilities.
**Navigation**: `file_pwd` (working dir), `file_cd` (change working dir), `file_phd` (home dir), `file_chd` (change home dir)
**Operations**: `file_ls`, `file_mk`, `file_rm`, `file_set`, `file_get`, `file_info` (metadata: type, size, existence)
**Database**: `file_table`, `file_mkfield`, `file_rmfield`, `file_split` (split large files), `file_debug`

[Back to Top](#grapa-system-functions-quick-reference)

## Networking <!-- tags: network, net, connect, send, receive -->
Network interface, connection, and certificate management.
`net_mac`, `net_interfaces`, `net_connect`, `net_bind`, `net_listen`, `net_onlisten`, `net_disconnect`, `net_proxy`, `net_certificate`, `net_private`, `net_trusted`, `net_verify`, `net_chain`, `net_host`, `net_send`, `net_receive`, `net_pending`, `net_onreceive`

[Back to Top](#grapa-system-functions-quick-reference)

## HTTP <!-- tags: http, read, send, message -->
HTTP read, send, and message operations.
`http_read`, `http_send`, `http_message`

[Back to Top](#grapa-system-functions-quick-reference)

## Threading <!-- tags: thread, lock, wait, start, stop -->
Thread management and synchronization.
`thread_trylock`, `thread_lock`, `thread_unlock`, `thread_wait`, `thread_signal`, `thread_waiting`, `thread_start`, `thread_stop`, `thread_started`, `thread_suspend`, `thread_resume`, `thread_suspended`

[Back to Top](#grapa-system-functions-quick-reference)

## GUI Widgets <!-- tags: gui, widget, show, hide, event -->
Graphical user interface widget management.
`widget_new`, `widget_show`, `widget_hide`, `widget_redraw`, `widget_resizable`, `widget_resize`, `widget_parent`, `widget_child`, `widget_next`, `widget_focus`, `widget_get`, `widget_set`, `widget_handle`, `widget_callback`, `widget_event_key`, `widget_append`, `widget_post`, `widget_clear`

[Back to Top](#grapa-system-functions-quick-reference)

## OOP <!-- tags: class, object, oop -->
Object-oriented programming support.
`class`, `obj`

[Back to Top](#grapa-system-functions-quick-reference)

## Usage
```grapa
// Direct call
result = $sys.function_name(params);

// In BNF grammar
expression = $sys.add(term, expression);

// In library files
function = op(){@this.$sys.function_name(@var.{params});};
```

**Total: 200+ system functions available**

[Back to Top](#grapa-system-functions-quick-reference)

---

## See Also
- [Getting Started](GETTING_STARTED.md)
- [Examples](EXAMPLES.md)
- [Python-to-Grapa Migration Guide](PYTHON_TO_GRAPA_MIGRATION.md)
- [JS-to-Grapa Migration Guide](JS_TO_GRAPA_MIGRATION.md)
- [System Functions (Detailed)](SYSTEM_FUNCTIONS.md)
- [Troubleshooting](TROUBLESHOOTING.md) 