# Advanced Concurrency and Callbacks

## Overview

Grapa provides sophisticated concurrency and callback capabilities that go far beyond traditional async/await patterns. The language was designed from the ground up to be **parallel by design** with advanced callback systems for complex asynchronous operations.

## Thread Safety and Parallel-by-Design Architecture

### Universal Thread Safety
Grapa is **fully thread safe** in all supported environments:
- **Command line** - Safe concurrent operations
- **Grapa shell** - Safe interactive use
- **Python/GrapaPy** - Safe integration

All built-in operations—including `map`, `filter`, `reduce`, `$thread`, and `$net`—are safe to use concurrently without special precautions.

### Automatic Variable Locking
**CRITICAL FEATURE**: Every access to a Grapa variable automatically locks/unlocks the underlying object. This was extensively stress-tested with hundreds of threads to ensure:
- No crashes under extreme concurrent access
- Cross-platform thread safety
- Worker object reliability
- Predictable behavior under load

## Advanced Callback Systems

### $thread Callback Architecture

Grapa's `$thread` system provides sophisticated callback capabilities:

```grapa
/* Basic thread with callbacks */
myRun = op(input) {
    "myRun:".echo();
    $sys().echo(@$local);
    input.c = input.a + input.b;
    "\n".echo();
    @$local;
};

myDone = op(input, result) {
    "myDone:".echo();
    $sys().echo(@$local);
    "\n".echo();
};

t = $thread();
t.start(myRun, {a:1, b:2}, myDone);
```

**Output:**
```
myRun:{"input":{"a":1,"b":2}}
myDone:{"input":{"a":1,"b":2,"c":3},"result":{"input":{"a":1,"b":2,"c":3}}}
```

### **Full Coroutine Support with Advanced Control**

Grapa's `$thread` system is a **complete coroutine implementation** with advanced control methods:

#### **Core Coroutine Methods**

```grapa
/* Create a coroutine thread */
thread = $thread(op() {
    "Starting coroutine".echo();
    thread.suspend();  /* Pause execution */
    "Resumed from suspend".echo();
    thread.wait();     /* Wait for signal */
    "Woken by signal".echo();
});

/* Control the coroutine */
thread.resume();   /* Resume from suspend */
thread.signal();   /* Wake from wait */
```

#### **Advanced Synchronization Methods**

```grapa
/* Thread synchronization primitives */
thread = $thread(op() {
    /* Try to acquire lock */
    if (thread.trylock()) {
        "Lock acquired".echo();
        /* Critical section */
        thread.unlock();  /* Release lock */
    } else {
        "Lock busy".echo();
    };
    
    /* Wait for condition */
    thread.wait();
    "Condition met".echo();
});

/* Control from another thread */
thread.lock();     /* Acquire lock (blocking) */
thread.signal();   /* Wake waiting thread */
thread.unlock();   /* Release lock */
```

#### **State Inspection Methods**

```grapa
/* Check thread state */
thread = $thread(op() {
    thread.suspend();
    "Suspended".echo();
});

/* State inspection */
if (thread.suspended()) {
    "Thread is suspended".echo();
};

if (thread.waiting()) {
    "Thread is waiting".echo();
};
```

#### **Real-World Coroutine Usage**

The lexer→compiler→executor pipeline demonstrates sophisticated coroutine usage:

```grapa
/* Lexer thread - pauses when input queue is empty */
lexer_thread = $thread(op() {
    while (true) {
        if (input_queue.empty()) {
            lexer_thread.wait();  /* Wait for input */
        };
        token = process_input();
        compiler_queue.add(token);
        compiler_thread.signal();  /* Wake compiler */
    };
});

/* Compiler thread - pauses when lexer queue is empty */
compiler_thread = $thread(op() {
    while (true) {
        if (lexer_queue.empty()) {
            compiler_thread.wait();  /* Wait for tokens */
        };
        code = compile_tokens();
        executor_queue.add(code);
        executor_thread.signal();  /* Wake executor */
    };
});

/* Executor thread - pauses when compiler queue is empty */
executor_thread = $thread(op() {
    while (true) {
        if (compiler_queue.empty()) {
            executor_thread.wait();  /* Wait for code */
        };
        execute_code();
    };
});
```

**Key Features:**
- **`suspend()`/`resume()`**: Direct coroutine control
- **`wait()`/`signal()`**: Condition variable support
- **`trylock()`/`lock()`/`unlock()`**: Thread synchronization primitives
- **`waiting()`/`suspended()`**: State inspection
- **Queue-based coordination**: Built-in support for producer-consumer patterns
- **Cooperative multitasking**: Explicit control flow without complex async patterns

### Widget Callback System

The widget system provides rich callback capabilities with object references:

```grapa
/* Widget with advanced callbacks */
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();

/* Post callback handlers */
w.set({
    on_post_start: op(o) {
        o.set({"color":"YELLOW"});
        o.redraw();
    },
    on_post_echo: op(o, data) {
        o.set({"append":data.str(), "key":"end"});
    },
    on_post_prompt: op(o, data) {
        o.set({"append":"\ngrapa> ", "key":"end"});
    },
    on_post_end: op(o, data) {
        o.set({
            "append":"\n" + data.str(),
            "key":"end",
            "color":"white",
            "cursor_state":"show",
            "cursor_color":"black"
        });
        o.redraw();
    }
});
```

### Network Callback System

Network operations support sophisticated callback patterns:

```grapa
/* Network with callbacks */
processPost = op(in) {
    {processed: in};
};

postHandler = op(in) {
    $local.data = in.split("\r").join("");
    $local.len = data.len() - data.split("\n\n")[0].len() - 2;
    if (len < 0) len = 0;
    $local.body = data.right(len);
    $local.rstr = processPost(body).str();
    "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: " + rstr.len().str() + "\r\n\r\n" + rstr;
};

postConnectHandler = op(netSession) {
    netSession.data = "";
};

postMessageHandler = op(netSession, message, hasmore) {
    netSession.data += message;
    if (hasmore == 0) {
        netSession.send(postHandler(netSession.data));
        netSession.data = "";
    };
};

n = $net();
n.onlisten(':12345', postMessageHandler, postConnectHandler);
```

## Structured Concurrency with Functional Methods

### Parallel Processing by Default

Grapa's functional methods provide **structured concurrency** that's superior to traditional async/await:

```grapa
/* Parallel by default - creates one thread per item */
small_data = [1, 2, 3, 4, 5];
squares = small_data.map(op(x) { x * x; });

/* For large datasets, limit threads to avoid resource exhaustion */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### Advanced Functional Operations

```grapa
/* Parallel map with additional data */
numbers = [1, 2, 3, 4, 5];
result = numbers.map(op(x, multiplier) { x * multiplier; }, 10);
/* Result: [10, 20, 30, 40, 50] */

/* Parallel filter with threshold */
filtered = numbers.filter(op(x, threshold) { x > threshold; }, 3);
/* Result: [4, 5] */

/* Parallel reduce with thread count */
sum = large_data.reduce(op(acc, x) { acc + x; }, 0, 4);
```

### Method Chaining with Parallel Processing

```grapa
/* Complex parallel processing pipeline */
result = (1000000).range(0,1)
    .filter(op(x) { x % 2 == 0; }, null, 8)      /* Parallel filter */
    .map(op(x) { x * x; }, null, 8)              /* Parallel map */
    .reduce(op(acc, x) { acc + x; }, 0, 4);      /* Parallel reduce */
```

## Execution Tree Metaprogramming

### Human-Readable Execution Trees

Grapa's execution trees are **human-readable and manipulable**, providing advanced metaprogramming capabilities:

```grapa
/* Compile script to execution tree */
script = "x = 5; y = x * 2; y.echo();";
tree = $sys().compile(script);

/* Execution tree is human-readable */
tree.echo();
/* Output shows the structured execution tree */
```

### Runtime Tree Manipulation

```grapa
/* Create and manipulate execution trees */
custom_tree = op(x, y) { x + y; };
tree_str = custom_tree.str();  /* Human-readable tree representation */

/* Execute tree with parameters */
result = custom_tree(5, 3);  /* Result: 8 */
```

## Advanced Concurrency Patterns

### Worker Thread Coordination with Functional Methods

Grapa's `.map()` and `.filter()` methods are not just data processing tools - they're **sophisticated concurrency primitives** that can coordinate multiple worker threads. This enables structured concurrency patterns where all threads must complete before proceeding.

#### **Thread Synchronization Barrier**

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
/* Result: [100, 200, 300, 400, 500] */
```

#### **Parallel Task Execution**

```grapa
/* Execute multiple independent tasks in parallel */
tasks = [
    op() { "Task A: Database query".echo(); sleep(2); "A done".echo(); },
    op() { "Task B: API call".echo(); sleep(1); "B done".echo(); },
    op() { "Task C: File processing".echo(); sleep(3); "C done".echo(); }
];

results = tasks.map(op(task) { task(); });
/* All tasks complete before proceeding */
("All tasks completed").echo();
```

#### **Resource Pool Coordination**

```grapa
/* Coordinate multiple resource operations */
resources = ["db1", "db2", "db3", "cache1", "cache2"].map(op(resource) {
    ("Connecting to " + resource).echo();
    /* Simulate connection */
    sleep(1);
    ("Connected to " + resource).echo();
    resource + "_connection";
});
/* All resources connected before proceeding */
("All resources ready").echo();
/* Result: ["db1_connection", "db2_connection", "db3_connection", "cache1_connection", "cache2_connection"] */
```

#### **System Initialization Pattern**

```grapa
/* Initialize multiple system components in parallel */
components = [
    op() { initialize_database(); },
    op() { initialize_cache(); },
    op() { initialize_api_server(); },
    op() { initialize_file_system(); }
];

initialized = components.map(op(init) { init(); });
/* All components initialized before proceeding */
("System ready").echo();
```

#### **Microservices Coordination**

```grapa
/* Coordinate multiple microservice calls */
services = [
    op() { call_user_service(); },
    op() { call_payment_service(); },
    op() { call_inventory_service(); }
];
results = services.map(op(service) { service(); });
/* All services respond before proceeding */
```

### Worker Pattern Integration

Grapa's worker pattern is deeply integrated throughout the system:

```grapa
/* Worker pattern for complex operations */
worker_op = op(input) {
    /* Complex processing */
    result = input.map(op(x) { x * x; });
    result;
};

/* Execute with worker thread */
t = $thread();
t.start(worker_op, large_dataset, op(result) { result.echo(); });
```

### Network-Based Distributed Processing

```grapa
/* Distributed processing across network */
distributed_op = op(data) {
    /* Process data across multiple nodes */
    processed = data.map(op(item) { process_item(item); });
    processed;
};

/* Network callback for distributed results */
network_handler = op(message, hasmore) {
    if (hasmore == 0) {
        /* Process complete message */
        result = distributed_op(message);
        send_result(result);
    };
};
```

## Comparison with Traditional Async/Await

### Grapa's Superior Approach

| Feature | Traditional Async/Await | Grapa's Approach |
|---------|------------------------|------------------|
| **Parallel Processing** | Manual Promise.all() | Automatic with .map()/.filter() |
| **Thread Safety** | Manual synchronization | Automatic variable locking |
| **Callback Context** | Limited scope | Rich object references |
| **Execution Trees** | Not accessible | Human-readable and manipulable |
| **Structured Concurrency** | Complex patterns | Built-in with functional methods |
| **Network Integration** | Separate libraries | Native $net() with callbacks |

### Why Grapa's Approach is Superior

1. **Automatic Parallelism**: `.map()` and `.filter()` are parallel by default
2. **Rich Callbacks**: Object references provide full context access
3. **Execution Trees**: Human-readable, manipulable execution structures
4. **Built-in Thread Safety**: No manual synchronization required
5. **Network Integration**: Native distributed processing capabilities

## Best Practices

### Thread Count Management

```grapa
/* For small datasets - let Grapa handle parallelism */
small_data = [1, 2, 3, 4, 5];
result = small_data.map(op(x) { x * x; });

/* For large datasets - specify thread count */
large_data = (1000000).range(0,1);
result = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### Performance Considerations: Parallel vs Sequential

**Important:** `.map()` and `.filter()` can split processing across multiple worker threads, which provides significant performance benefits for most operations. The minimal data copying cost is typically negligible compared to the computational work being performed. Consider the trade-offs:

```grapa
/* For large datasets with simple operations - use sequential */
big_data = (1000000).range();
result = [];
i = 0;
while (i < big_data.len()) {
    result += big_data[i] * 2;
    i += 1;
}

/* For smaller datasets with complex work - use parallel */
small_data = (1000).range();
result = small_data.map(op(x) { 
    /* Complex calculation that benefits from parallelization */
    complex_calculation = x * x + x.sqrt() + x.sin();
    complex_calculation;
}, 4);

/* .reduce() is more efficient - can use PTR references */
sum = big_data.reduce(op(acc, x) { acc + x; }, 0, 4);
```

**When to use each approach:**
- **Use sequential (for/while)** for large datasets with simple operations
- **Use parallel (.map/.filter)** for smaller datasets with complex operations
- **Use .reduce()** for large datasets when possible (more efficient)

### Callback Design Patterns

```grapa
/* Rich callback with object reference */
widget_callback = op(o, data) {
    /* o = widget object reference */
    o.set({"text": data.str()});
    o.redraw();
};

/* Network callback with session management */
network_callback = op(session, message, hasmore) {
    session.data += message;
    if (hasmore == 0) {
        process_complete_message(session.data);
        session.data = "";
    };
};
```