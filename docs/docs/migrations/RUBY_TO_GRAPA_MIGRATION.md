# Ruby-to-Grapa Migration Guide

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

## Common Ruby-to-Grapa Mappings

| Ruby                        | Grapa Equivalent                                  |
|-----------------------------|---------------------------------------------------|
| `require 'json'`            | `$file().get("file.json").decode("JSON")`        |
| `File.write('file.txt', d)` | `$file().set("file.txt", d)`                      |
| `raise`                     | `exit`                                            |
| `Thread.new { ... }`        | `$thread().start(op() { ... })`                   |
| `sleep 1`                   | `$sys().sleep(1000)`                              |
| `ENV['HOME']`               | `$sys().getenv($HOME)`                            |
| `system('ls')`              | `$file().ls()`                                    |
| `exit`                      | `exit`                                            |
| `class Foo; end`            | `class Foo {}`                                    |
| `module Bar; end`           | WIP                                               |
| `include Bar`               | WIP                                               |
| `attr_accessor :x`          | `obj.x` (direct property access)                  |
| `@x`                        | `@x` (reference variable by name)                 |
| `@@x`                       | `$global.x` or `$root.x` (global/root scope)      |
| `$x`                        | `$global.x`                                       |
| `:symbol`                   | WIP                                               |
| `lambda { |x| ... }`        | `op(x){...}` or `op(x)("...")`                   |
| `proc { |x| ... }`          | `op(x){...}`                                      |
| `method(:foo)`              | `obj.getname()` (partial reflection)              |
| `send(:foo)`                | WIP                                               |
| `respond_to?(:foo)`         | WIP                                               |
| `Object.new`                | `obj {}`                                          |
| `self`                      | `self`                                            |
| `super`                     | `super`                                           |
| `yield`                     | WIP                                               |
| `block_given?`              | WIP                                               |
| `begin ... ensure ... end`  | WIP                                               |
| `alias :foo :bar`           | WIP                                               |
| `undef :foo`                | WIP                                               |
| `defined?(foo)`             | WIP                                               |
| `__FILE__`                  | WIP                                               |
| `__LINE__`                  | WIP                                               |

## Notes
- See [Operators](../operators/) and [System](../sys/) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `@x` to reference a variable by name.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions. 

## Ruby-to-Grapa Migration Guide (WIP Features)

### Main Table: Core Gaps (Tracked for Enhancement)
| Ruby Feature                | Grapa Equivalent | Status/Note |
|-----------------------------|------------------|-------------|
| `alias :foo :bar`           | —                | **GAP**: Meta-programming not yet supported |
| `undef :foo`                | —                | **GAP**: Meta-programming not yet supported |
| `defined?(foo)`             | —                | **GAP**: Meta-programming not yet supported |
| `__FILE__`                  | —                | **GAP**: No file macro |
| `__LINE__`                  | —                | **GAP**: No line macro |
| `yield`                     | —                | **GAP**: Block/yield not supported |
| `block_given?`              | —                | **GAP**: Block/yield not supported |
| `begin ... ensure ... end`  | —                | **GAP**: No ensure/finally |

### Nice to Have
| Ruby Feature                | Grapa Equivalent | Note |
|-----------------------------|------------------|------|
| `respond_to?(:foo)`         | —                | Reflection, rarely needed |
| `send(:foo)`                | —                | Reflection, rarely needed |
| `:symbol`                   | —                | Ruby-specific, not common |
| `class inheritance`         | `class (ParentClass)` | Grapa supports inheritance |
| `method overriding`         | —                | Use regular method definitions |
| `object/class composition`  | `class {...}`/object | Grapa supports composition |

### Rarely Used
| Ruby Feature                | Grapa Equivalent | Note |
|-----------------------------|------------------|------|
| `module Bar; end`           | —                | Ruby module system, not relevant |
| `include Bar`               | —                | Ruby module system, not relevant | 

## Work-in-Progress (WIP) Items

Some Ruby idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Meta-programming**: `alias`, `undef`, `defined?` - No meta-programming capabilities
- **File/line macros**: `__FILE__`, `__LINE__` - No file/line macros
- **Block/yield**: `yield`, `block_given?` - No block/yield mechanism
- **Ensure/finally**: `begin ... ensure ... end` - No ensure/finally mechanism
- **Symbols**: `:symbol` - No symbol type
- **Method reflection**: `respond_to?`, `send` - Limited reflection capabilities
- **Module system**: `module`, `include` - No module system
- **Mixins**: - No mixin system
- **Singleton classes**: - No singleton class system
- **Method missing**: - No method missing mechanism
- **Method aliasing**: - No method aliasing
- **Method undefining**: - No method undefining
- **Method redefinition**: - No method redefinition
- **Method visibility**: `private`, `protected` - No access control
- **Method arity**: - No method arity system
- **Method parameters**: - No method parameter system
- **Method defaults**: - No method default parameters
- **Method splat**: - No splat operator
- **Method keyword args**: - No keyword arguments

### Nice to Have
These would improve developer experience but aren't essential:

- **Class inheritance**: - Use object composition
- **Method overriding**: - Use regular method definitions
- **Method super**: - Use regular method calls
- **Method chaining**: - Use regular method calls
- **Method composition**: - Use function composition
- **Method currying**: - Use function composition
- **Method partial application**: - Use function composition
- **Method memoization**: - Use caching
- **Method caching**: - Use caching
- **Method optimization**: - Use Grapa's built-in optimization

### Rarely Used
These are advanced features that most developers won't miss:

- **Ruby-specific features**: - Use Grapa equivalents
- **Ruby gems**: - Use Grapa's library system
- **Ruby bundler**: - Use Grapa's dependency management
- **Ruby rake**: - Use Grapa's build system
- **Ruby irb**: - Use Grapa's REPL
- **Ruby pry**: - Use Grapa's debugging tools
- **Ruby rspec**: - Use Grapa's testing framework
- **Ruby minitest**: - Use Grapa's testing framework
- **Ruby cucumber**: - Use Grapa's testing framework
- **Ruby capybara**: - Use Grapa's testing framework

> **Note:** Many "missing" features are actually available in Grapa through different mechanisms. For example, Ruby's meta-programming is replaced by Grapa's built-in code generation, and Ruby's blocks are replaced by Grapa's function composition.

## Customizing Grapa for Familiar Syntax

If you prefer Ruby-style function calls, you can define your own `puts()` function in Grapa:

```grapa
# Define a puts function similar to Ruby
puts = op("value"=""){value.echo();};
puts("Hello from Grapa!");
```

This can make migration easier for those used to Ruby's `puts()` or similar functions.

> **Advanced:** Grapa also allows advanced users to customize or extend the language syntax using `$RULE` or by modifying `$global` rules. This enables you to inject your own grammar or override built-in behaviors to match your preferred style. For most users, we recommend learning the canonical Grapa method syntax, but this flexibility is available if needed.

## See Also
- [Basic Syntax Guide](syntax/basic_syntax.md)
- [Operator Reference](syntax/operator.md)

---

*If you have more Ruby idioms you want mapped to Grapa, please open an issue or PR!*

### Custom match() Function for Regex

Ruby users often use `str.match` for regex checks. You can define a similar function in Grapa:

```grapa
# Define a match function that returns true if the pattern is found
match = op("text"="", "pattern"="") {
    text.grep(pattern, "x").len() > 0;
};

# Usage
if (match("hello world", "world")) {
    "Found!".echo();
} else {
    "Not found.".echo();
}
```

This is a handy workaround until Grapa adds a native `.match()` method.