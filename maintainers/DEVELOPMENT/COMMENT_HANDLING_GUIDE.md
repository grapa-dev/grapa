# Grapa Comment Handling Guide

**Status:** Active – Update as new findings or improvements are made.

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

## Examples

### ✅ Supported Comment Locations
```grapa
/* This is a valid comment before a command */
x = 5;

y = 10; /* This is valid after a command */

/* Standalone comment line */
z = x + y;

/* Comments inside function bodies are supported */
function test() {
    /* Comment before command */
    a = 1;
    b = 2; /* Comment after command */
    /* Standalone comment */
    return a + b;
}
```

### ❌ Unsupported Comment Locations
```grapa
/* This will cause syntax errors */

class MyClass {
    /* ❌ Comments inside class definitions are NOT supported */
    field1 = 1;
    field2 = 2; /* ❌ Comments after class members are NOT supported */
}

function test(
    /* ❌ Comments inside parameter lists are NOT supported */
    param1,
    param2
) {
    /* This is supported */
}
```

## See Also
- **Language Improvement Plan**: `maintainers/DEVELOPMENT/LANGUAGE_IMPROVEMENT_PLAN.md`
- **BNF Reference**: `lib/grapa/$grapa.grc`
- **Canonical Syntax**: `docs-src/docs/syntax/basic_syntax.md`

---

*Last Updated: December 2024*
*This guide was developed based on analysis during the GrapaDBX investigation.* 