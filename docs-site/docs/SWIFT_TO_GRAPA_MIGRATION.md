# Swift-to-Grapa Migration Guide

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

## Swift-to-Grapa Migration Guide (WIP Features)

> **Note:** All core language gaps are tracked internally for future development.

### Main Table: Core Gaps (Tracked for Enhancement)
| Swift Feature               | Grapa Equivalent | Status/Note |
|-----------------------------|------------------|-------------|
| `import Foundation`         | —                | **GAP**: No module/import system |
| `protocol Baz {}`           | —                | **GAP**: No protocol/interface |
| `extension Foo {}`          | —                | **GAP**: No extension support |
| `defer`                     | —                | **GAP**: No defer/finally |
| `#file`                     | —                | **GAP**: No file macro |
| `#line`                     | —                | **GAP**: No line macro |
| `typealias Foo = Bar`       | —                | **GAP**: No typealias |
| `Any`                       | —                | **GAP**: No Any/advanced type |
| `as?` (optional cast)       | —                | **GAP**: No optional cast |
| `is` (type check)           | —                | **GAP**: No direct type check |
| `guard`                     | —                | **GAP**: No guard statement |
| `@escaping`                 | —                | **GAP**: No annotation/meta-programming |
| `@objc`                     | —                | **GAP**: No annotation/meta-programming |

### Nice to Have
| Swift Feature               | Grapa Equivalent | Note |
|-----------------------------|------------------|------|
| `lazy var x: Int = ...`     | —                | Rarely needed, not blocking |
| `var x: Int? = nil`         | —                | Rarely needed, not blocking |
| `x ?? 0`                    | —                | Rarely needed, not blocking |
| `x?.doSomething()`          | —                | Rarely needed, not blocking |
| `x!`                        | —                | Rarely needed, not blocking |
| `guard let x = y else { ... }` | —             | Rarely needed, not blocking |
| `#available(iOS 13, *)`     | —                | Rarely needed, not blocking |
| `@available(iOS 13, *)`     | —                | Rarely needed, not blocking |
| `struct Bar {}`             | `obj Bar {}`     | Use object constructors |
| `class Foo {}`              | `class Foo {}`   | Use object constructors |
| `inheritance`               | `class (ParentClass)` | Grapa supports inheritance |
| `method overriding`         | —                | Use regular method definitions |
| `method overloading`        | —                | Use different function names |
| `default parameters`        | —                | Use explicit checks |
| `variadic parameters`       | —                | Use array arguments |
| `inout parameters`          | —                | Use regular parameters |
| `autoclosure`               | —                | Use regular closures |
| `escaping closures`         | —                | Use regular closures |

### Rarely Used
| Swift Feature               | Grapa Equivalent | Note |
|-----------------------------|------------------|------|
| `func foo<T>(x: T) -> T`    | —                | Generics, rarely needed |
| `inout`                     | —                | Rarely needed |
| `@autoclosure`              | —                | Rarely needed |

## Work-in-Progress (WIP) Items

Some Swift idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Module system**: `import Foundation` - No module system
- **Protocols**: `protocol Baz {}` - No protocol system
- **Extensions**: `extension Foo {}` - No extension system
- **Defer**: `defer` - No defer mechanism
- **File/line macros**: `#file`, `#line` - No file/line macros
- **Type aliases**: `typealias Foo = Bar` - No type alias system
- **Any type**: `Any` - No Any type system
- **Optional casting**: `as?` - No optional casting
- **Type checking**: `is` - No type checking operator
- **Guard statements**: `guard` - No guard statement
- **Annotations**: `@escaping`, `@objc` - No annotation system
- **Generics**: `<T>` - No generic system
- **Optionals**: `Int?` - No optional type system
- **Optional chaining**: `x?.doSomething()` - No optional chaining
- **Force unwrapping**: `x!` - No force unwrapping
- **Nil coalescing**: `x ?? 0` - No nil coalescing
- **Lazy properties**: `lazy var` - No lazy property system
- **Computed properties**: - No computed property system
- **Property observers**: - No property observer system
- **Access control**: `private`, `public` - No access control system

### Nice to Have
These would improve developer experience but aren't essential:

- **Structs**: `struct Bar {}` - Use object constructors
- **Classes**: `class Foo {}` - Use object constructors
- **Inheritance**: - Use object composition
- **Method overriding**: - Use regular method definitions
- **Method overloading**: - Use different function names
- **Default parameters**: - Use explicit checks
- **Variadic parameters**: - Use array arguments
- **Inout parameters**: `inout` - Use regular parameters
- **Autoclosure**: `@autoclosure` - Use regular closures
- **Escaping closures**: `@escaping` - Use regular closures

### Rarely Used
These are advanced features that most developers won't miss:

- **Swift-specific features**: - Use Grapa equivalents
- **SwiftUI**: - Use Grapa's UI system
- **Combine**: - Use Grapa's event system
- **Core Data**: - Use Grapa's data system
- **Foundation**: - Use Grapa's built-in functions
- **UIKit**: - Use Grapa's UI system
- **AppKit**: - Use Grapa's UI system
- **Swift Package Manager**: - Use Grapa's dependency management
- **CocoaPods**: - Use Grapa's dependency management
- **Carthage**: - Use Grapa's dependency management

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, Swift's optionals are replaced by Grapa's explicit null checking, and Swift's protocols are replaced by Grapa's object composition and duck typing.

## Common Swift-to-Grapa Mappings

| Swift                        | Grapa Equivalent                                  |
|------------------------------|---------------------------------------------------|
| `FileManager.default.createFile(...)` | `$file().set(...)`                       |
| `Thread.sleep(forTimeInterval: 1)` | `$sys().sleep(1000)`                        |
| `ProcessInfo.processInfo.environment["HOME"]` | `$sys().getenv($HOME)`           |
| `exit(0)`                    | `exit`                                            |
| `class Foo {}`               | `class Foo {}`                                    |
| `struct Bar {}`              | `obj Bar {}`                                      |
| `func foo(x: Int) -> Int`    | `op(x){...}` or `op(x)("...")`                   |
| `let x = ...`                | `x = ...`                                         |
| `var x = ...`                | `x = ...`                                         |
| `self`                       | `self`                                            |
| `super`                      | `super`                                           |
| `switch x { ... }`           | `switch(x) { ... }`                               |
| `x = condition ? a : b`      | `x = (condition) ? a : b`                         |
| `try { ... } catch { ... }`  | `try { ... } catch (err) { ... }`                 |
| `@escaping`                  | WIP                                               |
| `@objc`                      | WIP                                               |

## Notes
- See [Operators](../operators/) and [System](../sys/) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `@x` to reference a variable by name.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions. 