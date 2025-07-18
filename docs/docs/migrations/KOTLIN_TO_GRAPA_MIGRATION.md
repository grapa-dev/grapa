# Kotlin-to-Grapa Migration Guide

> **Important: Access Patterns for .get() and Indexing (Tested, v0.0.39)**
>
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|
> | $ARRAY    |      ✗      |     ✗      |       ✓         |      —       |
> | $LIST     |      ✗      |     ✗      |       ✓         |     ✓       |
> | $file     |      ✓      |     ✗      |        —         |      —       |
> | $TABLE    |      ✓      |     ✗      |        —         |      —       |
> | $OBJ      |      ✗      |     ✗      |       ✓         |     ✓       |
> | $STR      |      ✗      |     ✗      |       ✓         |      —       |
>
> See [Basic Syntax Guide](syntax/basic_syntax.md) for details.

## Kotlin-to-Grapa Migration Guide (WIP Features)

### Main Table: Core Gaps (Tracked for Enhancement)
| Kotlin Feature              | Grapa Equivalent | Status/Note |
|-----------------------------|------------------|-------------|
| `import kotlin.math.*`      | —                | **GAP**: No module/import system |
| `@file:JvmName("Foo")`     | —                | **GAP**: No annotation/meta-programming |
| `coroutineScope { ... }`    | —                | **GAP**: No coroutine/async support |
| `finally`                   | —                | **GAP**: No finally/ensure |
| `x!!` (non-null assert)     | —                | **GAP**: No null assertion |
| `x?.foo()` (safe call)      | —                | **GAP**: No safe call operator |
| `is` (type check)           | —                | **GAP**: No direct type check operator |
| `as` (type cast)            | —                | **GAP**: No direct type cast operator |

### Nice to Have
| Kotlin Feature              | Grapa Equivalent | Note |
|-----------------------------|------------------|------|
| `sealed class`              | —                | Advanced type, rarely needed |
| `enum class`                | —                | Advanced type, rarely needed |
| `inline fun`                | —                | Advanced meta-programming |
| `reified`                   | —                | Advanced meta-programming |
| `vararg`                    | —                | Advanced meta-programming |
| `tailrec fun`               | —                | Advanced meta-programming |
| `operator fun`              | —                | Advanced meta-programming |
| `infix fun`                 | —                | Advanced meta-programming |
| `extension fun`             | —                | Advanced meta-programming |
| `@Annotation`               | —                | Annotation, rarely needed |
| `inheritance`               | `class (ParentClass)` | Grapa supports inheritance |
| `method overriding`         | —                | Use regular method definitions |
| `object/class composition`  | `class {...}`/object | Grapa supports composition |

### Rarely Used
| Kotlin Feature              | Grapa Equivalent | Note |
|-----------------------------|------------------|------|
| `lateinit var x: Int`       | —                | Rarely used, not blocking |
| `companion object`          | —                | Rarely used, not blocking |

## Work-in-Progress (WIP) Items

Some Kotlin idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Module system**: `import kotlin.math.*` - No module system
- **Annotations**: `@file:JvmName("Foo")` - No annotation system
- **Coroutines**: `coroutineScope { ... }` - No coroutine system
- **Finally/ensure**: `finally` - No finally mechanism
- **Null assertion**: `x!!` - No null assertion operator
- **Safe call**: `x?.foo()` - No safe call operator
- **Type checking**: `is` - No type checking operator
- **Type casting**: `as` - No type casting operator
- **Sealed classes**: `sealed class` - No sealed class system
- **Enum classes**: `enum class` - No enum class system
- **Inline functions**: `inline fun` - No inline function system
- **Reified generics**: `reified` - No reified generics
- **Varargs**: `vararg` - No vararg system
- **Tail recursion**: `tailrec fun` - No tail recursion optimization
- **Operator functions**: `operator fun` - No operator function system
- **Infix functions**: `infix fun` - No infix function system
- **Extension functions**: `extension fun` - No extension function system
- **Data classes**: `data class` - No data class system
- **Companion objects**: `companion object` - No companion object system

### Nice to Have
These would improve developer experience but aren't essential:

- **Late initialization**: `lateinit var` - Use regular variables
- **Property delegation**: - Use regular properties
- **Lazy properties**: `lazy` - Use regular properties
- **Observable properties**: - Use regular properties
- **Vetoable properties**: - Use regular properties
- **Property accessors**: - Use regular methods
- **Backing fields**: - Use regular fields
- **Property references**: - Use regular references
- **Function references**: `::foo` - Use `@"foo"` for variable references
- **Type references**: - Use regular types

### Rarely Used
These are advanced features that most developers won't miss:

- **Kotlin-specific features**: - Use Grapa equivalents
- **Kotlin coroutines**: - Use Grapa's threading
- **Kotlin flow**: - Use Grapa's event system
- **Kotlin serialization**: - Use Grapa's serialization
- **Kotlin reflection**: - Use Grapa's reflection
- **Kotlin DSL**: - Use Grapa's syntax
- **Kotlin multiplatform**: - Use Grapa's platform abstraction
- **Kotlin native**: - Use Grapa's compilation
- **Kotlin JS**: - Use Grapa's web support
- **Kotlin Android**: - Use Grapa's mobile support

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, Kotlin's coroutines are replaced by Grapa's built-in parallel processing with `.map()` and `.filter()`, and Kotlin's null safety is replaced by Grapa's explicit null checking with `.isnull()`.

## Customizing Grapa for Familiar Syntax

If you prefer Kotlin-style function calls, you can define your own `println()` function in Grapa:

```grapa
println = op("value"=""){value.echo();};
println("Hello from Grapa!");
```

This can make migration easier for those used to Kotlin's `println()` or similar functions.

> **Advanced:** Grapa also allows advanced users to customize or extend the language syntax using `$RULE` or by modifying `$global` rules. This enables you to inject your own grammar or override built-in behaviors to match your preferred style. For most users, we recommend learning the canonical Grapa method syntax, but this flexibility is available if needed.

## Common Kotlin-to-Grapa Mappings

| Kotlin                        | Grapa Equivalent                                  |
|-------------------------------|---------------------------------------------------|
| `import kotlin.math.*`         | Built-in math functions (no import needed)        |
| `File("file.txt").writeText(d)`| `$file().set("file.txt", d)`                    |
| `thread { ... }`              | `$thread().start(op() { ... })`                   |
| `Thread.sleep(1000)`          | `$sys().sleep(1000)`                              |
| `System.getenv("HOME")`      | `$sys().getenv($HOME)`                            |
| `exitProcess(0)`              | `exit`                                            |
| `object : Foo {}`             | `obj : Foo {}`                                    |
| `fun foo(x: Int) = ...`       | `op(x){...}` or `op(x)("...")`                   |
| `val x = ...`                 | `x = ...`                                         |
| `var x = ...`                 | `x = ...`                                         |
| `@file:JvmName("Foo")`       | WIP                                               |
| `coroutineScope { ... }`      | WIP                                               |
| `this`                        | `self`                                            |
| `super`                       | `super`                                           |
| `when (x) { ... }`            | `switch(x) { ... }`                               |
| `x = if (cond) a else b`      | `x = (cond) ? a : b`                              |
| `try { ... } catch { ... }`   | `try { ... } catch (err) { ... }`                 |
| `finally`                     | WIP                                               |
| `::foo` (function ref)        | Not relevant (functions are data, pass directly) |
| `x!!` (non-null assert)       | `x.isnull() ? error : x` (explicit null check)    |
| `x?.foo()` (safe call)        | `x.isnull() ? null : x.foo()` (explicit null check) |
| `is` (type check)             | `text.type() == $STR` (type comparison)               |
| `as` (type cast)              | `x.str()`, `x.int()`, `x.float()` (explicit conversion) |
| `data class`                  | `class ... {}` or objects/lists (everything is data in Grapa) |
| `companion object`            | `$global` or `$root` for static/global            |
| `lateinit var`                | Regular variables (assign when ready, no special syntax) |

### Type Checking Examples

Grapa provides type checking using `.type()` method and type constants:

```grapa
/* Kotlin: if (value is String) { ... } */
/* Grapa equivalent: */
if (value.type() == $STR) {
    "Value is a string".echo();
}

/* Kotlin: when (value) { is String -> ... is Int -> ... } */
/* Grapa equivalent: */
switch (value.type()) {
    case $STR: "String value".echo();
    case $INT: "Integer value".echo();
    case $FLOAT: "Float value".echo();
    case $ARRAY: "Array value".echo();
    case $LIST: "List value".echo();
    default: "Unknown type".echo();
}

/* Check multiple types */
if (value.type() == $STR || value.type() == $INT) {
    "Value is string or integer".echo();
}
```

Common type constants: `$STR`, `$INT`, `$FLOAT`, `$BOOL`, `$ARRAY`, `$LIST`, `$TABLE`, `$TIME`, `$ERR`

## Notes
- See [Operators](../operators/) and [System](../sys/) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions. 

### Custom matches() Function for Regex

Kotlin users often use `str.matches` for regex checks. You can define a similar function in Grapa:

```grapa
// Define a matches function that returns true if the pattern is found
matches = op("text"="", "pattern"="") {
    text.grep(pattern, "x").len() > 0;
};

// Usage
if (matches("hello world", "world")) {
    "Found!".echo();
} else {
    "Not found.".echo();
}
```

This is a handy workaround until Grapa adds a native `.match()` method.

**Note:** While not as concise as Kotlin's operator syntax, this approach is explicit and clear about what operations are being performed.

### Late Initialization Examples

Grapa doesn't need special `lateinit` syntax because variables can be assigned at any time:

```grapa
/* Kotlin: lateinit var name: String */
/* Grapa equivalent: Just declare and assign later */

MyClass = class {
    name = "";  /* Can be empty initially */
    
    /* Initialize when ready */
    init = op() {
        name = "John";  /* Assign when convenient */
    };
    
    /* Or assign conditionally */
    setup = op(condition) {
        if (condition) {
            name = "Active User";
        } else {
            name = "Inactive User";
        }
    };
};

/* Usage */
obj = obj MyClass;
obj.init();  /* name is now "John" */
```

**Key differences:**
- **No special syntax** - Just use regular variables
- **No null safety concerns** - Grapa's dynamic typing handles this naturally
- **Flexible assignment** - Assign whenever convenient
- **No compile-time constraints** - Runtime assignment is always possible