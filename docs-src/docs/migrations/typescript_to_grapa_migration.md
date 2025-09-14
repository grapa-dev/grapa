# TypeScript-to-Grapa Migration Guide

> **Important: Access Patterns for .get() and Indexing (Tested, v0.0.39)**
>
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|
> | $ARRAY    |      ✗      |     ✗      |       ✓         |      —       |
> | $GOBJ     |      ✗      |     ✗      |       ✓         |     ✓       |
> | $file     |      ✓      |     ✗      |        —         |      —       |
> | $TABLE    |     ✓*      |     ✗      |        —         |      —       |
> | $OBJ      |      ✗      |     ✗      |       ✗         |     ✓       |
> *$TABLE .get() requires two arguments: key and field.
>
> - For $GOBJ and $OBJ, use bracket or dot notation (e.g., obj["key"], obj.key, obj[2]).
> - For $ARRAY, use bracket notation (e.g., arr[1]).
> - Only $file and $TABLE support .get().
> - This is based on direct testing in Grapa v0.0.39.

---

This guide helps TypeScript users transition to Grapa by mapping common TypeScript idioms, patterns, and code to their Grapa equivalents.

> **See Also:**
> - [Basic Syntax Guide](../syntax/basic_syntax.md)
> - [Operator Reference](../syntax/operator.md)

## Syntax Mapping Table

| TypeScript | Grapa |
|------------|-------|
| `let x: number = 5;` | `x = 5;` |
| `x += 1;` | `x += 1;` |
| `let s: string = "hi";` | `s = "hi";` |
| `s += "!";` | `s += "!";` |
| `let arr: number[] = [1,2,3];` | `arr = [1, 2, 3];` |
| `arr[0]` | `arr[0]` |
| `let obj: {[key: string]: number} = {};` | `obj = {}` |
| `obj["key"]` | `obj["key"]`<br>`obj.key`<br>`obj."key"` |
| `const {a, b} = obj` | `$local ++= obj; a.echo(); b.echo();` |
| `for (let i=0; i<arr.length; i++)` | `for i in arr { ... }`<br>`i = 0; while (i < arr.len()) { ...; i += 1; }`<br>`arr.map(op(x) { ... })`<br>`(n).range(0,1).map(op(i) { ... })` |
| `if (cond) { ... } else { ... }` | `if (cond) { ... } else { ... }` |
| `function f(x: number): number { ... }` | `f = op(x) { ... };` |
| `/* comment */` (block only, own line) | `/* comment */` (block)<br>`/** comment */` (doc block)<br>`// comment` (line)<br>`/// comment` (doc line) |
| `null/undefined` | `null` |
| `arr.map(f)` | `arr.map(op(x) { f(x); })` |
| `arr.filter(f)` | `arr.filter(op(x) { f(x); })` |
| `arr.reduce(f, init)` | `arr.reduce(op(a, b) { f(a, b); }, init)` |
| `arr.length` | `arr.len()` |
| `x = x + 1` | `x = x + 1;`<br>`x += 1;` (preferred) |
| `s = s + "x"` | `s = s + "x";`<br>`s += "x";` (preferred) |

> **Note:** Both `x = x + 1;` and `x += 1;` (and `s = s + "x";` and `s += "x";`) are valid in Grapa. The `+=` form is idiomatic and preferred in most cases.
>
> **String Interpolation:** For combining strings and values, use `"Hello ${name}".interpolate()` instead of `"Hello " + name`. String interpolation is more powerful and less error-prone than concatenation. It also provides elegant solutions for complex method chaining: `"${'hello'.upper()} ${'world'.lower()}".interpolate()`.

> **Note:** `.getfield("key")` is for `$file` and `$TABLE`. `.get()/.set()` is for `$WIDGET`. For `$GOBJ`/`$OBJ`, use `obj["key"]`, `obj.key`, or `obj."key"`. For `$ARRAY`, use `arr[index]` (bracket notation only).

## Access Patterns: Objects, Lists, Arrays, Files, and Tables

Below are all valid ways to access elements in Grapa data structures. See the canonical [Basic Syntax Guide](../syntax/basic_syntax.md) for the latest tested rules.

### $GOBJ and $OBJ

```grapa
obj = {"a": 1, "b": 2, "c": 3};

value = obj["b"];      /* Returns 2 */
value = obj.key;        /* Returns value for key 'key' if present */
value = obj."b";       /* Returns 2 */

/* $GOBJ only: */
value = obj[1];         /* Returns 2 (by index) */
name = obj.getname(1);  /* Returns "b" (key name at index 1) */
```

- Dot notation (`obj.key`) and bracket notation (`obj["key"]`) are both valid for $GOBJ/$OBJ.
- `.get()` is NOT valid for $GOBJ/$OBJ.

### $ARRAY

```grapa
arr = [10, 20, 30];

value = arr[1];         /* Returns 20 */
/* Note: .get(index) is not supported for arrays - use bracket notation */
```

- Use bracket notation or `.get(index)` for $ARRAY.
- Dot notation and `.get("key")` are NOT valid for $ARRAY.

### $file

```grapa
files = $file().ls();
file_info = files.get(0);   /* Correct */
```

- Always use `.get(index)` for $file results.
- Bracket and dot notation are NOT valid for $file.

### $TABLE

```grapa
table = {}.table("ROW");
table.mkfield("name", "STR", "VAR");
table.set("user1", "Alice", "name");

value = table.get("user1", "name");   /* Correct */
```

- Always use `.get(key, field)` for $TABLE.
- Bracket and dot notation are NOT valid for $TABLE.

> **Reference Table:**
> | Type      | .get("key") | .get(index) | Bracket Notation | Dot Notation |
> |-----------|:-----------:|:-----------:|:----------------:|:------------:|
> | $ARRAY    |      ✗      |     ✗      |       ✓         |      —       |
> | $GOBJ     |      ✗      |     ✗      |       ✓         |     ✓       |
> | $file     |      ✓      |     ✗      |        —         |      —       |
> | $TABLE    |     ✓*      |     ✗      |        —         |      —       |
> | $OBJ      |      ✗      |     ✗      |       ✗         |     ✓       |
> *$TABLE .get() requires two arguments: key and field.
>
> *$TABLE .getfield() requires two arguments: key and field.
>
> **Key Findings:**
> - **Arrays (`[]`)**: Use `array[index]` and `array.len()` for access and length
> - **Lists (`{}`)**: Use `list[key]` or `list.key` for access, `list.len()` for length
> - **Objects (class)**: Use `object.property` or `object[key]` for access
> - **`.getfield()/.setfield()` method**: Use for `$file` and `$TABLE` types
> - **`.get()/.set()` method**: Exclusively for `$WIDGET` types
> - **`.size()` method**: Not supported on any type (use `.len()` instead)
> - **`.keys()` method**: Not supported on `$GOBJ` (use iteration instead)

See [Basic Syntax Guide](../syntax/basic_syntax.md) for empirical test results and future updates.

## Common Pitfalls
- ✅ **For loops now supported** - Use `for x in arr { ... }` for native iteration
- No `.push()`/`.pop()`—use `+=` and manual index for pop
- ✅ **Line comments now supported** - Use `// comment` or `/// doc comment` in addition to block comments
- No implicit truthy/falsy—use explicit boolean checks
- All statements and blocks must end with a semicolon (`;`)
- Use `.map()`, `.reduce()`, `.filter()` as methods, not global functions
- Use `.range()` for sequence generation instead of manual while loops
- Use `.range()` with `.reduce()` for for-loop-like accumulation or collection tasks
- Use `.range().map()` and `.range().filter()` for parallel sequence generation and filtering. For large arrays, always specify a thread count to avoid too many threads
- Use `.iferr()` for simple error fallback; use `if (result.type() == $ERR)` only for explicit error handling

## Example Code Pairs

**TypeScript:**
```ts
// Sum squares of even numbers
let result = arr.filter(x => x % 2 === 0).map(x => x * x).reduce((a, b) => a + b, 0);
```
**Grapa:**
```grapa
result = arr.filter(op(x) { x % 2 == 0; }).map(op(x) { x * x; }).reduce(op(a, b) { a + b; }, 0);
```

**TypeScript:**
```ts
// Read file lines
let lines = fs.readFileSync('file.txt', 'utf-8').split('\n');
```
**Grapa:**
```grapa
lines = $file().read("file.txt").split("\n");
```

**TypeScript:**
```ts
// Object access
let value = obj["key"];
```
**Grapa:**
```grapa
value = obj["key"];
value = obj.key;
value = obj."key";
```

**TypeScript:**
```ts
// File access
let value = file["key"];
```
**Grapa:**
```grapa
value = file.get("key");
```

**TypeScript:**
```ts
// Generate numbers 0..9
let seq = Array.from({length: 10}, (_, i) => i);
```
**Grapa:**
```grapa
seq = (10).range(0,1);
```

**TypeScript:**
```ts
// Sum numbers 0..9
let sum = Array.from({length: 10}, (_, i) => i).reduce((a, x) => a + x, 0);
```
**Grapa:**
```grapa
sum = (10).range(0,1).reduce(op(acc, x) { acc += x; }, 0);
```

**TypeScript:**
```ts
// Collect even numbers 0..9
let evens = Array.from({length: 10}, (_, i) => i).filter(x => x % 2 === 0);
```
**Grapa:**
```grapa
evens = (10).range(0,1).filter(op(x) { x % 2 == 0; });
```

**TypeScript:**
```ts
// Error fallback
let result = someOperation() ?? 0;
```
**Grapa:**
```grapa
result = someOperation().iferr(0);
```

> **Warning:** `.map()` and `.filter()` are parallel by default. For large arrays, specify a thread count:
> ```grapa
> big = (1000000).range(0,1).map(op(x) { x * x; }, 8);  // Limit to 8 threads
> ```

## Notes
- See [Operators](../syntax/operator.md) and [System](../sys/sys.md) docs for more details.
- Grapa supports variable scoping with `$global`, `$local`, and `$root`.
- Use `$sys().getenv()` for environment variables.
- Use `$thread()` for threading and `$sys().sleep()` for sleep.
- Use `op(){}` for lambdas and function definitions.

## See Also
- [Basic Syntax Guide](../syntax/basic_syntax.md)
- [Operator Reference](../syntax/operator.md)

---

*If you have more TypeScript idioms you want mapped to Grapa, please open an issue or PR!* 

### Custom match() Function for Regex

TypeScript users often use `str.match()` for regex checks. You can define a similar function in Grapa:

```grapa
// Define a match function that returns true if the pattern is found
match = op("text"="", "pattern"="") {
    text.grep(pattern, "x").len() > 0;
};

// Usage
if (match("hello world", "world")) {
    "Found!".echo();
} else {
    "Not found.".echo();
}
```

This is a handy workaround until Grapa adds a native `.match()` method.

## Debugging and Logging

Grapa provides sophisticated debugging capabilities that go beyond TypeScript's console methods. While Grapa doesn't have `__FILE__` and `__LINE__` macros, it offers superior debugging through the `.debug()` method with component targeting and level control:

### Basic Debug Output
```grapa
/* Standard output - equivalent to console.log() */
"Hello World".echo();

/* Debug output with component targeting */
"Debug message".debug(1, "component");

/* Debug with different levels */
"Info message".debug(1, "info");
"Warning message".debug(2, "warning");
"Error message".debug(3, "error");
```

### Enabling Debug Mode
```grapa
/* Enable debug mode for current session */
$sys().putenv("GRAPA_SESSION_DEBUG_MODE", "1");
$sys().putenv("GRAPA_SESSION_DEBUG_LEVEL", "2");

/* Enable debug for specific components */
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "database,grep,vector");

/* Enable all components at different levels */
$sys().putenv("GRAPA_SESSION_DEBUG_COMPONENTS", "grep:3,database:1,*:0");
```

### Command Line Debug Options
```bash
# Enable debug mode from command line
./grapa -d script.grc

# Enable debug with specific components
GRAPA_SESSION_DEBUG_COMPONENTS="debug:1" ./grapa -d script.grc

# Enable debug with multiple components
GRAPA_SESSION_DEBUG_COMPONENTS="database:2,grep:1,vector:0" ./grapa -d script.grc
```

### Debug vs Console Comparison

| TypeScript | Grapa |
|------------|-------|
| `console.log("Hello")` | `"Hello".echo()` |
| `console.debug("Debug info")` | `"Debug info".debug(1, "debug")` |
| `console.info("Info message")` | `"Info message".debug(1, "info")` |
| `console.warn("Warning")` | `"Warning".debug(2, "warning")` |
| `console.error("Error")` | `"Error".debug(3, "error")` |
| `console.log(\`Count: \${count}\`)` | `"Count: ${count}".interpolate().echo()` |
| `console.log(\`Processing \${i}/\${total}\`)` | `"Processing ${i}/${total}".interpolate().debug(1, "progress")` |

### Advanced Debug Patterns
```grapa
/* Debug with interpolation for clean output */
"Processing ${record_count} records...".debug(1, "process");

/* Debug with error handling */
result = some_operation();
if (result.type() == $ERR) {
    "Error occurred: ${result}".debug(3, "error");
} else {
    "Operation successful: ${result}".debug(1, "info");
};

/* Debug with component-specific formatting */
"Database query: ${query}".debug(2, "database");
"Network request: ${url}".debug(2, "network");
"File operation: ${filename}".debug(2, "filesystem");
```

> **Clarification on .get() Usage:**
> - `.get()` is **required** for `$file` and `$TABLE` access.
> - `.get()` is **not supported** for `$ARRAY`, `$GOBJ`, or `$OBJ` as of this writing.
> - Use bracket and dot notation for `$ARRAY`, `$GOBJ`, and `$OBJ`.
> - If more objects support `.get()` in the future, this guide will be updated. 

> **Comment Style:**
> - ✅ **Comprehensive comment support**: `/* */` (block), `/** */` (doc block), `//` (line), `///` (doc line)
> - Comments work everywhere including at end of lines and inside code blocks 

## Work-in-Progress (WIP) Items

Some TypeScript idioms don't have direct Grapa equivalents yet. These are categorized by priority:

### Core Gaps (True Language Gaps)
These represent fundamental language features that genuinely cannot be accomplished in Grapa:

- **Static typing**: `let x: number` - Grapa uses dynamic typing by design (see note below)
- **Type aliases**: `type MyType = string` - No type alias system
- **Union types**: `string | number` - No union type system
- **Intersection types**: `A & B` - No intersection type system
- **Literal types**: `"hello"` - No literal type system
- **Template literal types**: `` `Hello ${string}` `` - No template literal types (but see [String Templates and Dynamic Construction](../type/str.md#string-templates-and-dynamic-construction) for runtime templates)
- **Conditional types**: `T extends U ? X : Y` - No conditional type system
- **Mapped types**: `{ [K in keyof T]: T[K] }` - No mapped type system
- **Utility types**: `Partial<T>`, `Required<T>` - No utility type system
- **Namespace**: `namespace MyNamespace` - Grapa has dynamic namespaces (see note below)
- **Declaration merging**: - No declaration merging
- **Const assertions**: `as const` - Grapa has superior runtime const protection (see note below)

> **Important Note on Dynamic Typing:**
> Grapa's dynamic typing is a **fundamental design choice**, not a limitation. It enables Grapa's core strengths:
> - **Dynamic code execution** and meta-programming capabilities
> - **Runtime type introspection** with `.type()` method
> - **Flexible data processing** without compile-time type constraints
> - **System integration** that doesn't require type definitions
> 
> Grapa provides type safety through runtime checking and rich type introspection, which is often more flexible than static typing for data processing and system integration tasks.
> 
> **Why Grapa Doesn't Need Static Type Annotations:**
> Grapa's design philosophy prioritizes **runtime flexibility** over compile-time guarantees. Static type annotations would fundamentally change Grapa's nature and eliminate its core advantages:
> - **Meta-programming**: Grapa can modify its own code structure at runtime, which static typing would prevent
> - **Dynamic grammar changes**: Grapa's executable BNF system requires runtime type flexibility
> - **System integration**: Grapa can work with any data format without predefined type definitions
> - **Data processing**: Grapa's strength is handling diverse, changing data structures dynamically
> 
> For type safety, Grapa provides superior runtime mechanisms:
> ```grapa
> /* Runtime type checking */
> if (value.type() == $INT) {
>     /* Handle integer */
> } else if (value.type() == $STR) {
>     /* Handle string */
> }
> 
> /* Safe property access */
> result = obj.prop.iferr(null);  /* Returns null if property doesn't exist */
> 
> /* Type conversion with validation */
> if (input.type() == $STR) {
>     number = input.int();  /* Safe conversion */
> }
> ```

> **Important Note on Namespaces:**
> Grapa has a **dynamic namespace system** that's more flexible than TypeScript's static namespaces:
> - **$global namespace**: Global variables accessible everywhere
> - **$local namespace**: Function-local variables (automatic)
> - **Dynamic namespace creation**: Namespaces are created automatically for each execution context
> - **No explicit namespace declarations**: Namespaces are managed automatically by the execution tree
> - **Hierarchical access**: Variables can be accessed from parent namespaces

> **Important Note on Const Protection:**
> Grapa's `const` operator provides **superior runtime protection** compared to TypeScript's `as const`:
> - **Runtime modification prevention**: Grapa's const prevents actual modification at runtime
> - **Dynamic control**: Can toggle const on/off with `.setconst(false/true)`
> - **Performance optimization**: Designed for caching and optimization
> - **Thread safety**: Prevents blocking on access for concurrent threads
> - **No static type narrowing**: Grapa uses dynamic typing, so no compile-time literal type inference

### Nice to Have
These would improve developer experience but aren't essential:

- **Nullish coalescing**: `x ?? y` - Use `.ifnull()` for superior nullish coalescing: `x.ifnull(y)`
- **Interfaces**: `interface MyInterface` - Use object composition and duck typing
- **Generics**: `<T>` - Grapa has dynamic typing
- **Type guards**: `x is string` - Use `.type()` checks
- **Type assertions**: `x as string` - Use explicit conversion
- **Modules**: `import/export` - Use Grapa's include system and dynamic class loading
- **Decorators**: `@decorator` - Use function composition
- **Enums**: `enum MyEnum` - Use objects with constants
- **Class syntax**: `class MyClass` - Use `class MyClass {}` with `$new()` constructor or manual initialization
- **Access modifiers**: `private`, `public` - Use naming conventions
- **Abstract classes**: `abstract class` - Use regular classes
- **Method overloading**: - Use different function names
- **Index signatures**: `[key: string]: any`

### Property Access and Safe Navigation

**Why Grapa Doesn't Need Optional Chaining and Property Operators:**
Grapa's property access mechanisms are superior to TypeScript's optional chaining and property operators:

- **Optional Chaining (`?.`)**: Use `.iferr()` for superior safe property access with custom fallback values
- **Property Existence (`in`)**: Use `.type() != $ERR` for more explicit existence checking
- **Property Enumeration (`for...in`)**: Use manual key iteration for more controlled enumeration

**Grapa's Superior Property Access:**
```grapa
/* Optional chaining equivalent - BETTER */
/* TypeScript: obj?.user?.profile?.name */
name = obj.user.iferr(null).profile.iferr(null).name.iferr("default");

/* Property existence equivalent - MORE EXPLICIT */
/* TypeScript: "name" in user */
if (user.name.type() != $ERR) {
    /* Property exists and is accessible */
}

/* Property enumeration equivalent - MORE CONTROLLED */
/* TypeScript: for (let key in obj) */
keys = ["name", "age", "city"];
keys.map(op(key) {
    value = user[key].iferr("not set");
    (key + ": " + value.str()).echo();
});
```

**Advantages of Grapa's Approach:**
- **Custom fallback values** at each step (not just `undefined`)
- **Explicit existence checking** with `.type()` method
- **Controlled enumeration** without prototype chain issues
- **Functional programming patterns** over imperative loops
- **Better error handling** with explicit fallback values

### Data Structures and Collections

**Why Grapa Doesn't Need Specialized Collections:**
Grapa's unified data structure approach is superior to specialized collections:

- **Sets**: Use `.unique()` method on arrays: `[1, 2, 1, 3, 2].unique()` → `[1, 2, 3]`
- **Maps**: Use `$GOBJ` objects: `{key1: "value1", key2: "value2"}`
- **Priority Queues**: Use `.sort()` with custom comparators
- **Deques**: Use array operations with `+=` and `++=`
- **Iterators**: Use functional methods (`.map()`, `.filter()`, `.reduce()`) which are thread-safe and parallel

**Grapa's Superior Collection Capabilities:**
```grapa
/* Set-like behavior */
set_like = [1, 2, 1, 3, 2].unique();  /* [1, 2, 3] */

/* Map-like behavior */
map_like = {key1: "value1", key2: "value2"};

/* Priority queue with custom sorting */
priority_queue = [{priority: 3, data: "low"}, {priority: 1, data: "high"}];
sorted_queue = priority_queue.sort(0, 0, op(a, b) { a.priority <=> b.priority; });

/* Deque operations */
deque = [1, 2, 3];
deque += 4;        /* Add to end */
deque += 0 deque[0]; /* Add to beginning */

/* Parallel functional operations */
squares = numbers.map(op(x) { x * x });           /* Parallel processing */
evens = numbers.filter(op(x) { x % 2 == 0 });     /* Thread-safe */
sum = numbers.reduce(op(a, b) { a + b }, 0);      /* Sequential reduction */
```

**Advantages of Grapa's Approach:**
- **Unified syntax** across all data types
- **Parallel processing** built into functional methods
- **Cross-format compatibility** (works on `$ARRAY`, `$GOBJ`, `$OBJ`, `$XML`, etc.)
- **Simpler learning curve** - fewer specialized types to learn
- **Better performance** - optimized for Grapa's execution model

### Access Control and Visibility

**TypeScript:**
```typescript
class Example {
    public publicVar: string = "public";
    protected protectedVar: string = "protected";
    private privateVar: string = "private";
    
    public publicMethod(): string {
        return $this.publicVar;
    }
    
    protected protectedMethod(): string {
        return $this.protectedVar;
    }
    
    private privateMethod(): string {
        return $this.privateVar;
    }
}
```

**Grapa:**
```grapa
/* Grapa doesn't need access modifiers - all properties are accessible */
/* This aligns with Grapa's late-binding, dynamic philosophy */
example = {
    publicVar: "public",
    protectedVar: "protected", 
    privateVar: "private"
};

/* All properties are accessible - Grapa trusts developers */
example.publicVar.echo();      /* "public" */
example.protectedVar.echo();   /* "protected" */
example.privateVar.echo();     /* "private" */
```

**Note:** Grapa intentionally omits access control modifiers. This aligns with its late-binding philosophy where all properties are accessible, promoting transparency and reducing complexity.

### Meta-programming and Code Generation

**TypeScript:**
```typescript
// eval() for dynamic code execution (not recommended in TypeScript)
const code = "console.log('Hello, World!')";
eval(code);

// Function constructor
const dynamicFunc = new Function('x', 'return x * 2');

// Dynamic property access
const obj: any = { a: 1, b: 2 };
const key = 'a';
console.log(obj[key]);
```

**Grapa:**
```grapa
/* Grapa has superior built-in meta-programming capabilities */
/* Dynamic code execution via $sys().eval() */
code = "('Hello, World!').echo();";
$sys().eval(code);

/* Dynamic function creation via op() */
dynamicFunc = op(@<x,{x * 2}>);
result = dynamicFunc(21);  /* 42 */

/* Dynamic property access */
obj = {a: 1, b: 2};
key = "a";
obj[key].echo();  /* 1 */

/* Rule-based code generation */
rule = @<pattern,{@<action,{result}>}>;
generated_code = rule("input");
```

**Note:** Grapa's execution tree architecture provides superior meta-programming capabilities compared to traditional eval() approaches. The language is designed around dynamic code generation and manipulation.

### Guard Statements and Early Returns

**TypeScript:**
```typescript
function processData(data: any[] | null): any[] | null {
    if (!data) {
        return null;
    }
    
    if (data.length === 0) {
        return [];
    }
    
    // Process data...
    return processedResult;
}
```

**Grapa:**
```grapa
/* Grapa uses existing control flow for guard patterns */
processData = @<data,{
    iferr(data, return(null));
    ifnull(data, return(null));
    
    if(data.len() == 0, return([]));
    
    /* Process data... */
    return(processedResult);
}>;
```

**Note:** Guard statements are just coding patterns using existing control flow. Grapa's `iferr()`, `ifnull()`, `if()`, and `return()` provide all the functionality needed for guard patterns without requiring special syntax.

### Decorators and Function Composition

**TypeScript:**
```typescript
// Decorator function
function validate(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    descriptor.value = function(...args: any[]) {
        if (args[0] > 0) {
            return originalMethod.apply(this, args);
        } else {
            throw new Error("Invalid input");
        }
    };
    return descriptor;
}

class Example {
    @validate
    processData(data: number): number {
        return data * 2;
    }
}
```

**Grapa:**
```grapa
/* Grapa uses explicit function composition instead of decorators */
validate = op(f) { op(x) { if (x > 0) f(x); else $ERR("Invalid input"); }; };

processData = op(data) { data * 2; };

/* Compose functions explicitly */
validatedProcess = validate(processData);
result = validatedProcess(5);
```

**Note:** Grapa intentionally omits decorators in favor of explicit function composition. This approach is more transparent, flexible, and aligns with Grapa's late-binding philosophy. You can see exactly what's happening and compose functions dynamically.

### Generics and Type Abstraction

**TypeScript:**
```typescript
// Generic functions with type parameters
function sortArray<T>(items: T[]): T[] {
    return items.sort();
}

function filterArray<T>(items: T[], predicate: (item: T) => boolean): T[] {
    return items.filter(predicate);
}

// Usage with different types
const numbers: number[] = [3, 1, 4, 1, 5];
const strings: string[] = ["banana", "apple", "cherry"];

const sortedNumbers = sortArray(numbers);
const sortedStrings = sortArray(strings);
```

**Grapa:**
```grapa
/* Grapa handles type complexity in C++ libraries, not in scripts */
/* All methods work on any type automatically */

/* Sorting works on any array type */
numbers = [3, 1, 4, 1, 5];
sortedNumbers = numbers.sort();

strings = ["banana", "apple", "cherry"];
sortedStrings = strings.sort();

/* Filtering works on any type */
evens = numbers.filter(op(x) { x % 2 == 0; });
longStrings = strings.filter(op(s) { s.len() > 5; });

/* Mapping works on any type */
doubled = numbers.map(op(x) { x * 2; });
uppercase = strings.map(op(s) { s.upper(); });
```

**Note:** Grapa abstracts type complexity into C++ libraries rather than exposing generics in scripts. This approach is simpler, more performant, and leverages Grapa's dynamic typing strengths. The same methods work on all types without explicit type parameters.