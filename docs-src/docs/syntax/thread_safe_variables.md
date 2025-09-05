---
tags:
  - syntax
  - threading
  - concurrency
  - variables
  - thread-safety
---

# Thread-Safe Variable Declarations

## Overview

When writing concurrent code in Grapa, it's essential to understand how to properly declare and use variables that will be accessed by multiple threads. This document covers the thread-safe variable declaration patterns and best practices.

## ✅ **Sleep Function in Multi-Threaded Code**

**Good News**: Grapa's `.sleep()` function works correctly in multi-threaded applications with proper thread-local behavior:

- **Sleep calls are thread-local** - each thread can sleep independently
- **Concurrent sleep calls work correctly** - multiple threads can sleep simultaneously
- **Sleep duration is respected per thread** - each thread sleeps for its specified duration
- **No blocking between threads** - one thread's sleep doesn't block others

**Best Practice**: You can safely use `.sleep()` in multi-threaded code. It works well with `suspend()`, `resume()`, and condition variables for comprehensive thread coordination. See [Thread System](../sys/thread.md) for detailed information and examples.

## Variable Declaration Patterns

### 1. **Global Variables (Thread-Safe by Default)**

All variables declared at the global scope are automatically thread-safe in Grapa:

```grapa
/* Global variables are automatically thread-safe */
shared_counter = 0;
shared_data = [];
shared_flag = false;

/* These can be safely accessed from multiple threads */
```

### 2. **Local Variables (Thread-Local)**

Variables declared within a function or thread are local to that execution context:

```grapa
/* Local variables are thread-local and don't need special handling */
thread = $thread();
thread.start(
    op() {
        local_var = 42;  /* This is local to this thread */
        "Thread-local variable: ".echo() + local_var.echo();
        return "Done";
    },
    null,
    null
);
```

### 3. **Explicit Thread-Safe Declarations**

For variables that need to be shared between threads, use explicit declarations:

```grapa
/* Explicit thread-safe variable declarations */
$global shared_counter = 0;
$global shared_queue = [];
$global shared_lock = $thread();

/* These are explicitly marked as thread-safe */
```

## Best Practices for Thread-Safe Programming

### 1. **Use Explicit Global Declarations for Shared State**

```grapa
/* GOOD: Explicit global declarations for shared state */
$global shared_counter = 0;
$global shared_data = [];
$global data_lock = $thread();

/* BAD: Implicit global variables in concurrent code */
counter = 0;  /* Could cause race conditions */
data = [];    /* Not explicitly thread-safe */
```

### 2. **Protect Shared Variables with Locks**

```grapa
/* Thread-safe counter with lock protection */
$global counter = 0;
$global counter_lock = $thread();

/* Worker thread */
worker = $thread();
worker.start(
    op() {
        counter_lock.lock();
        counter++;
        current = counter;
        counter_lock.unlock();
        ("Counter incremented to: " + current).echo();
        return "Done";
    },
    null,
    null
);
```

### 3. **Use Condition Variables for Coordination**

```grapa
/* Thread-safe producer-consumer with condition variables */
$global queue = [];
$global queue_lock = $thread();
$global data_ready = $thread();
$global space_available = $thread();

/* Producer */
producer = $thread();
producer.start(
    op() {
        for (i = 0; i < 10; i++) {
            queue_lock.lock();
            while (queue.len() >= 5) {
                queue_lock.unlock();
                producer.suspend();  /* Wait for space */
                queue_lock.lock();
            };
            queue.push("Item " + i);
            queue_lock.unlock();
            data_ready.signal();
        };
        return "Producer done";
    },
    null,
    null
);
```

### 4. **Avoid Mutable Shared State When Possible**

```grapa
/* GOOD: Immutable data sharing */
$global config = {
    max_threads: 8,
    timeout: 5000,
    debug: false
};

/* BAD: Mutable shared state */
$global current_state = "unknown";  /* Could cause race conditions */
```

## Common Patterns and Anti-Patterns

### **Thread-Safe Counter Pattern**

```grapa
/* Thread-safe counter implementation */
$global counter = 0;
$global counter_lock = $thread();

increment_counter = op() {
    counter_lock.lock();
    counter++;
    result = counter;
    counter_lock.unlock();
    return result;
};

/* Multiple threads can safely call increment_counter() */
```

### **Thread-Safe Queue Pattern**

```grapa
/* Thread-safe queue implementation */
$global queue = [];
$global queue_lock = $thread();
$global not_empty = $thread();
$global not_full = $thread();

enqueue = op(item) {
    queue_lock.lock();
    queue.push(item);
    queue_lock.unlock();
    not_empty.signal();
};

dequeue = op() {
    queue_lock.lock();
    while (queue.len() == 0) {
        queue_lock.unlock();
        not_empty.wait();
        queue_lock.lock();
    };
    item = queue.shift();
    queue_lock.unlock();
    return item;
};
```

### **Thread-Safe Resource Pool Pattern**

```grapa
/* Thread-safe resource pool */
$global resource_pool = {
    available: ["Resource_A", "Resource_B", "Resource_C"],
    in_use: {},
    lock: $thread()
};

acquire_resource = op(worker_id) {
    resource_pool.lock.lock();
    if (resource_pool.available.len() > 0) {
        resource = resource_pool.available.pop();
        resource_pool.in_use[worker_id] = resource;
        resource_pool.lock.unlock();
        return resource;
    } else {
        resource_pool.lock.unlock();
        return null;
    };
};

release_resource = op(worker_id) {
    resource_pool.lock.lock();
    if (resource_pool.in_use[worker_id]) {
        resource = resource_pool.in_use[worker_id];
        resource_pool.available.push(resource);
        delete resource_pool.in_use[worker_id];
        resource_pool.lock.unlock();
        return true;
    } else {
        resource_pool.lock.unlock();
        return false;
    };
};
```

## Anti-Patterns to Avoid

### **1. Implicit Global Variables in Concurrent Code**

```grapa
/* BAD: Implicit global variables */
thread1 = $thread();
thread1.start(
    op() {
        counter = 0;  /* This creates an implicit global! */
        counter++;
        return "Done";
    },
    null,
    null
);

thread2 = $thread();
thread2.start(
    op() {
        counter++;  /* Race condition! */
        return "Done";
    },
    null,
    null
);
```

### **2. Unprotected Shared State**

```grapa
/* BAD: Unprotected shared state */
$global shared_data = [];

thread1 = $thread();
thread1.start(
    op() {
        shared_data.push("Data from thread 1");  /* Race condition! */
        return "Done";
    },
    null,
    null
);

thread2 = $thread();
thread2.start(
    op() {
        shared_data.push("Data from thread 2");  /* Race condition! */
        return "Done";
    },
    null,
    null
);
```

### **3. Missing Line Feeds in Output**

```grapa
/* BAD: Missing line feeds */
("Counter value: " + counter).echo();  /* No line feed */

/* GOOD: With line feeds */
("Counter value: " + counter + "\n").echo();
```

## Thread-Safe Variable Declaration Checklist

When writing concurrent code, ensure you:

1. **✅ Use `$global` for explicitly shared variables**
2. **✅ Protect shared variables with locks**
3. **✅ Use condition variables for coordination**
4. **✅ Add line feeds to output statements**
5. **✅ Avoid implicit global variables**
6. **✅ Minimize shared mutable state**
7. **✅ Use immutable data when possible**
8. **✅ Test with multiple threads**

## Example: Complete Thread-Safe Application

```grapa
/* Complete thread-safe application example */
$global shared_counter = 0;
$global shared_queue = [];
$global queue_lock = $thread();
$global data_ready = $thread();
$global workers_complete = 0;
$global completion_lock = $thread();

/* Producer thread */
producer = $thread();
producer.start(
    op() {
        for (i = 0; i < 10; i++) {
            queue_lock.lock();
            shared_queue.push("Item " + i);
            queue_lock.unlock();
            data_ready.signal();
            ("Produced item " + i + "\n").echo();
            $sys().sleep(100);
        };
        return "Producer completed\n";
    },
    null,
    op(input, result) {
        ("Producer callback: " + result).echo();
    }
);

/* Consumer threads */
for (worker_id = 0; worker_id < 3; worker_id++) {
    consumer = $thread();
    consumer.start(
        op(id) {
            items_processed = 0;
            while (items_processed < 4) {
                queue_lock.lock();
                while (shared_queue.len() == 0) {
                    queue_lock.unlock();
                    data_ready.wait();
                    queue_lock.lock();
                };
                item = shared_queue.shift();
                queue_lock.unlock();
                
                ("Worker " + id + " processed: " + item + "\n").echo();
                items_processed++;
                $sys().sleep(50);
            };
            
            completion_lock.lock();
            workers_complete++;
            completion_lock.unlock();
            
            return "Worker " + id + " completed\n";
        },
        worker_id,
        op(input, result) {
            ("Worker callback: " + result).echo();
        }
    );
};
```

## Summary

Thread-safe programming in Grapa requires:

1. **Explicit variable declarations** using `$global` for shared state
2. **Proper synchronization** using locks and condition variables
3. **Consistent output formatting** with line feeds
4. **Careful design** to minimize shared mutable state
5. **Thorough testing** with multiple concurrent threads

By following these patterns, you can write robust, thread-safe concurrent applications in Grapa.
