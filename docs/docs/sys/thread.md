# $thread

Grapa is fully thread safe in all supported environments (command line, Grapa shell, and Python/GrapaPy). All built-in operations—including map, filter, reduce, $thread, and $net—are safe to use concurrently. Users do not need to take any special precautions for thread safety in these environments.

**Design Note:**
- All variable and data structure updates in Grapa are internally synchronized (locked) at the C++ level. This includes variables, tables, and all core datatypes.
- You will never encounter crashes or corruption from concurrent access in normal Grapa usage.
- However, if your program logic allows multiple threads to read and write the same variable or data structure, you may see *logical* race conditions (unexpected values, overwrites, etc.). This is a design consideration, not a stability issue. Minimize shared mutable state between threads unless intentional.

*Note: Only if Grapa is integrated directly into a non-thread-safe C++ host (not anticipated for normal users) would additional thread safety considerations arise.*

---

## Locking and Shared State

- **Only `$thread()` objects provide explicit locking and unlocking via `lock()`, `unlock()`, and `trylock()`.**
- To protect access to a shared resource, create a `$thread()` lock object and use it to guard access.
- Calling `.lock()` or `.unlock()` on a regular variable (like an array or scalar) will return an error.

**Canonical Example:**
```grapa
lock_obj = $thread();
lock_obj.lock();
/* ... perform thread-safe operations on shared data ... */
lock_obj.unlock();
```

If you want to protect access to a shared variable, use a `$thread()` lock object for that purpose.

---

## Best Practices for Parallelism
- Prefer passing data by value or using thread-local variables.
- Minimize shared mutable state between threads. If you must share, use a `$thread()` lock object to guard access.
- Immutable data (or data marked `const`) is safe to share, but note that `const` is for performance/caching, not for locking (see below).
- For compile-time constants, use `static` (see [Function Operators](../operators/function.md)).

---

## Reference: Locking Methods (on $thread objects)
- `trylock()`: Attempts to acquire a lock, returns immediately.
- `lock()`: Acquires a lock, waits if necessary.
- `unlock()`: Releases the lock.
- `wait()`, `signal()`, `waiting()`: Thread coordination primitives.

---

## Thread Creation and Usage

Provides a thread library, cross functional with all platforms supported.

### type()
$thread

### start(runOp, input, doneOp)
Starts the runOp in the background, passing paramList. When the thread exists, doneOp is called. All 3 inputs are copied, as the originals are likely to go away after running the start command. So if an instance of object is passed in, the thread will end up using a copied instance and not the original instance.

The thread is run from the same namespace as where it is called. To use a shared object instance, access the variable from within the thread rather than passing in the variable. Or pass in the $ID for the variable and difference the variable from the thread (which essentially does the same thing as a variable lookup but allows a different variable name to be used). 

If accessing shared resources from within a thread, take care and use thread safe logic, such as a `$thread()` lock object.

**Example:**
```grapa
myRun = op(input) {"myRun:".echo();$sys().echo(@$local); input.c = input.a+input.b; "\n".echo(); @$local;};
myDone = op(input,result) {"myDone:".echo();$sys().echo(@$local); "\n".echo();};
t = $thread();
t.start(myRun,{a:1,b:2},myDone);
```

Output:
```
myRun:{"input":{"a":1,"b":2}}
myDone:{"input":{"a":1,"b":2,"c":3},"result":{"input":{"a":1,"b":2,"c":3}}}
```

The input parameter is passed to both the run op and done op. The done op also receives any output from the run op.

---

## Other Thread Methods

- `stop()`: Stops the thread.
- `started()`: Indicates the running state of the thread.
- `suspend()`: Suspends the thread. If the thread is processing a queue and the queue is empty, put the thread in suspend mode. Then after pushing data onto the queue, call resume to have the thread resume processing.
- `resume()`: See suspend.
- `suspended()`: Indicates whether the thread is in a suspended state.

---

## See Also
- [Function Operators: static and const](../operators/function.md)
- [Parallelism Examples](../use_cases/index.md)
