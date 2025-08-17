# Grapa Threading Architecture

## **Grapa's Parallel-by-Design Architecture**

Grapa was designed from the ground up to be **parallel by design**. This fundamental architectural decision manifests in several key ways:

### **Universal Locking System**
Many Grapa C++ objects inherit from `GrapaCritical`, providing built-in thread safety:
- `GrapaInt` - Arbitrary precision integers with locking
- `GrapaQueue` - Queue data structures with locking  
- `GrapaNet` - Network operations with locking
- `GrapaThread` - Thread management with locking
- `GrapaConsoleResponse` - Console I/O with locking
- `GrapaWidgetLock` - UI widget locking

### **Automatic Variable Locking**
**CRITICAL IMPLEMENTATION DETAIL**: Every access to a Grapa variable automatically locks/unlocks the underlying object. This was extensively stress-tested with hundreds of threads to ensure:
- No crashes under extreme concurrent access
- Cross-platform thread safety
- Worker object reliability
- Predictable behavior under load

### **Built-in Parallel Processing**
**`.map()`, `.filter()`, and `.reduce()` are parallel by default** and will create one thread per item. For large arrays, always specify a thread count to avoid resource exhaustion:

```grapa
/* Parallel by default - creates one thread per item */
small_data = [1, 2, 3, 4, 5];
squares = small_data.map(op(x) { x * x; });

/* For large datasets, limit threads to avoid resource exhaustion */
large_data = (1000000).range(0,1);
squares = large_data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### **Thread Safety Methods**
All `GrapaCritical` objects provide:
- `TryCritical()` - Try to acquire lock
- `WaitCritical()` - Wait for lock
- `LeaveCritical()` - Release lock
- `WaitCondition()` - Wait for condition
- `SendCondition()` - Signal condition

### **Worker Pattern Integration**
The `GrapaWorker`/`GrapaWorkEvent`/`GrapaWorkQueue` pattern is deeply integrated throughout the system, enabling parallel processing across various subsystems (grep, unified storage, etc.).

### **Documentation References**
- **Threading Documentation**: `docs-src/docs/sys/thread.md` - Comprehensive guide to `$thread` objects and locking
- **Parallel Processing**: `docs-src/docs/use_cases/parallel_concurrent_programming.md` - Examples and best practices
- **Functional Methods**: `docs-src/docs/syntax/basic_syntax.md` - `.map()`, `.filter()`, `.reduce()` with threading

### **Implications**
This design means that Grapa objects are inherently thread-safe at the C++ level, even if the language syntax doesn't currently expose locking primitives. Future language enhancements could leverage this built-in capability.

**The automatic locking system makes Grapa one of the most thread-safe languages ever designed, with built-in protection against race conditions at the variable access level.**

---

## Core C++ Objects

### 1. **Threading & Parallelism**

#### **GrapaThread** (Low-Level)
- **File**: `source/grapa/GrapaThread.h/cpp`
- **Purpose**: Cross-platform thread management
- **Platforms**: Windows (CreateThread), Linux/Mac (pthread)
- **Features**: 
  - Thread creation, suspension, resumption
  - Critical section management
  - Condition variables
  - Cross-platform synchronization primitives

#### **GrapaWorker** (High-Level - Preferred)
- **File**: `source/grapa/GrapaState.h` (GrapaWorker class)
- **Purpose**: Higher-level thread abstraction with Grapa integration
- **Usage**: Used in grep parallelization (`source/grep/`)
- **Features**:
  - Integrated with Grapa script execution
  - Automatic resource management
  - Easier to use than raw GrapaThread
  - Built-in Grapa context and variable management

**Recommendation**: Use `GrapaWorker` for parallelization, following the pattern established in `source/grep/`.

### 2. **File System Operations**

#### **GrapaFile** (Abstract Base)
- **File**: `source/grapa/GrapaFile.h`
- **Purpose**: Abstract file interface
- **Methods**: Open, Close, Read, Write, GetSize, SetSize, Create, Delete, Flush

#### **GrapaFileIO** (Cross-Platform Implementation)
- **File**: `source/grapa/GrapaFileIO.h/cpp`
- **Purpose**: Cross-platform file I/O operations
- **Platforms**: Windows (Win32 API), Linux/Mac (POSIX)
- **Features**:
  - Large file support (64-bit offsets)
  - Cross-platform file locking
  - Consistent error handling
  - Memory-mapped file operations

### 3. **Network Operations**

#### **GrapaNet** (Cross-Platform Network)
- **File**: `source/grapa/GrapaNet.h/cpp`
- **Purpose**: Cross-platform network operations
- **Platforms**: Windows (Winsock), Linux/Mac (BSD sockets)
- **Features**:
  - TCP/UDP socket support
  - Cross-platform address handling
  - Thread-safe network operations
  - Built-in error handling

### 4. **Data Structures**

#### **GrapaQueue** (Thread-Safe Queue)
- **File**: `source/grapa/GrapaQueue.h/cpp`
- **Purpose**: Thread-safe queue data structure
- **Features**:
  - Automatic locking on access
  - Cross-platform implementation
  - Memory-efficient design
  - Built-in error handling

#### **GrapaInt** (Arbitrary Precision)
- **File**: `source/grapa/GrapaInt.h/cpp`
- **Purpose**: Arbitrary precision integer arithmetic
- **Features**:
  - Thread-safe operations
  - Memory-efficient storage
  - Cross-platform compatibility
  - Built-in mathematical operations

## Implementation Patterns

### **Thread-Safe Object Pattern**
```cpp
class GrapaCritical {
protected:
    GrapaCriticalSection mCritical;
    
public:
    void TryCritical() { mCritical.TryCritical(); }
    void WaitCritical() { mCritical.WaitCritical(); }
    void LeaveCritical() { mCritical.LeaveCritical(); }
    void WaitCondition() { mCritical.WaitCondition(); }
    void SendCondition() { mCritical.SendCondition(); }
};

class GrapaInt : public GrapaCritical {
    // Thread-safe integer operations
    // Automatic locking on all access
};
```

### **Worker Pattern**
```cpp
class GrapaWorker {
private:
    GrapaThread* mThread;
    GrapaWorkQueue* mQueue;
    
public:
    void Start() { /* Start worker thread */ }
    void Stop() { /* Stop worker thread */ }
    void AddWork(GrapaWorkEvent* work) { /* Add work to queue */ }
};
```

### **Parallel Processing Pattern**
```cpp
// In GrapaArray::map()
for (int i = 0; i < count; i++) {
    if (threadCount > 1) {
        // Create worker threads for parallel processing
        GrapaWorker* worker = new GrapaWorker();
        worker->AddWork(new GrapaMapWorkEvent(item, function));
    } else {
        // Sequential processing
        result[i] = function(item);
    }
}
```

## Performance Considerations

### **Thread Pool Management**
- **Default Behavior**: One thread per item for small datasets
- **Large Datasets**: Limit thread count to avoid resource exhaustion
- **Memory Usage**: Each thread requires stack space and context
- **CPU Utilization**: Balance between parallelism and overhead

### **Locking Overhead**
- **Automatic Locking**: Every variable access includes lock/unlock
- **Performance Impact**: Minimal due to optimized critical sections
- **Scalability**: Designed for high-concurrency scenarios
- **Deadlock Prevention**: Built-in deadlock detection and prevention

### **Cross-Platform Compatibility**
- **Windows**: Uses Windows API for threading and synchronization
- **Linux/Mac**: Uses POSIX threads and synchronization primitives
- **Abstraction Layer**: Common interface across all platforms
- **Error Handling**: Consistent error reporting across platforms

## Best Practices

### **Thread Count Management**
```grapa
/* For small datasets - let Grapa handle threading */
data = [1, 2, 3, 4, 5];
result = data.map(op(x) { x * x; });

/* For large datasets - specify thread count */
data = (1000000).range(0,1);
result = data.map(op(x) { x * x; }, 8);  /* Limit to 8 threads */
```

### **Error Handling**
```grapa
/* Handle errors in parallel operations */
result = data.map(op(x) { 
    x.operation().iferr(0);  /* Return 0 on error */
}, 4);
```

### **Resource Management**
```grapa
/* Clean up resources after parallel operations */
data = large_dataset;
result = data.map(op(x) { process(x); }, 4);
/* Grapa automatically cleans up worker threads */
```

---

*This architecture provides Grapa with exceptional thread safety and parallel processing capabilities, making it well-suited for high-performance, concurrent applications.* 