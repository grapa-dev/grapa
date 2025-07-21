# Grapa Comment Handling Guide

## Supported Comment Style
- Only block comments (`/* ... */`) are supported in Grapa.
- Line comments (`// ...`) are NOT supported.

## Where Block Comments Are Supported
- **Between commands**, **after commands**, **before commands**, and as **standalone lines** at the top level and inside function bodies.
- This is governed by the `$command_list` rule in the BNF (`$grapa.grc`).
- Comments can appear:
  - Before a command
  - After a command (even at the end of a command line)
  - As a standalone line

## Where Block Comments Are NOT Supported
- **Inside class definitions** (i.e., between or after member declarations in `class { ... }`).
- **Inside parameter lists** or other places where only `$param` is allowed.
- **Inside array/object literals** (unless via command context).

## BNF Reference
- `$starcomment` rule: `('/*') <> ('*/')`
- `$command_list` rule allows `$starcomment` in all the above locations.
- `$classlist` (for class members) does NOT allow `$starcomment`.

## Empirical Findings
- Comments inside class definitions cause syntax errors.
- Comments at the end of a command line are supported both at the top level and inside `{}` blocks according to the BNF, but if issues are observed, it may be a parser bug.

## Recommendations
- Place comments only between top-level statements, between function body commands, or at the end of command lines (outside class definitions).
- If you encounter issues with comments in locations allowed by the BNF, review and improve the parser implementation.

## See Also
- [Backlog: Review and improve comment handling](../BACKLOG.md)
- [BNF: lib/grapa/$grapa.grc]

---
This guide should be updated as new findings or improvements are made. 