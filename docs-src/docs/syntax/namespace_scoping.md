# Namespace Scoping in Grapa

Grapa provides a sophisticated namespace scoping system that allows precise control over variable visibility and lifetime. Understanding this system is crucial for writing robust, thread-safe code.

## Core Namespace Types

Grapa defines three fundamental namespace types that can be accessed using special identifiers:

### `$global` - Global Namespace
- **Purpose**: Global variables accessible from anywhere in the program
- **Lifetime**: Entire program execution
- **Scope**: Application-wide
- **Use Case**: Configuration, shared state, constants

### `$this` - Object Context
- **Purpose**: Current object's namespace (root of object hierarchy)
- **Lifetime**: Object lifetime
- **Scope**: Object-wide
- **Use Case**: Object properties, methods, internal state

### `$local` - Local Context
- **Purpose**: Variables within the current block scope (`{}`)
- **Lifetime**: Block execution
- **Scope**: Function or block-level
- **Use Case**: Function parameters, temporary variables, thread-local storage
- **Special Feature**: `$local` is automatically initialized with all function parameters, providing access to the complete parameter list
- **Important**: `$local` properly isolates variables within function scope, protecting against external variable access and ensuring thread safety in recursive functions and multi-threaded execution

### `$parent` - Parent Function Context
- **Purpose**: Access to the calling function's local namespace
- **Lifetime**: Function call execution
- **Scope**: Parent function's `$local` namespace
- **Use Case**: Function communication, nested function data access, callback patterns
- **Behavior**: When called from top-level, returns the grammar system; when called from within a function, returns the calling function's `$local` namespace

### `$root` - Root Widget Context
- **Purpose**: Reference to the root widget/window in GUI applications
- **Lifetime**: Widget lifetime
- **Scope**: Widget hierarchy root
- **Use Case**: GUI programming, widget manipulation, window management
- **Implementation**: Variable lookup that resolves to the root widget

### `$self` - Current Widget Context
- **Purpose**: Reference to the current widget/object being processed
- **Lifetime**: Widget processing lifetime
- **Scope**: Current widget context
- **Use Case**: Widget callbacks, event handling, self-referential operations
- **Implementation**: Variable lookup that resolves to the current widget

## Syntax

All namespace variables must be accessed with the `$` prefix:

```grapa
// Core namespace variables:
$global.x = 5;           // Global namespace
$this.property = "value"; // Object context
$local.temp = 10;        // Local context

// Special context variables:
$parent.rule_name;       // Grammar system access
$root.hide();            // Root widget reference
$self.get("property");   // Current widget reference
```

## Critical Use Cases

### 1. Avoiding Variable Name Conflicts

When nested functions or loops use the same variable names, conflicts can occur:

```grapa
// PROBLEMATIC: Variable name conflict
i = 0;
while (i < 10) {
    someFunction();  // This function might use 'i' and conflict!
};

someFunction = op() {
    i = 0;  // This overwrites the outer loop's 'i'!
    while (i < 5) {
        // Unintended behavior
    };
};

// SOLUTION: Use local scoping
i = 0;
while (i < 10) {
    someFunction();  // Safe to call
};

someFunction = op() {
    $local.i = 0;  // Local variable, won't affect outer scope
    while ($local.i < 5) {
        // Safe to use i here
    };
};
```

### 2. Thread Safety

Functions that may be called from parallel processes must use local variables:

```grapa
// NOT THREAD SAFE
counter = 0;
unsafeFunction = op() {
    counter += 1;  // Shared global variable - race condition!
    return counter;
};

// THREAD SAFE
threadSafeFunction = op() {
    $local.counter = 0;  // Each thread gets its own copy
    $local.data = {};    // Thread-local storage
    $local.counter += 1;
    return $local.counter;
};
```

### 3. Function Parameter Safety

Functions should declare local variables to avoid conflicts with global state:

```grapa
// Example: HTTP client function
curl = op(url_str, options) {
    // Declare ALL local variables upfront
    $local.method = null;
    $local.cert = null;
    $local.url_part = null;
    $local.host = null;
    $local.client = null;
    $local.err = null;
    $local.slash_pos = null;
    $local.i = null;
    $local.request = null;
    $local.response = null;
    $local.header_end = null;
    $local.headers_raw = null;
    $local.response_body = null;
    $local.lines = null;
    $local.status_line = null;
    $local.status_parts = null;
    $local.status_code = null;
    $local.status_text = null;
    
    // Now safe to use all variables without conflicts
    method = "GET";
    if (options.method.type() == $STR) {
        method = options.method;
    };
    
    // ... rest of function implementation
};
```

## Best Practices

### 1. Declare Local Variables Upfront
Always declare local variables at the beginning of functions:

```grapa
myFunction = op(param1, param2) {
    // Declare all local variables first
    $local.result = null;
    $local.temp = null;
    $local.i = null;
    $local.data = {};
    
    // Then use them throughout the function
    result = param1 + param2;
    // ... rest of function
};
```

### 2. Use `$local` for Function Variables
Any variable used within a function should be declared as local:

```grapa
processData = op(data) {
    $local.processed = [];
    $local.i = 0;
    
    while ($local.i < data.len()) {
        $local.item = data[$local.i];
        $local.processed.append($local.item.process());
        $local.i += 1;
    };
    
    return $local.processed;
};
```

### 3. Multiple Ways to Add Variables to `$local`
You can declare multiple local variables using several different approaches:

```grapa
/* Method 1: Individual declarations */
f1 = op() {
    $local.x = 3;
    $local.y = 2;
    $local.i = 0;
    $local.t = [1,2,3];
    $local.v = {'a':22,'b':33};
    return $local;
};

/* Method 2: Reset $local to a list (replaces all existing variables) */
f2 = op() {
    $local = {x: 3, y: 2, i: 0, t: [1,2,3], v: {'a':22,'b':33}};
    return $local;
};

/* Method 3: Object extension with ++= (preserves existing variables) */
f3 = op() {
    $local.x = 3;  /* Individual declaration first */
    $local++={
        y: 2,
        i: 0,
        t: [1,2,3],
        v: {'a':22,'b':33}
    };
    return $local;
};

/* Method 4: Compact object extension (preserves existing variables) */
f4 = op() {
    $local.x = 3;  /* Individual declaration first */
    $local++={y:2,i:0,t:[1,2,3],v:{'a':22,'b':33}};
    return $local;
};

/* Method 5: Mixed approach */
f5 = op() {
    $local.g = 1;  /* Individual declaration */
    $local = {a: 1, b: "hi"};  /* Reset to list (loses 'g') */
    return $local;
};
```

**Key Differences:**
- **Individual declarations** (`$local.x = 3;`): Add one variable at a time
- **Reset to list** (`$local = {...}`): Replaces ALL existing variables with new structure
- **Object extension** (`$local++={...}`): Adds new variables while preserving existing ones
- **Compact extension** (`$local++={y:2,i:0}`): Same as extension but more compact syntax

**Note**: When you reset `$local` to a list, it replaces all existing local variables with the new list structure. Use object extension (`++=`) when you want to add variables while preserving existing ones.

### 4. Avoid Global Variables in Functions
Unless absolutely necessary, avoid using global variables within functions:

```grapa
// BAD: Using global variables
globalCounter = 0;
badFunction = op() {
    globalCounter += 1;  // Side effects, not thread-safe
};

// GOOD: Use local variables
goodFunction = op() {
    $local.counter = 0;
    $local.counter += 1;
    return $local.counter;
};
```

### 5. Thread-Safe Design
For functions that may run in parallel contexts:

```grapa
threadSafeProcessor = op(input) {
    // All variables are local - function is thread-safe
    $local.result = null;
    $local.temp = input.clone();
    $local.processed = $local.temp.transform();
    
    return $local.processed;
};
```

### 6. **Critical: Network Programming Thread Safety**
**Network message handlers and concurrent functions MUST use `$local` variables:**

```grapa
/* ✅ CRITICAL - Thread-safe network message handler */
httpMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Check for connection close */
        if (netSession.data.len() == 0) {
            netSession.disconnect();
            return(null);
        };
        
        /* ALL variables must be $local to avoid race conditions */
        $local.datasplit = netSession.data.split(" ");
        $local.method = datasplit[0];
        $local.path = datasplit[1];
        $local.response = "";
        
        /* Process request safely */
        if (path == "/") {
            response = "HTTP/1.1 200 OK\r\n\r\n<h1>Server</h1>";
        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        };
        
        netSession.send(response);
    };
};

/* ❌ DANGEROUS - Race conditions in concurrent execution */
badMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    
    if (hasmore == 0) {
        /* Global variables cause race conditions! */
        request = netSession.data;  // ❌ Race condition!
        method = request.left(3);   // ❌ Race condition!
        path = "";                  // ❌ Race condition!
        
        /* Multiple concurrent handlers will overwrite each other's variables */
    };
};
```

**Why this is critical:**
- Network message handlers run concurrently
- Global variables are shared across all concurrent executions
- Race conditions can cause data corruption, crashes, or incorrect behavior
- `$local` ensures each concurrent execution has its own variable instances

## Namespace Hierarchy

Variables are resolved in the following order:
1. **Local scope** (`$local`)
2. **Object scope** (`$this`) 
3. **Global scope** (`$global`)

```grapa
// Example showing hierarchy
$global.x = "global";
$this.x = "object";

myFunction = op() {
    $local.x = "local";
    
    x;  // Returns "local" (local scope)
    $this.x;  // Returns "object" (object scope)
    $global.x;  // Returns "global" (global scope)
};
```

## Special Context Variables

### `$parent` - Parent Function Access

`$parent` provides access to the calling function's local namespace, enabling powerful function communication patterns:

```grapa
/* Basic parent access */
child_function = op() {
    $local.result = "processed";
    $parent;  // Returns calling function's $local namespace
};

parent_function = op() {
    $local.data = "input";
    $local.status = "processing";
    child_function();  // Returns {"data":"input","status":"processing"}
};

parent_function();  // Returns {"data":"input","status":"processing"}
```

**Key Behaviors:**
- **From top-level**: Returns the grammar system (106+ rules)
- **From within functions**: Returns the calling function's `$local` namespace
- **Nested calls**: Always refers to the immediate parent, not the root
- **Block scope calls**: Returns the immediate calling scope's `$local` (even if it's a nested block)
- **Call stack tracing**: Can be used to trace back to the global namespace

```grapa
/* Nested function example */
grandchild = op() {
    $local.gc_data = "grandchild";
    $parent;  // Returns parent's $local, not grandparent's
};

child = op() {
    $local.c_data = "child";
    grandchild();  // Returns {"c_data":"child"}
};

parent = op() {
    $local.p_data = "parent";
    child();  // Returns {"c_data":"child"}
};

parent();  // Returns {"c_data":"child"}
```

**Critical: `$parent` Returns Immediate Calling Scope**

`$parent` always returns the **immediate calling scope's `$local`**, even if that scope is a nested block within a function:

```grapa
/* Example: $parent returns block scope, not function scope */
f1 = op() {
    $parent;  // Returns whatever scope called f1()
};

f2 = op() {
    $local.a = 1;           // Function scope: {"a":1}
    if (true) {
        $local.b = 2;       // Block scope: {"b":2}
        f1();               // Returns {"b":2} - block scope, not function scope!
    };
};

f2();  // Returns {"b":2}
```

**Key Point**: `$parent` does **NOT** return the function's `$local` - it returns the **immediate calling scope's `$local`**. In the example above, `f1()` returns the `if` block's `$local` (`{"b":2}`), not the function's `$local` (`{"a":1}`).

### Call Stack Tracing with `$parent`

`$parent` can be used to construct a call stack by leaving breadcrumbs in each function's `$local` namespace:

```grapa
/* Basic call stack tracing */
f1 = op() {
    $local.breadcrumb = "f1";
    f2();
};

f2 = op() {
    $local.breadcrumb = "f2";
    f3();
};

f3 = op() {
    $local.breadcrumb = "f3";
    $parent.breadcrumb;  // Returns "f2" (immediate parent)
};

f1();  // Returns "f2"
```

**Global Namespace Detection:**
```grapa
/* Detect if called from top-level */
f3 = op() {
    $local.breadcrumb = "f3";
    $parent == $global;  // Returns true if called from top-level
};

f3();  // Returns true (called from top-level)
```

**Advanced Call Stack Construction:**
```grapa
/* Recursive call stack builder */
buildCallStack = op() {
    $local.stack = {};
    $local.current = $local.breadcrumb;
    
    if ($parent != $global) {
        $local.stack = $parent.stack;
        $local.stack += $current;
    } else {
        $local.stack = {$current};
    };
    
    $local.stack;
};

/* Usage with breadcrumbs */
f1 = op() {
    $local.breadcrumb = "f1";
    buildCallStack();
};

f2 = op() {
    $local.breadcrumb = "f2";
    f1();
};

f2();  // Returns {"f2", "f1"}
```

**Debugging Applications:**
- **Function call tracing**: Track which functions called which
- **Error context**: Provide context about where errors occurred
- **Performance profiling**: Understand call patterns
- **Dynamic debugging**: Inspect calling context at runtime

### `$root` and `$self` - Widget Context

In GUI applications, `$root` and `$self` provide widget references:

```grapa
/* GUI widget example (from $editor.grc) */
button_callback = op() {
    $root.hide();           // Hide the root window
    $self.get("text");      // Get current widget's text
    $root.child("menu");    // Access child widgets
};

/* Widget hierarchy navigation */
menu_callback = op() {
    $local.h1 = $root.child("scrollitems").get("h");
    $local.y1 = $root.child("scrollitems").get("y");
    $root.redraw();         // Refresh the entire window
};
```

**Widget Context Variables:**
- **`$root`**: Reference to the root widget/window
- **`$self`**: Reference to the current widget being processed
- **Use Cases**: GUI programming, widget manipulation, event handling

## Implementation Details

The namespace system is implemented in `lib/grapa/$grapa.grc`:

```grapa
| $this {@<this,{}>}
| $global {@<global,{}>}
| $local {@<local,{}>}
| $parent {@<parent,{}>}
| $root {@<var,{$1}>}
| $self {@<var,{$1}>}
```

These rules map the namespace identifiers to their corresponding C++ implementations in `GrapaLibRule.cpp`:
- `GrapaLibraryRuleThisEvent::Run`
- `GrapaLibraryRuleGlobalEvent::Run` 
- `GrapaLibraryRuleLocalEvent::Run`
- `GrapaLibraryRuleParentEvent::Run`
- `$root` and `$self` are implemented as variable lookups in `GrapaWidget.cpp`

## Common Pitfalls

### 1. Forgetting to Declare Local Variables
```grapa
// BAD: Variable might conflict with global scope
function = op() {
    result = process();  // Could overwrite global 'result'
};

// GOOD: Explicitly declare as local
function = op() {
    $local.result = process();  // Safe local variable
};
```

### 2. Assuming Thread Safety
```grapa
// BAD: Not thread-safe
sharedCounter = 0;
increment = op() {
    sharedCounter += 1;  // Race condition in parallel execution
};

// GOOD: Thread-safe
increment = op() {
    $local.counter = 0;
    $local.counter += 1;
    return $local.counter;
};
```

### 3. Nested Scope Conflicts
```grapa
// BAD: Variable name conflict
outerLoop = op() {
    i = 0;
    while (i < 10) {
        innerLoop();  // innerLoop uses 'i' - conflict!
    };
};

innerLoop = op() {
    i = 0;  // Overwrites outer loop's 'i'
    while (i < 5) {
        // Unintended behavior
    };
};

// GOOD: Use local scoping
outerLoop = op() {
    $local.i = 0;
    while ($local.i < 10) {
        innerLoop();
    };
};

innerLoop = op() {
    $local.i = 0;  // Local variable, no conflict
    while ($local.i < 5) {
        // Safe to use i
    };
};
```

### 4. Misunderstanding `$parent` Behavior
```grapa
// CONFUSING: $parent returns immediate calling scope, not function scope
f1 = op() {
    $parent;  // Returns calling scope's $local
};

f2 = op() {
    $local.a = 1;           // Function scope
    if (true) {
        $local.b = 2;       // Block scope
        f1();               // Returns {"b":2}, not {"a":1}!
    };
};

// SOLUTION: Use $parent to access function scope from nested blocks
f3 = op() {
    $local.a = 1;           // Function scope
    if (true) {
        $local.b = 2;       // Block scope
        $parent;            // Returns {"a":1} - function scope
    };
};
```

**Key Point**: `$parent` always returns the **immediate calling scope's `$local`**, not necessarily the function's `$local`. This can be confusing when functions are called from nested blocks.

## Summary

Grapa's namespace scoping system provides powerful tools for managing variable visibility and lifetime. The system includes:

**Core Namespace Variables:**
- `$global` - Global variables accessible from anywhere
- `$this` - Current object's namespace  
- `$local` - Local variables within current block scope

**Special Context Variables:**
- `$parent` - Access to calling function's local namespace
- `$root` - Reference to root widget in GUI applications
- `$self` - Reference to current widget being processed

By understanding and properly using these namespace variables, you can write robust, thread-safe code that avoids variable conflicts and maintains clear scope boundaries.

**Key Takeaways**: 
- Always declare function variables as `$local` to ensure thread safety
- Use `$parent` for function communication and callback patterns
- Use `$parent` for call stack tracing and debugging with breadcrumbs
- Use `$root` and `$self` for GUI widget manipulation
- Leverage `$local`'s automatic parameter list initialization for introspection
- Use `$parent == $global` to detect top-level function calls
