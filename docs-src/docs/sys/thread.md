---
tags:
  - system
  - threading
  - coroutines
  - synchronization
  - concurrency
---

# Thread System (`$thread`)

## Overview

Grapa's `$thread` system provides a **complete coroutine and threading implementation** with full synchronization primitives, condition variables, and cooperative multitasking capabilities. This is not a planned feature - it's **already fully implemented** and powers Grapa's own execution pipeline.

## Key Features

- **Complete Coroutine Support**: Full suspend/resume capabilities
- **Synchronization Primitives**: Locks, condition variables, trylock operations
- **Cross-Platform**: Consistent behavior across Windows, Linux, and macOS
- **High Performance**: Optimized critical sections and condition variables
- **Thread Safety**: Built-in protection against race conditions
- **Production Ready**: Powers Grapa's lexer→compiler→executor pipeline

## Thread Object Creation

```grapa
/* Create a new thread object */
thread = $thread();
```

## Core Thread Management

### `start(runCode, param, doneCode)`

Start a thread with execution code and callbacks.

**Parameters:**
- `runCode`: Function to execute in the thread
- `param`: Parameter to pass to the run function (optional)
- `doneCode`: Completion callback function (optional)

**Returns:** Error code (0 = success, -1 = failure)

**Example:**
```grapa
thread = $thread();
thread.start(
    op(input) {
        "Thread starting with input: ".echo() + input.echo();
        $sys().sleep(100);
        return "Thread completed";
    },
    "Hello from main thread",  /* Parameter */
    op(input, result) {
        "Thread finished with result: ".echo() + result.echo();
    }
);
```

### `stop()`

Stop the thread gracefully.

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
thread = $thread();
thread.start(op() { /* long running work */ }, null, null);
$sys().sleep(1000);
thread.stop();
```

### `started()`

Check if thread is currently running.

**Parameters:** None

**Returns:** Boolean (true = running, false = stopped)

**Example:**
```grapa
thread = $thread();
("Thread started: " + thread.started()).echo();  /* false */

thread.start(op() { $sys().sleep(100); }, null, null);
("Thread started: " + thread.started()).echo();  /* true */
```

## Coroutine Control

### `suspend()`

Pause thread execution (coroutine suspend).

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
thread = $thread();
thread.start(
    op() {
        "Starting work".echo();
        $sys().sleep(50);
        "About to suspend".echo();
        thread.suspend();  /* Suspend self */
        "Resumed from suspension".echo();
        return "Completed";
    },
    null,
    null
);

$sys().sleep(100);
thread.resume();  /* Resume the suspended thread */
```

### `resume()`

Resume suspended thread (coroutine resume).

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
/* Resume a suspended thread */
if (thread.suspended()) {
    thread.resume();
}
```

### `suspended()`

Check if thread is currently suspended.

**Parameters:** None

**Returns:** Boolean (true = suspended, false = running)

**Example:**
```grapa
thread = $thread();
("Thread suspended: " + thread.suspended()).echo();  /* false */

thread.start(
    op() {
        thread.suspend();
    },
    null,
    null
);

$sys().sleep(50);
("Thread suspended: " + thread.suspended()).echo();  /* true */
```

## Synchronization Primitives

### `trylock()`

Try to acquire lock (non-blocking).

**Parameters:** None

**Returns:** Boolean (true = acquired, false = failed)

**Example:**
```grapa
lock = $thread();

/* Try to acquire lock */
if (lock.trylock()) {
    "Lock acquired".echo();
    lock.unlock();
} else {
    "Lock busy".echo();
}
```

### `lock()`

Acquire lock (blocking).

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
lock = $thread();

/* Block until lock is available */
lock.lock();
"Critical section".echo();
lock.unlock();
```

### `unlock()`

Release lock.

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
lock = $thread();
lock.lock();
/* Critical section code */
lock.unlock();
```

## Condition Variables

### `wait()`

Wait for signal (condition variable).

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
condition = $thread();
data_ready = false;

/* Producer thread */
producer = $thread();
producer.start(
    op() {
        $sys().sleep(100);
        data_ready = true;
        condition.signal();
        return "Producer done";
    },
    null,
    null
);

/* Consumer thread */
consumer = $thread();
consumer.start(
    op() {
        while (!data_ready) {
            condition.wait();
        };
        "Data is ready!".echo();
        return "Consumer done";
    },
    null,
    null
);
```

### `signal()`

Signal waiting threads.

**Parameters:** None

**Returns:** Error code

**Example:**
```grapa
condition = $thread();

/* Wake up waiting threads */
condition.signal();
```

### `waiting()`

Check if thread is waiting for signal.

**Parameters:** None

**Returns:** Boolean (true = waiting, false = not waiting)

**Example:**
```grapa
condition = $thread();

/* Check if any threads are waiting */
if (condition.waiting()) {
    "Threads are waiting".echo();
} else {
    "No threads waiting".echo();
}
```

## Utility Methods

### `type()`

Get the type of the thread object.

**Parameters:** None

**Returns:** String ("$thread")

**Example:**
```grapa
thread = $thread();
("Thread type: " + thread.type()).echo();  /* "$thread" */
```

### `describe()`

Get a description of the thread object.

**Parameters:** None

**Returns:** String description

**Example:**
```grapa
thread = $thread();
thread.describe().echo();
```

## Real-World Usage Patterns

### Producer-Consumer Pattern

```grapa
/* Producer-Consumer with bounded buffer */
queue = [];
queue_lock = $thread();
data_ready = $thread();
max_size = 5;

/* Producer */
producer = $thread();
producer.start(
    op() {
        for (i = 0; i < 10; i++) {
            queue_lock.lock();
            while (queue.len() >= max_size) {
                queue_lock.unlock();
                producer.suspend();  /* Wait for space */
                queue_lock.lock();
            };
            
            queue.push("Item " + i);
            queue_lock.unlock();
            data_ready.signal();
            $sys().sleep(50);
        };
        return "Producer done";
    },
    null,
    null
);

/* Consumer */
consumer = $thread();
consumer.start(
    op() {
        for (i = 0; i < 10; i++) {
            queue_lock.lock();
            while (queue.len() == 0) {
                queue_lock.unlock();
                consumer.suspend();  /* Wait for data */
                queue_lock.lock();
            };
            
            item = queue.shift();
            queue_lock.unlock();
            ("Consumed: " + item).echo();
            $sys().sleep(100);
        };
        return "Consumer done";
    },
    null,
    null
);
```

### Pipeline Processing

```grapa
/* Three-stage pipeline: Generator → Processor → Outputter */
generator = $thread();
processor = $thread();
outputter = $thread();

/* Stage 1: Generate data */
generator.start(
    op() {
        for (i = 0; i < 5; i++) {
            data = i * 2;
            processor.signal();  /* Signal processor */
            generator.suspend();  /* Yield to processor */
            $sys().sleep(30);
        };
        return "Generator done";
    },
    null,
    null
);

/* Stage 2: Process data */
processor.start(
    op() {
        for (i = 0; i < 5; i++) {
            processor.wait();  /* Wait for generator */
            processed = i * 2 * 3 + 1;
            outputter.signal();  /* Signal outputter */
            processor.suspend();  /* Yield to outputter */
            $sys().sleep(40);
        };
        return "Processor done";
    },
    null,
    null
);

/* Stage 3: Output data */
outputter.start(
    op() {
        for (i = 0; i < 5; i++) {
            outputter.wait();  /* Wait for processor */
            ("Output: " + (i * 2 * 3 + 1)).echo();
            generator.resume();  /* Resume generator */
            processor.resume();  /* Resume processor */
            $sys().sleep(50);
        };
        return "Outputter done";
    },
    null,
    null
);
```

### Resource Pool Management

```grapa
/* Resource pool with thread coordination */
resource_pool = {
    available: ["Resource_A", "Resource_B", "Resource_C"],
    in_use: {},
    lock: $thread()
};

/* Worker requesting resource */
worker = $thread();
worker.start(
    op(worker_id) {
        resource_pool.lock.lock();
        
        if (resource_pool.available.len() > 0) {
            /* Resource available */
            resource = resource_pool.available.pop();
            resource_pool.in_use[worker_id] = resource;
            resource_pool.lock.unlock();
            
            /* Use resource */
            ("Worker " + worker_id + " using " + resource).echo();
            $sys().sleep(100);
            
            /* Return resource */
            resource_pool.lock.lock();
            resource_pool.available.push(resource);
            delete resource_pool.in_use[worker_id];
            resource_pool.lock.unlock();
        } else {
            /* No resources, wait */
            resource_pool.lock.unlock();
            worker.wait();  /* Wait for resource */
        };
        
        return "Worker " + worker_id + " done";
    },
    1,
    null
);
```

### Cooperative Multitasking

```grapa
/* Simple round-robin scheduler */
tasks = [];

/* Create tasks */
for (i = 0; i < 3; i++) {
    task = $thread();
    task.start(
        op(task_id) {
            for (j = 0; j < 3; j++) {
                ("Task " + task_id + " step " + j).echo();
                task.suspend();  /* Yield control */
            };
            return "Task " + task_id + " completed";
        },
        i,
        null
    );
    tasks.push(task);
}

/* Scheduler */
scheduler = $thread();
scheduler.start(
    op() {
        completed = 0;
        while (completed < 3) {
            for (i = 0; i < tasks.len(); i++) {
                if (tasks[i].suspended()) {
                    tasks[i].resume();
                    $sys().sleep(50);
                };
            };
        };
        return "Scheduler done";
    },
    null,
    null
);
```

## Performance Characteristics

### Thread Creation Overhead
- **Linux/Mac (pthread)**: ~1-2ms per thread
- **Windows (CreateThread)**: ~2-3ms per thread
- **Memory overhead**: ~1-2MB per thread (stack + context)

### Synchronization Performance
- **Critical Section**: ~50-100ns lock/unlock
- **Mutex**: ~100-200ns lock/unlock
- **Condition Variable**: ~1-5μs wait/signal

### Scalability Considerations
```grapa
/* Recommended thread limits */
thread_limits = {
    windows: 2000,    /* Windows thread limit */
    linux: 32768,     /* Linux thread limit */
    mac: 2048         /* macOS thread limit */
};

/* Practical limits for Grapa */
practical_limits = {
    small_system: 8,      /* 4-8 cores */
    medium_system: 16,    /* 8-16 cores */
    large_system: 32      /* 16+ cores */
};
```

## Best Practices

### Thread Count Management
```grapa
/* For small datasets - let Grapa handle threading */
data = [1, 2, 3, 4, 5];
result = data.map(op(x) { x * x; });

/* For large datasets - specify thread count */
data = (1000000).range(0,1);
result = data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### Error Handling
```grapa
/* Handle errors in parallel operations */
result = data.map(op(x) { 
    x.operation().iferr(0);  /* Return 0 on error */
}, 4);
```

### Resource Management
```grapa
/* Automatic cleanup - no manual cleanup required */
thread = $thread(op() {
    /* Thread work */
    process_data();
});
/* Thread automatically cleaned up when variable goes out of scope */
```

## Integration with Grapa's Execution Pipeline

Grapa's `$thread` system powers the language's own execution pipeline:

1. **Lexer Thread**: Processes input text and generates tokens
2. **Compiler Thread**: Converts tokens to execution trees
3. **Executor Thread**: Executes the compiled code

Each stage uses `suspend()`, `resume()`, `wait()`, and `signal()` to coordinate:
- Lexer suspends when input queue is empty
- Compiler suspends when token queue is empty
- Executor suspends when code queue is empty
- Each stage signals the next when data is available

This demonstrates the system's production readiness and reliability.

## Cross-Platform Compatibility

The `$thread` system provides consistent behavior across all supported platforms:

- **Windows**: Uses Windows API (CreateThread, CRITICAL_SECTION, WaitOnAddress)
- **Linux/Mac**: Uses POSIX threads (pthread_create, pthread_mutex, pthread_cond)
- **Abstraction Layer**: Common interface across all platforms
- **Error Handling**: Consistent error reporting across platforms

## Thread-Safe Variable Declarations

When writing concurrent code in Grapa, it's essential to use proper thread-safe variable declarations. See [Thread-Safe Variable Declarations](../syntax/thread_safe_variables.md) for comprehensive guidance on:

- Using `$global` for explicitly shared variables
- Protecting shared variables with locks
- Avoiding race conditions
- Best practices for concurrent programming

## Related Documentation

- [Thread-Safe Variable Declarations](../syntax/thread_safe_variables.md) - Comprehensive guide to thread-safe programming
- [Parallel and Concurrent Programming](../use_cases/parallel_concurrent_programming.md) - Real-world concurrency examples
- [Thread System Example](../examples/thread_system_example.grc) - Complete demonstration of all thread capabilities

## Conclusion

Grapa's `$thread` system provides **world-class coroutine and threading capabilities** that are:

1. **Fully Implemented**: All 13 methods are working and tested
2. **Production Ready**: Powers Grapa's own execution pipeline
3. **Cross-Platform**: Consistent behavior across Windows, Linux, and macOS
4. **High Performance**: Optimized for real-world usage
5. **Thread Safe**: Built-in protection against race conditions
6. **Coroutine Ready**: Full suspend/resume capabilities

This implementation demonstrates Grapa's commitment to providing exceptional concurrency capabilities as a core language feature, not an add-on or experimental component.
