# Advanced Language Features

References:
- [Basic Syntax](syntax/basic_syntax.md)
- [Functional Programming](api_reference.md#functional-programming)
- [Metaprogramming](advanced_topics.md)

This document covers advanced language features in Grapa. **All planned advanced features have been implemented** - Grapa has achieved complete functional equivalence with modern languages.

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
$global["$custom_syntax"] = rule <$expression> '+' <$expression> {@<add,{$1,$3}>};

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
- **Runtime Modification** - Modify code structure at runtime
- **Reflection** - Inspect and analyze code structure

### **2. Advanced Metaprogramming - Grapa's Core Innovation**

**Metaprogramming is the essence of Grapa's design** - the language is fundamentally built around the concept that code is data and data is code. Grapa's metaprogramming capabilities surpass even LISP in many ways:

#### **$OP and $CODE - The Language Core**

Grapa's programming language **IS** metaprogramming. Every piece of code is built from `$OP` and `$CODE` structures:

```grapa
/* Grapa's programming language IS metaprogramming */
my_op = $OP("add", op(x, y) { return x + y; });
result = my_op(5, 3);  /* Dynamic function creation and execution */

/* Code construction at runtime */
dynamic_code = $CODE([
    $OP("echo", op(msg) { msg.echo(); }),
    $OP("call", op() { "Hello from dynamic code!".echo(); })
]);
dynamic_code.run();
```

#### **Runtime Rule Mutation**

The rules engine is **fully mutable at runtime** because all rules are stored in variables:

```grapa
/* Rules engine is fully mutable at runtime */
$RULE("custom_syntax", "my_command $STR", op(cmd, str) {
    "Custom syntax executed: " + str.echo();
});

/* Rules are stored in variables - fully dynamic */
my_rules = $RULE("dynamic_rule", "$INT + $INT", op(a, b) {
    return a + b;
});

/* Modify rules at runtime */
$global["$expression"] ++= rule 'custom' $STR { op(text:$2) { "Custom: " + text; } };
```

#### **Token-to-Code Transformation**

The rules engine converts tokens directly into executable `$OP` and `$CODE` structures:

```grapa
/* Every syntax construct becomes executable code */
expression = "5 + 3";
/* Internally becomes: $CODE([$OP("add", $INT(5), $INT(3))]) */

/* Custom syntax becomes executable code */
custom_expression = "custom 'hello world'";
/* Internally becomes: $CODE([$OP("custom", $STR("hello world"))]) */
```

#### **Why Grapa Surpasses LISP in Metaprogramming**

**LISP's Approach:**
```lisp
;; LISP uses S-expressions (symbolic expressions)
(defun add (x y) (+ x y))
;; Code is represented as lists: (defun add (x y) (+ x y))
```

**Grapa's Approach:**
```grapa
/* Grapa uses executable BNF with direct code generation */
$global["$function"] = rule 'defun' $ID '(' $params ')' $body {
    op(name:$2, params:$4, body:$6) {
        /* Directly creates executable $OP structure */
        return $OP(name, op(params) { body; });
    }
};

/* Usage creates executable code immediately */
defun add(x, y) { x + y; }
/* Becomes: $OP("add", op(x, y) { x + y; }) */
```

#### **Advanced Metaprogramming Patterns**

```grapa
/* Code generation and execution */
generate_function = op(name, body) {
    return $OP(name, op() { body; });
};

dynamic_func = generate_function("hello", "Hello World!".echo());
dynamic_func();  /* Executes: "Hello World!" */

/* Runtime code analysis */
analyze_code = op(code) {
    return {
        type: code.type(),
        description: code.describe(),
        parameters: code.parameters()
    };
};

/* Code transformation */
transform_code = op(code, transformer) {
    return transformer(code);
};
```

#### **Multi-Syntax Metaprogramming**

Grapa's metaprogramming extends beyond its own syntax:

```grapa
/* Add SQL syntax to the language */
$custom_command = rule select $STR from $STR {op(fields:$2,table:$4){
    /* SQL becomes executable Grapa code */
    return query_database(fields, table);
}};

/* Add PROLOG syntax to the language */
$custom_command ++= rule $STR '(' $STR ')' '.' {op(predicate:$1,args:$3){
    /* PROLOG becomes executable Grapa code */
    return add_fact(predicate, args);
}};

/* Execute multi-syntax code */
op()("select name, age from users")();  /* SQL syntax */
op()("parent(john,mary)")();            /* PROLOG syntax */
```

#### **Why This Makes Grapa Unique**

1. **Everything is Code**: Variables, functions, classes, syntax - all are `$OP` and `$CODE` structures
2. **Runtime Grammar Mutation**: The language grammar can change while running
3. **Direct Code Generation**: No intermediate representations - tokens become executable code
4. **Multi-Syntax Support**: Can add any syntax to the language at runtime
5. **Human-Readable Execution Trees**: Code structure is inspectable and manipulable

**This is why Grapa is probably more metaprogramming-focused than any other language** - metaprogramming isn't a feature, it's the foundation of the entire language design.

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
- **Function Expressions** - `op { ... }`, `op() { ... }`, or `op(x) { ... }` syntax (parameters optional)
- **Functional Methods** - `.map()`, `.filter()`, `.reduce()`
- **Parallel Processing** - Built-in parallel execution
- **Complex Context Objects** - Any data structure as initializers with state evolution

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

### **5. Class and Object Extension System**

Grapa provides a comprehensive extension system that allows developers to extend both individual objects and classes:

```grapa
/* Pattern 1: Extending individual objects */
test1 = {n:0};
test1 += (inc:op(inc:1){n+=inc;});
test1.inc();      /* n becomes 1 */
test1.inc(2);     /* n becomes 3 */

/* Pattern 2: Creating custom classes */
myclass = class ($GOBJ) {
    n = 0; 
    inc = op(inc:1){n+=inc;};
};
myclass ++= {
    m:0, 
    dec:op(dec:1){m=n; n-=dec;}
};
test2 = myclass();
test2.inc();      /* n becomes 1 */
test2.dec(5);     /* n becomes -4, m becomes 1 */

/* Pattern 3: Extending system classes */
$LIST();  /* Load system class */
@$LIST ++= {f:op(){55}};
@$LIST += (c:op(){$this.len();});
ar = [1,2,4];
ar.f();           /* Returns 55 */
ar.c();           /* Returns 3 */
```

**Key Features:**
- **Object Extension** - Add methods to individual objects with `+=`
- **Class Extension** - Extend classes with `++=` for multiple additions
- **System Class Extension** - Extend core language classes globally
- **Inheritance Support** - Custom classes inherit from system classes
- **Method Resolution** - Automatic method lookup through inheritance chain

### **6. Superior Concurrency**

Grapa's parallel-by-design architecture surpasses traditional async/await:

```grapa
/* Parallel processing with automatic thread management */
data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
processed = data.map(op(x) { x * x; }, 4);  /* 4 threads */

/* Structured concurrency */
filtered = data.filter(op(x) { x > 5; }, 4);
sum = filtered.reduce(op(acc, x) { acc + x; }, 0);
```

**Worker Thread Coordination:**
```grapa
/* Spawn multiple worker threads that all must complete */
workers = [1, 2, 3, 4, 5].map(op(worker_id) {
    /* Each worker does independent work */
    ("Worker " + worker_id.str() + " starting").echo();
    sleep(worker_id);  /* Simulate work */
    ("Worker " + worker_id.str() + " completed").echo();
    worker_id * 100;  /* Return result */
});
/* All workers complete before proceeding */
("All workers finished").echo();
```

**Advantages over Async/Await:**
- **No Manual Promise Management** - Automatic parallel execution
- **Thread Safety** - Built-in thread safety
- **Performance** - Optimized for data processing
- **Simplicity** - No complex async patterns
- **Structured Concurrency** - Automatic synchronization barriers
- **Worker Coordination** - Use functional methods as thread synchronization barriers

## Planned Advanced Features

### **1. Decorators/Annotations** 🔄 **WORKAROUND SUPPORTED**

**Status**: Not yet implemented
**Planned Syntax**: `$decorator` pattern

```grapa
/* Possible decorator syntax if implemented*/
$validate
$cache
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

### **2. Generics

**Status**: Fully implemented via dynamic typing
**Grapa's Approach**: Dynamic typing with runtime type checking

```grapa
/* Generic-like function using dynamic typing */
generic_function = op(x) {
    /* Dynamic type checking */
    if (x.type() == $LIST) process_array(x);
    else if (x.type() == $GOBJ) process_list(x);
    else process_other(x);
};
```

**Implemented Capabilities:**
- **Type-Safe Collections** - Via arrays and objects with .type() validation
- **Algorithm Templates** - Via function composition and dynamic typing
- **Type Constraints** - Via runtime type checking with .type() method
- **Specialization** - Via switch/case on type() results:

**Advantages over Static Generics:**
- **Runtime flexibility** - Types determined at execution time
- **Dynamic grammar changes** - Works with executable BNF system
- **Simpler syntax** - No complex type parameter syntax needed
- **Better performance** - No compile-time type checking overhead

### **3. Advanced Control Flow** ✅ **IMPLEMENTED**

**Status**: Fully implemented
**Capabilities**: Exception handling, return/break/continue, switch/case with expressions:

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

switch (value)
    case 1: /* code */;
    case 2: /* code */;
    default: /* code */;
```

#### **Implemented Exception Handling:**
```grapa
/* Implemented try/catch syntax */
try risky_operation();
catch (error): handle_error(error);
```

#### **Implemented Return/Break/Continue:**
```grapa
/* Implemented control flow */
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
$local ++= {name:"Alice", age:30};  /* Destructure into local namespace */
name.echo();  /* Direct access: "Alice" */
age.echo();   /* Direct access: 30 */

/* Array destructuring */
arr = [1, 2, 3, 4, 5];
$local ++= {first:arr[0], second:arr[1], rest:arr.range(2, arr.len())};
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
$local ++= {debug:true, timeout:5000};

/* Global namespace (persistent) */
$global ++= {config:true, version:"1.0"};

/* Object namespace (current object) */
@this ++= {name:"John", age:25};

/* Custom namespace */
@my_namespace ++= {data:123, flag:false};
```

**Duplicate Handling:**
```grapa
/* References access the last added in case of duplicates */:
$local ++= {a:1, b:2};
$local ++= {a:10, c:3};  /* 'a' now refers to 10, not 1 */

/* Remove duplicates if needed */
@$local.unique();  /* Removes duplicate entries */
```

#### **4b. Grapa's Switch-Based Pattern Matching** ✅ **IMPLEMENTED**

**Status**: Fully implemented with superior approach
**Grapa's Syntax**: Expression-based switch statements

```grapa
/* Each case can be a full boolean expression */:
switch (true)
    case (user.role == "admin" && user.age >= 18): "Admin access granted".echo();
    case (user.role == "user" && user.age >= 18): "User access granted".echo();
    case (user.age < 18): "Access denied - too young".echo();
    default: "Access denied - unknown role".echo();

/* Type-based pattern matching */
switch (true)
    case (data.type() == $STR): "String data".echo();
    case (data.type() == $INT): "Integer data".echo();
    case (data.type() == $LIST): "Array data".echo();
    case (data.type() == $GOBJ): "List data".echo();
    default: "Other data type".echo();

/* Complex data structure matching */
switch (true)
    case (user.name == "admin" && user.permissions.len() > 0): "Full admin".echo();
    case (user.permissions.find("read")): "Read access".echo();
    case (user.guest == true): "Guest access".echo();
    default: "No access".echo();
```

**Advantages of Grapa's Switch Pattern Matching:**
- **Full Expressions**: Each case can contain complex boolean expressions:
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

### **6. Enhanced Reflection** ✅ **IMPLEMENTED**

**Status**: Fully implemented with enhanced `.describe()` function
**Grapa's Capabilities**: Comprehensive object inspection and description

Grapa provides enhanced reflection capabilities through the `.describe()` method, which can be applied to any object with configurable options for detailed inspection.

#### **6a. Enhanced .describe() Method** ✅ **IMPLEMENTED**

**Status**: Fully implemented with comprehensive options
**Grapa's Syntax**: Configurable object description with multiple output formats

```grapa
/* Enhanced .describe() with parameters */
object.describe(options);

/* Options object structure */
options = {
    "properties": true,      /* Include property names and types */
    "methods": true,         /* Include method names and signatures */
    "structure": false,      /* Include internal structure details */
    "values": false,         /* Include actual values (for small objects) */
    "format": "text"         /* "text", "json", "xml" */
};
```

**Example Usage:**
```grapa
/* Basic description */
user = {name: "Alice", age: 30, greet: op() { "Hello!".echo(); }};
user.describe();
/* Returns: "List with 3 properties (keys: name, age, greet)" */

/* Detailed description with properties */
user.describe({properties: true, methods: true});
/* Returns: "List with 3 properties (keys: name, age, greet)" */

/* Structure description for complex objects */
table = {}.table("ROW");
table.describe({structure: true});
/* Returns detailed structure information */

/* JSON format for programmatic access */
user.describe({properties: true, format: "json"});
/* Returns: {"type":"list","length":3,"properties":3,"keys":["name","age","greet"]} */
```

#### **6b. Type-Specific Descriptions** ✅ **IMPLEMENTED**

**Status**: Fully implemented for all major types
**Grapa's Capabilities**: Detailed descriptions for each data type

**String Descriptions:**
```grapa
'hello'.describe();                    /* "String with length 5" */
'world'.describe({values: true});      /* "String with length 5: \"world\"" */
```

**Numeric Descriptions:**
```grapa
(123).describe();                      /* "Integer: 123" */
(2.3).describe({structure: true});     /* Shows 7 float components */
```

**Array Descriptions:**
```grapa
[1, 2, 3].describe();                  /* "Array with 3 elements" */
[1, 2, 3].describe({structure: true}); /* "Array with 3 elements (types: integer, integer, integer)" */
```

**List/Object Descriptions:**
```grapa
{name: 'Alice', age: 30}.describe();   /* "List with 2 properties (keys: name, age)" */
{name: 'Bob'}.describe({format: 'json'}); /* {"type":"list","length":1,"properties":1,"keys":["name"]} */
```

#### **6c. Float Structure Inspection** ✅ **IMPLEMENTED**

**Status**: Fully implemented with 7-component structure
**Grapa's Capabilities**: Detailed internal float component inspection

Grapa's `$FLOAT` type has 7 internal components that can be inspected:

```grapa
/* Show float structure details */
(2.3).describe({structure: true});
/* Returns: "Float: 2.3... (sign:false, trunc:false, fix:false, exp:1, max:184, extra:10, data:3689348814741910323)" */

/* JSON format with structure */
(2.3).describe({structure: true, format: "json"});
/* Returns: {"type":"float","value":"2.3...","structure":{"sign":false,"trunc":false,"fix":false,"exp":1,"max":184,"extra":10,"data":"3689348814741910323"}} */
```

**The 7 Float Components:**
1. **sign** - Boolean indicating if the number is signed
2. **trunc** - Boolean indicating if truncation occurred
3. **fix** - Boolean indicating if it's a fixed-point number
4. **exp** - Exponent value
5. **max** - Maximum bits
6. **extra** - Extra precision bits
7. **data** - The actual numeric data

**Complementary Methods:**
For programmatic access to float components, Grapa also provides:
- **`.decode("FLOAT")`** - Extract components into an object for manipulation
- **`.encode("FLOAT")`** - Create a float from component values

```grapa
/* Extract components for manipulation */
components = (2.3).decode("FLOAT");
/* Returns: {"sign":false,"trunc":false,"fix":false,"exp":1,"max":184,"extra":10,"data":3689348814741910323} */

/* Modify components and create new float */
components.max = 300;
components.extra = 20;
new_float = components.encode("FLOAT");

/* .describe() provides inspection, .decode()/.encode() provide manipulation */
```

#### **6d. Output Formats** ✅ **IMPLEMENTED**

**Status**: Multiple output formats supported
**Grapa's Capabilities**: Text and JSON output formats

**Text Format (Default):**
```grapa
'hello'.describe();                    /* "String with length 5" */
[1, 2, 3].describe({structure: true}); /* "Array with 3 elements (types: integer, integer, integer)" */
```

**JSON Format:**
```grapa
'hello'.describe({format: 'json'});    /* {"type":"string","length":5} */
[1, 2, 3].describe({format: 'json'});  /* {"type":"array","length":3} */
```

**Advantages of Grapa's Enhanced Reflection:**
- **Single Method**: One `.describe()` method handles all reflection needs
- **Configurable**: Options control what information is included
- **Multiple Formats**: Text for humans, JSON for programs
- **Type-Specific**: Detailed information for each data type
- **Float Structure**: Complete internal component inspection
- **Performance**: Efficient implementation with minimal overhead

### **7. System Class Redefinition** ✅ **IMPLEMENTED**

**Status**: Fully implemented but advanced feature
**Current**: `$global` syntax for redefining system classes

```grapa
/* Redefine $LIST with custom methods */
$global["$LIST"] = class ($GOBJ, $VECTOR) {
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
- **System Class Extension** - Add methods to `$LIST`, `$GOBJ`, `$CLASS`, `$OBJ`, etc.
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
$sys().setfreeze(my_variable, true);

/* Check if a variable is freeze (read-only) */
is_readonly = $sys().freeze(my_variable);
```

**Future Enhancement:**
The `+=` operator will be enhanced to safely add methods to system classes:
```grapa
/* Future capability (not yet implemented) */
$LIST += (custom_method: op() { $this.len(); });
```

### **Current Limitations and Future Enhancements**

#### **Object Lifecycle Management**
**Current State**: Grapa supports automatic constructors through the `$new()` method, but still lacks automatic destructors.

**What's Available:**
- **Automatic Constructors**: `$new()` method is automatically called when creating instances using `Class()` syntax
- **Parameter Support**: Constructors can accept parameters with default values:
- **Single Constructor**: Only one `$new()` method is supported per class

**What's Missing:**
- **Automatic Destructors**: No `destructor()` or `finalize()` method for cleanup
- **Multiple Constructors**: Only one `$new()` method per class is supported

**Current Usage:**
```grapa
/* Automatic constructor with $new() */
Person = class {
    name = "";
    age = 0;
    
    /* Automatic constructor - called when creating instances */
    $new = op(n:"", a:0) {
        name = n;
        age = a;
    };
    
    /* Method */
    getInfo = op() {
        ("Name: " + name + ", Age: " + age.str()).echo();
    };
};

/* Create instances - constructor called automatically */
person1 = Person();           /* name="", age=0 */
person2 = Person("Alice", 30); /* name="Alice", age=30 */
person2.getInfo();            /* Output: Name: Alice, Age: 30 */
```

**Alternative Manual Pattern:**
```grapa
/* Manual initialization pattern (for classes without $new()) */
Person = class {
    name = "";
    age = 0;
    
    /* Manual init method - must be called explicitly */
    init = op(n, a) {
        name = n;
        age = a;
    };
};

/* Create and manually initialize */
person = obj Person;
person.init("Alice", 30);  /* Must call init explicitly */
```

**Future Enhancement**: Multiple constructors and automatic destructor support may be added in future versions.

#### **Operator Overloading**
**Current State**: Operators work only with built-in types and predefined behaviors.

**What's Missing:**
- **Custom Operator Behavior**: Cannot define `+`, `-`, `*`, `/`, `==`, etc. for custom classes
- **Object Arithmetic**: Cannot perform arithmetic operations on custom objects
- **Custom Comparisons**: Cannot define custom equality or comparison behavior

**Workaround:**
```grapa
/* Use method names instead of operators */
Vector = class {
    x = 0;
    y = 0;
    
    /* Custom methods instead of operators */
    add = op(other) {
        result = obj Vector;
        result.x = x + other.x;
        result.y = y + other.y;
        result;
    };
    
    equals = op(other) {
        x == other.x && y == other.y;
    };
};

/* Usage */
v1 = obj Vector; v1.x = 1; v1.y = 2;
v2 = obj Vector; v2.x = 3; v2.y = 4;
v3 = v1.add(v2);  /* Instead of v1 + v2 */
```

**Future Enhancement**: Operator overloading will allow defining custom behavior for operators on custom classes.

### **7. Complex Context Objects in Functional Programming**

Grapa's functional methods support **any data structure as initializers**, enabling sophisticated state machines and context-aware operations. This goes far beyond simple accumulation to enable complex data processing pipelines.

#### **Stateful Operations with Dynamic Behavior**

```grapa
/* Dynamic algorithm switching based on data characteristics */
[1, 2, 3, 4, 5].reduce(op(state, item) {
    /* Track processing state */
    state.count += 1;
    state.sum += item;
    
    /* Switch algorithms based on data */
    if (item > 3) {
        state.algorithm = "multiply";
        state.operation = op(a, b) { a * b; };
    };
    
    /* Apply current operation */
    state.result = state.operation(state.result, item);
    state;
}, {
    count: 0,
    sum: 0,
    result: 1,
    algorithm: "add",
    operation: op(a, b) { a + b; }
});
```

#### **Context-Aware Data Processing**

```grapa
/* Map with complex state tracking */
[1, 2, 3, 4, 5].map(op(item, context) {
    /* Update context statistics */
    context.total += 1;
    context.sum += item;
    context.average = context.sum / context.total;
    
    /* Apply context-aware transformation */
    if (item > context.average) {
        item * 2;  /* Double above-average items */
    } else {
        item / 2;  /* Halve below-average items */
    };
}, {
    total: 0,
    sum: 0,
    average: 0
});
```

#### **Pipeline Processing with State Machines**

```grapa
/* Multi-stage data processing pipeline */
data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

/* Stage 1: Filter with state tracking */
filtered = data.filter(op(item, state) {
    state.seen += 1;
    if (state.seen <= 5) {  /* Process first 5 items */
        state.sum += item;
        true;
    } else {
        false;
    };
}, {seen: 0, sum: 0});

/* Stage 2: Transform with dynamic operations */
transformed = filtered.map(op(item, context) {
    if (item > context.threshold) {
        context.operation = op(x) { x * 2; };
    } else {
        context.operation = op(x) { x + 10; };
    };
    context.operation(item);
}, {threshold: 3, operation: op(x) { x; }});

/* Stage 3: Aggregate with complex state */
result = transformed.reduce(op(state, item) {
    state.count += 1;
    state.sum += item;
    state.max = item > state.max ? item : state.max;
    state.min = item < state.min ? item : state.min;
    state;
}, {count: 0, sum: 0, max: -999999, min: 999999});
```

#### **Advanced Use Cases**

1. **Dynamic Algorithm Selection**: Switch algorithms based on data characteristics
2. **State Tracking**: Maintain complex state across iterations
3. **Pipeline Processing**: Build multi-stage data processing workflows
4. **Configuration Management**: Use context objects for configuration and state
5. **Complex Aggregations**: Perform sophisticated aggregations with multiple metrics
6. **Machine Learning Pipelines**: Build ML preprocessing and feature engineering workflows
7. **Real-time Data Processing**: Process streaming data with stateful operations

#### **Key Advantages**

- **Flexibility**: Any data structure can serve as context
- **State Evolution**: Context can evolve and change during operations
- **Dynamic Operations**: Functions can be stored and modified within context
- **Composability**: Complex operations can be built from simple components
- **Performance**: Parallel processing with state management
- **Readability**: Clear separation of data, operations, and state

#### **Design Philosophy: Dynamic Typing Enables Flexibility**

Grapa's **dynamic typing** is a deliberate design choice that enables this level of flexibility. Unlike strict type-constrained languages, Grapa leaves type handling to the implementation, allowing:

- **Runtime Type Evolution**: Data structures can change types during execution
- **Dynamic Function Assignment**: Operations can be swapped and modified at runtime
- **Context-Aware Processing**: Any object can serve as context for any operation
- **Implementation-Driven Types**: Types emerge from how they're used, not predefined constraints

This design philosophy enables the sophisticated state machines and dynamic algorithms shown above, where the "type" of the context object evolves based on the data and operations being performed.

## Comparison with Other Languages

| Feature | Grapa | Python | JavaScript | Rust |
|---------|-------|--------|------------|------|
| **Executable BNF** | ✅ Yes | ❌ No | ❌ No | ❌ No |
| **Advanced Metaprogramming** | ✅ Superior | ⚠️ Limited | ⚠️ Limited | ⚠️ Limited |
| **Functional Programming** | ✅ Superior | ✅ Yes | ✅ Yes | ✅ Yes |
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


