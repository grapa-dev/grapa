---
tags:
  - maintainer
  - implementation
  - system-components
  - concurrency
  - threading
  - memory-model
  - session-management
  - distributed-computing
---

# Concurrency Model Architecture

This document provides comprehensive coverage of Grapa's concurrency model, from the automatic variable locking system to the architectural foundation for distributed multi-system concurrency.

## Overview

**Purpose:** Rock-solid concurrency with automatic thread safety
**Status:** Production-ready with foundation for future enhancements
**Architecture:** Parallel-by-design with automatic variable locking
**Key Features:** Session isolation, static optimization, distributed concurrency foundation

## Core Concurrency Principles

### **1. Automatic Variable Locking**

**CRITICAL IMPLEMENTATION DETAIL:** Every access to a Grapa variable automatically locks/unlocks the underlying object. This fundamental design ensures:

```cpp
// Every variable access is automatically thread-safe
GrapaInt x = 10;           // Automatic locking on creation
x = x + 5;                 // Automatic locking on read/write
y = x * 2;                 // Automatic locking on read
```

**Benefits:**
- **No race conditions** - Impossible to have concurrent access conflicts
- **Transparent safety** - No explicit locking code required
- **Cross-platform reliability** - Tested with hundreds of threads
- **Predictable behavior** - Consistent performance under load

### **2. Parallel-by-Design Architecture**

Grapa was designed from the ground up for concurrency with multiple threading approaches:

#### **Functional Method Parallelism**
```grapa
/* Parallel by default - creates one thread per item */
small_data = [1, 2, 3, 4, 5];
squares = small_data.map(op(x) { x * x; });

/* For large datasets, limit threads to avoid resource exhaustion */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */

/* Parallel filtering */
filtered = data.filter(op(x) { x > 100; }, 4);  /* 4 threads */

/* Parallel reduction (with thread coordination) */
sum = data.reduce(op(acc, x) { acc + x; }, 4);  /* 4 threads */
```

#### **Grep Parallelism**
```grapa
/* Parallel grep processing */
results = $grep("pattern", "large_file.txt", 8);  /* 8 threads */

/* Parallel grep with custom options */
results = $grep("regex_pattern", "*.txt", {
    threads: 16,           /* 16 parallel threads */
    recursive: true,       /* Process subdirectories */
    case_insensitive: true /* Case-insensitive matching */
});
```

#### **Explicit Threading with $thread**
```grapa
/* Explicit thread creation for custom parallel work */
thread1 = $thread(op() {
    /* Custom parallel work */
    result1 = process_data_chunk(chunk1);
});

thread2 = $thread(op() {
    /* Another parallel task */
    result2 = process_data_chunk(chunk2);
});

/* Wait for completion */
thread1.wait();
thread2.wait();
```

#### **4. Network Socket Listener Threading ($net)**
```grapa
/* Socket listener with multi-session threading */
server = $net();

/* Start listening with callback handlers */
server.onlisten("tcp://localhost:8080", 
    /* Message handler - called for each received message */
    op(message, connection) {
        /* Process message in separate thread */
        response = process_message(message);
        connection.send(response);
    },
    /* Connect handler - called when new client connects */
    op(connection) {
        /* Handle new connection in separate thread */
        log("New client connected: " + connection.host());
    },
    16  /* Maximum concurrent sessions */
);

/* Each client connection runs in its own thread */
/* Automatic session management and cleanup */
```

#### **5. Python Integration Threading (grapapy)**
```grapa
/* Python callback integration with threading */
/* Register Python callback function */
$global['python_callback'] = $py.callback(python_function);

/* Use Python callback in parallel processing */
data = [1, 2, 3, 4, 5, 6, 7, 8];
results = data.map(op(x) {
    /* This runs in parallel threads */
    /* Each thread calls back to Python with automatic locking */
    return $global['python_callback'](x);
}, 4);  /* 4 threads */

/* Python callbacks work with all threading approaches */
grep_results = $grep("pattern", "file.txt", {
    callback: $global['python_callback'],
    threads: 8
});
```

**Key Characteristics:**
- **Multiple threading approaches** - Functional methods, grep, explicit threads, network listeners, Python integration
- **Automatic thread management** with resource limits
- **Worker pattern integration** throughout the system
- **Cross-platform threading** (Windows CreateThread, Linux/Mac pthread)
- **Intelligent thread allocation** - Different strategies for different operations
- **Session-based concurrency** - Network listeners with per-session threading
- **Python threading bridge** - Threading capabilities within single-threaded Python

## Memory Model and Thread Safety

### **Deterministic Memory Management**

Grapa's memory management works seamlessly with concurrency:

```cpp
// Deterministic cleanup prevents memory leaks in concurrent scenarios
{
    GrapaInt x = 1000000;
    // Multiple threads can safely access x
    // Automatic locking ensures thread safety
    // Deterministic cleanup when scope ends
}
// No garbage collection needed - no dangling references
```

**Concurrency Benefits:**
- **No garbage collection pauses** - Deterministic cleanup
- **No memory leaks** - Scope-based resource management
- **No dangling references** - PTR system prevents invalid access
- **Predictable performance** - No GC interference with concurrent operations

### **Python Integration Threading**

Grapa's multiprocessing capabilities can be used within Python's single-threaded environment:

```python
import grapapy
g = grapapy.grapa()

# Python is single-threaded, but Grapa provides threading
def python_callback(data):
    # This callback is called from Grapa threads
    # Grapa automatically applies a lock to prevent multiple threads
    # from calling back into Python simultaneously
    print(f"Processing data: {data}")
    return data * 2

# Register Python callback with Grapa
g.eval("$global['python_callback'] = $py.callback(python_callback)")

# Use Grapa's threading capabilities from Python
g.eval("""
    // Parallel processing with Python callback
    data = [1, 2, 3, 4, 5, 6, 7, 8];
    results = data.map(op(x) {
        // This runs in parallel threads
        // Each thread calls back to Python with automatic locking
        return $global['python_callback'](x);
    }, 4);  // 4 threads
""")

# Get results back to Python
results = g.eval("results")
print(f"Results: {results}")
```

**Python Threading Benefits:**
- **Threading in single-threaded Python** - Grapa provides true parallelism
- **Automatic callback locking** - Prevents multiple threads calling Python simultaneously
- **Seamless integration** - Python callbacks work with all Grapa threading approaches
- **Performance improvement** - Parallel processing within Python applications

### **Global Variable Concurrency**

**IMPORTANT:** Global variables can be safely accessed by multiple threads due to automatic locking:

```grapa
/* Global variables are thread-safe by default */
$global['shared_counter'] = 0;

/* Multiple threads can safely access and modify */
thread1 = $thread(op() {
    $global['shared_counter'] += 1;  /* Automatic locking */
});

thread2 = $thread(op() {
    $global['shared_counter'] += 1;  /* Automatic locking */
});

/* Both threads can run concurrently without race conditions */
```

**Safety Guarantees:**
- **Atomic operations** - Each variable access is atomic
- **Consistent state** - No partial updates or corruption
- **Cross-thread visibility** - Changes visible to all threads
- **No explicit synchronization** - Automatic handling

## Session Management Architecture

### **Session Concept Foundation**

Grapa has a **session concept** that was fully implemented but temporarily stripped for release. The foundation remains:

```cpp
// Session isolation provides separate execution contexts
// Each session has its own:
// - LEX/compiler pipeline
// - Variable namespace
// - Thread pool
// - Resource management
```

**Architectural Components:**
- **Session isolation** - Separate execution contexts
- **Independent LEX/compiler pipelines** - No interference between sessions
- **Resource isolation** - Memory and thread pools per session
- **Cross-session communication** - Foundation for distributed computing

### **Network Session Management**

The `$net` socket listener implements a **session-based concurrency model**:

```cpp
// Network session management in GrapaNetConnect
class GrapaNetConnect : public GrapaThread {
private:
    GrapaRuleQueue mListeners;        // Active session connections
    GrapaRuleEvent* vReceiveLambda;   // Per-session message handler
    GrapaRuleEvent* vConnectLambda;   // Per-session connect handler
    
public:
    // Each client connection becomes a separate session thread
    void CreateSession() {
        GrapaRuleEvent* newNet = new GrapaRuleEvent(GrapaTokenType::OBJ);
        newNet->vNetConnect = new GrapaNetConnect();
        newNet->vNetConnect->mListenChild = true;  // Session flag
        
        // Start session in separate thread
        newNet->vNetConnect->Start(false);
        
        // Add to active sessions
        mListeners.PushTail(newNet);
    }
    
    // Automatic session cleanup
    void ManageSessions() {
        // Remove completed sessions
        // Clean up resources
        // Maintain session count limits
    }
};
```

**Network Session Features:**
- **Per-connection threading** - Each client connection runs in its own thread
- **Callback-based processing** - Message and connect handlers for each session
- **Automatic session management** - Creation, cleanup, and resource management
- **Session isolation** - Each session has independent execution context
- **Concurrent session limits** - Configurable maximum concurrent sessions

### **Future Session Implementation**

The session system will enable:

```grapa
/* Future session-based concurrency */
session1 = $session();  /* Create isolated session */
session2 = $session();  /* Another isolated session */

/* Sessions can run concurrently without interference */
session1.eval("x = 100; y = x * 2;");
session2.eval("a = 200; b = a / 2;");

/* Cross-session communication */
session1.send(session2, "data", [1, 2, 3, 4, 5]);
```

**Benefits:**
- **Complete isolation** - Sessions don't interfere with each other
- **Independent scaling** - Each session can use full system resources
- **Fault tolerance** - Session failure doesn't affect others
- **Resource management** - Per-session resource allocation

## Static Optimization

### **Static Qualifier for Performance**

The `static` qualifier optimizes large table access by making it read-only:

```grapa
/* Standard table access - automatic locking on every access */
large_table = $file("huge_dataset.csv").table();
result = large_table.filter(op(row) { row[0] > 100; });  /* Locked access */

/* Static optimization - read-only, no locking overhead */
static_table = static $file("huge_dataset.csv").table();
result = static_table.filter(op(row) { row[0] > 100; });  /* No locking */
```

**Performance Benefits:**
- **Eliminates locking overhead** - No lock/unlock cycles
- **Enables concurrent read access** - Multiple threads can read simultaneously
- **Significant performance improvement** - Especially for large datasets
- **Memory efficiency** - Shared read-only access

### **Static Implementation Details**

```cpp
// Static qualifier implementation
class GrapaStaticTable : public GrapaTable {
private:
    bool mIsStatic;  // Read-only flag
    
public:
    // Static tables skip locking operations
    virtual GrapaRuleEvent* Get(GrapaRuleEvent* pIndex) {
        if (mIsStatic) {
            // Direct access without locking
            return DirectAccess(pIndex);
        } else {
            // Standard locked access
            return LockedAccess(pIndex);
        }
    }
};
```

**Usage Guidelines:**
- **Use static for read-only operations** - Tables that won't be modified
- **Apply to large datasets** - Where locking overhead is significant
- **Ensure thread safety** - Only use when concurrent modification is impossible
- **Performance profiling** - Measure improvement for your specific use case

## Distributed Concurrency Foundation

### **Multi-System Concurrency Architecture**

Grapa has the foundation for distributed multi-system concurrency:

```cpp
// Architectural foundation for distributed computing
class GrapaDistributedSession {
private:
    std::vector<GrapaSession*> mRemoteSessions;
    GrapaCritical mCoordinationLock;
    
public:
    // Distribute work across multiple systems
    void DistributeWork(GrapaWorkEvent* pWork);
    
    // Wait for all remote sessions to complete
    void WaitForCompletion();
    
    // Aggregate results from all systems
    GrapaRuleEvent* AggregateResults();
};
```

**Historical Implementation:**
- **Network-based session coordination** - Work distributed across systems
- **Automatic job completion** - When all sessions finish
- **Result aggregation** - Combined results from all systems
- **Fault tolerance** - System failure handling

### **Distributed Concurrency Patterns**

```grapa
/* Future distributed concurrency patterns */
distributed_job = $distributed_job();

/* Add remote systems */
distributed_job.add_system("192.168.1.10:8080");
distributed_job.add_system("192.168.1.11:8080");
distributed_job.add_system("192.168.1.12:8080");

/* Distribute work */
distributed_job.distribute(large_dataset, op(item) {
    return process_item(item);
});

/* Wait for all systems to complete */
results = distributed_job.wait_for_completion();
```

**Architectural Components:**
- **Session coordination** - Cross-system session management
- **Work distribution** - Automatic load balancing
- **Result aggregation** - Combined results from all systems
- **Fault tolerance** - Handling system failures
- **Network communication** - Inter-system messaging

## Threading Implementation

### **Multiple Threading Approaches**

Grapa provides several ways to leverage multiple threads, each optimized for different use cases:

#### **1. Functional Method Parallelism**
```cpp
// .map(), .filter(), .reduce() automatically use worker threads
class GrapaFunctionalParallelism {
public:
    // Parallel map implementation
    GrapaRuleEvent* ParallelMap(GrapaRuleEvent* pData, GrapaRuleEvent* pFunction, int pThreadCount);
    
    // Parallel filter implementation  
    GrapaRuleEvent* ParallelFilter(GrapaRuleEvent* pData, GrapaRuleEvent* pFunction, int pThreadCount);
    
    // Parallel reduce implementation
    GrapaRuleEvent* ParallelReduce(GrapaRuleEvent* pData, GrapaRuleEvent* pFunction, int pThreadCount);
};
```

#### **2. Grep Parallelism**
```cpp
// $grep uses specialized parallel processing for text search
class GrapaGrepParallelism {
private:
    std::vector<GrapaWorker*> mGrepWorkers;
    GrapaWorkQueue mGrepQueue;
    
public:
    // Parallel grep processing
    GrapaRuleEvent* ParallelGrep(const char* pPattern, const char* pFile, int pThreadCount);
    
    // File chunk distribution for grep workers
    void DistributeFileChunks(const char* pFile, int pChunkSize);
};
```

#### **3. Explicit Threading ($thread)**
```cpp
// Direct thread creation for custom parallel work
class GrapaExplicitThreading {
public:
    // Create explicit thread
    GrapaThread* CreateThread(GrapaRuleEvent* pFunction);
    
    // Thread coordination and synchronization
    void WaitForThread(GrapaThread* pThread);
    void TerminateThread(GrapaThread* pThread);
};
```

#### **4. Network Socket Listener Threading ($net)**
```cpp
// Socket listener with multi-session threading
class GrapaNetConnect : public GrapaThread {
private:
    GrapaRuleQueue mListeners;        // Active session connections
    bool mListenMode, mListenChild;   // Listener mode flags
    GrapaRuleEvent* vReceiveLambda;   // Message handler callback
    GrapaRuleEvent* vConnectLambda;   // Connect handler callback
    
public:
    // Start listening with callback handlers
    GrapaError OnListen(GrapaScriptExec* pScriptExec, GrapaRuleEvent* pUrl, 
                       GrapaRuleEvent* pReceiveLambda, GrapaRuleEvent* pConnectLambda);
    
    // Main listener loop - creates new session threads
    virtual void Running();
    
    // Session management
    void ManageSessions();  // Clean up completed sessions
    void CreateSession();   // Create new session thread
};
```

#### **5. Python Integration Threading (grapapy)**
```cpp
// Python callback integration with automatic locking
class GrapaPyCallback {
private:
    PyObject* mPythonFunction;        // Python callback function
    GrapaCritical mCallbackLock;      // Lock for Python callback access
    
public:
    // Register Python callback function
    GrapaError RegisterCallback(PyObject* pFunction);
    
    // Call Python function with automatic locking
    GrapaRuleEvent* CallPython(GrapaRuleEvent* pArgs) {
        mCallbackLock.WaitCritical();  // Prevent multiple threads calling Python
        
        // Call Python function with GIL management
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyObject* result = PyObject_CallObject(mPythonFunction, pArgs);
        PyGILState_Release(gstate);
        
        mCallbackLock.LeaveCritical();
        return ConvertPythonResult(result);
    }
};
```

### **Core Threading Classes**

#### **GrapaCritical (Base Locking)**
```cpp
class GrapaCritical {
private:
    GrapaThreadLock mLock;
    GrapaThreadCondition mCondition;
    
public:
    // Core locking operations
    bool TryCritical();      // Try to acquire lock
    void WaitCritical();     // Wait for lock
    void LeaveCritical();    // Release lock
    void WaitCondition();    // Wait for condition
    void SendCondition();    // Signal condition
};
```

#### **GrapaThread (Low-Level Threading)**
```cpp
class GrapaThread : public GrapaCritical {
private:
    GrapaCritical mRunning, mSuspend;
    ThreadHandle mHandle;
    
public:
    // Cross-platform thread management
    bool Start(GrapaThreadFunction pFunction, void* pData);
    void Suspend();
    void Resume();
    void Terminate();
};
```

#### **GrapaWorker (High-Level Threading)**
```cpp
class GrapaWorker : public GrapaThread {
private:
    GrapaWorkQueue* mWorkQueue;
    GrapaScriptExec* mScriptExec;
    
public:
    // Integrated with Grapa script execution
    void ProcessWork(GrapaWorkEvent* pWork);
    void SetScriptContext(GrapaScriptExec* pExec);
};
```

### **Worker Pattern Integration**

The worker pattern is deeply integrated throughout Grapa, enabling multiple threading approaches:

```cpp
// Worker pattern for parallel processing
class GrapaWorkEvent : public GrapaEvent, public GrapaThread {
private:
    GrapaWorker* mWorker;
    GrapaRuleEvent* mWorkData;
    
public:
    // Execute work in parallel
    virtual void Execute();
    
    // Coordinate with other workers
    void Coordinate(GrapaWorkEvent* pOther);
};
```

**Usage Patterns:**
- **Functional method parallelism** - `.map()`, `.filter()`, `.reduce()` with worker threads
- **Grep parallelization** - Multiple workers process file chunks in parallel
- **Database operations** - Parallel query execution across multiple workers
- **File processing** - Concurrent file operations with worker coordination
- **Network operations** - Parallel network requests with worker management
- **Custom threading** - `$thread` operations using worker infrastructure
- **Network listeners** - Socket listeners with per-session threading and callbacks
- **Python integration** - Threading capabilities within single-threaded Python

## Performance Optimization

### **Thread Pool Management**

```cpp
// Thread pool for efficient resource management
class GrapaThreadPool {
private:
    std::vector<GrapaWorker*> mWorkers;
    GrapaWorkQueue mWorkQueue;
    GrapaCritical mPoolLock;
    
public:
    // Efficient thread reuse
    GrapaWorker* GetWorker();
    void ReturnWorker(GrapaWorker* pWorker);
    
    // Load balancing
    void DistributeWork(GrapaWorkEvent* pWork);
};
```

### **Concurrency Performance Patterns**

```grapa
/* Optimal concurrency patterns */

/* 1. Functional method parallelism */
optimal_threads = min(8, data.len());  /* Limit thread count */
result = data.map(op(x) { x * x; }, optimal_threads);

/* 2. Grep parallelism for text processing */
grep_results = $grep("pattern", "large_file.txt", optimal_threads);

/* 3. Static optimization for read-only access */
static_data = static $file("large_file.csv").table();
result = static_data.filter(op(row) { row[0] > 100; });

/* 4. Chunked processing for large datasets */
chunks = data.chunk(1000);  /* Process in chunks */
results = chunks.map(op(chunk) { process_chunk(chunk); }, 4);

/* 5. Pipeline processing with mixed parallelism */
pipeline = data
    .filter(op(x) { x > 0; }, 4)      /* Parallel filter */
    .map(op(x) { x * 2; }, 4)         /* Parallel map */
    .reduce(op(acc, x) { acc + x; }); /* Sequential reduce */

/* 6. Mixed threading approaches */
// Use grep for text processing
text_results = $grep("error", "*.log", { threads: 8 });

// Use functional methods for data processing
data_results = large_dataset.map(op(row) { process_row(row); }, 8);

// Use explicit threads for custom work
thread1 = $thread(op() { custom_work1(); });
thread2 = $thread(op() { custom_work2(); });
thread1.wait(); thread2.wait();

/* 7. Network listener with session threading */
server = $net();
server.onlisten("tcp://localhost:8080", 
    op(message, conn) { process_message(message); },
    op(conn) { log("Connected: " + conn.host()); },
    16  /* 16 concurrent sessions */
);

/* 8. Python integration threading */
// From Python: g.eval("$global['callback'] = $py.callback(python_function)")
results = data.map(op(x) { 
    return $global['callback'](x);  /* Thread-safe Python callback */
}, 8);
```

## Error Handling and Recovery

### **Concurrent Error Handling**

```cpp
// Error handling in concurrent scenarios
try {
    // Concurrent operations with automatic error propagation
    results = data.map(op(x) {
        if (x < 0) {
            throw "Negative value not allowed";
        }
        return x * x;
    }, 8);
} catch (error) {
    // Error handling for concurrent operations
    log("Concurrent operation failed: " + error);
    // Automatic cleanup of worker threads
}
```

### **Fault Tolerance Patterns**

```grapa
/* Fault tolerance in distributed scenarios */
distributed_job = $distributed_job();

/* Add error handling */
distributed_job.on_error(op(error, system) {
    log("System " + system + " failed: " + error);
    // Automatically redistribute work to other systems
});

/* Retry logic */
distributed_job.retry_failed_work(3);  /* Retry failed work 3 times */
```

## Future Enhancements

### **Planned Concurrency Features**

1. **Session System Restoration**
   - Complete session isolation implementation
   - Cross-session communication
   - Session resource management

2. **Distributed Computing**
   - Network-based session coordination
   - Automatic work distribution
   - Result aggregation across systems

3. **Advanced Synchronization**
   - Read-write locks for better performance
   - Lock-free data structures
   - Atomic operations optimization

4. **Concurrency Monitoring**
   - Thread performance profiling
   - Deadlock detection
   - Resource usage monitoring

### **Architectural Evolution**

```cpp
// Future concurrency architecture
class GrapaAdvancedConcurrency {
public:
    // Session-based concurrency
    GrapaSession* CreateSession();
    void DestroySession(GrapaSession* pSession);
    
    // Distributed computing
    void AddRemoteSystem(const char* pAddress);
    void DistributeWork(GrapaWorkEvent* pWork);
    
    // Advanced synchronization
    GrapaReadWriteLock* CreateReadWriteLock();
    GrapaLockFreeQueue* CreateLockFreeQueue();
};
```

## Related Documentation

### **Implementation Documentation**
- **[GRAPA_THREADING_ARCHITECTURE.md](GRAPA_THREADING_ARCHITECTURE.md)** - Basic threading architecture
- **[../CORE/MEMORY_MANAGEMENT_ARCHITECTURE.md](../CORE/MEMORY_MANAGEMENT_ARCHITECTURE.md)** - Memory management with concurrency
- **[../TYPE_SYSTEM/PTR_TYPE_AND_REFERENCE_SYSTEM.md](../TYPE_SYSTEM/PTR_TYPE_AND_REFERENCE_SYSTEM.md)** - PTR system and concurrency

### **User Documentation**
- **[../../docs-src/docs/sys/thread.md](../../docs-src/docs/sys/thread.md)** - Thread usage guide
- **[../../docs-src/docs/use_cases/parallel_concurrent_programming.md](../../docs-src/docs/use_cases/parallel_concurrent_programming.md)** - Parallel programming examples

## Notes

- **Rock-solid foundation**: Extensive testing with hundreds of threads
- **Automatic safety**: No explicit locking required for thread safety
- **Performance optimized**: Static qualifier for read-only optimization
- **Future-ready**: Foundation for distributed multi-system concurrency
- **Session architecture**: Ready for complete session isolation implementation
- **Cross-platform**: Windows, Linux, and Mac threading support
- **Memory efficient**: Deterministic cleanup prevents memory leaks in concurrent scenarios
