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

## Syntax

All three namespaces must be accessed with the `$` prefix:

```grapa
// Only these forms are valid:
$global.x = 5;
$this.property = "value";
$local.temp = 10;
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

### 3. Alternative: Reset `$local` to a List
You can also declare multiple local variables at once by resetting `$local` to a list:

```grapa
/* Method 1: Individual declarations */
f1 = op() {
    $local.a = 1;
    $local.b = "hi";
    $local.c = true;
    return $local;
};

/* Method 2: Reset $local to a list */
f2 = op() {
    $local = {a: 1, b: "hi", c: true};
    return $local;
};

/* Method 3: Mixed approach */
f3 = op() {
    $local.g = 1;  /* Individual declaration */
    $local = {a: 1, b: "hi"};  /* Reset to list */
    return $local;
};
```

**Note**: When you reset `$local` to a list, it replaces all existing local variables with the new list structure. This is useful for bulk initialization but be careful not to lose previously declared variables.

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

## Implementation Details

The namespace system is implemented in `lib/grapa/$grapa.grc`:

```grapa
| $this {@<this,{}>}
| $global {@<global,{}>}
| $local {@<local,{}>}
```

These rules map the namespace identifiers to their corresponding C++ implementations in `GrapaLibRule.cpp`:
- `GrapaLibraryRuleThisEvent::Run`
- `GrapaLibraryRuleGlobalEvent::Run` 
- `GrapaLibraryRuleLocalEvent::Run`

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

## Summary

Grapa's namespace scoping system provides powerful tools for managing variable visibility and lifetime. By understanding and properly using `$global`, `$this`, and `$local`, you can write robust, thread-safe code that avoids variable conflicts and maintains clear scope boundaries.

**Key Takeaway**: Always declare function variables as `$local` to ensure thread safety and avoid conflicts with parent scopes.
