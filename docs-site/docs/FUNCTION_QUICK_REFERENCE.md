---
tags:
  - user
  - highlevel
---
# Grapa System Functions Quick Reference

## Core Language
`rule`, `ruleval`, `ruleref`, `token`, `grammar`, `reset`, `shell`, `eval`, `exec`, `post`, `sleep`, `exit`, `getenv`, `putenv`, `include`

## Variables & Scope
`name`, `getname`, `lit`, `ref`, `setnull`, `var`, `assign`, `assignappend`, `assignextend`, `global`, `parent`, `this`, `local`, `static`, `const`, `setconst`

## Data Structures
`createarray`, `createtuple`, `createlist`, `createxml`, `createel`, `createtag`, `createop`, `createcode`, `createrule`, `extend`, `remove`, `prepend`, `pop`, `call`

## Search & Analysis
`search`, `findall`, `sort`, `argsort`, `unique`, `group`

## Control Flow
`return`, `break`, `if`, `while`, `scope`, `switch`, `case`, `plan`, `wrap`, `op`

## Functional Programming
`map`, `reduce`, `filter`, `range`

> **Note:** `map` and `filter` are parallel by default and robust for ETL workloads; `reduce` is sequential. All are production-ready for high-throughput data processing.

## Type & I/O
`isint`, `iferr`, `message`, `string`, `echo`, `console`, `prompt`

## Cryptography
`genprime`, `staticprime`, `isprime`, `isaks`, `random`, `setbit`, `clearbit`, `genbits`, `genkeys`, `encode`, `encoderaw`, `decode`, `sign`, `signadd`, `verify`, `verifyrecover`, `secret`

## Math - Basic
`setfloat`, `setfix`, `root`, `pow`, `mod`, `modpow`, `modinv`, `abs`, `gcd`, `e`, `pi`, `ln`, `log`, `add`, `sub`, `mul`, `div`

## Math - Trigonometry
`sin`, `cos`, `tan`, `cot`, `sec`, `csc`, `asin`, `acos`, `atan`, `acot`, `asec`, `acsc`

## Math - Hyperbolic
`sinh`, `cosh`, `tanh`, `coth`, `sech`, `csch`, `asinh`, `acosh`, `atanh`, `acoth`, `asech`, `acsch`

## Math - Special
`atan2`, `hypot`

## Bit Operations
`bsl`, `bsr`, `bor`, `band`, `or`, `and`, `xor`, `inv`

## Matrix Operations
`t`, `rref`, `det`, `rank`, `solve`, `cov`, `neg`

## Type Conversion
`bits`, `bytes`, `len`, `bool`, `not`, `int`, `raw`, `base`, `str`, `list`, `array`, `vector`, `xml`, `float`, `fix`, `time`, `type`, `describe`

## String Operations
`left`, `right`, `mid`, `midtrim`, `rtrim`, `ltrim`, `trim`, `rrot`, `lrot`, `rpad`, `lpad`, `reverse`, `replace`, `grep`, `split`, `join`, `lower`, `upper`

## Array/Matrix
`shape`, `reshape`, `dot`, `identity`, `diagonal`, `triu`, `tril`, `eigh`, `sum`, `mean`

## Comparison
`eq`, `neq`, `gteq`, `gt`, `lteq`, `lt`, `cmp`

## Time
`utc`, `tz`

## File System
**Navigation**: `file_pwd` (working dir), `file_cd` (change working dir), `file_phd` (home dir), `file_chd` (change home dir)
**Operations**: `file_ls`, `file_mk`, `file_rm`, `file_set`, `file_get`, `file_info` (metadata: type, size, existence)
**Database**: `file_table`, `file_mkfield`, `file_rmfield`, `file_split` (split large files), `file_debug`

## Networking
`net_mac`, `net_interfaces`, `net_connect`, `net_bind`, `net_listen`, `net_onlisten`, `net_disconnect`, `net_proxy`, `net_certificate`, `net_private`, `net_trusted`, `net_verify`, `net_chain`, `net_host`, `net_send`, `net_receive`, `net_pending`, `net_onreceive`

## HTTP
`http_read`, `http_send`, `http_message`

## Threading
`thread_trylock`, `thread_lock`, `thread_unlock`, `thread_wait`, `thread_signal`, `thread_waiting`, `thread_start`, `thread_stop`, `thread_started`, `thread_suspend`, `thread_resume`, `thread_suspended`

## GUI Widgets
`widget_new`, `widget_show`, `widget_hide`, `widget_redraw`, `widget_resizable`, `widget_resize`, `widget_parent`, `widget_child`, `widget_next`, `widget_focus`, `widget_get`, `widget_set`, `widget_handle`, `widget_callback`, `widget_event_key`, `widget_append`, `widget_post`, `widget_clear`

## OOP
`class`, `obj`

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

> **See Also:**
> - [Getting Started](GETTING_STARTED.md)
> - [JS-to-Grapa Migration Guide](JS_TO_GRAPA_MIGRATION.md)
> - [Examples](EXAMPLES.md) 