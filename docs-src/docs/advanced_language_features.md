# Advanced Language Features

References:
- [Basic Syntax](syntax/basic_syntax.md)
- [Functional Programming](api_reference.md#functional-programming)
- [Metaprogramming](advanced_topics.md)

This document covers advanced language features in Grapa, including both implemented features and planned enhancements.

## Overview

Grapa provides several advanced language features that go beyond basic syntax:

- **Executable BNF System** - Dynamic grammar modification and multi-syntax processing
- **Advanced Metaprogramming** - Human-readable execution trees and code generation
- **Functional Programming** - First-class functions and functional methods
- **Advanced Pattern Matching** - 100% ripgrep compatible with Unicode support
- **Superior Concurrency** - Parallel-by-design architecture

## Implemented Advanced Features

### **1. Executable BNF System**

Grapa's core innovation is its executable BNF system that enables dynamic language creation:

```grapa
/* Define custom syntax at runtime */
@global["$custom_syntax"] = rule <$expression> '+' <$expression> {@<add,{$1,$3}>};

/* Use custom syntax immediately */
result = 5 + 3;  /* Uses custom rule */
```

**Key Capabilities:**
- **Dynamic Grammar Definition** - Modify language syntax at runtime
- **Multi-Syntax Processing** - Handle JSON, XML, HTML, SQL natively
- **Domain-Specific Languages** - Create custom syntax for specific domains
- **Isolated Rule Execution** - Execute custom rules in isolated contexts

### **2. Advanced Metaprogramming**

Grapa provides superior metaprogramming through human-readable execution trees:

```grapa
/* Create and manipulate execution trees */
code = op(x) { x * 2; };
tree = code.plan();  /* Get execution tree */
result = tree();     /* Execute tree */
```

**Features:**
- **Human-Readable Trees** - Execution trees are inspectable and manipulable
- **Code Generation** - Generate code dynamically
- **Introspection** - Examine compiled code structure
- **Template Systems** - Advanced string interpolation with script execution

### **3. Functional Programming**

Grapa supports advanced functional programming patterns:

```grapa
/* Higher-order functions */
double = op(x) { x * 2; };
numbers = [1, 2, 3, 4, 5];
doubled = numbers.map(double);

/* Function composition */
compose = op(f, g) { op(x) { f(g(x)); }; };
add_one = op(x) { x + 1; };
double_and_add = compose(double, add_one);
```

**Features:**
- **First-Class Functions** - Functions as values
- **Lambda Expressions** - `op(x) { x * 2; }` syntax
- **Functional Methods** - `.map()`, `.filter()`, `.reduce()`
- **Parallel Processing** - Built-in parallel execution

### **4. Advanced Pattern Matching**

Grapa provides superior pattern matching capabilities:

```grapa
/* 100% ripgrep compatible pattern matching */
text = "Hello World 123";
matches = text.grep("\\d+");  /* Find digits */

/* Native boolean pattern matching */
has_digits = text.match("\\d+");  /* true/false */
```

**Features:**
- **PCRE2 Support** - Full regex compatibility
- **Unicode Pattern Matching** - Complete Unicode support
- **Binary Pattern Matching** - Byte-exact matching
- **Native `.match()` Method** - Boolean regex testing

### **5. Superior Concurrency**

Grapa's parallel-by-design architecture surpasses traditional async/await:

```grapa
/* Parallel processing with automatic thread management */
data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
processed = data.map(op(x) { x * x; }, 4);  /* 4 threads */

/* Structured concurrency */
filtered = data.filter(op(x) { x > 5; }, 4);
sum = filtered.reduce(op(acc, x) { acc + x; }, 0);
```

**Advantages over Async/Await:**
- **No Manual Promise Management** - Automatic parallel execution
- **Thread Safety** - Built-in thread safety
- **Performance** - Optimized for data processing
- **Simplicity** - No complex async patterns

## Planned Advanced Features

### **1. Decorators/Annotations** 🔄 **PLANNED**

**Status**: Not yet implemented
**Planned Syntax**: `@decorator` pattern

```grapa
/* Planned decorator syntax */
@validate
@cache
process_data = op(data) {
    /* Function implementation */
};
```

**Use Cases:**
- **Function Decorators** - Add behavior to functions
- **Property Decorators** - Add behavior to properties
- **Class Decorators** - Add behavior to classes
- **Validation** - Input/output validation
- **Caching** - Result caching
- **Logging** - Function call logging

**Current Workaround:**
```grapa
/* Use function composition instead */
validate = op(f) { op(x) { if (x > 0) f(x); else $ERR("Invalid input"); }; };
cached_process = validate(process_data);
```

### **2. Generics/Templates** 🔄 **PLANNED**

**Status**: Not yet implemented
**Planned Syntax**: Type parameters

```grapa
/* Planned generic syntax */
generic_function = op<T>(x: T) {
    /* Generic implementation */
};
```

**Use Cases:**
- **Type-Safe Collections** - Generic containers
- **Algorithm Templates** - Reusable algorithms
- **Type Constraints** - Constrained generics
- **Specialization** - Type-specific implementations

**Current Workaround:**
```grapa
/* Dynamic typing handles most use cases */
process_any = op(x) {
    /* Dynamic type checking */
    if (x.type() == $ARRAY) process_array(x);
    else if (x.type() == $LIST) process_list(x);
    else process_other(x);
};
```

### **3. Advanced Control Flow** 🔄 **IN PROGRESS**

**Status**: Partially implemented
**Missing**: Exception handling, return/break/continue

#### **Current Control Flow:**
```grapa
/* Basic control flow (implemented) */
if (condition) {
    /* code */
} elseif (other_condition) {
    /* code */
} else {
    /* code */
}

while (condition) {
    /* code */
}

switch (value) {
    case 1: /* code */;
    case 2: /* code */;
    default: /* code */;
}
```

#### **Planned Exception Handling:**
```grapa
/* Planned try/catch syntax */
try {
    risky_operation();
} catch (error) {
    handle_error(error);
}
```

#### **Planned Return/Break/Continue:**
```grapa
/* Planned control flow */
function_with_return = op() {
    if (condition) return value;
    /* continue processing */
};

loop_with_break = op() {
    while (true) {
        if (done) break;
        if (skip) continue;
        /* process */
    }
};
```

### **4. Grapa's Pattern Matching** ✅ **IMPLEMENTED**

**Status**: Fully implemented with superior approach
**Grapa's Capabilities**: Multiple pattern matching mechanisms

#### **4a. Grapa's Destructuring** ✅ **IMPLEMENTED**

**Status**: Fully implemented with superior approach
**Grapa's Syntax**: Namespace-based destructuring assignment

```grapa
/* Grapa's destructuring via namespace concatenation */
@local ++= {name:"Alice", age:30};  /* Destructure into local namespace */
name.echo();  /* Direct access: "Alice" */
age.echo();   /* Direct access: 30 */

/* Array destructuring */
arr = [1, 2, 3, 4, 5];
@local ++= {first:arr[0], second:arr[1], rest:arr.range(2, arr.len())};
first.echo();   /* 1 */
second.echo();  /* 2 */
rest.echo();    /* [3, 4, 5] */
```

**Use Cases:**
- **Object Destructuring** - Extract properties into any namespace
- **Array Destructuring** - Extract elements with custom variable names
- **Function Parameters** - Destructure parameters into local scope
- **Return Values** - Destructure return values into target namespace
- **Configuration** - Load config objects into global namespace

**Namespace Options:**
```grapa
/* Local namespace (function scope) */
@local ++= {debug:true, timeout:5000};

/* Global namespace (persistent) */
@global ++= {config:true, version:"1.0"};

/* Object namespace (current object) */
@this ++= {name:"John", age:25};

/* Custom namespace */
@my_namespace ++= {data:123, flag:false};
```

**Duplicate Handling:**
```grapa
/* References access the last added in case of duplicates */
@local ++= {a:1, b:2};
@local ++= {a:10, c:3};  /* 'a' now refers to 10, not 1 */

/* Remove duplicates if needed */
@local.unique();  /* Removes duplicate entries */
```

#### **4b. Grapa's Switch-Based Pattern Matching** ✅ **IMPLEMENTED**

**Status**: Fully implemented with superior approach
**Grapa's Syntax**: Expression-based switch statements

```grapa
/* Each case can be a full boolean expression */
switch (true) {
    case (user.role == "admin" && user.age >= 18): "Admin access granted".echo();
    case (user.role == "user" && user.age >= 18): "User access granted".echo();
    case (user.age < 18): "Access denied - too young".echo();
    default: "Access denied - unknown role".echo();
};

/* Type-based pattern matching */
switch (true) {
    case (data.type() == $STR): "String data".echo();
    case (data.type() == $INT): "Integer data".echo();
    case (data.type() == $ARRAY): "Array data".echo();
    case (data.type() == $LIST): "List data".echo();
    default: "Other data type".echo();
};

/* Complex data structure matching */
switch (true) {
    case (user.name == "admin" && user.permissions.len() > 0): "Full admin".echo();
    case (user.permissions.find("read")): "Read access".echo();
    case (user.guest == true): "Guest access".echo();
    default: "No access".echo();
};
```

**Advantages of Grapa's Switch Pattern Matching:**
- **Full Expressions**: Each case can contain complex boolean expressions
- **Sequential Evaluation**: Evaluates from first to last case, stops at first match
- **No Fall-Through**: Unlike C-style switches, Grapa doesn't fall through to next case
- **Pattern Matching**: Can match on data structure patterns, types, and conditions
- **Default Case**: Optional catch-all for unmatched conditions
- **Type Safety**: Can match on data types using `.type()` method

#### **4c. Grapa's Structural Querying** ✅ **IMPLEMENTED**

**Status**: Fully implemented with superior approach
**Grapa's Syntax**: `.findall()` method for structural pattern matching

```grapa
/* XML structural pattern matching */
xml = <root><item id="1">Hello</item><item id="2">World</item></root>;
xml.findall({name:"item"})                /* All item elements */
xml.findall({name:"item", attr:{id:"2"}}) /* Items with id="2" */
xml.findall({value:"Hello"})              /* Elements with content "Hello" */

/* LIST structural pattern matching */
data = {name:"Alice", age:30, city:"New York"};
data.findall({has:{name:"age"}})          /* Returns: {"data":{"name":"Alice","age":30}} */

/* ARRAY structural pattern matching */
arr = [{name:"Alice"}, {name:"Bob"}, {name:"Charlie"}];
arr.findall({has:{name:"name"}})          /* Returns: [{"name":"Alice"},{"name":"Bob"},{"name":"Charlie"}] */
```

**Advantages of Grapa's Structural Querying:**
- **Declarative**: Express patterns declaratively rather than imperatively
- **Powerful**: Can match complex nested structures
- **Flexible**: Supports logical operators (AND, OR) in patterns
- **Type-Agnostic**: Works across XML, LIST, and ARRAY types
- **Composable**: Can combine multiple pattern conditions

### **5. Advanced Module System** 🔄 **BASIC IMPLEMENTATION**

**Status**: Basic implementation exists
**Current**: `include` and `reset` commands

```grapa
/* Current module system */
include "module.grc";  /* Include file */
reset;                 /* Reset namespace */
```

**Planned Enhancements:**
- **Import/Export System** - Named imports/exports
- **Module Namespaces** - Isolated module contexts
- **Circular Dependencies** - Handle circular imports
- **Hot Reloading** - Module hot reloading

### **6. Enhanced Reflection** 🔄 **BASIC IMPLEMENTATION**

**Status**: Basic implementation exists
**Current**: `.type()` method

```grapa
/* Current reflection */
type = value.type();  /* Get type */
```

**Planned Enhancements:**
- **Property Reflection** - Enumerate properties
- **Method Reflection** - Enumerate methods
- **Runtime Property Access** - Dynamic property access
- **Type Introspection** - Detailed type information

### **7. System Class Redefinition** ✅ **IMPLEMENTED**

**Status**: Fully implemented but advanced feature
**Current**: `@global` syntax for redefining system classes

```grapa
/* Redefine $ARRAY with custom methods */
@global["$ARRAY"] = class ($LIST, $VECTOR) {
    custom_method = op() { 
        "Custom array method called".echo();
        $this.len();  /* Access the array's length */
    };
};

/* Use the new method on array instances */
arr = [1, 2, 3];
result = arr.custom_method();
/* Output: "Custom array method called" */
/* result: 3 */
```

**Capabilities:**
- **System Class Extension** - Add methods to `$ARRAY`, `$LIST`, `$CLASS`, `$OBJ`, etc.
- **Global Variable Redefinition** - Redefine any global variable or function
- **Runtime Modification** - Change system behavior at runtime
- **Method Preservation** - Keep existing methods while adding new ones

**⚠️ Important Warnings:**
- **System Dependencies**: Redefining system classes can break functionality that depends on them
- **Side Effects**: Changes affect all existing and future instances of that class
- **Testing Required**: Thorough testing needed before redefining system classes
- **Documentation**: Always document your changes for future reference

**Best Practices:**
1. **Test thoroughly** before redefining system classes
2. **Document your changes** for future reference
3. **Consider alternatives** like creating new classes instead of redefining existing ones
4. **Use in isolated environments** when possible
5. **Use read-only protection** for critical system components

**Read-Only Protection:**
```grapa
/* Make a variable read-only to prevent modification */
my_variable = "important data";
$sys().setconst(my_variable, true);

/* Check if a variable is read-only */
is_readonly = $sys().const(my_variable);
```

**Future Enhancement:**
The `+=` operator will be enhanced to safely add methods to system classes:
```grapa
/* Future capability (not yet implemented) */
$ARRAY += (custom_method: op() { $this.len(); });
```

## Comparison with Other Languages

| Feature | Grapa | Python | JavaScript | Rust |
|---------|-------|--------|------------|------|
| **Executable BNF** | ✅ Yes | ❌ No | ❌ No | ❌ No |
| **Advanced Metaprogramming** | ✅ Superior | ⚠️ Limited | ⚠️ Limited | ⚠️ Limited |
| **Functional Programming** | ✅ Yes | ✅ Yes | ✅ Yes | ✅ Yes |
| **Pattern Matching** | ✅ Superior | ⚠️ Limited | ⚠️ Limited | ✅ Yes |
| **Concurrency** | ✅ Superior | ⚠️ Complex | ⚠️ Complex | ✅ Yes |
| **Decorators** | 🔄 Planned | ✅ Yes | ✅ Yes | ❌ No |
| **Generics** | 🔄 Planned | ⚠️ Limited | ❌ No | ✅ Yes |
| **Destructuring** | ✅ Superior | ✅ Yes | ✅ Yes | ✅ Yes |

## Best Practices

### **When to Use Advanced Features**

1. **Executable BNF**: When you need domain-specific languages or multi-syntax processing
2. **Metaprogramming**: When you need code generation or dynamic behavior
3. **Functional Programming**: When processing data with map/reduce/filter patterns
4. **Pattern Matching**: When searching or validating text data
5. **Parallel Processing**: When processing large datasets

### **Performance Considerations**

- **Execution Trees**: Compiled once, executed many times
- **Parallel Processing**: Use appropriate thread counts for your data size
- **Pattern Matching**: PCRE2 JIT compilation for fast matching
- **Functional Methods**: Automatic parallelization for large datasets

### **Migration from Other Languages**

- **Python Decorators**: Use function composition with `op()`
- **JavaScript Async/Await**: Use Grapa's parallel processing
- **Rust Pattern Matching**: Use Grapa's unified dot notation
- **Generic Functions**: Use dynamic typing and runtime checking

## See also
- [API Reference](api_reference.md)
- [Examples](examples.md)
- [Use Cases](use_cases/index.md)
- [Language Reference](syntax/basic_syntax.md)


